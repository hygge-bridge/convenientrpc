#include "rpcprovider.h"
#include "rpcheader.pb.h"
#include <thread>
#include <functional>

// 根据cpu核心数量来设置线程个数
static const int kThreadNum = std::thread::hardware_concurrency();
// rpc数据头大小被存放在数据的前四个字节
static const int kHeaderSize = 4;

// 发布服务需要将对应服务和其方法的 名字和描述信息转入映射表中，
// 这样后续rpc框架才可以根据该映射表找到对应的方法
void RpcProvider::NotifyService(google::protobuf::Service* service) {
    // 获取服务名字和描述
    const google::protobuf::ServiceDescriptor* service_descriptor = service->GetDescriptor();
    const std::string service_name = service_descriptor->name();
    int method_cnt = service_descriptor->method_count();
    ServiceInfo service_info;
    // std::cout << "service name: " << service_name << std::endl;
    LOG_NORMAL("service name:%s", service_name.c_str());
    // 获取方法名字和描述
    for (int i = 0 ; i < method_cnt; ++i) {
        const google::protobuf::MethodDescriptor *method_descriptor = service_descriptor->method(i);
        const std::string method_name = method_descriptor->name();
        service_info.method_map.insert({method_name, method_descriptor});
        // std::cout << "method name: " << method_name << std::endl;
        LOG_NORMAL("method name:%s", method_name.c_str());
    }
    service_info.service = service;
    service_map_.insert({service_name, service_info});
}

void RpcProvider::Run() {
    // 创建tcp server对象
    std::string ip = RpcApplication::GetInstance().GetConfig().QueryConfigInfo("rpcserverip");
    uint16_t port = atoi(RpcApplication::GetInstance().GetConfig().QueryConfigInfo("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);
    muduo::net::TcpServer tcp_server(&event_loop_, address, "rpcprovider");
    // 绑定回调
    tcp_server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    tcp_server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, 
                                 std::placeholders::_2, std::placeholders::_3));

    tcp_server.setThreadNum(kThreadNum);  
    std::cout << "rpcprovider start: ip=" << ip << " port=" << port << std::endl;
    tcp_server.start();
    event_loop_.loop();
}


void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn) {
    // rpc是个短连接，返回rpc响应后就可以关闭连接 
    if (!conn->connected()) {
        conn->shutdown();  // 连接关闭，即关闭套接字
    }
}

// rpc数据被存放的数据结构如下：前四个字节为数据头大小、后续为数据头和参数。
// 数据头包含服务名、方法名和参数大小
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, 
                muduo::net::Buffer* buffer, muduo::Timestamp time) {
    std::string buf = buffer->retrieveAllAsString();
    uint32_t header_size = 0;
    // 将int强转为char*是为了将大小直接存储到header_size的内存中
    // 此处无法采用字符串转为整数，比如10000转成整数会占用5个字节
    buf.copy((char*)&header_size, 4, 0);  
    std::string header_str = buf.substr(4, header_size);
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    rpc::RpcHeader header;
    if (header.ParseFromString(header_str)) {
        service_name = header.service_name();
        method_name = header.method_name();
        args_size = header.args_size();
    }
    else {
        std::cout << "failed to parse from " << header_str << std::endl;
        return;
    }
    std::string args_str = buf.substr(4 + header_size, args_size);
    // 测试信息
    // std::cout << "---------------------------" << std::endl;
    // std::cout << "header_size: " << header_size << std::endl;
    // std::cout << "service_name: " << service_name << std::endl;
    // std::cout << "method_name: " << method_name << std::endl;
    // std::cout << "args_str: " << args_str << std::endl;
    // std::cout << "---------------------------" << std::endl;
    LOG_NORMAL("---------------------------");
    LOG_NORMAL("header_size: %d", header_size);
    LOG_NORMAL("service_name: %s", service_name.c_str());
    LOG_NORMAL("method_name: %s", method_name.c_str());
    LOG_NORMAL("args_str: %s", args_str.c_str());
    LOG_NORMAL("---------------------------");

    // 获取服务和方法描述
    auto service_it = service_map_.find(service_name);
    if (service_it == service_map_.end()) {
        // std::cout << "failed to find the " << service_name << std::endl;
        LOG_ERROR("failed to find the  %s", service_name.c_str());
        return;
    }
    auto method_it = service_it->second.method_map.find(method_name);
    if (method_it == service_it->second.method_map.end()) {
        // std::cout << "failed to find the " << method_name << std::endl;
        LOG_ERROR("failed to find the  %s", method_name.c_str());
        return;
    }
    google::protobuf::Service* service = service_it->second.service;
    const google::protobuf::MethodDescriptor *method = method_it->second;
    
    // 生成request请求、response响应和done回调，然后进行方法调用
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str)) {
        // std::cout << "failed to parse from " << args_str << std::endl;
        LOG_ERROR("failed to parse from %s", args_str.c_str());
        return;
    }
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();
    // 进行模板的实参推演时会将response推演为指针的引用，很奇怪，所以这里直接指定了模版的实参类型
    google::protobuf::Closure* done = google::protobuf::NewCallback
        <RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
        (this, &RpcProvider::SendRpcResponse, conn, response); 
    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* message) {
    std::string response_str;
    if (!message->SerializeToString(&response_str)) {
        // std::cout << "failed to serialize " << response_str << " to string" << std::endl;
        LOG_ERROR("failed to serialize %s to string", response_str.c_str());
        return;
    }
    conn->send(response_str);
    conn->shutdown();
}