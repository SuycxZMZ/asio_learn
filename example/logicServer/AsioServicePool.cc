#include "AsioServicePool.h"
#include <cstddef>
#include <memory>

AsioServicePool::AsioServicePool(size_t size)
    : _ioServices(size), _workers(size), _nextIOService(0) {
  for (size_t i = 0; i < size; ++i) {
    // 这里走的是移动赋值
    _workers[i] = std::unique_ptr<Work>(new Work(_ioServices[i]));
  }
  for (size_t i = 0; i < size; ++i) {
    _threads.emplace_back([this, i]() {
      _ioServices[i].run();
    });
  }
}

AsioServicePool::~AsioServicePool() {
  std::cout << "AsioIOServicePool destruct" << std::endl;
}

boost::asio::io_context &AsioServicePool::GetIOService() {
  auto &service = _ioServices[_nextIOService++];
  if (_nextIOService == _ioServices.size()) {
    _nextIOService = 0;
  }
  return service;
}
void AsioServicePool::Stop() {
  for (auto &work : _workers) {
    work->get_io_context().stop();
    work.reset();
  }
  for (auto &t : _threads) {
    t.join();
  }
}
