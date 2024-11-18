find_package(Boost REQUIRED COMPONENTS system context)

# 公共链接库
set(COMMON_LIBS Boost::system Boost::context pthread jsoncpp)

set(TEST_SOURCES
#  ./jsonExample/jsonDemo.cc
#  ./coExample/coClient.cc
#  ./coExample/coServer.cc
#  ./beastHttpServer/beastHttpServer.cc
  ./webSocket/websocket_client_coro.cc
  ./webSocket/websocket_server_coro.cc
)

# 自定义函数，用于编译每个 .cc 文件为同名的可执行文件
function(add_test_executable source_file)
  get_filename_component(target_name ${source_file} NAME_WE)
  add_executable(${target_name} ${source_file})
  target_link_libraries(${target_name} ${COMMON_LIBS})
endfunction()

# 手动添加需要编译的文件
foreach(source_file ${TEST_SOURCES})
  add_test_executable(${source_file})
endforeach()

#set(protoDemo_list protoExample/demo.cc protoExample/msg.pb.cc)
#add_executable(protoDemo ${protoDemo_list})
#target_link_libraries(protoDemo pthread protobuf)

#add_subdirectory(logicServer)
#add_subdirectory(BoostCoServer)
add_subdirectory(grpcExample)
add_subdirectory(webSocket)
add_subdirectory(nqueen)