#pragma once

#include <cstddef>
#include <thread>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "SafeQueue.h"

class Task{
public:
    static void all_thread_stop();
    static void all_thread_start();
    static bool get_thread_state();

private:
    static std::atomic<bool> m_running;
};

class ThreadPool{
public:
    ThreadPool() : m_stop_flag(false){}
    ~ThreadPool(){
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_stop_flag = true;
        }
        m_cond.notify_all();
        for (std::thread &t : m_workers){
            if (t.joinable()){
                t.join();
            }
        }
    }
    //实例化
    static ThreadPool* Instance(){
        static ThreadPool thread_pool;

        return &thread_pool;
    }
    //初始化
    void initialize(size_t num_threads){

        m_num_threads = num_threads;
    }
    //线程开始
    void start(){
        for (size_t i; i < m_num_threads; i ++){
            m_workers.emplace_back([this](){
                while(true){
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(m_queue_mutex);
                        m_cond.wait(lock, [this]() {return m_stop_flag || !m_tasks.empty();});
                        
                        if (m_stop_flag && m_tasks.empty())
                            return;
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }
                    task();
                }
            });
        }
    }
    //添加任务
    void enqueue(std::function<void()> func){
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_tasks.push(std::move(func));
        }
        m_cond.notify_one();
    }
private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    
    std::mutex m_queue_mutex;
    std::condition_variable m_cond;
    std::atomic<bool> m_stop_flag;
    
    size_t m_num_threads;
};
