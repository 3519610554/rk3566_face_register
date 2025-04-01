# 添加可执行文件目标
add_executable(${PROJECT_NAME} main.cpp)

# 定义一个包含子目录名称的变量
set(SUBDIRECTORY socket)

# 定义链接的库文件列表
set(LINK_LIBRARIES 
        net_socket           # 链接 "net_socket" 静态库
        Threads::Threads     # 链接线程库（提供多线程支持）
)

#CMake 会递归进入该目录并执行其中的 CMakeLists.txt
foreach(sub ${SUBDIRECTORY})
    add_subdirectory(${sub})
endforeach()


# 将链接的库链接到当前可执行文件(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LINK_LIBRARIES})