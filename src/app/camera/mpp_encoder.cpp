#include "mpp_encoder.h"

MppEncoder::MppEncoder() {
    m_ctx = nullptr;
    m_mpi = nullptr;
    m_cfg = nullptr;
    m_grp = nullptr;
    m_frm_buf = nullptr;
    m_pkt_buf = nullptr;
    m_frame = nullptr;
    m_packet = nullptr;
    m_initialized = false;
}

MppEncoder::~MppEncoder() {
    if (m_packet) {
        mpp_packet_deinit(&m_packet);
        m_packet = nullptr;
    }
    if (m_frame) {
        mpp_frame_deinit(&m_frame);
        m_frame = nullptr;
    }
    if (m_frm_buf) {
        mpp_buffer_put(m_frm_buf);
        m_frm_buf = nullptr;
    }
    if (m_pkt_buf) {
        mpp_buffer_put(m_pkt_buf);
        m_pkt_buf = nullptr;
    }
    if (m_grp) {
        mpp_buffer_group_put(m_grp);
        m_grp = nullptr;
    }
    if (m_ctx) {
        mpp_destroy(m_ctx);
        m_ctx = nullptr;
    }
}

void MppEncoder::initialize(int width, int height) {
    m_encoder.width = width;
    m_encoder.height = height;

    MPP_RET ret = mpp_create(&m_ctx, &m_mpi);
    if (ret != MPP_OK) {
        spdlog::error("mpp_create failed");
        return;
    }

    ret = mpp_init(m_ctx, MPP_CTX_ENC, MPP_VIDEO_CodingAVC); // H.264
    if (ret != MPP_OK) {
        spdlog::error("mpp_init failed");
        return;
    }

    ret = mpp_enc_cfg_init(&m_cfg);
    if (ret != MPP_OK) {
        spdlog::error("mpp_enc_cfg_init failed");
        return;
    }

    int hor_stride = MPP_ALIGN(width, 16);
    int ver_stride = MPP_ALIGN(height, 16);
    size_t stride = MPP_ALIGN(width, 16);
    size_t frame_size = stride * height * 3 / 2;
    
    mpp_enc_cfg_set_s32(m_cfg, "rc:gop", 30);
    mpp_enc_cfg_set_s32(m_cfg, "prep:width", width);
    mpp_enc_cfg_set_s32(m_cfg, "prep:height", height);
    mpp_enc_cfg_set_s32(m_cfg, "prep:hor_stride", hor_stride);
    mpp_enc_cfg_set_s32(m_cfg, "prep:ver_stride", ver_stride);
    mpp_enc_cfg_set_s32(m_cfg, "prep:format", MPP_FMT_YUV420SP);

    mpp_enc_cfg_set_s32(m_cfg, "rc:mode", MPP_ENC_RC_MODE_AVBR);
    mpp_enc_cfg_set_s32(m_cfg, "rc:bitrate", 20 * 1000 * 1000);
    mpp_enc_cfg_set_s32(m_cfg, "rc:min_bps", 10 * 1000 * 1000);
    mpp_enc_cfg_set_s32(m_cfg, "rc:max_bps", 40 * 1000 * 1000);

    mpp_enc_cfg_set_s32(m_cfg, "rc:fps_in_flex", 0);
    mpp_enc_cfg_set_s32(m_cfg, "rc:fps_in_num", 30);
    mpp_enc_cfg_set_s32(m_cfg, "rc:fps_in_denorm", 1);
    mpp_enc_cfg_set_s32(m_cfg, "rc:fps_out_num", 30);
    mpp_enc_cfg_set_s32(m_cfg, "rc:fps_out_denorm", 1);

    mpp_enc_cfg_set_s32(m_cfg, "codec:type", MPP_VIDEO_CodingAVC);
    mpp_enc_cfg_set_s32(m_cfg, "h264:profile", 77);
    mpp_enc_cfg_set_s32(m_cfg, "h264:level", 51);
    mpp_enc_cfg_set_s32(m_cfg, "h264:cabac_en", 1);
    mpp_enc_cfg_set_s32(m_cfg, "h264:trans8x8", 1);

    ret = m_mpi->control(m_ctx, MPP_ENC_SET_CFG, m_cfg);
    if (ret != MPP_OK) {
        spdlog::error("MPP_ENC_SET_CFG failed");
        return;
    }

    ret = mpp_buffer_group_get_internal(&m_grp, MPP_BUFFER_TYPE_ION);
    if (ret != MPP_OK) {
        spdlog::error("mpp_buffer_group_get_internal failed");
        return;
    }

    ret = mpp_buffer_get(m_grp, &m_frm_buf, frame_size);
    if (ret != MPP_OK) {
        spdlog::error("Failed to get input frame buffer");
        return;
    }

    ret = mpp_frame_init(&m_frame);
    if (ret != MPP_OK) {
        spdlog::error("Failed to init frame");
        return;
    }
    mpp_frame_set_width(m_frame, width);
    mpp_frame_set_height(m_frame, height);
    mpp_frame_set_hor_stride(m_frame, hor_stride);
    mpp_frame_set_ver_stride(m_frame, ver_stride);
    mpp_frame_set_fmt(m_frame, MPP_FMT_YUV420SP);
    mpp_frame_set_buffer(m_frame, m_frm_buf);

    ret = mpp_buffer_get(m_grp, &m_pkt_buf, frame_size);
    if (ret != MPP_OK) {
        spdlog::error("Failed to get packet buffer");
        return;
    }
    ret = mpp_packet_init(&m_packet, mpp_buffer_get_ptr(m_pkt_buf), frame_size);
    if (ret != MPP_OK) {
        spdlog::error("Failed to init packet");
        return;
    }

    m_initialized = true;
    spdlog::info("MPP encoder initialized: {}x{}", width, height);
}

void MppEncoder::bgr_to_nv12(const cv::Mat &bgr, uint8_t *nv12_buffer) {
    int width  = bgr.cols;
    int height = bgr.rows;
    int hor_stride = MPP_ALIGN(width, 16);

    // 1) 转成 I420
    cv::Mat yuv_i420;
    cv::cvtColor(bgr, yuv_i420, cv::COLOR_BGR2YUV_I420);

    uint8_t *y_plane = yuv_i420.data;
    uint8_t *u_plane = y_plane + width * height;
    uint8_t *v_plane = u_plane + (width/2)*(height/2);

    // 2) 拷贝 Y，并在每行后面补齐 padding
    for (int j = 0; j < height; ++j) {
        memcpy(nv12_buffer + j*hor_stride,
               y_plane + j*width,
               width);
        // padding 区域可不管内容
    }

    uint8_t *dst_uv = nv12_buffer + hor_stride*height;
    // 3) 交错 UV，同样每行 padding 到 hor_stride
    for (int j = 0; j < height/2; ++j) {
        uint8_t *row_uv = dst_uv + j*hor_stride;
        for (int i = 0; i < width/2; ++i) {
            row_uv[2*i  ] = u_plane[j*(width/2) + i];
            row_uv[2*i+1] = v_plane[j*(width/2) + i];
        }
    }
}

bool MppEncoder::encode(uint8_t* yuv_data, std::vector<uint8_t>& out_h264) {
    if (!m_initialized) {
        spdlog::error("Encoder not initialized");
        return false;
    }

    void* ptr = mpp_buffer_get_ptr(m_frm_buf);
    size_t buf_size = mpp_buffer_get_size(m_frm_buf);
    memcpy(ptr, yuv_data, buf_size);

    MPP_RET ret = m_mpi->encode_put_frame(m_ctx, m_frame);
    if (ret != MPP_OK) {
        spdlog::error("encode_put_frame failed");
        return false;
    }

    out_h264.clear();

    while (true) {
        ret = m_mpi->encode_get_packet(m_ctx, &m_packet);
        if (ret != MPP_OK || !m_packet)
            break;

        uint8_t* data = (uint8_t*)mpp_packet_get_pos(m_packet);
        size_t length = mpp_packet_get_length(m_packet);
        if (length > 0 && data) {
            out_h264.insert(out_h264.end(), data, data + length);
        }

        mpp_packet_deinit(&m_packet);
        ret = mpp_packet_init(&m_packet, mpp_buffer_get_ptr(m_pkt_buf), mpp_buffer_get_size(m_pkt_buf));
        if (ret != MPP_OK) {
            spdlog::error("Failed to reinit packet");
            return false;
        }
    }

    return !out_h264.empty();
}
