/*
 * Copyright 2019 Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "unizip_callback_gzip.h"
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

const char *versionCB_gzip(void) { return zlibVersion(); }

int deflateInitCB_gzip(unizip_streamp strm, int level)
{
    return deflateInit((z_streamp)strm, level);
}

int deflateCB_gzip(unizip_streamp strm, int flush)
{
    return deflate((z_streamp)strm, flush);
}

int deflateEndCB_gzip(unizip_streamp strm)
{
    return deflateEnd((z_streamp)strm);
}

int inflateInitCB_gzip(unizip_streamp strm)
{
    return inflateInit((z_streamp)strm);
}

int inflateCB_gzip(unizip_streamp strm, int flush)
{
    return inflate((z_streamp)strm, flush);
}
int inflateEndCB_gzip(unizip_streamp strm)
{
    return inflateEnd((z_streamp)strm);
}

int deflateCopyCB_gzip(unizip_streamp dest, unizip_streamp source)
{
    return deflateCopy((z_streamp)dest, (z_streamp)source);
}

int inflateCopyCB_gzip(unizip_streamp dest, unizip_streamp source)
{
    return inflateCopy((z_streamp)dest, (z_streamp)source);
}

int deflateResetCB_gzip(unizip_streamp strm)
{
    return deflateReset((z_streamp)strm);
}

int inflateResetCB_gzip(unizip_streamp strm)
{
    return inflateReset((z_streamp)strm);
}

int compressCB_gzip(Bytef *dest, uLongf *destLen, const Bytef *source,
                    uLong sourceLen)
{
    return compress(dest, destLen, source, sourceLen);
}

int uncompressCB_gzip(Bytef *dest, uLongf *destLen, const Bytef *source,
                      uLong sourceLen)
{
    return uncompress(dest, destLen, source, sourceLen);
}

uLong compressBound_gzip(uLong sourceLen) { return compressBound(sourceLen); }

void gzip_init(callback_t *cb_gzip)
{
    cb_gzip->versionCB = versionCB_gzip;
    cb_gzip->deflateCB = deflateCB_gzip;
    cb_gzip->deflateInitCB = deflateInitCB_gzip;
    cb_gzip->deflateEndCB = deflateEndCB_gzip;
    cb_gzip->inflateInitCB = inflateInitCB_gzip;
    cb_gzip->inflateCB = inflateCB_gzip;
    cb_gzip->inflateEndCB = inflateEndCB_gzip;
    cb_gzip->deflateCopyCB = deflateCopyCB_gzip;
    cb_gzip->inflateCopyCB = inflateCopyCB_gzip;
    cb_gzip->deflateResetCB = deflateResetCB_gzip;
    cb_gzip->inflateResetCB = inflateResetCB_gzip;
    cb_gzip->compressCB = compressCB_gzip;
    cb_gzip->uncompressCB = uncompressCB_gzip;
    cb_gzip->compressBoundCB = compressBound_gzip;
}
