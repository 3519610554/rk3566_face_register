#ifndef _SOCKET_H
#define _SOCKET_H

#include <cstddef>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>
#include "safe_queue.h"
#include "thread_state.h"

#define CLIENT_ALL  0

using json = nlohmann::json;

class Socket{
public:
    Socket();
    ~Socket();
    //实例化
    static Socket* Instance();
    //初始化
    void initialize(std::string yaml_path);
    //数据分包
    void data_subpackage(int sockfd, json data);
    //连接成功执行函数
    void connect_sucessfly_func_bind(std::function<void(int)> func);
    //接收数据命令处理绑定函数
    void recv_cmd_func_bind(std::string cmd, std::function<void(int, json)> func);
protected:
    //设置心跳包
    void set_keepalive(int keep_idle = 10, int keep_interval = 5, int keep_count = 3);
    //客户端线程删除
    void client_thread_erase(int client_id);
    //发送数据添加到缓冲区
    bool sned_data_add(int sockfd, json json_data);
    //发送数据
    void socket_send(int sockfd, json json_data);
    //读取指定字节数(阻塞)
    bool recv_exact(int sockfd, void *buffer, size_t length);
    //接收json消息
    bool receive_json_message(int sockfd, json &out_json);
    //数据解包
    void receive_json_unpack(int sockfd, json recv_json);
    //连接服务端线程
    void connect_thread();
    //发送数据线程
    void send_thread();
    //接收数据线程
    void receive_thread(int client_id);
private:
    int m_sock;
    sockaddr_in m_sa;
    std::vector<int> m_clients;
    SafeQueue<std::pair<int, json>> m_send_queue;
    std::function<void(int)> m_connect_func;
    std::unordered_map<std::string, std::string> m_recv_dict_buff;
    std::unordered_map<std::string, std::function<void(int, json)>> m_cmd_func;

    int m_local_port;
    int m_chunk_size;
};

#endif
