#include "LogicSystem.h"
#include "const.h"
#include <json/reader.h>
#include <json/value.h>
#include <mutex>

LogicSystem::LogicSystem() : _b_stop(false) {
  RegisterCallBacks();
  _worker_thread = std::thread(&LogicSystem::DealMsg, this);
}
void LogicSystem::DealMsg() {
  for (;;) {
    std::unique_lock<std::mutex> lock(_mutex);
    while (_msg_que.empty() && !_b_stop) {
      _consume.wait(lock);
    }

    // 关闭状态，把所有逻辑执行完后则退出循环
    if (_b_stop) {
      while (!_msg_que.empty()) {
        auto msg_node = _msg_que.front();
        std::cout << "recv_msg id:" << msg_node->_recvnode->_msg_id
                  << std::endl;
        auto call_back_it = _fun_callbacks.find(msg_node->_recvnode->_msg_id);
        if (_fun_callbacks.end() == call_back_it) {
          _msg_que.pop();
          continue;
        }
        call_back_it->second(msg_node->_session, msg_node->_recvnode->_msg_id,
                             std::string(msg_node->_recvnode->_data,
                                         msg_node->_recvnode->_cur_len));
        _msg_que.pop();
      }
    }

    // 如果没有停服，且说明队列中有数据
    auto msg_node = _msg_que.front();
    std::cout << "recv_msg id:" << msg_node->_recvnode->_msg_id << std::endl;
    auto call_back_it = _fun_callbacks.find(msg_node->_recvnode->_msg_id);
    if (_fun_callbacks.end() == call_back_it) {
      _msg_que.pop();
      continue;
    }
    call_back_it->second(
        msg_node->_session, msg_node->_recvnode->_msg_id,
        std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_cur_len));
    _msg_que.pop();
  }
}
void LogicSystem::RegisterCallBacks() {
  _fun_callbacks[MSG_HELLO_WORD] =
      std::bind(&LogicSystem::HelloWordCallBack, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3);
}
void LogicSystem::HelloWordCallBack(shared_ptr<CSession> session,
                                    const short &msg_id,
                                    const string &msg_data) {
  Json::Reader reader;
  Json::Value root;
  reader.parse(msg_data, root);
  std::cout << "recevie msg id  is " << root["id"].asInt() << " msg data is "
            << root["data"].asString() << std::endl;
  root["data"] =
      "server has received msg, msg data is " + root["data"].asString();
  std::string return_str = root.toStyledString();
  session->Send(return_str, root["id"].asInt());
}
LogicSystem::~LogicSystem() {
  _b_stop = true;
  _consume.notify_one();
  _worker_thread.join();
}
void LogicSystem::PostMsgToQue(shared_ptr<LogicNode> msg) {
  std::unique_lock<std::mutex> uni_lock(_mutex);
  _msg_que.push(msg);
  if (_msg_que.size() == 1) {
    uni_lock.unlock();
    _consume.notify_one();
  }
}