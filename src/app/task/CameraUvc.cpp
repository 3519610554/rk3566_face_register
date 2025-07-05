#include "CameraUvc.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

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

void CameraUvc::initialize(std::string yaml_path){
    YAML::Node camera = YAML::LoadFile(yaml_path)["camera"];
    m_camera_id = camera["id"].as<std::string>();
    m_camera_width = camera["width"].as<int>();
    m_camera_height = camera["height"].as<int>();
    m_cap = cv::VideoCapture(m_camera_id, cv::CAP_V4L2);
    // 设置摄像头参数：MJPG + 分辨率 + 帧率
    m_cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'));
    m_cap.set(cv::CAP_PROP_FRAME_WIDTH, m_camera_width);
    m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, m_camera_height);
    m_cap.set(cv::CAP_PROP_FPS, 30);
    setenv("DISPLAY", ":10.0", 1);

    spdlog::info("Camera fps: {}, m_camera_width: {}, m_camera_height: {}", m_cap.get(cv::CAP_PROP_FPS), m_camera_width, m_camera_height);
}

bool CameraUvc::frame_get(cv::Mat &frame){

    m_cap >> frame; 
    if (frame.empty()) {
        spdlog::error("faild to frame reading");
        return false;
    }
    cv::flip(frame, frame, 1);
    if (frame.cols == 0 || frame.rows == 0) {
        spdlog::info("无效图像：图像尺寸为 0。");
    }
    return true;
}

