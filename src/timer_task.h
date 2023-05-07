#ifndef _TIMER_TASK_H_
#define _TIMER_TASK_H_

#include "timer_base.h"

/**
 * @brief 时间轮算法
 * 32位tick可以分为8 6 6 6 6,共5个度量，即 111111 111111 111111 111111 11111111，
 * 64位tick可以分为
 *
 */

namespace CTimer {
    // 定时任务类
    class TimerTask : public TimerBase {
    public:
        TimerTask() : TimerBase() {
        }

        /**
         * @brief Construct a new Timer Task object
         *
         * @param cb 回调函数
         * @param interval 时间间隔
         */
        TimerTask(const Callback &cb, Tick_t interval)
            : TimerBase(cb, interval) {}

        TimerTask(Tick_t expire_time, const Callback &cb)
            : TimerBase(expire_time, cb) {}

        TimerTask(Tick_t interval, Tick_t expire_time, const Callback &cb)
            : TimerBase(interval, expire_time, cb) {}

        // 获取到期时间
        virtual Tick_t ExpireTime() const { return expire_time_; }

        // 获取执行时间间隔
        virtual Tick_t Interval() const { return interval_; }

        virtual Callback GetCallback() const {
            return cb_;
        }

        // 执行回调函数
        virtual void Run() { cb_(); }

        // bool operator==(const TimerTask &other) const {
        //     return *(cb_.target<void()>()) == *(other.cb_.target<void()>()) && interval_ == other.interval_ && expire_time_ == other.expire_time_;
        // }

        // bool operator>(const TimerTask &other) const {
        //     return *(cb_.target<void()>()) == *(other.cb_.target<void()>()) && interval_ == other.interval_ && expire_time_ > other.expire_time_;
        // }

        // bool operator<(const TimerTask &other) const {
        //     return *(cb_.target<void()>()) == *(other.cb_.target<void()>()) && interval_ == other.interval_ && expire_time_ < other.expire_time_;
        // }
    };
} // namespace CTimer

#endif /* _TIMER_TASK_H_ */
