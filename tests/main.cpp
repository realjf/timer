#include <iostream>
#include <chrono>
#include <thread>

#include "timer.h"

int main() {
    CTimer::Timer timer;
    timer.Start();
    for (int i = 0; i < 10000; ++i) {
        auto randMs = std::chrono::milliseconds(rand() % 86400000);
        int64_t expire_time = CTimer::Now() + randMs.count();
        CTimer::TimerTask task(expire_time, [i]() {
            std::cout << "Task " << i << " executed" << std::endl;
        });
        timer.AddTimer(task);
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
    timer.Stop();
    return 0;
}
