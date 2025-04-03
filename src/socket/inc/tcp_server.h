#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#include "net_socket.h"

class TcpServer : public NetSocket{
public:
    int initAcceptClient(const char *ip, int port);
    int acceptClientConnection();
private:
};

#endif
