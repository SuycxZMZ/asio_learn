#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H

#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <string>

class CSession;
class LogicNode;

class LogicSystem {
public:
  using FuncCallBack = std::function<void(std::shared_ptr<CSession>,
                                          const short &, const std::string &)>;
  ~LogicSystem();
  void PostMsgToQue(std::shared_ptr<LogicNode> msg);
  static LogicSystem& GetInstance();

private:
  LogicSystem();
  void DealMsg();
  void RegisterCallBack();
  void HelloWorldCallBack(std::shared_ptr<CSession>, const short &msg_id,
                          const std::string &msg_data);
  
private:
  
  std::thread _work_thread;
  std::mutex _mutex;
  std::queue<std::shared_ptr<LogicSystem>> _msg_que;
  std::condition_variable _consume;
  bool _b_stop;
  std::map<short, FuncCallBack> _func_callbacks;
};

#endif