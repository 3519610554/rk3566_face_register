#include "WebConnect.h"
#include <iostream>
#include <unistd.h>
#include <json.hpp>
#include <cstdint>
#include "LocalTime.h"
#include "Socket.h"
#include "FaceDetection.h"
#include "BackendSQLite.h"

#define CHUNK_SIZE                  512

using json = nlohmann::json;

WebConnect::WebConnect(){

    Socket::Instance()->connect_sucessfly_func_bind(
        [this](int client_id) {this->connect_successfly_func(client_id);});
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

void WebConnect::send_image(int sockfd, int id, std::string time, std::string imageBase64){
        
    json json_data;
    json_data["Id"] = id;
    json_data["Time"] = time;
    json_data["ImgBase64"] = imageBase64;
    std::string json_str = json_data.dump();

    data_subpackage(sockfd, "upload", json_str);
    std::cout << "send image" << std::endl;
}

void WebConnect::data_subpackage(int sockfd, std::string cmd, std::string data){

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
        Socket::Instance()->sned_data_add(sockfd, send_json);
    }
}

void WebConnect::type_in_recv_func(json json_data){

    std::string name = json_data["Data"]["Name"];
    FaceDetection::Instance()->enroll_face(name);
}

void WebConnect::connect_successfly_func(int client_id){

    std::vector<Backend_Info> data;

    BackendSQLite::Instance()->get_all_data(data);
    for (auto& row : data){
        send_image(client_id, row.id, row.time, row.base64);
    }
    std::cout << "send sql image: " << data.size() << std::endl;
}
