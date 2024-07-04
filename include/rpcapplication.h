#ifndef RPC_APPLICATION_H
#define RPC_APPLICATION_H

#include "rpcconfig.h"

// rpc应用程序类，通过该类来创建rpc框架服务
class RpcApplication {
public:
    static void Init(int argc, char** argv);
    static RpcApplication& GetInstance();
    static RpcConfig& GetConfig() { return config_; }
private:
    RpcApplication() {}
    RpcApplication(const RpcApplication&) = delete;
    RpcApplication(RpcApplication&&) = delete;
    RpcApplication& operator=(const RpcApplication&) = delete;
    RpcApplication& operator=(RpcApplication&&) = delete;

private:
    // 配置文件相关的成员变量
    static RpcConfig config_;
};

#endif