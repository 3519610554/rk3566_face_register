#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include <iostream>

namespace util {
    //获取当前时间戳
    long long get_cuurent_timestamp();
    //获取时间戳 ms
    long long get_cuurent_timestamp_ms();
    //获取当前时间
    std::string get_cuurent_time();
}

#endif
