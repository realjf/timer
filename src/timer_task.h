#ifndef _TIMER_TASK_H_
#define _TIMER_TASK_H_

#include <chrono>
#include <functional>

namespace CTimer {
    const int SLOT_BITS = 8;

    // 最小时间间隔 10ms
    const uint32_t kMinInterval = 10;
    // 最大时间间隔 24小时
    const uint32_t kMaxInterval = 24 * 60 * 60 * 1000;
    // 无效时间
    const int64_t kInvalidTime = -1;
    // 时间轮的槽数量，这里设置为8，表示时间轮的轮数为3，每一轮的槽数量为256
    const uint32_t kWheelSize = 1 << SLOT_BITS;

    const int SLOT_MASK = kWheelSize - 1;

    // 获取当前时间
    static int64_t
    Now() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

    // 定时任务类
    class TimerTask {
    public:
        using Callback = std::function<void()>;

        /**
         * @brief Construct a new Timer Task object
         *
         * @param cb 回调函数
         * @param interval 时间间隔
         */
        TimerTask(const Callback &cb, int64_t interval)
            : cb_(cb), interval_(interval), expire_time_(Now() + interval) {}

        TimerTask(int64_t expire_time, const Callback &cb)
            : cb_(cb), expire_time_(expire_time) {}

        // 获取到期时间
        int64_t ExpireTime() const { return expire_time_; }

        // 获取执行时间间隔
        int64_t Interval() const { return interval_; }

        // 执行回调函数
        void Run() { cb_(); }

        // 重新计算到期时间
        void Restart() { expire_time_ += interval_; }

        bool operator==(const TimerTask &other) const {
            return *(cb_.target<void()>()) == *(other.cb_.target<void()>()) && interval_ == other.interval_ && expire_time_ == other.expire_time_;
        }

        bool operator>(const TimerTask &other) const {
            return *(cb_.target<void()>()) == *(other.cb_.target<void()>()) && interval_ == other.interval_ && expire_time_ > other.expire_time_;
        }

        bool operator<(const TimerTask &other) const {
            return *(cb_.target<void()>()) == *(other.cb_.target<void()>()) && interval_ == other.interval_ && expire_time_ < other.expire_time_;
        }

    private:
        Callback cb_;         // 回调函数
        int64_t interval_;    // 任务执行间隔
        int64_t expire_time_; // 到期时间
    };
} // namespace CTimer

#endif /* _TIMER_TASK_H_ */
