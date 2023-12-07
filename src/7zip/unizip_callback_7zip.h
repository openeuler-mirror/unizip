#ifndef SRC_7ZIP_UNIZIP_CALLBACK_7ZIP_H_
#define SRC_7ZIP_UNIZIP_CALLBACK_7ZIP_H_
#include "unizip_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *versionCB_7zip(void);

int deflateInitCB_7zip(unizip_streamp strm, int level);

int deflateCB_7zip(unizip_streamp strm, int flush);

int deflateEndCB_7zip(unizip_streamp strm);

int inflateInitCB_7zip(unizip_streamp strm);

int inflateCB_7zip(unizip_streamp strm, int flush);

int inflateEndCB_7zip(unizip_streamp strm);

int deflateCopyCB_7zip(unizip_streamp dest, unizip_streamp source);

int inflateCopyCB_7zip(unizip_streamp dest, unizip_streamp source);

int deflateResetCB_7zip(unizip_streamp strm);

int inflateResetCB_7zip(unizip_streamp strm);

int compressCB_7zip(Bytef *dest, uLongf *destLen, Bytef *source, uLong sourceLen);

int uncompressCB_7zip(Bytef *dest, uLongf *destLen, Bytef *source, uLong sourceLen);

uLong compressBoundCB_7zip(uLong sourceLen);

#ifdef __cplusplus
}
#endif

#endif