#ifndef _WEB_COMMECT_H
#define _WEB_COMMECT_H

#include "OpencvPublic.h"
#include "Socket.h"

class WebConnect{
public:
    WebConnect();
    ~WebConnect();
    //实例化
    static WebConnect* Instance();
    //初始化
    void initialize();
    //线程启动
    void start();
    //发送图片
    void send_image(int sockfd, int id, std::string time, std::string imageBase64);
    //数据分包
    void data_subpackage(int sockfd, std::string cmd, std::string data);
    //录入人脸命令
    void type_in_recv_func(json json_data);
protected:
    //照片转buffter
    std::string mat_to_buffer(const cv::Mat& img);
    //连接执行函数
    void connect_successfly_func(int client_id);
private:
};

#endif
