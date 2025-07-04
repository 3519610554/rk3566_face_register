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
    void initialize(std::string yaml_path);
    //发送图片
    void send_image(int sockfd, int id, std::string time, std::string imageBase64);
    //发送id
    void send_image_id(int sockfd, json id_arr);
protected:
    //连接执行函数
    void connect_successfly_func(int client_id);
    //录入人脸命令
    void type_in_recv_func(int sockfd, json json_data);
    //删除上传照片
    void delete_image_recv_func(int sockfd, json json_data);
    //发送需要同步的照片
    void need_image_id_func(int sockfd, json json_data);
private:
};

#endif
