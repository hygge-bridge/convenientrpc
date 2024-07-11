#include "rpcchannel.h"
#include "rpcheader.pb.h"
#include "rpcapplication.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <memory>
#include <functional>
#include <string>

// 发送和接收缓冲区大小
static const int kBufSize = 1024;

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                  ::google::protobuf::RpcController* controller,
                  const ::google::protobuf::Message* request,
                  ::google::protobuf::Message* response,
                  ::google::protobuf::Closure* done) {
    const google::protobuf::ServiceDescriptor* service = method->service();
    const std::string service_name = service->name();
    const std::string method_name = method->name();
    // 序列化参数数据
    std::string args_str;
    if (!request->SerializeToString(&args_str)) {
        //std::cout << "Failed to serialize args to string" << std::endl;
        controller->SetFailed("Failed to serialize args to string");
        return;
    }
    uint32_t args_size = args_str.size();
    // 序列化rpc数据头
    rpc::RpcHeader rpcheader;
    rpcheader.set_service_name(service_name);
    rpcheader.set_method_name(method_name);
    rpcheader.set_args_size(args_size);
    std::string rpcheader_str;
    if (!rpcheader.SerializeToString(&rpcheader_str)) {
        //std::cout << "Failed to serialize rpcheader to string!" << std::endl;
        controller->SetFailed("Failed to serialize rpcheader to string!");
        return;
    }
    uint32_t rpcheader_size = rpcheader_str.size();
    // 组装数据
    std::string send_request_str;
    // 直接将二进制内存数据插入到发送字符串中
    send_request_str.insert(0, std::string((char*)&rpcheader_size, 4));
    send_request_str += rpcheader_str;
    send_request_str += args_str;

    // 打印调试信息
    std::cout << "-------------------------------" << std::endl;
    std::cout << "send_request_str: " << send_request_str << std::endl;
    std::cout << "rpcheader_size: " << rpcheader_size << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "-------------------------------" << std::endl;

    // 调用方无需进行高并发，所以直接利用原生Linux网络编程API进行网络通信
    std::unique_ptr<int, std::function<void(int*)>> clientfd_ptr(
        new int(socket(AF_INET, SOCK_STREAM, 0)), 
        [](int* clientfd) { close(*clientfd); }
    );  // 自定义删除器用于关闭套接字
    if (*clientfd_ptr == -1) {
        //std::cout << "Failed to create socket!" << std::endl;
        //exit(EXIT_FAILURE);
        controller->SetFailed("Failed to create socket!");
        return;
    }
    std::string ip = RpcApplication::GetInstance().GetConfig().QueryConfigInfo("rpcserverip");
    uint16_t port = atoi(RpcApplication::GetInstance().GetConfig().QueryConfigInfo("rpcserverport").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (connect(*clientfd_ptr, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        // std::cout << "Failed to connect to server" << std::endl;
        // exit(EXIT_FAILURE);
        controller->SetFailed("Failed to connect to server");
        return;
    }
    if (send(*clientfd_ptr, send_request_str.c_str(), send_request_str.size(), 0) == -1) {
        // std::cout << "Failed to send data to server" << std::endl;
        controller->SetFailed("Failed to send data to server");
        return;
    }
    char recv_buf[kBufSize];
    int recv_size = 0;
    if ((recv_size = recv(*clientfd_ptr, recv_buf, sizeof(recv_buf), 0)) == -1) {
        // std::cout << "Failed to receive data from server" << std::endl;
        controller->SetFailed("Failed to receive data from server");
        return;
    }
    if (!response->ParseFromArray(recv_buf, recv_size)) {
        // std::cout << "Failed to Parse recv_buf" << std::endl;
        controller->SetFailed("Failed to Parse recv_buf");
        return;
    }
}