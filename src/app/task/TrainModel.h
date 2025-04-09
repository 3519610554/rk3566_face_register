#ifndef _TRAIN_MODEL_H
#define _TRAIN_MODEL_H

#include <thread>
#include <atomic>
#include <queue>
#include <vector>
#include "OpencvPublic.h"

class TrainModel{
public:
    TrainModel();
    ~TrainModel();
    //训练模型
    void train_model();
    //模型图片大小修正
    void train_size(cv::Mat &image);
    //模型辨别数据
    bool train_model_get(cv::Mat face, int &label, double &confidence);
    //训练模型数据添加
    void train_data_add(std::vector<cv::Mat> face, std::vector<int> label);
    //实例化
    static TrainModel* Instance();
private:
    cv::FileStorage m_fs;
    cv::Ptr<cv::face::EigenFaceRecognizer> m_model;
    std::thread m_thread;
    std::atomic<bool> m_model_state;
    std::queue<std::pair<std::vector<cv::Mat>, std::vector<int>>> m_queue; 
};

#endif

