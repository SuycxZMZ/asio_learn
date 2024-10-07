#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
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

using namespace boost::asio;
const int MAX_LENGTH = 1024;
int main() {
  try {
    boost::asio::io_context ioctx;
    boost::asio::ip::tcp::endpoint server_ep(
        boost::asio::ip::address::from_string("127.0.0.1"), 8001);
    boost::asio::ip::tcp::socket sock(ioctx, server_ep.protocol());
    boost::system::error_code err = boost::asio::error::host_not_found;
    // sock.connect(server_ep, err);
    if (sock.connect(server_ep, err)) {
      std::cout << "connect server error and err code = " << err.value()
                << " Msg:"
                << err.message()
                << std::endl;
      return 0;
    }

    std::cout << "connect success ..." << std::endl;
    std::cout << "please input something to send:";
    char request[MAX_LENGTH];
    memset((void*)request, 0, sizeof(request));
    std::cin.getline(request, MAX_LENGTH);
    size_t request_length = strlen(request);
    boost::asio::write(sock, boost::asio::buffer(request, request_length));

    char reply[MAX_LENGTH];
    memset((void*)reply, 0, sizeof(reply));
    size_t reply_length =
        boost::asio::read(sock, boost::asio::buffer(reply, request_length));
    std::cout << "server reply:" << reply << std::endl;
  } catch (std::exception& e) {
    std::cerr << "Exception:" << e.what() << std::endl;
  }
  
  return 0;
}