CMAKE_CROSS := 

ROOT_DIR := ${PWD}
INSTALL_DIR	:= ${PWD}/target
BUILD_DIR	:= ${PWD}/build
CMAKE_STRIP := ${CMAKE_CROSS}strip
CMAKE_C_COMPILER := ${CMAKE_CROSS}gcc
CMAKE_CXX_COMPILER := ${CMAKE_CROSS}g++
FRAMEWORK_NAME := UVC_Camera

all: release

clean:
	rm -rf ${INSTALL_DIR}
	rm -rf ${BUILD_DIR}
release:
	@[ -e ${BUILD_DIR}/.build_ok ] && echo "release compilation completed ..." || mkdir -p ${BUILD_DIR}

	cd ${BUILD_DIR} && \
	cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DFRAMEWORK_NAME=${FRAMEWORK_NAME} \
	-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} ..\
	&& \
	make -j2 && make install && cd -
	touch ${BUILD_DIR}/.build_ok
	patchelf --set-rpath ${INSTALL_DIR}/lib/ ${INSTALL_DIR}/bin/${FRAMEWORK_NAME}
opencv:
	@[ ! -d ${BUILD_DIR}/opencv ] && git clone https://github.com/opencv/opencv.git --depth=1 ${BUILD_DIR}/opencv || echo "opencv source ready..."
	@[ -e ${BUILD_DIR}/opencv/.build_ok ] && echo "opencv compilation completed..." || mkdir -p ${BUILD_DIR}/opencv/build 

	cd ${BUILD_DIR}/opencv/build && \
	cmake -DCMAKE_BUILD_TYPE=Release -DSMALL_LOCALSIZE=ON -DENABLE_FAST_MATH=ON -DWITH_IPP=OFF \
		-DUSE_O3=ON -DENABLE_CXX11=ON -DWITH_TBB=ON -DWITH_OPENMP=ON -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DWITH_WEBP=OFF \
		-DWITH_OPENCL=ON -DWITH_OPENGL=OFF -DWITH_QT=OFF -DWITH_GTK=ON -DWITH_GTK_2_X=ON -DWITH_CUDA=OFF \
		-DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_opencv_apps=OFF -DBUILD_ZLIB=OFF \
		-DWITH_FFMPEG=OFF -DOPENCV_FFMPEG_SKIP_BUILD_CHECK=OFF -DBUILD_opencv_objdetect=ON \
		-DBUILD_opencv_calib3d=ON -DBUILD_opencv_dnn=ON -DBUILD_opencv_features2d=ON \
		-DBUILD_opencv_flann=ON -DBUILD_opencv_gapi=OFF -DBUILD_opencv_ml=OFF \
		-DWITH_GSTREAMER=OFF -DWITH_JAVA=OFF \
		-DBUILD_opencv_stitching=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF \
		-DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib-4.x/modules \
		-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
		-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} \
		-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} \
		-DINSTALL_DIR=${INSTALL_DIR} ${BUILD_DIR}/opencv && \
	make -j2 && make install && cd -
	touch ${BUILD_DIR}/opencv/.build_ok

