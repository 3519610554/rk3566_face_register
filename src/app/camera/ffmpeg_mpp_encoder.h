#ifndef _FFMPEG_MPP_ENCODER_H
#define _FFMPEG_MPP_ENCODER_H

#include <OpencvPublic.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/dict.h>
}

#ifndef MPP_ALIGN
#define MPP_ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))
#endif

class FFmpegMppEncoder {
public:
    FFmpegMppEncoder();
    ~FFmpegMppEncoder();
    int initialize(int width, int height, int fps);
    void encoder_push_stream(cv::Mat frame);
private:
    struct FFmpegInfo{
        AVCodecContext *enc_ctx;
        AVDictionary* param;
        AVFormatContext *fmt_ctx;
        AVStream *video_st;
        AVFrame *frame;
        SwsContext *sws_ctx;
        AVPacket *pkt;
        int64_t pts;
        int width;
        int height;
        int fps;
    };
private:
    FFmpegInfo m_ffmpeg;
};

#endif
