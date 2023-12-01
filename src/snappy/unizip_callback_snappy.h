#ifndef SRC_SNAPPY_UNIZIP_CALLBACK_SNAPPY_H_
#define SRC_SNAPPY_UNIZIP_CALLBACK_SNAPPY_H_
#include "unizip_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *versionCB_bzip2(void);

int deflateInitCB_snappy(unizip_streamp strm, int level);

int deflateCB_snappy(unizip_streamp strm, int flush);

int deflateEndCB_snappy(unizip_streamp strm);

int inflateInitCB_snappy(unizip_streamp strm);

int inflateCB_snappy(unizip_streamp strm, int flush);

int inflateEndCB_snappy(unizip_streamp strm);

int deflateCopyCB_snappy(unizip_streamp dest, unizip_streamp source);

int inflateCopyCB_snappy(unizip_streamp dest, unizip_streamp source);

int deflateResetCB_snappy(unizip_streamp strm);

int inflateResetCB_snappy(unizip_streamp strm);

int compressCB_snappy(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);

int uncompressCB_snappy(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);

uLong compressBound_snappy(uLong sourceLen);

void snappy_init(callback_t *cb_snappy);

#ifdef __cplusplus
}
#endif

#endif