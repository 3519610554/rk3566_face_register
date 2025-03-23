#ifndef _NET_SOCKET_H
#define _NET_SOCKET_H

#include <winsock2.h>

class NetSocket{
public:
    NetSocket();
    ~NetSocket();
    int initialize(const char *ip, int port, int socket_type);
    void cleanupNetwork();
    int send_message(const char *message);
    int receive_message(char *buffer, int buffer_size);
protected:
    WSADATA m_wsaData;
    SOCKET m_sock;
    sockaddr_in m_sa;
};

#endif
