#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <thread>
#include <set>

using namespace boost::asio;

const int MAX_LENGTH = 1024;
using sock_ptr = std::shared_ptr<ip::tcp::socket>;
using thread_set = std::set<std::shared_ptr<std::thread>>;
thread_set session_thread_set;

void session(sock_ptr sock) {
  try {
    for (;;) {
      char data[MAX_LENGTH];
      memset(data, '\0', MAX_LENGTH);
      boost::system::error_code err;
      // size_t length = boost::asio::read(*sock, buffer(data, MAX_LENGTH), err);
      size_t length = sock->read_some(buffer(data, MAX_LENGTH), err);
      if (err == boost::asio::error::eof) {
        std::cout << "client:" << sock->remote_endpoint().address().to_string()
                  << " close..." << std::endl;
        break;
      } else if(err) {
        throw boost::system::system_error(err);
      }

      std::cout << "recv from client:"
                << sock->remote_endpoint().address().to_string() << ":" << data
                << std::endl;
      // 回发
      // std::cout << "start send..." << std::endl;
      boost::asio::write(*sock, buffer(data, length));
      // std::cout << "send finish" << std::endl;
    }
  } catch (std::exception& e) {
    std::cerr << "Exception:" << e.what() << std::endl;
  }
}

void server(boost::asio::io_context& ioctx, unsigned short port) {
  boost::asio::ip::tcp::acceptor acceptor(
      ioctx, ip::tcp::endpoint(ip::address_v4::any(), port));
  for (;;) {
    sock_ptr sock = std::make_shared<ip::tcp::socket>(ioctx);
    acceptor.accept(*sock);
    auto t = std::make_shared<std::thread>(session, sock);
    session_thread_set.insert(t);
  }
}

int main() {
  try {
    boost::asio::io_context ioctx;
    server(ioctx, 8001);
    for (auto &t : session_thread_set) {
      if (t->joinable()) {
        t->join();
      }
    }
  } catch (std::exception& e) {
    std::cout << "exception : " << e.what() << std::endl;
    return 0;
  }
  return 0;
}