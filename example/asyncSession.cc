#include "asyncSession.h"
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

using namespace boost::asio;

void Session::Start() {
  memset(_data, 0, MAX_LENGTH);
  _socket.async_read_some(buffer(_data, MAX_LENGTH),
                          std::bind(&Session::handle_read, this,
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    shared_from_this()));
}

void Session::handle_read(const boost::system::error_code &ec,
                          size_t bytes_transferred,
                          std::shared_ptr<Session> _self_shared) {
  if (!ec) {
    // 观察粘包
    // print_recv_data(_data, bytes_transferred);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int copy_len = 0;
    while (bytes_transferred > 0) {
      if (!_b_head_parse) {
        // 收到的数据还不满足一个头部
        if (bytes_transferred + _recv_head_node->_cur_len < HEADER_LENGTH) {
          memcpy(_recv_head_node->_data + _recv_head_node->_cur_len,
                 _data + copy_len, bytes_transferred);
          _recv_head_node->_cur_len += bytes_transferred;
          memset(_data, 0, MAX_LENGTH);
          _socket.async_read_some(
              buffer(_data, MAX_LENGTH),
              std::bind(&Session::handle_read, this, std::placeholders::_1,
                        std::placeholders::_2, _self_shared));
          return;
        }
        // 收到的数据比头部要长
        // 复制头部剩余的数据
        int head_remain = HEADER_LENGTH - _recv_head_node->_cur_len;
        memcpy(_recv_head_node->_data + _recv_head_node->_cur_len,
               _data + copy_len, head_remain);
        // 更新已经处理的数据
        copy_len += head_remain;
        bytes_transferred -= head_remain;
        // 获取头部数据
        short data_len = 0;
        memcpy(&data_len, _recv_head_node->_data, HEADER_LENGTH);
        data_len =
            boost::asio::detail::socket_ops::network_to_host_short(data_len);
        std::cout << "header length:" << data_len << std::endl;
        // 头部长度非法
        if (data_len > MAX_LENGTH) {
          std::cout << "invalid header length !!!" << std::endl;
          _server->EraseSession(_uuid);
          return;
        }
        // 创建接收节点
        _recv_msg_node = std::make_shared<MsgNode>(data_len);

        // 本次没接收完
        if (bytes_transferred < data_len) {
          memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
                 _data + copy_len, bytes_transferred);
          _recv_msg_node->_data += bytes_transferred;
          memset(_data, 0, MAX_LENGTH);
          _socket.async_read_some(
              buffer(_data, MAX_LENGTH),
              std::bind(&Session::handle_read, this, std::placeholders::_1,
                        std::placeholders::_2, _self_shared));
          // 头部处理完
          _b_head_parse = true;
          return;
        }

        // bytes_transferred >= data_len
        memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
               _data + copy_len, data_len);
        _recv_msg_node->_cur_len += data_len;
        copy_len += data_len;
        bytes_transferred -= data_len;
        _recv_msg_node->_data[_recv_msg_node->_max_len] = '\0';
        std::cout << "recv msg:" << _recv_msg_node->_data << std::endl;

        // 回发测试
        this->Send(_recv_msg_node->_data, _recv_msg_node->_max_len);

        // 继续处理剩余的包
        _b_head_parse = false;
        _recv_head_node->Clear();
        if (bytes_transferred <= 0) {
          memset(_data, 0, MAX_LENGTH);
          _socket.async_read_some(
              buffer(_data, MAX_LENGTH),
              std::bind(&Session::handle_read, this, std::placeholders::_1,
                        std::placeholders::_2, _self_shared));
          return;
        }
        // 粘包了，下一轮操作
        continue;
      }
      // 已经处理完头部，处理上次没处理完的数据
      int msg_remain = _recv_msg_node->_max_len - _recv_msg_node->_cur_len;
      // 接收的数据仍然不满足一个完整包
      if (bytes_transferred < msg_remain) {
        memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
               _data + copy_len, bytes_transferred);
        _recv_msg_node->_cur_len += bytes_transferred;
        memset(_data, 0, MAX_LENGTH);
        _socket.async_read_some(buffer(_data, MAX_LENGTH),
                                std::bind(&Session::handle_read, this,
                                          std::placeholders::_1,
                                          std::placeholders::_2, _self_shared));
        return;
      }
      // bytes_transferred >= msg_remain
      memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data + copy_len,
             msg_remain);
      _recv_msg_node->_cur_len += msg_remain;
      bytes_transferred -= msg_remain;
      copy_len += msg_remain;
      _recv_msg_node->_data[_recv_msg_node->_max_len] = '\0';
      std::cout << "recv msg:" << _recv_msg_node->_data << std::endl;
      // 回发测试
      this->Send(_recv_msg_node->_data, _recv_msg_node->_max_len);
      // 处理剩余数据
      _b_head_parse = false;
      _recv_head_node->Clear();
      if (bytes_transferred <= 0) {
        memset(_data, 0, MAX_LENGTH);
        _socket.async_read_some(buffer(_data, MAX_LENGTH),
                                std::bind(&Session::handle_read, this,
                                          std::placeholders::_1,
                                          std::placeholders::_2, _self_shared));
        return;
      }
      continue;
    }
  } else {
    std::cout << "handle_read error and errorno:" << ec.value() << std::endl;
    _server->EraseSession(_uuid);
  }
}
void Session::handle_write(const boost::system::error_code &ec,
                           std::shared_ptr<Session> _self_shared) {
  if (!ec) {
    std::lock_guard<std::mutex> lock(_send_mtx);
    // 上一个元素发完了，发完回调进来，就要先弹出
    _send_queue.pop();
    if (!_send_queue.empty()) {
      auto &msgnode = _send_queue.front();
      boost::asio::async_write(
          _socket, boost::asio::buffer(msgnode->_data, msgnode->_max_len),
          std::bind(&Session::handle_write, this, std::placeholders::_1, _self_shared));
    }
  } else {
    std::cout << "handle_write error and errorno=" << ec.value() << std::endl;
    _server->EraseSession(_uuid);
  }
}

std::string &Session::GetUuid() { return _uuid; }

void Session::print_recv_data(char *data, int length) {
  std::stringstream ss;
  std::string result = "0x";
  for (int i = 0; i < length; i++) {
    std::string hexstr;
    ss << std::hex << std::setw(2) << std::setfill('0') << int(data[i])
       << std::endl;
    ss >> hexstr;
    result += hexstr;
  }
  std::cout << "receive raw data is : " << result << std::endl;
  ;
}

void Session::Send(char *msg, int max_length) {
  // 为true，队列里有数据，不发送，只加到队列中
  bool pending = false;
  std::lock_guard<std::mutex> lock(_send_mtx);
  int send_que_size = _send_queue.size();
  if (send_que_size > MAX_SENDQUE_SIZE) {
    std::cout << "session:" << _uuid << " send que fulled, and size is"
              << MAX_SENDQUE_SIZE << std::endl;
    return;
  }
  if (!_send_queue.empty()) {
    pending = true;
  }
  _send_queue.push(std::make_shared<MsgNode>(msg, max_length));
  if (pending) {
    return;
  }
  boost::asio::async_write(_socket, boost::asio::buffer(msg, max_length),
                           std::bind(&Session::handle_write, this,
                                     std::placeholders::_1,
                                     shared_from_this()));
}

Server::Server(io_context &ioctx, unsigned short port)
    : _ioctx(ioctx),
      _acceptor(ioctx,
                ip::tcp::endpoint(ip::address_v4::any(), 
                port)) {
  std::cout << "-------------- Server::Server() --------------" << std::endl;
  start_accept();
}

void Server::start_accept() {
  auto new_session = std::make_shared<Session>(_ioctx, this);
  // Session *new_session = new Session(_ioctx);
  _acceptor.async_accept(new_session->Socket(),
                         std::bind(&Server::handle_accept, this, new_session,
                                   std::placeholders::_1));
}
void Server::handle_accept(std::shared_ptr<Session> newSession,
                           const boost::system::error_code &ec) {
  if (ec.value() != 0) {
    std::cout << "handle_accept error occured and errno=" << ec.value()
              << std::endl;
    // delete newSession;
  } else {
    newSession->Start();
    _sessions.emplace(newSession->GetUuid(), newSession);
  }
  start_accept();
}

void Server::EraseSession(std::string &uuid) {
  _sessions.erase(uuid);
}