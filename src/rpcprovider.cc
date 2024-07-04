#include "rpcprovider.h"
#include <thread>

// 根据cpu核心数量来设置线程个数
static const int kThreadNum = std::thread::hardware_concurrency();

void RpcProvider::NotifyService(google::protobuf::Service* service) {

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