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

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "unizip_algorithm_uadk.h"
#include "unizip_callback_uadk.h"

#define ALG_TYPE WCRYPTO_GZIP
#define DEFAULT_STREAM_CHUNK (1024 * 64)
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
    if ((strm->ud = (struct zip_stream *)malloc(sizeof(struct zip_stream))) ==
        NULL) {
        return UNIZIP_MEM_ERROR;
    }
    int ret = UNIZIP_OK;
    int algType = strm->compression_flag;
    if (algType == WCRYPTO_ZLIB) {
        ret = hw_deflateInit(strm->ud, level);
    } else if (algType == WCRYPTO_GZIP) {
        int windowBits = 15;
        int gzipEncoding = 16;
        ret = hw_deflateInit2(strm->ud, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                              windowBits | gzipEncoding, DEF_MEM_LEVEL,
                              Z_DEFAULT_STRATEGY);
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
    unsigned long inStreamChunk = (unsigned long)strm->avail_in;
    unsigned long outStreamChunk = (unsigned long)strm->avail_out;
    if (outStreamChunk < DEFLATE_MIN_CHUNK) {
        outStreamChunk = DEFLATE_MIN_CHUNK;
    }
    ctx->avail_in = inStreamChunk;
    ctx->avail_out = outStreamChunk;
    ctx->total_in = (unsigned long)strm->total_in;
    ctx->total_out = (unsigned long)strm->total_out;
    memcpy(ctx->next_in, src, inStreamChunk);
    int err = 0;
    ctx->avail_out = outStreamChunk;
    err = hw_deflate(ctx, flush);
    if (err < 0) {
        return err;
    }
    int have = outStreamChunk - ctx->avail_out;
    memcpy(dst, ctx->next_out, have);
    dst += have;
    src += inStreamChunk - ctx->avail_in;
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
    if ((strm->ud = (struct zip_stream *)malloc(sizeof(struct zip_stream))) ==
        NULL) {
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
    unsigned long inStreamChunk = (unsigned long)strm->avail_in;
    unsigned long outStreamChunk = (unsigned long)strm->avail_out;
    if (outStreamChunk < INFLATE_MIN_CHUNK) {
        outStreamChunk = INFLATE_MIN_CHUNK;
    }
    ctx->avail_in = inStreamChunk;
    ctx->avail_out = outStreamChunk;
    ctx->total_in = (unsigned long)strm->total_in;
    ctx->total_out = (unsigned long)strm->total_out;
    memcpy(ctx->next_in, src, inStreamChunk);
    int err = 0;
    err = hw_inflate(ctx, flush);
    if (err < 0) {
        return err;
    }
    int have = outStreamChunk - ctx->avail_out;
    memcpy(dst, ctx->next_out, have);
    dst += have;
    src += inStreamChunk - ctx->avail_in;
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
    return hw_copy(dest->ud, source->ud, WCRYPTO_ZLIB, WCRYPTO_DEFLATE);
}

int inflateCopyCB_uadk(unizip_streamp dest, unizip_streamp source)
{
    dest->compression_flag = source->compression_flag;
    return hw_copy(dest->ud, source->ud, WCRYPTO_ZLIB, WCRYPTO_INFLATE);
}

int deflateResetCB_uadk(unizip_streamp strm)
{
    hw_deflateEnd(strm->ud);
    if ((strm->ud = (struct zip_stream *)malloc(sizeof(struct zip_stream))) ==
        NULL) {
        return UNIZIP_MEM_ERROR;
    }

    int ret = UNIZIP_OK;
    int algType = strm->compression_flag;
    if (algType == WCRYPTO_ZLIB) {
        ret = hw_deflateInit(strm->ud, Z_DEFAULT_COMPRESSION);
    } else if (algType == WCRYPTO_GZIP) {
        int windowBits = 15;
        int gzipEncoding = 16;
        ret = hw_deflateInit2(strm->ud, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                              windowBits | gzipEncoding, DEF_MEM_LEVEL,
                              Z_DEFAULT_STRATEGY);
    } else {
        ret = UNIZIP_DATA_ERROR;
    }
    resetUadk(strm);
    return ret;
}

int inflateResetCB_uadk(unizip_streamp strm)
{
    hw_inflateEnd(strm->ud);
    if ((strm->ud = (struct zip_stream *)malloc(sizeof(struct zip_stream))) ==
        NULL) {
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

int compressCB_uadk(Bytef *dest, uLongf *destLen, const Bytef *source,
                    uLong sourceLen)
{
    int ret = hw_stream_compress(ALG_TYPE, DEFAULT_STREAM_CHUNK,
                                 (unsigned char *)dest, (ulong *)destLen,
                                 (unsigned char *)source, sourceLen);
    return ret;
}

int uncompressCB_uadk(Bytef *dest, uLongf *destLen, const Bytef *source,
                      uLong sourceLen)
{
    int ret = hw_stream_decompress(ALG_TYPE, DEFAULT_STREAM_CHUNK,
                                   (unsigned char *)dest, (ulong *)destLen,
                                   (unsigned char *)source, sourceLen);
    return ret;
}

uLong compressBound_uadk(uLong sourceLen)
{
    return sourceLen * (1 + POSSIBLE_INCREASE) + (sourceLen >> 12) +
           (sourceLen >> 14) + (sourceLen >> 25) + 13;
}

void uadk_init(callback_t *cb_uadk)
{
    cb_uadk->versionCB = versionCB_uadk;
    cb_uadk->deflateCB = deflateCB_uadk;
    cb_uadk->deflateInitCB = deflateInitCB_uadk;
    cb_uadk->deflateEndCB = deflateEndCB_uadk;
    cb_uadk->inflateInitCB = inflateInitCB_uadk;
    cb_uadk->inflateCB = inflateCB_uadk;
    cb_uadk->inflateEndCB = inflateEndCB_uadk;
    cb_uadk->deflateCopyCB = deflateCopyCB_uadk;
    cb_uadk->inflateCopyCB = inflateCopyCB_uadk;
    cb_uadk->deflateResetCB = deflateResetCB_uadk;
    cb_uadk->inflateResetCB = inflateResetCB_uadk;
    cb_uadk->compressCB = compressCB_uadk;
    cb_uadk->uncompressCB = uncompressCB_uadk;
    cb_uadk->compressBoundCB = compressBound_uadk;
}

#endif // __aarch64__
