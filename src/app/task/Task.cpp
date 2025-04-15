#include "Task.h"

std::atomic<bool> Task::m_running{true};

void Task::all_thread_stop() {
    m_running.store(false);
}

void Task::all_thread_start() {
    m_running.store(true);
}

bool Task::get_thread_state() {
    return m_running.load();
}
