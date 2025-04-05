file(GLOB SRC_LIST
    "*.cpp"
)

#定义库
add_library(util_lib ${SRC_LIST})

#设置库的头文件路径
target_include_directories(util_lib PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
