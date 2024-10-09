#include "asyncSession.h"
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>

using namespace boost::asio::ip;

int main() {
  try {
    // 创建上下文服务
    boost::asio::io_context ioc;
    // 构造endpoint
    tcp::endpoint remote_ep(address::from_string("127.0.0.1"), 8001);
    tcp::socket sock(ioc);
    boost::system::error_code error = boost::asio::error::host_not_found;
    // sock.connect(remote_ep, error);
    if (sock.connect(remote_ep, error)) {
      std::cout << "connect failed, code is " << error.value() << " error msg is "
           << error.message();
      return 0;
    }
    std::thread send_thread([&sock] {
      for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        const char *request = "hello world!";
        short request_length = strlen(request);
        char send_data[MAX_LENGTH] = {0};
        // 转为网络字节序
        short request_host_length =
            boost::asio::detail::socket_ops::host_to_network_short(
                request_length);
        memcpy(send_data, &request_host_length, 2);
        memcpy(send_data + 2, request, request_length);
        boost::asio::write(sock,
                           boost::asio::buffer(send_data, request_length + 2));
      }
    });
    std::thread recv_thread([&sock] {
      for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        std::cout << "begin to receive..." << std::endl;
        char reply_head[HEADER_LENGTH];
        size_t reply_length = boost::asio::read(
            sock, boost::asio::buffer(reply_head, HEADER_LENGTH));
        short msglen = 0;
        memcpy(&msglen, reply_head, HEADER_LENGTH);
        // 转为本地字节序
        msglen = boost::asio::detail::socket_ops::network_to_host_short(msglen);
        char msg[MAX_LENGTH] = {0};
        size_t msg_length =
            boost::asio::read(sock, boost::asio::buffer(msg, msglen));
        std::cout << "Reply is: ";
        std::cout.write(msg, msglen) << std::endl;
        std::cout << "Reply len is " << msglen;
        std::cout << "\n";
      }
    });
    send_thread.join();
    recv_thread.join();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
  return 0;
}