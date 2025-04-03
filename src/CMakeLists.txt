# 添加可执行文件目标
add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(socket)
add_subdirectory(app)

# 将链接的库链接到当前可执行文件(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE 
    net_socket 
    Threads::Threads
    app_lib
)
