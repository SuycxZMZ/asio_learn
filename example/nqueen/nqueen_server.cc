//
// Created by yuan su on 24-11-17.
//
#include <vector>
#include <cstdlib>
#include <string>
#include <memory>
#include <iostream>
#include <grpcpp/grpcpp.h>
#include "nqueen.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using nqueen::NQueenSolver;
using nqueen::TaskRequest;
using nqueen::TaskResult;

// 计算N皇后问题的解决方案数量
class NQueenServiceImpl final : public NQueenSolver::Service {
public:
    Status Solve(ServerContext* context, const TaskRequest* request,
                 TaskResult* reply) override {
        int n = request->n();
        std::vector<int> partial = {request->partial().begin(), request->partial().end()};

        // 确保partial的大小为n
        if (partial.size() < n) {
            partial.resize(n, -1);
        }

        int solutions = 0;
        solve(n, partial, 0, solutions);
        reply->set_solutions(solutions);
        return Status::OK;
    }

private:
    // 修改后的solve函数，能够处理部分配置
    void solve(int n, std::vector<int>& board, int row, int& solutions) {
        if (row == n) {
            solutions++;
            return;
        }

        if (board[row] != -1) {
            // 如果当前行已经放置了皇后，跳过到下一行
            if (is_safe(board, row, board[row])) {
                solve(n, board, row + 1, solutions);
            }
            return;
        }

        for (int col = 0; col < n; ++col) {
            if (is_safe(board, row, col)) {
                board[row] = col;
                solve(n, board, row + 1, solutions);
                board[row] = -1;
            }
        }
    }

    bool is_safe(const std::vector<int>& board, int row, int col) {
        for (int i = 0; i < row; ++i) {
            if (board[i] == col || abs(board[i] - col) == abs(i - row)) {
                return false;
            }
        }
        return true;
    }
};

void RunServer(const std::string& server_address) {
    NQueenServiceImpl service;

    ServerBuilder builder;
    // 监听地址和端口
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // 注册服务
    builder.RegisterService(&service);
    // 构建并启动服务器
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    std::string server_address("0.0.0.0:50051");
    if (argc > 1) {
        server_address = std::string("0.0.0.0:") + argv[1];
    }
    RunServer(server_address);
    return 0;
}