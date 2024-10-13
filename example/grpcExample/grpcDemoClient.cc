#include "demo.grpc.pb.h"
#include "demo.pb.h"
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/client_context.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/security/credentials.h>
#include <iostream>
#include <memory>
#include <string>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using hello::Greeter;
using hello::HelloReply;
using hello::HelloRequest;

class FCClient {
public:
  FCClient(std::shared_ptr<Channel> channel)
      : _stub(Greeter::NewStub(channel)) {}
  std::string SayHello(std::string name) {
    ClientContext context;
    HelloReply reply;
    HelloRequest request;
    request.set_message(name);
    Status status = _stub->SayHello(&context, request, &reply);
    if (status.ok()) {
      return reply.message();
    } else {
      return "failure " + status.error_message();
    }
  }
private:
  std::unique_ptr<Greeter::Stub> _stub;
};

int main() {

  auto channel = grpc::CreateChannel("127.0.0.1:50051",
                                     grpc::InsecureChannelCredentials());
  FCClient client(channel);
  std::string res = client.SayHello("hello guitou");
  std::cout << "get reply : " << res << std::endl;
  return 0;

}