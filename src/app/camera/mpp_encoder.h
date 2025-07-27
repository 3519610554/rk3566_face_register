#ifndef _MPP_ENCODER_H
#define _MPP_ENCODER_H

#include <vector>
#include <cstring>

#include <rockchip/mpp_buffer.h>
#include <rockchip/mpp_frame.h>
#include <rockchip/mpp_packet.h>
#include <rockchip/mpp_task.h>
#include <rockchip/rk_type.h>
#include <rockchip/rk_mpi.h>

#include <spdlog/spdlog.h>

class MppEncoder {
public:
    MppEncoder();
    ~MppEncoder();
    void initialize(int width, int height);

    // 编码一帧 YUV 数据（输入为 YUV420）
    bool encode(uint8_t* yuv_data, size_t yuv_size, std::vector<uint8_t>& out_h264);
private:
    struct EncoderInfo{
        int width;
        int height;
    };
private:
    MppCtx m_ctx;
    MppApi* m_mpi;
    MppEncCfg m_cfg;
    MppBufferGroup m_grp;
    MppBuffer m_frm_buf;
    MppBuffer m_pkt_buf;
    MppFrame m_frame;
    MppPacket m_packet;
    EncoderInfo m_encoder;
    bool m_initialized;
};

#endif
