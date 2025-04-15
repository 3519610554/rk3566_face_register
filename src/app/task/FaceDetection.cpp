#include "FaceDetection.h"
#include "File.h"
#include "TrainModel.h"
#include "UserSQLite.h"

#define FACE_DEFAULT_MODEL  util::File::get_currentWorking_directory() + "/model/haarcascade_frontalface_default.xml"
#define FONT_PATH           util::File::get_currentWorking_directory() + "/font/NotoSansSC-VariableFont_wght.ttf"

FaceDetection::FaceDetection(){

    m_ft2 = cv::freetype::createFreeType2();
    m_ft2->loadFontData(FONT_PATH, 0);
    if (!m_face_cascade.load(FACE_DEFAULT_MODEL)) {
        std::cerr << "failed to load the face detection model!" << std::endl;
        return;
    }
    m_detection_state = false;
    m_count = 0;
    m_user_num = UserSQLite::Instance()->get_row_count();
    TrainModel::Instance();
}

FaceDetection::~FaceDetection(){


}

void FaceDetection::enroll_face_task(cv::Mat &frame, cv::Mat face, cv::Rect face_rect){

    TrainModel::Instance()->train_size(face);
    m_face_images.push_back(face);
    m_face_labels.push_back(m_user_num);
    m_count++;

    // 显示采集状态
    std::string label_text = "gather:" + std::to_string((int)((m_count*100)/50.0f)) + "%";
    cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 0), 2);
    putText(frame, label_text, cv::Point(25, 25), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);

    // 如果已经采集了50张图像，停止采集
    if (m_count >= 50) {
        m_count = 0;
        m_detection_state = false;
        std::cout << "The " << m_user_num << " facial image capture has been completed." << std::endl;
        TrainModel::Instance()->train_data_add(m_face_images, m_face_labels);
        UserSQLite::Instance()->insert_data(m_user_num, m_user_name);
        m_face_images.clear();
        m_face_labels.clear();
    }
}

void FaceDetection::detection_face_task(cv::Mat &frame, cv::Mat face, cv::Rect face_rect){

    // 进行人脸识别
    int predicted_label = -1;
    double confidence = 0.0;
    cv::Scalar scalar;
    bool state = TrainModel::Instance()->train_model_get(face, predicted_label, confidence);
    // if (state && (predicted_label != -1) && (confidence < 0.6)){
    if (state && (predicted_label != -1) && (confidence < CONFIDENCE_THRESHOLD)){
        scalar = cv::Scalar(0, 255, 0);
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << confidence;
        // std::string label_text = "name:" + UserSQLite::Instance()->get_name_by_id(predicted_label) + " C:" + ss.str();
        std::string label_text = UserSQLite::Instance()->get_name_by_id(predicted_label);
        cv::Point text_org(face_rect.x, face_rect.y - 35);
        cv::Scalar color(255, 0, 0);

        m_ft2->putText(frame, label_text, text_org, 30, color, -1, cv::LINE_AA, false);
        // cv::putText(frame, label_text, cv::Point(face_rect.x, face_rect.y - 10),
        //     cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
    }else {
        scalar = cv::Scalar(0, 255, 255);  // 黄色
    }
    cv::rectangle(frame, face_rect, scalar, 2);
}

void FaceDetection::detection_task(cv::Mat &frame, cv::Mat gray){

    // 检测人脸
    std::vector<cv::Rect> faces;
    m_face_cascade.detectMultiScale(gray, faces, 1.2, 5, 0, cv::Size(25, 25));

    for (size_t i = 0; i < faces.size(); i++) {
        cv::Mat face = gray(faces[i]);
        if (m_detection_state){
            enroll_face_task(frame, face, faces[i]);
        }else {
            detection_face_task(frame, face, faces[i]);
        }
    }  
}

void FaceDetection::enroll_face(std::string name){

    m_detection_state = true;
    m_user_name = name;
    m_user_num ++;
}

FaceDetection* FaceDetection::Instance(){

    static FaceDetection face_detection;

    return &face_detection;
}
