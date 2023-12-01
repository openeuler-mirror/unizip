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

#ifdef __aarch64__

#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <unistd.h>
#include "unizip_algorithm_uadk.h"


static int hw_init(struct zip_stream *zstrm, int alg_type, int comp_optype)
{
    struct wcrypto_comp_ctx_setup ctx_setup;
    struct wcrypto_comp_op_data *opdata;
    struct wd_blkpool_setup mm_setup;
    unsigned int block_mm_num = 3;
    struct wcrypto_paras *priv;
    struct zip_ctl *ctl;
    void *in, *out;
    struct wd_queue *q;
    void *zip_ctx;
    void *pool;
    int ret;

    q = calloc(1, sizeof(struct wd_queue));
    if (q == NULL) {
        return UNIZIP_MEM_ERROR;
    }

    switch (alg_type) {
        case WCRYPTO_ZLIB:
            q->capa.alg = "zlib";
            break;
        case WCRYPTO_GZIP:
            q->capa.alg = "gzip";
            break;
        default:
            ret = UNIZIP_DATA_ERROR;
            goto hw_q_free;
    }

    q->capa.latency = 0;
    q->capa.throughput = 0;

    priv = &q->capa.priv;
    priv->direction = comp_optype;

    ret = wd_request_queue(q);
    if (ret) {
        ERR_CODE("wd_request_queue fail\n");
        goto hw_q_free;
    }
    memset(&mm_setup, 0, sizeof(mm_setup));
    mm_setup.block_size = DMEMSIZE;
    mm_setup.block_num = block_mm_num;
    mm_setup.align_size = 128;
    pool = wd_blkpool_create(q, &mm_setup);
    if (!pool) {
        ERR_CODE("create pool fail\n");
        goto release_q;
    }
    in = wd_alloc_blk(pool);
    out = wd_alloc_blk(pool);

    if (in == NULL || out == NULL) {
        goto buf_free;
    }

    memset(&ctx_setup, 0, sizeof(ctx_setup));
    ctx_setup.alg_type = alg_type;
    ctx_setup.stream_mode = WCRYPTO_COMP_STATEFUL;
    ctx_setup.br.alloc = (void *)wd_alloc_blk;
    ctx_setup.br.free = (void *)wd_free_blk;
    ctx_setup.br.iova_map = (void *)wd_blk_iova_map;
    ctx_setup.br.iova_unmap = (void *)wd_blk_iova_unmap;
    ctx_setup.br.get_bufsize = (void *)wd_blksize;
    ctx_setup.br.usr = pool;
    zip_ctx = wcrypto_create_comp_ctx(q, &ctx_setup);
    if (!zip_ctx) {
        ERR_CODE("zip_alloc_comp_ctx fail\n");
        goto buf_free;
    }
    opdata = calloc(1, sizeof(struct wcrypto_comp_op_data));
    if (opdata == NULL) {
        ERR_CODE("alloc opdata fail\n");
        goto comp_ctx_free;
    }
    opdata->in = in;
    opdata->out = out;
    opdata->stream_pos = WCRYPTO_COMP_STREAM_NEW;
    opdata->alg_type = ctx_setup.alg_type;

    ctl = calloc(1, sizeof(struct zip_ctl));
    if (ctl == NULL) {
        ERR_CODE("alloc ctl fail\n");
        goto comp_opdata_free;
    }

    ctl->pool = pool;
    ctl->in = in; /* temp for opdata->in */
    ctl->out = out;
    ctl->ctx = zip_ctx;
    ctl->queue = q;
    ctl->opdata = opdata;

    zstrm->next_in = in;
    zstrm->next_out = out;
    zstrm->reserved = ctl;

    return UNIZIP_OK;

comp_opdata_free:
    free(opdata);
comp_ctx_free:
    wcrypto_del_comp_ctx(zip_ctx);
buf_free:
    if (in) {
        wd_free_blk(pool, in);
    }
    if (out) {
        wd_free_blk(pool, out);
    }
    wd_blkpool_destroy(pool);
release_q:
    wd_release_queue(q);
hw_q_free:
    free(q);
    return ret == UNIZIP_DATA_ERROR ? UNIZIP_DATA_ERROR : UNIZIP_MEM_ERROR;
}

static void hw_end(struct zip_stream *zstrm)
{
    struct zip_ctl *ctl = zstrm->reserved;
    struct wcrypto_comp_op_data *opdata = ctl->opdata;

    if (ctl->in) {
        wd_free_blk(ctl->pool, ctl->in);
    }
    if (ctl->out) {
        wd_free_blk(ctl->pool, ctl->out);
    }
    if (ctl->ctx) {
        wcrypto_del_comp_ctx(ctl->ctx);
    }
    if (ctl->queue) {
        wd_release_queue(ctl->queue);
        free(ctl->queue);
    }

    wd_blkpool_destroy(ctl->pool);
    free(opdata);
    free(ctl);
}

static unsigned int bit_reverse(register unsigned int x)
{
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));

    return ((x >> 16) | (x << 16));
}

/* output an empty store block */
static int append_store_block(struct zip_stream *zstrm, int flush)
{
    char store_block[5] = {0x1, 0x00, 0x00, 0xff, 0xff};
    struct zip_ctl *ctl = zstrm->reserved;
    struct wcrypto_comp_op_data *opdata = ctl->opdata;
    __u32 checksum = opdata->checksum;
    __u32 isize = opdata->isize;

    memcpy(zstrm->next_out, store_block, 5);
    zstrm->total_out += 5;
    zstrm->avail_out -= 5;
    if (flush != WCRYPTO_FINISH)
        return UNIZIP_STREAM_END;

    if (opdata->alg_type == WCRYPTO_ZLIB) { /* if zlib, ADLER32 */
        checksum = (__u32)cpu_to_be32(checksum);
        memcpy(zstrm->next_out + 5, &checksum, 4);
        zstrm->total_out += 4;
        zstrm->avail_out -= 4;
    } else if (opdata->alg_type == WCRYPTO_GZIP) {
        checksum = ~checksum;
        checksum = bit_reverse(checksum);
        /* if gzip, CRC32 and ISIZE */
        memcpy(zstrm->next_out + 5, &checksum, 4);
        memcpy(zstrm->next_out + 9, &isize, 4);
        zstrm->total_out += 8;
        zstrm->avail_out -= 8;
    } else {
        ERR_CODE("in append store block, wrong alg type %d.\n",
                 opdata->alg_type);
    }
    return UNIZIP_STREAM_END;
}

static int hw_send_and_recv(struct zip_stream *zstrm, int flush)
{
    struct zip_ctl *ctl = zstrm->reserved;
    struct wcrypto_comp_op_data *opdata = ctl->opdata;
    void *zip_ctx = ctl->ctx;
    int ret = 0;

    if (zstrm->avail_in == 0 && flush == WCRYPTO_FINISH) {
        return append_store_block(zstrm, flush);
    }

    opdata->flush = flush;
    opdata->in_len = zstrm->avail_in;
    opdata->avail_out = zstrm->avail_out;

    ret = wcrypto_do_comp(zip_ctx, opdata, NULL);
    if (ret < 0) {
        return ret;
    }
    if (opdata->stream_pos == WCRYPTO_COMP_STREAM_NEW) {
        opdata->stream_pos = WCRYPTO_COMP_STREAM_OLD;
        zstrm->total_out = 0;
        zstrm->total_in = 0;
    }

    zstrm->avail_in = opdata->in_len - opdata->consumed;
    zstrm->avail_out -= opdata->produced;
    zstrm->total_out += opdata->produced;
    zstrm->total_in += opdata->consumed;

    if (zstrm->avail_in > 0) {
        opdata->in += opdata->consumed;
    }
    if (zstrm->avail_in == 0) {
        opdata->in = ctl->in; /* origin addr in */
    }

    if (ret == 3 || (ret == 0 && flush == WCRYPTO_FINISH)) {
        ret = UNIZIP_STREAM_END;
    } else if (ret == 0 && opdata->status == WD_VERIFY_ERR) {
        ret = -WD_VERIFY_ERR; /* crc err */
    } else if (ret == 0 && opdata->status == WD_IN_EPARA) {
        ret = -WD_IN_EPARA; /* msg err */
    } else if (ret == 0 && opdata->status == WCRYPTO_DECOMP_END_NOSPACE) {
        ret = UNIZIP_STREAM_END; /* decomp_is_end region */
    } else if (ret == 0 && opdata->status == WCRYPTO_DECOMP_END) {
        ret = UNIZIP_STREAM_END; /* decomp_is_end region */
    }
    return ret;
}

int hw_deflateInit2_(struct zip_stream *zstrm, int level, int method,
                     int windowBits, int memLevel, int strategy,
                     const char *version, int stream_size)
{
    int alg_type;
    int wrap = 0;

    if (windowBits < 0) { /* suppress zlib wrapper */
        wrap = 0;
        windowBits = -windowBits;
    } else if (windowBits > 15) {
        wrap = 2; /* write gzip wrapper instead */
        windowBits -= 16;
    }

    if (wrap & 0x02)
        alg_type = WCRYPTO_GZIP;
    else
        alg_type = WCRYPTO_ZLIB;
    return hw_init(zstrm, alg_type, WCRYPTO_DEFLATE);
}

int hw_deflateInit_(struct zip_stream *zstrm, int level, const char *version,
                    int stream_size)
{
    if (zstrm == NULL) {
        return UNIZIP_DATA_ERROR;
    }
    return hw_deflateInit2_(zstrm, level, Z_DEFLATED, MAX_WBITS, DEF_MEM_LEVEL,
                            Z_DEFAULT_STRATEGY, version, stream_size);
}

int hw_deflate(struct zip_stream *zstrm, int flush)
{
    int ret;
    if (zstrm == NULL) {
        return UNIZIP_DATA_ERROR;
    }
    ret = hw_send_and_recv(zstrm, flush);
    return ret < 0 ? UNIZIP_STREAM_ERROR : ret;
}

int hw_deflateEnd(struct zip_stream *zstrm)
{
    if (zstrm == NULL) {
        return UNIZIP_DATA_ERROR;
    }
    hw_end(zstrm);
    return UNIZIP_OK;
}

int hw_inflateInit2_(struct zip_stream *zstrm, int windowBits,
                     const char *version, int stream_size)
{
    int wrap, alg_type;

    /* extract wrap request from windowBits parameter */
    if (windowBits < 0) {
        wrap = 0;
        windowBits = -windowBits;
    } else {
        wrap = (windowBits >> 4) + 5;
    }
    if (wrap & 0x01)
        alg_type = WCRYPTO_ZLIB;
    if (wrap & 0x02)
        alg_type = WCRYPTO_GZIP;

    return hw_init(zstrm, alg_type, WCRYPTO_INFLATE);
}

int hw_inflateInit_(struct zip_stream *zstrm, const char *version,
                    int stream_size)
{
    if (zstrm == NULL) {
        return UNIZIP_DATA_ERROR;
    }
    return hw_inflateInit2_(zstrm, DEF_WBITS, version, stream_size);
}

int hw_inflate(struct zip_stream *zstrm, int flush)
{
    int ret;
    if (zstrm == NULL)
        return UNIZIP_DATA_ERROR;
    ret = hw_send_and_recv(zstrm, flush);
    return ret < 0 ? UNIZIP_STREAM_ERROR : ret;
}

int hw_inflateEnd(struct zip_stream *zstrm)
{
    if (zstrm == NULL)
        return UNIZIP_DATA_ERROR;
    hw_end(zstrm);
    return UNIZIP_OK;
}

int hw_stream_compress(int alg_type, int blksize, unsigned char *dst,
                       unsigned long *dstlen, unsigned char *src,
                       unsigned long srclen)
{
    int flush, have;
    int ret;
    int level = 0;
    struct zip_stream zstrm;
    int windowBits = 15;
    int GZIP_ENCODING = 16;

    if (blksize < 0 || dst == NULL || src == NULL)
        return UNIZIP_DATA_ERROR;
    unsigned long stream_chunk = blksize;
    *dstlen = 0;

    if (alg_type == WCRYPTO_ZLIB) {
        ret = hw_deflateInit(&zstrm, level);
    } else {
        /* deflate for gzip data */
        ret = hw_deflateInit2(&zstrm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                              windowBits | GZIP_ENCODING, DEF_MEM_LEVEL,
                              Z_DEFAULT_STRATEGY);
    }
    if (ret != UNIZIP_OK) {
        return ret;
    }
    do {
        if (srclen > stream_chunk) {
            memcpy(zstrm.next_in, src, stream_chunk);
            src += stream_chunk;
            zstrm.avail_in = stream_chunk;
            srclen -= stream_chunk;
        } else {
            memcpy(zstrm.next_in, src, srclen);
            zstrm.avail_in = srclen;
            srclen = 0;
        }

        flush = srclen ? WCRYPTO_SYNC_FLUSH : WCRYPTO_FINISH;
        do {
            zstrm.avail_out = stream_chunk;
            ret = hw_deflate(&zstrm, flush);
            if (ret < 0) {
                hw_end(&zstrm);
                return ret;
            }
            have = stream_chunk - zstrm.avail_out;
            memcpy(dst, zstrm.next_out, have);
            dst += have;
            *dstlen += have;
        } while (zstrm.avail_in > 0);
        /* done when last data in file processed */
    } while (flush != WCRYPTO_FINISH);
    hw_end(&zstrm);
    return UNIZIP_OK;
}

int hw_stream_decompress(int alg_type, int blksize, unsigned char *dst,
                         unsigned long *dstlen, unsigned char *src,
                         unsigned long srclen)
{
    struct zip_stream zstrm;
    unsigned long out_size = 0;
    int have;
    int ret;

    if (blksize < 0 || dst == NULL || src == NULL)
        return UNIZIP_DATA_ERROR;
    unsigned long stream_chunk = blksize;

    if (alg_type == WCRYPTO_ZLIB) {
        ret = hw_inflateInit(&zstrm);
    } else {
        /* inflate for gzip data */
        ret = hw_inflateInit2(&zstrm, 16 + MAX_WBITS);
    }
    if (ret != UNIZIP_OK) {
        return ret;
    }
    do {
        if (srclen > stream_chunk) {
            memcpy(zstrm.next_in, src, stream_chunk);
            src += stream_chunk;
            zstrm.avail_in = stream_chunk;
            srclen -= stream_chunk;
        } else {
            memcpy(zstrm.next_in, src, srclen);
            zstrm.avail_in = srclen;
            srclen = 0;
        }

        if (zstrm.avail_in == 0) {
            ret = UNIZIP_STREAM_END;
            break;
        }

        /* finish compression if all of source has been read in */
        do {
            zstrm.avail_out = stream_chunk;
            ret = hw_inflate(&zstrm, WCRYPTO_SYNC_FLUSH);
            have = stream_chunk - zstrm.avail_out;
            if (ret < 0 || zstrm.total_out > *dstlen) {
                hw_end(&zstrm);
                return ret < 0 ? ret : UNIZIP_MEM_ERROR;
            }
            memcpy(dst, zstrm.next_out, have);
            dst += have;
            out_size += have;
        } while (zstrm.avail_in > 0);
        /* done when last data in file processed */
    } while (ret != UNIZIP_STREAM_END);
    *dstlen = out_size;
    hw_end(&zstrm);
    return UNIZIP_OK;
}

const char *uadk_getVersion(void)
{
    FILE *fp = popen("uadk_tool dfx --version", "r");
    if (fp == NULL) {
        return NULL;
    }
    char *version = NULL;
    char buffer[10000];
    size_t bytesRead = fread(buffer, 1, sizeof(buffer), fp);

    if (bytesRead > 0) {
        // 寻找版本信息的起始位置
        const char *versionPrefix = "UADK version: ";
        char *versionStart = strstr(buffer, versionPrefix);

        if (versionStart != NULL) {
            // 定位到版本号的起始位置
            versionStart += strlen(versionPrefix);

            // 寻找版本号的结束位置
            char *versionEnd = strchr(versionStart, '\n');
            if (versionEnd != NULL) {
                // 计算版本号的长度
                size_t versionLength = versionEnd - versionStart;

                // 分配内存保存版本号
                version = (char *)malloc(versionLength + 1);
                if (version != NULL) {
                    // 复制版本号到新分配的内存中
                    strncpy(version, versionStart, versionLength);
                    version[versionLength] = '\0'; // 确保字符串结尾
                }
            }
        }
    }
    pclose(fp); // 关闭文件流
    return version;
}

int hw_copy(struct zip_stream *dest, struct zip_stream *source, int alg_type,
            int comp_optype)
{
    if (dest != NULL && source != NULL) {
        memcpy(dest, source, sizeof(struct zip_stream));
        if (source->reserved != NULL) {
            dest->reserved =
                calloc(1, sizeof(struct zip_ctl)); // 创建reversed的空间 zip_ctl
            struct zip_ctl *dst_ctl = dest->reserved;
            struct zip_ctl *src_ctl = source->reserved;
            dst_ctl->opdata = calloc(
                1, sizeof(struct wcrypto_comp_op_data)); // 创建opdata的空间
                                                         // wcrypto_comp_op_data
            if (dest->reserved == NULL || dst_ctl->opdata == NULL)
                return UNIZIP_MEM_ERROR;
            memcpy(dst_ctl->opdata, src_ctl->opdata,
                   sizeof(struct wcrypto_comp_op_data));
        }
        return UNIZIP_OK;
    }
    return UNIZIP_DATA_ERROR;
}

#endif // __aarch64__
