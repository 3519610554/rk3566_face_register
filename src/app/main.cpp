#include <iostream>
#include "Task.h"
#include "CameraUvc.h"
#include "FaceDetection.h"
#include "WebConnect.h"
#include "Socket.h"
#include "FaceTask.h"


 
int main() {
    
    Task::all_thread_start();
    CameraUvc::Instance();
    FaceDetection::Instance();
    Socket::Instance();
    WebConnect::Instance();
    FaceTask::Instance();

    FaceTask::Instance()->start();
    FaceTask::Instance()->wait();

    return 0;
}

