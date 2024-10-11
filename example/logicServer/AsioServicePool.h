#pragma once
#include <boost/asio/io_context.hpp>
#include <cstddef>
#include <memory>
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include "Singleton.h"

class AsioServicePool : public Singleton<AsioServicePool> {
  friend Singleton<AsioServicePool>;

public:
  using IOService = boost::asio::io_context;
  using Work = boost::asio::io_context::work;
  using WorkPtr = std::unique_ptr<Work>;
  ~AsioServicePool();
  AsioServicePool(const AsioServicePool &) = delete;
  AsioServicePool &operator=(const AsioServicePool &) = delete;
  boost::asio::io_context &GetIOService();
  void Stop();
private:
  // AsioServicePool(size_t size = std::thread::hardware_concurrency());
  AsioServicePool(size_t size = 2);
  std::vector<IOService> _ioServices;
  std::vector<WorkPtr> _workers;
  std::vector<std::thread> _threads;
  size_t _nextIOService;
};