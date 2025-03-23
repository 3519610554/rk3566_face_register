#include "tcp_server.h"

int TcpServer::initialize(const char *ip, int port){

    return NetSocket::initialize(ip, port, SOCK_STREAM);
}

int TcpServer::initAcceptClient(const char *ip, int port){

    if (initialize(ip, port))
        return 1;

    // 绑定地址和端口
    if (bind(m_sock, (SOCKADDR*)&m_sa, sizeof(m_sa)) == SOCKET_ERROR) {
        // std::cerr << "Bind failed" << std::endl;
        cleanupNetwork();
        return 2;
    }

    // 监听连接
    if (listen(m_sock, SOMAXCONN) == SOCKET_ERROR) {
        // std::cerr << "Listen failed" << std::endl;
        cleanupNetwork();
        return 3;
    }

    return 0;
}

int TcpServer::acceptClientConnection(){

    m_sock = accept(m_sock, NULL, NULL);
    if (m_sock == INVALID_SOCKET) {
        // std::cerr << "Accept failed" << std::endl;
        cleanupNetwork();
        return 1;
    }
    // std::cout << "Client connected" << std::endl;

    return 0;
}
