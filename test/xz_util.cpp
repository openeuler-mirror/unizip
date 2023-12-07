/******************************************************************************
 * unizip_adapt.c
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
#include <lzma.h>

void xz_Deflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    lzma_stream strm = LZMA_STREAM_INIT;
    lzma_options_lzma opt_lzma2;
    lzma_lzma_preset(&opt_lzma2, LZMA_PRESET_DEFAULT);

    lzma_filter filters[] = {
        {.id = LZMA_FILTER_X86, .options = NULL},
        {.id = LZMA_FILTER_LZMA2, .options = &opt_lzma2},
        {.id = LZMA_VLI_UNKNOWN, .options = NULL},
    };
    lzma_stream_encoder(&strm, filters, LZMA_CHECK_CRC64);
    strm.next_in = (uint8_t *)next_in;
    strm.next_out = (uint8_t *)next_out;
    strm.avail_in = avail_in;
    strm.avail_out = avail_out;
    lzma_code(&strm, LZMA_FINISH);
    next_out[strm.total_out] = '\0';
}
void xz_Inflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    lzma_stream strm = LZMA_STREAM_INIT;
    lzma_stream_decoder(&strm, UINT64_MAX, LZMA_CONCATENATED);
    strm.next_in = (uint8_t *)next_in;
    strm.next_out = (uint8_t *)next_out;
    strm.avail_in = avail_in;
    strm.avail_out = avail_out;
    lzma_code(&strm, LZMA_RUN);
    next_out[strm.total_out] = '\0';
}

void xz_SetParamsDeflate_fun(unizip_streamp strm) { return; }

void xz_SetParamsInflate_fun(unizip_streamp strm) { return; }

const util_func util_xz = {xz_Deflate_func, xz_Inflate_func, xz_SetParamsDeflate_fun, xz_SetParamsInflate_fun};

TEST(Xz_testcases, testVersion)
{
    SetValue(8);
    test_version();
}
TEST(Xz_testcases, testDeflateInitEnd)
{
    SetValue(8);
    test_DeflateInitEnd();
}
TEST(Xz_testcases, testDeflateSeg)
{
    SetValue(8);
    test_DeflateSeg(&util_xz, 1);
}
TEST(Xz_testcases, testDeflateAll)
{
    SetValue(8);
    test_DeflateAll(&util_xz, 1);
}
TEST(Xz_testcases, testDeflateCopyReset)
{
    SetValue(8);
    test_DeflateCopyReset();
}
TEST(Xz_testcases, testInflateInitEnd)
{
    SetValue(8);
    test_InflateInitEnd();
}
TEST(Xz_testcases, testInflateSeg)
{
    SetValue(8);
    test_InflateSeg(&util_xz, 1);
}
TEST(Xz_testcases, testInflateAll)
{
    SetValue(8);
    test_InflateAll(&util_xz);
}
TEST(Xz_testcases, testInflateCopyReset)
{
    SetValue(8);
    test_InflateCopyReset();
}
