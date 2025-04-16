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
protected:
    //录入人脸任务
    void enroll_face_task(cv::Mat &frame, cv::Mat face, cv::Rect face_rect);
    //人脸检测任务
    void detection_face_task(cv::Mat &frame, cv::Mat face, cv::Rect face_rect, std::vector<int> &label);
private:
    // cv::dnn::Net m_net;
    cv::Ptr<cv::freetype::FreeType2> m_ft2;
    cv::CascadeClassifier m_face_cascade;
    std::vector<cv::Mat> m_face_images;
    std::vector<int> m_face_labels;
    std::vector<cv::Rect> m_faces;
    
    bool m_detection_state;
    int m_count;
    int m_user_num;
    std::string m_user_name;

    //上传参数
    size_t m_last_face_size;
    std::vector<int> m_last_label;

    int m_cnt;
};

#endif
