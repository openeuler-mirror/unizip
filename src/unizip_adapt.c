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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __aarch64__
#include "uadk/v1/wd.h"
#include <string.h>
#endif
#include "unizip_callback_7zip.h"
#include "unizip_callback_brotli.h"
#include "unizip_callback_bzip2.h"
#include "unizip_callback_gzip.h"
#include "unizip_callback_lz4.h"
#include "unizip_callback_snappy.h"
#include "unizip_callback_uadk.h"
#include "unizip_callback_xz.h"
#include "unizip_callback_zlib.h"
#include "unizip_callback_zstd.h"
#include "unizip_error_code.h"
#include "unizip_struct.h"
// Basic API

callback_t *cb = NULL;
int value = 1;
void SetValue(int v) { value = v; }

bool UadkEnabled(void)
{
#ifdef __aarch64__
    static bool inited = false;
    static bool enabled = false;

    if (inited) {
        return enabled;
    }

    char *env_ifm_uadk_enable = getenv("UNIZIP_UADK_ENABLE");
    if (env_ifm_uadk_enable != NULL && strcmp(env_ifm_uadk_enable, "NO") == 0) {
        inited = true;
        enabled = false;
    } else {
        inited = true;
        // check the uadk is enabled
        struct wd_queue queue;
        memset(&queue, 0, sizeof(struct wd_queue));
        queue.capa.alg = "zlib";
        int ret = wd_request_queue(&queue);
        if (ret) {
            ERR_CODE("wd_request_queue fail\n");
            enabled = false;
        } else {
            wd_release_queue(&queue);
            enabled = true;
        }
    }

    return enabled;
#else
    return false;
#endif
}

int get_algorithm(void)
{
    char *lib_value = getenv("UNIZIP_VALUE");
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
            bzip2_init(cb);
            break;
        case CASE_BROTLI:
            // brotli
            brotli_init(cb);
            break;
        case CASE_LZ4:
            // lz4
            lz4_init(cb);
            break;
        case CASE_SNAPPY:
            snappy_init(cb);
            break;
        case CASE_ZSTD:
            zstd_init(cb);
            break;
        case CASE_7ZIP:
            // 7zip
            zip7_init(cb);
            break;
#ifdef __aarch64__
        case CASE_UADK:
            // uadk
            if (UadkEnabled() == true) {
                uadk_init(cb);
            } else {
                ERR_CODE("UADK is disabled, could not use UADK.\n");
                return UNIZIP_ALGO_ERROR;
            }
            break;
#endif // __aarch64__
        case CASE_XZ:
            xz_init(cb);
            break;
        case CASE_GZIP:
            // gzip
            gzip_init(cb);
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
int unizip_compress(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    if (get_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return cb->compressCB(dest, destLen, source, sourceLen);
}

// 解压，根据源文件和输出文件首地址及长度完成操作
int unizip_uncompress(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
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
int unizip_deflateInit2(unizip_streamp strm, int level, int method, int windowBits, int memLevel, int strategy)
{
    if (value == CASE_ZLIB) {
        return cb->inflateInit2CB(strm, level, method, windowBits, memLevel, strategy);
    }
    return cb->inflateInitCB(strm, level);
}

// zlib提供的解压器初始化
int unizip_inflateInit2(unizip_streamp strm, int windowBits)
{
    if (value == CASE_ZLIB) {
        return cb->inflateInit2CB(strm, windowBits);
    }
    return cb->inflateInitCB(strm);
}

// zlib提供的压缩，根据源文件和输出文件首地址及长度完成操作
int unizip_compress2(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, int level)
{
    if (value == CASE_ZLIB) {
        return cb->compress2CB(dest, destLen, source, sourceLen, level);
    }
    return cb->compressCB(dest, destLen, source, sourceLen);
}

// zlib提供的解压，根据源文件和输出文件首地址及长度完成操作
int unizip_uncompress2(Bytef *dest, uLongf *destLen, const Bytef *source, uLong *sourceLen)
{
    if (value == CASE_ZLIB) {
        return cb->uncompress2CB(dest, destLen, source, sourceLen);
    }
    return cb->uncompressCB(dest, destLen, source, sourceLen);
}

// zlib提供的对属性压缩，根据压缩器属性完成操作
int unizip_deflateParams(unizip_streamp strm, int level, int strategy)
{
    return cb->deflateParamsCB(strm, level, strategy);
}

// zlib提供的对压缩器设置字典
int unizip_deflateSetDictionary(unizip_streamp strm, const Bytef *dictionary, uInt dictLength)
{
    return cb->deflateSetDictionaryCB(strm, dictionary, dictLength);
}

// zlib提供的填充字
int unizip_deflatePending(unizip_streamp strm, unsigned *pending, int *bits)
{
    return cb->deflatePendingCB(strm, pending, bits);
}

// zlib提供的设置头部
int unizip_deflateSetHeader(unizip_streamp strm, gz_headerp head) { return cb->deflateSetHeaderCB(strm, head); }

// zlib提供的对解压器设置字典
int unizip_inflateSetDictionary(unizip_streamp strm, const Bytef *dictionary, uInt dictLength)
{
    return cb->inflateSetDictionaryCB(strm, dictionary, dictLength);
}

// zlib提供的获取头部
int unizip_inflateGetHeader(unizip_streamp strm, gz_headerp head) { return cb->inflateGetHeaderCB(strm, head); }
