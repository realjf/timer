#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <atomic>
#include <thread>
#include <stdexcept>

class SpinLock {
public:
    // ATOMIC_FLAG_INIT 标识未加锁状态
    SpinLock() : m_lock(ATOMIC_FLAG_INIT), m_owner(0), m_counter(0) {
    }
    ~SpinLock() {
    }

    void lock() {
        const auto current_thread_id = std::this_thread::get_id();
        if (m_owner == current_thread_id) {
            ++m_counter;
            return;
        }
        while (m_lock.test_and_set(std::memory_order_acquire)) {
        }
        m_owner = current_thread_id;
        m_counter = 1;
    }

    void unlock() {
        if (m_owner != std::this_thread::get_id()) {
            throw std::logic_error("Unlocking thread does not own this lock!");
        }
        --m_counter;
        if (m_counter == 0) {
            m_owner = std::thread::id();
            m_lock.clear(std::memory_order_release);
        }
    }

private:
    std::atomic_flag m_lock; // 锁状态
    std::thread::id m_owner; // 线程id
    int m_counter;           // 锁计数
};

#endif /* _SPINLOCK_H_ */
