#ifndef _TIMER_HEAP_H_
#define _TIMER_HEAP_H_

#include <vector>
#include <algorithm>
#include <mutex>

#include "timer_task.h"

namespace CTimer {

    // 定时任务最小堆
    class TimerHeap {
    public:
        TimerHeap() {}

        // 插入定时任务
        void AddTimer(const TimerTask &task);

        // 删除定时任务
        void RemoveTimer(const TimerTask &task);

        // 获取最早的到期时间
        int64_t GetEarliestTime() const;

        // 获取所有到期的定时任务
        std::vector<TimerTask *> GetExpiredTasks();

    private:
        // 调整堆
        void SiftUp(int index);
        void SiftDown(int index);

        std::vector<TimerTask *> tasks_;
        mutable std::mutex mutex_;
    };


} // namespace CTimer

#endif /* _TIMER_HEAP_H_ */
