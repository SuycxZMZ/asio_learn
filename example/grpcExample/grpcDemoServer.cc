#include "demo.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <memory>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using hello::Greeter;
using hello::HelloReply;
using hello::HelloRequest;

class GreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext *context, const HelloRequest *request,
                  HelloReply *response) override {
    std::string prefix("HELLO SERVER HAS RECEIVED : ");
    response->set_message(prefix + request->message());
    return Status::OK;
  }
};

void RunServer() {
  std::string server_addr = "127.0.0.1:50051";
  GreeterServiceImpl service;
  ServerBuilder builder;
  builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on:" << server_addr << std::endl;
  server->Wait();
}

int main() {
  RunServer();
  return 0;
}
