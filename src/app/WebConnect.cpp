#include "WebConnect.h"
#include <iostream>
#include <unistd.h>
#include <json.hpp>
#include <cstdint>
#include "Base64.h"
#include "LocalTime.h"
#include "Socket.h"
#include "FaceDetection.h"

#define CHUNK_SIZE                  512

using json = nlohmann::json;

WebConnect::WebConnect(){

    Socket::Instance()->recv_cmd_func_bind("TypeIn", 
        [this](json json_data) { this->type_in_recv_func(json_data); });
}

WebConnect::~WebConnect(){

    
}


WebConnect* WebConnect::Instance(){

    static WebConnect web_connect;

    return &web_connect;
}

void WebConnect::initialize(){

    Socket::Instance()->initialize();
    
}

void WebConnect::start(){

    Socket::Instance()->start();
}

void WebConnect::send_image(const cv::Mat& img){

    std::string imgBase64 = util::encodeBase64(mat_to_buffer(img));
    std::string current_time = util::LocalTime::get_cuurent_time();
    json json_data;
    json_data["Time"] = current_time;
    json_data["ImgBase64"] = imgBase64;
    std::string json_str = json_data.dump();

    data_subpackage("upload", json_str);
}

void WebConnect::data_subpackage(std::string cmd, std::string data){

    size_t total_size = data.size();
    size_t chunk_size = CHUNK_SIZE;
    size_t num_chunks = (total_size + chunk_size - 1) / chunk_size;

    for (size_t i = 0; i < num_chunks; i ++){
        json send_json;
        size_t offset = i * chunk_size;
        size_t current_chunk_size = std::min(chunk_size, total_size - offset);
        std::string chunk_data = data.substr(offset, current_chunk_size);

        send_json["Cmd"] = cmd;
        send_json["Data"]["NumChunks"] = num_chunks;
        send_json["Data"]["CurrentBlockNum"] = i + 1;
        send_json["Data"]["Payload"] = chunk_data;
        Socket::Instance()->sned_data_add(send_json);
    }
}

void WebConnect::type_in_recv_func(json json_data){

    std::string name = json_data["Data"]["Name"];
    FaceDetection::Instance()->enroll_face(name);
}

std::string WebConnect::mat_to_buffer(const cv::Mat& img){

    std::vector<uchar> buf;
    cv::imencode(".jpg", img, buf);  // 将图像编码为 JPEG 格式
    return std::string(reinterpret_cast<char*>(buf.data()), buf.size());
}