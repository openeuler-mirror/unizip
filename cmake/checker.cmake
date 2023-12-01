# unizip: This is the cmake configuration file for the Unizip package
#
# Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
#
# Authors:
# huangduirong <huangduirong@huawei.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#

# check depends library and headers
find_package(PkgConfig REQUIRED)

macro(_CHECK)
if (${ARGV0} STREQUAL "${ARGV1}")
	message("error: can not find " ${ARGV2} " program")
	set(CHECKER_RESULT 1)
else()
	message("--  found " ${ARGV2} " --- works")
endif()
endmacro()

if (ENABLE_LIBUNIZIP)
# check libnettle
# pkg_check_modules(PC_LIBZLIB REQUIRED "nettle>=3")
# find_path(LIBZLIB_INCLUDE_DIR nettle/sm3.h
# 	HINTS ${PC_LIBZLIB_INCLUDEDIR} ${PC_LIBZLIB_INCLUDE_DIRS})
# _CHECK(LIBZLIB_INCLUDE_DIR "LIBZLIB_INCLUDE_DIR-NOTFOUND" "nettle/sm3.h")

find_library(LIBZLIB_LIBRARY z
	HINTS ${PC_LIBZLIB_LIBDIR} ${PC_LIBZLIB_LIBRARY_DIRS})
_CHECK(LIBZLIB_LIBRARY "LIBZLIB_LIBRARY-NOTFOUND" "libz.so")
find_library(LIBLZ4_LIBRARY lz4
	HINTS ${PC_LIBLZ4_LIBDIR} ${PC_LIBLZ4_LIBRARY_DIRS})
_CHECK(LIBLZ4_LIBRARY "LIBLZ4_LIBRARY-NOTFOUND" "liblz4.so")

find_library(LIBBROTLI_LIBRARY brotlidec
	HINTS ${PC_LIBBROTLI_LIBDIR} ${PC_LIBBROTLI_LIBRARY_DIRS})
_CHECK(LIBBROTLI_LIBRARY "LIBBROTLI_LIBRARY-NOTFOUND" "libbrotlidec.so")

find_library(LIBSNAPPY_LIBRARY snappy
	HINTS ${PC_LIBSNAPPY_LIBDIR} ${PC_LIBSNAPPY_LIBRARY_DIRS})
_CHECK(LIBSNAPPY_LIBRARY "LIBSNAPPY_LIBRARY-NOTFOUND" "libsnappy.so")

find_library(LIBBZIP2_LIBRARY bz2
	HINTS ${PC_LIBBZIP2_LIBDIR} ${PC_LIBBZIP2_LIBRARY_DIRS})
_CHECK(LIBBZIP2_LIBRARY "LIBBZIP2_LIBRARY-NOTFOUND" "libbz2.so")

find_library(LIBXZ_LIBRARY lzma
	HINTS ${PC_LIBXZ_LIBDIR} ${PC_LIBXZ_LIBRARY_DIRS})
_CHECK(LIBXZ_LIBRARY "LIBXZ_LIBRARY-NOTFOUND" "liblzma.so")

find_library(LIB7LZMA_LIBRARY 7lzma
	HINTS ${PC_LIB7LZMA_LIBDIR} ${PC_LIB7LZMA_LIBRARY_DIRS})
_CHECK(LIB7LZMA_LIBRARY "LIB7LZMA_LIBRARY-NOTFOUND" "lib7lzma.so")

message("arch is " ${CMAKE_SYSTEM_PROCESSOR} " --- works")
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
find_library(LIBWD_LIBRARY wd
	HINTS ${PC_LIBWD_LIBDIR} ${PC_LIBWD_LIBRARY_DIRS})
_CHECK(LIBWD_LIBRARY "LIBWD_LIBRARY-NOTFOUND" "libwd.so")
endif()

endif()

if (ENABLE_UT)
    pkg_check_modules(PC_GTEST "gtest")
    find_path(GTEST_INCLUDE_DIR gtest/gtest.h
        HINTS ${PC_GTEST_INCLUDEDIR} ${PC_GTEST_INCLUDE_DIRS})
    _CHECK(GTEST_INCLUDE_DIR "GTEST_INCLUDE_DIR-NOTFOUND" "gtest.h")
    find_library(GTEST_LIBRARY gtest
        HINTS ${PC_GTEST_LIBDIR} ${PC_GTEST_LIBRARY_DIRS})
    _CHECK(GTEST_LIBRARY "GTEST_LIBRARY-NOTFOUND" "libgtest.so")

    pkg_check_modules(PC_GMOCK "gmock")
    find_path(GMOCK_INCLUDE_DIR gmock/gmock.h
        HINTS ${PC_GMOCK_INCLUDEDIR} ${PC_GMOCK_INCLUDE_DIRS})
    _CHECK(GMOCK_INCLUDE_DIR "GMOCK_INCLUDE_DIR-NOTFOUND" "gmock.h")
    find_library(GMOCK_LIBRARY gmock
        HINTS ${PC_GMOCK_LIBDIR} ${PC_GMOCK_LIBRARY_DIRS})
    _CHECK(GMOCK_LIBRARY "GMOCK_LIBRARY-NOTFOUND" "libgmock.so")
endif()

if (ENABLE_GCOV)
    find_program(CMD_GCOV gcov)
    _CHECK(CMD_GCOV "CMD_GCOV-NOTFOUND" "gcov")

    find_program(CMD_LCOV lcov)
    _CHECK(CMD_LCOV "CMD_LCOV-NOTFOUND" "lcov")

    find_program(CMD_GENHTML genhtml)
    _CHECK(CMD_GENHTML "CMD_GENHTML-NOTFOUND" "genhtml")
endif()
