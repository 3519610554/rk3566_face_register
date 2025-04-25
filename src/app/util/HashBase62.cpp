#include "HashBase62.h"
#include "LocalTime.h"

const std::string BASE62_CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

namespace util {
std::string toBase62(size_t value){

    std::string result;
    do {
        result = BASE62_CHARS[value % 62] + result;
        value /= 62;
    } while (value > 0);
    return result;
}

std::string generateTimeHashString(){

    auto millis = get_cuurent_timestamp_ms();
    std::hash<long long> hasher;
    size_t hashVal = hasher(millis);

    return toBase62(hashVal);
}
}
