#ifndef _TRAIN_MODEL_H
#define _TRAIN_MODEL_H

#include <thread>
#include <atomic>
#include <queue>
#include <vector>
#include "OpencvPublic.h"
#include "SafeQueue.h"

#define MPDEL_TRAIN_METHODS     cv::face::LBPHFaceRecognizer
#define CONFIDENCE_THRESHOLD    100

class TrainModel{
public:
    TrainModel();
    ~TrainModel();
    //实例化
    static TrainModel* Instance();
    //初始化
    void initialize();
    //线程启动
    void start();
    //训练模型
    void train_model();
    //模型图片大小修正
    void train_size(cv::Mat &image);
    //模型辨别数据
    bool train_model_get(cv::Mat face, int &label, double &confidence);
    //训练模型数据添加
    void train_data_add(std::vector<cv::Mat> face, std::vector<int> label);
private:
    cv::FileStorage m_fs;
    // cv::Ptr<cv::face::EigenFaceRecognizer> m_model;
    cv::Ptr<MPDEL_TRAIN_METHODS> m_model;
    std::thread m_thread;
    std::atomic<bool> m_model_state;
    SafeQueue<std::pair<std::vector<cv::Mat>, std::vector<int>>> m_queue; 
};

#endif

