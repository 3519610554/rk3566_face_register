#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <iostream>

void worker(std::atomic<bool>& done) {
    std::cout << "Worker thread running\n";
    done.store(true);
}

TEST(ThreadTest, WorkerThreadTest){

    //线程完成标志位
    std::atomic<bool> done(false);

    //创建并启动线程
    std::thread t(worker, std::ref(done));
    t.join();

    EXPECT_TRUE(done.load());
}
