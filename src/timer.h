#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <vector>
#include <thread>
#include <iostream>
#include <condition_variable>
#include <math.h>
#include "timer_node.h"

namespace CTimer {
    const int Nano2Milliseconds = 1000000;

    template <typename TimerNodePtr, typename TimerNodeList, typename TimerClock>
    class Timer {
    public:
        typedef typename TimerClock::time_point TimePoint;

    public:
        explicit Timer(int precision = 10, int perWheelSlotNum = 60, int wheelNum = 24) : precision_(precision), slotNum_(perWheelSlotNum), wheelNum_(wheelNum), stop_(false), nextTimerId_(1) {
            wheels_.resize(wheelNum_);
            for (auto &wheel : wheels_) {
                wheel.resize(slotNum_);
            }
        }

        ~Timer() {
            Stop();
        }

        TimerId AddTimer(std::chrono::milliseconds expire, Callback callback) {
            auto now = TimerClock::now();
            auto expireTime = now + expire;
            // 构建定时器节点
            TimerNodePtr timerNode = std::make_shared<TimerNode<TimerClock>>(nextTimerId_++, expireTime, std::move(callback));

            std::lock_guard<std::mutex> lock(mutex_);
            AddTimerNode(timerNode);
            return timerNode->timerId;
        }

        TimerId AddTimer(std::chrono::seconds expire, Callback callback) {
            auto now = TimerClock::now();
            auto expireTime = now + expire;
            // 构建定时器节点
            TimerNodePtr timerNode = std::make_shared<TimerNode<TimerClock>>(nextTimerId_++, expireTime, std::move(callback));

            std::lock_guard<std::mutex> lock(mutex_);
            AddTimerNode(timerNode);
            return timerNode->timerId;
        }

        TimerId AddTimer(std::chrono::minutes expire, Callback callback) {
            auto now = TimerClock::now();
            auto expireTime = now + expire;
            // 构建定时器节点
            TimerNodePtr timerNode = std::make_shared<TimerNode<TimerClock>>(nextTimerId_++, expireTime, std::move(callback));

            std::lock_guard<std::mutex> lock(mutex_);
            AddTimerNode(timerNode);
            return timerNode->timerId;
        }

        TimerId AddTimer(std::chrono::hours expire, Callback callback) {
            auto now = TimerClock::now();
            auto expireTime = now + expire;
            // 构建定时器节点
            TimerNodePtr timerNode = std::make_shared<TimerNode<TimerClock>>(nextTimerId_++, expireTime, std::move(callback));

            std::lock_guard<std::mutex> lock(mutex_);
            AddTimerNode(timerNode);
            return timerNode->timerId;
        }

        TimerId AddTimer(TimePoint expireTime, Callback callback) {
            // 构建定时器节点
            TimerNodePtr timerNode = std::make_shared<TimerNode<TimerClock>>(nextTimerId_++, expireTime, std::move(callback));

            std::lock_guard<std::mutex> lock(mutex_);
            AddTimerNode(timerNode);
            return timerNode->timerId;
        }

        bool RemoveTimer(TimerId timerId) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto iter = timers_.find(timerId);
            if (iter == timers_.end()) {
                return false;
            }

            TimerNodePtr timerNode = iter->second;
            RemoveTimerNode(timerNode);

            return true;
        }

        void Start() {
            thread_ = std::thread(&Timer::ThreadFunc, this);
        }

        void Stop() {
            if (!thread_.joinable()) {
                return;
            }
            {
                std::lock_guard<std::mutex> lock(mutex_);
                stop_ = true;
                cv_.notify_one();
            }

            thread_.join();
        }

    private:
        void AddTimerNode(TimerNodePtr timerNode) {
            auto now = TimerClock::now();
            auto expireTime = timerNode->expireTime;

            // 如果已过期，直接运行回调函数
            if (expireTime <= now) {
                timerNode->callback(timerNode->timerId);
                timerNode->valid = false;
                return;
            }

            auto diff = expireTime - now;
            // 计算有多少个10ms，用于计算时间轮层级和槽位位置
            auto ticks = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() / precision_;
            if (ticks < slotNum_) {
                // 在第一层
                // 计算放置槽位位置
                // auto index = (now.time_since_epoch().count() / Nano2Milliseconds) % slotNum_;
                auto index = ticks % slotNum_;
                wheels_[0][index].push_back(timerNode);
                heap_.push(std::make_pair(expireTime, timerNode));
            } else {
                // 确认时间轮层级
                int wheelIndex = 1;
                ticks /= slotNum_;
                while (ticks >= slotNum_ && wheelIndex < wheelNum_) {
                    ticks /= slotNum_;
                    ++wheelIndex;
                }
                // 如果大于时间轮层级，则放在最后一层
                int index = 0;
                if (wheelIndex >= wheelNum_) {
                    wheelIndex = wheelNum_ - 1;
                    index = slotNum_ - 1; // 超出时间轮最长时间范围，放置在最后一个槽位
                } else {
                    index = ticks % slotNum_; // 计算槽位位置
                }

                wheels_[wheelIndex][index].push_back(timerNode);
            }

            timers_[timerNode->timerId] = timerNode;
            // std::cout << "add timer[" << timerNode->timerId << "] node successfully" << std::endl;
        }

        void RemoveTimerNode(TimerNodePtr timerNode) {
            timerNode->valid = false;
        }

        void ThreadFunc() {
            while (!stop_) {
                std::unique_lock<std::mutex> lock(mutex_);
                auto now = TimerClock::now();
                auto expireTime = now + std::chrono::milliseconds(precision_);
                // 清理第一层级时间轮的当前槽位无效定时器
                while (!heap_.empty() && heap_.top().first <= expireTime) {
                    auto timerNode = heap_.top().second;
                    heap_.pop();
                    // 检查有效性
                    if (!timerNode->valid) {
                        // 无效则舍弃
                        std::cout << "Timer " << timerNode->timerId << " is not valid, removed" << std::endl;
                        continue;
                    }
                    // 有效未到期添加回时间轮
                    AddTimerNode(timerNode);
                }

                // 获取当前槽位位置
                // auto index = (now.time_since_epoch().count() / Nano2Milliseconds) % slotNum_;
                auto index = current_slot_ % slotNum_;
                auto &slot = wheels_[0][index];
                std::vector<TimerNodePtr> expiredTimers; // 到期定时器数组
                expiredTimers.reserve(slot.size());

                // 处理第一层级时间轮到期的定时器，同时清理无效定时器
                for (auto iter = slot.begin(); iter != slot.end(); ++iter) {
                    auto &timerNode = *iter;

                    if (!timerNode->valid) {
                        iter = slot.erase(iter);
                        std::cout << "Timer " << timerNode->timerId << " is not valid, removed" << std::endl;
                        continue;
                    }

                    if (timerNode->expireTime <= now) {
                        // 定时器已到期
                        expiredTimers.push_back(timerNode);
                        iter = slot.erase(iter);
                    }
                }

                // 运行到期定时器回调函数，并移除定时器
                for (auto &timerNode : expiredTimers) {
                    timerNode->callback(timerNode->timerId);
                    RemoveTimerNode(timerNode);
                }

                // 时间轮开始转动
                for (int i = 1; i < wheelNum_; ++i) {
                    if (current_slot_ < slotNum_) {
                        break;
                    }
                    auto slotNum = static_cast<int>(pow(slotNum_, (i - 1)));
                    auto index = (current_slot_ / slotNum) % slotNum_;
                    auto &slot = wheels_[i][index];

                    std::vector<TimerNodePtr> expiredTimers;
                    expiredTimers.reserve(slot.size());

                    // 将下一个时间槽位定时器添加到第一层时间轮中
                    for (auto iter = slot.begin(); iter != slot.end();) {
                        auto &timerNode = *iter;

                        // 移除无效定时器
                        if (!timerNode->valid) {
                            iter = slot.erase(iter);
                            continue;
                        }

                        //
                        if (timerNode->expireTime <= now) {
                            expiredTimers.push_back(timerNode);
                            iter = slot.erase(iter);
                        } else {
                            ++iter;
                        }
                    }

                    for (auto &timerNode : expiredTimers) {
                        AddTimerNode(timerNode);
                    }
                }
                ++current_slot_;
                cv_.wait_until(lock, expireTime);
            }
        }

        const int precision_; // 精度，单位为毫秒
        const int slotNum_;   // 时间轮每层的槽数量
        const int wheelNum_;  // 时间轮层数
        int current_slot_;    // 当前槽位

        std::vector<std::vector<TimerNodeList>> wheels_;               // 时间轮数组：一维是时间轮层级数，二维是时间轮槽位数
        std::priority_queue<std::pair<TimePoint, TimerNodePtr>> heap_; // 最小堆，用于保存所有定时任务，同时用于获取最近快到期的定时器
        std::unordered_map<TimerId, TimerNodePtr> timers_;             // 计时器 ID 到计时器节点的映射
        std::mutex mutex_;                                             // 互斥锁
        std::condition_variable cv_;                                   // 条件变量
        std::thread thread_;                                           // 定时器线程
        bool stop_;                                                    // 是否停止定时器
        TimerId nextTimerId_;                                          // 下一个计时器的 ID
    };
} // namespace CTimer

#endif /* _TIMER_H_ */
