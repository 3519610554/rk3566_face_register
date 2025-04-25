#ifndef _FILE_H
#define _FILE_H

#include <iostream>

namespace util {
    //获取当前工作区目录
    std::string get_currentWorking_directory();

    //新建文件，目录文件夹自动生成
    FILE *create_file(const char *file, const char *mode);

    //判断文件是否存在
    bool file_exist(const char *path);
}

#endif
