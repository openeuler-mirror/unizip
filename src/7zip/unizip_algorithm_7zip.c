#include "unizip_algorithm_7zip.h"
#include "unizip_error_code.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int zip7_map_error(int ret) {
  switch (ret) {
  case SZ_OK:
    return UNIZIP_OK;
  case SZ_ERROR_MEM:
    return UNIZIP_MEM_ERROR;

  case SZ_ERROR_DATA:
    return UNIZIP_DATA_ERROR;

  case SZ_ERROR_OUTPUT_EOF:
  case SZ_ERROR_INPUT_EOF:
    return UNIZIP_BUF_ERROR;

  case SZ_ERROR_PARAM:
  case SZ_ERROR_THREAD:
  case SZ_ERROR_UNSUPPORTED:
    return UNIZIP_ERRNO;

  default:
    return UNIZIP_DATA_ERROR;
  }
}

void *zip7_allocate(void) {
  struct zip7_ctx *ctx;
  if ((ctx = (struct zip7_ctx *)malloc(sizeof(*ctx))) == NULL) {
    return NULL;
  }
  return ctx;
}

int zip7_deflateInit(struct zip7_ctx *ctx, int level) {
  if (level < levelMin_7zip || level > levelMax_7zip) {
    ctx->level = levelDefault_7zip;
  } else {
    ctx->level = level;
  }
  ctx->dest = NULL;
  ctx->destLen = 0;
  ctx->src = NULL;
  ctx->srcLen = 0;

  // zlib
  ctx->next_in = NULL;
  ctx->avail_in = 0;
  ctx->total_in = 0;
  ctx->next_out = NULL;
  ctx->avail_out = 0;
  ctx->total_out = 0;

  return UNIZIP_OK;
}

int zip7_deflate(struct zip7_ctx *ctx, int flush) {
  if (ctx->total_in == 0) {
    ctx->src = ctx->next_in;
    ctx->dest = ctx->next_out;
  }

  if (flush == actionEnd_7zip) {
    int res = zip7_deflate2(ctx);
    ctx->total_out = ctx->destLen + LZMA_PROPS_SIZE;
    ctx->next_out = ctx->dest + ctx->total_out;
    if (res != 0) {
      return zip7_map_error(res);
    }
    return UNIZIP_STREAM_END;
  } else {
    ctx->next_in += ctx->avail_in;
    ctx->total_in += ctx->avail_in;
    ctx->next_out += ctx->avail_in;
    ctx->total_out += ctx->avail_in;
    ctx->avail_in = 0;
    ctx->avail_out = 0;
  }

  return UNIZIP_OK;
}

int zip7_deflate2(struct zip7_ctx *ctx) {
  int ret;
  size_t outPropsSize = LZMA_PROPS_SIZE;
  ctx->srcLen = ctx->total_in;
  ctx->destLen = zip7_compressBound(ctx->srcLen);

  // 7zip默认的writeendmark为0，故不能直接调用LzmaCompress
  CLzmaEncProps props;
  LzmaEncProps_Init(&props);
  props.level = ctx->level;

  ret = LzmaEncode((Byte *)(ctx->dest + LZMA_PROPS_SIZE), &(ctx->destLen),
                   (Byte *)ctx->src, ctx->srcLen, &props, (Byte *)ctx->dest,
                   &outPropsSize, 1, NULL, &g_Alloc, &g_Alloc);

  if (outPropsSize != LZMA_PROPS_SIZE) {
    return UNIZIP_DATA_ERROR;
  }
  return zip7_map_error(ret);
}

int zip7_deflateEnd(struct zip7_ctx *ctx) {
  free(ctx);
  return UNIZIP_OK;
}

int zip7_inflateInit(struct zip7_ctx *ctx) {
  ctx->dest = NULL;
  ctx->destLen = 0;
  ctx->src = NULL;
  ctx->srcLen = 0;

  ctx->next_in = NULL;
  ctx->avail_in = 0;
  ctx->total_in = 0;
  ctx->next_out = NULL;
  ctx->avail_out = 0;
  ctx->total_out = 0;

  ctx->status = 0;
  LzmaDec_Construct(&ctx->dec);

  return UNIZIP_OK;
}

int zip7_inflate(struct zip7_ctx *ctx) {
  if (ctx->total_in == 0) {
    ctx->src = ctx->next_in;
    ctx->dest = ctx->next_out;
    SRes res = LzmaDec_Allocate(&ctx->dec, (Byte *)ctx->src, LZMA_PROPS_SIZE,
                                &g_Alloc);
    if (res != 0) {
      return zip7_map_error(res);
    }
    ctx->next_in += LZMA_PROPS_SIZE;
    ctx->total_in += LZMA_PROPS_SIZE;
    LzmaDec_Init(&ctx->dec);
  }

  if (ctx->avail_in == 0) {
    return UNIZIP_STREAM_END;
  }

  ctx->src = ctx->next_in;
  ctx->dest = ctx->next_out;
  ctx->srcLen = ctx->avail_in;
  ctx->destLen = ctx->avail_out;

  LzmaDec_DecodeToBuf(&ctx->dec, (Byte *)ctx->dest, &ctx->destLen,
                      (Byte *)ctx->src, &ctx->srcLen, LZMA_FINISH_END,
                      &ctx->status);

  ctx->next_in += ctx->srcLen;
  ctx->next_out += ctx->destLen;
  ctx->total_in += ctx->srcLen;
  ctx->total_out += ctx->destLen;
  ctx->avail_in -= ctx->srcLen;
  ctx->avail_out -= ctx->destLen;

  if (ctx->status == LZMA_STATUS_FINISHED_WITH_MARK) {
    return UNIZIP_STREAM_END;
  }

  return UNIZIP_OK;
}

int zip7_inflateEnd(struct zip7_ctx *ctx) {
  free(ctx);
  return UNIZIP_OK;
}

int zip7_copy(struct zip7_ctx *dest, struct zip7_ctx *source) {
  if (dest != NULL && source != NULL) {
    memcpy(dest, source, sizeof(struct zip7_ctx));
    return UNIZIP_OK;
  }
  return UNIZIP_ERRNO;
}

int zip7_compress(unsigned char *dest, size_t *destLen, unsigned char *src,
                  size_t srcLen) {
  int ret;
  size_t outPropsSize = LZMA_PROPS_SIZE;
  CLzmaEncProps props;
  LzmaEncProps_Init(&props);

  ret = LzmaEncode(dest + LZMA_PROPS_SIZE, destLen, src, srcLen, &props, dest,
                   &outPropsSize, 1, NULL, &g_Alloc, &g_Alloc);

  return zip7_map_error(ret);
}

int zip7_uncompress(unsigned char *dest, size_t *destLen, unsigned char *src,
                    size_t *srcLen) {
  int ret;
  ret = LzmaUncompress(dest, destLen, src + LZMA_PROPS_SIZE, srcLen, src,
                       LZMA_PROPS_SIZE);
  return zip7_map_error(ret);
}

unsigned long zip7_compressBound(unsigned long uncompressed_size) {
  // https://www.asawicki.info/news_1368_lzma_sdk_-_how_to_use.html
  unsigned long compressed_size =
      (unsigned long)((double)uncompressed_size +
                      (double)uncompressed_size / 3 + 128);

  if (compressed_size < uncompressed_size) {
    // uint64_max
    return ULLONG_MAX;
  }
  return compressed_size;
}
