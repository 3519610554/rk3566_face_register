#include "udp.h"
#include <iostream>

int Udp::initialize(const char *ip, int port){

    return NetSocket::initialize(ip, port, SOCK_DGRAM);
}

int Udp::send_message(const char *message){
    
    int result = sendto(m_sock, message, (int)strlen(message), 0, (SOCKADDR*)&m_sa, sizeof(m_sa));
    if (result == SOCKET_ERROR) {
        // std::cerr << "Send failed!" << std::endl;
        int errCode = WSAGetLastError();
        std::cerr << "Send failed! Error code: " << errCode << std::endl;
        return 1;
    }
    return 0;
}
