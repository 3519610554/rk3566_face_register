add_executable(${PROJECT_NAME} main.cpp)

# add_subdirectory(module1)
add_subdirectory(socket)

target_link_libraries(${PROJECT_NAME} PRIVATE fmt Threads::Threads)