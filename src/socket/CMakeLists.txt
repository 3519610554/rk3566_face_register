set(LIBRARY_FILE 
    net_socket.cpp
    tcp_server.cpp
    tcp_client.cpp
    udp.cpp
)

#定义库
add_library(net_socket ${LIBRARY_FILE})

#设置库的头文件路径
target_include_directories(net_socket INTERFACE ${CMAKE_CURRENT_LIST_DIR})
