#include <iostream>
#include <string>
#include <unistd.h>
#include "rpcapplication.h"

RpcConfig RpcApplication::config_;

// 用户输入配置文件时显示的帮助信息
void ShowArgInfo() {
    std::cout << "format: command -i <configfile>" << std::endl;
}

// 读取用户输入配置文件
void RpcApplication::Init(int argc, char** argv) {
    if (argc < 2) {
        ShowArgInfo();
        exit(EXIT_FAILURE);
    }
    int ch = 0;
    std::string config_file;
    while ((ch = getopt(argc, argv, "i:")) != -1) {
        switch(ch) {
        case 'i':
            config_file = optarg;
            break;
        case '?':
        case ':':
            ShowArgInfo();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    config_.LoadConfigFile(config_file);
}

RpcApplication& RpcApplication::GetInstance() {
    static RpcApplication app;
    return app;
}