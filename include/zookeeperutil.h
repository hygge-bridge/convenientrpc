#ifndef ZOOKEEPER_UTIL_H
#define ZOOKEEPER_UTIL_H

#include "rpcapplication.h"
#include <zookeeper/zookeeper.h>
#include <semaphore.h>
#include <string>

// zookeeper客户端类
class ZkClient {
public:
    ZkClient();
    ~ZkClient();
    // 启动zkClient，开始连接zkServer
    void Start();
    // 创建zk的一个节点，其ephemeral默认为0表示创建永久性节点
    void Create(const char* path, const char* data, int datalen, int ephemeral = 0);
    // 获取节点的数据
    std::string GetData(const char* path);
private:
    // zk的句柄
    zhandle_t* zhandle_;
};

#endif