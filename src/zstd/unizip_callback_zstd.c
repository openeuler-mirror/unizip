/******************************************************************************
 * unizip_callback_zstd.c
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
#include "unizip_callback_zstd.h"
#include "unizip_algorithm_zstd.h"

const char *versionCB_zstd(void) { return ZSTD_versionString(); }

void resetZstd(unizip_streamp strm)
{
    strm->next_in = NULL;
    strm->next_out = NULL;
    strm->avail_in = 0;
    strm->avail_out = 0;
    strm->total_in = 0;
    strm->total_out = 0;
}

int deflateInitCB_zstd(unizip_streamp strm, int level)
{
    // 在下层的返回值可以不用void*
    strm->ud = zstd_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    strm->compression_flag = level;
    int ret = zstd_deflateInit(strm->ud, level);
    resetZstd(strm);
    return ret;
}

int deflateCB_zstd(unizip_streamp strm, int flush)
{
    struct zstd_ctx *ctx = (struct zstd_ctx *)strm->ud;
    ctx->next_in = (char *)strm->next_in;
    ctx->next_out = (char *)strm->next_out;
    ctx->avail_in = strm->avail_in;
    ctx->avail_out = strm->avail_out;
    ctx->total_in = strm->total_in;
    ctx->total_out = strm->total_out;
    int ret = zstd_deflate(strm->ud, flush);
    strm->next_in = (Bytef *)ctx->next_in;
    strm->next_out = (Bytef *)ctx->next_out;
    strm->avail_in = ctx->avail_in;
    strm->avail_out = ctx->avail_out;
    strm->total_in = ctx->total_in;
    strm->total_out = ctx->total_out;
    return ret;
}

int deflateEndCB_zstd(unizip_streamp strm)
{
    resetZstd(strm);
    return zstd_deflateEnd(strm->ud);
}

int inflateInitCB_zstd(unizip_streamp strm)
{
    strm->ud = zstd_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    resetZstd(strm);
    return zstd_inflateInit(strm->ud);
}

int inflateCB_zstd(unizip_streamp strm, int flush)
{
    struct zstd_ctx *ctx = (struct zstd_ctx *)strm->ud;
    ctx->next_in = (char *)strm->next_in;
    ctx->next_out = (char *)strm->next_out;
    ctx->avail_in = strm->avail_in;
    ctx->avail_out = strm->avail_out;
    ctx->total_in = strm->total_in;
    ctx->total_out = strm->total_out;
    int ret = zstd_inflate(strm->ud);
    strm->next_in = (Bytef *)ctx->next_in;
    strm->next_out = (Bytef *)ctx->next_out;
    strm->avail_in = ctx->avail_in;
    strm->avail_out = ctx->avail_out;
    strm->total_in = ctx->total_in;
    strm->total_out = ctx->total_out;
    return ret;
}
int inflateEndCB_zstd(unizip_streamp strm)
{
    resetZstd(strm);
    return zstd_inflateEnd(strm->ud);
}

int deflateCopyCB_zstd(unizip_streamp dest, unizip_streamp source)
{
    return zstd_deflatecopy(dest->ud, source->ud);
}

int inflateCopyCB_zstd(unizip_streamp dest, unizip_streamp source)
{
    return zstd_inflatecopy(dest->ud, source->ud);
}

int deflateResetCB_zstd(unizip_streamp strm)
{
    resetZstd(strm);
    zstd_deflateEnd(strm->ud);
    strm->ud = zstd_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    return zstd_deflateInit(strm->ud, strm->compression_flag);
}

int inflateResetCB_zstd(unizip_streamp strm)
{
    resetZstd(strm);
    zstd_inflateEnd(strm->ud);
    strm->ud = zstd_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    return zstd_inflateInit(strm->ud);
}

int compressCB_zstd(Bytef *dest, uLongf *destLen, Bytef *source,
                    uLong sourceLen)
{
    int ret = zstd_compress((char *)dest, destLen, (char *)source, sourceLen);
    return ret;
}

int uncompressCB_zstd(Bytef *dest, uLongf *destLen, Bytef *source,
                      uLong sourceLen)
{
    int ret = zstd_decompress((char *)dest, destLen, (char *)source, sourceLen);
    return ret;
}

uLong compressBound_zstd(uLong sourceLen)
{
    return zstd_compressBound(sourceLen);
}

void zstd_init(callback_t *cb_zstd)
{
    cb_zstd->versionCB = versionCB_zstd;
    cb_zstd->deflateCB = deflateCB_zstd;
    cb_zstd->deflateInitCB = deflateInitCB_zstd;
    cb_zstd->deflateEndCB = deflateEndCB_zstd;
    cb_zstd->inflateInitCB = inflateInitCB_zstd;
    cb_zstd->inflateCB = inflateCB_zstd;
    cb_zstd->inflateEndCB = inflateEndCB_zstd;
    cb_zstd->deflateCopyCB = deflateCopyCB_zstd;
    cb_zstd->inflateCopyCB = inflateCopyCB_zstd;
    cb_zstd->deflateResetCB = deflateResetCB_zstd;
    cb_zstd->inflateResetCB = inflateResetCB_zstd;
    cb_zstd->compressCB = compressCB_zstd;
    cb_zstd->uncompressCB = uncompressCB_zstd;
    cb_zstd->compressBoundCB = compressBound_zstd;
}
