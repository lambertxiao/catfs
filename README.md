# catfs

FUSE-based file system backed by Amazon S3, UCloud S3

## 开发环境配置

### 编译libfuse

```
cd ~/workspace
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

### 安装包管理器vcpkg

通过vcpkg需要安装glog

```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install glog
```
