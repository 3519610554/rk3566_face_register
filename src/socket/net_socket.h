#ifndef _NET_SOCKET_H
#define _NET_SOCKET_H

#include <winsock2.h>

class NetSocket{
public:
    int connect_server(const char *ip, int port);
    void cleanupNetwork();
    int send_message(const char *message);
    int receive_message(char *buffer, int buffer_size);
private:
    WSADATA m_wsaData;
    SOCKET m_sock;
    // sockaddr_in m_serverAddr;
    sockaddr_in m_sa;
};

#endif
