#ifndef _THREAD_STATE_H
#define _THREAD_STATE_H

#include <atomic>
#include <condition_variable>
#include <mutex>

class ThreadState{
public:
    //等待 m_state 为false
    void wait_for_state_false();
    //等待 m_state 为true
    void wait_for_state_true();
    //设置状态和通知
    void set_state_and_notify(bool state);
    //获取当前状态
    bool get_state();
private:
    std::atomic<bool> m_state;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

#endif
