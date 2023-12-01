/******************************************************************************
 * unizip_callback_bzip2.c
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
#include "unizip_algorithm_zstd.h"
#include <limits.h>
#include <stdlib.h> // 包含 malloc 和 free 的声明
#include <string.h> // 包含 memcpy 的声明

#define zlibAction_end 4
#define zstdAction_end 1
#define zstdAction_process 0

static int map_error(int ret) {
  switch (-ret) {
  case ZSTD_error_no_error:
    return UNIZIP_OK;

  case ZSTD_error_dictionary_corrupted:
  case ZSTD_error_dictionary_wrong:
    return UNIZIP_NEED_DICT;

  case ZSTD_error_memory_allocation:
    return UNIZIP_MEM_ERROR;

  case ZSTD_error_corruption_detected:
  case ZSTD_error_checksum_wrong:
  case ZSTD_error_parameter_unsupported:
  case ZSTD_error_parameter_outOfBound:
    return UNIZIP_DATA_ERROR;

  default:
    return UNIZIP_DATA_ERROR;
  }
}

void *zstd_allocate(void) {
  struct zstd_ctx *ctx;
  if ((ctx = (struct zstd_ctx *)malloc(sizeof(*ctx))) == NULL) {
    return NULL;
  }
  ctx->zdstream = NULL;
  ctx->zcstream = NULL;
  ctx->in.src = NULL;
  ctx->in.pos = 0;
  ctx->in.size = 0;
  ctx->out.dst = NULL;
  ctx->out.pos = 0;
  ctx->out.size = 0;

  return ctx;
}

int zstd_deflatecopy(struct zstd_ctx *dest, struct zstd_ctx *source) {
  if (dest != NULL && source != NULL) {
    memcpy(dest, source, sizeof(struct zstd_ctx));
    return UNIZIP_OK;
  }
  return UNIZIP_DATA_ERROR;
}

int zstd_inflatecopy(struct zstd_ctx *dest, struct zstd_ctx *source) {
  if (dest != NULL && source != NULL) {
    memcpy(dest, source, sizeof(struct zstd_ctx));
    return UNIZIP_OK;
  }
  return UNIZIP_DATA_ERROR;
}

int zstd_deflateInit(struct zstd_ctx *ctx, int level) {
  ctx->in.src = NULL;
  ctx->in.pos = 0;
  ctx->in.size = 0;
  ctx->out.dst = NULL;
  ctx->out.pos = 0;
  ctx->out.size = 0;
  size_t ret;
  int tmpLevel = level;
  if (tmpLevel < ZSTD_minCLevel() || tmpLevel > ZSTD_maxCLevel()) {
    tmpLevel = 0; /* let zstd choose */
  }
  ctx->zcstream = ZSTD_createCStream();
  if (ctx->zcstream == NULL)
    return UNIZIP_MEM_ERROR;
  ret = ZSTD_initCStream(ctx->zcstream, tmpLevel);
  if (ZSTD_isError(ret))
    return UNIZIP_MEM_ERROR;
  return UNIZIP_OK;
}

int zstd_inflateInit(struct zstd_ctx *ctx) {
  ctx->in.src = NULL;
  ctx->in.pos = 0;
  ctx->in.size = 0;
  ctx->out.dst = NULL;
  ctx->out.pos = 0;
  ctx->out.size = 0;
  ctx->zdstream = ZSTD_createDStream();
  if (ctx->zdstream == NULL)
    return UNIZIP_MEM_ERROR;
  return UNIZIP_OK;
}

int zstd_deflateEnd(struct zstd_ctx *ctx) {
  size_t err = 0;
  err = ZSTD_freeCStream(ctx->zcstream);
  ctx->zcstream = NULL;
  ctx->out.dst = NULL;
  ctx->out.pos = 0;
  ctx->out.size = 0;
  ctx->in.src = NULL;
  ctx->in.pos = 0;
  ctx->in.size = 0;
  free(ctx);
  return map_error(err);
}

int zstd_inflateEnd(struct zstd_ctx *ctx) {
  size_t err = 0;
  err = ZSTD_freeDStream(ctx->zdstream);
  ctx->zdstream = NULL;
  ctx->out.dst = NULL;
  ctx->out.pos = 0;
  ctx->out.size = 0;
  ctx->in.src = NULL;
  ctx->in.pos = 0;
  ctx->in.size = 0;
  free(ctx);
  return map_error(err);
}

int zstd_deflate(struct zstd_ctx *ctx, int flush) {
  int action = (flush == zlibAction_end) ? zstdAction_end : zstdAction_process;
  size_t ret;
  ctx->in.src = ctx->next_in;
  ctx->out.dst = ctx->next_out;
  if (ctx->in.size <= ctx->in.pos &&
      (action != 1 || (action == 1 && ctx->out.size == 0 && ctx->in.size == 0 &&
                       ctx->out.pos == 0 && ctx->in.pos == 0))) {
    ctx->in.size = ctx->avail_in;
    ctx->in.pos = 0;
  }
  if (ctx->out.size <= ctx->out.pos) {
    ctx->out.size = ctx->avail_out;
    ctx->out.pos = 0;
  }
  size_t preIn = ctx->in.pos;
  size_t preOut = ctx->out.pos;
  if (action) {
    ret = ZSTD_endStream(ctx->zcstream, &ctx->out);
  } else {
    ret = ZSTD_compressStream(ctx->zcstream, &ctx->out, &ctx->in);
  }
  if (ZSTD_isError(ret)) {
    return map_error(ret);
  }
  ctx->next_in += ctx->in.pos;
  ctx->next_out += ctx->out.pos;
  ctx->total_in += ctx->in.pos - preIn;
  ctx->total_out += ctx->out.pos - preOut;
  if (ret == ZSTD_error_no_error && action) {
    return UNIZIP_STREAM_END;
  }
  return UNIZIP_OK;
}
int zstd_inflate(struct zstd_ctx *ctx) {
  size_t ret;
  ctx->in.src = ctx->next_in;
  ctx->out.dst = ctx->next_out;
  if (ctx->in.size <= ctx->in.pos) {
    ctx->in.size = ctx->avail_in;
    ctx->in.pos = 0;
  }
  if (ctx->out.size <= ctx->out.pos) {
    ctx->out.size = ctx->avail_out;
    ctx->out.pos = 0;
  }
  ret = ZSTD_decompressStream(ctx->zdstream, &ctx->out, &ctx->in);
  if (ZSTD_isError(ret)) {
    return map_error(ret);
  }
  ctx->next_in += ctx->in.pos;
  ctx->next_out += ctx->out.pos;
  ctx->total_in += ctx->in.pos;
  ctx->total_out += ctx->out.pos;
  if (ret == 0)
    return UNIZIP_STREAM_END;
  return UNIZIP_OK;
}

int zstd_compress(char *dest, unsigned long *destLen, const char *source,
                  unsigned long sourceLen) {
  size_t ret = ZSTD_compress(dest, *destLen, source, sourceLen, 0);
  if (ZSTD_isError(ret)) {
    return map_error(ret);
  }
  *destLen = ret;
  return UNIZIP_OK;
}

int zstd_decompress(char *dest, unsigned long *destLen, const char *source,
                    unsigned long sourceLen) {
  size_t ret = ZSTD_decompress(dest, *destLen, source, sourceLen);
  if (ZSTD_isError(ret)) {
    return map_error(ret);
  }
  *destLen = ret;
  return UNIZIP_OK;
}

unsigned long zstd_compressBound(unsigned long uncompressed_size) {
  unsigned long compressed_size = ZSTD_compressBound(uncompressed_size);
  if (compressed_size < uncompressed_size) {
    return ULONG_MAX;
  }
  return compressed_size;
}
