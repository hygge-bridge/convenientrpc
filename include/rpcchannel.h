#ifndef RPC_CHANNEL_H
#define RPC_CHANNEL_H

#include <google/protobuf/service.h>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

class RpcChannel : public google::protobuf::RpcChannel {
public:
    // 所有通过stub代理对象调用的rpc底层都是调用的此方法，主要做请求数据的序列化和网络发送
    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                  ::google::protobuf::RpcController* controller,
                  const ::google::protobuf::Message* request,
                  ::google::protobuf::Message* response,
                  ::google::protobuf::Closure* done);
};

#endif