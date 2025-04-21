#include <iostream>
#include "Task.h"
#include "CameraUvc.h"
#include "TrainModel.h"
#include "FaceDetection.h"
#include "WebConnect.h"
#include "FaceTask.h"

int main() {
    
    Task::all_thread_start();
    CameraUvc::Instance()->initialize();
    WebConnect::Instance()->initialize();
    TrainModel::Instance()->initialize();
    FaceDetection::Instance()->initialize();
    FaceTask::Instance()->initialize();

    WebConnect::Instance()->start();
    TrainModel::Instance()->start();
    FaceDetection::Instance()->start();
    FaceTask::Instance()->start();


    FaceTask::Instance()->wait();

    return 0;
}

