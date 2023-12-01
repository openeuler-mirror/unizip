#include <gtest/gtest.h>

#include "test_util.h"
#include "unizip_adapt.h"
#include <brotli/decode.h>
#include <brotli/encode.h>

void Brotli_Deflate_func(char *next_in, char *next_out, uLong avail_in,
                         uLong avail_out) {
  const uint8_t *in = (const uint8_t *)next_in;
  uint8_t *out = (uint8_t *)next_out;
  int quality = 1;
  int lgwin = 22;
  BrotliEncoderState *s = BrotliEncoderCreateInstance(NULL, NULL, NULL);
  BrotliEncoderSetParameter(s, BROTLI_PARAM_QUALITY, quality);
  BrotliEncoderSetParameter(s, BROTLI_PARAM_LGWIN, lgwin);
  BrotliEncoderOperation op = BROTLI_OPERATION_FINISH;
  ulong available_in = avail_in;
  ulong available_out = avail_out;
  BrotliEncoderCompressStream(s, op, &available_in, &in, &available_out, &out,
                              0);
}
void Brotli_Inflate_func(char *next_in, char *next_out, uLong avail_in,
                         uLong avail_out) {
  const uint8_t *in = (const uint8_t *)next_in;
  uint8_t *out = (uint8_t *)next_out;
  ulong available_in = avail_in;
  ulong available_out = avail_out;
  BrotliDecoderState *s = BrotliDecoderCreateInstance(NULL, NULL, NULL);
  BrotliDecoderDecompressStream(s, &available_in, &in, &available_out, &out, 0);
}

const util_func util_Brotli = {Brotli_Deflate_func, Brotli_Inflate_func};

TEST(Brotli_testcases, testVersion) {
  SetValue(3);
  test_version();
}
TEST(Brotli_testcases, testDeflateInitEnd) {
  SetValue(3);
  test_DeflateInitEnd();
}
// TEST(Brotli_testcases, testDeflateSeg)
// {
//     SetValue(3);
//     test_DeflateSeg(&util_Brotli, 20, nullptr);
// }
TEST(Brotli_testcases, testDeflateAll) {
  SetValue(3);
  test_DeflateAll(&util_Brotli, 2, nullptr);
}
TEST(Brotli_testcases, testDeflateCopyReset) {
  SetValue(3);
  test_DeflateCopyReset();
}
TEST(Brotli_testcases, testInflateInitEnd) {
  SetValue(3);
  test_InflateInitEnd();
}
TEST(Brotli_testcases, testInflateSeg) {
  SetValue(3);
  test_InflateSeg(&util_Brotli, 2, nullptr);
}
TEST(Brotli_testcases, testInflateAll) {
  SetValue(3);
  test_InflateAll(&util_Brotli, 20, nullptr);
}
TEST(Brotli_testcases, testInflateCopyReset) {
  SetValue(3);
  test_InflateCopyReset();
}
