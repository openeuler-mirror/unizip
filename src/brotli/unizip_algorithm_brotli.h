#ifndef SRC_BROTLI_UNIZIP_ALGORITHM_BROTLI_H_  
#define SRC_BROTLI_UNIZIP_ALGORITHM_BROTLI_H_  
#include <brotli/decode.h>
#include <brotli/encode.h>
#include <unizip_error_code.h>

struct brotli_ctx {
    int compression_flags;
    BrotliEncoderState *Enstate;
    BrotliDecoderState *Destate;
    BrotliEncoderOperation op;
    uint8_t *next_in;
    size_t available_in;
    uint8_t *next_out;
    size_t available_out;
    size_t total_in;
    size_t total_out;
    size_t avail_in_temp;
    size_t avail_out_temp;
    BrotliDecoderResult ret;
};

int brotli_map(int ret);

void *brotli_allocate(void);

int brotli_deflateInit(struct brotli_ctx *ctx, int level);

int brotli_inflateInit(struct brotli_ctx *ctx);

int brotli_deflateEnd(struct brotli_ctx *ctx);

int brotli_inflateEnd(struct brotli_ctx *ctx);

int brotli_inflate(struct brotli_ctx *ctx);

int brotli_deflate(struct brotli_ctx *ctx, int flush);

int brotli_compress(const uint8_t *source, size_t sourceLen, uint8_t *dest, size_t *destLen);

int brotli_decompress(const uint8_t *source, size_t sourceLen, uint8_t *dest, size_t *destLen);

int brotli_Copy(struct brotli_ctx *dest, struct brotli_ctx *source);

unsigned long brotli_compressBound(unsigned long uncompressed_size);

#endif