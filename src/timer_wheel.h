#ifndef _TIMER_WHEEL_H_
#define _TIMER_WHEEL_H_

#include <vector>
#include <queue>

#include "timer_task.h"
#include "min_heap.h"

namespace CTimer {

    // 时间轮类
    template <typename T>
    class TimerWheel {
    public:
        TimerWheel<T>();
        TimerWheel<T>(int bitShift, int wheelSize);
        TimerWheel<T>(TimerWheel<T> &&) = default;

        // 插入定时任务
        void AddTimer(const T &task);

        // 删除定时任务
        void RemoveTimer(const T &task);

        // 获取时间轮中最早的到期时间
        Tick_t GetEarliestTime() const;

        // 获取大于当前时间段所有定时任务
        std::vector<T> GetExpiredTimers(Tick_t expire_time);

        // 处理到期任务
        void Tick();

        int GetShiftBits() const;
        int GetWheelMask() const;
        std::vector<MinHeap<T> *> GetSlots() const;

    private:
        // 获取定时任务在时间轮中的位置
        int GetSlotIndex(Tick_t expire_time) const;

    private:
        int shiftBits_;                   // 时间轮每个槽位所占二进制位数
        int wheelMask_;                   // 时间轮大小掩码（用于取模运算）
        Tick_t curTick_;                  // 当前时间轮所在位置的 tick 值
        std::vector<MinHeap<T> *> slots_; // 每个槽位对应的定时器队列
    };

    template <typename T>
    TimerWheel<T>::TimerWheel() : shiftBits_(kBitShift), wheelMask_(kWheelSize - 1), curTick_(0) {
        // 初始化每个槽位为null
        for (unsigned int i = 0; i < kWheelSize; i++) {
            slots_.emplace_back(new MinHeap<T>());
        }
    }

    template <typename T>
    TimerWheel<T>::TimerWheel(int bitShift, int wheelSize) : shiftBits_(bitShift), wheelMask_(wheelSize - 1), curTick_(0) {
        // 初始化每个槽位为null
        for (int i = 0; i < wheelSize; i++) {
            slots_.emplace_back(new MinHeap<T>());
        }
    }

    template <typename T>
    void TimerWheel<T>::AddTimer(const T &task) {
        Tick_t expire_time = task.ExpireTime();
        // 计算应该放在哪个槽位
        int slotIndex = GetSlotIndex(expire_time);
        // 将定时器放入对应的槽位
        slots_[slotIndex]->push(task);
    }

    template <typename T>
    void TimerWheel<T>::RemoveTimer(const T &task) {
        Tick_t expire_time = task.ExpireTime();
        int slotIndex = GetSlotIndex(expire_time);
        // 从当前槽位中删除
        auto slot_tasks = slots_[slotIndex];

        slot_tasks->remove(task);
    }

    template <typename T>
    Tick_t TimerWheel<T>::GetEarliestTime() const {
        if (slots_[curTick_]->empty()) {
            return kInvalidTime;
        }
        return slots_[curTick_]->top().ExpireTime();
    }

    template <typename T>
    void TimerWheel<T>::Tick() {
        std::vector<T> tasks;
        // 取出当前槽位中的队列
        auto heap = slots_[curTick_];
        while (!heap->empty()) {
            auto task = heap->top();
            if (task.ExpireTime() <= Now()) {
                tasks.push_back(task);
                heap->pop();
            }
        }

        // 处理到期任务
        for (auto task : tasks) {
            task.Run();
            if (task.Interval() > 0) {
                AddTimer(task);
            }
        }

        // 更新时间轮的 tick 值
        curTick_ = (curTick_ + 1) & wheelMask_;
    }

    template <typename T>
    int TimerWheel<T>::GetShiftBits() const {
        return shiftBits_;
    }

    template <typename T>
    int TimerWheel<T>::GetWheelMask() const {
        return wheelMask_;
    }

    template <typename T>
    std::vector<MinHeap<T> *> TimerWheel<T>::GetSlots() const {
        return slots_;
    }

    template <typename T>
    int TimerWheel<T>::GetSlotIndex(Tick_t expire_time) const {
        return (expire_time >> shiftBits_) & wheelMask_;
    }

    template <typename T>
    std::vector<T> TimerWheel<T>::GetExpiredTimers(Tick_t expire_time) {
        std::vector<T> tasks;
        int slotIndex = GetSlotIndex(expire_time);
        auto slot_tasks = slots_[slotIndex];

        slot_tasks->traverse([&](auto a) { tasks.push_back(a);slot_tasks->remove(a); });
        return tasks;
    }

} // namespace CTimer

#endif /* _TIMER_WHEEL_H_ */
