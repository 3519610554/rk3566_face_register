CMAKE_CROSS := 

ROOT_DIR := ${PWD}
INSTALL_DIR	:= ${PWD}/target
BUILD_DIR	:= ${PWD}/build
CMAKE_STRIP := ${CMAKE_CROSS}strip
CMAKE_C_COMPILER := ${CMAKE_CROSS}gcc
CMAKE_CXX_COMPILER := ${CMAKE_CROSS}g++
FRAMEWORK_NAME := DEMO

all: release

clean:
	rm -rf ${INSTALL_DIR}
	rm -rf ${BUILD_DIR}

release:
	echo "build successfuly"

opencv:
	@[ ! -d ${BUILD_DIR}/opencv ] && git clone https://github.com/opencv/opencv.git --depth=1 ${BUILD_DIR}/opencv || echo "opencv soure ready..."
	@[ -e ${BUILD_DIR}/opencv/.build_ok ] && echo "opencv compilation completed..." || mkdir -p ${BUILD_DIR}/opencv/build

	cd ${BUILD_DIR}/opencv/build && \
	export LD_LIBRARY_PATH=${INSTALL_DIR}/lib:$LD_LIBRARY_PATH && \
	export PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig:$PKG_CONFIG_PATH && \
	export PKG_CONFIG_LIBDIR=${INSTALL_DIR}/lib:$PKG_CONFIG_LIBDIR && \
	cmake -DCMAKE_BUILD_TYPE=Release -DSMALL_LOCALSIZE=ON -DENABLE_FAST_MATH=ON \
		-DUSE_O3=ON -DENABLE_CXX11=ON -DWITH_TBB=ON -DWITH_OPENMP=ON \
		-DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DWITH_OPENCL=ON -DWITH_QT=OFF -DWITH_GTK=ON \
		-DWITH_GTK_2_X=ON -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF \
		-DWITH_FFMPEG=ON -DOPENCV_FFMPEG_SKIP_BUILD_CHECK=ON -DFFMPEG_DIR=${INSTALL_DIR} \
		-DBUILD_opencv_dnn=ON -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF\
		-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
		-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} \
		-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} \
		-DINSTALL_DIR=${INSTALL_DIR} ${BUILD_DIR}/opencv && \
	make -j4 && make install && cd -
	touch ${BUILD_DIR}/opencv/.build_ok


