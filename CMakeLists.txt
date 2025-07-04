cmake_minimum_required(VERSION 3.16)

project(${FRAMEWORK_NAME})

message(STATUS "FRAMEWORK_NAME: ${FRAMEWORK_NAME}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frtti -fexceptions -fpermissive")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")

set(CMAKE_SKIP_RPATH FALSE)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

# add_definitions(-fPIC)
add_definitions(-Ofast)
add_definitions(-ffast-math)
add_definitions(-funsafe-math-optimizations)
add_definitions(-D_GLIBCXX_PARALLEL)
add_definitions(-g)

message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")

# directory
set(DEP_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
message(STATUS "DEP_SOURCE_PATH: ${DEP_SOURCE_PATH}")

#全局头文件
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#librga
set(LIBRGA_API_PATH ${DEP_SOURCE_PATH}/librga)
include_directories( ${LIBRGA_API_PATH}/include)
set(LIBRGA_LIB ${LIBRGA_API_PATH}/lib/librga.a)

#stb_image
set(STD_IMAGE_API_PATH ${DEP_SOURCE_PATH}/stb_image)
include_directories( ${STD_IMAGE_API_PATH}/include)

#jpeg_turbo
set(JPEG_TURBO_API_PATH ${DEP_SOURCE_PATH}/jpeg_turbo)
include_directories( ${JPEG_TURBO_API_PATH}/include)
set(JPEG_TURBO_LIB ${JPEG_TURBO_API_PATH}/lib/libturbojpeg.a)

#target
include_directories(${CMAKE_INSTALL_PREFIX}/include)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

#opencv
include_directories(${CMAKE_INSTALL_PREFIX}/include/opencv4)

#openmp
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND OR OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    add_definitions(-fopenmp)
    message(STATUS "find OpenMP")
endif()

#opencl
find_package(OpenCL REQUIRED)
if (OpenCL_FOUND)
    include_directories(${OpenCL_INCLUDE_DIRS})
    message(STATUS "find OpenCL: ${OpenCL_LIBRARIES}")
endif()

#sqlite
include_directories(${CMAKE_INSTALL_PREFIX}/include/sqlite)

#curl
# include_directories(${CMAKE_INSTALL_PREFIX}/include/curl)

#json
include_directories(${CMAKE_INSTALL_PREFIX}/include/json)

#rknn
include_directories(${CMAKE_INSTALL_PREFIX}/include/rknn)

#spdlog
include_directories(${CMAKE_INSTALL_PREFIX}/include/spdlog)

#yaml-cpp
include_directories(${CMAKE_INSTALL_PREFIX}/include/yaml-cpp)

message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/app)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/app) 


INSTALL(TARGETS ${FRAMEWORK_NAME} DESTINATION bin)
