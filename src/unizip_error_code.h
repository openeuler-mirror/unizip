/******************************************************************************
 * unizip_adapt.h
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
#ifndef SRC_UNIZIP_ERROR_CODE_H_
#define SRC_UNIZIP_ERROR_CODE_H_

#define UNIZIP_OK 0
#define UNIZIP_STREAM_END 1
#define UNIZIP_NEED_DICT 2
#define UNIZIP_ERRNO (-1)
#define UNIZIP_STREAM_ERROR (-2)
#define UNIZIP_DATA_ERROR (-3)
#define UNIZIP_MEM_ERROR (-4)
#define UNIZIP_BUF_ERROR (-5)
#define UNIZIP_VERSION_ERROR (-6)

#define UNIZIP_ALGO_ERROR (-7)

#define ERR_CODE(format, args...) fprintf(stderr, format, ##args)
#endif // SRC_UNIZIP_ERROR_CODE_H_
