#pragma once
#include "CSession.h"
#include "Singleton.h"
#include <functional>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <map>
#include <queue>
#include <thread>

using FunCallBack = function<void(shared_ptr<CSession>, const short &msg_id,
                                  const string &msg_data)>;

class LogicSystem : public Singleton<LogicSystem> {
  friend class Singleton<LogicSystem>;

public:
  ~LogicSystem();
  void PostMsgToQue(shared_ptr<LogicNode> msg);

private:
  LogicSystem();
  void DealMsg();
  void RegisterCallBacks();
  void HelloWordCallBack(shared_ptr<CSession>, const short &msg_id,
                         const string &msg_data);
  std::thread _worker_thread;
  std::queue<shared_ptr<LogicNode>> _msg_que;
  std::mutex _mutex;
  std::condition_variable _consume;
  bool _b_stop;
  std::map<short, FunCallBack> _fun_callbacks;
};