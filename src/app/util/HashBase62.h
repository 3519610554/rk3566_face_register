#ifndef _HASH_BASE62_H
#define _HASH_BASE62_H

#include <iostream>

namespace util {
    //将整数转换为 Base62 编码字符串
    std::string toBase62(size_t value);
    //获取当前时间哈希Base62值
    std::string generateTimeHashString();
}

#endif
