#include "zookeeperutil.h"

// zk会话的超时事件
static const int kZkTimeout = 30000;
// 存放节点路径的缓冲区大小
static const int kPathBufSize = 128;
// 存放节点数据的缓冲区大小
static const int kDataBufSize = 64;

void WatcherFn(zhandle_t *zh, int type, 
        int state, const char *path, void *watcherCtx) {
    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_CONNECTED_STATE) {
            sem_t* sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZkClient::ZkClient()
    : zhandle_(nullptr) {}

ZkClient::~ZkClient() {
    if (zhandle_ != nullptr) {
        zookeeper_close(zhandle_);
    }
}

void ZkClient::Start() {
    std::string ip = RpcApplication::GetInstance().GetConfig().QueryConfigInfo("zookeeperip");
    std::string port = RpcApplication::GetInstance().GetConfig().QueryConfigInfo("zookeeperport");
    std::string host = ip + ":" + port;
    zhandle_ = zookeeper_init(host.c_str(), WatcherFn, kZkTimeout, nullptr, nullptr, 0);
    if (zhandle_ == nullptr) {
        LOG_ERROR("zookeeper_init error!");
        exit(EXIT_FAILURE);
    }
    // zookeeper_init为异步操作，所以这里使用信号量来通信
    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(zhandle_, &sem);
    sem_wait(&sem);
    std::cout << "zookeeper init success!" << std::endl;
}

// 先判断节点是否存在，避免重复创建节点
void ZkClient::Create(const char* path, const char* data, int datalen, int ephemeral) {
    int is_exist = zoo_exists(zhandle_, path, 0, nullptr);
    if (is_exist == ZNONODE) {
        char path_buf[kPathBufSize];
        is_exist = zoo_create(zhandle_, path, data, datalen, 
        &ZOO_OPEN_ACL_UNSAFE, ephemeral, path_buf, kPathBufSize);
        if (is_exist == ZOK) {
            LOG_NORMAL("znode create surccess, path is %s", path);
        }
        else {
            LOG_ERROR("znode create error");
            exit(EXIT_FAILURE);
        }
    }
}

std::string ZkClient::GetData(const char* path) {
    char buf[kDataBufSize];
    int buflen = kDataBufSize;
    int is_got = zoo_get(zhandle_, path, 0, buf, &buflen, nullptr);
    if (is_got == ZOK) {
        return buf;
    }
    else {
        LOG_NORMAL("get znode data error, path is %s", path);
        return "";
    }
}