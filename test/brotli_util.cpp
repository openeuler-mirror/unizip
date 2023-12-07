#include <gtest/gtest.h>

#include "test_util.h"
#include "unizip_adapt.h"
#include <brotli/decode.h>
#include <brotli/encode.h>

void Brotli_Deflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    const uint8_t *in = (const uint8_t *)next_in;
    uint8_t *out = (uint8_t *)next_out;
    int quality = 11;
    int lgwin = 22;
    BrotliEncoderState *s = BrotliEncoderCreateInstance(NULL, NULL, NULL);
    BrotliEncoderSetParameter(s, BROTLI_PARAM_QUALITY, quality);
    BrotliEncoderSetParameter(s, BROTLI_PARAM_LGWIN, lgwin);
    BrotliEncoderOperation op = BROTLI_OPERATION_FINISH;
    ulong available_in = avail_in;
    ulong available_out = avail_out;
    BrotliEncoderCompressStream(s, op, &available_in, &in, &available_out, &out, 0);
}
void Brotli_Inflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    const uint8_t *in = (const uint8_t *)next_in;
    uint8_t *out = (uint8_t *)next_out;
    ulong available_in = avail_in;
    ulong available_out = avail_out;
    BrotliDecoderState *s = BrotliDecoderCreateInstance(NULL, NULL, NULL);
    BrotliDecoderDecompressStream(s, &available_in, &in, &available_out, &out, 0);
}

void Brotli_SetParamsDeflate_fun(unizip_streamp strm) { return; }

void Brotli_SetParamsInflate_fun(unizip_streamp strm) { return; }

const util_func util_Brotli = {Brotli_Deflate_func, Brotli_Inflate_func, Brotli_SetParamsDeflate_fun,
                               Brotli_SetParamsInflate_fun};

TEST(Brotli_testcases, testVersion)
{
    SetValue(2);
    test_version();
}
TEST(Brotli_testcases, testDeflateInitEnd)
{
    SetValue(2);
    test_DeflateInitEnd();
}
TEST(Brotli_testcases, testDeflateSeg)
{
    SetValue(2);
    test_DeflateSeg(&util_Brotli, 0);
}
TEST(Brotli_testcases, testDeflateAll)
{
    SetValue(2);
    test_DeflateAll(&util_Brotli, 1);
}
TEST(Brotli_testcases, testDeflateCopyReset)
{
    SetValue(2);
    test_DeflateCopyReset();
}
TEST(Brotli_testcases, testInflateInitEnd)
{
    SetValue(2);
    test_InflateInitEnd();
}
TEST(Brotli_testcases, testInflateSeg)
{
    SetValue(2);
    test_InflateSeg(&util_Brotli, 0);
}
TEST(Brotli_testcases, testInflateAll)
{
    SetValue(2);
    test_InflateAll(&util_Brotli);
}
TEST(Brotli_testcases, testInflateCopyReset)
{
    SetValue(2);
    test_InflateCopyReset();
}
