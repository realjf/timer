#ifndef _TIMER_NODE_H_
#define _TIMER_NODE_H_

#include <chrono>
#include <memory>
#include <vector>
#include <functional>

namespace CTimer {
    using TimerId = uint64_t;
    using Callback = std::function<void(TimerId)>;
    using RelativeClock = std::chrono::steady_clock;
    using AbsoluteClock = std::chrono::system_clock;

    template <typename TimerClock>
    class TimerNode {
    public:
        typedef typename TimerClock::time_point TimePoint;

    public:
        TimerId timerId;
        TimePoint expireTime;
        Callback callback;
        bool valid;

        bool operator()(const TimerNode &node1, const TimerNode &node2) {
            return node1.expireTime > node2.expireTime;
        }

        TimerNode(TimerId id, TimePoint time, Callback cb)
            : timerId(id), expireTime(time), callback(std::move(cb)), valid(true) {}
    };

    using RelativeTimerNodePtr = std::shared_ptr<TimerNode<RelativeClock>>;
    using AbsoluteTimerNodePtr = std::shared_ptr<TimerNode<AbsoluteClock>>;

    using RelativeTimerNodeList = std::vector<RelativeTimerNodePtr>;
    using AbsoluteTimerNodeList = std::vector<AbsoluteTimerNodePtr>;

} // namespace CTimer

#endif /* _TIMER_NODE_H_ */
