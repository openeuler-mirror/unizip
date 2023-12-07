/******************************************************************************
 * unizip_callback_bzip2.c
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
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
#ifndef SRC_ZSTD_UNIZIP_ALGORITHM_ZSTD_H_
#define SRC_ZSTD_UNIZIP_ALGORITHM_ZSTD_H_
#include <stdint.h>
#include <unizip_error_code.h>
#include <zstd.h>
#include <zstd_errors.h>

#ifdef __cplusplus
extern "C" {
#endif

struct zstd_ctx {
    ZSTD_DStream *zdstream;
    ZSTD_CStream *zcstream;
    ZSTD_outBuffer out;
    ZSTD_inBuffer in;
    char *next_in;
    char *next_out;
    unsigned int avail_in;
    unsigned int avail_out;
    unsigned long total_in;
    unsigned long total_out;
};

// unizip_algorithm_xz.c
void *zstd_allocate(void);

int zstd_deflateInit(struct zstd_ctx *ctx, int level);

int zstd_inflateInit(struct zstd_ctx *ctx);

int zstd_deflateEnd(struct zstd_ctx *ctx);

int zstd_inflateEnd(struct zstd_ctx *ctx);

int zstd_inflate(struct zstd_ctx *ctx);

int zstd_deflate(struct zstd_ctx *ctx, int flush);

int zstd_deflatecopy(struct zstd_ctx *dest, struct zstd_ctx *source);

int zstd_inflatecopy(struct zstd_ctx *dest, struct zstd_ctx *source);

int zstd_decompress(char *dest, unsigned long *destLen, const char *source, unsigned long sourceLen);

int zstd_compress(char *dest, unsigned long *destLen, const char *source, unsigned long sourceLen);

unsigned long zstd_compressBound(unsigned long uncompressed_size);

#ifdef __cplusplus
}
#endif

#endif // SRC_ZSTD_UNIZIP_ALGORITHM_ZSTD_H_
