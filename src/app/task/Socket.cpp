#include "Socket.h"
#include <algorithm>
#include <exception>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>
#include <cstdio>
#include <spdlog/spdlog.h>
#include <netinet/tcp.h> 
#include <yaml-cpp/yaml.h>
#include "ThreadPool.h"
#include "HashBase62.h"
#include "node/node.h"
#include "node/parse.h"

Socket::Socket(){
    
}

Socket::~Socket(){
    
    for (auto& client : m_clients){
        shutdown(client, SHUT_RDWR);
        close(client);
    }
    shutdown(m_sock, SHUT_RDWR);
    close(m_sock);
}

Socket* Socket::Instance(){

    static Socket socket_client;

    return &socket_client;
}

void Socket::initialize(std::string yaml_path){

    YAML::Node socker = YAML::LoadFile(yaml_path);
    m_local_port = socker["local_port"].as<int>();
    m_chunk_size = socker["local_port"].as<int>();

    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    m_sa.sin_family = AF_INET;
    m_sa.sin_port = htons(m_local_port);
    set_keepalive();
    if (bind(m_sock, (struct sockaddr*)&m_sa, sizeof(m_sa)) < 0){
        spdlog::error("bind failed");
        close(m_sock);
        return;
    }
    if (listen(m_sock, 2) < 0){
        spdlog::error("listen failed");
        close(m_sock);
        return;
    }
    ThreadPool::Instance()->enqueue(std::bind(&Socket::connect_thread, this));
    ThreadPool::Instance()->enqueue(std::bind(&Socket::send_thread, this));
}

bool Socket::sned_data_add(int sockfd, json json_data){

    if (m_clients.empty())
        return true;
    m_send_queue.push(std::make_pair(sockfd, json_data));
    return false;
}

void Socket::data_subpackage(int sockfd, json data){

    std::string json_str = data.dump();
    size_t total_size = json_str.size();
    size_t chunk_size = m_chunk_size;
    size_t num_chunks = (total_size + chunk_size - 1) / chunk_size;
    std::string hash_base62 = util::generateTimeHashString();

    spdlog::info("send hash: {}", hash_base62);

    for (size_t i = 0; i < num_chunks; i ++){
        json send_json;
        size_t offset = i * chunk_size;
        size_t current_chunk_size = std::min(chunk_size, total_size - offset);
        std::string chunk_data = json_str.substr(offset, current_chunk_size);

        send_json["Hash"] = hash_base62;
        send_json["NumChunks"] = num_chunks;
        send_json["CurrentBlockNum"] = i + 1;
        send_json["Payload"] = chunk_data;
        sned_data_add(sockfd, send_json);
    }
}

void Socket::connect_sucessfly_func_bind(std::function<void(int)> func){

    m_connect_func = func;
}

void Socket::recv_cmd_func_bind(std::string cmd, std::function<void(int, json)> func){

    m_cmd_func[cmd] = func;
}

void Socket::set_keepalive(int keep_idle, int keep_interval, int keep_count){

    int yes = 1;
    int opt = 1;
    setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(m_sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes));
    setsockopt(m_sock, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(keep_idle));
    setsockopt(m_sock, IPPROTO_TCP, TCP_KEEPINTVL, &keep_interval, sizeof(keep_interval));
    setsockopt(m_sock, IPPROTO_TCP, TCP_KEEPCNT, &keep_count, sizeof(keep_count));
    
}

void Socket::client_thread_erase(int client_id){

    auto it = std::find(m_clients.begin(), m_clients.end(), client_id);
    if (it != m_clients.end()) {
        m_clients.erase(it);
    }
}

void Socket::socket_send(int sockfd, json json_data){

    std::string json_str = json_data.dump();
    uint32_t msg_len = htonl(json_str.size());

    ssize_t state = 0;
    state = send(sockfd, &msg_len, sizeof(msg_len), 0);
    state = send(sockfd, json_str.c_str(), json_str.size(), 0);
    if (!state){
        close(sockfd);   
        client_thread_erase(sockfd);
    }
}

bool Socket::recv_exact(int sockfd, void *buffer, size_t length){

    size_t total_received = 0;
    char *buf = static_cast<char*>(buffer);

    while(total_received < length){
        ssize_t bytes = recv(sockfd, buf+total_received, length-total_received, 0);
        if (bytes <= 0){
            close(sockfd);
            client_thread_erase(sockfd);
            return true;
        }
        total_received += bytes;
    }

    return false;
}

bool Socket::receive_json_message(int sockfd, json &out_json){

    uint32_t len_net = 0;
    
    if (recv_exact(sockfd, &len_net, sizeof(len_net))){
        spdlog::error("failed to read length.");
        return true;
    }

    uint32_t msg_len = ntohl(len_net);
    std::vector<char> buffer(msg_len);
    if (recv_exact(sockfd, buffer.data(), msg_len)){
        spdlog::error("failed to read full JSON message.");
        return true;
    }

    try{
        std::string json_str(buffer.begin(), buffer.end());
        out_json = json::parse(json_str);
    }catch(const std::exception& e){
        spdlog::error("JSON parse error: {}", e.what());
        return true;
    }

    return false;
}

void Socket::receive_json_unpack(int sockfd, json recv_json){
    std::string hash_str = recv_json["Hash"];
    std::string payload = recv_json["Payload"];
    int num_chunks = recv_json["NumChunks"];
    int current_block_num = recv_json["CurrentBlockNum"];

    if (m_recv_dict_buff.find(hash_str) == m_recv_dict_buff.end()){
        spdlog::info("recv hash: {}", hash_str);
        m_recv_dict_buff[hash_str] = "";
    }

    m_recv_dict_buff[hash_str] += payload;
    spdlog::debug("json_recv: {}", recv_json.dump());
    if (num_chunks != current_block_num)
        return;
    json json_obj = json::parse(m_recv_dict_buff[hash_str]);
    m_recv_dict_buff.erase(hash_str);
    spdlog::debug("json: {}", json_obj.dump());
    auto it = m_cmd_func.find(json_obj["Cmd"]);
    if (it != m_cmd_func.end()) {
        it->second(sockfd, json_obj);
    }
}

void Socket::connect_thread(){

    spdlog::info("connect thread started");

    while(ThreadPool::Instance()->get_thread_state()){
        int client_id = accept(m_sock, nullptr, nullptr);
        if (client_id <= 0)
            continue;
        spdlog::info("successfuly to client: {}", client_id);
        m_clients.push_back(client_id);
        ThreadPool::Instance()->enqueue(&Socket::receive_thread, this, client_id);
        if (m_connect_func)
            m_connect_func(client_id);
    }
}

void Socket::send_thread(){

    spdlog::info("server web send data thread started");

    while(ThreadPool::Instance()->get_thread_state()){
        std::pair<int, json> data = m_send_queue.pop();
        if (data.first == CLIENT_ALL){
            for (auto& client : m_clients){
                socket_send(client, data.second);
            }
        }else {
            socket_send(data.first, data.second);
        }
    }
}

void Socket::receive_thread(int client_id){

    spdlog::info("{} receive thread started!", client_id);

    while(ThreadPool::Instance()->get_thread_state()){
        json recv_json;
        if (receive_json_message(client_id, recv_json))
            return;
        receive_json_unpack(client_id, recv_json);
    }
}
