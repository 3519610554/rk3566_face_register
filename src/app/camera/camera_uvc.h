#ifndef CAMERAUVC_H
#define CAMERAUVC_H

#include <OpencvPublic.h>

#include "ffmpeg_mpp_encoder.h"
#include "safe_queue.h"
#include "thread_pool.h"

class CameraUvc{
public:
    CameraUvc();
    ~CameraUvc();
    //实例化
    static CameraUvc* Instance();
    //初始化
    void initialize(std::string yaml_path);
    //获取帧照片
    bool frame_get(cv::Mat &frame);
    //显示画面
    void frame_show(cv::Mat frame);
protected:
    void show_thread();
private:
    struct CameraInfo{
        std::string id;
        int width;
        int height;
        int fps;
    };
private:
    CameraInfo m_camera;
    cv::VideoCapture m_cap;
    FFmpegMppEncoder m_ffmpeg;
    SafeQueue<cv::Mat> m_frame;
};

#endif
