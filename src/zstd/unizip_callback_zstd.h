/******************************************************************************
 * unizip_callback_zstd.h
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
#ifndef SRC_ZSTD_UNIZIP_CALLBACK_ZSTD_H_
#define SRC_ZSTD_UNIZIP_CALLBACK_ZSTD_H_
#include "unizip_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *versionCB_zstd(void);

int deflateInitCB_zstd(unizip_streamp strm, int level);

int deflateCB_zstd(unizip_streamp strm, int flush);

int deflateEndCB_zstd(unizip_streamp strm);

int inflateInitCB_zstd(unizip_streamp strm);

int inflateCB_zstd(unizip_streamp strm, int flush);

int inflateEndCB_zstd(unizip_streamp strm);

int deflateCopyCB_zstd(unizip_streamp dest, unizip_streamp source);

int inflateCopyCB_zstd(unizip_streamp dest, unizip_streamp source);

int deflateResetCB_zstd(unizip_streamp strm);

int inflateResetCB_zstd(unizip_streamp strm);

int compressCB_zstd(Bytef *dest, uLongf *destLen, Bytef *source, uLong sourceLen);

int uncompressCB_zstd(Bytef *dest, uLongf *destLen, Bytef *source, uLong sourceLen);

uLong compressBoundCB_zstd(uLong sourceLen);


#ifdef __cplusplus
}
#endif

#endif // SRC_ZSTD_UNIZIP_CALLBACK_ZSTD_H_
