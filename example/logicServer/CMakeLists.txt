set(ALL_SOURCE_LIST 
  MsgNode.cc 
  CSession.cc 
  CServer.cc 
  LogicSystem.cc
  AsioServicePool.cc
  logicServer.cc
)
add_executable(logicServer ${ALL_SOURCE_LIST})
target_link_libraries(logicServer ${COMMON_LIBS})

# set(jsonClient_list jsonClient.cc)
# add_executable(jsonClient ${jsonClient_list})
# target_link_libraries(logicServer ${COMMON_LIBS})