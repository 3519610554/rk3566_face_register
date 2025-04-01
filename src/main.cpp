#include <iostream>
#include "net_socket.h"
// #include "tcp_server.h"
// #include "tcp_client.h"
#include "udp.h"
#include "tcp_client.h"

#define HOST    "10.34.54.178"
#define PORT    8080

int main() {
    
    // TcpClient server;

    // int state = server.connectServer(HOST, PORT);
    // std::cout << "state: " << state << std::endl;
    // if (!state){
    //     while(1){
    //         int input_data = 0;
    //         std::cin >> input_data;
    //         if (input_data==1){
    //             server.send_message("Hello");
    //         }else if (input_data==2){
    //             break;
    //         }
    //     }
    // }

    TcpClient server;

    int state = server.connectServer(HOST, 8080);
    std::cout << "state: " << state << std::endl;
    if (!state){
        while(1){
            int input_data = 0;
            std::cin >> input_data;
            if (input_data==1){
                send(server.getSock(), "Hello", strlen("Hello"), 0);
            }else if (input_data==2){
                break;
            }
        }
    }

    // Udp client;
    // std::cout << "begin\n";
    // int state = client.initialize("10.34.45.164", 8080);
    // std::cout << "state: " << state << std::endl;
    // if (!state){
    //     while(1){
    //         int input_data = 0;
    //         std::cin >> input_data;
    //         if (input_data==1){
    //             int res = client.send_message("Hello");
    //             std::cout << "res: " << res << std::endl;
    //         }else if (input_data==2){
    //             break;
    //         }
    //     }
    // }
    // std::cout << "Hello World\n";
    
    return 0;
}

