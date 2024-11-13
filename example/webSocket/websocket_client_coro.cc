// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// 本代码分发在 Boost 软件许可证1.0版本下，详细信息参见LICENSE_1_0.txt文件。
// 官方仓库: https://github.com/boostorg/beast

// -----------------------------------------------------------------------------
// 示例：WebSocket 客户端，协程实现
// -----------------------------------------------------------------------------

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/spawn.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>

namespace beast = boost::beast;         // beast命名空间，用于网络流相关的类
namespace http = beast::http;           // http命名空间，用于处理HTTP
namespace websocket = beast::websocket; // websocket命名空间，用于WebSocket协议
namespace net = boost::asio;            // net命名空间，用于异步操作和网络相关类
using tcp = boost::asio::ip::tcp;       // 使用TCP协议

// -----------------------------------------------------------------------------
// 失败报告函数
void fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n"; // 打印错误信息
}

// 发送 WebSocket 消息并打印响应
void do_session(
        std::string host,                   // 主机名
        std::string const& port,            // 端口号
        std::string const& text,            // 要发送的文本消息
        net::io_context& ioc,               // I/O 上下文
        net::yield_context yield)           // 用于协程支持
{
    beast::error_code ec;                    // 用于存储错误代码

    // 创建用于 I/O 的对象
    tcp::resolver resolver(ioc);             // DNS 解析器，解析主机名和端口
    websocket::stream<beast::tcp_stream> ws(ioc); // WebSocket 流对象，用于处理WebSocket连接

    // 解析主机名
    auto const results = resolver.async_resolve(host, port, yield[ec]);
    if(ec)
        return fail(ec, "resolve");          // 如果解析出错，返回错误

    // 设置超时时间
    beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));

    // 连接到解析结果的 IP 地址
    auto ep = beast::get_lowest_layer(ws).async_connect(results, yield[ec]);
    if(ec)
        return fail(ec, "connect");          // 如果连接出错，返回错误

    // 更新主机字符串，用于设置 WebSocket 握手时的 Host HTTP 头部
    host += ':' + std::to_string(ep.port());

    // 关闭TCP流的超时，因为WebSocket有自己的超时机制
    beast::get_lowest_layer(ws).expires_never();

    // 设置建议的 WebSocket 超时选项
    ws.set_option(
            websocket::stream_base::timeout::suggested(
                    beast::role_type::client));

    // 设置 User-Agent 字段
    ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-client-coro");
            }));

    // 执行 WebSocket 握手
    ws.async_handshake(host, "/", yield[ec]);
    if(ec)
        return fail(ec, "handshake");       // 如果握手失败，返回错误

    // 发送消息
    ws.async_write(net::buffer(std::string(text)), yield[ec]);
    if(ec)
        return fail(ec, "write");           // 如果发送失败，返回错误

    // 创建缓冲区用于接收消息
    beast::flat_buffer buffer;

    // 读取消息到缓冲区
    ws.async_read(buffer, yield[ec]);
    if(ec)
        return fail(ec, "read");            // 如果读取失败，返回错误

    // 关闭 WebSocket 连接
    ws.async_close(websocket::close_code::normal, yield[ec]);
    if(ec)
        return fail(ec, "close");           // 如果关闭失败，返回错误

    // 如果成功关闭连接，输出接收到的消息
    std::cout << beast::make_printable(buffer.data()) << std::endl;
}

// -----------------------------------------------------------------------------
// 主函数
int main(int argc, char** argv)
{
    // 检查命令行参数
    if(argc != 4)
    {
        std::cerr <<
                  "用法: websocket-client-coro <host> <port> <text>\n" <<
                  "示例:\n" <<
                  "    websocket-client-coro echo.websocket.org 80 \"Hello, world!\"\n";
        return EXIT_FAILURE;                 // 参数不正确，退出
    }
    auto const host = argv[1];
    auto const port = argv[2];
    auto const text = argv[3];

    // I/O 上下文，所有 I/O 操作都基于此
    net::io_context ioc;

    // 启动异步操作
    boost::asio::spawn(ioc, std::bind(
                               &do_session,
                               std::string(host),
                               std::string(port),
                               std::string(text),
                               std::ref(ioc),
                               std::placeholders::_1),
                       [](std::exception_ptr ex)
                       {
                           // 如果协程中出现异常，抛出该异常
                           if (ex)
                               std::rethrow_exception(ex);
                       });

    // 运行 I/O 服务，直到套接字关闭
    ioc.run();

    return EXIT_SUCCESS;                   // 退出程序
}