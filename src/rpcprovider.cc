#include "rpcprovider.h"
#include <thread>

// 根据cpu核心数量来设置线程个数
static const int kThreadNum = std::thread::hardware_concurrency();

// 发布服务需要将对应服务和其方法的 名字和描述信息转入映射表中，
// 这样后续rpc框架才可以根据该映射表找到对应的方法
void RpcProvider::NotifyService(google::protobuf::Service* service) {
    // 获取服务名字和描述
    const google::protobuf::ServiceDescriptor* service_descriptor = service->GetDescriptor();
    const std::string service_name = service_descriptor->name();
    int method_cnt = service_descriptor->method_count();
    ServiceInfo service_info;
    std::cout << "service name: " << service_name << std::endl;
    // 获取方法名字和描述
    for (int i = 0 ; i < method_cnt; ++i) {
        const google::protobuf::MethodDescriptor *method_descriptor = service_descriptor->method(i);
        const std::string method_name = method_descriptor->name();
        service_info.method_map.insert({method_name, method_descriptor});
        std::cout << "method name: " << method_name << std::endl;
    }
    service_info.service = service;
    service_map_.insert({service_name, service_info});
}

void RpcProvider::Run() {
    std::string ip = RpcApplication::GetInstance().GetConfig().QueryConfigInfo("rpcserverip");
    uint16_t port = atoi(RpcApplication::GetInstance().GetConfig().QueryConfigInfo("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);
    muduo::net::TcpServer tcp_server(&event_loop_, address, "rpcprovider");
    tcp_server.setThreadNum(kThreadNum);  
    std::cout << "rpcprovider start: ip=" << ip << " port=" << port << std::endl;
    tcp_server.start();
    event_loop_.loop();
}


void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn) {

}


void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, 
                muduo::net::Buffer* buffer, muduo::Timestamp time) {
    
}