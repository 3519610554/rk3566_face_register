#include "camera_uvc.h"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include <cstdlib>

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
    std::string ffmpeg_cmd =
        "/root/target/bin/ffmpeg -f h264 -i - "
        "-f rtsp -rtsp_transport tcp rtsp://0.0.0.0:8554/live";

    m_ffmpeg = popen(
        ffmpeg_cmd.c_str(), 
        "w"
    );

    if (!m_ffmpeg) {
        spdlog::error("无法启动 FFmpeg 进程");
        return;
    }
    ThreadPool::Instance()->enqueue(&CameraUvc::mediamtx_thread, this);
    ThreadPool::Instance()->enqueue(&CameraUvc::show_thread, this);
}

bool CameraUvc::frame_get(cv::Mat &frame) {
    m_cap >> frame;
    
    if (frame.empty()) {
        spdlog::info("无法读取帧");
        return false;
    }
    cv::flip(frame, frame, 1);
    
    return true;
}

void CameraUvc::frame_show(cv::Mat frame){
    m_frame.push(frame); 
}


void CameraUvc::show_thread(){

    spdlog::info("显示视频线程启动...");

    while(true){
        cv::Mat frame = m_frame.pop();
        int width = frame.cols;
        int height = frame.rows;
        int hor_stride = MPP_ALIGN(width, 16);
        int ver_stride = MPP_ALIGN(height, 16);
        size_t stride = MPP_ALIGN(width, 16);
        size_t frame_size = stride * height * 3 / 2;

        std::vector<uint8_t> nv12_buffer(frame_size);

        m_encoder.bgr_to_nv12(frame, nv12_buffer.data());

        std::vector<uint8_t> encoded;
        if (!m_encoder.encode(nv12_buffer.data(), encoded)) {
            spdlog::error("编码失败");
            return;
        }
        spdlog::info("Encoded frame size: {}", encoded.size());

        size_t write_size = fwrite(encoded.data(), 1, encoded.size(), m_ffmpeg);
        fflush(m_ffmpeg);

        if (write_size != encoded.size()) {
            spdlog::error("写入 FFmpeg 进程失败");
            return;
        }
        cv::imshow("USB Camera", frame);
        cv::waitKey(1);
    }
}

void CameraUvc::mediamtx_thread(){
    int ret = system("./mediamtx");
    if (ret != 0) {
        spdlog::error("mediamtx 启动失败，返回码 {}", ret);
    }
}
