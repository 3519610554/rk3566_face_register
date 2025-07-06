#include "MppJpegDecoder.h"
#include <OpencvPublic.h>
#include <rk_mpi_cmd.h>
#include <cstring>

MppJpegDecoder::MppJpegDecoder() {}

MppJpegDecoder::~MppJpegDecoder() {
    if (m_ctx) {
        mpp_destroy(m_ctx);
        m_ctx = nullptr;
    }
}

bool MppJpegDecoder::init(int width, int height) {
    m_width = width;
    m_height = height;

    if (mpp_create(&m_ctx, &m_mpi) != MPP_OK) return false;
    if (mpp_init(m_ctx, MPP_CTX_DEC, MPP_VIDEO_CodingMJPEG) != MPP_OK) return false;

    return true;
}


bool MppJpegDecoder::decode(unsigned char* jpeg_data, size_t jpeg_size, unsigned char* out_bgr) {
    MppPacket packet = nullptr;
    MppFrame frame = nullptr;

    mpp_packet_init(&packet, jpeg_data, jpeg_size);
    mpp_packet_set_pos(packet, jpeg_data);
    mpp_packet_set_length(packet, jpeg_size);
    mpp_packet_set_eos(packet);

    if (m_mpi->decode_put_packet(m_ctx, packet) != MPP_OK) {
        mpp_packet_deinit(&packet);
        return false;
    }

    while (true) {
        if (m_mpi->decode_get_frame(m_ctx, &frame) != MPP_OK) break;
        if (!frame) break;

        if (mpp_frame_get_errinfo(frame)) continue;

        MppBuffer buf = mpp_frame_get_buffer(frame);
        if (!buf) continue;

        uint8_t* yuv = (uint8_t*)mpp_buffer_get_ptr(buf);

        // NV12 YUV图像的高度是1.5倍
        int yuv_height = m_height * 3 / 2;

        // 创建 OpenCV Mat 指向YUV数据，格式CV_8UC1 单通道
        cv::Mat yuv_img(yuv_height, m_width, CV_8UC1, yuv);

        // 创建BGR图像Mat
        cv::Mat bgr_img;

        // 转换NV12到BGR
        cv::cvtColor(yuv_img, bgr_img, cv::COLOR_YUV2BGR_NV12);

        // 拷贝转换后的数据到 out_bgr
        memcpy(out_bgr, bgr_img.data, m_width * m_height * 3);

        break;
    }

    mpp_packet_deinit(&packet);
    return true;
}