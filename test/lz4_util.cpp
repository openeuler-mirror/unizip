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
#include <gtest/gtest.h>
#include "unizip_adapt.h"
#include "test_util.h"
#include <time.h>
#include <unistd.h>
#include <random>
#include <fstream>
#include <lz4.h>

void lz4_deflate_func(char *next_in, char *next_out, uLong avail_in,
                     uLong avail_out) {
    LZ4_stream_t* lz4_stream = LZ4_createStream();
    LZ4_compress_fast_continue(lz4_stream, next_in, next_out, avail_in, avail_out, 1);
    LZ4_freeStream(lz4_stream);
}


void lz4_inflate_func(char *next_in, char *next_out, uLong avail_in,
                     uLong avail_out) {
    LZ4_streamDecode_t* lz4_dstream = LZ4_createStreamDecode();
    LZ4_decompress_safe_continue(lz4_dstream, next_in, next_out, avail_in, avail_out);
    LZ4_freeStreamDecode(lz4_dstream);
}

const util_func util_Lz4 = {lz4_deflate_func, lz4_inflate_func};


TEST(Lz4_testcases, testVersion) {
    SetValue(4);
    test_version();
}

TEST(Lz4_testcases, testDeflateInitEnd) {
    SetValue(4);
    test_DeflateInitEnd();
}

// TEST(Lz4_testcases, testDeflateSeg) {
//     SetValue(4);
//     test_DeflateSeg(2, nullptr);
// }

TEST(Lz4_testcases, testDeflateAll) {
    SetValue(4);
    test_DeflateAll(&util_Lz4, 2, nullptr);
}

TEST(Lz4_testcases, testDeflateCopyReset) {
    SetValue(4);
    test_DeflateCopyReset();
}

TEST(Lz4_testcases, testInflateInitEnd) {
    SetValue(4);
    test_InflateInitEnd();
}

// // TEST(Lz4_testcases, testInflateSeg) {
// //     // 分段inflate设置
// //     SetValue(4);
// //     SetUpLz4();

// //     //测试数据
// //     int dataLen = 200;
// //     char *inputData = nullptr;
// //     test_InflateSegLZ4(dataLen, inputData);
// // }

TEST(Lz4_testcases, testInflateAll) {
    SetValue(4);
    test_InflateAll(&util_Lz4, 200, nullptr);
}

TEST(Lz4_testcases, testInflateCopyReset) {
    SetValue(4);
    test_InflateCopyReset();
}
