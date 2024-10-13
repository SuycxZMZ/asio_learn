#include "beastHttpServer.h"
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/ostream.hpp>
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/fields.hpp>
#include <boost/beast/http/impl/read.hpp>
#include <boost/beast/http/impl/write.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/core/ignore_unused.hpp>
#include <cstddef>
#include <ctime>
#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <memory>

std::size_t request_count() {
  static std::size_t count = 0;
  return ++count;
}

std::time_t now() { return std::time(0); }

http_connection::http_connection(tcp::socket socket)
    : _socket(std::move(socket)),
      _dead_time(_socket.get_executor(), std::chrono::seconds(60)) {}

void http_connection::start() {
  read_request();
  check_deadline();
}

void http_connection::read_request() {
  auto self_prt = shared_from_this();
  http::async_read(
      _socket, _buffer, _request,
      [self_prt](beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (!ec) {
          self_prt->process_request();
        }
      });
}

void http_connection::process_request() {
  _response.version(_request.version());
  _response.keep_alive(false);
  switch (_request.method()) {
  case http::verb::get:
    _response.result(http::status::ok);
    _response.set(http::field::server, "beast");
    create_response();
    break;
  case http::verb::post:
    _response.result(http::status::ok);
    _response.set(http::field::server, "beast");
    create_post_response();
    break;
  default:
    _response.result(http::status::bad_request);
    _response.set(http::field::content_type, "text/plain");
    beast::ostream(_response.body())
        << "Invalid request-method '" << std::string(_request.method_string())
        << "'";
    break;
  }
  write_response();
}

void http_connection::create_response() {
  if (_request.target() == "/count") {
    _response.set(http::field::content_type, "text/html");
    beast::ostream(_response.body())
        << "<html>\n"
        << "<head><title>Request count</title></head>\n"
        << "<body>\n"
        << "<h1>Request count</h1>\n"
        << "<p>There have been " << request_count()
        << " requests so far.</p>\n"
        << "</body>\n"
        << "</html>\n";
  } else if (_request.target() == "/time") {
    _response.set(http::field::content_type, "text/html");
    beast::ostream(_response.body())
        << "<html>\n"
        << "<head><title>Current time</title></head>\n"
        << "<body>\n"
        << "<h1>Current time</h1>\n"
        << "<p>The current time is " << now()
        << " seconds since the epoch.</p>\n"
        << "</body>\n"
        << "</html>\n";
  } else {
    _response.result(http::status::not_found);
    _response.set(http::field::content_type, "text/plain");
    beast::ostream(_response.body()) << "File not found\r\n";
  }
}

void http_connection::create_post_response() {
  if (_request.target() == "/email") {
    auto &body = this->_request.body();
    auto body_str = boost::beast::buffers_to_string(body.data());
    std::cout << "receive body is " << body_str << std::endl;
    this->_response.set(http::field::content_type, "text/json");
    
    Json::Value root;
    Json::Reader reader;
    Json::Value src_root;

    bool parse_success = reader.parse(body_str, src_root);
    if (!parse_success) {
      std::cout << "Failed to parse JSON data!" << std::endl;
      root["error"] = 1001;
      std::string jsonstr = root.toStyledString();
      beast::ostream(this->_response.body()) << jsonstr;
      return;
    }

    auto email = src_root["email"].asString();
    std::cout << "email is " << email << std::endl;

    root["error"] = 0;
    root["email"] = src_root["email"];
    root["msg"] = "recevie email post success";
    std::string jsonstr = root.toStyledString();
    beast::ostream(this->_response.body()) << jsonstr;
  } else {
    _response.result(http::status::not_found);
    _response.set(http::field::content_type, "text/plain");
    beast::ostream(_response.body()) << "File not found\r\n";
  }
}

void http_connection::write_response() {
  auto self_ptr = shared_from_this();
  _response.content_length(_response.body().size());
  http::async_write(
      _socket, _response, [self_ptr](beast::error_code ec, std::size_t) {
      self_ptr->_socket.shutdown(tcp::socket::shutdown_send, ec);
      self_ptr->_dead_time.cancel();     
  });
}

void http_connection::check_deadline() {
  auto self_ptr = shared_from_this();
  _dead_time.async_wait([self_ptr](beast::error_code ec) {
    if (!ec) {
      self_ptr->_socket.close();
    }
  });
}

void http_server(tcp::acceptor &acceptor, tcp::socket &socket) {
  acceptor.async_accept(socket, [&](beast::error_code ec) {
    if (!ec) {
      std::make_shared<http_connection>(std::move(socket))->start();
      http_server(acceptor, socket);
    }
  });
}

int main() {

  try {
    auto const address = net::ip::make_address("0.0.0.0");
    unsigned short port = static_cast<unsigned short>(8080);

    net::io_context ioc{1};

    tcp::acceptor acceptor{ioc, {address, port}};
    tcp::socket socket{ioc};
    http_server(acceptor, socket);
    ioc.run();
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return 0;
}