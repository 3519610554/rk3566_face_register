#include <iostream>
#include "FaceTask.h"
 
int main() {
    
    FaceTask::Instance()->start();
    FaceTask::Instance()->wait();

    return 0;
}

