#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>
#include <exception>
#include <iostream>
#include "asyncSession.h"
using namespace boost::asio;

int main() {
  try {
    io_context ioctx;
    Server server(ioctx, 8001);
    ioctx.run();
  } catch (std::exception& e) {
    std::cerr << "Exception:" << e.what() << std::endl;
  }
  return 0;
}