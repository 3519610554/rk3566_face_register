set(SRC_FILE_PATH "${CMAKE_CURRENT_LIST_DIR}/src")

set(LIBRARY_FILE 
    ${SRC_FILE_PATH}/server.cpp
)

#定义库
add_library(web_server_lib ${LIBRARY_FILE})

target_link_libraries(web_server_lib PRIVATE
    net_socket
)

#设置库的头文件路径
target_include_directories(web_server_lib PUBLIC "${CMAKE_CURRENT_LIST_DIR}/inc")
