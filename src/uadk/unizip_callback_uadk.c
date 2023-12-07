/******************************************************************************
 * unizip_callback_uadk.c
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * jiang-chenyang <jiangcy1129@163.com>
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

#ifdef __aarch64__

#include "unizip_callback_uadk.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "unizip_algorithm_uadk.h"

#define ALG_TYPE WCRYPTO_GZIP
#define DEFAULT_STREAM_CHUNK (1024 * 64)
#define MAX_STREAM_CHUNK (1024 * 1024)
#define DEFLATE_MIN_CHUNK 256
#define INFLATE_MIN_CHUNK 32
#define POSSIBLE_INCREASE 0.1

const char *versionCB_uadk(void) { return uadk_getVersion(); }

void resetUadk(unizip_streamp strm)
{
    strm->next_in = NULL;
    strm->next_out = NULL;
    strm->avail_in = 0;
    strm->avail_out = 0;
    strm->total_in = 0;
    strm->total_out = 0;
}

int deflateInitCB_uadk(unizip_streamp strm, int level)
{
    if ((strm->ud = (struct zip_stream *)malloc(sizeof(struct zip_stream))) == NULL) {
        return UNIZIP_MEM_ERROR;
    }
    int ret = UNIZIP_OK;
    int algType = strm->compression_flag;
    if (algType == WCRYPTO_ZLIB) {
        ret = hw_deflateInit(strm->ud, level);
    } else if (algType == WCRYPTO_GZIP) {
        int windowBits = 15;
        int gzipEncoding = 16;
        ret =
            hw_deflateInit2(strm->ud, level, Z_DEFLATED, windowBits | gzipEncoding, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    } else {
        ret = UNIZIP_DATA_ERROR;
    }
    resetUadk(strm);
    return ret;
}

int deflateCB_uadk(unizip_streamp strm, int flush)
{
    if (flush == Z_FINISH) {
        flush = WCRYPTO_FINISH;
    } else if (flush == Z_NO_FLUSH) {
        flush = WCRYPTO_NO_FLUSH;
    }
    struct zip_stream *ctx = (struct zip_stream *)strm->ud;
    unsigned char *src = (unsigned char *)strm->next_in;
    unsigned char *dst = (unsigned char *)strm->next_out;
    unsigned long in_stream_chunk = (unsigned long)strm->avail_in;
    unsigned long out_stream_chunk = (unsigned long)strm->avail_out;
    if (out_stream_chunk < DEFLATE_MIN_CHUNK) {
        out_stream_chunk = DEFLATE_MIN_CHUNK;
    }
    if (out_stream_chunk > MAX_STREAM_CHUNK) {
        out_stream_chunk = MAX_STREAM_CHUNK;
    }
    ctx->total_in = (unsigned long)strm->total_in;
    ctx->total_out = (unsigned long)strm->total_out;
    unsigned long srclen = in_stream_chunk; // 全部输入数据
    int err = 0;
    do {
        if (srclen > MAX_STREAM_CHUNK) {
            memcpy(ctx->next_in, src, MAX_STREAM_CHUNK);
            src += MAX_STREAM_CHUNK;
            ctx->avail_in = MAX_STREAM_CHUNK;
            srclen -= MAX_STREAM_CHUNK;
        } else {
            memcpy(ctx->next_in, src, srclen);
            src += srclen;
            ctx->avail_in = srclen;
            srclen = 0;
        }
        do {
            ctx->avail_out = out_stream_chunk;
            err = hw_deflate(ctx, flush);
            if (err < 0) {
                return err;
            }
            int have = out_stream_chunk - ctx->avail_out;
            memcpy(dst, ctx->next_out, have);
            dst += have;
        } while (ctx->avail_in > 0);
    } while (srclen > 0);
    strm->next_in = src;
    strm->next_out = dst;
    strm->avail_in = ctx->avail_in;
    strm->avail_out = ctx->avail_out;
    strm->total_in = ctx->total_in;
    strm->total_out = ctx->total_out;
    return err;
}

int deflateEndCB_uadk(unizip_streamp strm)
{
    resetUadk(strm);
    return hw_deflateEnd(strm->ud);
}

int inflateInitCB_uadk(unizip_streamp strm)
{
    if ((strm->ud = (struct zip_stream *)malloc(sizeof(struct zip_stream))) == NULL) {
        return UNIZIP_MEM_ERROR;
    }
    int ret = UNIZIP_OK;
    int algType = strm->compression_flag;
    if (algType == WCRYPTO_ZLIB) {
        ret = hw_inflateInit(strm->ud);
    } else if (algType == WCRYPTO_GZIP) {
        int gzipEncoding = 16;
        ret = hw_inflateInit2(strm->ud, gzipEncoding + MAX_WBITS);
    } else {
        ret = UNIZIP_DATA_ERROR;
    }
    resetUadk(strm);
    return ret;
}

int inflateCB_uadk(unizip_streamp strm, int flush)
{
    if (flush == Z_NO_FLUSH) {
        flush = WCRYPTO_NO_FLUSH;
    } else if (flush == Z_FINISH) {
        flush = WCRYPTO_FINISH;
    }
    struct zip_stream *ctx = (struct zip_stream *)strm->ud;
    unsigned char *src = (unsigned char *)strm->next_in;
    unsigned char *dst = (unsigned char *)strm->next_out;
    unsigned long in_stream_chunk = (unsigned long)strm->avail_in;
    unsigned long out_stream_chunk = (unsigned long)strm->avail_out;
    if (out_stream_chunk < INFLATE_MIN_CHUNK) {
        out_stream_chunk = INFLATE_MIN_CHUNK;
    }
    if (out_stream_chunk > MAX_STREAM_CHUNK) {
        out_stream_chunk = MAX_STREAM_CHUNK;
    }
    unsigned long srclen = in_stream_chunk; // 全部输入数据
    ctx->total_in = (unsigned long)strm->total_in;
    ctx->total_out = (unsigned long)strm->total_out;
    int err = 0;
    do {
        if (srclen > MAX_STREAM_CHUNK) {
            memcpy(ctx->next_in, src, MAX_STREAM_CHUNK);
            src += MAX_STREAM_CHUNK;
            ctx->avail_in = MAX_STREAM_CHUNK;
            srclen -= MAX_STREAM_CHUNK;
        } else {
            memcpy(ctx->next_in, src, srclen);
            src += srclen;
            ctx->avail_in = srclen;
            srclen = 0;
        }
        if (ctx->avail_in == 0) {
            err = UNIZIP_STREAM_END;
            break;
        }
        do {
            ctx->avail_out = out_stream_chunk;
            err = hw_inflate(ctx, flush);
            if (err < 0) {
                return err;
            }
            int have = out_stream_chunk - ctx->avail_out;
            memcpy(dst, ctx->next_out, have);
            dst += have;
        } while (ctx->avail_in > 0);
    } while (srclen > 0);
    strm->next_in = src;
    strm->next_out = dst;
    strm->avail_in = ctx->avail_in;
    strm->avail_out = ctx->avail_out;
    strm->total_in = ctx->total_in;
    strm->total_out = ctx->total_out;
    return err;
}

int inflateEndCB_uadk(unizip_streamp strm)
{
    resetUadk(strm);
    return hw_inflateEnd(strm->ud);
}

int deflateCopyCB_uadk(unizip_streamp dest, unizip_streamp source)
{
    dest->compression_flag = source->compression_flag;
    return deflateInitCB_uadk(dest, Z_DEFAULT_COMPRESSION);
}

int inflateCopyCB_uadk(unizip_streamp dest, unizip_streamp source)
{
    dest->compression_flag = source->compression_flag;
    return inflateInitCB_uadk(dest);
}

int deflateResetCB_uadk(unizip_streamp strm)
{
    hw_deflateEnd(strm->ud);
    return deflateInitCB_uadk(strm, Z_DEFAULT_COMPRESSION);
}

int inflateResetCB_uadk(unizip_streamp strm)
{
    hw_inflateEnd(strm->ud);
    return inflateInitCB_uadk(strm);
}

int compressCB_uadk(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    int ret = hw_stream_compress(ALG_TYPE, DEFAULT_STREAM_CHUNK, (unsigned char *)dest, (ulong *)destLen,
                                 (unsigned char *)source, sourceLen);
    return ret;
}

int uncompressCB_uadk(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    int ret = hw_stream_decompress(ALG_TYPE, DEFAULT_STREAM_CHUNK, (unsigned char *)dest, (ulong *)destLen,
                                   (unsigned char *)source, sourceLen);
    return ret;
}

uLong compressBoundCB_uadk(uLong sourceLen)
{
    return sourceLen * (1 + POSSIBLE_INCREASE) + (sourceLen >> 12) + (sourceLen >> 14) + (sourceLen >> 25) + 13;
}

#endif // __aarch64__
