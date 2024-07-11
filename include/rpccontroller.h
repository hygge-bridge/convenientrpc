#ifndef RPC_CONTROLLER_H
#define RPC_CONTROLLER_H

#include <google/protobuf/service.h>
#include <string>

class RpcController : public google::protobuf::RpcController {
public:
    RpcController();
    void Reset();
    void SetFailed(const std::string& reason);
    
    bool Failed() const { return is_failed_; }
    std::string ErrorText() const { return err_text_; }

    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);
private:
    bool is_failed_;
    std::string err_text_;
};

#endif