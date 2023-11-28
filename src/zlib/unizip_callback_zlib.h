/******************************************************************************
 * unizip_callback_zlib.h
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
#ifndef SRC_ZLIB_UNIZIP_CALLBACK_ZLIB_H
#define SRC_ZLIB_UNIZIP_CALLBACK_ZLIB_H
#include "unizip_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *versionCB_zlib(void);

int deflateInitCB_zlib(unizip_streamp strm, int level);

int deflateCB_zlib(unizip_streamp strm, int flush);

int deflateEndCB_zlib(unizip_streamp strm);

int inflateInitCB_zlib(unizip_streamp strm);

int inflateCB_zlib(unizip_streamp strm, int flush);

int inflateEndCB_zlib(unizip_streamp strm);

int deflateCopyCB_zlib(unizip_streamp dest, unizip_streamp source);

int inflateCopyCB_zlib(unizip_streamp dest, unizip_streamp source);

int deflateResetCB_zlib(unizip_streamp strm);

int inflateResetCB_zlib(unizip_streamp strm);

int compressCB_zlib(Bytef *dest, uLongf *destLen, const Bytef *source,
                    uLong sourceLen);

int uncompressCB_zlib(Bytef *dest, uLongf *destLen, const Bytef *source,
                      uLong sourceLen);

uLong compressBound_zlib(uLong sourceLen);

void zlib_init(callback_t *cb_zlib);

#ifdef __cplusplus
}
#endif

#endif