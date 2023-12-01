/******************************************************************************
 * unizip_callback_xz.c
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
#include "unizip_callback_xz.h"
#include <stdio.h>
#include <stdlib.h>
#include "unizip_algorithm_xz.h"

const char *versionCB_xz(void) { return lzma_version_string(); }

void resetXz(unizip_streamp strm)
{
    strm->next_in = NULL;
    strm->next_out = NULL;
    strm->avail_in = 0;
    strm->avail_out = 0;
    strm->total_in = 0;
    strm->total_out = 0;
}

int deflateInitCB_xz(unizip_streamp strm, int level)
{
    strm->ud = xz_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    strm->compression_flag = level;
    int ret = xz_deflateInit(strm->ud, level);
    resetXz(strm);
    return ret;
}

int deflateCB_xz(unizip_streamp strm, int flush)
{
    struct xz_ctx *ctx = (struct xz_ctx *)strm->ud;
    ctx->next_in = (char *)strm->next_in;
    ctx->next_out = (char *)strm->next_out;
    ctx->avail_in = strm->avail_in;
    ctx->avail_out = strm->avail_out;
    ctx->total_in = strm->total_in;
    ctx->total_out = strm->total_out;
    int ret = xz_deflate(strm->ud, flush);
    strm->next_in = (Bytef *)ctx->next_in;
    strm->next_out = (Bytef *)ctx->next_out;
    strm->avail_in = ctx->avail_in;
    strm->avail_out = ctx->avail_out;
    strm->total_in = (uLong)ctx->total_in;
    strm->total_out = (uLong)ctx->total_out;
    return ret;
}

int deflateEndCB_xz(unizip_streamp strm)
{
    resetXz(strm);
    return xz_End(strm->ud);
}

int inflateInitCB_xz(unizip_streamp strm)
{
    strm->ud = xz_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    resetXz(strm);
    return xz_inflateInit(strm->ud);
}

int inflateCB_xz(unizip_streamp strm, int flush)
{
    struct xz_ctx *ctx = (struct xz_ctx *)strm->ud;
    ctx->next_in = (char *)strm->next_in;
    ctx->next_out = (char *)strm->next_out;
    ctx->avail_in = strm->avail_in;
    ctx->avail_out = strm->avail_out;
    ctx->total_in = strm->total_in;
    ctx->total_out = strm->total_out;
    int ret = xz_inflate(strm->ud, flush);
    strm->next_in = (Bytef *)ctx->next_in;
    strm->next_out = (Bytef *)ctx->next_out;
    strm->avail_in = ctx->avail_in;
    strm->avail_out = ctx->avail_out;
    strm->total_in = (uLong)ctx->total_in;
    strm->total_out = (uLong)ctx->total_out;
    return ret;
}
int inflateEndCB_xz(unizip_streamp strm)
{
    resetXz(strm);
    return xz_End(strm->ud);
}

int deflateCopyCB_xz(unizip_streamp dest, unizip_streamp source)
{
    dest->compression_flag = source->compression_flag;
    return xz_copy(dest->ud, source->ud);
}

int inflateCopyCB_xz(unizip_streamp dest, unizip_streamp source)
{
    return xz_copy(dest->ud, source->ud);
}

int deflateResetCB_xz(unizip_streamp strm)
{
    resetXz(strm);
    xz_End(strm->ud);
    strm->ud = xz_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    return xz_deflateInit(strm->ud, strm->compression_flag);
}

int inflateResetCB_xz(unizip_streamp strm)
{
    resetXz(strm);
    xz_End(strm->ud);
    strm->ud = xz_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    return xz_inflateInit(strm->ud);
}

int compressCB_xz(Bytef *dest, uLongf *destLen, const Bytef *source,
                  uLong sourceLen)
{
    return xz_compress((uint8_t *)source, sourceLen, (uint8_t *)dest, destLen);
}

int uncompressCB_xz(Bytef *dest, uLongf *destLen, const Bytef *source,
                    uLong sourceLen)
{
    return xz_decompress((uint8_t *)source, sourceLen, (uint8_t *)dest,
                         destLen);
}

uLong compressBound_xz(uLong sourceLen) { return xz_compressBound(sourceLen); }

void xz_init(callback_t *cb_xz)
{
    cb_xz->versionCB = versionCB_xz;
    cb_xz->deflateCB = deflateCB_xz;
    cb_xz->deflateInitCB = deflateInitCB_xz;
    cb_xz->deflateEndCB = deflateEndCB_xz;
    cb_xz->inflateInitCB = inflateInitCB_xz;
    cb_xz->inflateCB = inflateCB_xz;
    cb_xz->inflateEndCB = inflateEndCB_xz;
    cb_xz->deflateCopyCB = deflateCopyCB_xz;
    cb_xz->inflateCopyCB = inflateCopyCB_xz;
    cb_xz->deflateResetCB = deflateResetCB_xz;
    cb_xz->inflateResetCB = inflateResetCB_xz;
    cb_xz->compressCB = compressCB_xz;
    cb_xz->uncompressCB = uncompressCB_xz;
    cb_xz->compressBoundCB = compressBound_xz;
}
