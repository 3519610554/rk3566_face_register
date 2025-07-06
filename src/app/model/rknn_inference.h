#ifndef _RNKK_INFERENCE_H
#define _RNKK_INFERENCE_H

#include <rknn_api.h>
#include "OpencvPublic.h"
#include <vector>
#include <string>
#include "retinaface.h"
#include "rknn_clip_utils.h"
#include "clip_tokenizer.h"

class RnkkInference{
public:
    RnkkInference();
    ~RnkkInference();
    static RnkkInference* Instance();
    void initialize(std::string yaml_path);
    void detection_face(cv::Mat image, std::vector<cv::Rect> &object);
protected:
    void cvMat_to_imageBuffer(cv::Mat& mat, image_buffer_t* image);
private:
    rknn_app_context_t m_rknn_app_ctx;
    std::string m_model_path;
    int m_width;
    int m_height;
};

#endif
