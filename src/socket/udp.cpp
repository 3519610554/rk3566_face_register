#include "udp.h"
#include <iostream>
#include <cstring>

int Udp::initialize(const char *ip, int port){

    return NetSocket::initialize(ip, port, SOCK_DGRAM);
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
