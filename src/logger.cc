#include "logger.h"

Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}
   
void Logger::AddLog(const std::string& msg) {
    msg_queue_.Push(msg);
}

static const int kBufSize = 128;
// 创建文件名为时间+消息的日志文件，如
// 2024-7-11-log.txt
// 然后将日志水平、具体时间信息和日志消息写入到文件
Logger::Logger() {
    std::thread write_log_task([&]() {
        for (;;) {
            // 以a+的模式创建或追加文件
            std::time_t now = std::time(nullptr);
            std::tm* local_time = std::localtime(&now);
            char file_name[kBufSize];
            std::strftime(file_name, kBufSize, "%Y-%m-%d-log.txt", local_time);
            std::unique_ptr<FILE, std::function<void(FILE*)>> fp(
                fopen(file_name, "a+"),
                [](FILE* p) { fclose(p); }
            );
            // 写入信息到文件
            std::string msg = msg_queue_.Pop();
            char concrete_time[kBufSize];
            std::strftime(concrete_time, kBufSize, "-%H:%M:%S ---> ", local_time);
            std::string msg_level = msg_level_ == kNormal ? "NORMAL" : "ERROR";
            msg_level += concrete_time;
            msg.insert(0, msg_level);
            msg.append("\n");
            fputs(msg.c_str(), fp.get());
        }
    });
    write_log_task.detach();
}