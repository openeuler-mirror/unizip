/******************************************************************************
 * unizip_callback_lz4.c
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * ge-ziwen <1109674186@qq.com>
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
#include <fstream>
#include <gtest/gtest.h>
#include <lz4.h>
#include <random>
#include <time.h>
#include <unistd.h>

void lz4_deflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    LZ4_compress_default(next_in, next_out, avail_in, avail_out);
}

void lz4_inflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    LZ4_streamDecode_t *lz4_dstream = LZ4_createStreamDecode();
    LZ4_decompress_safe_continue(lz4_dstream, next_in, next_out, avail_in, avail_out);
    LZ4_freeStreamDecode(lz4_dstream);
}

void lz4_SetParamsDeflate_fun(unizip_streamp strm) { return; }

void lz4_SetParamsInflate_fun(unizip_streamp strm) { return; }

const util_func util_Lz4 = {lz4_deflate_func, lz4_inflate_func, lz4_SetParamsDeflate_fun, lz4_SetParamsInflate_fun};

TEST(Lz4_testcases, testVersion)
{
    SetValue(3);
    test_version();
}

TEST(Lz4_testcases, testDeflateInitEnd)
{
    SetValue(3);
    test_DeflateInitEnd();
}

// TEST(Lz4_testcases, testDeflateSeg) {
//     SetValue(3);
//     test_DeflateSeg(2, nullptr);
// }

TEST(Lz4_testcases, testDeflateAll)
{
    SetValue(3);
    test_DeflateAll(&util_Lz4, 1);
}

TEST(Lz4_testcases, testDeflateCopyReset)
{
    SetValue(3);
    test_DeflateCopyReset();
}

TEST(Lz4_testcases, testInflateInitEnd)
{
    SetValue(3);
    test_InflateInitEnd();
}

// // TEST(Lz4_testcases, testInflateSeg) {
// //     // 分段inflate设置
// //     SetValue(3);
// //     SetUpLz4();

// //     //测试数据
// //     int dataLen = 200;
// //     char *inputData = nullptr;
// //     test_InflateSegLZ4(dataLen, inputData);
// // }

TEST(Lz4_testcases, testInflateAll)
{
    SetValue(3);
    test_InflateAll(&util_Lz4);
}

TEST(Lz4_testcases, testInflateCopyReset)
{
    SetValue(3);
    test_InflateCopyReset();
}
