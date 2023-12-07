#include "unizip_callback_brotli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unizip_algorithm_brotli.h"

const char *versionCB_brotli(void)
{
    uint32_t decoderVersion = BrotliDecoderVersion();
    char *version = malloc(16);
    if (version != NULL) {
        int major = decoderVersion >> 24;
        int minor = (decoderVersion >> 12) & 0xFFF;
        int patch = decoderVersion & 0xFFF;
        int len = 0;
        len += snprintf(version + len, sizeof(version) - len, "%d", major);
        len += snprintf(version + len, sizeof(version) - len, ".");
        len += snprintf(version + len, sizeof(version) - len, "%d", minor);
        len += snprintf(version + len, sizeof(version) - len, ".");
        len += snprintf(version + len, sizeof(version) - len, "%d", patch);
    }
    return version;
}

void resetBrotli(unizip_streamp strm)
{
    strm->next_in = NULL;
    strm->next_out = NULL;
    strm->avail_in = 0;
    strm->avail_out = 0;
    strm->total_in = 0;
    strm->total_out = 0;
}

int deflateInitCB_brotli(unizip_streamp strm, int level)
{
    strm->ud = brotli_allocate();
    strm->compression_flag = level;
    int ret = brotli_deflateInit(strm->ud, level);
    resetBrotli(strm);
    return ret;
}

int deflateCB_brotli(unizip_streamp strm, int flush)
{
    struct brotli_ctx *ctx = (struct brotli_ctx *)strm->ud;
    ctx->next_in = strm->next_in;
    ctx->available_in = strm->avail_in;
    ctx->next_out = strm->next_out;
    ctx->available_out = strm->avail_out;
    ctx->total_in = strm->total_in;
    ctx->total_out = strm->total_out;
    int status = brotli_deflate(ctx, flush);
    strm->next_in = ctx->next_in;
    strm->avail_in = ctx->available_in;
    strm->next_out = ctx->next_out;
    strm->avail_out = ctx->available_out;
    strm->total_in = ctx->total_in;
    strm->total_out = ctx->total_out;
    return status;
}

int deflateEndCB_brotli(unizip_streamp strm)
{
    struct brotli_ctx *ctx = (struct brotli_ctx *)strm->ud;
    resetBrotli(strm);
    return brotli_deflateEnd(ctx);
}

int inflateInitCB_brotli(unizip_streamp strm)
{
    strm->ud = brotli_allocate();
    resetBrotli(strm);
    return brotli_inflateInit(strm->ud);
}

int inflateCB_brotli(unizip_streamp strm)
{
    struct brotli_ctx *ctx = (struct brotli_ctx *)strm->ud;
    ctx->next_in = strm->next_in;
    ctx->available_in = strm->avail_in;
    ctx->next_out = strm->next_out;
    ctx->available_out = strm->avail_out;
    ctx->total_in = strm->total_in;
    ctx->total_out = strm->total_out;
    int status = brotli_inflate(ctx);
    strm->next_in = ctx->next_in;
    strm->avail_in = ctx->available_in;
    strm->next_out = ctx->next_out;
    strm->avail_out = ctx->available_out;
    strm->total_in = ctx->total_in;
    strm->total_out = ctx->total_out;
    return status;
}

int inflateEndCB_brotli(unizip_streamp strm)
{
    struct brotli_ctx *ctx = (struct brotli_ctx *)strm->ud;
    resetBrotli(strm);
    return brotli_inflateEnd(ctx);
}

int deflateCopyCB_brotli(unizip_streamp dest, unizip_streamp source) { return brotli_Copy(dest->ud, source->ud); }

int inflateCopyCB_brotli(unizip_streamp dest, unizip_streamp source) { return brotli_Copy(dest->ud, source->ud); }

int deflateResetCB_brotli(unizip_streamp strm)
{
    brotli_deflateEnd(strm->ud);
    strm->ud = brotli_allocate();
    if (strm->ud == NULL) {
        return UNIZIP_MEM_ERROR;
    }
    return brotli_deflateInit(strm->ud, strm->compression_flag);
}

int inflateResetCB_brotli(unizip_streamp strm)
{
    brotli_inflateEnd(strm->ud);
    strm->ud = brotli_allocate();
    if (strm->ud == NULL) {
        return UNIZIP_MEM_ERROR;
    }
    return brotli_inflateInit(strm->ud);
}

int compressCB_brotli(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    return brotli_compress((const uint8_t *)source, (size_t)sourceLen, (uint8_t *)dest, (size_t *)destLen);
}

int uncompressCB_brotli(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    return brotli_decompress((const uint8_t *)source, (size_t)sourceLen, (uint8_t *)dest, (size_t *)destLen);
}

uLong compressBoundCB_brotli(uLong sourceLen) { return brotli_compressBound(sourceLen); }
