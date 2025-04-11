#ifndef _WEB_H
#define _WEB_H

#include "OpencvPublic.h"
#include <vector>
#include <curl/curl.h>

class Web{
public:
    Web();
    ~Web();
    void send_image_to_flask(const cv::Mat& img);
    static Web* Instance();
protected:
    std::vector<uchar> mat_to_buffer(const cv::Mat& img);
private:
    CURL* m_curl;
};

#endif
