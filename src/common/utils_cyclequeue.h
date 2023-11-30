#pragma once

#include <stdint.h>
#include <cstdlib>

#include "utils_log.h"
//循环队列，单线程安全
template<class TYPE>
class CycleQueue {
    static_assert(sizeof(TYPE) % 8 == 0, "sizeof(TYPE) must multiple of 8.");
public:
    CycleQueue(const uint64_t cell_size) : cell_max_size_(cell_size) { }
    CycleQueue(const CycleQueue&) = delete;
    CycleQueue& operator=(const CycleQueue&) = delete;

    ~CycleQueue() { unInit(); }

public:
    bool init() {
        addr_ = (TYPE*)aligned_alloc(sizeof(TYPE), sizeof(TYPE) * cell_max_size_);
        if (!addr_) {
            log_err(kENew, "aligned_alloc failed. size[%ld]", sizeof(TYPE) * cell_max_size_);
            return false;
        }

        memset(addr_, 0, sizeof(TYPE) * cell_max_size_);
        return true;
    }

    TYPE* alloc() { return &(addr_[(idx_++)%cell_max_size_]); }
    void reset() { idx_ = 0; }

private:
    void unInit() {
        if (addr_) {
            free(addr_);
            addr_ = nullptr;
        }
        idx_ = 0;
    }

private:
    const uint64_t cell_max_size_ = 0; // 队列大小
    uint64_t idx_ = 0; // 记录当前使用的位置
    TYPE   *addr_ = nullptr;
};