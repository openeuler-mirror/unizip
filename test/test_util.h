/******************************************************************************
 * test_util.h:test function
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

#ifndef TEST_TEST_UTIL_H_
#define TEST_TEST_UTIL_H_
#include "unizip_adapt.h"
#include <gtest/gtest.h>
#include <zlib.h>
#define MEMEQ(length, a, b) (!memcmp((a), (b), (length)))

// 在gtest中，将原有的FAIL修改为abort();
#define ASSERT(x)                                                              \
    do {                                                                       \
        if (!(x)) {                                                            \
            fprintf(stderr, "Assert failed: %s:%d: %s\n", __FILE__, __LINE__,  \
                    #x);                                                       \
            abort();                                                           \
        }                                                                      \
    } while (0)

typedef void Deflate_func(char *next_in, char *next_out, uLong avail_in,
                          uLong avail_out);
typedef void Inflate_func(char *next_in, char *next_out, uLong avail_in,
                          uLong avail_out);
struct util_func {
    Deflate_func *Deflate_fun;
    Inflate_func *Inflate_fun;
};
void uadk_changeLib(int compression_flag);
void test_version(void);
void test_DeflateInitEnd(void);
void test_DeflateSeg(int dataLen, char *inputData);
void test_DeflateAll(const util_func *uf, int dataLen, char *inputData);
void test_DeflateCopyReset(void);
void test_InflateInitEnd(void);
void test_InflateSeg(int dataLen, char *inputData);
void test_InflateAll(const util_func *uf, int dataLen, char *inputData);
void test_InflateCopyReset(void);

#endif // TEST_TEST_UTIL_H_
