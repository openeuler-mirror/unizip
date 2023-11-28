/******************************************************************************
 * unizip_adapt.c
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
#include "unizip_adapt.h"
#include "unizip_error_code.h"
// Basic API

callback_t *cb = NULL;
int value = 2;
void SetValue(int v) { value = v; }
int get_algorithm(void)
{
    char *lib_value = getenv("LIB_VALUE");
    if (lib_value != NULL) {
        value = atoi(lib_value);
    }
    if (cb == NULL) {
        if ((cb = (callback_t *)malloc(sizeof(callback_t))) == NULL) {
            return -1;
        }
    }
    switch (value) {
        case CASE_ZLIB:
            // zlib
            zlib_init(cb);
            break;
        case CASE_BZIP2:
            break;
        case CASE_BROTLI:
            // brotli
            break;
        case CASE_LZ4:
            // lz4
            break;
        case CASE_SNAPPY:
            break;
        case CASE_ZSTD:
            break;
        case CASE_7ZIP:
            // 7zip
            break;
        case CASE_UADK:
            // uadk
            break;
        case CASE_XZ:
            break;
        case CASE_GZIP:
            // gzip
            break;
        default:
            ERR_CODE("Invalid OPTION value in environment variable.\n");
            return UNIZIP_ALGO_ERROR;
    }
    return 0;
}

// 获取软件包版本号
const char *unizip_Version(void)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        ERR_CODE("undifined algo\n");
        return "can't find algo!\n";
    }
    return cb->versionCB();
}

// 压缩器初始化
int unizip_deflateInit(unizip_streamp strm, int level)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->deflateInitCB(strm, level);
    // 用原版函数，强制类型转换即可
}

// 解压器初始化
int unizip_inflateInit(unizip_streamp strm)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->inflateInitCB(strm);
}

// 压缩结束
int unizip_deflateEnd(unizip_streamp strm)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->deflateEndCB(strm);
}

// 解压结束
int unizip_inflateEnd(unizip_streamp strm)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->inflateEndCB(strm);
}

// 压缩函数，根据压缩器的属性进行压缩
int unizip_deflate(unizip_streamp strm, int flush)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->deflateCB(strm, flush);
}

// 解压函数，根据解压器的属性进行解压
int unizip_inflate(unizip_streamp strm, int flush)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->inflateCB(strm, flush);
}

// Common API

// 压缩器复制
int unizip_deflateCopy(unizip_streamp dest, unizip_streamp source)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->deflateCopyCB(dest, source);
}

// 解压器复制
int unizip_inflateCopy(unizip_streamp dest, unizip_streamp source)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->inflateCopyCB(dest, source);
}

// 根据错误码输出错误信息
const char *unizip_zError(int err)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        ERR_CODE("undifined algo\n");
        return "undifined algo\n";
    }
    return zError(err);
}

// 压缩器重置，等价于先end再init
int unizip_deflateReset(unizip_streamp strm)
{
    // end + init
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->deflateResetCB(strm);
}

// 解压器重置，等价于先end再init
int unizip_inflateReset(unizip_streamp strm)
{
    // end + init
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->inflateResetCB(strm);
}

// 压缩，根据源文件和输出文件首地址及长度完成操作
int unizip_compress(Bytef *dest, uLongf *destLen, const Bytef *source,
                    uLong sourceLen)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->compressCB(dest, destLen, source, sourceLen);
}

// 解压，根据源文件和输出文件首地址及长度完成操作
int unizip_uncompress(Bytef *dest, uLongf *destLen, const Bytef *source,
                      uLong sourceLen)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->uncompressCB(dest, destLen, source, sourceLen);
}

// 压缩文件长度预估
uLong unizip_compressBound(uLong sourceLen)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->compressBoundCB(sourceLen);
}

// Unique API for Zlib

// zlib提供的压缩器初始化
int unizip_deflateInit2(unizip_streamp strm, int level, int method,
                        int windowBits, int memLevel, int strategy)
{
    if (value == CASE_ZLIB) {
        return deflateInit2((z_stream *)strm, level, method, windowBits,
                            memLevel, strategy);
    }
    return cb->inflateInitCB(strm, level);
}

// zlib提供的解压器初始化
int unizip_inflateInit2(unizip_streamp strm, int windowBits)
{
    if (value == CASE_ZLIB) {
        return inflateInit2((z_stream *)strm, windowBits);
    }
    return cb->inflateInitCB(strm);
}

// zlib提供的压缩，根据源文件和输出文件首地址及长度完成操作
int unizip_compress2(Bytef *dest, uLongf *destLen, const Bytef *source,
                     uLong sourceLen, int level)
{
    if (value == CASE_ZLIB) {
        return compress2(dest, destLen, source, sourceLen, level);
    }
    return cb->compressCB(dest, destLen, source, sourceLen);
}

// zlib提供的解压，根据源文件和输出文件首地址及长度完成操作
int unizip_uncompress2(Bytef *dest, uLongf *destLen, const Bytef *source,
                       uLong *sourceLen)
{
    if (value == CASE_ZLIB) {
        return uncompress2(dest, destLen, source, sourceLen);
    }
    return cb->uncompressCB(dest, destLen, source, sourceLen);
}

// zlib提供的对属性压缩，根据压缩器属性完成操作
int unizip_deflateParams(unizip_streamp strm, int level, int strategy)
{
    return deflateParams((z_stream *)strm, level, strategy);
}

// zlib提供的对压缩器设置字典
int unizip_deflateSetDictionary(unizip_streamp strm, const Bytef *dictionary,
                                uInt dictLength)
{
    return deflateSetDictionary((z_stream *)strm, dictionary, dictLength);
}

// zlib提供的填充字
int unizip_deflatePending(unizip_streamp strm, unsigned *pending, int *bits)
{
    return deflatePending((z_stream *)strm, pending, bits);
}

// zlib提供的设置头部
int unizip_deflateSetHeader(unizip_streamp strm, gz_headerp head)
{
    return deflateSetHeader((z_stream *)strm, head);
}

// zlib提供的对解压器设置字典
int unizip_inflateSetDictionary(unizip_streamp strm, const Bytef *dictionary,
                                uInt dictLength)
{
    return inflateSetDictionary((z_stream *)strm, dictionary, dictLength);
}

// zlib提供的获取头部
int unizip_inflateGetHeader(unizip_streamp strm, gz_headerp head)
{
    return inflateGetHeader((z_stream *)strm, head);
}
