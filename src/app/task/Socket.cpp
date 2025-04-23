#include "Socket.h"
#include <algorithm>
#include <exception>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>
#include <chrono> 
#include <cstdio>
#include <netinet/tcp.h> 
#include "Task.h"

// #define FLASK_HOST                  "10.34.4.112"
// #define FLASK_PORT                  8080

#define FLASK_HOST                  "10.34.53.17"
#define FLASK_PORT                  8080

Socket::Socket(){

}

Socket::~Socket(){
    
    for (auto& client : m_client_threads){
        close(client.first);
    }
    close(m_sock);
}

Socket* Socket::Instance(){

    static Socket socket_client;

    return &socket_client;
}

void Socket::initialize(){

    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    m_sa.sin_family = AF_INET;
    m_sa.sin_port = htons(FLASK_PORT);
    if (bind(m_sock, (struct sockaddr*)&m_sa, sizeof(m_sa)) < 0){
        perror("bind failed");
        close(m_sock);
        return;
    }
    if (listen(m_sock, 2) < 0){
        perror("listen failed");
        close(m_sock);
        return;
    }
    set_keepalive();
}

void Socket::start(){

    m_thread.push_back(std::thread(&Socket::connect_thread, this));
    m_thread.push_back(std::thread(&Socket::send_thread, this));
}

bool Socket::sned_data_add(int sockfd, json json_data){

    if (m_client_threads.empty())
        return true;
    m_send_queue.push(std::make_pair(sockfd, json_data));
    return false;
}

void Socket::connect_sucessfly_func_bind(std::function<void(int)> func){

    m_connect_func = func;
}

void Socket::recv_cmd_func_bind(std::string cmd, std::function<void(json)> func){

    m_cmd_func[cmd] = func;
}

void Socket::set_keepalive(int keep_idle, int keep_interval, int keep_count){

    int yes = 1;
    setsockopt(m_sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes));
    setsockopt(m_sock, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(keep_idle));
    setsockopt(m_sock, IPPROTO_TCP, TCP_KEEPINTVL, &keep_interval, sizeof(keep_interval));
    setsockopt(m_sock, IPPROTO_TCP, TCP_KEEPCNT, &keep_count, sizeof(keep_count));
}

void Socket::client_thread_erase(int client_id){

    m_client_threads.erase(
        std::remove_if(m_client_threads.begin(), m_client_threads.end(), 
        [client_id](const std::pair<int, std::thread>& client){
            return client.first == client_id;
        })   
    );
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
            return false;
        }
        total_received += bytes;
    }

    return true;
}

bool Socket::receive_json_message(int sockfd, json &out_json){

    uint32_t len_net = 0;
    
    if (!recv_exact(sockfd, &len_net, sizeof(len_net))){
        std::cerr << "failed to read length." << std::endl;
        return true;
    }

    uint32_t msg_len = ntohl(len_net);
    std::vector<char> buffer(msg_len);
    if (!recv_exact(sockfd, buffer.data(), msg_len)){
        std::cerr << "failed to read full JSON message." << std::endl;
        return true;
    }

    try{
        std::string json_str(buffer.begin(), buffer.end());
        out_json = json::parse(json_str);
    }catch(const std::exception& e){
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return true;
    }

    return false;
}

void Socket::connect_thread(){

    std::cout << "connect thread started" << std::endl;

    while(Task::get_thread_state()){
        int client_id = accept(m_sock, nullptr, nullptr);
        if (client_id <= 0)
            continue;
        std::cout << "successfuly to client: " << client_id << " connect!" << std::endl;
        m_client_threads.push_back(std::make_pair(client_id, std::thread(&Socket::receive_thread, this, client_id)));
        if (m_connect_func)
            m_connect_func(client_id);
    }
}

void Socket::send_thread(){

    std::cout << "server web send data thread started!" << std::endl;

    while(Task::get_thread_state()){
        std::pair<int, json> data = m_send_queue.pop();
        if (data.first == CLIENT_ALL){
            for (auto& client : m_client_threads){
                socket_send(client.first, data.second);
            }
        }else {
            socket_send(data.first, data.second);
        }
    }
}

void Socket::receive_thread(int client_id){

    std::cout << client_id << " receive thread started!" << std::endl;

    while(Task::get_thread_state()){
        json recv_json;
        if (receive_json_message(client_id, recv_json))
            break;
        auto it = m_cmd_func.find(recv_json["Cmd"]);
        if (it != m_cmd_func.end()) {
            it->second(recv_json);
        }
        std::cout << "json: " << recv_json.dump() << std::endl;
    }
    client_thread_erase(client_id);
}

