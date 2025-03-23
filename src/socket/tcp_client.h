#ifndef _TCP_CLIENT_H
#define _TCP_CLIENT_H

#include "net_socket.h"

class TcpClient : public NetSocket{
public:
    int initialize(const char *ip, int port);
    int connectServer(const char *ip, int port);
private:
};

#endif
