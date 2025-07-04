CMAKE_CROSS := 

ROOT_DIR := ${PWD}
INSTALL_DIR	:= ${PWD}/target
BUILD_DIR	:= ${PWD}/build
CMAKE_STRIP := ${CMAKE_CROSS}strip
CMAKE_C_COMPILER := ${CMAKE_CROSS}gcc
CMAKE_CXX_COMPILER := ${CMAKE_CROSS}g++
FRAMEWORK_NAME := UVC_Camera
TOOLCHAIN_FILE := ${PWD}/scripts/arm-toolchain.cmake
THREAD_NUM := 14

.PHONY: all release debug clean build

all: release

clean:
	rm -rf ${INSTALL_DIR}
	rm -rf ${BUILD_DIR}

release:
	$(MAKE) build BUILD_TYPE=Release

debug:
	$(MAKE) build BUILD_TYPE=Debug

build:
	@[ -e ${BUILD_DIR}/.build_ok ] && echo "$(BUILD_TYPE) compilation completed ..." || mkdir -p ${BUILD_DIR}

	cd ${BUILD_DIR} && \
	cmake -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} \
		-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
		-DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
		-DFRAMEWORK_NAME=${FRAMEWORK_NAME} .. \
	&& \
	make -j${THREAD_NUM} && make install && cd -

	cp -r assets config ${INSTALL_DIR}
	touch ${BUILD_DIR}/.build_ok
	patchelf --set-rpath ${INSTALL_DIR}/lib/ ${INSTALL_DIR}/bin/${FRAMEWORK_NAME}

opencv:
	@[ ! -d ${BUILD_DIR}/opencv ] && git clone https://github.com/opencv/opencv.git --depth=1 ${BUILD_DIR}/opencv || echo "opencv source ready..."
	@[ ! -d ${BUILD_DIR}/opencv/opencv_contrib-4.x ] && git clone -b 4.x https://github.com/opencv/opencv_contrib.git --depth=1 ${BUILD_DIR}/opencv/opencv_contrib-4.x || echo "opencv_contrib-4.x source ready..."
	@[ -e ${BUILD_DIR}/opencv/.build_ok ] && echo "opencv compilation completed..." || mkdir -p ${BUILD_DIR}/opencv/build 

	cd ${BUILD_DIR}/opencv/build && \
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} \
		-DSMALL_LOCALSIZE=ON -DENABLE_FAST_MATH=ON -DWITH_IPP=OFF \
		-DUSE_O3=ON -DENABLE_CXX11=ON -DWITH_TBB=ON -DWITH_OPENMP=ON -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DWITH_WEBP=OFF \
		-DWITH_OPENCL=ON -DWITH_OPENGL=OFF -DWITH_QT=OFF -DWITH_GTK=ON -DWITH_GTK_2_X=ON -DWITH_CUDA=OFF \
		-DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_opencv_apps=OFF -DBUILD_ZLIB=OFF \
		-DWITH_FFMPEG=OFF -DOPENCV_FFMPEG_SKIP_BUILD_CHECK=OFF -DBUILD_opencv_objdetect=ON \
		-DBUILD_opencv_calib3d=ON -DBUILD_opencv_dnn=ON -DBUILD_opencv_features2d=ON \
		-DBUILD_opencv_flann=ON -DBUILD_opencv_gapi=OFF -DBUILD_opencv_ml=OFF \
		-DWITH_GSTREAMER=OFF -DWITH_JAVA=OFF -DOPENCV_ENABLE_FREE=ON \
		-DBUILD_opencv_stitching=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF \
		-DWITH_FREETYPE=ON -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib-4.x/modules \
		-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
		-DINSTALL_DIR=${INSTALL_DIR} ${BUILD_DIR}/opencv && \
	make -j${THREAD_NUM} && make install && cd -
	touch ${BUILD_DIR}/opencv/.build_ok

sqlite:
	@[ ! -d ${BUILD_DIR}/sqlite ] && wget https://sqlite.org/2025/sqlite-autoconf-3490100.tar.gz -O "${BUILD_DIR}/sqlite.tar.gz" -P ${BUILD_DIR} || echo "sqlite source ready..."
	@[ -f ${BUILD_DIR}/sqlite.tar.gz ] && { \
    tar -xzf ${BUILD_DIR}/sqlite.tar.gz -C ${BUILD_DIR} && \
		rm -f ${BUILD_DIR}/sqlite.tar.gz && \
		mv ${BUILD_DIR}/sqlite-autoconf-3490100 ${BUILD_DIR}/sqlite; \
	} || echo "sqlite.tar.gz missing, skipping extraction"
	cd ${BUILD_DIR}/sqlite && \
	./configure --host=aarch64-linux-gnu \
		--prefix=${INSTALL_DIR} \
		--includedir=${INSTALL_DIR}/include/sqlite && \
	make -j${THREAD_NUM} && sudo make install && cd -

libcurl:
	@[ ! -d ${BUILD_DIR}/libcurl ] && git clone https://github.com/curl/curl.git --depth=1 ${BUILD_DIR}/libcurl || echo "libcurl source ready..."
	@[ -e ${BUILD_DIR}/libcurl/.build_ok ] && echo "libcurl compilation completed..." || mkdir -p ${BUILD_DIR}/libcurl/build 

	cd ${BUILD_DIR}/libcurl/build && \
	cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} && \
	make -j$(nproc) && sudo make install && cd -
	touch ${BUILD_DIR}/libcurl/.build_ok

json:
	@[ ! -d ${INSTALL_DIR}/include/json ] echo "json include exist" || mkdir -p ${INSTALL_DIR}/include/json
	wget https://github.com/nlohmann/json/releases/download/v3.12.0/json.hpp -P ${INSTALL_DIR}/include/json
	
spdlog:
	@[ ! -d ${BUILD_DIR}/spdlog ] && git clone https://github.com/gabime/spdlog.git --depth=1 ${BUILD_DIR}/spdlog || echo "spdlog source ready..."

	cp -r ${BUILD_DIR}/spdlog/include/spdlog ${INSTALL_DIR}/include/spdlog

rknn:
	@[ ! -d ${BUILD_DIR}/rknn ] && git clone https://github.com/airockchip/rknn-toolkit2.git --depth=1 ${BUILD_DIR}/rknn || echo "rknn source ready..."
	@[ ! -d ${INSTALL_DIR}/include/rknn ] echo "rknn include exist" || mkdir -p ${INSTALL_DIR}/include/rknn

	cp -r ${BUILD_DIR}/rknn/rknpu2/runtime/Linux/librknn_api/include/*.h ${INSTALL_DIR}/include/rknn
	cp -r ${BUILD_DIR}/rknn/rknpu2/runtime/Linux/librknn_api/aarch64/*.so ${INSTALL_DIR}/lib

yaml-cpp: 
	@[ ! -d ${BUILD_DIR}/yaml-cpp ] && git clone https://github.com/jbeder/yaml-cpp.git --depth=1 ${BUILD_DIR}/yaml-cpp || echo "yaml-cpp source ready..."
	@[ -e ${BUILD_DIR}/yaml-cpp/.build_ok ] && echo "yaml-cpp compilation completed..." || mkdir -p ${BUILD_DIR}/yaml-cpp/build 

	cd ${BUILD_DIR}/yaml-cpp/build && \
	cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
		-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} .. && \
	make -j$(nproc) && sudo make install && cd -
	touch ${BUILD_DIR}/yaml-cpp/.build_ok
