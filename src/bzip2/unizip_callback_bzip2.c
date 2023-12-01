/******************************************************************************
 * unizip_callback_bzip2.c
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
#include "unizip_callback_bzip2.h"
#include "unizip_algorithm_bzip2.h"

const char *versionCB_bzip2(void) { return BZ2_bzlibVersion(); }

void resetBzip2(unizip_streamp strm)
{
    strm->next_in = NULL;
    strm->next_out = NULL;
    strm->avail_in = 0;
    strm->avail_out = 0;
    strm->total_in = 0;
    strm->total_out = 0;
}

int deflateInitCB_bzip2(unizip_streamp strm, int level)
{
    // 在下层的返回值可以不用void*
    strm->ud = bzip2_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    strm->compression_flag = level;
    int ret = bzip2_deflateInit(strm->ud, level);
    resetBzip2(strm);
    return ret;
}

int deflateCB_bzip2(unizip_streamp strm, int flush)
{
    // 在这对上层属性进行更新，保证底层函数参数精简，不感知上层属性内容
    struct bzip2_ctx *ctx = (struct bzip2_ctx *)strm->ud;
    ctx->next_in = (char *)strm->next_in;
    ctx->next_out = (char *)strm->next_out;
    ctx->avail_in = strm->avail_in;
    ctx->avail_out = strm->avail_out;
    ctx->total_in = strm->total_in;
    ctx->total_out = strm->total_out;
    int ret = bzip2_deflate(strm->ud, flush);
    strm->next_in = (Bytef *)ctx->next_in;
    strm->next_out = (Bytef *)ctx->next_out;
    strm->avail_in = ctx->avail_in;
    strm->avail_out = ctx->avail_out;
    strm->total_in = ctx->total_in;
    strm->total_out = ctx->total_out;
    return ret;
}

int deflateEndCB_bzip2(unizip_streamp strm)
{
    resetBzip2(strm);
    return bzip2_deflateEnd(strm->ud);
}

int inflateInitCB_bzip2(unizip_streamp strm)
{
    strm->ud = bzip2_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    resetBzip2(strm);
    return bzip2_inflateInit(strm->ud);
}

int inflateCB_bzip2(unizip_streamp strm, int flush)
{
    struct bzip2_ctx *ctx = (struct bzip2_ctx *)strm->ud;
    ctx->next_in = (char *)strm->next_in;
    ctx->next_out = (char *)strm->next_out;
    ctx->avail_in = strm->avail_in;
    ctx->avail_out = strm->avail_out;
    ctx->total_in = strm->total_in;
    ctx->total_out = strm->total_out;
    int ret = bzip2_inflate(strm->ud);
    strm->next_in = (Bytef *)ctx->next_in;
    strm->next_out = (Bytef *)ctx->next_out;
    strm->avail_in = ctx->avail_in;
    strm->avail_out = ctx->avail_out;
    strm->total_in = ctx->total_in;
    strm->total_out = ctx->total_out;
    return ret;
}
int inflateEndCB_bzip2(unizip_streamp strm)
{
    resetBzip2(strm);
    return bzip2_inflateEnd(strm->ud);
}

int deflateCopyCB_bzip2(unizip_streamp dest, unizip_streamp source)
{
    dest->compression_flag = source->compression_flag;
    return bzip2_copy(dest->ud, source->ud);
}

int inflateCopyCB_bzip2(unizip_streamp dest, unizip_streamp source)
{
    return bzip2_copy(dest->ud, source->ud);
}

int deflateResetCB_bzip2(unizip_streamp strm)
{
    resetBzip2(strm);
    bzip2_deflateEnd(strm->ud);
    strm->ud = bzip2_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    return bzip2_deflateInit(strm->ud, strm->compression_flag);
}

int inflateResetCB_bzip2(unizip_streamp strm)
{
    resetBzip2(strm);
    bzip2_inflateEnd(strm->ud);
    strm->ud = bzip2_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    return bzip2_inflateInit(strm->ud);
}

int compressCB_bzip2(Bytef *dest, uLongf *destLen, const Bytef *source,
                     uLong sourceLen)
{
    int ret = bzip2_compress((char *)dest, destLen, (char *)source, sourceLen);
    return ret;
}

int uncompressCB_bzip2(Bytef *dest, uLongf *destLen, const Bytef *source,
                       uLong sourceLen)
{
    int ret =
        bzip2_decompress((char *)dest, destLen, (char *)source, sourceLen);
    return ret;
}

uLong compressBound_bzip2(uLong sourceLen)
{
    return bzip2_compressBound(sourceLen);
}

void bzip2_init(callback_t *cb_bzip2)
{
    cb_bzip2->versionCB = versionCB_bzip2;
    cb_bzip2->deflateCB = deflateCB_bzip2;
    cb_bzip2->deflateInitCB = deflateInitCB_bzip2;
    cb_bzip2->deflateEndCB = deflateEndCB_bzip2;
    cb_bzip2->inflateInitCB = inflateInitCB_bzip2;
    cb_bzip2->inflateCB = inflateCB_bzip2;
    cb_bzip2->inflateEndCB = inflateEndCB_bzip2;
    cb_bzip2->deflateCopyCB = deflateCopyCB_bzip2;
    cb_bzip2->inflateCopyCB = inflateCopyCB_bzip2;
    cb_bzip2->deflateResetCB = deflateResetCB_bzip2;
    cb_bzip2->inflateResetCB = inflateResetCB_bzip2;
    cb_bzip2->compressCB = compressCB_bzip2;
    cb_bzip2->uncompressCB = uncompressCB_bzip2;
    cb_bzip2->compressBoundCB = compressBound_bzip2;
}
