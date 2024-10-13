#include "LogicSystem.h"
#include "const.h"
#include "CSession.h"
#include <iostream>
#include <functional>
#include <json/reader.h>
#include <json/value.h>
#include <thread>

LogicSystem::LogicSystem() : _b_stop(false)
{
    RegisterCallBack();
    _work_thread = std::thread(&LogicSystem::DealMsg, this);
}

LogicSystem::~LogicSystem()
{
    
}
void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> msg)
{

}

LogicSystem &LogicSystem::GetInstance() {
  static LogicSystem instance;
  return instance;
}

void LogicSystem::DealMsg()
{

}

void LogicSystem::RegisterCallBack()
{
  _func_callbacks[MSG_HELLO_WORD] =
      std::bind(&LogicSystem::HelloWorldCallBack, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3);
}

void LogicSystem::HelloWorldCallBack(std::shared_ptr<CSession> session, const short &msg_id,
                        const std::string &msg_data)
{
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

