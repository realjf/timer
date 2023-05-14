#include <gtest/gtest.h>
#include "timer_wheel.h"
#include <thread>

TEST(testComp, testComp1) {
    auto tw = CTimer::TimerWheel<CTimer::TimerTask>();
    for (int i = 0; i < 9; i++) {
        auto g = i;
        auto task = CTimer::TimerTask(CTimer::AddSeconds(g), [g]() { std::cout << g << std::endl; });
        tw.AddTimer(task);

        if (i == 5) {
            tw.RemoveTimer(task);
        }
    }

    for (int i = 0; i < 10000; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        tw.Tick();
    }

    std::cout << "earliest: " << CTimer::TimeStampF(tw.GetEarliestTime()) << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(10));

    for (auto task : tw.GetExpiredTimers(CTimer::Now())) {
        auto t1 = task.ExpireTime();
        std::cout << CTimer::TimeStampF(t1) << std::endl;
        task.Run();
        tw.RemoveTimer(task);
    }
}
