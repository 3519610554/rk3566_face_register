# 添加可执行文件目标
add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(util)
add_subdirectory(app)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    util_lib 
    app_lib
)
