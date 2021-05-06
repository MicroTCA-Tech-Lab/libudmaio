#pragma once

#include <chrono>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class ConcurrentQueue {
public:
    T pop(std::chrono::milliseconds timeout = std::chrono::milliseconds{0}) {
        std::unique_lock<std::mutex> lock{_mutex};
        while (_queue.empty()) {
            if (_abort) {
                return {};
            }
            if (timeout.count()) {
                if (_cond.wait_for(lock, timeout) == std::cv_status::timeout) {
                    return {};
                }
            } else {
                _cond.wait(lock);
            }
        }
        auto val = std::move(_queue.front());
        _queue.pop();
        lock.unlock();
        _cond.notify_one();
        return val;
    }

    void push(T item) {
        std::unique_lock<std::mutex> lock(_mutex);
        while (_queue.size() >= MAX_ELEMS) {
            if (_abort) {
                return;
            }
            _cond.wait(lock);
        }
        _queue.push(std::move(item));
        lock.unlock();
        _cond.notify_one();
    }

    void abort() {
        _abort = true;
        _cond.notify_one();
    }

    ConcurrentQueue() = default;
    ConcurrentQueue(const ConcurrentQueue&) = delete;
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

private:
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _cond;
    constexpr static size_t MAX_ELEMS = 1000;
    bool _abort = false;
};
