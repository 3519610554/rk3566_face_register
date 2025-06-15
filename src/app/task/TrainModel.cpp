#include "TrainModel.h"
#include "File.h"
#include "ThreadPool.h"
#include <thread>
#include <utility>
#include <chrono> 
#include <spdlog/spdlog.h>

#define FACE_MODEL_PATH         "/model/faces/"
#define FACE_MODEL_YML          (FACE_MODEL_PATH"face_gather.yml")
#define FACE_MODEL              (FACE_MODEL_PATH"face_model.xml")

#define TARIN_WIDTH                 100
#define TARIN_HEIGHT                100

#define FS_FACES_WRITE(x, data)     x << "faces" << data
#define FS_LABELS_WRITE(x, data)    x << "labels" << data

#define FS_FACES_READ(x, data)      x["faces"] >> data
#define FS_LABELS_READ(x, data)     x["labels"] >> data

TrainModel::TrainModel(){
    util::create_file(FACE_MODEL_PATH, nullptr);
    m_model = MPDEL_TRAIN_METHODS::create();
}

TrainModel::~TrainModel(){

}

TrainModel* TrainModel::Instance(){

    static TrainModel train_model;

    return &train_model;
}

void TrainModel::initialize(){

    if (util::file_exist(FACE_MODEL)){
        m_model->read(FACE_MODEL);
        m_model_state.store(true);
    }else {
        m_model_state.store(false);
    }
    ThreadPool::Instance()->enqueue(&TrainModel::train_model, this);
}

void TrainModel::train_model(){

    while(ThreadPool::Instance()->get_thread_state()){
        std::vector<cv::Mat> face_images;
        std::vector<int> face_labels;
        std::pair<std::vector<cv::Mat>, std::vector<int>> data = m_queue.pop();
        if (util::file_exist(FACE_MODEL_YML)){
            m_fs.open(FACE_MODEL_YML, cv::FileStorage::READ);
            FS_FACES_READ(m_fs, face_images);
            FS_LABELS_READ(m_fs, face_labels);        
            face_images.insert(face_images.end(), data.first.begin(), data.first.end());
            face_labels.insert(face_labels.end(), data.second.begin(), data.second.end());
            m_fs.release();
        }else {
            face_images.assign(data.first.begin(), data.first.end());
            face_labels.assign(data.second.begin(), data.second.end());
        }
        cv::Ptr<MPDEL_TRAIN_METHODS> train_model = MPDEL_TRAIN_METHODS::create();
        train_model->train(face_images, face_labels);
        train_model->save(FACE_MODEL);
        m_model_state.store(false);
        m_model = train_model;
        m_model_state.store(true);
        spdlog::info("train successfuly!");

        m_fs.open(FACE_MODEL_YML, cv::FileStorage::WRITE);
        FS_FACES_WRITE(m_fs, face_images);
        FS_LABELS_WRITE(m_fs, face_labels);
        m_fs.release();
    }
}

void TrainModel::train_size(cv::Mat &image){

    cv::resize(image, image, cv::Size(TARIN_WIDTH, TARIN_HEIGHT));
}

bool TrainModel::train_model_get(cv::Mat face, int &label, double &confidence){

    if (m_model_state.load()){
        train_size(face);
        m_model->predict(face, label, confidence); 
        // confidence /= 10000.0f;
        return true;
    }
    return false;
}

void TrainModel::train_data_add(std::vector<cv::Mat> face, std::vector<int> label){

    m_queue.push(std::make_pair(face, label));
}
