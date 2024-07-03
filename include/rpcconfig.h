#ifndef RPC_CONFIG_H
#define RPC_CONFIG_H

#include <string>
#include <unordered_map>

// 配置文件相关类
class RpcConfig {
public:
    // 加载配置文件
    void LoadConfigFile(const std::string& config_file);

    // 查询配置信息
    std::string QueryConfigInfo(const std::string& key) const;
private:
    // 去除字符串中多余的空格
    void Strip(std::string& str);

    // 配置文件信息的map表，配置信息如：ip=127.0.0.1
    std::unordered_map<std::string, std::string> config_map_;
};

#endif