#ifndef _TIMER_HEAP_H_
#define _TIMER_HEAP_H_

#include <vector>
#include <algorithm>
#include <mutex>

#include "timer_task.h"

namespace CTimer {

    // 定时任务最小堆
    template <typename T>
    class TimerHeap {
    public:
        TimerHeap();

        // 插入定时任务
        void AddTimer(const T &task);

        // 删除定时任务
        void RemoveTimer(const T &task);

        // 获取最早的到期时间
        Tick_t GetEarliestTime() const;

        // 获取所有到期的定时任务
        std::vector<T> GetExpiredTimers();

    private:
        // 调整堆
        void SiftUp(int index);
        void SiftDown(int index);

        std::vector<T> tasks_;
        mutable std::mutex mutex_;
    };

#ifdef TIMER_HEAP_IMPLEMENTATION
    template <typename T>
    TimerHeap<T>::TimerHeap() : tasks_(0), mutex_() {
    }

    template <typename T>
    void TimerHeap<T>::AddTimer(const T &task) {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.push_back(task);
        SiftUp(tasks_.size() - 1);
    }

    template <typename T>
    void TimerHeap<T>::RemoveTimer(const T &task) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = tasks_.begin(); it != tasks_.end(); ++it) {
            if (*it == task) {
                delete it;
                tasks_.erase(it);
                break;
            }
        }
        std::make_heap(tasks_.begin(), tasks_.end(), [](const T &a, const T &b) {
            return a > b;
        });
    }

    template <typename T>
    Tick_t TimerHeap<T>::GetEarliestTime() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (tasks_.empty()) {
            return kInvalidTime;
        }
        return tasks_.front().ExpireTime();
    }

    template <typename T>
    std::vector<T> TimerHeap<T>::GetExpiredTimers() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<T> tasks;
        while (!tasks_.empty() && tasks_.front().ExpireTime() <= Now()) {
            tasks.push_back(tasks_.front());
            tasks_.erase(tasks_.begin());
        }

        std::make_heap(tasks_.begin(), tasks_.end(), [](const T &a, const T &b) {
            return a > b;
        });
        return tasks;
    }

    template <typename T>
    void TimerHeap<T>::SiftUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (*tasks_[index] < *tasks_[parent]) {
                std::swap(tasks_[index], tasks_[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    template <typename T>
    void TimerHeap<T>::SiftDown(int index) {
        while ((unsigned int)index * 2 + 1 < tasks_.size()) {
            int left = index * 2 + 1;
            size_t right = static_cast<size_t>(index * 2 + 2);
            int min_child = left;
            if (right < tasks_.size() && *tasks_[right] < *tasks_[left]) {
                min_child = right;
            }
            if (*tasks_[index] > *tasks_[min_child]) {
                std::swap(tasks_[index], tasks_[min_child]);
                index = min_child;
            } else {
                break;
            }
        }
    }
#endif // !TIMER_HEAP_IMPLEMENTATION

} // namespace CTimer

#endif /* _TIMER_HEAP_H_ */
