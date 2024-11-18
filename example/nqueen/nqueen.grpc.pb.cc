// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: nqueen.proto

#include "nqueen.pb.h"
#include "nqueen.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace nqueen {

static const char* NQueenSolver_method_names[] = {
  "/nqueen.NQueenSolver/Solve",
};

std::unique_ptr< NQueenSolver::Stub> NQueenSolver::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< NQueenSolver::Stub> stub(new NQueenSolver::Stub(channel));
  return stub;
}

NQueenSolver::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_Solve_(NQueenSolver_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status NQueenSolver::Stub::Solve(::grpc::ClientContext* context, const ::nqueen::TaskRequest& request, ::nqueen::TaskResult* response) {
  return ::grpc::internal::BlockingUnaryCall< ::nqueen::TaskRequest, ::nqueen::TaskResult, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Solve_, context, request, response);
}

void NQueenSolver::Stub::experimental_async::Solve(::grpc::ClientContext* context, const ::nqueen::TaskRequest* request, ::nqueen::TaskResult* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::nqueen::TaskRequest, ::nqueen::TaskResult, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Solve_, context, request, response, std::move(f));
}

void NQueenSolver::Stub::experimental_async::Solve(::grpc::ClientContext* context, const ::nqueen::TaskRequest* request, ::nqueen::TaskResult* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Solve_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::nqueen::TaskResult>* NQueenSolver::Stub::PrepareAsyncSolveRaw(::grpc::ClientContext* context, const ::nqueen::TaskRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::nqueen::TaskResult, ::nqueen::TaskRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Solve_, context, request);
}

::grpc::ClientAsyncResponseReader< ::nqueen::TaskResult>* NQueenSolver::Stub::AsyncSolveRaw(::grpc::ClientContext* context, const ::nqueen::TaskRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncSolveRaw(context, request, cq);
  result->StartCall();
  return result;
}

NQueenSolver::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      NQueenSolver_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< NQueenSolver::Service, ::nqueen::TaskRequest, ::nqueen::TaskResult, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](NQueenSolver::Service* service,
             ::grpc::ServerContext* ctx,
             const ::nqueen::TaskRequest* req,
             ::nqueen::TaskResult* resp) {
               return service->Solve(ctx, req, resp);
             }, this)));
}

NQueenSolver::Service::~Service() {
}

::grpc::Status NQueenSolver::Service::Solve(::grpc::ServerContext* context, const ::nqueen::TaskRequest* request, ::nqueen::TaskResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace nqueen

