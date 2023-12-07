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

int deflateInitCB_gzip(unizip_streamp strm, int level) { return deflateInit((z_streamp)strm, level); }

int deflateCB_gzip(unizip_streamp strm, int flush) { return deflate((z_streamp)strm, flush); }

int deflateEndCB_gzip(unizip_streamp strm) { return deflateEnd((z_streamp)strm); }

int inflateInitCB_gzip(unizip_streamp strm) { return inflateInit((z_streamp)strm); }

int inflateCB_gzip(unizip_streamp strm, int flush) { return inflate((z_streamp)strm, flush); }
int inflateEndCB_gzip(unizip_streamp strm) { return inflateEnd((z_streamp)strm); }

int deflateCopyCB_gzip(unizip_streamp dest, unizip_streamp source)
{
    return deflateCopy((z_streamp)dest, (z_streamp)source);
}

int inflateCopyCB_gzip(unizip_streamp dest, unizip_streamp source)
{
    return inflateCopy((z_streamp)dest, (z_streamp)source);
}

int deflateResetCB_gzip(unizip_streamp strm) { return deflateReset((z_streamp)strm); }

int inflateResetCB_gzip(unizip_streamp strm) { return inflateReset((z_streamp)strm); }

int compressCB_gzip(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    return compress(dest, destLen, source, sourceLen);
}

int uncompressCB_gzip(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    return uncompress(dest, destLen, source, sourceLen);
}

uLong compressBoundCB_gzip(uLong sourceLen) { return compressBound(sourceLen); }

int deflateSetHeaderCB_gzip(unizip_streamp strm, gz_headerp head) { return deflateSetHeader((z_stream *)strm, head); }

int inflateGetHeaderCB_gzip(unizip_streamp strm, gz_headerp head) { return inflateGetHeader((z_stream *)strm, head); }

int deflateInit2CB_gzip(unizip_streamp strm, int level, int method, int windowBits, int memLevel, int strategy)
{
    return deflateInit2((z_stream *)strm, level, method, windowBits, memLevel, strategy);
}

int inflateInit2CB_gzip(unizip_streamp strm, int windowBits) { return inflateInit2((z_stream *)strm, windowBits); }

int compress2CB_gzip(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, int level)
{
    return compress2(dest, destLen, source, sourceLen, level);
}

int uncompress2CB_gzip(Bytef *dest, uLongf *destLen, const Bytef *source, uLong *sourceLen)
{
    return uncompress2(dest, destLen, source, sourceLen);
}
