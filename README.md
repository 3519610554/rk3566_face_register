# 运行环境
- C++ 14
- sqlite 1.x

# 构建环境
- cmake 3.22.1

# 项目目录结构

```bash
.
├── 3rdparty/   #第三方依赖库
│
├── assets/     #资源文件
│ ├── font/     #字体文件
│ ├── model/    #人脸识别模型文件
│ └── database/ # SQLite 数据库存放目录
│
├── scripts/    #自定义 CMake 模块和脚本
│
├── config/     #项目配置文件（如摄像头参数、系统配置等）
│
├── docs/       #项目相关文档
│
├── include/    #项目公共头文件
│
├── src/        #源代码目录
│ └── app/      #应用程序主代码
│
├── target/     #最终目标文件目录
│
└── web/        #Web 前端相关目录
```

---

# 初始化子仓库
```bash
git submodule update --init --recursive --depth 1
```

# 交叉编译主机需要安装的依赖
```bash
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu -y
sudo apt install patchelf libomp-14-dev -y
```

# 交叉编译环境需要安装的依赖

```bash
sudo apt-get install libgomp1 libstdc++-12-dev build-essential cmake pkg-config -y
```

# rk3566和交叉编译环境需要安装的依赖
```bash
sudo apt-get install libdrm-dev libpthread-stubs0-dev libv4l-dev -y

# 基础图像处理库（JPEG、PNG、TIFF等）
sudo apt install libjpeg-dev libpng-dev libtiff-dev -y

# GTK 相关（你启用了 WITH_GTK=ON，WITH_GTK_2_X=ON，用于图形界面）
sudo apt install libgtk2.0-dev -y

# 多线程支持（TBB 和 OpenMP）
sudo apt install libtbb-dev -y

# 字体渲染 FreeType（WITH_FREETYPE=ON）
sudo apt install libfreetype6-dev -y

# OpenCL 支持
sudo apt install ocl-icd-opencl-dev -y
```

# 交叉编译完成后运行在目标板

1. 同步文件到目标板
```bash
rsync -avz ./target/ root@<ip>:/root/target/
```

2. 设置临时运行库路径
```bash
export LD_LIBRARY_PATH=/root/target/lib:$LD_LIBRARY_PATH
```

3. 开始运行
```bash
cd /root/target/bin

4. 客户端拉流
```bash
ffplay -fflags nobuffer -flags low_delay -framedrop -strict experimental rtsp://192.168.10.8:8554/live
```
./UVC_Camera
```
