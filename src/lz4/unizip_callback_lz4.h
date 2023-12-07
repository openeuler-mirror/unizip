/******************************************************************************
 * unizip_algorithm_lz4.c
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * ge-ziwen <hugin1_1@sina.acom>
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
#ifndef SRC_LZ4_UNIZIP_CALLBACK_LZ4_H_
#define SRC_LZ4_UNIZIP_CALLBACK_LZ4_H_
#include "unizip_struct.h"

const char *versionCB_lz4(void);

int deflateInitCB_lz4(unizip_streamp strm, int level);

int deflateCB_lz4(unizip_streamp strm, int flush);

int deflateEndCB_lz4(unizip_streamp strm);

int inflateInitCB_lz4(unizip_streamp strm);

int inflateCB_lz4(unizip_streamp strm, int flush);

int inflateEndCB_lz4(unizip_streamp strm);

int deflateCopyCB_lz4(unizip_streamp dest, unizip_streamp source);

int inflateCopyCB_lz4(unizip_streamp dest, unizip_streamp source);

int deflateResetCB_lz4(unizip_streamp strm);

int inflateResetCB_lz4(unizip_streamp strm);

int compressCB_lz4(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);

int uncompressCB_lz4(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);

uLong compressBoundCB_lz4(uLong sourceLen);

#endif // SRC_LZ4_UNIZIP_CALLBACK_LZ4_H_
