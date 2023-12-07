#include "unizip_callback_7zip.h"
#include "lzma-sdk/C/7zVersion.h"
#include "lzma-sdk/C/LzmaLib.h"
#include "unizip_algorithm_7zip.h"

const char *versionCB_7zip(void) { return MY_VERSION_NUMBERS; }

void reset_7zip(unizip_streamp strm)
{
    strm->next_in = NULL;
    strm->next_out = NULL;
    strm->avail_in = 0;
    strm->avail_out = 0;
    strm->total_in = 0;
    strm->total_out = 0;
}

int deflateInitCB_7zip(unizip_streamp strm, int level)
{
    strm->ud = zip7_allocate();
    strm->compression_flag = level;
    struct zip7_ctx *ctx = (struct zip7_ctx *)strm->ud;
    int ret = zip7_deflateInit(ctx, level);
    reset_7zip(strm);
    return ret;
}

int deflateCB_7zip(unizip_streamp strm, int flush)
{
    struct zip7_ctx *ctx = (struct zip7_ctx *)strm->ud;
    ctx->next_in = (char *)strm->next_in;
    ctx->next_out = (char *)strm->next_out;
    ctx->avail_in = strm->avail_in;
    ctx->avail_out = strm->avail_out;
    ctx->total_in = strm->total_in;
    ctx->total_out = strm->total_out;
    int ret = zip7_deflate(ctx, flush);
    strm->next_in = (Bytef *)ctx->next_in;
    strm->next_out = (Bytef *)ctx->next_out;
    strm->avail_in = ctx->avail_in;
    strm->avail_out = ctx->avail_out;
    strm->total_in = ctx->total_in;
    strm->total_out = ctx->total_out;
    return ret;
}

int deflateEndCB_7zip(unizip_streamp strm)
{
    struct zip7_ctx *ctx = (struct zip7_ctx *)strm->ud;
    reset_7zip(strm);
    return zip7_deflateEnd(ctx);
}

int inflateInitCB_7zip(unizip_streamp strm)
{
    strm->ud = zip7_allocate();
    struct zip7_ctx *ctx = (struct zip7_ctx *)strm->ud;
    int ret = zip7_inflateInit(ctx);
    reset_7zip(strm);
    return ret;
}

int inflateCB_7zip(unizip_streamp strm, int flush)
{
    struct zip7_ctx *ctx = (struct zip7_ctx *)strm->ud;
    ctx->next_in = (char *)strm->next_in;
    ctx->next_out = (char *)strm->next_out;
    ctx->avail_in = strm->avail_in;
    ctx->avail_out = strm->avail_out;
    ctx->total_in = strm->total_in;
    ctx->total_out = strm->total_out;
    int ret = zip7_inflate(ctx);
    strm->next_in = (Bytef *)ctx->next_in;
    strm->next_out = (Bytef *)ctx->next_out;
    strm->avail_in = ctx->avail_in;
    strm->avail_out = ctx->avail_out;
    strm->total_in = ctx->total_in;
    strm->total_out = ctx->total_out;
    return ret;
}
int inflateEndCB_7zip(unizip_streamp strm)
{
    struct zip7_ctx *ctx = (struct zip7_ctx *)strm->ud;
    reset_7zip(strm);
    return zip7_inflateEnd(ctx);
}

int deflateCopyCB_7zip(unizip_streamp dest, unizip_streamp source)
{
    return zip7_copy(dest->ud, source->ud);
}

int inflateCopyCB_7zip(unizip_streamp dest, unizip_streamp source)
{
    return zip7_copy(dest->ud, source->ud);
}

int deflateResetCB_7zip(unizip_streamp strm)
{
    zip7_deflateEnd(strm->ud);
    strm->ud = zip7_allocate();
    if (strm->compression_flag > levelMax_7zip ||
        strm->compression_flag < levelMin_7zip) {
        strm->compression_flag = levelDefault_7zip;
    }
    return zip7_deflateInit(strm->ud, strm->compression_flag);
}

int inflateResetCB_7zip(unizip_streamp strm)
{
    zip7_inflateEnd(strm->ud);
    strm->ud = zip7_allocate();
    return zip7_inflateInit(strm->ud);
}

int compressCB_7zip(Bytef *dest, uLongf *destLen, Bytef *source,
                    uLong sourceLen)
{
    int ret = zip7_compress((unsigned char *)dest, destLen,
                            (unsigned char *)source, sourceLen);
    return ret;
}

int uncompressCB_7zip(Bytef *dest, uLongf *destLen, Bytef *source,
                      uLong sourceLen)
{
    int ret = zip7_uncompress((unsigned char *)dest, destLen,
                              (unsigned char *)source, &sourceLen);
    return ret;
}

uLong compressBoundCB_7zip(uLong sourceLen)
{
    return zip7_compressBound(sourceLen);
}

