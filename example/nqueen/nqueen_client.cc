//
// Created by yuan su on 24-11-17.
//

/*
===========================================
| N-Queens Problem Solutions (N=8 to 20) |
===========================================
|  N  |      Solutions                   |
|-----|----------------------------------|
|  8  |            92                    |
|  9  |           352                    |
| 10  |           724                    |
| 11  |         2,680                    |
| 12  |        14,200                    |
| 13  |        73,712                    |
| 14  |       365,596                    |
| 15  |     2,279,184                    |
| 16  |    14,772,512                    |
| 17  |    95,815,104                    |
| 18  |   666,090,624                    |
| 19  | 4,968,057,848                    |
| 20  |39,029,188,884                    |
===========================================
*/

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <future>
#include <chrono> // 添加用于时间测量的头文件
#include <grpcpp/grpcpp.h>
#include "nqueen.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using nqueen::NQueenSolver;
using nqueen::TaskRequest;
using nqueen::TaskResult;

// NQueenClient 类定义
class NQueenClient {
public:
    NQueenClient(std::shared_ptr<Channel> channel)
            : stub_(NQueenSolver::NewStub(channel)) {}

    // 发送任务并获取结果
    int Solve(int n, const std::vector<int>& partial) {
        TaskRequest request;
        TaskResult reply;
        request.set_n(n);
        for (const auto& pos : partial) {
            request.add_partial(pos);
        }
        ClientContext context;
        Status status = stub_->Solve(&context, request, &reply);
        if (status.ok()) {
            return reply.solutions();
        } else {
            std::cerr << "RPC failed: " << status.error_message() << std::endl;
            return 0;
        }
    }

private:
    std::unique_ptr<NQueenSolver::Stub> stub_;
};

// 本地单线程 N 皇后求解器（回溯算法）
class LocalNQueenSolver {
public:
    LocalNQueenSolver(int n) : n_(n), total_solutions_(0) {}

    // 开始求解并返回总解法数量
    long long Solve() {
        std::vector<int> board(n_, -1);
        backtrack(board, 0);
        return total_solutions_;
    }

private:
    int n_;
    long long total_solutions_;

    // 回溯函数
    void backtrack(std::vector<int>& board, int row) {
        if (row == n_) {
            total_solutions_++;
            return;
        }
        for (int col = 0; col < n_; ++col) {
            if (isSafe(board, row, col)) {
                board[row] = col;
                backtrack(board, row + 1);
                board[row] = -1; // 回溯
            }
        }
    }

    // 检查当前位置是否安全
    bool isSafe(const std::vector<int>& board, int row, int col) {
        for (int i = 0; i < row; ++i)
            if (board[i] == col || abs(board[i] - col) == abs(i - row))
                return false;
        return true;
    }
};

// 生成所有可能的初始部分棋盘配置，并分发给服务器
void distribute_tasks(int n, int server_count, long long& distributed_solutions, double& distributed_time) {
    // 生成所有在第0行放置皇后的初始任务
    std::vector<std::vector<int>> initial_tasks;
    for (int col = 0; col < n; ++col) {
        std::vector<int> partial(n, -1);
        partial[0] = col;
        initial_tasks.push_back(partial);
    }

    int total_tasks = initial_tasks.size();
    distributed_solutions = 0;
    distributed_time = 0.0;

    std::vector<std::future<int>> futures;

    auto start_time = std::chrono::high_resolution_clock::now(); // 开始计时

    for (int i = 0; i < total_tasks; ++i) {
        int server_id = i % server_count;
        std::string server_address = "localhost:" + std::to_string(50051 + server_id);

        // 创建 shared_ptr 管理 client 实例
        std::shared_ptr<NQueenClient> client = std::make_shared<NQueenClient>(
                grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));

        // 异步处理每个任务，捕获 shared_ptr
        futures.emplace_back(
                std::async(std::launch::async,
                           [client, n, &initial_tasks, i]() {
                               return client->Solve(n, initial_tasks[i]);
                           }
                )
        );

        std::cout << "Dispatched Task " << i + 1 << " to " << server_address << std::endl;
    }

    // 收集所有任务的结果
    for (int i = 0; i < total_tasks; ++i) {
        int solutions = futures[i].get();
        distributed_solutions += solutions;
        std::cout << "Task " << i + 1 << "/" << total_tasks
                  << " returned " << solutions << " solutions." << std::endl;
    }

    auto end_time = std::chrono::high_resolution_clock::now(); // 结束计时
    std::chrono::duration<double> duration = end_time - start_time;
    distributed_time = duration.count();

    std::cout << "Total distributed solutions for " << n << " queens: " << distributed_solutions << std::endl;
    std::cout << "Distributed solving time: " << distributed_time << " seconds." << std::endl;
}

// 本地单线程求解并测量时间
long long local_solve(int n, double& solve_time) {
    LocalNQueenSolver solver(n);

    auto start_time = std::chrono::high_resolution_clock::now(); // 开始计时
    long long solutions = solver.Solve();
    auto end_time = std::chrono::high_resolution_clock::now(); // 结束计时

    std::chrono::duration<double> duration = end_time - start_time;
    solve_time = duration.count();

    std::cout << "Total local solutions for " << n << " queens: " << solutions << std::endl;
    std::cout << "Local solving time: " << solve_time << " seconds." << std::endl;

    return solutions;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <n> <server_count>" << std::endl;
        return 1;
    }

    int n = std::stoi(argv[1]);
    int server_count = std::stoi(argv[2]);

    std::cout << "Starting distributed solving for " << n << " queens using " << server_count << " servers." << std::endl;

    long long distributed_solutions = 0;
    double distributed_time = 0.0;

    // 分布式求解
    distribute_tasks(n, server_count, distributed_solutions, distributed_time);

    // 本地单线程求解
    std::cout << "\nStarting local single-threaded solving for " << n << " queens." << std::endl;
    double local_time = 0.0;
    long long local_solutions = local_solve(n, local_time);

    // 对比结果
    std::cout << "\n===== Comparison =====" << std::endl;
    std::cout << "Distributed Solutions: " << distributed_solutions << " | Time: " << distributed_time << " seconds." << std::endl;
    std::cout << "Local Solutions:      " << local_solutions << " | Time: " << local_time << " seconds." << std::endl;

    // 验证解法数量是否一致
    if (distributed_solutions == local_solutions) {
        std::cout << "Verification: SUCCESS - Both methods found the same number of solutions." << std::endl;
    } else {
        std::cout << "Verification: FAILURE - Mismatch in the number of solutions." << std::endl;
    }

    return 0;
}