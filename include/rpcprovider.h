#ifndef RPC_PROVIDER_H
#define RPC_PROVIDER_H

#include "rpcapplication.h"
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include <string>
#include <unordered_map>

// rpc服务提供类
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

    // 用于服务的回调，返回rpc响应
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* message);

    // 服务的详细信息
    struct ServiceInfo {
        // service指针，用于后续底层调用上层的服务
        google::protobuf::Service* service;  
        // 方法名与方法描述的映射表
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> method_map;  
    };
    // 服务名与服务信息的映射表
    std::unordered_map<std::string, ServiceInfo> service_map_;

    muduo::net::EventLoop event_loop_;
};

#endif