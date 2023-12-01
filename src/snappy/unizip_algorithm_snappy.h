#ifndef SRC_SNAPPY_UNIZIP_ALGORITHM_SNAPPY_H_
#define SRC_SNAPPY_UNIZIP_ALGORITHM_SNAPPY_H_
#include <unizip_error_code.h>
#include "snappy-c.h"
#ifdef __cplusplus
extern "C" {
#endif
struct snappy_ctx {
    int compression_flags;
    char *input;
    size_t input_length;
    char *compressed;
    size_t compressed_length;
    char *uncompressed;
    size_t uncompressed_length;
    size_t result;
    char *next_in;
    unsigned int avail_in;
    unsigned long total_in;
    char *next_out;
    unsigned int avail_out;
    unsigned long total_out;
};

int snappy_error(int ret);

void *snappy_allocate(void);

int snappy_deflateInit(struct snappy_ctx *ctx, int level);

int snappy_inflateInit(struct snappy_ctx *ctx);

int snappy_deflateEnd(struct snappy_ctx *ctx);

int snappy_inflateEnd(struct snappy_ctx *ctx);

int snappy_inflate(struct snappy_ctx *ctx);

int snappy_deflate(struct snappy_ctx *ctx, int flush);

int snappy_deflateprocess(struct snappy_ctx *ctx);

int snappy_inflateprocess(struct snappy_ctx *ctx);

int snappy_decompress_new(char *source, size_t sourceLen, char *dest, size_t *destLen);

int snappy_compress_new(char *source, size_t sourceLen, char *dest, size_t *destLen);

int snappy_Copy(struct snappy_ctx *dest, struct snappy_ctx *source);

unsigned long snappy_compressBound(unsigned long uncompressed_size);

#ifdef __cplusplus
}
#endif

#endif