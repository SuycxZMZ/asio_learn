#pragma once
#include "CSession.h"
#include <boost/asio.hpp>
#include <map>
#include <memory.h>
#include <mutex>

using boost::asio::ip::tcp;
class CServer {
public:
  CServer(boost::asio::io_context &io_context, short port);
  void ClearSession(std::string& uuid);

private:
  void HandleAccept(std::shared_ptr<CSession>,
                    const boost::system::error_code &error);
  void StartAccept();
  boost::asio::io_context &_io_context;
  short _port;
  tcp::acceptor _acceptor;
  std::map<std::string, std::shared_ptr<CSession>> _sessions;
  std::mutex _mutex;
};