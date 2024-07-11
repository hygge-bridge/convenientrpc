#include "rpccontroller.h"

RpcController::RpcController() 
    : is_failed_(false)
    , err_text_("") {}

void RpcController::Reset() {
    is_failed_ = false;
    err_text_ = "";
}

void RpcController::SetFailed(const std::string& reason) {
    is_failed_ = true;
    err_text_ = reason;
}

// 本项目暂时无需使用这些方法，故不予具体实现
void RpcController::StartCancel() {}
bool RpcController::IsCanceled() const { return false; }
void RpcController::NotifyOnCancel(google::protobuf::Closure* callback) {}