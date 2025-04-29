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
    //实例化
    static FaceDetection* Instance();
    //初始化
    void initialize();
    //添加处理帧
    void frame_data_add(cv::Mat frame);
    //人脸录入
    void enroll_face(std::string name);
protected:
    //图像处理线程
    void dispose_thread();
    //获取人脸
    size_t detection_faces(cv::Mat image, std::vector<cv::Rect> &objects);
    //录入人脸任务
    void enroll_face_task(cv::Mat &frame, cv::Mat &gray, std::vector<cv::Rect> faces);
    //人脸检测任务
    void detection_face_task(cv::Mat &frame, cv::Mat &gray, std::vector<cv::Rect> faces);
private:
    //中文字体
    cv::Ptr<cv::freetype::FreeType2> m_ft2;
    //检测模型
    cv::CascadeClassifier m_face_cascade;
    //人脸坐标
    std::vector<cv::Rect> m_faces;
    //录入照片
    std::vector<cv::Mat> m_enroll_face_images;
    //录入标签
    std::vector<int> m_enroll_face_labels;
    //处理帧队列
    SafeQueue<cv::Mat> m_frame;
    //帧间隔
    int m_frame_interval_cnt;
    //录入和检测任务函数
    std::function<void(cv::Mat&, cv::Mat&, std::vector<cv::Rect>)> m_face_task;
    //录入人脸总数
    int m_user_num;
    //录入人名
    std::string m_user_name;
    //检测上一次有效标签的个数
    std::vector<int> m_last_detection_label;
};

#endif
