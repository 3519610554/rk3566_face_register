#ifndef CAMERAUVC_H
#define CAMERAUVC_H

#include "OpencvPublic.h"

#define CAP_WIDTH                   320
#define CAP_HEIGHT                  320

class CameraUvc{
public:
    CameraUvc();
    ~CameraUvc();
    //实例化
    static CameraUvc* Instance();
    //初始化
    void initialize(std::string camera_id = "/dev/video0");
    //获取帧照片
    bool frame_get(cv::Mat &frame);
private:
    cv::VideoCapture m_cap;
};

#endif
