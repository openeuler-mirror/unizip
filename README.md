# README

# unizip

#### 介绍

the framework of integrating existing archiving libraries into unified program interfaces

#### 软件架构

![](document/static/structure.png)

  详细设计见 document/design

#### 安装教程

1. 执行如下命令安装 gmock-devel、cmake、make、zlib-devel、LibLZMA、BZip2、Zstd、Snappy、Brotli、lz4

```
yum install -y gmock-devel cmake make gcc-c++ bzip2-devel zlib-devel xz-devel libzstd-devel snappy-devel brotli-devel lz4-devel
```

2. 使用如下命令 clone 代码

```
git clone https://gitee.com/openeuler/unizip.git
```

3. 进入 unizip 目录，执行如下命令进行编译

```
sudo mkdir build
cd build
cmake ..
make
```

4. 执行如下命令运行测试用例：

```
ctest
```

5. 执行如下命令生成覆盖率报告：

```
make coverage
```

6. 执行如下命令安装：

```
make install
```

#### 参与贡献

1. Fork 本仓库
2. 新建 Feat_xxx 分支
3. 提交代码
4. 新建 Pull Request
