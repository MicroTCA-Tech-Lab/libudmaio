//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

/// Helper class to implement a blocking FIFO between threads
template <typename T> class ConcurrentQueue {
  public:
    /// @brief Pop an element from the queue, block if none available
    /// @param timeout timeout in ms, default 0 = no timeout, return empty T if elapsed
    /// @return Element popped from the queue
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

    /// @brief Push an element to the queue
    /// @param item Element to push to the queue
    void push(T item) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_queue.size() >= MAX_ELEMS) {
            lock.unlock();
            throw std::runtime_error("libudmaio: ConcurrentQueue full");
        }
        _queue.push(std::move(item));
        lock.unlock();
        _cond.notify_one();
    }

    /// @brief Abort any blocking consumers
    void abort() {
        _abort = true;
        _cond.notify_one();
    }

    ConcurrentQueue() = default;
    ConcurrentQueue(const ConcurrentQueue &) = delete;
    ConcurrentQueue &operator=(const ConcurrentQueue &) = delete;

  private:
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _cond;
    constexpr static size_t MAX_ELEMS = 64;
    bool _abort = false;
};
