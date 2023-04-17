#include "spinlock.h"
#include <iostream>

SpinLock lock;
int count = 0;

void increase_count() {
    lock.lock();
    ++count;
    std::cout << "Thread " << std::this_thread::get_id() << " increased count to " << count << std::endl;
    lock.unlock();
}

int main() {
    std::thread t1(increase_count);
    std::thread t2(increase_count);
    std::thread t3(increase_count);
    std::thread t4(increase_count);
    std::thread t5(increase_count);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    std::cout << "Final count value: " << count << std::endl;
    return 0;
}
