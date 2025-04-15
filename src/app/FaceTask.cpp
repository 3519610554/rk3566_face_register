#include "FaceTask.h"
#include "InputCheck.h"
#include "CameraUvc.h"
#include "FaceDetection.h"
#include "WebConnect.h"
#include "Task.h"

FaceTask::FaceTask(){

    m_input_name_flag = false;
}

FaceTask::~FaceTask(){
    
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
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        FaceDetection::Instance()->detection_task(frame, gray);

        cv::imshow("USB Camera", frame);
        cv::waitKey(1);

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
