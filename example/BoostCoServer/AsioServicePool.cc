#include "AsioServicePool.h"

AsioServicePool::AsioServicePool(
    std::size_t threadNum) {
  
    }
AsioServicePool::~AsioServicePool() {}
boost::asio::io_context &AsioServicePool::GetIOService() {}
void AsioServicePool::Stop() {}
AsioServicePool &AsioServicePool::GetInstance() {}