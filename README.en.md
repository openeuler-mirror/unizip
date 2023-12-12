# README_en

# unizip

#### Description

This project is based on the openEuler community's unified compression library. With this compression library, adaptation to different compression software is achievable without modifying the source code of open-source software.

#### Software Architecture

![](document/static/structureEN.png)

  For detailed design, please refer to the wiki.

#### Installation

1. Execute the following command to install gmock-devel, cmake, make, zlib-devel, LibLZMA, BZip2, Zstd, Snappy, Brotli, lz4:

```
yum install -y gmock-devel cmake make gcc-c++ bzip2-devel zlib-devel xz-devel libzstd-devel snappy-devel brotli-devel lz4-devel
```

2. Use the following command to clone the code:

```
git clone https://gitee.com/openeuler/unizip.git
```

3. Enter the "unzip" directory and execute the following command for compilation:

```
sudo mkdir build
cd build
cmake ..
make
```

4. Execute the following command to run the test cases:

```
ctest
```

5. Execute the following command to generate coverage reports:

```
make coverage
```

6. Execute the following command to install:

```
make install
```

#### Instructions

Open-source software can use unizip as a replacement for the zlib library. The modifications required are as follows (using curl as an example):

1. Modify the compilation parameter configuration file of the open-source code. Depending on the compilation method of different open-source software, modify different files such as `configure.ac`or `CMakeLists.txt`, etc. Taking curl as an example, modify `configure.ac` and add the following content to control whether to use unizip through the parameter --with-unizip:

```
AC_ARG_WITH([unizip],
    [AS_HELP_STRING([--with-unizip], [Enable UNIZIP (default is no)])],
    [use_unizip="$withval"],
    [use_unizip="no"])

if test "x$use_unizip" = "xyes"; then
    AC_MSG_WARN([UNIZIP is enabled.])
    PKG_CHECK_MODULES([UNIZIP], [unizip],
        [LIBS="-lunizip $LIBS"
        AC_DEFINE([USE_UNIZIP], [1], [Define if UNIZIP is enabled])],
        [
        AC_MSG_ERROR([Could not find UNIZIP library])
        ])
fi
```

2. Search for all locations where `zlib.h` is used in the source code file, replace `zlib.h` in the source code with `unizip_adapt.h`, and rename the structure. Take curl as an example:

For instance, replace the following code in the` content_encoding.c`:

```cpp
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif
```

With:

```cpp
#ifndef USE_UNIZIP
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif
#else
#include "unizip_adapt.h"
#define z_stream_s unizip_stream_s    //Rename the structure
#define z_stream unizip_stream        //Rename the structure
#define z_streamp unizip_streamp      //Rename the structure
#endif
```

3. Modify the compilation script of the open-source software. Add --with-unizip to configure to enable  unizip instead of zlib. Using `curl.spec` as an example:

```cpp
common_configure_opts="--cache-file=../config.cache \
         --enable-ldap \
         --enable-ldaps \
         --enable-manual \
-        --with-brotli \
         --with-libidn2 \
         --with-libpsl \
-        --with-libssh
+        --with-libssh \
+        --with-unizip                //Enable unizip
```



#### Contribution

1. Fork the repository
2. Create Feat_xxx branch
3. Commit your code
4. Create Pull Request
