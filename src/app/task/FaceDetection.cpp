#include "FaceDetection.h"
#include "File.h"
#include "TrainModel.h"
#include "UserSQLite.h"
#include "WebConnect.h"
#include "Task.h"
#include <sys/types.h>

// #define NET_PROTOTXT        util::File::get_currentWorking_directory() + "/model/deploy.prototxt"
// #define NET_CAFFEMODEL      util::File::get_currentWorking_directory() + "/model/res10_300x300_ssd_iter_140000.caffemodel"
// #define FACE_DEFAULT_MODEL  util::File::get_currentWorking_directory() + "/model/haarcascade_frontalface_default.xml"
#define FACE_DEFAULT_MODEL  util::File::get_currentWorking_directory() + "/model/haarcascade_frontalface_alt2.xml"
// #define FACE_DEFAULT_MODEL  util::File::get_currentWorking_directory() + "/model/det_500m.onnx"
// #define FACE_DEFAULT_MODEL  util::File::get_currentWorking_directory() + "/model/haarcascade_frontalcatface.xml"
#define FONT_PATH           util::File::get_currentWorking_directory() + "/font/NotoSansSC-VariableFont_wght.ttf"

FaceDetection::FaceDetection(){

    m_ft2 = cv::freetype::createFreeType2();
    m_ft2->loadFontData(FONT_PATH, 0);
    if (!m_face_cascade.load(FACE_DEFAULT_MODEL)) {
        std::cerr << "failed to load the face detection model!" << std::endl;
        return;
    }
    m_detection_state = false;
    m_user_num = UserSQLite::Instance()->get_row_count();
    TrainModel::Instance();
    m_thread = std::thread(&FaceDetection::dispose_thread, this);
}

FaceDetection::~FaceDetection(){


}

size_t FaceDetection::detection_faces(cv::Mat image, std::vector<cv::Rect> &objects){

    static int frmae_cnt = 0;

    if (++frmae_cnt < 2)
        return 0;
    frmae_cnt = 0;

    m_face_cascade.detectMultiScale(image, objects, 1.2, 6, 0, cv::Size(30, 30));

    return objects.size();
}

void FaceDetection::dispose_thread(){

    std::cout << "dispose thread start successfuly" << std::endl;

    while(true){
        cv::Mat frame = m_frame.pop();
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        detection_faces(gray, m_faces);
        size_t faces_size = m_faces.size();
        std::vector<int> label;
        for (size_t i = 0; i < faces_size; i++) {
            cv::Mat face = gray(m_faces[i]);
            if (m_detection_state){
                enroll_face_task(frame, face, m_faces[i]);
            }else {
                detection_face_task(frame, face, m_faces[i], label);
            }
        }  
        cv::imshow("USB Camera", frame);
        cv::waitKey(1);
        // if (label.size() <= 0)
        //     return;
        // std::sort(label.begin(), label.end());
        // if (m_last_face_size != label.size() || label != m_last_label){
        //     m_last_face_size = label.size();
        //     m_last_label = label;
        //     std::sort(m_last_label.begin(), m_last_label.end());
        //     WebConnect::Instance()->send_image(frame);
        //     std::cout << "send image" << std::endl;
        // }
    }
}

void FaceDetection::enroll_face_task(cv::Mat &frame, cv::Mat face, cv::Rect face_rect){

    TrainModel::Instance()->train_size(face);
    m_face_images.push_back(face);
    m_face_labels.push_back(m_user_num);
    size_t count = m_face_images.size();

    // 显示采集状态
    std::string label_text = "gather:" + std::to_string((int)((count*100)/50.0f)) + "%";
    cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 0), 2);
    putText(frame, label_text, cv::Point(25, 25), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);

    // 如果已经采集了50张图像，停止采集
    if (count >= 50) {
        m_detection_state = false;
        std::cout << "The " << m_user_num << " facial image capture has been completed." << std::endl;
        TrainModel::Instance()->train_data_add(m_face_images, m_face_labels);
        UserSQLite::Instance()->insert_data(m_user_num, m_user_name);
        m_face_images.clear();
        m_face_labels.clear();
    }
}

void FaceDetection::detection_face_task(cv::Mat &frame, cv::Mat face, cv::Rect face_rect, std::vector<int> &label){

    // 进行人脸识别
    int predicted_label = -1;
    double confidence = 0.0;
    cv::Scalar scalar;
    bool state = TrainModel::Instance()->train_model_get(face, predicted_label, confidence);
    if (state && (predicted_label != -1) && (confidence < CONFIDENCE_THRESHOLD)){
        label.push_back(predicted_label);
        scalar = cv::Scalar(0, 255, 0);
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << confidence;
        // std::string label_text = "name:" + UserSQLite::Instance()->get_name_by_id(predicted_label) + " C:" + ss.str();
        std::string label_text = UserSQLite::Instance()->get_name_by_id(predicted_label);
        cv::Point text_org(face_rect.x, face_rect.y - 35);
        cv::Scalar color(255, 0, 0);

        m_ft2->putText(frame, label_text, text_org, 30, color, -1, cv::LINE_AA, false);
    }else {
        scalar = cv::Scalar(0, 255, 255);  // 黄色
    }
    cv::rectangle(frame, face_rect, scalar, 2);
}

void FaceDetection::frame_data_add(cv::Mat frame){

    m_frame.push(frame);
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
