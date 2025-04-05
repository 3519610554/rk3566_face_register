#ifndef _UDP_H
#define _UDP_H

#include "net_socket.h"

class Udp : public NetSocket{
public:
    int initialize(const char *ip, int port);
    int send_message(const char *message);
    int receive_message(char *buf, int buf_size, sockaddr_in &client_addr,
                        std::string &client_ip, int &client_port);
private:

};

#endif
