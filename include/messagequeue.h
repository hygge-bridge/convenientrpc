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
        queue_.push(data);
        cond_variable_.notify_one();
    }

    T Pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (queue_.empty()) {
            cond_variable_.wait(lock);
        }
        T data = queue_.front();
        queue_.pop();
        return data;
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_variable_;
};

#endif