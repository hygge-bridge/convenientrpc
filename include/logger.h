#ifndef LOGGER_H
#define LOGGER_h

#include "messagequeue.h"
#include <string>
#include <thread>
#include <ctime>
#include <memory>
#include <functional>

// 日志水平
enum LogLevel {
    kNormal, // 普通日志
    kError,  // 错误日志
};

// 日志类
class Logger {
public:
    static Logger& GetInstance();
    void SetLogLevel(LogLevel level) { msg_level_ = level; }
    // 将日志信息添加到消息队列中
    void AddLog(const std::string& msg);
private:
    int msg_level_;
    MessageQueue<std::string> msg_queue_;

    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};

static const int kMsgBufSize = 1024;
// 定义宏，让用户可以便捷地书写日志
#define LOG_NORMAL(logmsgformat, ...) \
do { \
    Logger& logger = Logger::GetInstance(); \
    logger.SetLogLevel(kNormal); \
    char msg[kMsgBufSize]; \
    snprintf(msg, kMsgBufSize, logmsgformat, ##__VA_ARGS__); \
    logger.AddLog(std::string(msg)); \
} while(0); 

#define LOG_ERROR(logmsgformat, ...) \
do { \
    Logger& logger = Logger::GetInstance(); \
    logger.SetLogLevel(kError); \
    char msg[kMsgBufSize]; \
    snprintf(msg, kMsgBufSize, logmsgformat, ##__VA_ARGS__); \
    logger.AddLog(std::string(msg)); \
} while(0); 

#endif