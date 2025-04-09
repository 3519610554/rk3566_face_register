#include "File.h"
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

#define    _unlink      unlink
#define    _rmdir       rmdir
#define    _access      access

std::string util::File::get_currentWorking_directory(){
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        return std::string(buffer);
    } else {
        std::cerr << "failed to get the current directory" << std::endl;
        return "";
    }
}

FILE *util::File::create_file(const char *file, const char *mode){
    std::string path = file;
    std::string dir;
    size_t index = 1;
    FILE *ret = nullptr;
    while (true) {
        index = path.find('/', index) + 1;
        dir = path.substr(0, index);
        if (dir.length() == 0) {
            break;
        }
        if (_access(dir.c_str(), 0) == -1) { //access函数是查看是不是存在
            if (mkdir(dir.c_str(), 0777) == -1) {  //如果不存在就用mkdir函数来创建
                std::cout << "mkdir : " << dir;
                return nullptr;
            }
        }
    }
    if (path[path.size() - 1] != '/') {
        ret = fopen(file, mode);
    }
    return ret;
}

bool util::File::file_exist(const char *path){

    auto fp = fopen(path, "rb");
    if (!fp) {
        return false;
    }
    fclose(fp);
    return true;
}
