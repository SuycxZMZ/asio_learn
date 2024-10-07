#include "asyncSession.h"
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <cstring>
#include <functional>
#include <iostream>

using namespace boost::asio;

void Session::Start() {
  memset(_data, 0, MAX_LENGTH);
  _socket.async_read_some(buffer(_data, MAX_LENGTH),
                          std::bind(&Session::handle_read, this,
                                    std::placeholders::_1,
                                    std::placeholders::_2));
}

void Session::handle_read(const boost::system::error_code &ec,
                          size_t bytes_transferred) {
  if (ec.value() != 0) {
    std::cout << "handle_read error occured and errno=" << ec.value()
              << std::endl;
    delete this;
    return;
  }
  std::cout << "recv data:" << _data << std::endl;
  async_write(_socket, buffer(_data, bytes_transferred),
              std::bind(&Session::handle_write, this, std::placeholders::_1));
}
void Session::handle_write(const boost::system::error_code &ec) {
  if (ec.value() != 0) {
    std::cout << "handle_write error occured and errno=" << ec.value()
              << std::endl;
    delete this;
    return;
  }
  memset(_data, 0, MAX_LENGTH);
  _socket.async_read_some(buffer(_data, MAX_LENGTH),
                          std::bind(&Session::handle_read, this,
                                    std::placeholders::_1,
                                    std::placeholders::_2));
}

Server::Server(io_context &ioctx, unsigned short port)
    : _ioctx(ioctx),
      _acceptor(ioctx, ip::tcp::endpoint(ip::address_v4::any(), port)) {
  std::cout << "-------------- Server::Server() --------------" << std::endl;
  start_accept();
}

void Server::start_accept() {
  Session *new_session = new Session(_ioctx);
  _acceptor.async_accept(new_session->Socket(),
                         std::bind(&Server::handle_accept, this, new_session,
                                   std::placeholders::_1));
}
void Server::handle_accept(Session *newSession,
                           const boost::system::error_code &ec) {
  if (ec.value() != 0) {
    std::cout << "handle_accept error occured and errno=" << ec.value()
              << std::endl;
    delete newSession;
  } else {
    newSession->Start();
  }
  start_accept();
}