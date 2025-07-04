#include <iostream>
#include <spdlog/spdlog.h>
#include "ThreadPool.h"
#include "CameraUvc.h"
#include "TrainModel.h"
#include "FaceDetection.h"
#include "WebConnect.h"
#include "FaceTask.h"

int main(int argc, char* argv[]) {

    std::string yaml_path = "../config/app.yaml";

    if (argc > 1){
        yaml_path = argv[1];
        spdlog::info("config yaml path: {}", yaml_path);
    }

    ThreadPool::Instance()->initialize(8);
    CameraUvc::Instance()->initialize(yaml_path);
    WebConnect::Instance()->initialize(yaml_path);
    TrainModel::Instance()->initialize(yaml_path);
    FaceDetection::Instance()->initialize(yaml_path);
    FaceTask::Instance()->initialize();

    ThreadPool::Instance()->start();
    ThreadPool::Instance()->wait();

    return 0;
}
