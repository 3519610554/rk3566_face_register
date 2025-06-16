#include "RnkkInference.h"
#include "spdlog.h"
#include "File.h"
#include <spdlog/spdlog.h>
#include <string>
#include <fstream>
#include "image_utils.h"
#include "image_drawing.h"
#include "file_utils.h"

#define RNKK_MODEL_PATH     "../assets/model/RetinaFace_mobile320.rknn"
#define RNKK_WIDTH          640
#define RNKK_HEIGHT         640

RnkkInference::RnkkInference(){

    memset(&m_rknn_app_ctx, 0, sizeof(rknn_app_context_t));
}

RnkkInference::~RnkkInference(){

    int ret = release_retinaface_model(&m_rknn_app_ctx);
    if (ret != 0) {
        spdlog::error("release_retinaface_model fail! ret= {}", ret);
    }
}

RnkkInference* RnkkInference::Instance(){

    static  RnkkInference rnkk;

    return &rnkk;
}

void RnkkInference::initialize(){

    int ret = init_retinaface_model(RNKK_MODEL_PATH, &m_rknn_app_ctx);
    if (ret != 0){
        spdlog::error("init_retinaface_model fail! ret={} model_path={}", ret, RNKK_MODEL_PATH);
    }
}

void RnkkInference::detection_face(cv::Mat image, std::vector<cv::Rect> &object){

    image_buffer_t src_image;
    memset(&src_image, 0, sizeof(image_buffer_t));
    cvMat_to_imageBuffer(image, &src_image);
    
    retinaface_result result;
    int ret = inference_retinaface_model(&m_rknn_app_ctx, &src_image, &result);
    if (ret != 0) {
        spdlog::error("init_retinaface_model fail! ret={}", ret);
        goto out;
    }
    for (int i = 0; i < result.count; ++i) {
        int rx = result.object[i].box.left;
        int ry = result.object[i].box.top;
        int rw = result.object[i].box.right - result.object[i].box.left;
        int rh = result.object[i].box.bottom - result.object[i].box.top;
        object.push_back(cv::Rect(rx, ry, rw, rh));
        // draw_rectangle(&src_image, rx, ry, rw, rh, COLOR_GREEN, 3);
        // char score_text[20];
        // snprintf(score_text, 20, "%0.2f", result.object[i].score);
        // printf("face @(%d %d %d %d) score=%f\n", result.object[i].box.left, result.object[i].box.top,
        //        result.object[i].box.right, result.object[i].box.bottom, result.object[i].score);
        // draw_text(&src_image, score_text, rx, ry, COLOR_RED, 20);
        // for(int j = 0; j < 5; j++) {
        //     draw_circle(&src_image, result.object[i].ponit[j].x, result.object[i].ponit[j].y, 2, COLOR_ORANGE, 4);
        // }
    }
    out:
    if (src_image.virt_addr != NULL) {
        free(src_image.virt_addr);
    }
}

void RnkkInference::cvMat_to_imageBuffer(cv::Mat& mat, image_buffer_t* image){

    if (image == nullptr) return;

    memset(image, 0, sizeof(image_buffer_t));
    image->width = mat.cols;
    image->height = mat.rows;
    image->format = IMAGE_FORMAT_RGB888;
    image->size = get_image_size(image);
    image->virt_addr = (unsigned char*)malloc(image->size);
    if (image->virt_addr == NULL) {
        spdlog::error("malloc buffer size:{} fail!", image->size);
        return;
    }
    memcpy((void *)image->virt_addr, mat.data, image->size);
}

