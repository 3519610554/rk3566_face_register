#include "face_task.h"
#include "camera_uvc.h"
#include "face_detection.h"
#include "web_connect.h"
#include "thread_pool.h"
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
