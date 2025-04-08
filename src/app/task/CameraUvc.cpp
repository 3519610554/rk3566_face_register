#include "CameraUvc.h"
#include <iostream>
#include <iomanip>

#include "InputCheck.h"
#include "File.h"

#define FACE_MODEL_PATH         util::File::get_currentWorking_directory()+"/model/faces/"
#define FACE_MODEL_YML          (FACE_MODEL_PATH+"face_gather.yml").c_str()
#define FACE_MODEL              (FACE_MODEL_PATH+"face_model.xml").c_str()

#define CAP_WIDTH                   320
#define CAP_HEIGHT                  240

#define FS_FACES_WRITE(x, data)     x << "faces" << data
#define FS_LABELS_WRITE(x, data)    x << "labels" << data

#define FS_FACES_READ(x, data)      x["faces"] >> data
#define FS_LABELS_READ(x, data)     x["labels"] >> data

CameraUvc::CameraUvc(std::string camera_id){

    m_cap = cv::VideoCapture(camera_id, cv::CAP_V4L2);
    // 设置摄像头参数：MJPG + 分辨率 + 帧率
    m_cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    m_cap.set(cv::CAP_PROP_FRAME_WIDTH, CAP_WIDTH);
    m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, CAP_HEIGHT);
    m_cap.set(cv::CAP_PROP_FPS, 30);
    setenv("DISPLAY", ":10.0", 1);

    std::cout << "fps: " << m_cap.get(cv::CAP_PROP_FPS) << std::endl;

    std::string current_path = util::File::get_currentWorking_directory() + "/model/haarcascade_frontalface_default.xml";
    if (!m_face_cascade.load(current_path)) {
        std::cerr << "failed to load the face detection model!" << std::endl;
        return;
    }

    util::File::create_file(FACE_MODEL_YML, "a");

    m_model = cv::face::EigenFaceRecognizer::create();
    try{
        m_model->read(FACE_MODEL);
    }catch (const std::exception& e){
        std::cout << "read face model exception caught: " << e.what() << std::endl;
    }
    m_detection_state = false;
    m_count = 0;
}

CameraUvc::~CameraUvc(){
    
    m_cap.release();
    cv::destroyAllWindows();
}

void CameraUvc::start(){

    m_ready = false;
    m_thread = std::thread(&CameraUvc::run, this);
    m_train_thread = std::thread(&CameraUvc::train_model, this);
}  

void CameraUvc::wait(){

    m_thread.join();
}

void CameraUvc::enroll_face(cv::Mat &frame, cv::Mat face, cv::Rect face_rect){

    // 显示矩形框
    cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 0), 2);

    cv::resize(face, face, cv::Size(CAP_WIDTH, CAP_HEIGHT));
    m_face_images.push_back(face);
    m_face_labels.push_back(1);
    m_count++;

    // 显示采集状态
    putText(frame, "be entering...", cv::Point(25, 25), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);

    // 如果已经采集了50张图像，停止采集
    if (m_count >= 50) {
        m_count = 0;
        m_detection_state = false;
        m_ready = true;
    }
}

void CameraUvc::train_model(){

    while(1){
        while(!m_ready);
        std::vector<cv::Mat> face_images;
        std::vector<int> face_labels;

        try{
            m_fs.open(FACE_MODEL_YML, cv::FileStorage::READ);
            FS_FACES_READ(m_fs, face_images);
            FS_LABELS_READ(m_fs, face_labels);        
            face_images.insert(face_images.end(), m_face_images.begin(), face_images.end());
            face_labels.insert(face_labels.end(), m_face_labels.begin(), m_face_labels.end());
            m_fs.release();
        }catch(const std::exception& e){
            face_images.assign(m_face_images.begin(), m_face_images.end());
            face_labels.assign(m_face_labels.begin(), m_face_labels.end());
        }
        
        m_model->train(face_images, face_labels);
        m_model->save(FACE_MODEL);
        std::cout << "train accomplish!" << std::endl;

        m_fs.open(FACE_MODEL_YML, cv::FileStorage::APPEND);
        FS_FACES_WRITE(m_fs, face_images);
        FS_LABELS_WRITE(m_fs, face_labels);
        m_fs.release();
        m_face_images.clear();
        m_face_labels.clear();
        m_ready = false;
    }
}

void CameraUvc::detection_face(cv::Mat &frame, cv::Mat face, cv::Rect face_rect){

    // 进行人脸识别
    int predicted_label = -1;
    double confidence = 0.0;
    try{
        cv::resize(face, face, cv::Size(CAP_WIDTH, CAP_HEIGHT));
        m_model->predict(face, predicted_label, confidence); 
        confidence = confidence / 10000.0f;
    }catch(const std::exception& e){
        return;
    }
    if (predicted_label == -1 || confidence > 0.6)
        return;
    // 绘制识别结果
    cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 0), 2);  // 绘制人脸框
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << confidence;
    std::string confidence_str = ss.str();
    // 在人脸上方显示识别的标签
    std::string label_text = "P:" + std::to_string(predicted_label) + " C:" + confidence_str;
    cv::putText(frame, label_text, cv::Point(face_rect.x, face_rect.y - 10),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
}

void CameraUvc::run() {
    cv::Mat frame;

    while (1) {
        m_cap >> frame;
        if (frame.empty()) {
            std::cerr << "读取帧失败" << std::endl;
            break;
        }
        cv::flip(frame, frame, 1);

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // 检测人脸
        std::vector<cv::Rect> faces;
        m_face_cascade.detectMultiScale(gray, faces, 1.2, 5, 0, cv::Size(25, 25));

        //在每个人脸上画框
        for (size_t i = 0; i < faces.size(); i++) {
            cv::Mat face = gray(faces[i]);
            if (m_detection_state){
                enroll_face(frame, face, faces[i]);
            }else {
                detection_face(frame, face, faces[i]);
            }    
        }
        cv::imshow("USB Camera", frame);
        cv::waitKey(1);

        int ch = util::InputCheck::get_char_non_blocking();
        if (ch == 'q') {
            std::cout << "already exists" << std::endl;
            break;
        }else if (ch == 'd'){
            m_detection_state = true;
        }
    }
}
