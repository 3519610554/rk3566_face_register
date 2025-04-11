#include "Web.h"
#include <iostream>

#define FLASK_HOST      "10.34.116.127:5000"
#define FLASK_URL       "http://" FLASK_HOST "/upload"

Web::Web(){

    curl_global_init(CURL_GLOBAL_DEFAULT);
    m_curl = curl_easy_init();
}

Web::~Web(){

    if (m_curl) {
        curl_easy_cleanup(m_curl);
        m_curl = nullptr;
    }
    curl_global_cleanup();
}

std::vector<uchar> Web::mat_to_buffer(const cv::Mat& img){

    std::vector<uchar> buf;
    cv::imencode(".jpg", img, buf);  // 将图像编码为 JPEG 格式
    return buf;
}

void Web::send_image_to_flask(const cv::Mat& img){

    CURLcode res;
    struct curl_mime *mime;
    struct curl_mimepart *part;

    if(m_curl) {
        std::vector<uchar> imgBuffer = mat_to_buffer(img);

        mime = curl_mime_init(m_curl);
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "file");
        curl_mime_filename(part, "image.jpg");
        curl_mime_data(part, reinterpret_cast<const char*>(imgBuffer.data()), imgBuffer.size());
        
        curl_easy_setopt(m_curl, CURLOPT_URL, FLASK_URL);
        curl_easy_setopt(m_curl, CURLOPT_MIMEPOST, mime);

        res = curl_easy_perform(m_curl);

        if(res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

        curl_mime_free(mime);
    }
}

Web* Web::Instance(){

    static Web web;

    return &web;
}
