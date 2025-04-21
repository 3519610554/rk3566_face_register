#include "ThreadState.h"

void ThreadState::wait_for_state_false(){

    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [this]() { return !m_state.load(); });
}

void ThreadState::wait_for_state_true(){

    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [this]() { return m_state.load(); });
}

void ThreadState::set_state_and_notify(bool state){
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_state.store(state);
    }
    m_cv.notify_all();
}

bool ThreadState::get_state(){

    return m_state.load();
}
