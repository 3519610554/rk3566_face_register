cmake_minimum_required(VERSION 3.16)

project(MyProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#让 CMake 可以找到cmake 目录下的 CMake 文件
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

#载入通用设置
include(CommonSettings)
#载入外部库
include(ExternalLibraries)
#多线程支持
include(Threads)
#CI/CD
include(Pipeline)

add_subdirectory(src)
add_subdirectory(tests)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
