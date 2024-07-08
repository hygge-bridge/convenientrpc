#include "user.pb.h"
#include "rpcapplication.h"
#include "rpcchannel.h"

int main(int argc, char** argv) {
    RpcApplication::Init(argc, argv);
    hygge::UserServiceRpc_Stub user_stub(new RpcChannel());
    
    hygge::LoginRequest request;
    request.set_name("hygge");
    request.set_pwd("123");
    hygge::LoginResponse response;
    user_stub.Login(nullptr, &request, &response, nullptr);

    if (response.success() == 0) {
        std::cout << "rpc method: login() error! err code: " << response.res().errcode() << 
                     " err msg" << response.res().errmsg() << std::endl;
    }
    else {
        std::cout << "rpc method: login() success! success: " << response.success() << std::endl;
    }
    return 0;
}