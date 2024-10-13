# 个人使用

## [example/jsonExample](example/jsonExample)

json库：[jsoncpp](https://github.com/open-source-parsers/jsoncpp)简单使用

## [example/protoExample](example/protoExample)

protobuf简单使用，[grpc和配套的protobuf安装](https://www.llfc.club/category?catid=225RaiVNI8pFDD5L4m807g7ZwmF#!aid/2TIG572uTKxQxned7LCk8KoulfL)

## [example/logicServer](example/logicServer)

一个简单的分层服务器例子，使用多个`io_context`，每个汇话最终回调把接收信息加入逻辑处理器的任务队列，
逻辑处理线程从任务队列中取任务执行。单线程处理任务，多线程处理网络IO，回发的操作其实也在逻辑线程里，不影响理解

## [example/coExample](example/coExample)

`boost`协程例子，一个单线程异步echoServer和一个同步client

## [example/BoostCoServer](example/BoostCoServer)

接收部分使用boost协程，一个简单的分层服务器例子，使用多个`io_context`，每个汇话最终回调把接收信息加入逻辑处理器的任务队列，
逻辑处理线程从任务队列中取任务执行。
