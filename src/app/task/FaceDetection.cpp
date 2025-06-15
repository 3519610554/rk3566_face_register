#include "FaceDetection.h"
#include "File.h"
#include "Socket.h"
#include "TrainModel.h"
#include "UserSQLite.h"
#include "WebConnect.h"
#include "BackendSQLite.h"
#include "ThreadPool.h"
#include "Base64.h"
#include "LocalTime.h"
#include "opencv2/core/mat.hpp"
#include <cstddef>
#include <sys/types.h>
#include <spdlog/spdlog.h>
#include "RnkkInference.h"

#define TASK_SWITCH(x)      std::bind(&FaceDetection::x, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

#define FONT_PATH           "./font/NotoSansSC-VariableFont_wght.ttf"


FaceDetection::FaceDetection(){

    m_frame_interval_cnt = 0;
    m_face_task = TASK_SWITCH(detection_face_task);
    m_user_num = UserSQLite::Instance()->get_row_count();
}

FaceDetection::~FaceDetection(){


}

FaceDetection* FaceDetection::Instance(){

    static FaceDetection face_detection;

    return &face_detection;
}

void FaceDetection::initialize(){

    m_ft2 = cv::freetype::createFreeType2();
    m_ft2->loadFontData(FONT_PATH, 0);
    ThreadPool::Instance()->enqueue(&FaceDetection::dispose_thread, this);
    RnkkInference::Instance()->initialize();
}

size_t FaceDetection::detection_faces(cv::Mat image, std::vector<cv::Rect> &objects){

    if (++m_frame_interval_cnt < 3)
        return objects.size();
    m_frame_interval_cnt = 0;
    objects.clear();
    RnkkInference::Instance()->detection_face(image, objects);
    if (objects.size() > 0){
        spdlog::info("face num: {}", objects.size());
    }

    return objects.size();
}

void FaceDetection::dispose_thread(){

    spdlog::info("dispose thread started");

    while(true){
        cv::Mat frame = m_frame.pop();
        // cv::Mat gray;
        // cv::cvtColor(frame, gray, cv::COLOR_RGBA2RGB);
        // detection_faces(frame, m_faces);
        // m_face_task(frame, frame, m_faces);
        cv::imshow("USB Camera", frame);
        cv::waitKey(1);
        spdlog::info("loop");
    }
}

void FaceDetection::enroll_face_task(cv::Mat &frame, cv::Mat &gray, std::vector<cv::Rect> faces){

    size_t faces_size = faces.size();
    if (faces_size==0 || faces_size > 1)
        return;

    cv::Rect face_rect = faces.front();
    cv::Mat face = gray(face_rect);
    TrainModel::Instance()->train_size(face);
    m_enroll_face_images.push_back(face);
    m_enroll_face_labels.push_back(m_user_num);
    size_t count = m_enroll_face_images.size();

    // 显示采集状态
    std::string label_text = "gather:" + std::to_string((int)((count*100)/50.0f)) + "%";
    cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 0), 2);
    putText(frame, label_text, cv::Point(25, 25), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);

    if (count < 50) 
        return;
    m_face_task = TASK_SWITCH(detection_face_task);
    spdlog::info("The {} facial image capture has been completed.", m_user_num);
    TrainModel::Instance()->train_data_add(m_enroll_face_images, m_enroll_face_labels);
    UserSQLite::Instance()->insert_data(m_user_num, m_user_name);
    m_enroll_face_images.clear();
    m_enroll_face_labels.clear();
}

void FaceDetection::detection_face_task(cv::Mat &frame, cv::Mat &gray, std::vector<cv::Rect> faces){

    size_t faces_size = faces.size();
    if (faces_size == 0)
        return;

    std::vector<int> detection_label;
    std::string label_text = "";
    for (size_t i = 0; i < faces_size; i ++){
        int predicted_label = -1;
        double confidence = 0.0;
        cv::Rect face_rect = faces[i];
        cv::Mat face = gray(face_rect);
        cv::Scalar scalar;
        
        bool state = TrainModel::Instance()->train_model_get(face, predicted_label, confidence);
        if (state && (predicted_label != -1) && (confidence < CONFIDENCE_THRESHOLD)){
            detection_label.push_back(predicted_label);
            scalar = cv::Scalar(0, 255, 0);
            label_text = UserSQLite::Instance()->get_name_by_id(predicted_label);
            cv::Point text_org(face_rect.x, face_rect.y - 35);
            cv::Scalar color(255, 0, 0);
            m_ft2->putText(frame, label_text, text_org, 30, color, -1, cv::LINE_AA, false);
        }else {
            scalar = cv::Scalar(0, 255, 255);  // 黄色
        }
        cv::rectangle(frame, face_rect, scalar, 2);
    }
    if (detection_label.size() == 0)
        return;
    std::sort(detection_label.begin(), detection_label.end());
    if (m_last_detection_label == detection_label)
        return;
    m_last_detection_label = detection_label;
    std::string imgBase64 = util::encodeBase64(util::mat_to_buffer(frame));
    std::string current_time = util::get_cuurent_time();
    int imageId = BackendSQLite::Instance()->insert_data(current_time, imgBase64);
    WebConnect::Instance()->send_image(CLIENT_ALL, imageId, current_time, imgBase64);
    spdlog::info("{} was identified", label_text);
}

void FaceDetection::frame_data_add(cv::Mat frame){

    m_frame.push(frame);
}

void FaceDetection::enroll_face(std::string name){

    m_face_task = TASK_SWITCH(enroll_face_task);
    m_user_name = name;
    m_user_num ++;
}
