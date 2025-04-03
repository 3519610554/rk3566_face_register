set(SRC_FILE_PATH "${CMAKE_CURRENT_LIST_DIR}/src")

set(LIBRARY_FILE 
    ${SRC_FILE_PATH}/net_socket.cpp
    ${SRC_FILE_PATH}/tcp_server.cpp
    ${SRC_FILE_PATH}/tcp_client.cpp
    ${SRC_FILE_PATH}/udp.cpp
)

#定义库
add_library(net_socket ${LIBRARY_FILE})

#设置库的头文件路径
target_include_directories(net_socket PUBLIC "${CMAKE_CURRENT_LIST_DIR}/inc")
