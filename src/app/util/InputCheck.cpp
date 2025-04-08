#include "InputCheck.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int util::InputCheck::get_char_non_blocking(){

    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // 禁用缓冲区和回显
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // 设置为非阻塞

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // 恢复原有设置

    if(ch != EOF) {
        return ch;
    }

    return -1;  // 没有输入
}
