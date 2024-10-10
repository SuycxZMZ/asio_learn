#include "CServer.h"
#include <csignal>
#include <iostream>
#include <mutex>

using namespace std;
bool bstop = false;
std::condition_variable cond_quit;
std::mutex mutex_quit;

int main() {
  try {
    boost::asio::io_context io_context;
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&io_context](auto, auto) { io_context.stop(); });
    CServer s(io_context, 8001);
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << endl;
  }
}