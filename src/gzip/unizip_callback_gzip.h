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
#ifndef SRC_GZIP_UNIZIP_CALLBACK_GZIP_H_
#define SRC_GZIP_UNIZIP_CALLBACK_GZIP_H_
#include "unizip_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *versionCB_gzip(void);

int deflateInitCB_gzip(unizip_streamp strm, int level);

int deflateCB_gzip(unizip_streamp strm, int flush);

int deflateEndCB_gzip(unizip_streamp strm);

int inflateInitCB_gzip(unizip_streamp strm);

int inflateCB_gzip(unizip_streamp strm, int flush);

int inflateEndCB_gzip(unizip_streamp strm);

int deflateCopyCB_gzip(unizip_streamp dest, unizip_streamp source);

int inflateCopyCB_gzip(unizip_streamp dest, unizip_streamp source);

int deflateResetCB_gzip(unizip_streamp strm);

int inflateResetCB_gzip(unizip_streamp strm);

int compressCB_gzip(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);

int uncompressCB_gzip(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);

uLong compressBoundCB_gzip(uLong sourceLen);

int deflateSetHeaderCB_gzip(unizip_streamp strm, gz_headerp head);

int inflateGetHeaderCB_gzip(unizip_streamp strm, gz_headerp head);

int deflateInit2CB_gzip(unizip_streamp strm, int level, int method, int windowBits, int memLevel, int strategy);

int inflateInit2CB_gzip(unizip_streamp strm, int windowBits);

int compress2CB_gzip(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, int level);

int uncompress2CB_gzip(Bytef *dest, uLongf *destLen, const Bytef *source, uLong *sourceLen);

#ifdef __cplusplus
}
#endif

#endif // SRC_GZIP_UNIZIP_CALLBACK_GZIP_H_
