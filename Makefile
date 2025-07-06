CMAKE_CROSS := 

CMAKE_STRIP := ${CMAKE_CROSS}strip
CMAKE_C_COMPILER := ${CMAKE_CROSS}gcc
CMAKE_CXX_COMPILER := ${CMAKE_CROSS}g++
ROOT_DIR := ${PWD}
INSTALL_DIR	:= ${ROOT_DIR}/target
BUILD_DIR	:= ${ROOT_DIR}/build
THIRD_PARTY_DIR := ${ROOT_DIR}/3rdparty
TOOLCHAIN_FILE := ${ROOT_DIR}/scripts/arm-toolchain.cmake
FRAMEWORK_NAME := UVC_Camera
THREAD_NUM := 14

.PHONY: all release debug clean build

all: release

clean:
	rm -rf ${INSTALL_DIR}
	rm -rf ${BUILD_DIR}

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

release:
	$(MAKE) build BUILD_TYPE=Release

debug:
	$(MAKE) build BUILD_TYPE=Debug

opencv:
	@[ -e ${BUILD_DIR}/$@/.build_ok ] && echo "$@ compilation completed..." || mkdir -p ${BUILD_DIR}/$@ 

	cd ${BUILD_DIR}/$@ && \
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} \
		-DSMALL_LOCALSIZE=ON -DENABLE_FAST_MATH=ON -DWITH_IPP=OFF \
		-DUSE_O3=ON -DENABLE_CXX11=ON -DWITH_TBB=ON -DWITH_OPENMP=ON -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DWITH_WEBP=OFF \
		-DWITH_OPENCL=ON -DWITH_OPENGL=OFF -DWITH_QT=OFF -DWITH_GTK=ON -DWITH_GTK_2_X=ON -DWITH_CUDA=OFF \
		-DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_opencv_apps=OFF -DBUILD_ZLIB=OFF \
		-DWITH_FFMPEG=OFF -DOPENCV_FFMPEG_SKIP_BUILD_CHECK=OFF -DBUILD_opencv_objdetect=ON \
		-DBUILD_opencv_calib3d=ON -DBUILD_opencv_dnn=ON -DBUILD_opencv_features2d=ON \
		-DBUILD_opencv_flann=ON -DBUILD_opencv_gapi=OFF -DBUILD_opencv_ml=OFF \
		-DWITH_GSTREAMER=OFF -DWITH_JAVA=OFF -DOPENCV_ENABLE_FREE=ON \
		-DWITH_JPEG=ON \
		-DBUILD_opencv_stitching=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF \
		-DWITH_FREETYPE=ON -DOPENCV_EXTRA_MODULES_PATH=$(THIRD_PARTY_DIR)/opencv_contrib-4.x/modules \
		-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
		-DINSTALL_DIR=${INSTALL_DIR} ${BUILD_DIR}/$@ $(THIRD_PARTY_DIR)/$@ && \
	make -j${THREAD_NUM} && make install && cd -
	touch ${BUILD_DIR}/$@/.build_ok

sqlite:
	@[ -e ${BUILD_DIR}/$@/.build_ok ] && echo "$@ compilation completed..." || mkdir -p ${BUILD_DIR}/$@ 

	cd ${BUILD_DIR}/$@ && \
	$(THIRD_PARTY_DIR)/$@/configure \
		--host=aarch64-linux-gnu \
		--prefix=${INSTALL_DIR} \
		--disable-tcl \
		--includedir=${INSTALL_DIR}/include/$@ && \
	make -j${THREAD_NUM} && sudo make install && cd -

json:
	cp -r ${THIRD_PARTY_DIR}/$@/single_include/$@ ${INSTALL_DIR}/include/$@
	
spdlog:
	cp -r ${THIRD_PARTY_DIR}/$@/include/$@ ${INSTALL_DIR}/include/$@

rknn:
	@[ ! -d ${INSTALL_DIR}/include/$@ ] echo "$@ include exist" || mkdir -p ${INSTALL_DIR}/include/$@

	cp -r ${THIRD_PARTY_DIR}/$@/rknpu2/runtime/Linux/librknn_api/include/*.h ${INSTALL_DIR}/include/$@
	cp -r ${THIRD_PARTY_DIR}/$@/rknpu2/runtime/Linux/librknn_api/aarch64/*.so ${INSTALL_DIR}/lib

yaml-cpp: 
	@[ -e ${BUILD_DIR}/$@/.build_ok ] && echo "$@ compilation completed..." || mkdir -p ${BUILD_DIR}/$@

	cd ${BUILD_DIR}/$@ && \
	cmake \
		-DCMAKE_INSTALL_INCLUDEDIR=${INSTALL_DIR}/include/$@ \
		-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
		-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} \
		$(THIRD_PARTY_DIR)/$@ && \
	make -j$(nproc) && sudo make install && cd -
	touch ${BUILD_DIR}/$@/.build_ok

mpp:
	@[ -e ${BUILD_DIR}/$@/.build_ok ] && echo "$@ compilation completed..." || mkdir -p ${BUILD_DIR}/$@

	cd $(BUILD_DIR)/$@ && \
	cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
		-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} $(THIRD_PARTY_DIR)/$@ && \
	make -j$(nproc) && sudo make install && cd -

jpeg_turbo:
	@[ -e ${BUILD_DIR}/$@/.build_ok ] && echo "$@ compilation completed..." || mkdir -p ${BUILD_DIR}/$@

	cd $(BUILD_DIR)/$@ && \
	cmake \
		-DCMAKE_INSTALL_INCLUDEDIR=${INSTALL_DIR}/include/$@ \
		-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
		-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} \
		-DENABLE_SHARED=ON \
		-DENABLE_STATIC=OFF \
		-DENABLE_TURBOJPEG=ON \
		$(THIRD_PARTY_DIR)/$@ && \
	make -j$(nproc) && sudo make install && cd -

librga:
	@[ -d ${INSTALL_DIR}/include/$@ ] && echo "$@ compilation completed..." || mkdir -p ${INSTALL_DIR}/include/$@

	cp -r ${THIRD_PARTY_DIR}/$@/include/* ${INSTALL_DIR}/include/$@
	cp -r ${THIRD_PARTY_DIR}/$@/libs/Linux/gcc-aarch64/*.so ${INSTALL_DIR}/lib
