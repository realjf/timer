#include "timer_heap.h"

namespace CTimer {
    void TimerHeap::AddTimer(const TimerTask &task) {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.push_back(new TimerTask(task));
        SiftUp(tasks_.size() - 1);
    }

    void TimerHeap::RemoveTimer(const TimerTask &task) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = tasks_.begin(); it != tasks_.end(); ++it) {
            if (**it == task) {
                delete *it;
                tasks_.erase(it);
                break;
            }
        }
        std::make_heap(tasks_.begin(), tasks_.end(), [](const TimerTask *a, const TimerTask *b) {
            return *a > *b;
        });
    }

    int64_t TimerHeap::GetEarliestTime() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (tasks_.empty()) {
            return kInvalidTime;
        }
        return tasks_.front()->ExpireTime();
    }

    std::vector<TimerTask *> TimerHeap::GetExpiredTasks() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<TimerTask *> tasks;
        while (!tasks_.empty() && tasks_.front()->ExpireTime() <= Now()) {
            tasks.push_back(tasks_.front());
            tasks_.erase(tasks_.begin());
        }
        std::make_heap(tasks_.begin(), tasks_.end(), [](const TimerTask *a, const TimerTask *b) {
            return *a > *b;
        });
        return tasks;
    }

    void TimerHeap::SiftUp(int index) {
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

    void TimerHeap::SiftDown(int index) {
        while ((unsigned int)index * 2 + 1 < tasks_.size()) {
            int left = index * 2 + 1;
            int right = index * 2 + 2;
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
} // namespace CTimer
