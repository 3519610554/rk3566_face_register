#include <iostream>
#include "net_socket.h"
// #include "tcp_server.h"
// #include "tcp_client.h"
#include "udp.h"

int main() {
    
    // TcpServer server;

    // int state = server.initAcceptClient("172.20.10.2", 5000);
    // std::cout << "state: " << state << std::endl;
    // state = server.acceptClientConnection();
    // std::cout << "state: " << state << std::endl;
    // if (!state){
    //         while(1){
    //             int input_data = 0;
    //             std::cin >> input_data;
    //             if (input_data==1){
    //                 server.send_message("Hello");
    //             }else if (input_data==2){
    //                 break;
    //             }
    //         }
    //     }
    Udp client;
    std::cout << "begin\n";
    int state = client.initialize("172.20.10.2", 8080);
    std::cout << "state: " << state << std::endl;
    if (!state){
        while(1){
            int input_data = 0;
            std::cin >> input_data;
            if (input_data==1){
                int res = client.send_message("Hello");
                std::cout << "res: " << res << std::endl;
            }else if (input_data==2){
                break;
            }
        }
    }
    client.cleanupNetwork();
    std::cout << "Hello World\n";
    
    return 0;
}

