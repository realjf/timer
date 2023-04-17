#include "timer.h"

namespace CTimer {
    void Timer::Start() {
        quit_ = false;
        thread_.reset(new std::thread(&Timer::TimerThreadFunc, this));
    }

    void Timer::Stop() {
        quit_ = true;
        cv_.notify_one();
        if (thread_ && thread_->joinable()) {
            thread_->join();
        }
    }

    void Timer::AddTimer(const TimerTask &task) {
        int64_t expire_time = task.ExpireTime();
        int64_t now = Now();
        if (expire_time < now + kMinInterval || expire_time >= now + kMaxInterval) {
            // 不合法的定时任务，直接返回
            return;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        if (expire_time < now + kWheelSize) {
            // 在时间轮的精度范围内，直接添加到时间轮中
            wheel_.AddTimer(task);
        } else {
            // 超过时间轮的精度范围，添加到最小堆中
            heap_.AddTimer(task);
        }
        cv_.notify_one();
    }

    void Timer::TimerThreadFunc() {
        while (!quit_) {
            int64_t now = Now();
            int64_t earliest_time = std::min(wheel_.GetEarliestTime(), heap_.GetEarliestTime());
            if (earliest_time == kInvalidTime || earliest_time > now + kWheelSize) {
                // 所有定时任务都超过时间轮的精度范围，等待下一个定时任务
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait_for(lock, std::chrono::milliseconds(kMinInterval));
                continue;
            }
            if (earliest_time < now + kMinInterval) {
                // 最早的定时任务已经到期，直接执行
                std::vector<TimerTask *> expired_tasks;
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    expired_tasks = heap_.GetExpiredTasks();
                }
                for (auto task : expired_tasks) {
                    task->Run();
                    delete task;
                }
                expired_tasks = wheel_.GetExpiredTasks(now);
                for (auto task : expired_tasks) {
                    heap_.AddTimer(*task);
                }
            } else {
                // 等待最早的定时任务到期
                int64_t timeout = earliest_time - now;
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait_for(lock, std::chrono::milliseconds(timeout));
            }
        }
    }
}
