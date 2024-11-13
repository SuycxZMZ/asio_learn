set(WEBSOCKET_LIB_SRC
    shared_state.cc
    http_session.cc
    listener.cc
    websocket_session.cc
)

add_library(WEBSOCKET_LIB SHARED ${WEBSOCKET_LIB_SRC})
set(WEBSOCKET_LIB
    BoostCoServer
    ${COMMON_LIBS}
)

add_executable(webSocketServer main.cc)
target_link_libraries(webSocketServer ${COSERVER_LIBS})

#add_executable(testClient testClient.cc)
#target_link_libraries(testClient ${COSERVER_LIBS})
