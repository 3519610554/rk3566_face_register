#ifndef _NET_SOCKET_H
#define _NET_SOCKET_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using SocketType = SOCKET;
#define SockaddrType        SOCKADDR
#define INET_PTON(x1,x2,x3) InetPton(x1,x2,x3)
#define CLOSE_SOCKET(s)     closesocket(s)
#define GET_ERROR()         WSAGetLastError()
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
using   SocketType = int;
#define SockaddrType        struct sockaddr
#define INET_PTON(x1,x2,x3) inet_pton(x1,x2,x3)
#define CLOSE_SOCKET(s)     close(s)
#define GET_ERROR()         strerror(errno)
#endif

class NetSocket{
public:
    NetSocket();
    ~NetSocket();
    int initialize(const char *ip, int port, int socket_type);
    SocketType getSock();
protected:    
    SocketType m_sock;
    sockaddr_in m_sa;
#ifdef _WIN32
    WSADATA m_wsaData;
#endif
};

#endif
