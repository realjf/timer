#ifndef _TIMER_BASE_H_
#define _TIMER_BASE_H_

#include <chrono>
#include <functional>
#include <iostream>

namespace CTimer {

    // 即tick从00000001~11111111，即从1到255,经历256个tick
    const int kSlotBits = 8;

    // 最小时间间隔 10ms，即 1s=100tick, 1tick=10ms
    const uint32_t kMinInterval = 10;
    // 最大时间间隔 24小时
    const uint32_t kMaxInterval = 24 * 60 * 60 * 1000;
    // 无效时间
    const int64_t kInvalidTime = -1;
    // 时间轮的槽数量，这里设置为8，表示时间轮的轮数为3，每一轮的槽数量为256
    const uint32_t kWheelSlotSize = 1 << kSlotBits;

    const int kSlotMask = kWheelSlotSize - 1;

    const uint32_t kBitShift = 8;
    const uint32_t kWheelSize = 1 << kBitShift;

    typedef uint64_t Tick_t;

    typedef std::chrono::system_clock TimerClock;

    typedef std::function<void()> Callback;

    typedef TimerClock::time_point TimePoint;

    // 获取当前时间
    static Tick_t
    Now() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   TimerClock::now().time_since_epoch())
            .count();
    }

    // 格式化时间点
    static std::string TimePointF(TimePoint tp) {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        char buf[20];
        strftime(buf, 20, "%Y-%m-%d %H:%M:%S", localtime(&t));
        return std::string(buf);
    }

    // 格式化时间戳
    static std::string TimeStampF(int64_t timestamp) {
        auto ms = std::chrono::milliseconds(timestamp);
        auto tp = std::chrono::system_clock::time_point(ms);
        return TimePointF(tp);
    }

    // 格式化时间
    static std::string TimeF(std::time_t t) {
        char buf[20];
        strftime(buf, 20, "%Y-%m-%d %H:%M:%S", localtime(&t));
        return std::string(buf);
    }

    static Tick_t AddMilliSeconds(int ms) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(TimerClock::now().time_since_epoch()).count() + ms;
    }

    static Tick_t AddSeconds(int s) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(TimerClock::now().time_since_epoch()).count() + s * 1000;
    }

    static Tick_t AddMinutes(int min) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(TimerClock::now().time_since_epoch()).count() + min * 60 * 1000;
    }

    static Tick_t AddHours(int hour) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(TimerClock::now().time_since_epoch()).count() + hour * 3600 * 1000;
    }

    class TimerBase {
    public:
        TimerBase() : cb_([]() { std::cout << "timer called!" << std::endl; }), interval_(kMinInterval), expire_time_(Now() + interval_) {
        }
        /**
         * @brief Construct a new Timer Task object
         *
         * @param cb 回调函数
         * @param interval 时间间隔
         */
        TimerBase(const Callback &cb, Tick_t interval)
            : cb_(cb), interval_(interval), expire_time_(Now() + interval) {}

        TimerBase(Tick_t expire_time, const Callback &cb)
            : cb_(cb), interval_(0), expire_time_(expire_time) {}

        TimerBase(Tick_t interval, Tick_t expire_time, const Callback &cb)
            : cb_(cb), interval_(interval), expire_time_(expire_time) {}

        // 获取到期时间
        virtual Tick_t ExpireTime() const = 0;

        // 获取执行时间间隔
        virtual Tick_t Interval() const = 0;

        virtual Callback GetCallback() const = 0;

        virtual ~TimerBase() {}

        // 执行回调函数
        virtual void Run() = 0;

        bool operator==(const TimerBase &other) const {
            return *(cb_.target<void()>()) == *(other.cb_.target<void()>()) && interval_ == other.interval_ && expire_time_ == other.expire_time_;
        }

        bool operator>(const TimerBase &other) const {
            return *(cb_.target<void()>()) == *(other.cb_.target<void()>()) && interval_ == other.interval_ && expire_time_ > other.expire_time_;
        }

        bool operator<(const TimerBase &other) const {
            return *(cb_.target<void()>()) == *(other.cb_.target<void()>()) && interval_ == other.interval_ && expire_time_ < other.expire_time_;
        }

        bool operator>=(const TimerBase &other) const {
            return *(cb_.target<void()>()) == *(other.cb_.target<void()>()) && interval_ == other.interval_ && expire_time_ >= other.expire_time_;
        }

        bool operator<=(const TimerBase &other) const {
            return *(cb_.target<void()>()) == *(other.cb_.target<void()>()) && interval_ == other.interval_ && expire_time_ <= other.expire_time_;
        }

    protected:
        Callback cb_;        // 回调函数
        Tick_t interval_;    // 任务执行间隔
        Tick_t expire_time_; // 到期时间
    };
} // namespace CTimer

#endif /* _TIMER_BASE_H_ */
