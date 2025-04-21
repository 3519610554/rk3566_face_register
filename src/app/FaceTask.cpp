#include "FaceTask.h"
#include "InputCheck.h"
#include "CameraUvc.h"
#include "FaceDetection.h"
#include "WebConnect.h"
#include "Task.h"

FaceTask::FaceTask(){

    
}

FaceTask::~FaceTask(){
    
}

void FaceTask::initialize(){

    
}

void FaceTask::start(){

    m_thread = std::thread(&FaceTask::run, this);
}  

void FaceTask::wait(){
    if (m_thread.joinable()) {
        m_thread.join(); 
    }
}

void FaceTask::run(){

    while (true) {
        cv::Mat frame;
        if (!CameraUvc::Instance()->frame_get(frame)){
            break;
        }         
        FaceDetection::Instance()->frame_data_add(frame);

        int ch = util::InputCheck::get_char_non_blocking();
        if (ch == 'q') {
            std::cout << "already exists" << std::endl;
            break;
        }
    }
    Task::all_thread_stop();
}

FaceTask* FaceTask::Instance(){

    static FaceTask face_task;

    return &face_task;
}
