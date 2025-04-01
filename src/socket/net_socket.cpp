#include "net_socket.h"

NetSocket::NetSocket(){
#ifdef _WIN32
    WSAStartup(MAKEWORD(2, 2), &m_wsaData);
#endif
}

NetSocket::~NetSocket(){
    CLOSE_SOCKET(m_sock);
#ifdef _WIN32
    WSACleanup();
#endif
}

int NetSocket::initialize(const char *ip, int port, int socket_type){

    m_sock = socket(AF_INET, socket_type, 0);
    if (m_sock == -1) {
        CLOSE_SOCKET(m_sock);
        perror("socket create fail!");
        exit(EXIT_FAILURE);
    }

    m_sa.sin_family = AF_INET;
    m_sa.sin_port = htons(port);
    if (!INET_PTON(AF_INET, ip, &(m_sa.sin_addr))){
        perror("connection failed");
        CLOSE_SOCKET(m_sock);
        exit(EXIT_FAILURE);
    }

    return 0;
}

SocketType NetSocket::getSock(){

    return m_sock;
}
