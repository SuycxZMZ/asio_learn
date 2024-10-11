#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <exception>

namespace this_coro = boost::asio::this_coro;
using namespace boost::asio;

#if defined(BOOST_ASIO_ENABLE_HANDLER_TRACKING)
#define use_awaitable                                                          \
  boost::asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif

awaitable<void> echo(ip::tcp::socket socket) {
  try {
    char data[1024];
    for (;;) {
      std::size_t n =
          co_await socket.async_read_some(buffer(data), use_awaitable);
      co_await async_write(socket, buffer(data, n), use_awaitable);
    }
  } catch (std::exception& e) {
    std::printf("Exception: %s\n", e.what());
  }
}

awaitable<void> listener() {
  auto executor = co_await this_coro::executor;
  ip::tcp::acceptor acceptor(executor, {ip::address_v4::any(), 8001});
  for (;;) {
    ip::tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
    co_spawn(executor, echo(std::move(socket)), detached);
  }
}

int main() {
  try {
    io_context ioctx(1);
    signal_set signals(ioctx, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) { ioctx.stop(); });

    co_spawn(ioctx, listener(), detached);
    ioctx.run();
  } catch (std::exception &e) {
    std::printf("Exception: %s\n", e.what());
  }
  return 0;
}