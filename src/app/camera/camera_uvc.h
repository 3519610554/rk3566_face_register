#ifndef CAMERAUVC_H
#define CAMERAUVC_H

#include <OpencvPublic.h>
#include <turbojpeg.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/hwcontext_drm.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#ifdef __cplusplus
}
#endif

#include "drm_func.h"
#include "rga_func.h"

struct drm_buf {
	int drm_buf_fd = -1;
	unsigned int drm_buf_handle;
	void *drm_buf_ptr = NULL;
	size_t drm_buf_size = 0;
};

class CameraUvc{
public:
    CameraUvc();
    ~CameraUvc();
    //实例化
    static CameraUvc* Instance();
    //初始化
    void initialize(std::string yaml_path);
    //获取帧照片
    bool frame_get(cv::Mat &frame);
private:
    tjhandle         m_tjh{nullptr};
    // V4L2设备文件描述符
    int m_fd = -1;                    
    struct buffer {
        void* start;
        size_t length;
    };
    std::vector<buffer> m_buffers;
    std::vector<unsigned char> m_dstBuf;
    unsigned int m_buffer_count;
    int m_width;
    int m_height;
    cv::Mat m_frameMat;

    // /* ffmpeg */
	/* ffmpeg */
	AVFormatContext *format_context_input;
	int video_stream_index_input;
	int audio_stream_index_input;

	AVCodec *codec_input_video;
	AVCodec *codec_input_audio;

	AVCodecContext *codec_ctx_input_video;
	AVCodecContext *codec_ctx_input_audio;

	int video_frame_size = 0;
	int audio_frame_size = 0;
	int video_frame_count = 0;
	int audio_frame_count = 0;

	drm_context drm_ctx;
	rga_context rga_ctx;
	struct drm_buf drm_buf_for_rga1;
	struct drm_buf drm_buf_for_rga2;

    AVPacket *packet_input_tmp;
    AVFrame *frame_input_tmp;

    cv::Mat *mat4show;
};

#endif
