#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include <iostream>

namespace util {
class LocalTime{
public:
    //获取当前时间戳
    static long long get_cuurent_timestamp();
    //获取当前时间
    static std::string get_cuurent_time();
};
}

#endif
