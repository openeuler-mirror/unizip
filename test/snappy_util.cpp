#include <gtest/gtest.h>

#include "unizip_adapt.h"
#include "test_util.h"
#include <snappy-c.h>


void Snappy_Deflate_func(char* next_in, char* next_out, uLong avail_in , uLong avail_out) {
    size_t avail_out_new = avail_out;
    snappy_compress((const char *)next_in,(size_t)avail_in,next_out,&avail_out_new);
}
void Snappy_Inflate_func(char* next_in, char* next_out, uLong avail_in , uLong avail_out) {
    size_t avail_out_new = avail_out;
    snappy_uncompress((const char *)next_in,(size_t)avail_in,next_out,&avail_out_new);
}

const util_func util_Snappy={Snappy_Deflate_func, Snappy_Inflate_func};

TEST(Snappy_testcases, testVersion)
{
    SetValue(5);
    test_version();
}
TEST(Snappy_testcases, testDeflateInitEnd)
{
    SetValue(5);
    test_DeflateInitEnd();
}

/*
TEST(Snappy_testcases, testDeflate1kb)
{
    SetValue(5);
    test_Deflate1kb();
}
TEST(Snappy_testcases, testDeflate10kb)
{
    SetValue(5);
    test_Deflate10kb();
}
TEST(Snappy_testcases, testDeflate100kb)
{
    SetValue(5);
    test_Deflate100kb();
}


TEST(Snappy_testcases, testDeflateLarge)
{
    SetValue(5);
    test_DeflateLarge(&util_Snappy);
}*/
TEST(Snappy_testcases, testDeflateCopyReset)
{
    SetValue(5);
    test_DeflateCopyReset();
}
TEST(Snappy_testcases, testInflateInitEnd)
{
    SetValue(5);
    test_InflateInitEnd();
}

/*
TEST(Snappy_testcases, testInflate1kb)
{
    SetValue(5);
    test_Inflate1kb();
}
TEST(Snappy_testcases, testInflate10kb)
{
    SetValue(5);
    test_Inflate10kb();
}
TEST(Snappy_testcases, testInflate100kb)
{
    SetValue(5);
    test_Inflate100kb();
}

*/
TEST(Snappy_testcases, testDeflateAll)
{
    SetValue(5);
    test_DeflateAll(&util_Snappy,39,nullptr);
}
TEST(Snappy_testcases, testInflateAll)
{
    SetValue(5);
    test_InflateAll(&util_Snappy,39,nullptr);
}
TEST(Snappy_testcases, testInflateCopyReset)
{
    SetValue(5);
    test_InflateCopyReset();
}
