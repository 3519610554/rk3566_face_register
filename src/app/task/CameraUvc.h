#ifndef CAMERAUVC_H
#define CAMERAUVC_H

#include <thread>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/face.hpp>

class CameraUvc{
public:
    CameraUvc(std::string camera_id = "/dev/video0");
    ~CameraUvc();
    void start();
    void wait();
protected:
    //训练模型
    void train_model();
    //录入人脸
    void enroll_face(cv::Mat &frame, cv::Mat face, cv::Rect face_rect);
    void detection_face(cv::Mat &frame, cv::Mat face, cv::Rect face_rect);
    void run();
private:
    std::thread m_thread;
    std::thread m_train_thread;
    cv::VideoCapture m_cap;
    cv::CascadeClassifier m_face_cascade;
    std::vector<cv::Mat> m_face_images;
    std::vector<int> m_face_labels;
    cv::Ptr<cv::face::EigenFaceRecognizer> m_model;
    cv::FileStorage m_fs;
    std::atomic<bool> m_ready;
    bool m_detection_state;
    int m_count;
};

#endif
