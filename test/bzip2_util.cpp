/******************************************************************************
 * bzip2_util.cpp
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
#include <gtest/gtest.h>

#include "test_util.h"
#include "unizip_adapt.h"
#include <bzlib.h>

void bzip2_Deflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    bz_stream strm;
    strm.bzalloc = NULL;
    strm.bzfree = NULL;
    strm.opaque = NULL;
    BZ2_bzCompressInit(&strm, 9, 0, 30);
    strm.next_in = next_in;
    strm.next_out = next_out;
    strm.avail_in = avail_in;
    strm.avail_out = avail_out;
    BZ2_bzCompress(&strm, BZ_RUN);
    BZ2_bzCompress(&strm, BZ_FINISH);
    next_out[((unsigned long)strm.total_out_hi32 << 32) + (unsigned long)strm.total_out_lo32] = '\0';
}
void bzip2_Inflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    bz_stream strm;
    strm.bzalloc = NULL;
    strm.bzfree = NULL;
    strm.opaque = NULL;
    BZ2_bzDecompressInit(&strm, 0, 0);
    strm.next_in = next_in;
    strm.next_out = next_out;
    strm.avail_in = avail_in;
    strm.avail_out = avail_out;
    BZ2_bzDecompress(&strm);
    next_out[((unsigned long)strm.total_out_hi32 << 32) + (unsigned long)strm.total_out_lo32] = '\0';
}

void bzip2_SetParamsDeflate_fun(unizip_streamp strm) { return; }

void bzip2_SetParamsInflate_fun(unizip_streamp strm) { return; }

const util_func util_bzip2 = {bzip2_Deflate_func, bzip2_Inflate_func, bzip2_SetParamsDeflate_fun,
                              bzip2_SetParamsInflate_fun};

TEST(Bzip2_testcases, testVersion)
{
    SetValue(1);
    test_version();
}
TEST(Bzip2_testcases, testDeflateInitEnd)
{
    SetValue(1);
    test_DeflateInitEnd();
}
TEST(Bzip2_testcases, testDeflateSeg)
{
    SetValue(1);
    test_DeflateSeg(&util_bzip2, 1);
}
TEST(Bzip2_testcases, testDeflateAll)
{
    SetValue(1);
    test_DeflateAll(&util_bzip2, 1);
}
TEST(Bzip2_testcases, testDeflateCopyReset)
{
    SetValue(1);
    test_DeflateCopyReset();
}
TEST(Bzip2_testcases, testInflateInitEnd)
{
    SetValue(1);
    test_InflateInitEnd();
}
TEST(Bzip2_testcases, testInflateSeg)
{
    SetValue(1);
    test_InflateSeg(&util_bzip2, 0);
}
TEST(Bzip2_testcases, testInflateAll)
{
    SetValue(1);
    test_InflateAll(&util_bzip2);
}

TEST(Bzip2_testcases, testInflateCopyReset)
{
    SetValue(1);
    test_InflateCopyReset();
}
