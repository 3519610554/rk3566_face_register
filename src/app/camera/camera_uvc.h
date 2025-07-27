#ifndef CAMERAUVC_H
#define CAMERAUVC_H

#include <OpencvPublic.h>

#include "mpp_encoder.h"

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
    MppEncoder m_encoder;
    FILE* m_ffmpeg;
};

#endif
