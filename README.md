# 项目目录结构说明

本项目基于 RK3566 平台，集成摄像头驱动及人脸识别功能，使用 CMake 构建，包含 SQLite 数据库及字体资源等。以下是项目目录结构及各目录说明：

```bash
├── 3rdparty/   #第三方依赖库
│
├── assets/     #资源文件
│ ├── font/     #字体文件
│ └── model/    #人脸识别模型文件
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

## 目录功能说明

- **3rdparty/**  
  存放第三方依赖库源码或预编译文件，方便管理与更新。

- **assets/**  
  项目资源文件目录，包含字体文件和人脸识别模型文件。

- **build/**  
  由 CMake 和编译系统自动生成的构建输出目录，包含中间文件和编译生成物。

- **cmake/**  
  存放项目自定义的 CMake 模块和工具脚本，辅助构建流程。

- **config/**  
  项目运行时所需的配置文件，如摄像头参数、数据库配置等。

- **database/**  
  SQLite 数据库文件存放位置，存储人脸数据、识别记录等。

- **docs/**  
  项目相关文档，如设计说明、使用手册等。

- **include/**  
  公共头文件目录，供各模块及外部依赖调用。

- **src/**  
  项目主代码目录，`app/` 目录存放应用层逻辑代码。

- **target/**  
  编译完成后生成的最终产物目录，包含可执行文件、库文件及共享资源。

- **web/**  
  Web 服务器相关文件夹，包含 Python 缓存、静态文件和 HTML 模板，用于项目中可能的前端界面。

---

## 初始化子仓库
```bash
git submodule update --init --recursive
```

## 需要安装的依赖
```bash
sudo apt-get install build-essential cmake libdrm-dev libpthread-stubs0-dev
```

## 交叉编译完成后运行在目标板

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
