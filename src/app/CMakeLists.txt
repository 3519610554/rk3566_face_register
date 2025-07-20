include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/camera
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/database
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/model/rknn
    ${CMAKE_CURRENT_SOURCE_DIR}/system
    ${CMAKE_CURRENT_SOURCE_DIR}/util
    ${CMAKE_CURRENT_SOURCE_DIR}/rockchip
)

file(GLOB_RECURSE SRC_LIST 
    "*.cpp"
    "*.c"
)
file(GLOB_RECURSE HEADERS "*.h")

add_library(app STATIC ${SRC_LIST} ${HEADERS})

add_executable(${FRAMEWORK_NAME} 
    ${SRC_LIST}
)

set(EXTRA_LIBS
    opencv_imgcodecs
    opencv_highgui
    opencv_imgproc
    opencv_core
    opencv_videoio
    opencv_objdetect
    opencv_features2d
    opencv_flann
    opencv_calib3d  
    opencv_face
    opencv_freetype
    opencv_dnn  
    sqlite3
    rknnrt
    OpenCL
    pthread            
    dl
    stdc++
    yaml-cpp
    turbojpeg
    avfilter 
    avdevice 
    avformat
    avcodec
    avutil
    swresample
    swscale
    rockchip_mpp
    drm
    rga
    z
    lzma
    postproc
)

target_link_libraries(${FRAMEWORK_NAME}
    ${LIBRGA_LIB}
    ${JPEG_TURBO_LIB}
    ${EXTRA_LIBS}
)
