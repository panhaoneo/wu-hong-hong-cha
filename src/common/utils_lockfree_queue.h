//lock free queue. for one producer and one consumer queue
//reference:https://github.com/facebook/folly/blob/main/folly/ProducerConsumerQueue.h
#include <atomic>
#include <stdint.h>
#include <cstdlib>
#include <type_traits>
#include <utility>
#include <assert.h>
#include "utils_log.h"

template<typename T>
class LockFreeQueue {
public:
    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue &operator=(const LockFreeQueue&) = delete;

    LockFreeQueue() :read_index_(0), write_index_(0) {}
    ~LockFreeQueue() {
        if (records_) {
            if (!std::is_trivially_destructible<T>::value) {
                uint32_t read_index = read_index_;
                uint32_t end_index = write_index_;
                while (read_index != end_index) {
                    records_[read_index].~T();
                    if (++read_index == size_) {
                        read_index = 0;
                    }
                }
            }
            std::free(records_);
        }
    }

    bool init(uint32_t size) {
        assert(size >= 2);
        size_ = size;
        records_ = static_cast<T*>(std::malloc(sizeof(T) * size));
        if (!records_) {
            log_err(kEInit, "malloc failed.");
            return false;
        }
        return true;
    }

    template <class... Args>
    bool push(Args&&... record_args) {
        auto cur_index = write_index_.load(std::memory_order_relaxed);
        auto next = cur_index + 1;
        if (next == size_) {
            next = 0;
        }
        if (next != read_index_.load(std::memory_order_acquire)) {
            new (&records_[cur_index]) T(std::forward<Args>(record_args)...);
            write_index_.store(next, std::memory_order_release);
            return true;
        }
        //queue is full
        return false;
    }

    bool pop(T &record) {
        auto const cur_index = read_index_.load(std::memory_order_relaxed);
        if (cur_index == write_index_.load(std::memory_order_acquire)) {
            //queue is empty
            return false;
        }
        auto next = cur_index + 1;
        if (next == size_) {
            next = 0;
        }
        record = std::move(records_[cur_index]);
        records_[cur_index].~T();
        read_index_.store(next, std::memory_order_release);
        return true;
    }

    bool is_empty() const {
        return read_index_.load(std::memory_order_acquire) == write_index_.load(std::memory_order_acquire);
    }

    bool is_full() const {
        auto next = write_index_.load(std::memory_order_acquire) + 1;
        if (next == size_) {
            next = 0;
        }
        return next == read_index_.load(std::memory_order_acquire);
    }

    uint32_t size() const {
        auto write_index = write_index_.load(std::memory_order_acquire);
        auto read_index = read_index_.load(std::memory_order_acquire);
        if (write_index >= read_index) {
            return write_index - read_index;
        }
        return size_ - (read_index - write_index);
    }

private:
    uint32_t size_ = 0;
    T *records_ = nullptr;

    std::atomic<uint32_t> read_index_;
    std::atomic<uint32_t> write_index_;
};
