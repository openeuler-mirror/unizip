/******************************************************************************
 * unizip_callback_uadk.h
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * jiang-chenyang <jiangcy1129@163.com>
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
#ifndef SRC_UADK_UNIZIP_CALLBACK_UADK_H_
#define SRC_UADK_UNIZIP_CALLBACK_UADK_H_
#ifdef __aarch64__

#include "unizip_struct.h"

const char *versionCB_uadk(void);

int deflateInitCB_uadk(unizip_streamp strm, int level);

int deflateCB_uadk(unizip_streamp strm, int flush);

int deflateEndCB_uadk(unizip_streamp strm);

int inflateInitCB_uadk(unizip_streamp strm);

int inflateCB_uadk(unizip_streamp strm, int flush);

int inflateEndCB_uadk(unizip_streamp strm);

int deflateCopyCB_uadk(unizip_streamp dest, unizip_streamp source);

int inflateCopyCB_uadk(unizip_streamp dest, unizip_streamp source);

int deflateResetCB_uadk(unizip_streamp strm);

int inflateResetCB_uadk(unizip_streamp strm);

int compressCB_uadk(Bytef *dest, uLongf *destLen, const Bytef *source,
                    uLong sourceLen);

int uncompressCB_uadk(Bytef *dest, uLongf *destLen, const Bytef *source,
                      uLong sourceLen);

uLong compressBound_uadk(uLong sourceLen);

void uadk_init(callback_t *cb_uadk);

#endif // __aarch64__

#endif // SRC_UADK_UNIZIP_CALLBACK_UADK_H_
