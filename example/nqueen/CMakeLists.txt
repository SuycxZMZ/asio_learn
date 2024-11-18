find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(NQUEENSERVER_SRC
        nqueen_server.cc
        nqueen.pb.cc
        nqueen.grpc.pb.cc
)

set(NQUEENCLIENT_SRC
        nqueen_client.cc
        nqueen.pb.cc
        nqueen.grpc.pb.cc
)

add_executable(nqueen_server ${NQUEENSERVER_SRC})
add_executable(nqueen_client ${NQUEENCLIENT_SRC})

target_link_libraries(nqueen_server
    gRPC::grpc++
    protobuf
    pthread
)

target_link_libraries(nqueen_client
    gRPC::grpc++
    protobuf
    pthread
)