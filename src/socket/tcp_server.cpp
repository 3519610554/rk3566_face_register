#include "tcp_server.h"
#include <iostream>

int TcpServer::initialize(const char *ip, int port){

    return NetSocket::initialize(ip, port, SOCK_STREAM);
}

int TcpServer::initAcceptClient(const char *ip, int port){

    if (initialize(ip, port))
        return 1;

    // 绑定地址和端口
    if (bind(m_sock, (SockaddrType*)&m_sa, sizeof(m_sa)) == -1) {
        // std::cerr << "Bind failed" << std::endl;
        CLOSE_SOCKET(m_sock);
        return 2;
    }

    // 监听连接
    if (listen(m_sock, SOMAXCONN) == -1) {
        // std::cerr << "Listen failed" << std::endl;
        CLOSE_SOCKET(m_sock);
        return 3;
    }

    return 0;
}

int TcpServer::acceptClientConnection(){

    m_sock = accept(m_sock, NULL, NULL);
#ifdef _WIN32
    if (m_sock == INVALID_SOCKET)
#else
    if (m_sock < 0)
#endif
    {
        perror("accept failed!");
        exit(EXIT_FAILURE);
    }
    // std::cout << "Client connected" << std::endl;

    return m_sock;
}

