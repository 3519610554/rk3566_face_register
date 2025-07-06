#include "CameraUvc.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>

CameraUvc::CameraUvc(): 
    m_buffer_count(4),
    m_tjh(tjInitDecompress()){
    if (!m_tjh) {
        spdlog::error("TurboJPEG init failed: {}", tjGetErrorStr());
    }
}

CameraUvc::~CameraUvc(){
    if (m_tjh) {
        tjDestroy(m_tjh);
        m_tjh = nullptr;
    }

    if (m_fd >= 0) {
        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(m_fd, VIDIOC_STREAMOFF, &type);
        for (auto &buf : m_buffers) {
            munmap(buf.start, buf.length);
        }
        close(m_fd);
    }
    cv::destroyAllWindows();
}

CameraUvc* CameraUvc::Instance(){

    static CameraUvc camera;

    return &camera;
}

void CameraUvc::initialize(std::string yaml_path){
    //解析配置
    YAML::Node cfg = YAML::LoadFile(yaml_path)["camera"];
    std::string dev = cfg["id"].as<std::string>();
    m_width  = cfg["width"].as<int>();
    m_height = cfg["height"].as<int>();

    //打开设备
    m_fd = open(dev.c_str(), O_RDWR);
    if (m_fd < 0) {
        spdlog::error("Failed to open device: {}", dev);
        return;
    }

    //设置 MJPEG 格式
    v4l2_format fmt = {};
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = m_width;
    fmt.fmt.pix.height      = m_height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field       = V4L2_FIELD_NONE;
    if (ioctl(m_fd, VIDIOC_S_FMT, &fmt) < 0) {
        spdlog::error("VIDIOC_S_FMT failed");
        close(m_fd);
        m_fd = -1;
        return;
    }
    m_width  = fmt.fmt.pix.width;
    m_height = fmt.fmt.pix.height;
    spdlog::info("Camera resolution: {}x{}", m_width, m_height);

    //请求缓冲区
    v4l2_requestbuffers req = {};
    req.count  = m_buffer_count;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(m_fd, VIDIOC_REQBUFS, &req) < 0) {
        spdlog::error("VIDIOC_REQBUFS failed");
        close(m_fd);
        m_fd = -1;
        return;
    }

    //mmapBuffers
    m_buffers.resize(req.count);
    for (int i = 0; i < (int)req.count; ++i) {
        v4l2_buffer buf = {};
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;
        if (ioctl(m_fd, VIDIOC_QUERYBUF, &buf) < 0) {
            spdlog::error("VIDIOC_QUERYBUF failed");
            close(m_fd);
            m_fd = -1;
            return;
        }
        m_buffers[i].length = buf.length;
        m_buffers[i].start  =
            mmap(nullptr, buf.length,
                 PROT_READ | PROT_WRITE,
                 MAP_SHARED, m_fd, buf.m.offset);
        if (m_buffers[i].start == MAP_FAILED) {
            spdlog::error("mmap failed");
            close(m_fd);
            m_fd = -1;
            return;
        }
    }

    //入队所有缓冲区
    for (int i = 0; i < (int)m_buffers.size(); ++i) {
        v4l2_buffer buf = {};
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;
        ioctl(m_fd, VIDIOC_QBUF, &buf);
    }

    //开启采集
    {
        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(m_fd, VIDIOC_STREAMON, &type) < 0) {
            spdlog::error("VIDIOC_STREAMON failed");
            close(m_fd);
            m_fd = -1;
            return;
        }
    }

    // 预分配解码缓冲区和 Mat
    m_dstBuf .resize(m_width * m_height * 3);
    m_frameMat = cv::Mat(m_height, m_width, CV_8UC3, m_dstBuf.data());

    // if (!m_mppDecoder.init(m_width, m_height)) {
    //     spdlog::error("MPP JPEG decoder init failed");
    // }
}

bool CameraUvc::frame_get(cv::Mat &frame){
    if (m_fd < 0 || !m_tjh) {
        return false;
    }

    //1) DQBUF
    v4l2_buffer buf = {};
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (ioctl(m_fd, VIDIOC_DQBUF, &buf) < 0) {
        return false;
    }

    auto data   = reinterpret_cast<unsigned char*>(m_buffers[buf.index].start);
    auto length = buf.bytesused;

    //2) 解码到 m_dstBuf
    if (tjDecompress2(
            m_tjh,
            data,
            length,
            m_dstBuf.data(),
            m_width,
            0,
            m_height,
            TJPF_BGR,
            TJFLAG_FASTDCT
        ) < 0)
    {
        spdlog::error("TurboJPEG decode error: {}", tjGetErrorStr());
        ioctl(m_fd, VIDIOC_QBUF, &buf);
        return false;
    }
    // if (!m_mppDecoder.decode(data, length, m_dstBuf.data())) {
    //     spdlog::error("MPP JPEG decode failed");
    //     ioctl(m_fd, VIDIOC_QBUF, &buf);
    //     return false;
    // }

    //3) 返回预分配好的 Mat
    cv::flip(m_frameMat, frame, 1);

    //4) QBUF
    ioctl(m_fd, VIDIOC_QBUF, &buf);
    return true;
}

