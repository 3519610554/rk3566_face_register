#include "udp.h"
#include <iostream>
#include <cstring>

int Udp::initialize(const char *ip, int port){

    int state = NetSocket::initialize(ip, port, SOCK_DGRAM);

    if (bind(m_sock, (struct sockaddr*)&m_sa, sizeof(m_sa)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        CLOSE_SOCKET(m_sock);
        return -1;
    }
    return state;
}

int Udp::send_message(const char *message){
    
    int result = sendto(m_sock, message, (int)strlen(message), 0, (SockaddrType*)&m_sa, sizeof(m_sa));
    if (result == -1) {
        // std::cerr << "Send failed!" << std::endl;
        std::cerr << "Send failed! Error code: " << GET_ERROR() << std::endl;
        return 1;
    }
    return 0;
}

int Udp::receive_message(char *buf, int buf_size, sockaddr_in &client_addr,
            std::string &client_ip, int &client_port){

    socklen_t client_addr_len = sizeof(client_addr);
    memset(buf, 0, buf_size);
    int recv_len = recvfrom(m_sock, buf, buf_size - 1, 0, 
                            (struct sockaddr*)&client_addr, &client_addr_len);
    char receive_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, receive_ip, sizeof(receive_ip));
    client_ip = receive_ip;
    client_port = ntohs(client_addr.sin_port);
    return recv_len;
}
