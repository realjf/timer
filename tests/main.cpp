#include "timer.h"

int main() {
    CTimer::Timer<CTimer::AbsoluteTimerNodePtr, CTimer::AbsoluteTimerNodeList, CTimer::AbsoluteClock> timer;
    timer.Start();
    std::vector<CTimer::TimerId> timerIds;

    // 添加一万个定时器
    for (int i = 0; i < 10; ++i) {
        auto timerId = timer.AddTimer(std::chrono::milliseconds(i * 1000), [](CTimer::TimerId id) {
            std::cout << "timer " << id << " callback" << std::endl;
        });

        timerIds.push_back(timerId);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    // for (auto timerId : timerIds) {
    //     timer.RemoveTimer(timerId);
    // }

    std::this_thread::sleep_for(std::chrono::milliseconds(15000));

    timer.Stop();

    return 0;
}
