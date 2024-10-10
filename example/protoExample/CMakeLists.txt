set(protoDemo_list demo.cc msg.pb.cc)
add_executable(protoDemo ${protoDemo_list})
target_link_libraries(protoDemo pthread protobuf)