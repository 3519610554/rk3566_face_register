#ifndef _SOCKET_H
#define _SOCKET_H

#include "OpencvPublic.h"
#include <cstddef>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <json.hpp>
#include <atomic>
#include <unordered_map>
#include "SafeQueue.h"
#include "ThreadState.h"

using json = nlohmann::json;

class Socket{
public:
    Socket();
    ~Socket();
    //实例化
    static Socket* Instance();
    //初始化
    void initialize();
    //线程启动
    void start();
    //发送数据
    void sned_data_add(json json_data);
    //接收数据命令处理绑定函数
    void recv_cmd_func_bind(std::string cmd, std::function<void(json)> func);
protected:
    //设置心跳包
    void set_keepalive(int keep_idle = 10, int keep_interval = 5, int keep_count = 3);
    //发送数据
    void socket_send(json json_data);
    //读取指定字节数(阻塞)
    bool recv_exact(int sockfd, void *buffer, size_t length);
    //接收json消息
    bool receive_json_message(int sockfd, json &out_json);
    //连接服务端线程
    void connect_thread();
    //发送数据线程
    void send_thread();
    //接收数据线程
    void receive_thread();
private:
    int m_sock;
    sockaddr_in m_server_addr;

    std::thread m_connect_thread;
    std::thread m_send_thread;
    std::thread m_receive_thread;

    ThreadState m_connect_state;
    
    SafeQueue<json> m_send_queue;
    std::unordered_map<std::string, std::function<void(json)>> m_cmd_func;
};

#endif
