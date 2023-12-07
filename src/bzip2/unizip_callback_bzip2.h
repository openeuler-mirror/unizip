/******************************************************************************
 * unizip_callback_bzip2.h
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
#ifndef SRC_BZIP2_UNIZIP_CALLBACK_BZIP2_H_
#define SRC_BZIP2_UNIZIP_CALLBACK_BZIP2_H_
#include "unizip_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *versionCB_bzip2(void);

int deflateInitCB_bzip2(unizip_streamp strm, int level);

int deflateCB_bzip2(unizip_streamp strm, int flush);

int deflateEndCB_bzip2(unizip_streamp strm);

int inflateInitCB_bzip2(unizip_streamp strm);

int inflateCB_bzip2(unizip_streamp strm, int flush);

int inflateEndCB_bzip2(unizip_streamp strm);

int deflateCopyCB_bzip2(unizip_streamp dest, unizip_streamp source);

int inflateCopyCB_bzip2(unizip_streamp dest, unizip_streamp source);

int deflateResetCB_bzip2(unizip_streamp strm);

int inflateResetCB_bzip2(unizip_streamp strm);

int compressCB_bzip2(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);

int uncompressCB_bzip2(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);

uLong compressBoundCB_bzip2(uLong sourceLen);

#ifdef __cplusplus
}
#endif

#endif // SRC_BZIP2_UNIZIP_CALLBACK_BZIP2_H_
