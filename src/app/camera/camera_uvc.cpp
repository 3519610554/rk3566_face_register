#include "camera_uvc.h"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

CameraUvc::CameraUvc(){

}

CameraUvc::~CameraUvc(){
    pclose(m_ffmpeg);
    m_cap.release();
    cv::destroyAllWindows();
}

CameraUvc* CameraUvc::Instance(){

    static CameraUvc camera;

    return &camera;
}

void CameraUvc::initialize(std::string yaml_path){
    //解析配置
    YAML::Node camera = YAML::LoadFile(yaml_path)["camera"];
    m_camera.id = camera["id"].as<std::string>();
    m_camera.width  = camera["width"].as<int>();
    m_camera.height = camera["height"].as<int>();   
    m_camera.fps = camera["fps"].as<int>();   
    
    cv::VideoCapture cap(m_camera.id, cv::CAP_V4L2);
    if (!cap.isOpened()) {
        spdlog::info("无法打开摄像头");
        return;
    }
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
    cap.set(cv::CAP_PROP_FRAME_WIDTH, m_camera.width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, m_camera.height);
    cap.set(cv::CAP_PROP_FPS, m_camera.fps);

    m_cap = cap;
    m_encoder.initialize(m_camera.width, m_camera.height);
    m_ffmpeg = popen(
        "ffmpeg -re -f h264 -i - "
        "-c copy -f rtsp rtsp://localhost:8554/live.stream",
        "w"
    );

    if (!m_ffmpeg) {
        spdlog::error("无法启动 FFmpeg 进程");
        return;
    }
}

bool CameraUvc::frame_get(cv::Mat &frame) {
    m_cap >> frame;
    
    if (frame.empty()) {
        spdlog::info("无法读取帧");
        return false;
    }

    cv::flip(frame, frame, 1);
    cv::Mat yuv;
    cv::cvtColor(frame, yuv, cv::COLOR_BGR2YUV_I420);
    std::vector<uint8_t> h264_data;
    int yuv_size = m_camera.width * m_camera.height * 3 / 2;
    bool success = m_encoder.encode(yuv.data, yuv_size, h264_data);
    if (success && !h264_data.empty()) {
        fwrite(h264_data.data(), 1, h264_data.size(), m_ffmpeg);
        fflush(m_ffmpeg);
    }
    return true;
}
