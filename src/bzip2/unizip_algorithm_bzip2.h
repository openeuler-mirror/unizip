/******************************************************************************
 * unizip_algorithm_bzip2.h
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
#ifndef SRC_BZIP2_UNIZIP_ALGORITHM_BZIP2_H_
#define SRC_BZIP2_UNIZIP_ALGORITHM_BZIP2_H_
#include <bzlib.h>
#include <stdint.h>
#include <unizip_error_code.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bzip2_ctx {
    bz_stream bzstr;
    char *next_in;
    char *next_out;
    unsigned int avail_in;
    unsigned int avail_out;
    unsigned long total_in;
    unsigned long total_out;
};

// unizip_algorithm_xz.c
void *bzip2_allocate(void);

int bzip2_deflateInit(struct bzip2_ctx *ctx, int level);

int bzip2_inflateInit(struct bzip2_ctx *ctx);

int bzip2_deflateEnd(struct bzip2_ctx *ctx);

int bzip2_inflateEnd(struct bzip2_ctx *ctx);

int bzip2_inflate(struct bzip2_ctx *ctx);

int bzip2_deflate(struct bzip2_ctx *ctx, int flush);

int bzip2_copy(struct bzip2_ctx *dest, struct bzip2_ctx *source);

int bzip2_decompress(char *dest, unsigned long *destLen, char *source,
                     unsigned long sourceLen);

int bzip2_compress(char *dest, unsigned long *destLen, char *source,
                   unsigned long sourceLen);

unsigned long bzip2_compressBound(unsigned long uncompressed_size);

#ifdef __cplusplus
}
#endif

#endif // SRC_BZIP2_UNIZIP_ALGORITHM_BZIP2_H_
