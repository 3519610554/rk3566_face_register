#include "net_socket.h"
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

void NetSocket::cleanupNetwork(){

    closesocket(m_sock);
    WSACleanup();
}

int NetSocket::connect_server(const char *ip, int port){

    if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
        // std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    if ((m_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        // std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 2;
    }

    m_sa.sin_family = AF_INET;
    m_sa.sin_port = htons(port);
    if (!InetPton(AF_INET, ip, &(m_sa.sin_addr))){
        // std::cerr << "Connection failed" << std::endl;
        WSACleanup();
        return 3;
    }

    if (connect(m_sock, (SOCKADDR*)&m_sa, sizeof(m_sa)) == SOCKET_ERROR) {
        // std::cerr << "Connection failed" << std::endl;
        WSACleanup();
        return 4;
    }

    return 0;
}

int NetSocket::send_message(const char *message) {
    int result = send(m_sock, message, (int)strlen(message), 0);
    if (result == SOCKET_ERROR) {
        // std::cerr << "Send failed" << std::endl;
        return 1;
    }
    return 0;
}

int NetSocket::receive_message(char *buffer, int buffer_size) {
    int result = recv(m_sock, buffer, buffer_size, 0);
    if (result > 0) {
        buffer[result] = '\0';  // 确保接收到的消息是以'\0'结束
        return result;
    } else if (result == 0) {
        // std::cerr << "Connection closed by server" << std::endl;
        return 0;
    } else {
        // std::cerr << "Receive failed" << std::endl;
        return -1;
    }
}
