#ifndef CAMERAUVC_H
#define CAMERAUVC_H

#include "OpencvPublic.h"

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
    cv::VideoCapture m_cap;
    std::string m_camera_id;
    int m_camera_width;
    int m_camera_height;
};

#endif
