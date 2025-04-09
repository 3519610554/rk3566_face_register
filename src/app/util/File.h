#ifndef _FILE_H
#define _FILE_H

#include <iostream>

namespace util {
class File {
public:
    //获取当前工作区目录
    static std::string get_currentWorking_directory();

    //新建文件，目录文件夹自动生成
    static FILE *create_file(const char *file, const char *mode);

    //判断文件是否存在
    static bool file_exist(const char *path);
};
}

#endif
