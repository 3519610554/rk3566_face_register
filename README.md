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
git submodule update --init --recursive
```

# 需要安装的依赖
```bash
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
sudo apt install patchelf
sudo apt-get install build-essential cmake libdrm-dev libpthread-stubs0-dev
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
./UVC_Camera
```
