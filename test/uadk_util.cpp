/******************************************************************************
 * uadk_util.cpp
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * jiang-chenyang <jiangcy1129@163.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 ********************************************************************************/
#ifdef __aarch64__

#include "test_util.h"
#include "unizip_adapt.h"
#include "unizip_algorithm_uadk.h"
#include <gtest/gtest.h>

int ALG_TYPE = WCRYPTO_GZIP; // 与unizip_callback_uadk.c中的ALG_TYPE一致
void uadk_Deflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    struct zip_stream zstream;
    if (ALG_TYPE == WCRYPTO_ZLIB) {
        hw_deflateInit(&zstream, Z_DEFAULT_COMPRESSION);
    } else {
        int windowBits = 15;
        int GZIP_ENCODING = 16;
        hw_deflateInit2(&zstream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits | GZIP_ENCODING, DEF_MEM_LEVEL,
                        Z_DEFAULT_STRATEGY);
    }
    char* dst = next_out;
    memcpy(zstream.next_in, (unsigned char *)next_in, avail_in);
    zstream.avail_in = avail_in;
    zstream.avail_out = avail_out;
    hw_deflate(&zstream, WCRYPTO_NO_FLUSH);
    ulong total_out = zstream.total_out;
    memcpy(dst, zstream.next_out, total_out);
    dst += total_out;
    
    next_in = zstream.next_in;
    avail_in = zstream.avail_in;
    avail_out = zstream.avail_out;
    memcpy(zstream.next_in, (unsigned char *)next_in, avail_in);
    hw_deflate(&zstream, WCRYPTO_FINISH);
    memcpy(dst, zstream.next_out, zstream.total_out - total_out); // 仅拷贝这次输出的内容
    total_out = zstream.total_out;
    next_out[total_out] = '\0';
    hw_deflateEnd(&zstream);
}

void uadk_Inflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    struct zip_stream zstream;
    if (ALG_TYPE == WCRYPTO_ZLIB)
        hw_inflateInit(&zstream);
    else
        hw_inflateInit2(&zstream, 16 + MAX_WBITS);
    memcpy(zstream.next_in, (unsigned char *)next_in, avail_in);
    zstream.avail_in = avail_in;
    zstream.avail_out = 1048576; // 指定一个足够大的值确保一次性解压
    hw_inflate(&zstream, WCRYPTO_NO_FLUSH);
    ulong total_out = zstream.total_out;
    memcpy(next_out, zstream.next_out, total_out);
    next_out[total_out] = '\0';
    hw_inflateEnd(&zstream);
}

void uadk_SetParamsDeflate_fun(unizip_streamp strm) { return; }

void uadk_SetParamsInflate_fun(unizip_streamp strm) { return; }

const util_func util_uadk = {uadk_Deflate_func, uadk_Inflate_func, uadk_SetParamsDeflate_fun,
                             uadk_SetParamsInflate_fun};

TEST(Uadk_testcases, testVersion)
{
    if (UadkEnabled() == false) {
        return;
    }
    uadk_changeLib(WCRYPTO_GZIP); // 指定默认测试的类型为GZIP
    SetValue(7);
    test_version();
}
TEST(Uadk_testcases, testDeflateInitEnd)
{
    if (UadkEnabled() == false) {
        return;
    }
    SetValue(7);
    test_DeflateInitEnd();
}
TEST(Uadk_testcases, testDeflateSeg)
{
    if (UadkEnabled() == false) {
        return;
    }
    SetValue(7);
    test_DeflateSeg(&util_uadk, 0);
    uadk_changeLib(WCRYPTO_ZLIB); // 额外测试ZLIB类型
    ALG_TYPE = WCRYPTO_ZLIB;
    test_DeflateSeg(&util_uadk, 0);
    ALG_TYPE = WCRYPTO_GZIP; // 测试后切换回GZIP类型
    uadk_changeLib(WCRYPTO_GZIP);
}
TEST(Uadk_testcases, testDeflateAll)
{
    if (UadkEnabled() == false) {
        return;
    }
    SetValue(7);
    test_DeflateAll(&util_uadk, 0);
}
TEST(Uadk_testcases, testDeflateCopyReset)
{
    if (UadkEnabled() == false) {
        return;
    }
    SetValue(7);
    test_DeflateCopyReset();
    uadk_changeLib(WCRYPTO_ZLIB); // 额外测试ZLIB类型
    test_DeflateCopyReset();
    uadk_changeLib(WCRYPTO_GZIP); // 测试后切换回GZIP类型
}
TEST(Uadk_testcases, testInflateInitEnd)
{
    if (UadkEnabled() == false) {
        return;
    }
    SetValue(7);
    test_InflateInitEnd();
}
TEST(Uadk_testcases, testInflateSeg)
{
    if (UadkEnabled() == false) {
        return;
    }
    SetValue(7);
    test_InflateSeg(&util_uadk, 0);
}
TEST(Uadk_testcases, testInflateAll)
{
    if (UadkEnabled() == false) {
        return;
    }
    SetValue(7);
    test_InflateAll(&util_uadk);
}
TEST(Uadk_testcases, testInflateCopyReset)
{
    if (UadkEnabled() == false) {
        return;
    }
    SetValue(7);
    test_InflateCopyReset();
    uadk_changeLib(WCRYPTO_ZLIB); // 额外测试ZLIB类型
    test_InflateCopyReset();
    uadk_changeLib(WCRYPTO_GZIP);
}

#endif // __aarch64__