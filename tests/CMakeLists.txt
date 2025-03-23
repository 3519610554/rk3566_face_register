# 启用测试支持
enable_testing()

# 添加 GoogleTest 库
find_package(GTest REQUIRED)

# 添加测试可执行文件
add_executable(runTests test_thread.cpp)

# 链接 GoogleTest 和测试文件
target_link_libraries(runTests PRIVATE GTest::GTest GTest::Main)

# 添加到 CTest 测试框架
add_test(NAME WorkerThreadTest COMMAND runTests)

