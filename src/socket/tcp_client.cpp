#include "tcp_client.h"

int TcpClient::initialize(const char *ip, int port){

    return NetSocket::initialize(ip, port, SOCK_STREAM);
}

/**
 * @brief 连接到服务器
 * 
 * @param ip    目标 IP
 * @param port  目标端口号
 * @return int  0 成功
 */
int TcpClient::connectServer(const char *ip, int port){

    if (initialize(ip, port))
        return 1;
    if (connect(m_sock, (SOCKADDR*)&m_sa, sizeof(m_sa)) == SOCKET_ERROR) {
        WSACleanup();
        return 2;
    }
    return 0;
}
