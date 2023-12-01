/******************************************************************************
 * unizip_algorithm_xz.h
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
#ifndef SRC_XZ_UNIZIP_ALGORITHM_XZ_H_
#define SRC_XZ_UNIZIP_ALGORITHM_XZ_H_
#include <lzma.h>
#include <unizip_error_code.h>

#ifdef __cplusplus
extern "C" {
#endif
struct xz_ctx {
    lzma_stream xzstr;
    // 把next_in等属性补充在这里
    char *next_in;
    char *next_out;
    unsigned int avail_in;
    unsigned int avail_out;
    uint64_t total_in;
    uint64_t total_out;
};

// unizip_algorithm_xz.c

void *xz_allocate(void);

int xz_deflateInit(struct xz_ctx *ctx, int level);

int xz_inflateInit(struct xz_ctx *ctx);

int xz_End(struct xz_ctx *ctx);

int xz_decompress(uint8_t *source, size_t sourceLen, uint8_t *dest,
                  size_t *destLen);

int xz_compress(uint8_t *source, size_t sourceLen, uint8_t *dest,
                size_t *destLen);

int xz_deflate(struct xz_ctx *ctx, int flush);

int xz_inflate(struct xz_ctx *ctx, int flush);

int xz_copy(struct xz_ctx *dest, struct xz_ctx *source);

unsigned long xz_compressBound(unsigned long uncompressed_size);

#ifdef __cplusplus
}
#endif

#endif // SRC_XZ_UNIZIP_ALGORITHM_XZ_H_
