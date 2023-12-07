/******************************************************************************
 * unizip_callback_lz4.c
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * ge-ziwen <1109674186@qq.com>
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
#ifndef SRC_LZ4_UNIZIP_ALGORITHM_LZ4_H_
#define SRC_LZ4_UNIZIP_ALGORITHM_LZ4_H_
#include <lz4.h>
#include "unizip_error_code.h"
#define levelMin_lz4 0
#define levelMax_lz4 65537
#define zlibAction_end 4
struct lz4_ctx {
    char *next_in;
    char *next_out;
    unsigned int avail_in;
    unsigned int avail_out;
    uint64_t total_in;
    uint64_t total_out;
    int level;

    // for reserve purpose
    int input_length;
    char *src;
    char *dst;
};

void *lz4_allocate(void);

int lz4_deflateInit(struct lz4_ctx *ctx, int level);

int lz4_inflateInit(struct lz4_ctx *ctx);

int lz4_deflateEnd(struct lz4_ctx *ctx);

int lz4_inflateEnd(struct lz4_ctx *ctx);

int lz4_inflate(struct lz4_ctx *ctx, int flush);

int lz4_deflate(struct lz4_ctx *ctx, int flush);

int lz4_end(struct lz4_ctx *ctx);

// 完成一个深拷贝
int lz4_copy(struct lz4_ctx *dest, struct lz4_ctx *source);

unsigned long lz4_compressBound(unsigned long uncompressed_size);

int lz4_compress(char *dest, unsigned long *destLen, char *source,
                 unsigned long sourceLen);

int lz4_decompress(char *dest, unsigned long *destLen, const char *source,
                   unsigned long sourceLen);

#endif // SRC_LZ4_UNIZIP_ALGORITHM_LZ4_H_
