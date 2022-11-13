# catfs

## 简介

CATFS是一个能在linux环境中将一个对象存储的存储桶bucket挂载到本地目录的工具，建立挂载后，可以像操作本地文件系统一样操作存储桶中的文件。CATFS计划接入多种后端存储，优先S3, US3, OSS等对象存储。

## 运行环境

> catfs基于fuse实现，需要您的机器已安装了fuse，可通过执行 `modprobe fuse` 查看支持情况。

- Linux

## 项目进度

本项目还处于开发阶段，功能支持情况如下

| posix 接口 | 实现情况 |
| - | - |
| lookup | ✅ |
| forget | ✅ |
| getattr | ✅ |
| setattr | ✅ |
| readlink | |
| mknod | ✅ |
| mkdir | ✅ |
| unlink | ✅ |
| rmdir | ✅ |
| symlink | |
| rename | ✅ |
| link | |
| open | ✅ |
| create | ✅ |
| read | ✅ |
| write | ✅ |
| flush | ✅ |
| release | ✅ |
| fsync |  |
| opendir | ✅ |
| readdir | ✅ |
| readdirplus | ✅ |
| releasedir | ✅ |
| fsyncdir |  |
| setxattr |  |
| getxattr |  |
| listxattr |  |
| removexattr | |
| access | |
| getlk |  |
| setlk |  |
| bmap |  |
| poll |  |
| write_buf | ✅ |
| retrieve_reply | |
| forget_multi | |
| flock | |
| fallocate | |
| copy_file_range | |
| lseek | |

## 当前版本使用限制

- 不支持随即写
- 不支持追加写

## todo list

- [ ] 读写并发控制
- [ ] 多线程写入
- [ ] 元数据可以外置，支持多个client端共享元数据
- [ ] 小文件追加写
- [ ] 小文件随即写

## 架构图

![](https://lambert4.cn-bj.ufileos.com/struct.png)

## 如何使用

### 运行方式

```
usage: ./catfs --bucket=string --mount=string [options] ...
options:
  -b, --bucket              bucket name (string)
  -m, --mount               mount point (string)
      --bucket_prefix       bucket key prefix (string [=])
      --public_key          public key (string [=])
      --private_key         private key (string [=])
      --endpoint            storage backend endpoint (string [=])
      --stor_backend        specified storage backend (string [=s3])
      --passwd              specify access file (string [=/etc/catfs/catfs.yaml])
      --gid                 gid (unsigned int [=0])
      --uid                 uid (unsigned int [=0])
      --dcache_timeout      dentry cache timeout, unit is seconds (unsigned int [=300])
      --retry               number of times to retry a failed I/O (unsigned int [=3])
      --parallel            number of parallel I/O thread (unsigned int [=32])
      --level               set log level: error/warn/info/debug (string [=info])
      --log_dir             set log dir (string [=])
  -f, --foreground          running in foreground
      --singlethread        singlethread
      --max_idle_threads    max_idle_threads (unsigned int [=4])
  -?, --help                print this message
```

### 配置文件格式

默认路径：/etc/catfs/catfs.yaml

```yaml
public_key: TOKEN_4b26930b-d888-44c4-8931-XXXXXXX
private_key: 4955517f-ec32-4254-bebe-XXXXXXX
endpoint: internal.s3-cn-sh2.ufileos.com
```

## 开发

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
