#include "web_connect.h"
#include <iostream>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include <cstdint>
#include <spdlog/spdlog.h>
#include "local_time.h"
#include "socket.h"
#include "face_detection.h"
#include "backend_sqlite.h"


#define CHUNK_SIZE                  512

using json = nlohmann::json;

WebConnect::WebConnect(){

    Socket::Instance()->connect_sucessfly_func_bind(
        [this](int client_id) {this->connect_successfly_func(client_id);});
    Socket::Instance()->recv_cmd_func_bind("TypeIn", 
        [this](int sockfd, json json_data) { this->type_in_recv_func(sockfd, json_data); });
    Socket::Instance()->recv_cmd_func_bind("DeleteImage", 
        [this](int sockfd, json json_data) { this->delete_image_recv_func(sockfd, json_data); });
    Socket::Instance()->recv_cmd_func_bind("NeedImageId", 
        [this](int sockfd, json json_data) { this->need_image_id_func(sockfd, json_data); });
}

WebConnect::~WebConnect(){

    
}

WebConnect* WebConnect::Instance(){

    static WebConnect web_connect;

    return &web_connect;
}

void WebConnect::initialize(std::string yaml_path){

    Socket::Instance()->initialize(yaml_path);
    
    
}

void WebConnect::send_image(int sockfd, int id, std::string time, std::string imageBase64){
        
    json json_data;
    json_data["Cmd"] = "Upload";
    json_data["Id"] = id;
    json_data["Time"] = time;
    json_data["ImgBase64"] = imageBase64;
    std::string json_str = json_data.dump();

    Socket::Instance()->data_subpackage(sockfd, json_data);

    spdlog::info("send image");
}

void WebConnect::send_image_id(int sockfd, json id_arr){

    json json_data;
    json_data["Cmd"] = "UploadId";
    json_data["Id"] = id_arr;
    Socket::Instance()->data_subpackage(sockfd, json_data);

    spdlog::info("send all id");
}

void WebConnect::connect_successfly_func(int client_id){

    std::vector<int> id_data;

    BackendSQLite::Instance()->get_all_id(id_data);
    json j_array = json::array();
    for (auto& row : id_data){
        j_array.push_back(row);
    }
    send_image_id(client_id, j_array);

    spdlog::info("send sql image: {}", id_data.size());
}

void WebConnect::type_in_recv_func(int sockfd, json json_data){

    std::string name = json_data["Name"];
    FaceDetection::Instance()->enroll_face(name);
}

void WebConnect::delete_image_recv_func(int sockfd, json json_data){

    int image_id = json_data["Id"];
    BackendSQLite::Instance()->delete_by_id(image_id);
    spdlog::info("delete image id: {}", image_id);
}

void WebConnect::need_image_id_func(int sockfd, json json_data){

    for (auto &item : json_data["Id"]){
        Backend_Info data;
        if (!BackendSQLite::Instance()->get_data_by_id(item, data))
            continue;
        send_image(sockfd, data.id, data.time, data.base64);
    }
    spdlog::info("photos that need to be synchronized: {}", json_data["Id"].dump());
}
