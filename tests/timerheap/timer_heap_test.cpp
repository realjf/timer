#include <gtest/gtest.h>
#define TIMER_HEAP_IMPLEMENTATION
#include "timer_heap.h"
#include <thread>

TEST(testComp, testComp1) {
    auto heap = CTimer::TimerHeap<CTimer::TimerTask>();
    for (int i = 0; i < 9; i++) {
        auto g = i;
        auto task = CTimer::TimerTask(CTimer::AddSeconds(g), [g]() { std::cout << g << std::endl; });
        heap.AddTimer(task);

        if (i == 5) {
            heap.RemoveTimer(task);
        }
    }

    std::cout << "earliest: " << CTimer::TimeStampF(heap.GetEarliestTime()) << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(10));

    for (auto task : heap.GetExpiredTimers()) {
        auto t1 = task.ExpireTime();
        std::cout << CTimer::TimeStampF(t1) << std::endl;
        task.Run();
        heap.RemoveTimer(task);
    }
}
