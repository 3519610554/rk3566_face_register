#ifndef _WEB_H
#define _WEB_H

#include "OpencvPublic.h"
#include <vector>

namespace util {
class Web{
public:
    static void send_image_to_flask(const cv::Mat& img);
protected:
    static std::vector<uchar> mat_to_buffer(const cv::Mat& img);
private:

};
}

#endif
