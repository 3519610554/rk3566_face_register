#ifndef SERVER_H
#define SERVER_H

#include "udp.h"
#include <thread>
#include <vector>
#include <mutex>
#include <set>

#define HOST    "10.34.45.164"
#define PORT    8080

struct ClientInfo{
    std::string ip;
    int port;
    sockaddr_in addr;

    ClientInfo(std::string ip, int port, sockaddr_in addr)
        :ip(ip), port(port), addr(addr) {}
    bool operator<(const ClientInfo& other) const {
        return ip < other.ip || port < other.port;
    }
};

class WebServer{
public:
    void init();
    void start();
    void post_message(std::string ip, int port, char* message);
    void run();
    void stop();
private:
    Udp m_web_server;
    std::set<ClientInfo> Clients;
};

#endif
