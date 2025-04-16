include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/task
    ${CMAKE_CURRENT_SOURCE_DIR}/util
)

file(GLOB_RECURSE SRC_LIST "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_library(app STATIC ${SRC_LIST} ${HEADERS})

add_executable(${FRAMEWORK_NAME} 
    ${SRC_LIST}
)

set (EXTRA_LIBS 
    opencv_highgui 
    opencv_imgproc 
    opencv_imgcodecs 
    opencv_core 
    # opencv_video 
    opencv_videoio 
    # opencv_dnn 
    opencv_objdetect 
    opencv_face
    opencv_freetype
    # opencv_dnn
    stdc++ dl
    -std=c++11
    -lsqlite3
    -lcurl
)

target_link_libraries(${FRAMEWORK_NAME}
    pthread
    ${EXTRA_LIBS}
)
