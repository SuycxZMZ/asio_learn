set(LIB_SRC
  MsgNode.cc
  CSession.cc
  CServer.cc
  LogicSystem.cc
  AsioThreadPool.cc
  AsioServicePool.cc
)

add_library(boostServer SHARED ${LIB_SRC})
set(LIBS
  boostServer
  ${COMMON_LIBS}
)

add_executable(logicServer logicServer.cc)
target_link_libraries(logicServer ${LIBS})

add_executable(jsonClient jsonClient.cc)
target_link_libraries(jsonClient ${LIBS})