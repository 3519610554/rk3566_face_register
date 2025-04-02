# 设置 Boost 根目录，包含 include 和 lib 目录
set(BOOST_ROOT "${CMAKE_CURRENT_LIST_DIR}/arm64")
set(Boost_INCLUDE_DIRS "${BOOST_ROOT}/include")
set(Boost_LIBRARY_DIRS "${BOOST_ROOT}/lib")
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${BOOST_ROOT}/lib/cmake/Boost-1.87.0")
set(Boost_NO_SYSTEM_PATHS ON)

include_directories(${Boost_INCLUDE_DIRS})

message(STATUS "Current system architecture: ${CMAKE_SYSTEM_PROCESSOR}")


# 查找 Boost 包，指定需要的组件（system 和 asio）
find_package(Boost REQUIRED COMPONENTS system)
# find_package(Boost REQUIRED)

message("Boost version: ${Boost_VERSION}")
message("Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message("Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
message("Boost_LIBRARIES: ${Boost_LIBRARIES}")

# 创建并链接 Boost 库
add_library(boost_lib INTERFACE)
target_link_libraries(boost_lib INTERFACE Boost::system)

