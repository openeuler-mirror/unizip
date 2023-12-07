#ifndef SRC_7ZIP_UNIZIP_ALGORITHM_7ZIP_H_
#define SRC_7ZIP_UNIZIP_ALGORITHM_7ZIP_H_
#include "lzma-sdk/C/7zAlloc.h"
#include "lzma-sdk/C/7zTypes.h"
#include "lzma-sdk/C/Alloc.h"
#include "lzma-sdk/C/LzmaDec.h"
#include "lzma-sdk/C/LzmaEnc.h"
#include "lzma-sdk/C/LzmaLib.h"
#include "lzma-sdk/C/Precomp.h"

#define levelMin_7zip 1
#define levelMax_7zip 9
#define levelDefault_7zip 5
#define actionEnd_7zip 4

#ifdef __cplusplus
extern "C" {
#endif

struct zip7_ctx {
    char *dest;
    size_t destLen;
    char *src;
    size_t srcLen;

    int level;

    // inflate
    CLzmaDec dec;
    ELzmaStatus status;

    // zlib
    char *next_in;
    int avail_in;
    long total_in;
    char *next_out;
    int avail_out;
    long total_out;
};

int zip7_map_error(int ret);

void *zip7_allocate(void);

int zip7_deflateInit(struct zip7_ctx *ctx, int level);

int zip7_inflateInit(struct zip7_ctx *ctx);

int zip7_deflate(struct zip7_ctx *ctx, int flush);

int zip7_inflate(struct zip7_ctx *ctx);

int zip7_deflate2(struct zip7_ctx *ctx);

int zip7_deflateEnd(struct zip7_ctx *ctx);

int zip7_inflateEnd(struct zip7_ctx *ctx);

int zip7_copy(struct zip7_ctx *dest, struct zip7_ctx *source);

int zip7_compress(unsigned char *dest, size_t *destLen, unsigned char *src,
                  size_t srcLen);

int zip7_uncompress(unsigned char *dest, size_t *destLen, unsigned char *src,
                    size_t *srcLen);

unsigned long zip7_compressBound(unsigned long uncompressed_size);

#ifdef __cplusplus
}
#endif

#endif