#ifndef _WEB_COMMECT_H
#define _WEB_COMMECT_H

#include "OpencvPublic.h"
#include "Socket.h"

class WebConnect{
public:
    WebConnect();
    ~WebConnect();
    //发送图片
    void send_image(const cv::Mat& img);
    //数据分包
    void data_subpackage(std::string cmd, std::string data);
    void type_in_recv_func(json json_data);
    void Test(json json_data);
    //实例化
    static WebConnect* Instance();
protected:
    //照片转buffter
    std::string mat_to_buffer(const cv::Mat& img);
private:
};

#endif
