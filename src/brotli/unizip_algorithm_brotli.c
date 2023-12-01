#include "unizip_algorithm_brotli.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define levelMin_brotli 0
#define levelMax_brotli 9
#define levelDef_brotli 6
#define brotliAction_end 4

int brotli_map(int ret)
{
    switch (ret) {
        case 1:
            return UNIZIP_OK;
        case 0:
            return UNIZIP_ERRNO;
        default:
            return UNIZIP_ERRNO;
    }
}

void *brotli_allocate(void)
{
    struct brotli_ctx *ctx;
    if ((ctx = (struct brotli_ctx *)malloc(sizeof(*ctx))) == NULL) {
        return NULL;
    }
    memset(&ctx->Enstate, 0, sizeof(ctx->Enstate));
    memset(&ctx->Destate, 0, sizeof(ctx->Destate));
    return ctx;
}

int brotli_Copy(struct brotli_ctx *dest, struct brotli_ctx *source)
{
    if (dest != NULL && source != NULL) {
        memcpy(dest, source, sizeof(struct brotli_ctx));
        return UNIZIP_OK;
    }
    return UNIZIP_MEM_ERROR;
}

int brotli_deflateInit(struct brotli_ctx *ctx, int level)
{
    if (level > levelMax_brotli || level < levelMin_brotli) {
        ctx->compression_flags = levelDef_brotli;
    }
    if (ctx == NULL) {
        return UNIZIP_MEM_ERROR;
    }
    ctx->Enstate = BrotliEncoderCreateInstance(NULL, NULL, NULL);
    ctx->op = BROTLI_OPERATION_PROCESS;
    ctx->next_in = NULL;
    ctx->available_in = 0;
    ctx->next_out = NULL;
    ctx->available_out = 0;
    ctx->total_out = 0;
    ctx->total_in = 0;
    ctx->avail_in_temp = 0;
    ctx->avail_out_temp = 0;
    int quality = 1;
    int lgwin = 22;
    BrotliEncoderSetParameter(ctx->Enstate, BROTLI_PARAM_QUALITY, quality);
    BrotliEncoderSetParameter(ctx->Enstate, BROTLI_PARAM_LGWIN, lgwin);
    return UNIZIP_OK;
}

int brotli_deflate(struct brotli_ctx *ctx, int flush)
{
    const uint8_t *in = (const uint8_t *)ctx->next_in;
    int ret;
    ctx->avail_in_temp = ctx->available_in;
    ctx->avail_out_temp = ctx->available_out;
    if (flush == brotliAction_end) {
        ctx->op = BROTLI_OPERATION_FINISH;
        ctx->available_in = ctx->avail_in_temp;
        ctx->available_out = ctx->avail_out_temp;
        ret = BrotliEncoderCompressStream(ctx->Enstate, ctx->op, &(ctx->available_in), &in, &(ctx->available_out),
                                          &(ctx->next_out), &(ctx->total_out));
        ctx->next_in += ctx->avail_in_temp - ctx->available_in;
        ctx->total_in += ctx->avail_in_temp - ctx->available_in;
        if (ctx->available_in == ctx->avail_in_temp && ctx->available_out == ctx->avail_out_temp) {
            return UNIZIP_STREAM_END;
        }
        return ret;
    } else if (flush == 0) {
        ctx->op = BROTLI_OPERATION_PROCESS;
        ret = BrotliEncoderCompressStream(ctx->Enstate, ctx->op, &(ctx->available_in), &in, &(ctx->available_out),
                                          &(ctx->next_out), &(ctx->total_out));
        ctx->next_in += ctx->avail_in_temp - ctx->available_in;
        ctx->total_in += ctx->avail_in_temp - ctx->available_in;
        return brotli_map(ret);
    } else {
        return UNIZIP_ERRNO;
    }
}

int brotli_deflateEnd(struct brotli_ctx *ctx)
{
    if (ctx) {
        BrotliEncoderDestroyInstance(ctx->Enstate);
        ctx->op = BROTLI_OPERATION_PROCESS;
        ctx->next_in = NULL;
        ctx->available_in = 0;
        ctx->next_out = NULL;
        ctx->available_out = 0;
        ctx->total_out = 0;
        ctx->total_in = 0;
        ctx->avail_in_temp = 0;
        ctx->avail_out_temp = 0;
        free(ctx);
    }
    return UNIZIP_OK;
}

int brotli_inflateInit(struct brotli_ctx *ctx)
{
    ctx->Destate = BrotliDecoderCreateInstance(NULL, NULL, NULL);
    ctx->op = BROTLI_OPERATION_PROCESS;
    ctx->next_in = NULL;
    ctx->available_in = 0;
    ctx->next_out = NULL;
    ctx->available_out = 0;
    ctx->total_out = 0;
    ctx->total_in = 0;
    ctx->avail_in_temp = 0;
    ctx->avail_out_temp = 0;
    ctx->ret = BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT;
    return UNIZIP_OK;
}

int brotli_inflate(struct brotli_ctx *ctx)
{
    const uint8_t *in = (const uint8_t *)ctx->next_in;
    ctx->avail_in_temp = ctx->available_in;
    ctx->avail_out_temp = ctx->available_out;
    if (ctx->ret == BROTLI_DECODER_RESULT_SUCCESS) {
        if (BrotliDecoderIsFinished(ctx->Destate)) {
            return UNIZIP_STREAM_END;
        }
    } else if (ctx->ret == BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT ||
               ctx->ret == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
        ctx->ret = BrotliDecoderDecompressStream(ctx->Destate, &ctx->available_in, &in, &ctx->available_out,
                                                 &ctx->next_out, 0);
        ctx->total_in += ctx->avail_in_temp - ctx->available_in;
        ctx->next_in += ctx->avail_in_temp - ctx->available_in;
        ctx->total_out += ctx->avail_out_temp - ctx->available_out;
        return UNIZIP_OK;
    } else {
        return UNIZIP_ERRNO;
    }
    return UNIZIP_OK;
}

int brotli_inflateEnd(struct brotli_ctx *ctx)
{
    if (ctx) {
        BrotliDecoderDestroyInstance(ctx->Destate);
        ctx->ret = BROTLI_DECODER_RESULT_SUCCESS;
        ctx->op = BROTLI_OPERATION_PROCESS;
        ctx->next_in = NULL;
        ctx->available_in = 0;
        ctx->next_out = NULL;
        ctx->available_out = 0;
        ctx->total_out = 0;
        ctx->total_in = 0;
        ctx->avail_in_temp = 0;
        ctx->avail_out_temp = 0;
        free(ctx);
    }
    return UNIZIP_OK;
}

int brotli_compress(const uint8_t *source, size_t sourceLen, uint8_t *dest, size_t *destLen)
{
    BrotliEncoderState *s = BrotliEncoderCreateInstance(NULL, NULL, NULL);
    if (!s) {
        return UNIZIP_MEM_ERROR;
    }
    if (BrotliEncoderCompress(BROTLI_DEFAULT_QUALITY, BROTLI_DEFAULT_WINDOW, BROTLI_DEFAULT_MODE, sourceLen, source,
                              destLen, dest)) {
        BrotliEncoderDestroyInstance(s);
        return 0;
    } else {
        BrotliEncoderDestroyInstance(s);
        return UNIZIP_ERRNO;
    }
}

int brotli_decompress(const uint8_t *source, size_t sourceLen, uint8_t *dest, size_t *destLen)
{
    BrotliDecoderState *s = BrotliDecoderCreateInstance(NULL, NULL, NULL);
    if (!s) {
        return UNIZIP_MEM_ERROR;
    }
    if (BrotliDecoderDecompress(sourceLen, source, destLen, dest)) {
        BrotliDecoderDestroyInstance(s);
        return 0;
    } else {
        BrotliDecoderDestroyInstance(s);
        return UNIZIP_ERRNO;
    }
}

unsigned long brotli_compressBound(unsigned long uncompressed_size)
{
    unsigned long compressed_size = (unsigned long)BrotliEncoderMaxCompressedSize((size_t)uncompressed_size);
    if (compressed_size < uncompressed_size) {
        return ULONG_MAX;
    }
    return compressed_size;
}