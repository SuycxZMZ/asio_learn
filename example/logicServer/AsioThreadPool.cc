#include "AsioThreadPool.h"
#include <boost/asio/io_context.hpp>

AsioThreadPool::~AsioThreadPool() {}

boost::asio::io_context &AsioThreadPool::GetIOService() { return _service; }

void AsioThreadPool::Stop() {
  _service.stop();
  _work.reset();
  for (auto &t : _threads) {
    t.join();
  }
}

AsioThreadPool::AsioThreadPool(int threadNum)
    : _work(new boost::asio::io_context::work(_service)) {
  for (int i = 0; i < threadNum; ++i) {
    _threads.emplace_back([this]() {
      _service.run();
    });
  }
}
