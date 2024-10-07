#ifndef ENDPOINTS_H
#define ENDPOINTS_H

// ---------------- 基础API练习 ---------------- //
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <cstddef>
#include <memory>
#include <queue>
extern int client_end_point();
extern int server_end_point();
extern int create_tcp_socket();
extern int create_acceptor_socket();
extern int connect_to_end();
extern int dns_connect_to_end();
extern int accept_new_connection();
extern void use_const_buffer();
extern void use_buffer_str();
extern int send_data_by_write_some();
extern int send_data_by_send();
extern int send_data_by_wirte();
extern int read_data_by_read_some();
extern int read_data_by_receive();
extern int read_data_by_read();
extern std::string read_data_by_until(boost::asio::ip::tcp::socket &sock);

// 异步接口
class MsgNode;
class Session {
public:
  Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
  void Connect(const boost::asio::ip::tcp::endpoint &ep);
  void WriteCallBackErr(const boost::system::error_code &ec,
                        size_t bytes_transferred,
                        std::shared_ptr<MsgNode> node);
  void WriteToSocketErr(const std::string buf);

  void WriteCallBack(const boost::system::error_code &ec,
                     size_t bytes_transferred);
  void WriteToSocket(const std::string buf);

  void WriteAllToSocket(const std::string buf);
  void WriteAllCallBack(const boost::system::error_code &ec,
                        size_t bytes_transferred);

  void ReadFromSocket();
  void ReadCallBack(const boost::system::error_code &ec,
                    size_t bytes_transferred);

  void ReadAllFromSocket();
  void ReadAllCallBack(const boost::system::error_code &ec,
                       size_t bytes_transferred);
private:
  std::queue<std::shared_ptr<MsgNode>> _send_queue;
  std::shared_ptr<boost::asio::ip::tcp::socket> _socket;
  std::shared_ptr<MsgNode> _send_node;
  std::shared_ptr<MsgNode> _recv_node;
  bool _send_pending;
  bool _recv_pending;
};
// extern int async_write_data();

// 最大报文接收大小
const int RECVSIZE = 1024;
class MsgNode {
public:
  MsgNode(const char *msg, int total_len) : _total_len(total_len), _cur_len(0) {
    _msg = new char[total_len];
    memcpy(_msg, msg, total_len);
  }
  MsgNode(int total_len) : _total_len(total_len), _cur_len(0) {
    _msg = new char[total_len];
  }
  ~MsgNode() { delete[] _msg; }
  // 消息首地址
  char *_msg;
  // 总长度
  int _total_len;
  // 当前长度
  int _cur_len;
};

#endif