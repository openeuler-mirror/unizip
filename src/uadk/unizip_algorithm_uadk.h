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

#ifndef SRC_UADK_UNIZIP_ALGORITHM_UADK_H_
#define SRC_UADK_UNIZIP_ALGORITHM_UADK_H_

#ifdef __aarch64__

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "uadk/v1/wd.h"
#include "uadk/v1/wd_bmm.h"
#include "uadk/v1/wd_comp.h"
#include "unizip_error_code.h"

#define DMEMSIZE (1024 * 1024) /* 1M */
#define HW_CTX_SIZE (64 * 1024)

/* bytes of data for a request */
#define BLOCK_SIZE (1024 * 1024)
#define REQ_CACHE_NUM 4

/* stream api  */
#define swab32(x)                                                              \
    ((((x)&0x000000ff) << 24) | (((x)&0x0000ff00) << 8) |                      \
     (((x)&0x00ff0000) >> 8) | (((x)&0xff000000) >> 24))

#define cpu_to_be32(x) swab32(x)

/* wrap as zlib basic interface */
#define HZLIB_VERSION "1.0.1"

#ifndef MAX_WBITS
#define MAX_WBITS 15 /* 32K LZ77 window */
#endif

/* compression levels */
#define Z_BEST_SPEED 1
#define Z_BEST_COMPRESSION 9
#define Z_DEFAULT_COMPRESSION (-1)

/* Maximum value for memLevel in deflateInit2 */
#ifndef MAX_MEM_LEVEL
#ifdef MAXSEG_64K
#define MAX_MEM_LEVEL 8
#else
#define MAX_MEM_LEVEL 9
#endif
#endif

/* default memLevel */
#ifndef DEF_MEM_LEVEL
#if MAX_MEM_LEVEL >= 8
#define DEF_MEM_LEVEL 8
#else
#define DEF_MEM_LEVEL MAX_MEM_LEVEL
#endif
#endif

/* compression strategy; see deflateInit2() below for details */
#define Z_DEFAULT_STRATEGY 0

/* default windowBits for decompression. MAX_WBITS is for compression only */
#ifndef DEF_WBITS
#define DEF_WBITS MAX_WBITS
#endif

/* The deflate compression method (the only one supported in this version) */
#define Z_DEFLATED 8

struct zip_stream {
    void *next_in;           /* next input byte */
    unsigned long avail_in;  /* number of bytes available at next_in */
    unsigned long total_in;  /* total nb of input bytes read so far */
    void *next_out;          /* next output byte should be put there */
    unsigned long avail_out; /* remaining free space at next_out */
    unsigned long total_out; /* total nb of bytes output so far */
    char *msg;               /* last error message, NULL if no error */
    void *workspace;         /* memory allocated for this stream */
    int data_type;           /* the data type: ascii or binary */
    unsigned long adler;     /* adler32 value of the uncompressed data */
    void *reserved;          /* reserved for future use */
};

struct zip_ctl {
    void *pool;
    void *in;
    void *out;
    void *ctxbuf;
    void *queue;
    void *ctx;
    void *opdata;
};

struct wd_lock {
    __u8 lock;
};

TAILQ_HEAD(wd_blk_list, wd_blk_hd);
struct wd_blkpool {
    struct wd_lock pool_lock;
    unsigned int free_blk_num;
    unsigned int alloc_failures;
    struct wd_queue *q;
    struct wd_blk_list head;
    void *usr_mem_start;
    void *act_start;
    unsigned int act_hd_sz;
    unsigned int act_blk_sz;
    unsigned long act_mem_sz;
    struct wd_blkpool_setup setup;
};

#define hw_deflateInit(strm, level)                                            \
    hw_deflateInit_(strm, level, HZLIB_VERSION, sizeof(struct zip_stream))
#define hw_inflateInit(strm)                                                   \
    hw_inflateInit_(strm, HZLIB_VERSION, (int)sizeof(struct zip_stream))
#define hw_deflateInit2(strm, level, method, windowBits, memLevel, strategy)   \
    hw_deflateInit2_(strm, level, method, windowBits, memLevel, (strategy),    \
                     HZLIB_VERSION, (int)sizeof(struct zip_stream))
#define hw_inflateInit2(strm, windowBits)                                      \
    hw_inflateInit2_(strm, windowBits, HZLIB_VERSION,                          \
                     (int)sizeof(struct zip_stream))

#ifdef __cplusplus
extern "C" {
#endif

const char *uadk_getVersion(void);

int hw_deflateInit_(struct zip_stream *zstrm, int level, const char *version,
                    int stream_size);

int hw_deflateInit2_(struct zip_stream *zstrm, int level, int method,
                     int windowBits, int memLevel, int strategy,
                     const char *version, int stream_size);

int hw_inflateInit_(struct zip_stream *zstrm, const char *version,
                    int stream_size);

int hw_inflateInit2_(struct zip_stream *zstrm, int windowBits,
                     const char *version, int stream_size);

int hw_deflate(struct zip_stream *zstrm, int flush);

int hw_stream_compress(int alg_type, int blksize, unsigned char *dst,
                       unsigned long *dstlen, unsigned char *src, unsigned long srclen);

int hw_stream_decompress(int alg_type, int blksize, unsigned char *dst,
                         unsigned long *dstlen, unsigned char *src, unsigned long srclen);

int hw_deflateEnd(struct zip_stream *zstrm);

int hw_inflateEnd(struct zip_stream *zstrm);

int hw_copy(struct zip_stream *dest, struct zip_stream *source, int alg_type,
            int comp_optype);

int hw_inflate(struct zip_stream *zstrm, int flush);

#ifdef __cplusplus
}
#endif

#endif // __aarch64__

#endif // SRC_UADK_UNIZIP_ALGORITHM_UADK_H_
