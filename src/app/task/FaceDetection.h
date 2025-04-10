#ifndef _FACE_DETECTION_H
#define _FACE_DETECTION_H

#include "OpencvPublic.h"

class FaceDetection{
public:
    FaceDetection();
    ~FaceDetection();
    //人脸检测任务
    void detection_task(cv::Mat &frame, cv::Mat gray);
    //人脸录入
    void enroll_face(std::string name);
    //实例化
    static FaceDetection* Instance();
private:
    //录入人脸任务
    void enroll_face_task(cv::Mat &frame, cv::Mat face, cv::Rect face_rect);
    //人脸检测任务
    void detection_face_task(cv::Mat &frame, cv::Mat face, cv::Rect face_rect);
    cv::CascadeClassifier m_face_cascade;
    std::vector<cv::Mat> m_face_images;
    std::vector<int> m_face_labels;
    
    bool m_detection_state;
    int m_count;
    int m_user_num;
    std::string m_user_name;
};

#endif
