#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <iostream>
#include <boost/asio.hpp>

#define HOST    "10.34.54.178"
#define PORT    "8080"
#define TCP_BOOST   boost::asio::ip::tcp

int main() {

    try{
        boost::asio::io_context io_context;
        TCP_BOOST::resolver resolver(io_context);
        TCP_BOOST::resolver::results_type endpoints = resolver.resolve(HOST, PORT);
        TCP_BOOST::socket socket(io_context);

        boost::asio::connect(socket, endpoints);

        std::string msg = "Hello from Boost.Asio Client!";

        boost::asio::write(socket, boost::asio::buffer(msg));

        // 读取服务器返回的数据
        char buffer[1024] = {0};
        size_t len = socket.read_some(boost::asio::buffer(buffer));
        std::cout << "收到服务器消息：" << std::string(buffer, len) << std::endl;
    } catch (std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }
    
    return 0;
}

