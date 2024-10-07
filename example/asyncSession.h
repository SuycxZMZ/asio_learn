#ifndef ASYNCSESSION_H
#define ASYNCSESSION_H
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstddef>
using namespace boost::asio;
class Session {
public:
  Session(io_context &ioctx) : _socket(ioctx) {}
  ip::tcp::socket &Socket() { return _socket; }

  // 汇话开始
  void Start();

private:
  void handle_read(const boost::system::error_code &ec,
                   size_t bytes_transferred);
  void handle_write(const boost::system::error_code &ec);

private:
  ip::tcp::socket _socket;
  enum { MAX_LENGTH = 1024 };
  char _data[MAX_LENGTH];
};

class Server {
public:
  Server(io_context &ioctx, unsigned short port);
  
private:
  void start_accept();
  void handle_accept(Session *newSession, const boost::system::error_code &ec);

private:
  io_context & _ioctx;
  ip::tcp::acceptor _acceptor;
};

#endif