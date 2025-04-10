#include "Web.h"
#include <curl/curl.h>
#include <iostream>

#define FLASK_HOST      "10.34.116.127:5000"
#define FLASK_URL       "http://" FLASK_HOST "/upload"

std::vector<uchar> util::Web::mat_to_buffer(const cv::Mat& img){

    std::vector<uchar> buf;
    cv::imencode(".jpg", img, buf);  // 将图像编码为 JPEG 格式
    return buf;
}

void util::Web::send_image_to_flask(const cv::Mat& img){

    CURL *curl;
    CURLcode res;
    struct curl_mime *mime;
    struct curl_mimepart *part;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        std::cout << "curl init successfuly" << std::endl;
        // 设置 Flask 后端的 URL
        const char* url = FLASK_URL;

        // 将 OpenCV 图像转换为字节流
        std::vector<uchar> imgBuffer = mat_to_buffer(img);

        mime = curl_mime_init(curl);
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "file");
        curl_mime_filename(part, "image.jpg");
        curl_mime_data(part, reinterpret_cast<const char*>(imgBuffer.data()), imgBuffer.size());
        
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        // 设置 cURL 请求
        // curl_easy_setopt(curl, CURLOPT_URL, url);
        // curl_easy_setopt(curl, CURLOPT_POST, 1L);
        // curl_easy_setopt(curl, CURLOPT_POSTFIELDS, imgBuffer.data());
        // curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, imgBuffer.size());

        // 执行请求
        res = curl_easy_perform(curl);

        if(res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

