/******************************************************************************
 * gzip_util.cpp:gzip test function
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

void gzip_Deflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    zlib_deflate(next_in, next_out, avail_in, avail_out);
}
void gzip_Inflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    zlib_inflate(next_in, next_out, avail_in, avail_out);
}

void gzip_SetParamsDeflate_fun(unizip_streamp strm) { return; }

void gzip_SetParamsInflate_fun(unizip_streamp strm) { return; }

const util_func util_gzip = {gzip_Deflate_func, gzip_Inflate_func, gzip_SetParamsDeflate_fun,
                             gzip_SetParamsInflate_fun};

TEST(Gzip_testcases, testVersion)
{
    SetValue(9);
    test_version();
}
TEST(Gzip_testcases, testDeflateInitEnd)
{
    SetValue(9);
    test_DeflateInitEnd();
}
TEST(Gzip_testcases, testDeflateSeg)
{
    SetValue(9);
    test_DeflateSeg(&util_gzip, 1);
}
TEST(Gzip_testcases, testDeflateAll)
{
    SetValue(9);
    test_DeflateAll(&util_gzip, 1);
}
TEST(Gzip_testcases, testDeflateCopyReset)
{
    SetValue(9);
    test_DeflateCopyReset();
}
TEST(Gzip_testcases, testInflateInitEnd)
{
    SetValue(9);
    test_InflateInitEnd();
}
TEST(Gzip_testcases, testInflateSeg)
{
    SetValue(9);
    test_InflateSeg(&util_gzip, 0);
}
TEST(Gzip_testcases, testInflateAll)
{
    SetValue(9);
    test_InflateAll(&util_gzip);
}
TEST(Gzip_testcases, testInflateCopyReset)
{
    SetValue(9);
    test_InflateCopyReset();
}
TEST(Gzip_testcases, testDeflateSetHeader)
{
    SetValue(9);
    test_DeflateSetHeader();
}
TEST(Zlib_testcases, testCompress2)
{
    SetValue(9);
    test_compress2();
}
TEST(Gzip_testcases, testInflateGetHeader)
{
    SetValue(9);
    test_InflateGetHeader();
}