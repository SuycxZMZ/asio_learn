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

add_executable(boostCouroutineServer boostCouroutineServer.cc)
target_link_libraries(boostCouroutineServer ${COSERVER_LIBS})

add_executable(testClient testClient.cc)
target_link_libraries(testClient ${COSERVER_LIBS})
