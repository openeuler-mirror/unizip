/******************************************************************************
 * zstd_util.cpp
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * shen-yi <1847401037@qq.com>
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
#include "unizip_adapt.h"
#include <gtest/gtest.h>
#include <stdlib.h> // free
#include <string.h> // memset, strcat, strlen
#include <zstd.h>   // presumes zstd library is installed

void zstd_Deflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    ZSTD_CStream *zcstream;
    zcstream = ZSTD_createCStream();
    ZSTD_initCStream(zcstream, 0);
    ZSTD_inBuffer in;
    ZSTD_outBuffer out;
    in.src = next_in;
    out.dst = next_out;
    in.size = avail_in;
    out.size = avail_out;
    in.pos = 0;
    out.pos = 0;
    ZSTD_compressStream(zcstream, &out, &in);
    ZSTD_endStream(zcstream, &out);
    next_out[out.pos] = '\0';
}
void zstd_Inflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    ZSTD_DStream *zdstream;
    zdstream = ZSTD_createDStream();
    ZSTD_inBuffer in;
    ZSTD_outBuffer out;
    in.src = next_in;
    out.dst = next_out;
    in.size = avail_in;
    out.size = avail_out;
    in.pos = 0;
    out.pos = 0;
    ZSTD_decompressStream(zdstream, &out, &in);
    next_out[out.pos] = '\0';
}

void zstd_SetParamsDeflate_fun(unizip_streamp strm) { return; }

void zstd_SetParamsInflate_fun(unizip_streamp strm) { return; }

const util_func util_zstd = {zstd_Deflate_func, zstd_Inflate_func, zstd_SetParamsDeflate_fun,
                             zstd_SetParamsInflate_fun};

TEST(Zstd_testcases, testVersion)
{
    SetValue(5);
    test_version();
}
TEST(Zstd_testcases, testDeflateInitEnd)
{
    SetValue(5);
    test_DeflateInitEnd();
}
TEST(Zstd_testcases, testDeflateSeg)
{
    SetValue(5);
    test_DeflateSeg(&util_zstd, 1);
}
TEST(Zstd_testcases, testDeflateAll)
{
    SetValue(5);
    test_DeflateAll(&util_zstd, 1);
}
TEST(Zstd_testcases, testDeflateCopyReset)
{
    SetValue(5);
    test_DeflateCopyReset();
}
TEST(Zstd_testcases, testInflateInitEnd)
{
    SetValue(5);
    test_InflateInitEnd();
}
TEST(Zstd_testcases, testInflateSeg)
{
    SetValue(5);
    test_InflateSeg(&util_zstd, 0);
}
TEST(Zstd_testcases, testInflateAll)
{
    SetValue(5);
    test_InflateAll(&util_zstd);
}
TEST(Zstd_testcases, testInflateCopyReset)
{
    SetValue(5);
    test_InflateCopyReset();
}
