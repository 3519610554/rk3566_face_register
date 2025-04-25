#include "FaceTask.h"
#include "CameraUvc.h"
#include "FaceDetection.h"
#include "WebConnect.h"
#include "ThreadPool.h"
#include <spdlog/spdlog.h>

FaceTask::FaceTask(){

    
}

FaceTask::~FaceTask(){
    
}

FaceTask* FaceTask::Instance(){

    static FaceTask face_task;

    return &face_task;
}

void FaceTask::initialize(){

    ThreadPool::Instance()->enqueue(std::bind(&FaceTask::run, this));
}

void FaceTask::run(){

    while (true) {
        cv::Mat frame;
        if (!CameraUvc::Instance()->frame_get(frame)){
            break;
        }         
        FaceDetection::Instance()->frame_data_add(frame);
    }
}
