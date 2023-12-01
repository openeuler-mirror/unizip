/******************************************************************************
 * zlib_util.cpp:zlib test function
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * chen-yufan <1109674186@qq.com>
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
#include <gtest/gtest.h>

#include "test_util.h"
#include "unizip_adapt.h"

TEST(Zlib_testcases, testVersion) {
    SetValue(1);
    test_version();
}
TEST(Zlib_testcases, testDeflateInitEnd) {
    SetValue(1);
    test_DeflateInitEnd();
}
// TEST(Zlib_testcases, testDeflateSeg) {
//     SetValue(1);
//     test_DeflateSeg(1, nullptr);
// }
TEST(Zlib_testcases, testDeflateCopyReset) {
    SetValue(1);
    test_DeflateCopyReset();
}
TEST(Zlib_testcases, testInflateInitEnd) {
    SetValue(1);
    test_InflateInitEnd();
}
// TEST(Zlib_testcases, testInflateSeg) {
//     SetValue(1);
//     test_InflateSeg(2, nullptr);
// }
TEST(Zlib_testcases, testInflateCopyReset) {
    SetValue(1);
    test_InflateCopyReset();
}
