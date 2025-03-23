#ifndef _UDP_H
#define _UDP_H

#include "net_socket.h"

class Udp : public NetSocket{
public:
    int initialize(const char *ip, int port);
    int send_message(const char *message);
private:

};

#endif
