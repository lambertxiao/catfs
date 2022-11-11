# catfs

FUSE-based file system backed by Amazon S3, UCloud S3

## 写在前面

本项目还处于开发阶段，当前支持功能如下

- [ ] 目录相关操作
    - [x] mkdir
    - [x] open_dir
    - [x] read_dir
    - [x] release_dir

- [ ] 文件相关操作
    - [x] open
    - [x] read
        - [x] direct read
        - [ ] read aheaed
        - [ ] read_after_write_done
    - [ ] write
        - [ ] 顺序写
        - [ ] 随机写
    - [ ] flush
        - [ ] sync_flush
        - [ ] async_flush
    - [x] release

- [ ] meta
    - [ ] 使用本地内存存meta
    - [ ] 使用redis存meta

- [ ] 链接支持

    - [ ] 软链
    - [ ] 硬链

- [ ] 读写并发控制

## 架构图

## 开发环境配置

### 下载源码

```
git clone https://github.com/lambertxiao/catfs.git
cd catfs && git submodule update --init --recurisive
```

### 编译libfuse

```
git clone https://github.com/libfuse/libfuse
cd libfuse; mkdir build; cd build
meson ..

# 指定编译参数，编译静态链接库
meson configure -Ddefault_library=static

# 执行ninja, 如无报错会在当前lib目录下生成需要的.a文件
ninja

# 将libfuse3.a移动到系统lib路径
mv lib/libfuse3.a /usr/local/lib
```

### 编译aws-sdk-cpp

```
git clone --recurse-submodules https://github.com/aws/aws-sdk-cpp
cd aws-sdk-cpp
mkdir build && cd build
cmake ../ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/usr/local/lib/awslib -DBUILD_SHARED_LIBS=OFF
make
make -j `nproc` install
```

整个aws-sdk-cpp编译很慢，理论上只需要编译和s3相关的模块，后续来优化这块
