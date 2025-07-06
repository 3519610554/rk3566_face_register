#ifndef CAMERAUVC_H
#define CAMERAUVC_H

#include <OpencvPublic.h>
#include <turbojpeg.h>
#include "mpp_jpeg_decoder.h"

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
    MppJpegDecoder m_mppDecoder;
    tjhandle         m_tjh{nullptr};
    // V4L2设备文件描述符
    int m_fd = -1;                    
    struct buffer {
        void* start;
        size_t length;
    };
    std::vector<buffer> m_buffers;
    std::vector<unsigned char> m_dstBuf;
    unsigned int m_buffer_count;
    int m_width;
    int m_height;
    cv::Mat m_frameMat;
};

#endif
