#include "LogicSystem.h"
#include "CSession.h"
#include "const.h"
#include <functional>
#include <iostream>
#include <json/reader.h>
#include <json/value.h>
#include <mutex>
#include <thread>

LogicSystem::LogicSystem() : _b_stop(false) {
  RegisterCallBack();
  _work_thread = std::thread(&LogicSystem::DealMsg, this);
}

LogicSystem::~LogicSystem() {
  _b_stop = true;
  _consume.notify_one();
  _work_thread.join();
}
void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> msg) {
  std::unique_lock<std::mutex> lock(_mutex);
  _msg_que.emplace(msg);
  if (_msg_que.size() == 1) {
    lock.unlock();
    _consume.notify_one();
  }
}

LogicSystem &LogicSystem::GetInstance() {
  static LogicSystem instance;
  return instance;
}

void LogicSystem::DealMsg() {
  for (;;) {
    std::unique_lock<std::mutex> lock(_mutex);
    _consume.wait(lock,
                  [this]() -> bool { return !_msg_que.empty() || _b_stop; });

    // 如果是关闭状态，则要把所有逻辑执行完再退出
    if (_b_stop) {
      while (!_msg_que.empty()) {
        auto msg_node = _msg_que.front();
        std::cout << "recv_msg id is : " << msg_node->_recv_node->_msg_id
                  << std::endl;
        auto callback_it = _func_callbacks.find(msg_node->_recv_node->_msg_id);
        if (_func_callbacks.end() == callback_it) {
          _msg_que.pop();
          continue;
        }
        callback_it->second(msg_node->_session, msg_node->_recv_node->_msg_id,
                            std::string(msg_node->_recv_node->_data,
                                        msg_node->_recv_node->_total_len));
        _msg_que.pop();
      }
      break;
    }

    // 正常运行状态
    auto msg_node = _msg_que.front();
    std::cout << "recv_msg id is : " << msg_node->_recv_node->_msg_id
              << std::endl;
    auto callback_it = _func_callbacks.find(msg_node->_recv_node->_msg_id);
    if (_func_callbacks.end() == callback_it) {
      _msg_que.pop();
      continue;
    }
    callback_it->second(msg_node->_session, msg_node->_recv_node->_msg_id,
                        std::string(msg_node->_recv_node->_data,
                                    msg_node->_recv_node->_total_len));
    _msg_que.pop();
  }
}

void LogicSystem::RegisterCallBack() {
  _func_callbacks[MSG_HELLO_WORD] =
      std::bind(&LogicSystem::HelloWorldCallBack, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3);
}

void LogicSystem::HelloWorldCallBack(std::shared_ptr<CSession> session,
                                     const short &msg_id,
                                     const std::string &msg_data) {
  Json::Reader reader;
  Json::Value root;
  reader.parse(msg_data, root);
  std::cout << "recevie msg id  is " << root["id"].asInt() << " msg data is "
            << root["data"].asString() << std::endl;
  root["data"] =
      "server has received msg, msg data is " + root["data"].asString();
  std::string ret_str = root.toStyledString();
  session->Send(ret_str, root["id"].asInt());
}
