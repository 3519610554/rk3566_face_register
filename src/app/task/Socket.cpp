#include "Socket.h"
#include <exception>
#include <string>
#include <unistd.h>
#include <vector>
#include <chrono> 
#include <cstdio>
#include <netinet/tcp.h> 
#include "Task.h"

#define FLASK_HOST                  "10.34.4.112"
#define FLASK_PORT                  8080

Socket::Socket(){
    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_port = htons(FLASK_PORT);
    inet_pton(AF_INET, FLASK_HOST, &m_server_addr.sin_addr);

    set_connect_state_and_notify(false);
    m_connect_thread = std::thread(&Socket::connect_thread, this);
    m_send_thread = std::thread(&Socket::send_thread, this);
    m_receive_thread = std::thread(&Socket::receive_thread, this);
}

Socket::~Socket(){

    close(m_sock);
}

void Socket::sned_data_add(json json_data){

    m_send_queue.push(json_data);
}

void Socket::recv_cmd_func_bind(std::string cmd, std::function<void(json)> func){

    m_cmd_func[cmd] = func;
}

Socket* Socket::Instance(){

    static Socket socket_client;

    return &socket_client;
}

void Socket::set_keepalive(int keep_idle, int keep_interval, int keep_count){

    int yes = 1;
    setsockopt(m_sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes));
    setsockopt(m_sock, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(keep_idle));
    setsockopt(m_sock, IPPROTO_TCP, TCP_KEEPINTVL, &keep_interval, sizeof(keep_interval));
    setsockopt(m_sock, IPPROTO_TCP, TCP_KEEPCNT, &keep_count, sizeof(keep_count));
}

void Socket::socket_send(json json_data){

    std::string json_str = json_data.dump();
    uint32_t msg_len = htonl(json_str.size());

    ssize_t state = 0;
    state = send(m_sock, &msg_len, sizeof(msg_len), 0);
    state = send(m_sock, json_str.c_str(), json_str.size(), 0);
    set_connect_state_and_notify((state <= 0)?false:true);
}


bool Socket::recv_exact(int sockfd, void *buffer, size_t length){

    size_t total_received = 0;
    char *buf = static_cast<char*>(buffer);

    while(total_received < length){
        ssize_t bytes = recv(sockfd, buf+total_received, length-total_received, 0);
        if (bytes <= 0){
            set_connect_state_and_notify(false);
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

void Socket::wait_for_connect_state_false(){

    std::unique_lock<std::mutex> lock(m_mutex_connect);
    m_cv_connect.wait(lock, [this]() { return !m_connect_state.load(); });
    lock.unlock();
}

void Socket::wait_for_connect_state_true(){
    
    std::unique_lock<std::mutex> lock(m_mutex_connect);
    m_cv_connect.wait(lock, [this]() { return m_connect_state.load(); });
    lock.unlock();
}

void Socket::set_connect_state_and_notify(bool state){
    {
        std::lock_guard<std::mutex> lock(m_mutex_connect);
        m_connect_state = state;
    }
    m_cv_connect.notify_all();
}

void Socket::connect_thread(){

    while(Task::get_thread_state()){
        wait_for_connect_state_false();
        if (connect(m_sock, (struct sockaddr*)&m_server_addr, sizeof(m_server_addr)) < 0){
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }else {
            std::cout << "successfly to flask server connect!" << std::endl;
            set_keepalive();
            set_connect_state_and_notify(true);
        }
    }
}

void Socket::send_thread(){

    std::cout << "server web send data thread started!" << std::endl;

    while(Task::get_thread_state()){
        wait_for_connect_state_true();
        json data = m_send_queue.pop();
        socket_send(data);
    }
}

void Socket::receive_thread(){

    std::cout << "server web receive data thread started!" << std::endl;

    while(Task::get_thread_state()){
        wait_for_connect_state_true();
        json recv_json;
        if (receive_json_message(m_sock, recv_json))
            return;
        auto it = m_cmd_func.find(recv_json["Cmd"]);
        if (it != m_cmd_func.end()) {
            it->second(recv_json);
        }
        std::cout << "json: " << recv_json.dump() << std::endl;
    }
}

