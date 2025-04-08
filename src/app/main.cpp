#include <iostream>
#include "CameraUvc.h"
 
int main() {

    CameraUvc uvc;
    uvc.start();
    uvc.wait();

    return 0;
}

