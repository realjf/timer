#include <gtest/gtest.h>
#include "timer.h"
#include <thread>

TEST(testComp, testComp1) {
    auto timer = CTimer::Timer<CTimer::TimerTask>();

    timer.Start();

    for (int i = 0; i < 9; i++) {
        auto g = i;
        auto task = CTimer::TimerTask(CTimer::AddSeconds(g), [g]() { std::cout << g << std::endl; });
        timer.AddTimer(task);
    }
}
