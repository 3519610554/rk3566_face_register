#ifndef _SOCKET_H
#define _SOCKET_H

#include "OpencvPublic.h"
#include <cstddef>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <json.hpp>
#include <unordered_map>
#include <vector>
#include "SafeQueue.h"
#include "ThreadState.h"

#define CLIENT_ALL  0

using json = nlohmann::json;

class Socket{
public:
    Socket();
    ~Socket();
    //实例化
    static Socket* Instance();
    //初始化
    void initialize();
    //发送数据
    bool sned_data_add(int sockfd, json json_data);
    //连接成功执行函数
    void connect_sucessfly_func_bind(std::function<void(int)> func);
    //接收数据命令处理绑定函数
    void recv_cmd_func_bind(std::string cmd, std::function<void(json)> func);
protected:
    //设置心跳包
    void set_keepalive(int keep_idle = 10, int keep_interval = 5, int keep_count = 3);
    //客户端线程删除
    void client_thread_erase(int client_id);
    //发送数据
    void socket_send(int sockfd, json json_data);
    //读取指定字节数(阻塞)
    bool recv_exact(int sockfd, void *buffer, size_t length);
    //接收json消息
    bool receive_json_message(int sockfd, json &out_json);
    //连接服务端线程
    void connect_thread();
    //发送数据线程
    void send_thread();
    //接收数据线程
    void receive_thread(int client_id);
private:
    int m_sock;
    sockaddr_in m_sa;
    SafeQueue<std::pair<int, json>> m_send_queue;
    std::vector<int> m_clients;
    std::function<void(int)> m_connect_func;
    std::unordered_map<std::string, std::function<void(json)>> m_cmd_func;
};

#endif
