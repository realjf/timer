#ifndef _TIMER_H_
#define _TIMER_H_

#include <thread>
#include <memory>
#include <atomic>
#include <condition_variable>

#include "timer_task.h"
#include "timer_wheel.h"
#include "timer_heap.h"

namespace CTimer {
    // 定时器类
    class Timer {
    public:
        Timer() : quit_(false) {}

        // 启动定时器线程
        void Start();

        // 停止定时器线程
        void Stop();

        // 添加定时任务
        void AddTimer(const TimerTask &task);

    private:
        // 定时器线程函数
        void TimerThreadFunc();

        TimerWheel<TimerTask> wheel_;
        TimerHeap heap_;
        std::unique_ptr<std::thread> thread_;
        std::atomic<bool> quit_;
        std::condition_variable cv_;
        mutable std::mutex mutex_;
    };


} // namespace CTimer

#endif /* _TIMER_H_ */
