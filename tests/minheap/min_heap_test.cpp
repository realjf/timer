#include <gtest/gtest.h>
#include "../../src/timer_task.h"
#include "../../src/min_heap.h"

TEST(testComp, testComp1) {
    auto heap = CTimer::MinHeap<CTimer::TimerTask>();
    for (int i = 0; i < 9; i++) {
        auto g = i;
        auto task = CTimer::TimerTask(CTimer::AddSeconds(g), [g]() { std::cout << g << std::endl; });
        heap.push(task);

        if (i == 5) {
            heap.remove(task);
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));

    while (!heap.empty() && heap.top().ExpireTime() < CTimer::Now()) {
        auto task = heap.top();
        auto t1 = task.ExpireTime();
        std::cout << CTimer::TimeStampF(t1) << std::endl;
        task.Run();
        heap.remove(task);
    }
}
