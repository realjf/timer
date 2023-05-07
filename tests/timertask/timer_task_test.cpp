#include <gtest/gtest.h>
#include "timer_task.h"

TEST(testComp, testComp1) {
    auto task = CTimer::TimerTask();
    std::cout << "expire_time: " << task.ExpireTime() << std::endl;
    std::cout << "interval: " << task.Interval() << std::endl;

    task.Run();
}
