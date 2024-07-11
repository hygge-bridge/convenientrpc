#include <iostream>
#include <fstream>
#include "rpcconfig.h"
#include "rpcapplication.h"

void RpcConfig::Strip(std::string& str) {
    int slow = 0;
    for (int fast = 0; fast < str.size(); ++fast) {
        if (str[fast] != ' ') {
            str[slow++] = str[fast];
        }
    }
    str = str.substr(0, slow);
}

void RpcConfig::LoadConfigFile(const std::string& config_file) {
    std::fstream file(config_file);
    if (!file.is_open()) {
        // std::cout << "The file:" << config_file <<  " cannot be opened. " << std::endl;
        LOG_ERROR("The file: %s cannot be opened.", config_file.c_str());
        exit(EXIT_FAILURE);
    }
    std::string line_buf;
    while (std::getline(file, line_buf)) {
        Strip(line_buf);
        // 忽略注释信息
        if (line_buf[0] == '#') {
            continue;
        }
        // 获取配置信息
        int equal_pos = line_buf.find_first_of('=');
        std::string key = line_buf.substr(0, equal_pos);
        std::string val = line_buf.substr(equal_pos + 1, line_buf.size() - equal_pos - 1);
        config_map_.insert({key, val});
    }
    file.close();
}

std::string RpcConfig::QueryConfigInfo(const std::string& key) const {
    auto it = config_map_.find(key);
    if (it == config_map_.end()) {
        return std::string();
    }
    return it->second;
}