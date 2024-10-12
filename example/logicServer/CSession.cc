#include "CSession.h"
#include "MsgNode.h"
#include "const.h"
#include "CServer.h"
#include "LogicSystem.h"
#include <boost/asio/buffer.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/write.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cstring>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>

CSession::CSession(boost::asio::io_context &io_context, CServer *server)
    : _socket(io_context), _server(server), _b_close(false),
      _b_head_parse(false) {
  boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
  _uuid = boost::uuids::to_string(a_uuid);
  _recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

CSession::~CSession() { std::cout << "~CSession destruct" << std::endl; }

tcp::socket &CSession::GetSocket() { return _socket; }

std::string &CSession::GetUuid() { return _uuid; }

void CSession::Start() {
  memset(_data, 0, MAX_LENGTH);
  _socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH),
                          std::bind(&CSession::HandleRead, this,
                                    std::placeholders::_1,
                                    std::placeholders::_2, shared_from_this()));
}
void CSession::Send(char *msg, short max_length, short msgid) {
  std::lock_guard<std::mutex> lock(_send_lock);
  int send_que_size = _send_que.size();
  if (send_que_size > MAX_SENDQUE) {
    std::cout << "session: " << _uuid << " send que fulled, size is "
              << MAX_SENDQUE << std::endl;
    return;
  }

  _send_que.push(std::make_shared<SendNode>(msg, max_length, msgid));
  if (send_que_size > 0) {
    return;
  }
  auto &msgnode = _send_que.front();
  boost::asio::async_write(
      _socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
      std::bind(&CSession::HandleWrite, this, std::placeholders::_1,
                shared_from_this()));
}
void CSession::Send(std::string msg, short msgid) {
  std::lock_guard<std::mutex> lock(_send_lock);
  int send_que_size = _send_que.size();
  if (send_que_size > MAX_SENDQUE) {
    std::cout << "session: " << _uuid << " send que fulled, size is "
              << MAX_SENDQUE << std::endl;
    return;
  }

  _send_que.push(std::make_shared<SendNode>(msg.c_str(), msg.length(), msgid));
  if (send_que_size > 0) {
    return;
  }
  auto &msgnode = _send_que.front();
  boost::asio::async_write(
      _socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
      std::bind(&CSession::HandleWrite, this, std::placeholders::_1,
                shared_from_this()));
}
void CSession::Close() {
  _socket.close();
  _b_close = true;
}

void CSession::HandleRead(const boost::system::error_code &error,
                          size_t bytes_transferred,
                          std::shared_ptr<CSession> shared_self) {
  try {
    if (!error) {
      int copy_len = 0;
      while (bytes_transferred > 0) {
        if (!_b_head_parse) {

          // 收到的数据还不足一个头部的大小
          if (bytes_transferred + _recv_head_node->_cur_len < HEAD_TOTAL_LEN) {
            memcpy(_recv_head_node->_data + _recv_head_node->_cur_len,
                   _data + copy_len, bytes_transferred);
            memset(_data, 0, MAX_LENGTH);
            _socket.async_read_some(
                boost::asio::buffer(_data, MAX_LENGTH),
                std::bind(&CSession::HandleRead, this, std::placeholders::_1,
                          std::placeholders::_2, shared_self));
            return;
          }

          // 数据大于一个头部
          // 先复制头部剩余的部分
          int head_remain = HEAD_TOTAL_LEN - _recv_head_node->_cur_len;
          memcpy(_recv_head_node->_data + _recv_head_node->_cur_len,
                 _data + copy_len, head_remain);
          copy_len += head_remain;
          bytes_transferred -= head_remain;
          // 获取头部的MSGID
          short msg_id = 0;
          memcpy(&msg_id, _recv_head_node->_data, HEAD_ID_LEN);
          msg_id =
              boost::asio::detail::socket_ops::network_to_host_short(msg_id);
          std::cout << "msg id:" << msg_id << std::endl;
          if (msg_id > MAX_LENGTH) {
            std::cout << "invalid msg id:" << msg_id << std::endl;
            _server->ClearSession(_uuid);
            return;
          }

          short msg_len = 0;
          memcpy(&msg_len, _recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);
          msg_len =
              boost::asio::detail::socket_ops::network_to_host_short(msg_len);
          std::cout << "msg len:" << msg_len << std::endl;
          if (msg_len > MAX_LENGTH) {
            std::cout << "invalid data length is " << msg_len << endl;
            _server->ClearSession(_uuid);
            return;
          }

          _recv_msg_node = std::make_shared<RecvNode>(msg_len, msg_id);

          // 消息的长度没到一个包，先暂存到_recv_msg_node
          if (bytes_transferred < msg_len) {
            memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
                   _data + copy_len, bytes_transferred);
            _recv_msg_node->_cur_len += bytes_transferred;
            memset(_data, 0, MAX_LENGTH);
            _socket.async_read_some(
                boost::asio::buffer(_data, MAX_LENGTH),
                std::bind(&CSession::HandleRead, this, std::placeholders::_1,
                          std::placeholders::_2, shared_self));
            // 头部处理完，标记一下
            _b_head_parse = true;
            return;
          }

          // 消息长度大于等于一个完整包
          memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
                 _data + copy_len, msg_len);
          _recv_msg_node->_cur_len += msg_len;
          copy_len += msg_len;
          bytes_transferred -= msg_len;
          _recv_msg_node->_data[_recv_msg_node->_cur_len] = '\0';

          // 将完整消息包放入逻辑队列，给逻辑层执行，即业务执行
          LogicSystem::GetInstance()->PostMsgToQue(
              std::make_shared<LogicNode>(shared_from_this(), _recv_msg_node));

          // 处理剩余的数据
          _b_head_parse = false;
          _recv_head_node->Clear();
          if (bytes_transferred <= 0) {
            memset(_data, 0, MAX_LENGTH);
            _socket.async_read_some(
                boost::asio::buffer(_data, MAX_LENGTH),
                std::bind(&CSession::HandleRead, this, std::placeholders::_1,
                          std::placeholders::_2, shared_self));
            return;
          }
          continue;
        }

        // 头部处理完
        int remain_msg = _recv_msg_node->_total_len - _recv_msg_node->_cur_len;
        // 本次数据太少，不满一个包
        if (bytes_transferred < remain_msg) {
          memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
                 _data + copy_len, bytes_transferred);
          _recv_msg_node->_cur_len += bytes_transferred;
          ::memset(_data, 0, MAX_LENGTH);
          _socket.async_read_some(
              boost::asio::buffer(_data, MAX_LENGTH),
              std::bind(&CSession::HandleRead, this, std::placeholders::_1,
                        std::placeholders::_2, shared_self));
          return;
        }

        // 数据可以搞满一个包
        memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
               _data + copy_len, remain_msg);
        _recv_msg_node->_cur_len += remain_msg;
        bytes_transferred -= remain_msg;
        copy_len += remain_msg;
        _recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
        // cout << "receive data is " << _recv_msg_node->_data << endl;
        // 将消息投递到逻辑队列中
        LogicSystem::GetInstance()->PostMsgToQue(
            std::make_shared<LogicNode>(shared_from_this(), _recv_msg_node));

        // 继续轮询剩余未处理数据
        _b_head_parse = false;

        _recv_head_node->Clear();
        // 接收的数据处理完
        if (bytes_transferred <= 0) {
          ::memset(_data, 0, MAX_LENGTH);
          _socket.async_read_some(
              boost::asio::buffer(_data, MAX_LENGTH),
              std::bind(&CSession::HandleRead, this, std::placeholders::_1,
                        std::placeholders::_2, shared_self));
          return;
        }
        continue;
      }
    } else {
      std::cout << "handle read failed, error is " << error.message() << endl;
      Close();
      _server->ClearSession(_uuid);
    }
  } catch (std::exception& e) {
    std::cout << "Exception code is " << e.what() << std::endl;
  }
}
void CSession::HandleWrite(const boost::system::error_code &error,
                           std::shared_ptr<CSession> shared_self) {
  try {
    if (!error) {
      std::lock_guard<std::mutex> lock(_send_lock);
      _send_que.pop();
      if (!_send_que.empty()) {
        auto &next_node = _send_que.front();
        boost::asio::async_write(
            _socket,
            boost::asio::buffer(next_node->_data, next_node->_total_len),
            std::bind(&CSession::HandleWrite, this, std::placeholders::_1,
                      shared_from_this()));
      }
    } else {
      std::cout << "handle write failed, error is " << error.message() << endl;
      Close();
      _server->ClearSession(_uuid);
    }
  } catch (std::exception& e) {
    std::cerr << "Exception code : " << e.what() << std::endl;
  }
}

LogicNode::LogicNode(shared_ptr<CSession> session,
                     shared_ptr<RecvNode> recvnode)
    : _session(session), _recvnode(recvnode) {}
