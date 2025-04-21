#include "CameraUvc.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

#define CAP_WIDTH                   320
#define CAP_HEIGHT                  176

CameraUvc::CameraUvc(){

    
}

CameraUvc::~CameraUvc(){
    
    m_cap.release();
    cv::destroyAllWindows();
}

CameraUvc* CameraUvc::Instance(){

    static CameraUvc camera;

    return &camera;
}

void CameraUvc::initialize(std::string camera_id){

    m_cap = cv::VideoCapture(camera_id, cv::CAP_V4L2);
    // 设置摄像头参数：MJPG + 分辨率 + 帧率
    m_cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    m_cap.set(cv::CAP_PROP_FRAME_WIDTH, CAP_WIDTH);
    m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, CAP_HEIGHT);
    m_cap.set(cv::CAP_PROP_FPS, 30);
    setenv("DISPLAY", ":10.0", 1);

    std::cout << "fps: " << m_cap.get(cv::CAP_PROP_FPS) << std::endl;
}

bool CameraUvc::frame_get(cv::Mat &frame){

    m_cap >> frame; 
    if (frame.empty()) {
        std::cerr << "读取帧失败" << std::endl;
        return false;
    }
    cv::flip(frame, frame, 1);
    return true;
}

