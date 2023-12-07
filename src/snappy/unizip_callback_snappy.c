#include "unizip_callback_snappy.h"
#include <stdlib.h>
#include <string.h>
#include "unizip_algorithm_snappy.h"

const char *versionCB_snappy(void) { return " 1.1.10 "; }

void resetSnappy(unizip_streamp strm)
{
    strm->next_in = NULL;
    strm->next_out = NULL;
    strm->avail_in = 0;
    strm->avail_out = 0;
    strm->total_in = 0;
    strm->total_out = 0;
}

int deflateInitCB_snappy(unizip_streamp strm, int level)
{
    strm->ud = snappy_allocate();
    if (strm->ud == NULL) {
        return UNIZIP_MEM_ERROR;
    }
    strm->compression_flag = level;
    int ret = snappy_deflateInit(strm->ud, level);
    resetSnappy(strm);
    return ret;
}

int deflateCB_snappy(unizip_streamp strm, int flush)
{
    struct snappy_ctx *ctx = (struct snappy_ctx *)strm->ud;
    ctx->next_in = (char *)strm->next_in;
    ctx->avail_in = strm->avail_in;
    ctx->total_in = strm->total_in;
    ctx->next_out = (char *)strm->next_out;
    ctx->avail_out = strm->avail_out;
    ctx->total_out = strm->total_out;
    int status = snappy_deflate(strm->ud, flush);
    strm->next_in = (unsigned char *)ctx->next_in;
    strm->avail_in = ctx->avail_in;
    strm->total_in = ctx->total_in;
    strm->next_out = (unsigned char *)ctx->next_out;
    strm->avail_out = ctx->avail_out;
    strm->total_out = ctx->total_out;
    return status;
}

int deflateEndCB_snappy(unizip_streamp strm)
{
    struct snappy_ctx *ctx = (struct snappy_ctx *)strm->ud;
    resetSnappy(strm);
    return snappy_deflateEnd(ctx);
}

int inflateInitCB_snappy(unizip_streamp strm)
{
    strm->ud = snappy_allocate();
    if (strm->ud == NULL) {
        return UNIZIP_MEM_ERROR;
    }
    resetSnappy(strm);
    return snappy_inflateInit(strm->ud);
}

int inflateCB_snappy(unizip_streamp strm, int flush)
{
    struct snappy_ctx *ctx = (struct snappy_ctx *)strm->ud;
    ctx->next_in = (char *)strm->next_in;
    ctx->avail_in = strm->avail_in;
    ctx->total_in = strm->total_in;
    ctx->next_out = (char *)strm->next_out;
    ctx->avail_out = strm->avail_out;
    ctx->total_out = strm->total_out;
    int status = snappy_inflate(strm->ud);
    strm->next_in = (unsigned char *)ctx->next_in;
    strm->avail_in = ctx->avail_in;
    strm->total_in = ctx->total_in;
    strm->next_out = (unsigned char *)ctx->next_out;
    strm->avail_out = ctx->avail_out;
    strm->total_out = ctx->total_out;
    return status;
}

int inflateEndCB_snappy(unizip_streamp strm)
{
    struct snappy_ctx *ctx = (struct snappy_ctx *)strm->ud;
    resetSnappy(strm);
    return snappy_inflateEnd(ctx);
}

int deflateCopyCB_snappy(unizip_streamp dest, unizip_streamp source) { return snappy_Copy(dest->ud, source->ud); }

int inflateCopyCB_snappy(unizip_streamp dest, unizip_streamp source) { return snappy_Copy(dest->ud, source->ud); }

int deflateResetCB_snappy(unizip_streamp strm)
{
    snappy_deflateEnd(strm->ud);
    strm->ud = snappy_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;
    return snappy_deflateInit(strm->ud, strm->compression_flag);
}

int inflateResetCB_snappy(unizip_streamp strm)
{
    snappy_inflateEnd(strm->ud);
    strm->ud = snappy_allocate();
    if (strm->ud == NULL)
        return UNIZIP_MEM_ERROR;

    return snappy_inflateInit(strm->ud);
}

int compressCB_snappy(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    return snappy_compress_new((char *)source, (size_t)sourceLen, (char *)dest, (size_t *)destLen);
}

int uncompressCB_snappy(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    return snappy_decompress_new((char *)source, (size_t)sourceLen, (char *)dest, (size_t *)destLen);
}

uLong compressBoundCB_snappy(uLong sourceLen) { return snappy_compressBound(sourceLen); }
