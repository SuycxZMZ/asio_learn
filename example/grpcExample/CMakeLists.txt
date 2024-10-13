find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(SERVER_SRC
  demo.pb.cc
  demo.grpc.pb.cc
  grpcDemoServer.cc 
)

set(Client_SRC
  demo.pb.cc
  demo.grpc.pb.cc
  grpcDemoClient.cc
)

add_executable(grpcDemoServer ${SERVER_SRC})
add_executable(grpcDemoClient ${Client_SRC})

target_link_libraries(grpcDemoServer
  gRPC::grpc++
)

target_link_libraries(grpcDemoClient
  gRPC::grpc++
)