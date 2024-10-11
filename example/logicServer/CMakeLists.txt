# set(ALL_SOURCE_LIST 
#   MsgNode.cc 
#   CSession.cc 
#   CServer.cc 
#   LogicSystem.cc
#   AsioServicePool.cc
#   logicServer.cc
# )

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

# add_executable(logicServer ${ALL_SOURCE_LIST})
# target_link_libraries(logicServer ${COMMON_LIBS})
set(logicServer_SRC logicServer.cc)
add_executable(logicServer ${logicServer_SRC})
target_link_libraries(logicServer ${LIBS})

set(jsonClient_SRC jsonClient.cc)
add_executable(jsonClient ${jsonClient_SRC})
target_link_libraries(jsonClient ${LIBS})
# set(jsonClient_list jsonClient.cc)
# add_executable(jsonClient ${jsonClient_list})
# target_link_libraries(logicServer ${COMMON_LIBS})