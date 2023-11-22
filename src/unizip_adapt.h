/******************************************************************************
 * unizip_adapt.h
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * chen-yufan <1109674186@qq.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 ********************************************************************************/
#ifndef SRC_UNIZIP_ADAPT_H_
#define SRC_UNIZIP_ADAPT_H_

#include <stdio.h>
#include <stdlib.h>

#include "unizip_callback_zlib.h"
#include "unizip_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

void set_value(int v);

// 获取软件包版本号
const char *unizip_Version(void);

// 压缩器初始化
int unizip_deflateInit(unizip_streamp strm, int level);

// 解压器初始化
int unizip_inflateInit(unizip_streamp strm);

// 压缩结束
int unizip_deflateEnd(unizip_streamp strm);

//解压结束
int unizip_inflateEnd(unizip_streamp strm);

//压缩函数，根据压缩器的属性进行压缩
int unizip_deflate(unizip_streamp strm, int flush);

//解压函数，根据解压器的属性进行解压
int unizip_inflate(unizip_streamp strm, int flush);

// Common API 其他软件包在下层需要做好适配

// 压缩器复制
int unizip_deflateCopy(unizip_streamp dest, unizip_streamp source);

// 解压器复制
int unizip_inflateCopy(unizip_streamp dest, unizip_streamp source);

// 根据错误码输出错误信息
const char *unizip_zError(int err);

// 压缩器重置，等价于先end再init
int unizip_deflateReset(unizip_streamp strm);

// 解压器重置，等价于先end再init
int unizip_inflateReset(unizip_streamp strm);

// 压缩，根据源文件和输出文件首地址及长度完成操作
int unizip_compress(Bytef *dest, uLongf *destLen, const Bytef *source,
                    uLong sourceLen);

// 解压，根据源文件和输出文件首地址及长度完成操作
int unizip_uncompress(Bytef *dest, uLongf *destLen, const Bytef *source,
                      uLong sourceLen);

// 压缩文件长度预估
uLong unizip_compressBound(uLong sourceLen);

// Unique API for Zlib

// zlib提供的压缩器初始化
int unizip_deflateInit2(unizip_streamp strm, int level, int method,
                        int windowBits, int memLevel, int strategy);

// zlib提供的解压器初始化
int unizip_inflateInit2(unizip_streamp strm, int windowBits);

// zlib提供的压缩，根据源文件和输出文件首地址及长度完成操作
int unizip_compress2(Bytef *dest, uLongf *destLen, const Bytef *source,
                     uLong sourceLen, int level);

// zlib提供的解压，根据源文件和输出文件首地址及长度完成操作
int unizip_uncompress2(Bytef *dest, uLongf *destLen, const Bytef *source,
                       uLong *sourceLen);

// zlib提供的对属性压缩，根据压缩器属性完成操作
int unizip_deflateParams(unizip_streamp strm, int level, int strategy);

// zlib提供的对压缩器设置字典
int unizip_deflateSetDictionary(unizip_streamp strm, const Bytef *dictionary,
                                uInt dictLength);

// zlib提供的填充字
int unizip_deflatePending(unizip_streamp strm, unsigned *pending, int *bits);

// zlib提供的设置头部
int unizip_deflateSetHeader(unizip_streamp strm, gz_headerp head);

// zlib提供的对解压器设置字典
int unizip_inflateSetDictionary(unizip_streamp strm, const Bytef *dictionary,
                                uInt dictLength);

// zlib提供的获取头部
int unizip_inflateGetHeader(unizip_streamp strm, gz_headerp head);

#ifdef __cplusplus
}
#endif

#endif // SRC_UNIZIP_ADAPT_H_
