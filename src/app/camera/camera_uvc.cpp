#include "camera_uvc.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>

void list_input_formats() {
    const AVInputFormat* ifmt = nullptr;
    void* opaque = nullptr;
    printf("Available input formats:\n");
    while ((ifmt = av_demuxer_iterate(&opaque))) {
        printf("  %s\n", ifmt->name);
    }
}

CameraUvc::CameraUvc(){
}

CameraUvc::~CameraUvc(){
    cv::destroyAllWindows();
}

CameraUvc* CameraUvc::Instance(){

    static CameraUvc camera;

    return &camera;
}

void CameraUvc::initialize(std::string yaml_path){
    //解析配置
    YAML::Node cfg = YAML::LoadFile(yaml_path)["camera"];
    std::string dev = cfg["id"].as<std::string>();
    m_width  = cfg["width"].as<int>();
    m_height = cfg["height"].as<int>();

    int ret;
	long long ts_mark = 0;
    const char *input_stream_url = dev.c_str();

    avdevice_register_all();
    avformat_network_init();
    av_log_set_level(AV_LOG_INFO);
    AVDictionary *opts = nullptr;
	// av_dict_set(&opts, "rtsp_transport", "+udp+tcp", 0);
	// av_dict_set(&opts, "rtsp_flags", "+prefer_tcp", 0);
	// av_dict_set(&opts, "threads", "auto", 0);
    av_dict_set(&opts, "video_size", "640x480", 0);
    av_dict_set(&opts, "framerate", "30", 0);

    // format_context_input = avformat_alloc_context();
    // ret = avformat_open_input(&format_context_input, input_stream_url, nullptr, &opts);
    const AVInputFormat* input_fmt = av_find_input_format("v4l2");
    ret = avformat_open_input(&format_context_input, input_stream_url, input_fmt, &opts);
    if (ret < 0) {
		spdlog::warn("avformat_open_input filed: {}", ret);
		return;
	}
    ret = avformat_find_stream_info(format_context_input, nullptr);
	if (ret < 0) {
		spdlog::warn("avformat_find_stream_info filed: {}", ret);
		return;
	}

    av_dump_format(format_context_input, 0, input_stream_url, 0);

    //视频部分
    video_stream_index_input = av_find_best_stream(format_context_input, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (video_stream_index_input < 0) {
        spdlog::warn("Could not find video stream");
        return;
    }

    AVStream *video_stream = format_context_input->streams[video_stream_index_input];
    
    const AVCodec *codec_input_video = avcodec_find_decoder(video_stream->codecpar->codec_id);
    if (codec_input_video == NULL) {
        spdlog::warn("avcodec_find_decoder_by_name failed...");
        return;
    }

    codec_ctx_input_video = avcodec_alloc_context3(codec_input_video);
    if (codec_input_video == NULL) {
        spdlog::warn("avcodec_alloc_context3 failed...");
        return;
    }

    ret = avcodec_parameters_to_context(codec_ctx_input_video, video_stream->codecpar);
    if (ret < 0) {
        spdlog::warn("avcodec_parameters_to_context for video failed");
        return;
    }
    codec_ctx_input_video->pix_fmt = AV_PIX_FMT_DRM_PRIME;
    codec_ctx_input_video->lowres = codec_input_video->max_lowres;
    codec_ctx_input_video->flags2 |= AV_CODEC_FLAG2_FAST;

    spdlog::info("video decoder name: {}", codec_input_video->name);

    // 打开解码器
    AVDictionary *video_opts = nullptr;
    av_dict_set(&video_opts, "strict", "1", 0);
    ret = avcodec_open2(codec_ctx_input_video, codec_input_video, &video_opts);
    av_dict_free(&video_opts);
    if (ret < 0) {
        spdlog::warn("avcodec_open2 for video failed: {}", ret);
        return;
    }

    // ==== 音频部分 ====
    audio_stream_index_input = av_find_best_stream(format_context_input, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (audio_stream_index_input >= 0) {
        AVStream *audio_stream = format_context_input->streams[audio_stream_index_input];
        const AVCodec *codec_input_audio = avcodec_find_decoder(audio_stream->codecpar->codec_id);
        if (!codec_input_audio) {
            spdlog::warn("avcodec_find_decoder for audio failed");
            return;
        }

        codec_ctx_input_audio = avcodec_alloc_context3(codec_input_audio);
        if (!codec_ctx_input_audio) {
            spdlog::warn("avcodec_alloc_context3 for audio failed");
            return;
        }

        ret = avcodec_parameters_to_context(codec_ctx_input_audio, audio_stream->codecpar);
        if (ret < 0) {
            spdlog::warn("avcodec_parameters_to_context for audio failed");
            return;
        }

        ret = avcodec_open2(codec_ctx_input_audio, codec_input_audio, nullptr);
        if (ret < 0) {
            spdlog::warn("avcodec_open2 for audio failed: {}", ret);
            return;
        }
    }

    /* Init Mat */
    size_t buffer_size = m_width * m_height * 3;
    drm_buf_for_rga2.drm_buf_ptr = (uint8_t*)malloc(buffer_size);
    if (drm_buf_for_rga2.drm_buf_ptr == nullptr) {
        spdlog::error("malloc drm_buf_for_rga2.drm_buf_ptr failed");
        return;
    }
    mat4show = new cv::Mat(cv::Size(m_width, m_height), CV_8UC3, drm_buf_for_rga2.drm_buf_ptr);
    packet_input_tmp = av_packet_alloc();
	frame_input_tmp = av_frame_alloc(); 
}

bool CameraUvc::frame_get(cv::Mat &frame) {
    int ret = av_read_frame(format_context_input, packet_input_tmp);
    if (ret < 0) {
        // 读取帧失败，通常是流结束或者出错
        return false;
    }

    if (packet_input_tmp->stream_index == video_stream_index_input) {
        video_frame_size += packet_input_tmp->size;
        video_frame_count++;

        // 发送packet给解码器
        ret = avcodec_send_packet(codec_ctx_input_video, packet_input_tmp);
        if (ret < 0) {
            printf("avcodec_send_packet failed: %d\n", ret);
            av_packet_unref(packet_input_tmp);
            return false;
        }

        while (ret >= 0) {
            ret = avcodec_receive_frame(codec_ctx_input_video, frame_input_tmp);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                // 需要更多packet或已结束，退出解码循环
                break;
            } else if (ret < 0) {
                printf("avcodec_receive_frame failed: %d\n", ret);
                break;
            }

            // 解码成功得到frame_input_tmp，数据是 DRM PRIME 格式，需要转换
            auto av_drm_frame = reinterpret_cast<const AVDRMFrameDescriptor *>(frame_input_tmp->data[0]);
            auto layer = &reinterpret_cast<AVDRMFrameDescriptor *>(frame_input_tmp->data[0])->layers[0];
            
            int fd = av_drm_frame->objects[0].fd;
            int w = layer->planes[0].pitch;
            int h = layer->planes[1].offset / w;

            // 使用 RGA 做格式转换和缩放，转换为BGR格式，输出到 drm_buf_for_rga2.drm_buf_fd 指向的内存
            rknn_img_resize_phy_to_phy(&rga_ctx,
                                       fd, w, h, RK_FORMAT_YCbCr_420_SP,  // 输入格式 YUV420SP
                                       drm_buf_for_rga2.drm_buf_fd, m_width, m_height, RK_FORMAT_BGR_888); // 输出格式 BGR888
            
            spdlog::info("RGA input w={}, h={}, pitch={}", w, h, layer->planes[0].pitch);

            // 现在 mat4show 已指向 drm_buf_for_rga2.drm_buf_ptr ，数据是最新的 BGR 图像
            // 将 mat4show 拷贝给参数 frame
            if (drm_buf_for_rga2.drm_buf_ptr == nullptr) {
                spdlog::error("drm_buf_for_rga2.drm_buf_ptr is nullptr! Memory not initialized.");
                break;
            }
            frame = mat4show->clone();

            // 只取第一帧解码结果，跳出循环（或者你需要可以处理所有解码帧）
            break;
        }
    }

    av_packet_unref(packet_input_tmp);
    av_frame_unref(frame_input_tmp);

    return true;
}

