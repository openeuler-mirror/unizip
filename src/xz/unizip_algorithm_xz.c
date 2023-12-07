/******************************************************************************
 * unizip_algorithm_xz.c
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
#include "unizip_algorithm_xz.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define levelMin_xz 0
#define levelMax_xz 9
#define zlibAction_end 4

int map_error(lzma_ret ret)
{
    switch (ret) {
        case LZMA_OK:
            return UNIZIP_OK;
        case LZMA_STREAM_END:
            return UNIZIP_STREAM_END;
        case LZMA_DATA_ERROR:
        case LZMA_UNSUPPORTED_CHECK:
            return UNIZIP_DATA_ERROR;

        case LZMA_MEM_ERROR:
            return UNIZIP_MEM_ERROR;

        case LZMA_OPTIONS_ERROR:
            return UNIZIP_STREAM_ERROR;

        default:
            return UNIZIP_DATA_ERROR;
    }
}

void *xz_allocate(void)
{
    struct xz_ctx *ctx;
    if ((ctx = (struct xz_ctx *)malloc(sizeof(*ctx))) == NULL) {
        return NULL;
    }
    memset(&ctx->xzstr, 0, sizeof(ctx->xzstr));
    return ctx;
}

int xz_copy(struct xz_ctx *dest, struct xz_ctx *source)
{
    if (dest != NULL && source != NULL) {
        memcpy(dest, source, sizeof(struct xz_ctx));
        return UNIZIP_OK;
    }
    return UNIZIP_MEM_ERROR;
}

int xz_deflateInit(struct xz_ctx *ctx, int level)
{
    lzma_options_lzma opt_lzma2;
    int tmpLevel = level;
    if (tmpLevel < levelMin_xz || tmpLevel > levelMax_xz) {
        tmpLevel = LZMA_PRESET_DEFAULT;
    }
    if (lzma_lzma_preset(&opt_lzma2, tmpLevel)) {
        return UNIZIP_DATA_ERROR;
    }
    lzma_filter filters[] = {
        {.id = LZMA_FILTER_X86, .options = NULL},
        {.id = LZMA_FILTER_LZMA2, .options = &opt_lzma2},
        {.id = LZMA_VLI_UNKNOWN, .options = NULL},
    };
    ctx->xzstr.avail_in = 0;
    ctx->xzstr.next_in = NULL;
    ctx->xzstr.avail_out = 0;
    ctx->xzstr.next_out = NULL;
    lzma_ret ret = lzma_stream_encoder(&ctx->xzstr, filters, LZMA_CHECK_CRC64);
    return map_error(ret);
}

int xz_inflateInit(struct xz_ctx *ctx)
{
    lzma_stream tmp = LZMA_STREAM_INIT;
    ctx->xzstr = tmp;
    ctx->xzstr.next_in = NULL;
    ctx->xzstr.avail_out = 0;
    ctx->xzstr.next_out = NULL;
    lzma_ret ret =
        lzma_stream_decoder(&ctx->xzstr, UINT64_MAX, LZMA_CONCATENATED);
    return map_error(ret);
}

int xz_End(struct xz_ctx *ctx)
{
    if (ctx != NULL) {
        lzma_end(&ctx->xzstr);
        free(ctx);
    }
    return UNIZIP_OK;
}

int xz_compress(uint8_t *source, size_t sourceLen, uint8_t *dest,
                size_t *destLen)
{
    lzma_stream xzstr = LZMA_STREAM_INIT;
    lzma_options_lzma opt_lzma2;
    if (lzma_lzma_preset(&opt_lzma2, LZMA_PRESET_DEFAULT)) {
        return UNIZIP_DATA_ERROR;
    }
    lzma_filter filters[] = {
        {.id = LZMA_FILTER_X86, .options = NULL},
        {.id = LZMA_FILTER_LZMA2, .options = &opt_lzma2},
        {.id = LZMA_VLI_UNKNOWN, .options = NULL},
    };
    lzma_ret ret = lzma_stream_encoder(&xzstr, filters, LZMA_CHECK_CRC64);

    if (dest == NULL || destLen == NULL || source == NULL)
        return UNIZIP_DATA_ERROR;

    xzstr.next_in = source;
    xzstr.next_out = dest;
    xzstr.avail_in = sourceLen;
    xzstr.avail_out = *destLen;
    ret = lzma_code(&xzstr, LZMA_FINISH);
    if (ret == LZMA_STREAM_END) {
        *destLen -= xzstr.avail_out;
        return UNIZIP_OK; // Decompression is complete
    }
    return map_error(ret);
}

int xz_decompress(uint8_t *source, size_t sourceLen, uint8_t *dest,
                  size_t *destLen)
{
    if (dest == NULL || destLen == NULL || source == NULL)
        return UNIZIP_DATA_ERROR;
    lzma_stream xzstr = LZMA_STREAM_INIT;
    lzma_ret ret =
        lzma_stream_decoder(&xzstr, UINT64_MAX, LZMA_CONCATENATED);
    xzstr.next_in = source;
    xzstr.next_out = dest;
    xzstr.avail_in = sourceLen;
    xzstr.avail_out = *destLen;
    ret = lzma_code(&xzstr, LZMA_FINISH);
    if (ret == LZMA_STREAM_END) {
        *destLen -= xzstr.avail_out;
        return UNIZIP_OK; // Decompression is complete
    }
    return map_error(ret);
}

int xz_deflate(struct xz_ctx *ctx, int flush)
{
    lzma_action action = (flush == zlibAction_end) ? LZMA_FINISH : LZMA_RUN;
    ctx->xzstr.next_in = (uint8_t *)ctx->next_in;
    ctx->xzstr.next_out = (uint8_t *)ctx->next_out;
    if (ctx->xzstr.avail_in == 0 &&
        (action != LZMA_FINISH ||
         (action == LZMA_FINISH && ctx->total_in == 0))) {
        ctx->xzstr.avail_in = ctx->avail_in;
    }
    if (ctx->xzstr.avail_out == 0) {
        ctx->xzstr.avail_out = ctx->avail_out;
    }
    lzma_ret ret = lzma_code(&ctx->xzstr, action);
    ctx->next_in += ctx->xzstr.total_in - ctx->total_in;
    ctx->next_out += ctx->xzstr.total_out - ctx->total_out;
    ctx->total_in = ctx->xzstr.total_in;
    ctx->total_out = ctx->xzstr.total_out;
    return map_error(ret);
}

int xz_inflate(struct xz_ctx *ctx, int flush)
{
    lzma_action action = (flush == zlibAction_end) ? LZMA_FINISH : LZMA_RUN;
    ctx->xzstr.next_in = (uint8_t *)ctx->next_in;
    ctx->xzstr.next_out = (uint8_t *)ctx->next_out;
    if (ctx->xzstr.avail_in == 0) {
        ctx->xzstr.avail_in = ctx->avail_in;
    }
    if (ctx->xzstr.avail_out == 0) {
        ctx->xzstr.avail_out = ctx->avail_out;
    }
    lzma_ret ret = lzma_code(&ctx->xzstr, action);
    ctx->next_in += ctx->xzstr.total_in - ctx->total_in;
    ctx->next_out += ctx->xzstr.total_out - ctx->total_out;
    ctx->total_in = ctx->xzstr.total_in;
    ctx->total_out = ctx->xzstr.total_out;
    return map_error(ret);
}

unsigned long xz_compressBound(unsigned long uncompressed_size)
{
    // According to
    // https://sourceforge.net/p/sevenzip/discussion/45797/thread/b6bd62f8/
    unsigned long compressed_size =
        (unsigned long)((double)uncompressed_size * 1.001) + 1024 + 13;
    if (compressed_size < uncompressed_size) {
        return ULLONG_MAX;
    }
    return compressed_size;
}
