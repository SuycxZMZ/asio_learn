// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/detached.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>

namespace beast = boost::beast;         // 从 <boost/beast.hpp> 导入命名空间
namespace websocket = beast::websocket; // 从 <boost/beast/websocket.hpp> 导入 WebSocket 支持
namespace net = boost::asio;            // 从 <boost/asio.hpp> 导入异步 I/O 支持
using tcp = boost::asio::ip::tcp;       // 从 <boost/asio/ip/tcp.hpp> 导入 TCP 协议支持

// 报告错误信息
void fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

// 处理 WebSocket 会话（协程）
void do_session(tcp::socket socket, net::yield_context yield) {
    beast::error_code ec;

    // 创建 WebSocket 流对象
    websocket::stream<beast::tcp_stream> ws(std::move(socket));

    // 设置超时
    beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));

    // 执行 WebSocket 握手操作
    ws.async_accept(yield[ec]);
    if(ec)
        return fail(ec, "accept");

    // 关闭超时，因为 WebSocket 自带超时控制
    beast::get_lowest_layer(ws).expires_never();

    // 为 WebSocket 流设置建议的超时参数
    ws.set_option(
            websocket::stream_base::timeout::suggested(
                    beast::role_type::server));

    // 处理循环，持续接收和发送消息
    for(;;) {
        // 创建缓冲区以存储消息
        beast::flat_buffer buffer;

        // 异步读取消息到缓冲区
        ws.async_read(buffer, yield[ec]);
        if(ec == websocket::error::closed)
            break;
        if(ec)
            return fail(ec, "read");

        // 将接收到的消息原样发回客户端（回声服务）
        ws.async_write(buffer.data(), yield[ec]);
        if(ec)
            return fail(ec, "write");
    }
}

// 监听传入连接的函数
void do_listen(
        net::io_context& ioc,
        tcp::endpoint endpoint,
        net::yield_context yield) {
    beast::error_code ec;

    // 创建并打开监听 socket
    tcp::acceptor acceptor(ioc);
    acceptor.open(endpoint.protocol(), ec);
    if(ec) {
        fail(ec, "open");
        return;
    }

    // 允许地址重用
    acceptor.set_option(net::socket_base::reuse_address(true), ec);
    if(ec) {
        fail(ec, "set_option");
        return;
    }

    // 绑定到指定端口
    acceptor.bind(endpoint, ec);
    if(ec) {
        fail(ec, "bind");
        return;
    }

    // 开始监听连接
    acceptor.listen(net::socket_base::max_listen_connections, ec);
    if(ec) {
        fail(ec, "listen");
        return;
    }

    for(;;) {
        // 接受传入连接
        tcp::socket socket(ioc);
        acceptor.async_accept(socket, yield[ec]);
        if(ec) {
            fail(ec, "accept");
        } else {
            // 每个连接创建一个会话
            boost::asio::spawn(
                    acceptor.get_executor(),
                    [socket = std::move(socket)](boost::asio::yield_context yield) mutable {
                        do_session(std::move(socket), yield);
                    },
                    boost::asio::detached
            );
        }
    }
}

int main(int argc, char* argv[]) {
    // 检查命令行参数
    if(argc != 3) {
        std::cerr << "Usage: websocket-server-coro <address> <port>\n" <<
                  "Example:\n" <<
                  "    websocket-server-coro 0.0.0.0 8080\n";
        return EXIT_FAILURE;
    }
    auto const address = net::ip::make_address(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));

    // 创建 io_context 对象用于所有 I/O 操作
    net::io_context ioc{ 1 };

    // 启动异步操作
    boost::asio::spawn(ioc,
               std::bind(
                       &do_listen,
                       std::ref(ioc),
                       tcp::endpoint{address, port},
                       std::placeholders::_1), boost::asio::detached);

    // 运行 I/O 服务，直到所有连接结束
    ioc.run();

    return EXIT_SUCCESS;
}