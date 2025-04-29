#ifndef _CAFFE_MODEL_H
#define _CAFFE_MODEL_H

#include "OpencvPublic.h"


class CaffeModel{
public:
    static CaffeModel* Instance();
    void initialize();
    void detection_face(cv::Mat image, std::vector<cv::Rect> &objects);
private:
    cv::dnn::Net m_net;
};

#endif
