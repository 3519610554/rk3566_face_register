#include "CaffeModel.h"
#include "opencv2/dnn/dnn.hpp"
#include <string>
#include <spdlog/spdlog.h>
#include <opencv2/core/ocl.hpp>

#define PROTO_PATH      "/root/CMake/model/deploy.prototxt"
#define MODEL_PATH      "/root/CMake/model/res10_300x300_ssd_iter_140000.caffemodel"


CaffeModel* CaffeModel::Instance(){

    static CaffeModel caff_model;

    return &caff_model;
}

void CaffeModel::initialize(){

    m_net = cv::dnn::readNetFromCaffe(PROTO_PATH, MODEL_PATH);
    if (cv::ocl::haveOpenCL()) {
        spdlog::info("OpenCL is available!");
        // cv::ocl::setUseOpenCL(true);
        // m_net.setPreferableBackend(cv::dnn::Backend::DNN_BACKEND_OPENCV);
        // m_net.setPreferableTarget(cv::dnn::Target::DNN_TARGET_CPU);
    } else {
        spdlog::warn("OpenCL is not available!");
    }
}

void CaffeModel::detection_face(cv::Mat image, std::vector<cv::Rect> &objects){

    cv::Scalar scalar(104.0, 177.0, 123.0);
    cv::Mat blob = cv::dnn::blobFromImage(image, 1.0, cv::Size(300, 300), scalar, false, false);

    m_net.setInput(blob);
    cv::Mat detection = m_net.forward();
    cv::Mat detectionMat(detection.size[2], detection.size[3], CV_8U, detection.ptr<float>());

    for (int i = 0; i < detectionMat.rows; i++) {
        float confidence = detectionMat.at<float>(i, 2);

        if (confidence > 0.5) { // 置信度阈值
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * image.cols);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * image.rows);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * image.cols);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * image.rows);

            objects.push_back(cv::Rect(x1, x2, x2 - x1, y2 - y1));
        }
    }

    // cv::UMat uimg = image.getUMat(cv::ACCESS_READ);
    // cv::UMat ublob;
    // cv::dnn::blobFromImage(uimg, ublob,
    //                        1.0, cv::Size(300,300),
    //                        cv::Scalar(104.0,177.0,123.0),
    //                        false, false);
    // m_net.setInput(ublob);
    // cv::UMat uout;
    // try{
    //     m_net.forward(uout);
    // }catch (const cv::Exception& e) {
    //     spdlog::error("OpenCL 调优失败: {}，使用 CPU 回退", e.what());
    // }
    // cv::Mat detection = uout.getMat(cv::ACCESS_READ);  // device→host 拷贝一次
    // // detection 的 shape 为 [1,1,N,7]，类型为 CV_32F
    // int numDet = detection.size[2];
    // int numInfo = detection.size[3];
    // cv::Mat detectionMat(numDet, numInfo, CV_32F, detection.ptr<float>());
    // for (int i = 0; i < numDet; i++) {
    //     float confidence = detectionMat.at<float>(i, 2);
    //     if (confidence > 0.5f) {
    //         int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * image.cols);
    //         int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * image.rows);
    //         int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * image.cols);
    //         int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * image.rows);
    //         int w  = x2 - x1;
    //         int h  = y2 - y1;
    //         objects.emplace_back(x1, y1, w, h);
    //     }
    // }
}
