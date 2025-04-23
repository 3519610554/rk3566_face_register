#ifndef _FACE_TASK_H
#define _FACE_TASK_H

#include <thread>
#include <vector>

class FaceTask{
public:
    FaceTask();
    ~FaceTask();
    //实例化
    static FaceTask* Instance();
    //初始化
    void initialize();
protected:
    void run();
private:
};

#endif
