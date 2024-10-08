#ifndef ASYNCSESSION_H
#define ASYNCSESSION_H
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cstddef>
#include <cstring>
#include <map>
#include <memory>
#include <mutex>
#include <queue>

#define HEADER_LENGTH 2
#define MAX_LENGTH 1024 * 2

using namespace boost::asio;

class Server;
class MsgNode;
class Session : public std::enable_shared_from_this<Session> {
public:
  Session(io_context &ioctx, Server *server)
      : _socket(ioctx), _server(server), _b_head_parse(false) {
    boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
    _uuid = boost::uuids::to_string(a_uuid);
    _recv_head_node = std::make_shared<MsgNode>(HEADER_LENGTH);
  }
  ip::tcp::socket &Socket() { return _socket; }
  // 汇话开始
  void Start();
  std::string &GetUuid();
  void Send(char *msg, int max_length);

private:
  void handle_read(const boost::system::error_code &ec,
                   size_t bytes_transferred,
                   std::shared_ptr<Session> _self_shared);
  void handle_write(const boost::system::error_code &ec,
                    std::shared_ptr<Session> _self_shared);
  void print_recv_data(char* data, int length);
private:
  ip::tcp::socket _socket;
  char _data[MAX_LENGTH];
  std::queue<std::shared_ptr<MsgNode>> _send_queue;
  std::mutex _send_mtx;
  Server *_server;
  std::string _uuid;
  // 接收到的数据节点
  std::shared_ptr<MsgNode> _recv_msg_node;
  bool _b_head_parse;
  // 接收到的头部结构
  std::shared_ptr<MsgNode> _recv_head_node;
};

class Server {
public:
  Server(io_context &ioctx, unsigned short port);
  void EraseSession(std::string &uuid);

private:
  void start_accept();
  void handle_accept(std::shared_ptr<Session> newSession,
                     const boost::system::error_code &ec);

private:
  io_context &_ioctx;
  ip::tcp::acceptor _acceptor;
  std::map<std::string, std::shared_ptr<Session>> _sessions;
};

class MsgNode {
  friend class Session;

public:
  // 主要发送时使用
  MsgNode(char *msg, int max_len)
      : _max_len(max_len + HEADER_LENGTH), _cur_len(0) {
    _data = new char[_max_len + 1];
    memcpy(_data, &max_len, HEADER_LENGTH);
    memcpy(_data + HEADER_LENGTH, msg, max_len);
    _data[_max_len] = '\0';
  }

  // 主要接收时使用
  MsgNode(unsigned short max_len) : _max_len(max_len), _cur_len(0) {
    _data = new char[max_len + 1];
    memset(_data, 0, max_len + 1);
  }
  ~MsgNode() { delete[] _data; }

  void Clear() {
    memset(_data, 0, _max_len);
    _cur_len = 0;
  }

private:
  int _cur_len;
  int _max_len;
  char *_data;
};

#endif