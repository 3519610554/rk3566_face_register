#ifndef _FACE_DETECTION_H
#define _FACE_DETECTION_H

#include "OpencvPublic.h"
#include "SafeQueue.h"
#include <thread>
#include <vector>

class FaceDetection{
public:
    FaceDetection();
    ~FaceDetection();
    //添加处理帧
    void frame_data_add(cv::Mat frame);
    //人脸录入
    void enroll_face(std::string name);
    //实例化
    static FaceDetection* Instance();
protected:
    //图像处理线程
    void dispose_thread();
    //获取人脸
    size_t detection_faces(cv::Mat image, std::vector<cv::Rect> &objects);
    //录入人脸任务
    void enroll_face_task(cv::Mat &frame, cv::Mat face, cv::Rect face_rect);
    //人脸检测任务
    void detection_face_task(cv::Mat &frame, cv::Mat face, cv::Rect face_rect, std::vector<int> &label);
private:
    // cv::dnn::Net m_net;
    //中文字体
    cv::Ptr<cv::freetype::FreeType2> m_ft2;
    //检测模型
    cv::CascadeClassifier m_face_cascade;
    //人脸坐标
    std::vector<cv::Rect> m_faces;
    //录入照片
    std::vector<cv::Mat> m_face_images;
    //录入标签
    std::vector<int> m_face_labels;
    std::thread m_thread;
    //处理帧队列
    SafeQueue<cv::Mat> m_frame;
    //录入和检测标志位
    bool m_detection_state;
    //录入人脸总数
    int m_user_num;
    //录入人名
    std::string m_user_name;
    
    //上传参数
    size_t m_last_face_size;
    std::vector<int> m_last_label;
};

#endif
