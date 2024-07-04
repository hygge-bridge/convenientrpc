#ifndef RPC_PROVIDER_H
#define RPC_PROVIDER_H

#include "rpcapplication.h"
#include "google/protobuf/service.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include <string>

class RpcProvider {
public:
    // 发布rpc服务
    void NotifyService(google::protobuf::Service* service);

    // 启动rpc服务
    void Run();

private:
    // 处理用户连接的回调函数
    void OnConnection(const muduo::net::TcpConnectionPtr& conn);

    // 处理可读写事件的回调函数
    void OnMessage(const muduo::net::TcpConnectionPtr& conn, 
                    muduo::net::Buffer* buffer, muduo::Timestamp time);
    muduo::net::EventLoop event_loop_;
};

#endif