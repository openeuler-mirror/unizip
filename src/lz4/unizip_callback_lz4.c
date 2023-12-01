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
#include "unizip_callback_lz4.h"
#include <stdio.h>
#include <stdlib.h>
#include "unizip_algorithm_lz4.h"

const char *versionCB_lz4(void) { return LZ4_versionString(); }

int deflateInitCB_lz4(unizip_streamp strm, int level)
{
    strm->ud = lz4_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    strm->compression_flag = level;
    struct lz4_ctx *ctx = (struct lz4_ctx *)strm->ud;
    strm->next_in = NULL;
    strm->next_out = NULL;
    strm->avail_in = 0;
    strm->avail_out = 0;
    strm->total_in = 0;
    strm->total_out = 0;
    return lz4_deflateInit(ctx, level);
}

int deflateCB_lz4(unizip_streamp strm, int flush)
{
    struct lz4_ctx *ctx = (struct lz4_ctx *)strm->ud;
    ctx->total_in = strm->total_in;
    ctx->next_in = (char *)strm->next_in;
    ctx->total_out = strm->total_out;
    ctx->next_out = (char *)strm->next_out;
    ctx->avail_in = strm->avail_in;
    ctx->avail_out = strm->avail_out;

    int status = lz4_deflate(ctx, flush);

    strm->next_in = (Bytef *)ctx->next_in;
    strm->total_in = (uLong)ctx->total_in;
    strm->next_out = (Bytef *)ctx->next_out;
    strm->total_out = (uLong)ctx->total_out;
    strm->avail_in = ctx->avail_in;
    strm->avail_out = ctx->avail_out;
    return status;
}

int deflateEndCB_lz4(unizip_streamp strm) { return lz4_deflateEnd(strm->ud); }

int inflateInitCB_lz4(unizip_streamp strm)
{
    strm->ud = lz4_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    strm->next_in = NULL;
    strm->next_out = NULL;
    strm->avail_in = 0;
    strm->avail_out = 0;
    strm->total_in = 0;
    strm->total_out = 0;
    return lz4_inflateInit(strm->ud);
}

int inflateCB_lz4(unizip_streamp strm, int flush)
{
    struct lz4_ctx *ctx = (struct lz4_ctx *)strm->ud;
    ctx->avail_in = strm->avail_in;
    ctx->avail_out = strm->avail_out;
    ctx->next_in = (char *)strm->next_in;
    ctx->next_out = (char *)strm->next_out;
    ctx->total_in = strm->total_in;

    int status = lz4_inflate(strm->ud, flush);

    strm->next_in = (Bytef *)ctx->next_in;
    strm->next_out = (Bytef *)ctx->next_out;
    strm->avail_in = ctx->avail_in;
    strm->avail_out = ctx->avail_out;
    strm->total_in = ctx->total_in;
    strm->total_out = ctx->total_out;

    return status;
}

int inflateEndCB_lz4(unizip_streamp strm) { return lz4_inflateEnd(strm->ud); }

int deflateCopyCB_lz4(unizip_streamp dest, unizip_streamp source)
{
    return lz4_copy(dest->ud, source->ud);
}

int inflateCopyCB_lz4(unizip_streamp dest, unizip_streamp source)
{
    return lz4_copy(dest->ud, source->ud);
}

int deflateResetCB_lz4(unizip_streamp strm)
{
    strm->next_in = NULL;
    strm->next_out = NULL;
    strm->avail_in = 0;
    strm->total_in = 0;
    strm->total_out = 0;
    strm->avail_out = 0;

    lz4_end(strm->ud);
    strm->ud = lz4_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    if (strm->compression_flag > levelMax_lz4 ||
        strm->compression_flag < levelMin_lz4) {
        strm->compression_flag = 1;
    }

    return lz4_deflateInit(strm->ud, strm->compression_flag);
}

int inflateResetCB_lz4(unizip_streamp strm)
{
    strm->next_in = NULL;
    strm->next_out = NULL;
    strm->avail_in = 0;
    strm->total_in = 0;
    strm->total_out = 0;
    strm->avail_out = 0;
    lz4_end(strm->ud);
    strm->ud = lz4_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    return lz4_inflateInit(strm->ud);
}

int compressCB_lz4(Bytef *dest, uLongf *destLen, const Bytef *source,
                   uLong sourceLen)
{
    return lz4_compress((char *)dest, destLen, (char *)source, sourceLen);
}

int uncompressCB_lz4(Bytef *dest, uLongf *destLen, const Bytef *source,
                     uLong sourceLen)
{
    char *dest_c = (char *)dest;
    char *source_c = (char *)source;
    unsigned long destLen_c = (unsigned long)*destLen;
    return lz4_decompress(dest_c, &destLen_c, source_c, sourceLen);
}

uLong compressBound_lz4(uLong sourceLen)
{
    /*
    The upper bound size shoud be 0x7E000000 bytes in lz4
    */
    return lz4_compressBound(sourceLen);
}

void lz4_init(callback_t *cb_lz4)
{
    cb_lz4->versionCB = versionCB_lz4;
    cb_lz4->deflateCB = deflateCB_lz4;
    cb_lz4->deflateInitCB = deflateInitCB_lz4;
    cb_lz4->deflateEndCB = deflateEndCB_lz4;
    cb_lz4->inflateInitCB = inflateInitCB_lz4;
    cb_lz4->inflateCB = inflateCB_lz4;
    cb_lz4->inflateEndCB = inflateEndCB_lz4;
    cb_lz4->deflateCopyCB = deflateCopyCB_lz4;
    cb_lz4->inflateCopyCB = inflateCopyCB_lz4;
    cb_lz4->deflateResetCB = deflateResetCB_lz4;
    cb_lz4->inflateResetCB = inflateResetCB_lz4;
    cb_lz4->compressCB = compressCB_lz4;
    cb_lz4->uncompressCB = uncompressCB_lz4;
    cb_lz4->compressBoundCB = compressBound_lz4;
}
