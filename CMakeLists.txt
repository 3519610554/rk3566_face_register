# 设置 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.16)

# 定义项目名称及其版本信息
project(MyProject VERSION 1.0 LANGUAGES CXX)

# 设置 C++ 的标准版本为 C++17，并强制要求使用该标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 让 CMake 可以找到 cmake 目录下的 CMake 模块文件
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# 定义需要包含的外部 CMake 文件列表
set(LIBRARIES_FILE 
    CommonSettings      # 载入通用设置
    ExternalLibraries   # 载入外部库
    Threads             # 支持多线程
    Pipeline            # 集成 CI/CD 设置
)

# 定义需要添加的子目录
set(SUBDIRECTORY
    src         # 源代码目录
    tests       # 测试代码目录
)

# 循环遍历外部 CMake 文件列表，并逐个包含它们
foreach(library ${LIBRARIES_FILE})
    include(${library})   # 导入外部 CMake 配置文件
endforeach()

# 添加子目录，编译源码和测试目录下的内容
foreach(sub ${SUBDIRECTORY})
    add_subdirectory(${sub})   # 导入外部 CMake 配置文件
endforeach()

# 安装目标文件到指定的二进制文件夹（bin）
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
message(STATUS "src: ${CMAKE_BINARY_DIR}")