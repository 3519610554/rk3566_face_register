#include <spdlog/spdlog.h>
#include "thread_pool.h"
#include "camera_uvc.h"
#include "train_model.h"
#include "face_detection.h"
#include "web_connect.h"
#include "face_task.h"

int main(int argc, char* argv[]) {

    std::string yaml_path = "../config/app.yaml";

    if (argc > 1){
        yaml_path = argv[1];
        spdlog::info("config yaml path: {}", yaml_path);
    }

    ThreadPool::Instance()->initialize(10);
    CameraUvc::Instance()->initialize(yaml_path);
    WebConnect::Instance()->initialize(yaml_path);
    TrainModel::Instance()->initialize(yaml_path);
    FaceDetection::Instance()->initialize(yaml_path);
    FaceTask::Instance()->initialize();

    ThreadPool::Instance()->start();
    ThreadPool::Instance()->wait();

    return 0;
}
