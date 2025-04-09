#ifndef CAMERAUVC_H
#define CAMERAUVC_H

#include "OpencvPublic.h"

class CameraUvc{
public:
    CameraUvc(std::string camera_id = "/dev/video0");
    ~CameraUvc();
    //获取帧照片
    bool frame_get(cv::Mat &frame);
    //实例化
    static CameraUvc* Instance();
private:
    cv::VideoCapture m_cap;
};

#endif
