#include "CServer.h"
#include "CSession.h"
#include "AsioServicePool.h"
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
    std::lock_guard<std::mutex> lock(_mutex);
    _sessions.insert(make_pair(new_session->GetUuid(), new_session));
  } else {
    cout << "session accept failed, error is " << error.message() << endl;
  }

  StartAccept();
}
void CServer::StartAccept() {
  auto& io_context = AsioServicePool::GetInstance()->GetIOService();
  auto new_session = std::make_shared<CSession>(io_context, this);
  _acceptor.async_accept(new_session->GetSocket(),
                         std::bind(&CServer::HandleAccept, this, new_session,
                                   std::placeholders::_1));
}