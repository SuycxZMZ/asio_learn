#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <condition_variable>
#include <csignal>
#include <exception>
#include <iostream>
#include <mutex>
#include "AsioServicePool.h"
#include "CServer.h"

bool bstop = false;
std::condition_variable cond_quit;
std::mutex mutex_quit;

int main() {

  try {
    auto &pool = AsioServicePool::GetInstance();
    boost::asio::io_context io_context;
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&io_context, &pool](auto, auto) {
      io_context.stop();
      pool.Stop();
    });
    CServer s(io_context, 8001);
    io_context.run();
  } catch (std::exception& e) {
    std::cerr << "Exception : " << e.what() << std::endl;
  }

  return 0;
}
