#include <iostream>
#include <string>
#include "user.pb.h"
#include "rpcapplication.h"
#include "rpcprovider.h"

// 用户服务类，不实现具体业务，只是作为简单示例
class UserService : public hygge::UserServiceRpc {
public:
    // 登录服务
    bool Login(std::string& name, std::string& pwd) {
        std::cout << "Login: " << name << " " << pwd << std::endl;
        return true;
    }
    
    // 重写Login方法
    void Login(::google::protobuf::RpcController* controller,
                       const ::hygge::LoginRequest* request,
                       ::hygge::LoginResponse* response,
                       ::google::protobuf::Closure* done) {
        std::string name = request->name();
        std::string pwd = request->pwd();
        bool login_res = Login(name, pwd);  // 模拟做一些业务

        hygge::ResultCode* res = response->mutable_res();
        res->set_errcode(0);
        res->set_errmsg("");
        response->set_success(1);

        done->Run();
    }
};

int main(int argc, char** argv) {
    // 发布rpc的使用示例
    RpcApplication::Init(argc, argv);
    RpcProvider provider;
    provider.NotifyService(new UserService());
    provider.Run();
    return 0;
}