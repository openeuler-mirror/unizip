#ifndef UNIZ_CALLBACK_BROTLI_H
#define UNIZ_CALLBACK_BROTLI_H
#include "unizip_struct.h"

const char *versionCB_brotli(void);

int deflateInitCB_brotli(unizip_streamp strm, int level);

int deflateCB_brotli(unizip_streamp strm, int flush);

int deflateEndCB_brotli(unizip_streamp strm);

int inflateInitCB_brotli(unizip_streamp strm);

int inflateCB_brotli(unizip_streamp strm);

int inflateEndCB_brotli(unizip_streamp strm);

int deflateCopyCB_brotli(unizip_streamp dest, unizip_streamp source);

int inflateCopyCB_brotli(unizip_streamp dest, unizip_streamp source);

int deflateResetCB_brotli(unizip_streamp strm);

int inflateResetCB_brotli(unizip_streamp strm);

int compressCB_brotli(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);

int uncompressCB_brotli(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);

uLong compressBound_brotli(uLong sourceLen);

void brotli_init(callback_t *cb_brotli);

#endif