#include "LocalTime.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>


namespace util{

long long get_cuurent_timestamp(){

    auto now = std::chrono::system_clock::now();
    
    auto duration = now.time_since_epoch();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    return timestamp;
} 

long long get_cuurent_timestamp_ms(){

    auto now = std::chrono::system_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    return millis;
}

std::string get_cuurent_time(){
    
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* local_tm = std::localtime(&now_time_t);
    std::ostringstream oss;
    oss << std::put_time(local_tm, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}
}
