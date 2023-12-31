/******************************************************************************
 * unizip_callback_zlib.c
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * chen-yufan <1109674186@qq.com>
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
#include "unizip_callback_zlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

const char *versionCB_zlib(void) { return zlibVersion(); }

int deflateInitCB_zlib(unizip_streamp strm, int level) { return deflateInit((z_streamp)strm, level); }

int deflateCB_zlib(unizip_streamp strm, int flush) { return deflate((z_streamp)strm, flush); }

int deflateEndCB_zlib(unizip_streamp strm) { return deflateEnd((z_streamp)strm); }

int inflateInitCB_zlib(unizip_streamp strm) { return inflateInit((z_streamp)strm); }

int inflateCB_zlib(unizip_streamp strm, int flush) { return inflate((z_streamp)strm, flush); }
int inflateEndCB_zlib(unizip_streamp strm) { return inflateEnd((z_streamp)strm); }

int deflateCopyCB_zlib(unizip_streamp dest, unizip_streamp source)
{
    return deflateCopy((z_streamp)dest, (z_streamp)source);
}

int inflateCopyCB_zlib(unizip_streamp dest, unizip_streamp source)
{
    return inflateCopy((z_streamp)dest, (z_streamp)source);
}

int deflateResetCB_zlib(unizip_streamp strm) { return deflateReset((z_streamp)strm); }

int inflateResetCB_zlib(unizip_streamp strm) { return inflateReset((z_streamp)strm); }

int compressCB_zlib(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    return compress(dest, destLen, source, sourceLen);
}

int uncompressCB_zlib(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    return uncompress(dest, destLen, source, sourceLen);
}

uLong compressBoundCB_zlib(uLong sourceLen) { return compressBound(sourceLen); }

int deflateInit2CB_zlib(unizip_streamp strm, int level, int method, int windowBits, int memLevel, int strategy)
{
    return deflateInit2((z_stream *)strm, level, method, windowBits, memLevel, strategy);
}

int inflateInit2CB_zlib(unizip_streamp strm, int windowBits) { return inflateInit2((z_stream *)strm, windowBits); }

int compress2CB_zlib(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, int level)
{
    return compress2(dest, destLen, source, sourceLen, level);
}

int uncompress2CB_zlib(Bytef *dest, uLongf *destLen, const Bytef *source, uLong *sourceLen)
{
    return uncompress2(dest, destLen, source, sourceLen);
}

int deflateParamsCB_zlib(unizip_streamp strm, int level, int strategy)
{
    return deflateParams((z_stream *)strm, level, strategy);
}

int deflateSetDictionaryCB_zlib(unizip_streamp strm, const Bytef *dictionary, uInt dictLength)
{
    return deflateSetDictionary((z_stream *)strm, dictionary, dictLength);
}

int deflatePendingCB_zlib(unizip_streamp strm, unsigned *pending, int *bits)
{
    return deflatePending((z_stream *)strm, pending, bits);
}

int deflateSetHeaderCB_zlib(unizip_streamp strm, gz_headerp head) { return deflateSetHeader((z_stream *)strm, head); }

int inflateSetDictionaryCB_zlib(unizip_streamp strm, const Bytef *dictionary, uInt dictLength)
{
    return inflateSetDictionary((z_stream *)strm, dictionary, dictLength);
}

int inflateGetHeaderCB_zlib(unizip_streamp strm, gz_headerp head) { return inflateGetHeader((z_stream *)strm, head); }
