/******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ********************************************************************************/
#include <gtest/gtest.h>

#include "test_util.h"
#include "unizip_adapt.h"

#include "lzma-sdk/C/7zAlloc.h"
#include "lzma-sdk/C/7zTypes.h"
#include "lzma-sdk/C/Alloc.h"
#include "lzma-sdk/C/LzmaDec.h"
#include "lzma-sdk/C/LzmaEnc.h"
#include "lzma-sdk/C/LzmaLib.h"
#include "lzma-sdk/C/Precomp.h"

void z7_Deflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    CLzmaEncProps props;
    size_t outPropsSize = LZMA_PROPS_SIZE;
    size_t destLen = avail_out;
    LzmaEncProps_Init(&props);

    LzmaEncode((Byte *)(next_out + LZMA_PROPS_SIZE), &destLen, (Byte *)next_in, avail_in, &props, (Byte *)next_out,
               &outPropsSize, 1, NULL, &g_Alloc, &g_Alloc);
}

void z7_Inflate_func(char *next_in, char *next_out, uLong avail_in, uLong avail_out)
{
    CLzmaDec dec;
    LzmaDec_Construct(&dec);

    LzmaDec_Allocate(&dec, (Byte *)next_in, LZMA_PROPS_SIZE, &g_Alloc);

    LzmaDec_Init(&dec);

    size_t destLen = avail_out;
    size_t srcLen = avail_in - LZMA_PROPS_SIZE;
    ELzmaStatus status;

    LzmaDec_DecodeToBuf(&dec, (Byte *)next_out, &destLen, (Byte *)(next_in + LZMA_PROPS_SIZE), &srcLen,
                        LZMA_FINISH_END, &status);

    LzmaDec_Free(&dec, &g_Alloc);
}

const util_func util_7zip = {z7_Deflate_func, z7_Inflate_func};

TEST(zip7_testcases, testVersion)
{
    SetValue(7);
    test_version();
}
TEST(zip7_testcases, testDeflateInitEnd)
{
    SetValue(7);
    test_DeflateInitEnd();
}
// TEST(zip7_testcases, testDeflateSeg) {
//   SetValue(7);
//   test_DeflateSeg(2, nullptr);
// }
TEST(zip7_testcases, testDeflateAll)
{
    SetValue(7);
    test_DeflateAll(&util_7zip, 200, nullptr);
}
TEST(zip7_testcases, testDeflateCopyReset)
{
    SetValue(7);
    test_DeflateCopyReset();
}
TEST(zip7_testcases, testInflateInitEnd)
{
    SetValue(7);
    test_InflateInitEnd();
}
TEST(zip7_testcases, testInflateSeg)
{
    SetValue(7);
    test_InflateSeg(&util_7zip, 2, nullptr);
}
TEST(zip7_testcases, testInflateAll)
{
    SetValue(7);
    test_InflateAll(&util_7zip, 200, nullptr);
}
TEST(zip7_testcases, testInflateCopyReset)
{
    SetValue(7);
    test_InflateCopyReset();
}
