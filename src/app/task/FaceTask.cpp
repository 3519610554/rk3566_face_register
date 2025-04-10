#include "FaceTask.h"
#include "InputCheck.h"
#include "CameraUvc.h"
#include "FaceDetection.h"
#include "Web.h"

FaceTask::FaceTask(){

    CameraUvc::Instance();
    FaceDetection::Instance();
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

        // cv::imshow("USB Camera", frame);
        util::Web::send_image_to_flask(frame);
        // cv::waitKey(1);

        int ch = util::InputCheck::get_char_non_blocking();
        if (ch == -1)
            continue;
        if (m_input_name_flag){
            m_input_name_flag = false;
            FaceDetection::Instance()->enroll_face(std::string(1, ch));
        }else {
            if (ch == 'q') {
                std::cout << "already exists" << std::endl;
                break;
            }else if (ch == 'd'){
                m_input_name_flag = true;
                
            }
        }
    }
    m_thread.detach();
    exit(0);
}

FaceTask* FaceTask::Instance(){

    static FaceTask face_task;

    return &face_task;
}
