#include "endpoints.h"
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/is_placeholder.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <streambuf>
#include <string>
#include <vector>

int client_end_point() {
  std::string raw_client_ip = "127.0.0.1";
  unsigned short client_port = 3333;
  boost::system::error_code ec;
  boost::asio::ip::address asio_ip_address =
      boost::asio::ip::address::from_string(raw_client_ip, ec);
  if (ec.value() != 0) {
    std::cout << "pase ip string error and errno=" << ec.value()
              << " err msg=" << ec.message() << std::endl;
    return ec.value();
  }

  boost::asio::ip::tcp::endpoint ep(asio_ip_address, client_port);
  return 0;
}

int server_end_point() {
  unsigned short server_port = 3333;
  boost::asio::ip::address server_ip_address =
      boost::asio::ip::address_v4::any();
  boost::asio::ip::tcp::endpoint ep(server_ip_address, server_port);
  return 0;
}

int create_tcp_socket() {
  boost::asio::io_context ioctx;
  boost::asio::ip::tcp protocol = boost::asio::ip::tcp::v4();
  boost::asio::ip::tcp::socket sock(ioctx);
  return 0;
}

int create_acceptor_socket() {
  boost::asio::io_context ioctx;
  boost::asio::ip::tcp::acceptor ac(
      ioctx, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 3333));
  return 0;
}

int connect_to_end() {
  std::string raw_connect_ip = "127.0.0.1";
  unsigned short port = 3333;
  try {
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address::from_string(raw_connect_ip), port);
    boost::asio::io_context ioctx;
    boost::asio::ip::tcp::socket sock(ioctx, ep.protocol());
    sock.connect(ep);
  } catch (boost::system::system_error &e) {
    std::cout << "connect error:" << e.code() << ".Msg:" << e.what()
              << std::endl;
    return e.code().value();
  }
  return 0;
}

int dns_connect_to_end() {
  std::string host = "llfc.club";
  std::string port = "3333";
  boost::asio::io_context ioctx;
  boost::asio::ip::tcp::resolver::query res_query(
      host, port, boost::asio::ip::tcp::resolver::query::numeric_service);
  boost::asio::ip::tcp::resolver resolver(ioctx);
  try {
    auto it = resolver.resolve(res_query);
    boost::asio::ip::tcp::socket sock(ioctx);
    boost::asio::connect(sock, it);
  } catch (boost::system::system_error &e) {
    std::cout << "connect error:" << e.code() << ".Msg:" << e.what()
              << std::endl;
    return e.code().value();
  }
  return 0;
}

int accept_new_connection() {
  const int BACKLOG_SIZE = 30;
  unsigned short port = 3333;
  boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::any(), port);
  boost::asio::io_context ioctx;
  try {
    // 创建一个acceptor
    boost::asio::ip::tcp::acceptor acceptor(ioctx, ep.protocol());
    // 绑定
    acceptor.bind(ep);
    // 监听
    acceptor.listen();
    // 单个连接
    boost::asio::ip::tcp::socket sock(ioctx);
    // 连接
    acceptor.accept(sock);
  } catch (boost::system::system_error &e) {
    std::cout << "connect error:" << e.code() << ".Msg:" << e.what()
              << std::endl;
    return e.code().value();
  }
  return 0;
}

void use_const_buffer() {
  std::string buf = "hello word, hello boost";
  boost::asio::const_buffer asio_buf(buf.c_str(), buf.size());
  std::vector<boost::asio::const_buffer> buffer_sequence;
  buffer_sequence.emplace_back(asio_buf);
}

void use_buffer_str() {
  // 直接使用原始字符串构造
  boost::asio::const_buffers_1 output_buffer =
      boost::asio::buffer("hello word, hello boost");
  // 使用char* 数组构造
  const size_t BUF_SIZE_BYTES = 20;
  std::unique_ptr<char[]> output(new char[BUF_SIZE_BYTES]);
  auto output_buf =
      boost::asio::buffer(static_cast<void *>(output.get()), BUF_SIZE_BYTES);
}

void wirte_to_socket(boost::asio::ip::tcp::socket &sock) {
  std::string buf = "Hello World!";
  std::size_t total_bytes_written = 0;
  // 循环发送
  // write_some返回每次写入的字节数
  // total_bytes_written是已经发送的字节数。
  // 每次发送buf.length()- total_bytes_written)字节数据

  while (total_bytes_written != buf.length()) {
    total_bytes_written += sock.write_some(boost::asio::buffer(
        buf.c_str() + total_bytes_written, buf.length() - total_bytes_written));
  }
}

int send_data_by_write_some() {
  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address::from_string(raw_ip_address), port_num);
    boost::asio::io_service ios;
    // Step 1. Allocating and opening the socket.
    boost::asio::ip::tcp::socket sock(ios, ep.protocol());
    sock.connect(ep);
    wirte_to_socket(sock);
  } catch (boost::system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
              << ". Message: " << e.what();
    return e.code().value();
  }
  return 0;
}

int send_data_by_send() {
  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address::from_string(raw_ip_address), port_num);
    boost::asio::io_service ios;
    // Step 1. Allocating and opening the socket.
    boost::asio::ip::tcp::socket sock(ios, ep.protocol());
    sock.connect(ep);
    std::string buf = "Hello World!";
    int send_length = sock.send(boost::asio::buffer(buf.c_str(), buf.length()));
    if (send_length <= 0) {
      std::cout << "send failed" << std::endl;
      return 0;
    }
  } catch (boost::system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
              << ". Message: " << e.what();
    return e.code().value();
  }
  return 0;
}

int send_data_by_wirte() {
  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address::from_string(raw_ip_address), port_num);
    boost::asio::io_service ios;
    // Step 1. Allocating and opening the socket.
    boost::asio::ip::tcp::socket sock(ios, ep.protocol());
    sock.connect(ep);
    std::string buf = "Hello World!";
    int send_length = boost::asio::write(
        sock, boost::asio::buffer(buf.c_str(), buf.length()));
    if (send_length <= 0) {
      std::cout << "send failed" << std::endl;
      return 0;
    }
  } catch (boost::system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
              << ". Message: " << e.what();

    return e.code().value();
  }
  return 0;
}

std::string read_from_socket(boost::asio::ip::tcp::socket &sock) {
  const unsigned char MESSAGE_SIZE = 7;
  char buf[MESSAGE_SIZE];
  std::size_t total_bytes_read = 0;

  while (total_bytes_read != MESSAGE_SIZE) {
    total_bytes_read += sock.read_some(boost::asio::buffer(
        buf + total_bytes_read, MESSAGE_SIZE - total_bytes_read));
  }

  return std::string(buf, total_bytes_read);
}

int read_data_by_read_some() {
  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;
  try {
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address::from_string(raw_ip_address), port_num);
    boost::asio::io_service ios;
    boost::asio::ip::tcp::socket sock(ios, ep.protocol());
    sock.connect(ep);
    read_from_socket(sock);
  } catch (boost::system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
              << ". Message: " << e.what();
    return e.code().value();
  }

  return 0;
}

int read_data_by_receive() {
  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address::from_string(raw_ip_address), port_num);
    boost::asio::io_service ios;
    boost::asio::ip::tcp::socket sock(ios, ep.protocol());
    sock.connect(ep);
    const unsigned char BUFF_SIZE = 7;
    char buffer_receive[BUFF_SIZE];
    int receive_length =
        sock.receive(boost::asio::buffer(buffer_receive, BUFF_SIZE));
    if (receive_length <= 0) {
      std::cout << "receive failed" << std::endl;
    }
  } catch (boost::system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
              << ". Message: " << e.what();

    return e.code().value();
  }

  return 0;
}

int read_data_by_read() {
  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address::from_string(raw_ip_address), port_num);
    boost::asio::io_service ios;
    boost::asio::ip::tcp::socket sock(ios, ep.protocol());
    sock.connect(ep);
    const unsigned char BUFF_SIZE = 7;
    char buffer_receive[BUFF_SIZE];
    int receive_length =
        boost::asio::read(sock, boost::asio::buffer(buffer_receive, BUFF_SIZE));
    if (receive_length <= 0) {
      std::cout << "receive failed" << std::endl;
    }
  } catch (boost::system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
              << ". Message: " << e.what();

    return e.code().value();
  }

  return 0;
}

std::string read_data_by_until(boost::asio::ip::tcp::socket &sock) {
  boost::asio::streambuf buf;
  // Synchronously read data from the socket until
  // '\n' symbol is encountered.
  boost::asio::read_until(sock, buf, '\n');
  std::string message;
  // Because buffer 'buf' may contain some other data
  // after '\n' symbol, we have to parse the buffer and
  // extract only symbols before the delimiter.
  std::istream input_stream(&buf);
  std::getline(input_stream, message);
  return message;
}

Session::Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
    : _socket(socket), _send_pending(false), _recv_pending(false) {

}

void Session::Connect(const boost::asio::ip::tcp::endpoint &ep) {
  _socket->connect(ep);
}

void Session::WriteCallBackErr(const boost::system::error_code &ec,
                               size_t bytes_transferred,
                               std::shared_ptr<MsgNode> node) {
  if (bytes_transferred + node->_cur_len < node->_total_len) {
    node->_cur_len += bytes_transferred;
    _socket->async_write_some(
        boost::asio::buffer(node->_msg + node->_cur_len,
                            node->_total_len - node->_cur_len),
        std::bind(&Session::WriteCallBackErr, this, std::placeholders::_1,
                  std::placeholders::_2, node));
  }
}
void Session::WriteToSocketErr(const std::string buf) {
  _send_node = std::make_shared<MsgNode>(buf.c_str(), buf.length());
  this->_socket->async_write_some(
      boost::asio::buffer(_send_node->_msg, _send_node->_total_len),
      std::bind(&Session::WriteCallBackErr, this, std::placeholders::_1,
                std::placeholders::_2,
                _send_node));
}

void Session::WriteCallBack(const boost::system::error_code &ec,
                            size_t bytes_transferred) {
  if (ec.value() != 0) {
    std::cout << "error occured and errcode=" << ec.value()
              << " Msg=" << ec.message() << std::endl;
    return;
  }
  auto &send_data = _send_queue.front();
  send_data->_cur_len += bytes_transferred;
  if (send_data->_cur_len < send_data->_total_len) {
    this->_socket->async_write_some(
        boost::asio::buffer(send_data->_msg + send_data->_cur_len,
                            send_data->_total_len - send_data->_cur_len),
        std::bind(&Session::WriteCallBack, this, std::placeholders::_1,
                  std::placeholders::_2));
    return;
  }
  _send_queue.pop();
  if (_send_queue.empty()) {
    _send_pending = false;
  }
  if (!_send_queue.empty()) {
    auto &data = _send_queue.front();
    this->_socket->async_write_some(
        boost::asio::buffer(data->_msg, data->_total_len),
        std::bind(&Session::WriteCallBack, this, std::placeholders::_1,
                  std::placeholders::_2));
    
  }
}
void Session::WriteToSocket(const std::string buf) {
  _send_queue.emplace(std::make_shared<MsgNode>(buf.c_str(), buf.length()));
  if (_send_pending) {
    return;
  }
  this->_socket->async_write_some(boost::asio::buffer(buf),
                                  std::bind(&Session::WriteCallBack, this,
                                            std::placeholders::_1,
                                            std::placeholders::_2));
  _send_pending = true;
}

void Session::WriteAllToSocket(const std::string buf) {
  _send_queue.emplace(new MsgNode(buf.c_str(), buf.length()));
  if (_send_pending) {
    return;
  }
  this->_socket->async_send(boost::asio::buffer(buf),
                            std::bind(&Session::WriteAllCallBack, this,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
  _send_pending = true;
}
void Session::WriteAllCallBack(const boost::system::error_code &ec,
                               size_t bytes_transferred) {
  if (ec.value() != 0) {
    std::cout << "error occured and errorno=" << ec.value()
              << " Msg=" << ec.what() << std::endl;
    return;
  }
  _send_queue.pop();
  if (_send_queue.empty()) {
    _send_pending = false;
    return;
  } else {
    auto &data = _send_queue.front();
    this->_socket->async_send(
        boost::asio::buffer(data->_msg, data->_total_len),
                              std::bind(&Session::WriteAllCallBack, this,
                                        std::placeholders::_1,
                                        std::placeholders::_2));
  }
}

void Session::ReadFromSocket() {
  if (_recv_pending) {
    return;
  }
  _recv_node = std::make_shared<MsgNode>(RECVSIZE);
  _socket->async_read_some(
      boost::asio::buffer(_recv_node->_msg, _recv_node->_total_len),
      std::bind(&Session::ReadCallBack, this, std::placeholders::_1,
                std::placeholders::_2));
  _recv_pending = true;
}
void Session::ReadCallBack(const boost::system::error_code &ec,
                           size_t bytes_transferred) {
  _recv_node->_cur_len += bytes_transferred;
  if (_recv_node->_cur_len < _recv_node->_total_len) {
    _socket->async_read_some(
        boost::asio::buffer(_recv_node->_msg + _recv_node->_cur_len,
                            _recv_node->_total_len - _recv_node->_cur_len),
        std::bind(&Session::ReadCallBack, this, std::placeholders::_1,
                  std::placeholders::_2));
    return;
  }
  _recv_pending = false;
  _recv_node = nullptr;
}

void Session::ReadAllFromSocket() {
  if (_recv_pending) {
    return;
  }
  _recv_node = std::make_shared<MsgNode>(RECVSIZE);
  _socket->async_receive(
      boost::asio::buffer(_recv_node->_msg, _recv_node->_total_len),
      std::bind(&Session::ReadAllCallBack, this, std::placeholders::_1,
                std::placeholders::_2));
  _recv_pending = true;
}
void Session::ReadAllCallBack(const boost::system::error_code &ec,
                              size_t bytes_transferred) {
  _recv_node->_cur_len += bytes_transferred;
  _recv_node = nullptr;
  _recv_pending = false;
}
// int async_write_data() {
//   std::string raw_ip_address = "127.0.0.1";
//   unsigned short port_num = 3333;
//   try {
//     boost::asio::ip::tcp::endpoint ep(
//         boost::asio::ip::address::from_string(raw_ip_address), port_num);
//     boost::asio::io_context iox;
//     auto socket_ptr =
//         std::make_shared<boost::asio::ip::tcp::socket>(iox,
//         ep.protocol());
//     auto session_ptr = std::make_shared<Session>(socket_ptr);
//     session_ptr->Connect(ep);
//     session_ptr->WriteToSocket("Hello world");
//     iox.run();
//   } catch (boost::system::system_error &e) {
//     std::cout << "Error occured! Error code = " << e.code()
//               << " . Message: " << e.what();
//     return e.code().value();
//   }
//   return 0;
// }