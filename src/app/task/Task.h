#pragma once

#include <atomic>

class Task{
public:
    static void all_thread_stop();
    static void all_thread_start();
    static bool get_thread_state();

private:
    static std::atomic<bool> m_running;
};
