# unizip

#### 介绍
the framework of integrating existing archiving libraries into unified program interfaces

#### 软件架构
&emsp;&emsp;见[document/design](./document/design.md)


#### 安装教程

1. 执行如下命令安装gmock-devel、cmake、make、zlib-devel、LibLZMA、BZip2、Zstd、Snappy、Brotli、lz4
    ```
    yum install -y gmock-devel cmake make gcc-c++ bzip2-devel zlib-devel xz-devel libzstd-devel snappy-devel brotli-devel lz4-devel
    ```
2.  使用如下命令clone代码
    ```
    git clone https://gitee.com/openeuler/unizip.git
    ```


#### 使用说明

1.  进入ifm_nettle目录，使用如下命令运行build.sh进行测试
    ```
    ./build.sh
    ```

#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request
