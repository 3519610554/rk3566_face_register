# 设置 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.16)

# 定义项目名称及其版本信息
project(MyProject VERSION 1.0 LANGUAGES CXX)

# 设置 C++ 的标准版本为 C++17，并强制要求使用该标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#生成 compile_commands.json 文件，主要用于代码分析和智能补全
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)


set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 让 CMake 可以找到 cmake 目录下的 CMake 模块文件
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(CommonSettings)
include(ExternalLibraries)
include(Threads)
include(Pipeline)

add_subdirectory(src) 
add_subdirectory(tests) 
add_subdirectory(external) 

# 安装目标文件到指定的二进制文件夹（bin）
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
message(STATUS "src: ${CMAKE_BINARY_DIR}")
