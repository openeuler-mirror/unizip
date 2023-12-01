#include "unizip_algorithm_snappy.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define SnappyAction_End 4

int snappy_error(int ret)
{
    switch (ret) {
        case SNAPPY_OK:
            return UNIZIP_OK;
        case SNAPPY_INVALID_INPUT:
            return UNIZIP_DATA_ERROR;
        case SNAPPY_BUFFER_TOO_SMALL:
            return UNIZIP_BUF_ERROR;
        default:
            return UNIZIP_ERRNO;
    }
}

void *snappy_allocate(void)
{
    struct snappy_ctx *ctx;
    if ((ctx = (struct snappy_ctx *)malloc(sizeof(*ctx))) == NULL) {
        return NULL;
    }
    return ctx;
}

int snappy_Copy(struct snappy_ctx *dest, struct snappy_ctx *source)
{
    if (dest != NULL && source != NULL) {
        memcpy(dest, source, sizeof(struct snappy_ctx));
        return UNIZIP_OK;
    }
    return UNIZIP_MEM_ERROR;
}

int snappy_deflateInit(struct snappy_ctx *ctx, int level)
{
    if (ctx == NULL) {
        return UNIZIP_MEM_ERROR;
    }
    ctx->input = NULL;
    ctx->input_length = 0;
    ctx->compressed = NULL;
    ctx->compressed_length = 0;
    ctx->uncompressed = NULL;
    ctx->uncompressed_length = 0;
    ctx->result = 0;
    ctx->next_in = NULL;
    ctx->avail_in = 0;
    ctx->total_in = 0;
    ctx->next_out = NULL;
    ctx->avail_out = 0;
    ctx->total_out = 0;
    return UNIZIP_OK;
}

int snappy_deflate(struct snappy_ctx *ctx, int flush)
{
    if (flush == 0) {
        return UNIZIP_OK;
    } else if (flush == SnappyAction_End) {
        ctx->input = ctx->next_in;
        ctx->compressed = ctx->next_out;
        ctx->total_in = ctx->avail_in;
        ctx->avail_in = 0;
        return snappy_deflateprocess(ctx);
    } else {
        return (UNIZIP_ERRNO);
    }
}

int snappy_deflateprocess(struct snappy_ctx *ctx)
{
    ctx->input_length = ctx->total_in;
    ctx->compressed_length = snappy_max_compressed_length(ctx->input_length);
    int ret = snappy_compress(ctx->input, ctx->input_length, ctx->compressed, &(ctx->compressed_length));
    ctx->total_out = ctx->compressed_length;
    return snappy_error(ret);
}

int snappy_deflateEnd(struct snappy_ctx *ctx)
{
    if (ctx) {
        ctx->input = NULL;
        ctx->input_length = 0;
        ctx->compressed = NULL;
        ctx->uncompressed = NULL;
        ctx->uncompressed_length = 0;
        ctx->result = 0;
        ctx->next_in = NULL;
        ctx->avail_in = 0;
        ctx->total_in = 0;
        ctx->next_out = NULL;
        ctx->avail_out = 0;
        ctx->total_out = 0;
        free(ctx);
    }
    return UNIZIP_OK;
}

int snappy_inflateInit(struct snappy_ctx *ctx)
{
    ctx->input = NULL;
    ctx->input_length = 0;
    ctx->compressed = NULL;
    ctx->compressed_length = 0;
    ctx->uncompressed = NULL;
    ctx->uncompressed_length = 0;
    ctx->result = 0;
    ctx->next_in = NULL;
    ctx->avail_in = 0;
    ctx->total_in = 0;
    ctx->next_out = NULL;
    ctx->avail_out = 0;
    ctx->total_out = 0;
    return UNIZIP_OK;
}

int snappy_inflate(struct snappy_ctx *ctx)
{
    ctx->compressed = ctx->next_in;
    ctx->uncompressed = ctx->next_out;
    ctx->total_in = ctx->avail_in;
    ctx->avail_in = 0;
    return snappy_inflateprocess(ctx);
}

int snappy_inflateprocess(struct snappy_ctx *ctx)
{
    ctx->compressed_length = ctx->total_in;
    snappy_uncompressed_length(ctx->compressed, ctx->compressed_length, &(ctx->result));
    ctx->uncompressed_length = ctx->result;
    int ret =
        snappy_uncompress(ctx->compressed, ctx->compressed_length, ctx->uncompressed, &(ctx->uncompressed_length));
    ctx->total_out = ctx->uncompressed_length;
    return snappy_error(ret);
}

int snappy_inflateEnd(struct snappy_ctx *ctx)
{
    if (ctx) {
        ctx->input = NULL;
        ctx->input_length = 0;
        ctx->compressed = NULL;
        ctx->compressed_length = 0;
        ctx->uncompressed = NULL;
        ctx->uncompressed_length = 0;
        ctx->result = 0;
        ctx->next_in = NULL;
        ctx->avail_in = 0;
        ctx->total_in = 0;
        ctx->next_out = NULL;
        ctx->avail_out = 0;
        ctx->total_out = 0;
        free(ctx);
    }
    return UNIZIP_OK;
}

int snappy_compress_new(char *source, size_t sourceLen, char *dest, size_t *destLen)
{
    if (dest == NULL || destLen == NULL || source == NULL) {
        return UNIZIP_DATA_ERROR;
    }
    struct snappy_ctx *ctx = snappy_allocate();
    if (ctx == NULL) {
        return UNIZIP_MEM_ERROR;
    }
    int ret;
    ret = snappy_deflateInit(ctx, 0);
    if (ret != SNAPPY_OK) {
        return ret;
    }
    ctx->input = source;
    ctx->input_length = sourceLen;
    ctx->compressed = dest;
    ctx->compressed_length = *destLen;
    ret = snappy_compress(ctx->input, ctx->input_length, ctx->compressed, &(ctx->compressed_length));
    *destLen = ctx->compressed_length;
    return snappy_error(ret);
}

int snappy_decompress_new(char *source, size_t sourceLen, char *dest, size_t *destLen)
{
    if (dest == NULL || destLen == NULL || source == NULL) {
        return UNIZIP_DATA_ERROR;
    }
    struct snappy_ctx *ctx = snappy_allocate();
    if (ctx == NULL) {
        return UNIZIP_MEM_ERROR;
    }
    int ret;
    ret = snappy_inflateInit(ctx);
    if (ret != SNAPPY_OK) {
        return ret;
    }
    ctx->compressed = source;
    ctx->compressed_length = sourceLen;
    ctx->uncompressed = dest;
    ctx->uncompressed_length = *destLen;
    ret = snappy_uncompress(ctx->compressed, ctx->compressed_length, ctx->uncompressed, &(ctx->uncompressed_length));
    *destLen = ctx->uncompressed_length;
    return snappy_error(ret);
}

unsigned long snappy_compressBound(unsigned long uncompressed_size)
{
    unsigned long compressed_size = (unsigned long)snappy_max_compressed_length((size_t)uncompressed_size);
    if (compressed_size < uncompressed_size) {
        return ULONG_MAX;
    }
    return compressed_size;
}