#include <spdlog/spdlog.h>
#include "thread_pool.h"
#include "camera_uvc.h"
#include "train_model.h"
#include "face_detection.h"
#include "web_connect.h"
#include "face_task.h"

int main(int argc, char* argv[]) {

    std::string yaml_path = "../config/app.yaml";

    if (argc > 1){
        yaml_path = argv[1];
        spdlog::info("config yaml path: {}", yaml_path);
    }

    ThreadPool::Instance()->initialize(10);
    CameraUvc::Instance()->initialize(yaml_path);
    WebConnect::Instance()->initialize(yaml_path);
    TrainModel::Instance()->initialize(yaml_path);
    FaceDetection::Instance()->initialize(yaml_path);
    FaceTask::Instance()->initialize();

    ThreadPool::Instance()->start();
    ThreadPool::Instance()->wait();

    return 0;
}



// #include <iostream>
// #include <opencv2/opencv.hpp>

// extern "C" {
// #include <libavcodec/avcodec.h>
// #include <libavformat/avformat.h>
// #include <libswscale/swscale.h>
// #include <libavutil/imgutils.h>
// #include <libavutil/opt.h>
// #include <libavutil/dict.h>
// }

// int main() {
//     // ---------- 1. 打开摄像头 ----------
//     cv::VideoCapture cap(1, cv::CAP_V4L2);
//     cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
//     cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
//     cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
//     cap.set(cv::CAP_PROP_FPS, 30);

//     if (!cap.isOpened()) {
//         std::cerr << "摄像头打开失败！" << std::endl;
//         return -1;
//     }

//     int width = 640, height = 480, fps = 30;

//     // ---------- 2. 初始化编码器（h264_rkmpp） ----------
//     const AVCodec *codec = avcodec_find_encoder_by_name("h264_rkmpp");
//     if (!codec) {
//         std::cerr << "未找到编码器 h264_rkmpp" << std::endl;
//         return -1;
//     }

//     AVCodecContext *enc_ctx = avcodec_alloc_context3(codec);
//     enc_ctx->width = width;
//     enc_ctx->height = height;
//     enc_ctx->pix_fmt = AV_PIX_FMT_NV12;
//     enc_ctx->time_base = AVRational{1, fps};
//     enc_ctx->framerate = AVRational{fps, 1};
//     enc_ctx->bit_rate = 2000000;
//     enc_ctx->gop_size = 30;

//     AVDictionary* param = nullptr;
//     av_dict_set(&param, "profile", "main", 0);
//     av_dict_set(&param, "preset", "ultrafast", 0);
//     av_dict_set_int(&param, "rc_mode", 0, 0);  // CBR 模式

//     if (avcodec_open2(enc_ctx, codec, &param) < 0) {
//         std::cerr << "编码器打开失败" << std::endl;
//         return -1;
//     }
//     av_dict_free(&param);

//     // ---------- 3. 初始化输出 RTSP ----------
//     AVFormatContext *fmt_ctx = nullptr;
//     avformat_alloc_output_context2(&fmt_ctx, nullptr, "rtsp", "rtsp://127.0.0.1:8554/live");

//     if (!fmt_ctx) {
//         std::cerr << "RTSP 输出上下文创建失败" << std::endl;
//         return -1;
//     }

//     AVStream *video_st = avformat_new_stream(fmt_ctx, nullptr);
//     video_st->codecpar->codec_id = AV_CODEC_ID_H264;
//     video_st->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
//     video_st->codecpar->width = enc_ctx->width;
//     video_st->codecpar->height = enc_ctx->height;
//     video_st->codecpar->format = enc_ctx->pix_fmt;
//     video_st->time_base = enc_ctx->time_base;

//     if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
//         if (avio_open(&fmt_ctx->pb, fmt_ctx->url, AVIO_FLAG_WRITE) < 0) {
//             std::cerr << "无法打开 RTSP 输出 URL" << std::endl;
//             return -1;
//         }
//     }

//     if (avformat_write_header(fmt_ctx, nullptr) < 0) {
//         std::cerr << "写入 RTSP 头部失败" << std::endl;
//         return -1;
//     }

//     // ---------- 4. 初始化 Frame 和 SwsContext ----------
//     AVFrame *frame = av_frame_alloc();
//     frame->format = enc_ctx->pix_fmt;
//     frame->width = enc_ctx->width;
//     frame->height = enc_ctx->height;
//     if (av_frame_get_buffer(frame, 32) < 0) {
//         std::cerr << "frame buffer 分配失败" << std::endl;
//         return -1;
//     }

//     SwsContext *sws_ctx = sws_getContext(
//         width, height, AV_PIX_FMT_BGR24,
//         width, height, AV_PIX_FMT_NV12,
//         SWS_BICUBIC, nullptr, nullptr, nullptr
//     );

//     AVPacket *pkt = av_packet_alloc();
//     int64_t pts = 0;

//     // ---------- 5. 主循环 ----------
//     cv::Mat img;
//     while (true) {
//         cap >> img;
//         if (img.empty()) break;

//         // BGR 转 NV12
//         uint8_t* src_slices[1] = { img.data };
//         int src_stride[1] = { static_cast<int>(img.step) };
//         sws_scale(sws_ctx, src_slices, src_stride, 0, height, frame->data, frame->linesize);

//         frame->pts = pts++;

//         if (avcodec_send_frame(enc_ctx, frame) < 0) continue;

//         while (avcodec_receive_packet(enc_ctx, pkt) == 0) {
//             pkt->stream_index = video_st->index;
//             pkt->pts = av_rescale_q(pkt->pts, enc_ctx->time_base, video_st->time_base);
//             pkt->dts = pkt->pts;
//             pkt->duration = av_rescale_q(pkt->duration, enc_ctx->time_base, video_st->time_base);
//             av_interleaved_write_frame(fmt_ctx, pkt);
//             av_packet_unref(pkt);
//         }
//     }

//     // ---------- 6. 清理 ----------
//     av_write_trailer(fmt_ctx);
//     avcodec_free_context(&enc_ctx);
//     av_frame_free(&frame);
//     av_packet_free(&pkt);
//     sws_freeContext(sws_ctx);
//     if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
//         avio_close(fmt_ctx->pb);
//     }
//     avformat_free_context(fmt_ctx);

//     return 0;
// }
