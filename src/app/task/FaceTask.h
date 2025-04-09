#ifndef _FACE_TASK_H
#define _FACE_TASK_H

#include <thread>

class FaceTask{
public:
    FaceTask();
    ~FaceTask();
    void start();
    void wait();
    static FaceTask* Instance();
protected:
    void run();
private:
    std::thread m_thread;
};

#endif
