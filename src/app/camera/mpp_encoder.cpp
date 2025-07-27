#include "mpp_encoder.h"

MppEncoder::MppEncoder(){
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

MppEncoder::~MppEncoder(){
    if (m_grp) {
        mpp_buffer_group_put(m_grp);
        m_grp = nullptr;
    }
    if (m_ctx) {
        mpp_destroy(m_ctx);
        m_ctx = nullptr;
    }
}

void MppEncoder::initialize(int width, int height){
    m_encoder.height = height;
    m_encoder.width = width;

    // 初始化 mpp
    MPP_RET ret = mpp_create(&m_ctx, &m_mpi);
    if (ret != MPP_OK) {
        spdlog::error("Failed to create MPP");
        return;
    }

    ret = mpp_init(m_ctx, MPP_CTX_ENC, MPP_VIDEO_CodingAVC);
    if (ret != MPP_OK) {
        spdlog::error("Failed to init MPP");
        mpp_destroy(m_ctx);
        return;
    }

    // 获取 cfg 并设置参数
    m_mpi->control(m_ctx, MPP_ENC_GET_CFG, &m_cfg);
        if (ret != MPP_OK || m_cfg == nullptr) {
        spdlog::error("Failed to get encoder config");
        mpp_destroy(m_ctx);
        return;
    }

    mpp_enc_cfg_set_s32(m_cfg, "prep:width", m_encoder.width);
    mpp_enc_cfg_set_s32(m_cfg, "prep:height", m_encoder.height);
    mpp_enc_cfg_set_s32(m_cfg, "prep:hor_stride", m_encoder.width);
    mpp_enc_cfg_set_s32(m_cfg, "prep:ver_stride", m_encoder.height);
    mpp_enc_cfg_set_s32(m_cfg, "prep:format", MPP_FMT_YUV420SP);  // NV12

    mpp_enc_cfg_set_s32(m_cfg, "rc:mode", MPP_ENC_RC_MODE_CBR);
    mpp_enc_cfg_set_s32(m_cfg, "rc:fps_in_num", 30);
    mpp_enc_cfg_set_s32(m_cfg, "rc:fps_out_num", 30);
    mpp_enc_cfg_set_s32(m_cfg, "rc:bitrate", 2 * 1024 * 1024);
    mpp_enc_cfg_set_s32(m_cfg, "video:profile", 100); // high

    m_mpi->control(m_ctx, MPP_ENC_SET_CFG, m_cfg);

    // 分配 buffer group
    ret = mpp_buffer_group_get_internal(&m_grp, MPP_BUFFER_TYPE_DRM);
    if (ret != MPP_OK) {
        spdlog::error("Failed to get buffer group");
        mpp_destroy(m_ctx);
        return;
    }

    m_initialized = true;
}

bool MppEncoder::encode(uint8_t* yuv_data, size_t yuv_size, std::vector<uint8_t>& out_h264){
    MPP_RET ret;

    ret = mpp_buffer_get(m_grp, &m_frm_buf, yuv_size);
    if (ret != MPP_OK) {
        spdlog::error("Failed to get m_frame buffer");
        mpp_buffer_group_put(m_grp);
        mpp_destroy(m_ctx);
        return false;
    }

    memcpy(mpp_buffer_get_ptr(m_frm_buf), yuv_data, yuv_size);

    // 填充 m_frame
    mpp_frame_init(&m_frame);
    mpp_frame_set_width(m_frame, m_encoder.width);
    mpp_frame_set_height(m_frame, m_encoder.height);
    mpp_frame_set_fmt(m_frame, MPP_FMT_YUV420SP);
    mpp_frame_set_buffer(m_frame, m_frm_buf);

    m_mpi->encode_put_frame(m_ctx, m_frame);

    // 获取编码数据
    ret = m_mpi->encode_get_packet(m_ctx, &m_packet);
    if (ret == MPP_OK && m_packet) {
        void *ptr  = mpp_packet_get_pos(m_packet);
        size_t len = mpp_packet_get_length(m_packet);
        out_h264.assign((uint8_t *)ptr, (uint8_t *)ptr + len);
        mpp_packet_deinit(&m_packet);
    }

    mpp_frame_deinit(&m_frame);
    mpp_buffer_put(m_frm_buf);mpp_frame_deinit(&m_frame);
    mpp_buffer_put(m_frm_buf);

    return true;
}
