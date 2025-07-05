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

// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <turbojpeg.h>
// #include <opencv2/opencv.hpp>
// #include <spdlog/spdlog.h>

// int main() {
//     // 1) 读文件到内存
//     std::ifstream file("frame.jpg", std::ios::binary);
//     if (!file.is_open()) {
//         spdlog::error("无法打开 frame.jpg");
//         return -1;
//     }
//     file.seekg(0, std::ios::end);
//     size_t jpegSize = file.tellg();
//     file.seekg(0, std::ios::beg);
//     std::vector<unsigned char> jpegBuf(jpegSize);
//     file.read(reinterpret_cast<char*>(jpegBuf.data()), jpegSize);
//     file.close();

//     // 2) 用 turbojpeg 解码
//     tjhandle tjd = tjInitDecompress();
//     if (!tjd) {
//         spdlog::error("tjInitDecompress 失败: {}", tjGetErrorStr());
//         return -1;
//     }

//     int width, height, jpegSubsamp, jpegColorspace;
//     if (tjDecompressHeader3(tjd,
//                             jpegBuf.data(),
//                             jpegSize,
//                             &width,
//                             &height,
//                             &jpegSubsamp,
//                             &jpegColorspace) < 0)
//     {
//         spdlog::error("tjDecompressHeader3 失败: {}", tjGetErrorStr());
//         tjDestroy(tjd);
//         return -1;
//     }
//     spdlog::info("JPEG 分辨率: {} x {}, 采样: {}", width, height, jpegSubsamp);

//     // 输出格式选择 RGB
//     int pixelFormat = TJPF_BGR;  // OpenCV BGR 格式
//     std::vector<unsigned char> dstBuf(width * height * 3);

//     if (tjDecompress2(tjd,
//                       jpegBuf.data(),
//                       jpegSize,
//                       dstBuf.data(),
//                       width,
//                       0,  // pitch = width * 3
//                       height,
//                       pixelFormat,
//                       0) < 0)
//     {
//         spdlog::error("tjDecompress2 失败: {}", tjGetErrorStr());
//         tjDestroy(tjd);
//         return -1;
//     }
//     tjDestroy(tjd);

//     // 3) 将原始像素数据封装成 cv::Mat 并显示
//     cv::Mat img(height, width, CV_8UC3, dstBuf.data());
//     if (img.empty()) {
//         spdlog::error("封装 Mat 失败");
//         return -1;
//     }
//     cv::imshow("TurboJPEG Decoded", img);
//     cv::waitKey(0);
//     cv::destroyAllWindows();

//     return 0;
// }

