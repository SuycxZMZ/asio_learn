#include "endpoints.h"
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/is_placeholder.hpp>
#include <boost/system/system_error.hpp>
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/system/detail/error_code.hpp>
#include <string>

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
  } catch (boost::system::system_error& e) {
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
  } catch (boost::system::system_error& e) {
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
}