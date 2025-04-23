#include <iostream>
#include "Task.h"
#include "CameraUvc.h"
#include "TrainModel.h"
#include "FaceDetection.h"
#include "WebConnect.h"
#include "FaceTask.h"
// #include "ThreadPool.h"

int main() {
    
    ThreadPool::Instance()->initialize(7);
    Task::all_thread_start();
    CameraUvc::Instance()->initialize();
    WebConnect::Instance()->initialize();
    TrainModel::Instance()->initialize();
    FaceDetection::Instance()->initialize();
    FaceTask::Instance()->initialize();

    ThreadPool::Instance()->start();
    ThreadPool::Instance()->wait();

    return 0;
}

