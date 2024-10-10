#include "CServer.h"
#include "CSession.h"
#include <functional>
#include <memory>

CServer::CServer(boost::asio::io_context &io_context, short port)
    : _io_context(io_context), _port(port),
      _acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
  std::cout << "Server start success, listen on port : " << _port << std::endl;
  StartAccept();
}

void CServer::ClearSession(std::string &uuid) { _sessions.erase(uuid); }

void CServer::HandleAccept(std::shared_ptr<CSession> new_session,
                           const boost::system::error_code &error) {
  if (!error) {
    new_session->Start();
    _sessions.emplace(new_session->GetUuid(), new_session);
  } else {
    std::cout << "session accept failed, error is " << error.what()
              << std::endl;
  }
}
void CServer::StartAccept() {
  auto new_session = std::make_shared<CSession>(_io_context, this);
  _acceptor.async_accept(new_session->GetSocket(),
                         std::bind(&CServer::HandleAccept, this, new_session,
                                   std::placeholders::_1));
}