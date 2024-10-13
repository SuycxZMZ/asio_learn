#ifndef BEASTHTTPSERVER_H
#define BEASTHTTPSERVER_H

#include <boost/asio/steady_timer.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <cstddef>
#include <ctime>
#include <memory>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

std::size_t request_count();
std::time_t now();

class http_connection : public std::enable_shared_from_this<http_connection> {
public:
  http_connection(tcp::socket socket);
  void start();

private:
  void read_request();
  void process_request();
  void create_response();
  void create_post_response();
  void write_response();
  void check_deadline();

private:
  tcp::socket _socket;
  beast::flat_buffer _buffer{8192};
  http::request<http::dynamic_body> _request;
  http::response<http::dynamic_body> _response;
  net::steady_timer _dead_time;

};

void http_server(tcp::acceptor& acceptor, tcp::socket& socket);

#endif