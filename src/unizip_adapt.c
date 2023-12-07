/******************************************************************************
 * unizip_adapt.c
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * chen-yufan <1109674186@qq.com>
 * shen-yi <1847401037@qq.com>
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
#include <string.h>
#include "uadk/v1/wd.h"
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

#define BASIC_INTERFACE(alg_name)                                              \
    versionCB_##alg_name, deflateInitCB_##alg_name, deflateCB_##alg_name,      \
        deflateEndCB_##alg_name, inflateInitCB_##alg_name,                     \
        inflateCB_##alg_name, inflateEndCB_##alg_name,                         \
        deflateCopyCB_##alg_name, inflateCopyCB_##alg_name,                    \
        deflateResetCB_##alg_name, inflateResetCB_##alg_name,                  \
        compressCB_##alg_name, uncompressCB_##alg_name,                        \
        compressBoundCB_##alg_name

#define INIT2_INTERFACE(alg_name)                                              \
    deflateInit2CB_##alg_name, inflateInit2CB_##alg_name

#define COMMPRESS2_INTERFACE(alg_name)                                         \
    compress2CB_##alg_name, uncompress2CB_##alg_name

#define DICTIONARY_INTERFACE(alg_name)                                         \
    deflateSetDictionaryCB_##alg_name, inflateSetDictionaryCB_##alg_name

#define HEADER_INTERFACE(alg_name)                                             \
    deflateSetHeaderCB_##alg_name, inflateGetHeaderCB_##alg_name

#define EXTERN_INTERFACE(alg_name)                                             \
    deflateParamsCB_##alg_name, deflatePendingCB_##alg_name

callback_t zlib_ck = {ALG_ZLIB,
                      BASIC_INTERFACE(zlib),
                      INIT2_INTERFACE(zlib),
                      COMMPRESS2_INTERFACE(zlib),
                      DICTIONARY_INTERFACE(zlib),
                      HEADER_INTERFACE(zlib),
                      EXTERN_INTERFACE(zlib)};

callback_t gzip_ck = {ALG_GZIP,
                      BASIC_INTERFACE(gzip),
                      INIT2_INTERFACE(gzip),
                      COMMPRESS2_INTERFACE(gzip),
                      NULL,
                      NULL,
                      HEADER_INTERFACE(gzip),
                      NULL,
                      NULL};

callback_t zip7_ck = {ALG_7ZIP, BASIC_INTERFACE(7zip),
                      NULL,     NULL,
                      NULL,     NULL,
                      NULL,     NULL,
                      NULL,     NULL,
                      NULL,     NULL};

callback_t brotli_ck = {ALG_BROTLI, BASIC_INTERFACE(brotli),
                        NULL,       NULL,
                        NULL,       NULL,
                        NULL,       NULL,
                        NULL,       NULL,
                        NULL,       NULL};

callback_t bzip2_ck = {ALG_BZIP2, BASIC_INTERFACE(bzip2),
                       NULL,      NULL,
                       NULL,      NULL,
                       NULL,      NULL,
                       NULL,      NULL,
                       NULL,      NULL};

callback_t lz4_ck = {ALG_LZ4, BASIC_INTERFACE(lz4),
                     NULL,    NULL,
                     NULL,    NULL,
                     NULL,    NULL,
                     NULL,    NULL,
                     NULL,    NULL};

callback_t snappy_ck = {ALG_SNAPPY, BASIC_INTERFACE(snappy),
                        NULL,       NULL,
                        NULL,       NULL,
                        NULL,       NULL,
                        NULL,       NULL,
                        NULL,       NULL};

callback_t xz_ck = {ALG_XZ, BASIC_INTERFACE(xz),
                    NULL,   NULL,
                    NULL,   NULL,
                    NULL,   NULL,
                    NULL,   NULL,
                    NULL,   NULL};

callback_t zstd_ck = {ALG_ZSTD, BASIC_INTERFACE(zstd),
                      NULL,     NULL,
                      NULL,     NULL,
                      NULL,     NULL,
                      NULL,     NULL,
                      NULL,     NULL};

#ifdef __aarch64__
callback_t uadk_ck = {ALG_UADK, BASIC_INTERFACE(uadk),
                      NULL,     NULL,
                      NULL,     NULL,
                      NULL,     NULL,
                      NULL,     NULL,
                      NULL,     NULL};

#else
callback_t uadk_ck = {ALG_UADK, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                      NULL,     NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                      NULL,     NULL, NULL, NULL, NULL, NULL, NULL};
#endif

const callback_t *alg_cks[] = {&zlib_ck,   &bzip2_ck, &brotli_ck, &lz4_ck,
                               &snappy_ck, &zstd_ck,  &zip7_ck,   &uadk_ck,
                               &xz_ck,     &gzip_ck};

int value = CASE_INVALID;
void SetValue(int v) { value = v; }

int UadkEnabled(void)
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

int check_algorithm(void)
{
    if (value == CASE_INVALID) {
        char *lib_value = getenv("UNIZIP_VALUE");
        if (lib_value != NULL) {
            value = atoi(lib_value);
        } else {
            value = CASE_ZLIB;
        }
    }
    if (value < CASE_ZLIB || value > CASE_GZIP) {
        ERR_CODE("Invalid OPTION value in environment variable.\n");
        return UNIZIP_ALGO_ERROR;
    } else if (value == CASE_UADK && UadkEnabled() != true) {
        ERR_CODE("UADK is disabled, could not use UADK.\n");
        return UNIZIP_ALGO_ERROR;
    }
    return 0;
}

// 获取软件包版本号
const char *unizip_Version(void)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        ERR_CODE("undifined algo\n");
        return "can't find algo!\n";
    }
    return alg_cks[value]->versionCB();
}

// 压缩器初始化
int unizip_deflateInit(unizip_streamp strm, int level)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->deflateInitCB(strm, level);
    // 用原版函数，强制类型转换即可
}

// 解压器初始化
int unizip_inflateInit(unizip_streamp strm)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->inflateInitCB(strm);
}

// 压缩结束
int unizip_deflateEnd(unizip_streamp strm)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->deflateEndCB(strm);
}

// 解压结束
int unizip_inflateEnd(unizip_streamp strm)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->inflateEndCB(strm);
}

// 压缩函数，根据压缩器的属性进行压缩
int unizip_deflate(unizip_streamp strm, int flush)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->deflateCB(strm, flush);
}

// 解压函数，根据解压器的属性进行解压
int unizip_inflate(unizip_streamp strm, int flush)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->inflateCB(strm, flush);
}

// Common API

// 压缩器复制
int unizip_deflateCopy(unizip_streamp dest, unizip_streamp source)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->deflateCopyCB(dest, source);
}

// 解压器复制
int unizip_inflateCopy(unizip_streamp dest, unizip_streamp source)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->inflateCopyCB(dest, source);
}

// 根据错误码输出错误信息
const char *unizip_zError(int err)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        ERR_CODE("undifined algo\n");
        return "undifined algo\n";
    }
    return zError(err);
}

// 压缩器重置，等价于先end再init
int unizip_deflateReset(unizip_streamp strm)
{
    // end + init
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->deflateResetCB(strm);
}

// 解压器重置，等价于先end再init
int unizip_inflateReset(unizip_streamp strm)
{
    // end + init
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->inflateResetCB(strm);
}

// 压缩，根据源文件和输出文件首地址及长度完成操作
int unizip_compress(Bytef *dest, uLongf *destLen, const Bytef *source,
                    uLong sourceLen)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->compressCB(dest, destLen, source, sourceLen);
}

// 解压，根据源文件和输出文件首地址及长度完成操作
int unizip_uncompress(Bytef *dest, uLongf *destLen, const Bytef *source,
                      uLong sourceLen)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->uncompressCB(dest, destLen, source, sourceLen);
}

// 压缩文件长度预估
uLong unizip_compressBound(uLong sourceLen)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->compressBoundCB(sourceLen);
}

// Unique API for Zlib

// zlib提供的压缩器初始化
int unizip_deflateInit2(unizip_streamp strm, int level, int method,
                        int windowBits, int memLevel, int strategy)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    if (NULL == alg_cks[value]->deflateInit2CB) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->deflateInit2CB(strm, level, method, windowBits,
                                          memLevel, strategy);
}

// zlib提供的解压器初始化
int unizip_inflateInit2(unizip_streamp strm, int windowBits)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    if (NULL == alg_cks[value]->inflateInit2CB) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->inflateInit2CB(strm, windowBits);
}

// zlib提供的压缩，根据源文件和输出文件首地址及长度完成操作
int unizip_compress2(Bytef *dest, uLongf *destLen, const Bytef *source,
                     uLong sourceLen, int level)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    if (NULL == alg_cks[value]->compress2CB) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->compress2CB(dest, destLen, source, sourceLen, level);
}

// zlib提供的解压，根据源文件和输出文件首地址及长度完成操作
int unizip_uncompress2(Bytef *dest, uLongf *destLen, const Bytef *source,
                       uLong *sourceLen)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    if (NULL == alg_cks[value]->uncompress2CB) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->uncompress2CB(dest, destLen, source, sourceLen);
}

// zlib提供的对属性压缩，根据压缩器属性完成操作
int unizip_deflateParams(unizip_streamp strm, int level, int strategy)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->deflateParamsCB(strm, level, strategy);
}

// zlib提供的对压缩器设置字典
int unizip_deflateSetDictionary(unizip_streamp strm, const Bytef *dictionary,
                                uInt dictLength)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->deflateSetDictionaryCB(strm, dictionary, dictLength);
}

// zlib提供的填充字
int unizip_deflatePending(unizip_streamp strm, unsigned *pending, int *bits)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->deflatePendingCB(strm, pending, bits);
}

// zlib提供的设置头部
int unizip_deflateSetHeader(unizip_streamp strm, gz_headerp head)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->deflateSetHeaderCB(strm, head);
}

// zlib提供的对解压器设置字典
int unizip_inflateSetDictionary(unizip_streamp strm, const Bytef *dictionary,
                                uInt dictLength)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->inflateSetDictionaryCB(strm, dictionary, dictLength);
}

// zlib提供的获取头部
int unizip_inflateGetHeader(unizip_streamp strm, gz_headerp head)
{
    if (check_algorithm() == UNIZIP_ALGO_ERROR) {
        return UNIZIP_ALGO_ERROR;
    }
    return alg_cks[value]->inflateGetHeaderCB(strm, head);
}
