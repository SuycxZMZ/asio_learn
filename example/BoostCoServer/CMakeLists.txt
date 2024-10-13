#include_directories(./)

set(COSERVER_LIB_SRC
  MsgNode.cc
  CServer.cc
  CSession.cc
  AsioServicePool.cc
  LogicSystem.cc
)

add_library(BoostCoServer SHARED ${COSERVER_LIB_SRC})
set(COSERVER_LIBS
  BoostCoServer
  ${COMMON_LIBS}
)