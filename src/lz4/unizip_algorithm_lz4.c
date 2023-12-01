/******************************************************************************
 * unizip_callback_lz4.h
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * ge-ziwen <hugin1_1@sina.com>
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
#include "unizip_algorithm_lz4.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "unizip_error_code.h"

void *lz4_allocate(void)
{
    struct lz4_ctx *ctx;

    if ((ctx = (struct lz4_ctx *)malloc(sizeof(*ctx))) == NULL) {
        // 报错分配失败信息
        return NULL;
    }
    return ctx;
}

int lz4_copy(struct lz4_ctx *dest, struct lz4_ctx *source)
{
    if (dest != NULL && source != NULL) {
        memcpy(dest, source, sizeof(struct lz4_ctx));
        return UNIZIP_OK;
    }

    return UNIZIP_DATA_ERROR;
}

unsigned long lz4_compressBound(unsigned long uncompressed_size)
{
    /*
    The upper bound size shoud be 0x7E000000 bytes in lz4
    Same as setting of LZ4_COMPRESSBOUND macro in bottom.
    */
    return uncompressed_size > LZ4_MAX_INPUT_SIZE
               ? 0
               : (uncompressed_size) + (((uncompressed_size) / 255) + 16);
}

int lz4_compress(char *dest, unsigned long *destLen, char *source,
                 unsigned long sourceLen)
{
    if (dest == NULL || destLen == NULL || source == NULL)
        return UNIZIP_DATA_ERROR;
    int result =
        LZ4_compress_default(source, dest, (int)sourceLen, (int)*destLen);
    // In 1.9.2 return 0 if the compression failed
    if (result <= 0)
        return UNIZIP_BUF_ERROR;
    (*destLen) = (unsigned long)result;
    return UNIZIP_OK;
}

int lz4_decompress(char *dest, unsigned long *destLen, const char *source,
                   unsigned long sourceLen)
{
    if (dest == NULL || destLen == NULL || source == NULL) {
        return UNIZIP_DATA_ERROR;
    }

    int result =
        LZ4_decompress_safe(source, dest, (int)sourceLen, (int)*destLen);
    if (result <= 0)
        return UNIZIP_BUF_ERROR;
    (*destLen) = (unsigned long)result;
    return UNIZIP_OK;
}

int lz4_deflateInit(struct lz4_ctx *ctx, int level)
{
    if (ctx == NULL)
        return UNIZIP_STREAM_ERROR;
    if (levelMin_lz4 < level && level <= levelMax_lz4) {
        ctx->level = level;
    } else {
        ctx->level = 1; // 模仿设置压缩等级的行为
    }
    // 在上层传值之前
    ctx->avail_in = 0;
    ctx->next_in = NULL;
    ctx->avail_out = 0;
    ctx->next_out = NULL;
    ctx->total_in = 0;
    ctx->total_out = 0;

    ctx->src = NULL;
    ctx->dst = NULL;
    ctx->input_length = 0;

    return UNIZIP_OK;
}

int lz4_deflateprocess(struct lz4_ctx *ctx)
{
    ctx->input_length = ctx->total_in;
    int ret =
        LZ4_compress_default(ctx->src, ctx->dst, ctx->total_in, ctx->avail_out);
    if (ret <= 0)
        return UNIZIP_BUF_ERROR; // too small output

    ctx->total_out = ret; // compressed size
    // 调整输出
    ctx->next_out = ctx->dst + ret;
    ctx->next_in += ctx->input_length;
    ctx->avail_out = ctx->avail_out - ret;
    ctx->dst = ctx->next_out;
    ctx->avail_in = 0;
    return UNIZIP_STREAM_END; // 返回END如果成功了
}

int lz4_deflate(struct lz4_ctx *ctx, int flush)
{
    /*
    Since only the block compress APT exist in LZ4.
    */
    if (flush == 0) {
        return UNIZIP_OK;
    } else if (flush == zlibAction_end) {
        ctx->src = ctx->next_in;
        ctx->dst = ctx->next_out;
        ctx->total_in = ctx->avail_in;
        ctx->avail_in = 0;
        return lz4_deflateprocess(ctx);
    } else {
        return UNIZIP_ERRNO;
    }
    return UNIZIP_OK;
}

int lz4_end(struct lz4_ctx *ctx)
{
    if (ctx) {
        free(ctx);
        return UNIZIP_OK;
    }
    return UNIZIP_BUF_ERROR;
}

int lz4_deflateEnd(struct lz4_ctx *ctx)
{
    /*
     * End the resource.
     */
    if (ctx) {
        free(ctx);
        return UNIZIP_OK;
    }
    return UNIZIP_BUF_ERROR;
}

int lz4_inflateInit(struct lz4_ctx *ctx)
{
    /*
    Default: Dictionary compression is dismissed.
    */
    if (ctx == NULL)
        return UNIZIP_STREAM_ERROR;
    ctx->avail_in = 0;
    ctx->avail_out = 0;
    ctx->next_in = NULL;
    ctx->next_out = NULL;
    ctx->total_in = 0;
    ctx->total_out = 0;

    ctx->src = NULL;
    ctx->dst = NULL;
    ctx->input_length = 0;

    return UNIZIP_OK;
}

int lz4_inflateprocess(struct lz4_ctx *ctx)
{
    /*
    zlib的deflate和inflate主要由两种语义状态，尽可能地使用块状压缩模仿，需要设计对外表示地属性。
    */
    ctx->input_length = ctx->total_in;
    int ret =
        LZ4_decompress_safe(ctx->src, ctx->dst, ctx->total_in, ctx->avail_out);
    if (ret <= 0)
        return UNIZIP_BUF_ERROR; // too small output

    ctx->total_out = ret; // compressed size
    // 调整输出
    ctx->next_out = ctx->dst + ret;
    ctx->next_in += ctx->input_length;
    ctx->avail_out = ctx->avail_out - ret;
    ctx->dst = ctx->next_out;
    ctx->avail_in = 0;
    return UNIZIP_STREAM_END; // 返回END如果成功了
}

int lz4_inflate(struct lz4_ctx *ctx, int flush)
{
    ctx->src = ctx->next_in;
    ctx->dst = ctx->next_out;
    ctx->total_in = ctx->avail_in;
    ctx->avail_in = 0;
    return lz4_inflateprocess(ctx);
};

int lz4_inflateEnd(struct lz4_ctx *ctx)
{
    if (ctx) {
        free(ctx);
    }
    return UNIZIP_OK;
}
