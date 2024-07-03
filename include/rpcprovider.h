#ifndef RPC_PROVIDER_H
#define RPC_PROVIDER_H

#include "google/protobuf/service.h"

class RpcProvider {
public:
    // 发布rpc服务
    void NotifyService(google::protobuf::Service* service);

    // 启动rpc服务
    void Run();
};

#endif