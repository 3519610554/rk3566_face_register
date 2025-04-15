#include "timestamp.h"
#include <chrono>

long long util::Timestamp::get_timestamp(){

    auto now = std::chrono::system_clock::now();
    
    auto duration = now.time_since_epoch();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    return timestamp;
}
