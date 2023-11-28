/******************************************************************************
 * unizip_struct.h
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
#ifndef SRC_UNIZIP_STRUCT_H_
#define SRC_UNIZIP_STRUCT_H_
#include <zlib.h>

#define CASE_ZLIB 1
#define CASE_BZIP2 2
#define CASE_BROTLI 3
#define CASE_LZ4 4
#define CASE_SNAPPY 5
#define CASE_ZSTD 6
#define CASE_7ZIP 7
#define CASE_UADK 8
#define CASE_XZ 9
#define CASE_GZIP 10

typedef struct unizip_stream_s {
    z_const Bytef *next_in; /* next input byte */
    uInt avail_in;          /* number of bytes available at next_in */
    uLong total_in;         /* total number of input bytes read so far */

    Bytef *next_out; /* next output byte will go here */
    uInt avail_out;  /* remaining free space at next_out */
    uLong total_out; /* total number of bytes output so far */

    z_const char *msg;                /* last error message, NULL if no error */
    struct internal_state FAR *state; /* not visible by applications */

    alloc_func zalloc; /* used to allocate the internal state */
    free_func zfree;   /* used to free the internal state */
    voidpf opaque;     /* private data object passed to zalloc and zfree */

    int data_type;  /* best guess about the data type: binary or text */
                    /* for deflate, or the decoding state for inflate */
    uLong adler;    /* Adler-32 or CRC-32 value of the uncompressed data */
    uLong reserved; /* reserved for future use */

    // 新增属性
    int compression_flag; // unused

    void *ud; // 下层压缩库所需的数据结构将存储在这里
} unizip_stream;

typedef unizip_stream *unizip_streamp;

typedef struct callback {
    const char *(*versionCB)();
    int (*deflateInitCB)();
    int (*deflateCB)();
    int (*deflateEndCB)();
    int (*inflateInitCB)();
    int (*inflateCB)();
    int (*inflateEndCB)();
    int (*deflateCopyCB)();
    int (*inflateCopyCB)();
    int (*deflateResetCB)();
    int (*inflateResetCB)();
    int (*compressCB)();
    int (*uncompressCB)();
    uLong (*compressBoundCB)();
} callback_t;

#endif // SRC_UNIZIP_STRUCT_H_
