#ifndef _TIMER_H_
#define _TIMER_H_

#include <thread>
#include <memory>
#include <atomic>
#include <cmath>
#include <condition_variable>

#include "timer_task.h"
#include "timer_wheel.h"
#include "timer_heap.h"

namespace CTimer {
    // 定时器类
    template <typename T>
    class Timer {
    public:
        Timer(Tick_t tickInterval, const std::vector<int> &wheelSizes) : tickInterval_(tickInterval),
                                                                         wheelSizes_(wheelSizes),
                                                                         heap_(),
                                                                         quit_(false) {
            // 计算每个时间轮需要占据的二进制位数，并根据 wheelSizes 创建多个时间轮
            int shiftBits = 0;
            for (int i = 0; i < wheelSizes_.size(); i++) {
                shifts_.push_back(shiftBits);
                shiftBits += log2(wheelSizes_[i]);
                wheels_.push_back(TimerWheel<T>(shiftBits, wheelSizes_[i]));
            }
        }

        Timer() : tickInterval_(kMinInterval),
                  heap_(),
                  quit_(false) {
            wheelSizes_ = std::vector<int>({8, 6, 6, 6, 6});
            // 计算每个时间轮需要占据的二进制位数，并根据 wheelSizes 创建多个时间轮
            int shiftBits = 0;
            for (int i = 0; i < wheelSizes_.size(); i++) {
                shifts_.push_back(shiftBits);
                shiftBits += log2(wheelSizes_[i]);
                wheels_.push_back(TimerWheel<T>(shiftBits, wheelSizes_[i]));
            }
        }

        // 启动定时器线程
        void Start();

        // 停止定时器线程
        void Stop();

        // 添加定时任务
        void AddTimer(const T &task);

    private:
        // 定时器线程函数
        void TimerThreadFunc();

        Tick_t tickInterval_;                 // 时间粒度
        std::vector<int> wheelSizes_;         // 每个时间轮的大小
        std::vector<int> shifts_;             // 每个时间轮需要占据的二进制位数
        std::vector<TimerWheel<T>> wheels_;   // 多层时间轮
        TimerHeap<T> heap_;                   // 用于存储大于多层时间轮范围的定时器
        std::unique_ptr<std::thread> thread_; // 当前线程
        std::atomic<bool> quit_;              // 退出标记
        std::condition_variable cv_;
        mutable std::mutex mutex_;
    };

    template <typename T>
    void Timer<T>::Start() {
        quit_ = false;
        thread_.reset(new std::thread(&Timer::TimerThreadFunc, this));
    }

    template <typename T>
    void Timer<T>::Stop() {
        quit_ = true;
        cv_.notify_one();
        if (thread_ && thread_->joinable()) {
            thread_->join();
        }
    }

    template <typename T>
    void Timer<T>::AddTimer(const T &task) {
        Tick_t expire_time = task.ExpireTime();
        Tick_t now = Now();
        std::lock_guard<std::mutex> lock(mutex_);
        if (expire_time <= 0) {
            task.GetCallback();
        }
        // 将定时器按照时间粒度拆分成不同的层级
        for (int i = 0; i < wheelSizes_.size(); i++) {
            // 获取当前层级下的时间轮和对应槽位
            TimerWheel<T> &curWheel = wheels_[i];
            int slotIndex = (expire_time >> shifts_[i]) & curWheel.GetWheelMask();
            // 如果到达最后一层，则将定时器添加到槽位中
            if (i == wheelSizes_.size() - 1) {
                curWheel.AddTimer(task);
            } else {
                // 否则将定时器拆分成更精细的粒度，并将其添加到下一层的相应槽位中
                Tick_t interval = curWheel.GetSlots().size() << shifts_[i];
                expire_time = (expire_time >> shifts_[i]) << shifts_[i] + wheels_[i].GetShiftBits();
                T t(interval, expire_time, task.GetCallback());
                wheels_[i + 1].AddTimer(t);
            }
        }

        // 超过时间轮的精度范围，添加到最小堆中
        heap_.AddTimer(task);
        cv_.notify_one();
    }

    template <typename T>
    void Timer<T>::TimerThreadFunc() {
        while (!quit_) {
            Tick_t now = Now();
            Tick_t earliest_time = std::min<Tick_t>(wheels_[0].GetEarliestTime(), heap_.GetEarliestTime());
            if (earliest_time == kInvalidTime || earliest_time > now + wheelSizes_[0]) {
                // 所有定时任务都超过时间轮的精度范围，等待下一个定时任务
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait_for(lock, std::chrono::milliseconds(tickInterval_));
                continue;
            }
            if (earliest_time < now + tickInterval_) {
                // 最早的定时任务已经到期，直接执行
                std::vector<T> expired_tasks;
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    // expired_tasks =
                }
                for (auto task : expired_tasks) {
                    task.Run();
                }

                expired_tasks = wheels_[0].GetExpiredTimers(now);
                for (auto task : expired_tasks) {
                    heap_.AddTimer(task);
                }
            } else {
                // 等待最早的定时任务到期
                Tick_t timeout = earliest_time - now;
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait_for(lock, std::chrono::milliseconds(timeout));
            }
        }
    }

} // namespace CTimer

#endif /* _TIMER_H_ */
