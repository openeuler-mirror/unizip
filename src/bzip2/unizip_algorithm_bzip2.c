/******************************************************************************
 * unizip_algorithm_bzip2.c
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
#include "unizip_algorithm_bzip2.h"
#include <limits.h>
#include <stdlib.h> // 包含 malloc 和 free 的声明
#include <string.h> // 包含 memcpy 的声明

#define levelMin_bzip2 1
#define levelMax_bzip2 9
#define zlibAction_end 4

// 用宏定义将zlib的错误码保存在.h文件中，重构变量名
static int map_error(int ret)
{
    switch (ret) {
        case BZ_FINISH_OK:
        case BZ_FLUSH_OK:
        case BZ_OK:
        case BZ_RUN_OK:
            return UNIZIP_OK;

        case BZ_STREAM_END:
            return UNIZIP_STREAM_END;
        case BZ_PARAM_ERROR:
        case BZ_DATA_ERROR:
        case BZ_DATA_ERROR_MAGIC:
        case BZ_UNEXPECTED_EOF:
            return UNIZIP_DATA_ERROR;

        case BZ_MEM_ERROR:
            return UNIZIP_MEM_ERROR;

        case BZ_CONFIG_ERROR: /* actually, bzip2 miscompiled */
        case BZ_IO_ERROR:
        case BZ_OUTBUFF_FULL:
        case BZ_SEQUENCE_ERROR:
            return UNIZIP_STREAM_ERROR;
        default:
            return UNIZIP_DATA_ERROR;
    }
}
unsigned long bzip2_totalsize(unsigned int size_hi, unsigned int size_lo)
{
    return ((unsigned long)size_hi << 32) + (unsigned long)size_lo;
}

void *bzip2_allocate(void)
{
    struct bzip2_ctx *ctx;
    if ((ctx = (struct bzip2_ctx *)malloc(sizeof(*ctx))) == NULL) {
        return NULL;
    }
    ctx->bzstr.bzalloc = NULL;
    ctx->bzstr.bzfree = NULL;
    ctx->bzstr.opaque = NULL;
    return ctx;
}

int bzip2_copy(struct bzip2_ctx *dest, struct bzip2_ctx *source)
{
    if (dest != NULL && source != NULL) {
        memcpy(dest, source, sizeof(struct bzip2_ctx));
        return UNIZIP_OK;
    }
    return UNIZIP_DATA_ERROR;
}

int bzip2_deflateInit(struct bzip2_ctx *ctx, int level)
{
    int ret;
    ctx->bzstr.avail_in = 0;
    ctx->bzstr.next_in = NULL;
    ctx->bzstr.avail_out = 0;
    ctx->bzstr.next_out = NULL;
    int tempLevel = level;
    if (tempLevel < levelMin_bzip2 || tempLevel > levelMax_bzip2) {
        tempLevel = levelMax_bzip2;
    }
    ret = BZ2_bzCompressInit(&ctx->bzstr, tempLevel, 0, 0);
    return map_error(ret);
}
int bzip2_inflateInit(struct bzip2_ctx *ctx)
{
    int ret;

    ctx->bzstr.avail_in = 0;
    ctx->bzstr.next_in = NULL;
    ctx->bzstr.avail_out = 0;
    ctx->bzstr.next_out = NULL;

    ret = BZ2_bzDecompressInit(&ctx->bzstr, 0, 0);

    return map_error(ret);
}

int bzip2_deflateEnd(struct bzip2_ctx *ctx)
{
    int err = 0;
    if (ctx != NULL) {
        err = BZ2_bzCompressEnd(&ctx->bzstr);
        free(ctx);
    }
    return map_error(err);
}

int bzip2_inflateEnd(struct bzip2_ctx *ctx)
{
    int err = 0;
    if (ctx != NULL) {
        err = BZ2_bzDecompressEnd(&ctx->bzstr);
        free(ctx);
    }
    return map_error(err);
}

int bzip2_deflate(struct bzip2_ctx *ctx, int flush)
{
    int action = (flush == zlibAction_end) ? BZ_FINISH : BZ_RUN;
    ctx->bzstr.next_in = (char *)ctx->next_in;
    ctx->bzstr.next_out = (char *)ctx->next_out;

    if (ctx->bzstr.avail_in == 0 &&
        (action != BZ_FINISH ||
         (action == BZ_FINISH && ctx->bzstr.total_in_hi32 == 0 &&
          ctx->bzstr.total_in_lo32 == 0))) {
        ctx->bzstr.avail_in = ctx->avail_in;
    }
    if (ctx->bzstr.avail_out == 0) {
        ctx->bzstr.avail_out = ctx->avail_out;
    }
    int ret = BZ2_bzCompress(&ctx->bzstr, action);
    if (ret != BZ_RUN_OK && ret != BZ_FINISH_OK && ret != BZ_STREAM_END) {
        return map_error(ret);
    }
    ctx->next_in +=
        bzip2_totalsize(ctx->bzstr.total_in_hi32, ctx->bzstr.total_in_lo32) -
        ctx->total_in;
    ctx->next_out +=
        bzip2_totalsize(ctx->bzstr.total_out_hi32, ctx->bzstr.total_out_lo32) -
        ctx->total_out;
    ctx->total_in =
        bzip2_totalsize(ctx->bzstr.total_in_hi32, ctx->bzstr.total_in_lo32);
    ctx->total_out =
        bzip2_totalsize(ctx->bzstr.total_out_hi32, ctx->bzstr.total_out_lo32);

    return map_error(ret);
}

int bzip2_inflate(struct bzip2_ctx *ctx)
{
    ctx->bzstr.next_in = (char *)ctx->next_in;
    ctx->bzstr.next_out = (char *)ctx->next_out;
    if (ctx->bzstr.avail_in == 0) {
        ctx->bzstr.avail_in = ctx->avail_in;
    }
    if (ctx->bzstr.avail_out == 0) {
        ctx->bzstr.avail_out = ctx->avail_out;
    }
    int ret = BZ2_bzDecompress(&ctx->bzstr);
    if (ret != BZ_OK && ret != BZ_STREAM_END) {
        return map_error(ret);
    }
    ctx->next_in +=
        bzip2_totalsize(ctx->bzstr.total_in_hi32, ctx->bzstr.total_in_lo32) -
        ctx->total_in;
    ctx->next_out +=
        bzip2_totalsize(ctx->bzstr.total_out_hi32, ctx->bzstr.total_out_lo32) -
        ctx->total_out;
    ctx->total_in =
        bzip2_totalsize(ctx->bzstr.total_in_hi32, ctx->bzstr.total_in_lo32);
    ctx->total_out =
        bzip2_totalsize(ctx->bzstr.total_out_hi32, ctx->bzstr.total_out_lo32);
    return map_error(ret);
}

int bzip2_compress(char *dest, unsigned long *destLen, char *source,
                   unsigned long sourceLen)
{
    int ret =
        BZ2_bzBuffToBuffCompress(dest, (unsigned int *)destLen, (char *)source,
                                 (unsigned int)sourceLen, levelMax_bzip2, 0, 0);
    return map_error(ret);
}

int bzip2_decompress(char *dest, unsigned long *destLen, char *source,
                     unsigned long sourceLen)
{
    int ret = BZ2_bzBuffToBuffDecompress(dest, (unsigned int *)destLen, source,
                                         (unsigned int)sourceLen, 0, 0);
    return map_error(ret);
}

unsigned long bzip2_compressBound(unsigned long uncompressed_size)
{
    // According to https://en.wikipedia.org/wiki/Bzip2
    unsigned long compressed_size =
        (unsigned long)((double)uncompressed_size * 1.25);

    if (compressed_size < uncompressed_size) {
        return ULONG_MAX;
    }
    return compressed_size;
}
