#ifndef _TIMER_WHEEL_H_
#define _TIMER_WHEEL_H_

#include <vector>

#include "timer_task.h"

namespace CTimer {

    // 时间轮类
    template <typename T>
    class TimerWheel {
    public:
        TimerWheel() {}
        TimerWheel(int64_t interval, int64_t max_time)
            : interval_(interval), max_time_(max_time), current_slot_(0), slots_(max_time_ / interval_),
              sub_wheels_(max_time_ / interval_ / slots_.size()) {}

        // 插入定时任务
        void AddTimer(const TimerTask &task);

        // 删除定时任务
        void RemoveTimer(const TimerTask &task);

        // 获取时间轮中最早的到期时间
        int64_t GetEarliestTime() const;

        // 获取大于当前时间段所有定时任务
        std::vector<TimerTask *> GetExpiredTasks(int64_t expire_time);

        // 处理到期任务
        void Tick();

    private:
        // 获取定时任务在时间轮中的位置
        std::pair<int, int> GetPos(int64_t expire_time) const;

        // 获取子时间轮中的定时任务并添加到当前时间轮
        void Cascade(int wheel_num, int slot_num);

    private:
        int64_t interval_;                            // 时间轮间隔
        int64_t max_time_;                            // 时间轮最长时间
        int current_slot_;                            // 当前时间轮槽位
        std::vector<std::vector<TimerTask *>> slots_; // 时间轮槽
        std::vector<TimerWheel<T>> sub_wheels_;       // 子时间轮
    };

    template <typename T>
    void TimerWheel<T>::AddTimer(const TimerTask &task) {
        int64_t expire_time = task.ExpireTime();
        std::pair<int, int> pos = GetPos(expire_time);

        // 添加到当前时间轮
        if (pos.first == 0) {
            slots_[pos.second].push_back(new T(task));
        }
        // 添加到子时间轮
        else {
            sub_wheels_[pos.first - 1].AddTimer(task);
        }
    }

    template <typename T>
    void TimerWheel<T>::RemoveTimer(const TimerTask &task) {
        int64_t expire_time = task.ExpireTime();
        std::pair<int, int> pos = GetPos(expire_time);
        // 从当前时间轮中删除
        if (pos.first == 0) {
            auto &tasks = slots_[pos.second];
            for (auto it = tasks.begin(); it != tasks.end(); ++it) {
                if (*it == &task) {
                    tasks.erase(it);
                    break;
                }
            }
        }
        // 从子时间轮中删除
        else {
            sub_wheels_[pos.first - 1].RemoveTimer(task);
        }
    }

    template <typename T>
    int64_t TimerWheel<T>::GetEarliestTime() const {
        if (slots_[current_slot_].empty()) {
            return max_time_;
        }
        return slots_[current_slot_].front()->ExpireTime();
    }

    template <typename T>
    void TimerWheel<T>::Tick() {
        // 获取当前时间轮中到期的任务并添加到队列
        std::vector<TimerTask *> tasks;
        for (auto &task : slots_[current_slot_]) {
            if (task->ExpireTime() <= Now()) {
                tasks.push_back(task);
            }
        }
        // 从当前时间轮中删除到期任务
        for (auto task : tasks) {
            RemoveTimer(*task);
        }

        // 处理到期任务
        for (auto task : tasks) {
            task->Run();
            task->Restart();
            AddTimer(*task);
        }

        // 移动当前时间轮指针
        current_slot_ = (current_slot_ + 1) % slots_.size();

        // 处理子时间轮中的任务
        if (current_slot_ == 0) {
            for (int i = 0; i < sub_wheels_.size(); ++i) {
                if (sub_wheels_[i].GetEarliestTime() <= (i + 1) * interval_) {
                    sub_wheels_[i].Tick();
                }
            }
        }
    }

    template <typename T>
    std::pair<int, int> TimerWheel<T>::GetPos(int64_t expire_time) const {
        expire_time = std::max(expire_time, Now());
        int slot_num = (current_slot_ + expire_time / interval_) % slots_.size();
        int wheel_num = (expire_time / interval_ / slots_.size());

        if (wheel_num >= sub_wheels_.size()) {
            return {0, slot_num};
        } else {
            return {wheel_num + 1, slot_num};
        }
    }

    template <typename T>
    std::vector<TimerTask *> TimerWheel<T>::GetExpiredTasks(int64_t expire_time) {
        std::vector<TimerTask *> tasks;
        std::pair<int, int> pos = GetPos(expire_time);

        // 从当前时间轮中取出
        if (pos.first == 0) {
            auto &slot_tasks = slots_[pos.second];
            for (auto it = slot_tasks.begin(); it != slot_tasks.end(); ++it) {
                tasks.push_back(*it);
                slot_tasks.erase(it);
            }
        } // 从子时间轮中取出
        else {
            tasks = sub_wheels_[pos.first - 1].GetExpiredTasks(expire_time);
        }

        return tasks;
    }

    template <typename T>
    void TimerWheel<T>::Cascade(int wheel_num, int slot_num) {
        const auto &slots = slots_;
        if (wheel_num > 0) {
            slots = sub_wheels_[wheel_num].slots_;
        }
        auto expired_tasks = slots[slot_num];
        for (const auto &task : expired_tasks) {
            if (task->Interval() > 0) {
                // 添加到父时间轮的对应槽中
                sub_wheels_[wheel_num - 1].AddTimer(task, (task->ExpireTime() >> (SLOT_BITS * (wheel_num - 1))) & SLOT_MASK);
            } else {
                // 添加到当前时间轮的对应槽中
                AddTimer(task, slot_num);
            }
        }
    }

} // namespace CTimer

#endif /* _TIMER_WHEEL_H_ */
