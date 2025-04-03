#include "server.h"
#include <cstdio>
#include <string>
#include <vector>
#include <cstring>

#define LOG_INFO(fmt, ...)  printf(fmt "\n", __VA_ARGS__)
#define LOG_ERROR(fmt, ...) printf(fmt "\n", __VA_ARGS__)

void WebServer::init(){

    int state = m_web_server.initialize(HOST, PORT);
    LOG_INFO("state: %d", state);
}

#define BUFFER_SIZE 1024

void WebServer::start(){

    while(1){
        char buffer[BUFFER_SIZE];
        std::string client_ip;
        int client_port;
        sockaddr_in client_addr;
        m_web_server.receive_message(buffer, BUFFER_SIZE, client_addr, client_ip, client_port);
        Clients.insert(ClientInfo(client_ip, client_port, client_addr));
        LOG_INFO("client %s receive: %s", client_ip.c_str(), buffer);
        post_message(client_ip, client_port, buffer);
    }
}

void WebServer::post_message(std::string ip, int port, char* message){

    std::string full_message = "Client " + ip + ": " + message;
    for (const auto& client : Clients) {
        
        if (client.ip == ip&&port == client.port)
            continue;
        sendto(m_web_server.getSock(), full_message.c_str(), full_message.size(), 0,
            (struct sockaddr*)&client.addr, sizeof(client.addr));
    }
}

void WebServer::run(){

    init();
    start();
}

void WebServer::stop(){


}
