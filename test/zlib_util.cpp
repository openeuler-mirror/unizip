/******************************************************************************
 * zlib_util.cpp:zlib test function
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * shen-yi <1847103037@qq.com>
 *
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
#include "test_util.h"
#include <gtest/gtest.h>

extern void zlib_deflate(char *next_in, char *next_out, uLong avail_in, uLong avail_out);
extern void zlib_inflate(char *next_in, char *next_out, uLong avail_in, uLong avail_out);

void zlib_Deflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    zlib_deflate(next_in, next_out, avail_in, avail_out);
}
void zlib_Inflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    zlib_inflate(next_in, next_out, avail_in, avail_out);
}

void zlib_SetParamsDeflate_fun(unizip_streamp strm) { return; }

void zlib_SetParamsInflate_fun(unizip_streamp strm) { return; }

const util_func util_zlib = {zlib_Deflate_func, zlib_Inflate_func,zlib_SetParamsDeflate_fun,zlib_SetParamsInflate_fun};

TEST(Zlib_testcases, testVersion)
{
    SetValue(0);
    test_version();
}
TEST(Zlib_testcases, testDeflateInitEnd)
{
    SetValue(0);
    test_DeflateInitEnd();
}
TEST(Zlib_testcases, testDeflateSeg)
{
    SetValue(0);
    test_DeflateSeg(&util_zlib, 1);
}
TEST(Zlib_testcases, testDeflateAll)
{
    SetValue(0);
    test_DeflateAll(&util_zlib, 1);
}
TEST(Zlib_testcases, testDeflateCopyReset)
{
    SetValue(0);
    test_DeflateCopyReset();
}
TEST(Zlib_testcases, testInflateInitEnd)
{
    SetValue(0);
    test_InflateInitEnd();
}
TEST(Zlib_testcases, testInflateSeg)
{
    SetValue(0);
    test_InflateSeg(&util_zlib, 0);
}
TEST(Zlib_testcases, testInflateAll)
{
    SetValue(0);
    test_InflateAll(&util_zlib);
}
TEST(Zlib_testcases, testInflateCopyReset)
{
    SetValue(0);
    test_InflateCopyReset();
}
TEST(Zlib_testcases, testDeflateInit2)
{
    SetValue(0);
    test_DeflateInit2();
}
TEST(Zlib_testcases, testInflateInit2)
{
    SetValue(0);
    test_InflateInit2();
}
TEST(Zlib_testcases, testDeflateParams)
{
    SetValue(0);
    test_DeflateParams();
}
TEST(Zlib_testcases, testDeflateSetDictionary)
{
    SetValue(0);
    test_DeflateSetDictionary();
}
TEST(Zlib_testcases, testInflateSetDictionary)
{
    SetValue(0);
    test_InflateSetDictionary();
}
TEST(Zlib_testcases, testDeflateSetHeader)
{
    SetValue(0);
    test_DeflateSetHeader();
}
TEST(Zlib_testcases, testInflateGetHeader)
{
    SetValue(0);
    test_InflateGetHeader();
}
TEST(Zlib_testcases, testCompress2)
{
    SetValue(0);
    test_compress2();
}
TEST(Zlib_testcases, testSetValue)
{
    SetValue(-1);
    test_zError();
}
