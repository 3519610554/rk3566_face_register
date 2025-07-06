#ifndef MPP_JEPG_DECODER_H
#define MPP_JEPG_DECODER_H

#include <rk_mpi.h>

class MppJpegDecoder {
public:
    MppJpegDecoder();
    ~MppJpegDecoder();
    bool init(int width, int height);
    bool decode(unsigned char* jpeg_data, size_t jpeg_size, unsigned char* out_bgr);

private:
    MppCtx m_ctx = nullptr;
    MppApi* m_mpi = nullptr;
    int m_width = 0;
    int m_height = 0;
};

#endif
