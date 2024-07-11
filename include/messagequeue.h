#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

// 消息队列，线程安全
template<typename T>
class MessageQueue {
public:
    void Push(const T& data) {
        std::lock_guard<std::mutex> lock(mutex_);
        msg_queue_.push(data);
        cond_variable_.notify_one();
    }

    T Pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (msg_queue_.empty()) {
            cond_variable_.wait(lock);
        }
        T data = msg_queue_.front();
        msg_queue_.pop();
        return data;
    }

private:
    std::queue<T> msg_queue_;
    std::mutex mutex_;
    std::condition_variable cond_variable_;
};

#endif