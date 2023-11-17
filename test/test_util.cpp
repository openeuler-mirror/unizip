/******************************************************************************
 * test_util.cpp:test function
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
#include "test_util.h"
#include <cstring>
#include <ctime>
#include <fstream>
#include <gtest/gtest.h>
#include <random>
#include <string>
#include <unistd.h>
#include <zlib.h>

std::string getParentDirectory(const std::string &path)
{
    size_t found = path.find_last_of("/\\");
    if (found != std::string::npos) {
        return path.substr(0, found);
    }
    return ""; // Return empty string if no parent directory found
}
unizip_stream c_stream;
unizip_stream d_stream;
uLong uncomprLen = 1048576;
uLong comprLen = 1048576;
Byte *compr = reinterpret_cast<Byte *>(calloc(static_cast<uInt>(comprLen), 1));
Byte *comprCmp =
    reinterpret_cast<Byte *>(calloc(static_cast<uInt>(comprLen), 1));
Byte *uncompr =
    reinterpret_cast<Byte *>(calloc(static_cast<uInt>(uncomprLen), 1));
Byte *uncomprCmp =
    reinterpret_cast<Byte *>(calloc(static_cast<uInt>(uncomprLen), 1));
const int bufferSize = 1048576; // 1MB，根据需要调整大小

void SetUp()
{
    unizip_deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
    unizip_inflateInit(&d_stream);
    uncomprLen = 1048576;
    comprLen = 1048576;
}

void TearDown()
{
    // 在每个测试用例之后清理资源
    // 可以在这里添加必要的清理代码
    memset(compr, 0, static_cast<uInt>(comprLen));
    memset(uncompr, 0, static_cast<uInt>(uncomprLen));
    /* unizip_deflateEnd(&c_stream);
    unizip_inflateEnd(&d_stream); */
}

void test_version(void)
{
    EXPECT_STRNE(unizip_Version(), NULL) << "wrong version";
}

void test_DeflateInitEnd(void)
{
    SetUp();
    static unizip_stream temp_stream;
    temp_stream.zalloc = (alloc_func)0;
    temp_stream.zfree = (free_func)0;
    temp_stream.opaque = (voidpf)0;
    int err = unizip_deflateInit(&temp_stream, Z_DEFAULT_COMPRESSION);
    EXPECT_EQ(err, Z_OK) << "deflateInit failed";
    err = unizip_deflateEnd(&temp_stream);
    EXPECT_EQ(err, Z_OK) << "deflateEnd failed";
    TearDown();
}

void test_DeflateSeg(int dataLen, char *inputData)
{
    SetUp();
    if (inputData == nullptr) {
        inputData = new char[dataLen * 1024]; // 为dataLen千字节分配内存
        // 生成随机数据
        std::default_random_engine generator(
            static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> distribution(0, 255);
        for (int i = 0; i < dataLen * 1024; i++) {
            inputData[i] = static_cast<char>(distribution(generator));
        }
    }
    c_stream.next_in = (unsigned char *)inputData;
    c_stream.next_out = compr;
    uLong len = dataLen * 1024;
    int err;
    // 压缩数据
    while (c_stream.total_in < len && c_stream.total_out < comprLen) {
        c_stream.avail_in = 1024;
        c_stream.avail_out = 1024;
        err = unizip_deflate(&c_stream, Z_NO_FLUSH);
        ASSERT_EQ(err, Z_OK) << "deflate failed";
    }
    for (;;) {
        c_stream.avail_out = 1;
        err = unizip_deflate(&c_stream, Z_FINISH);
        if (err == Z_STREAM_END)
            break;
        ASSERT_EQ(err, Z_OK) << "deflate failed";
    }
    comprLen = c_stream.total_out;
    unizip_deflateEnd(&c_stream);
    inputData[len] = '\0';
    strcpy((char *)uncompr, "garbage");
    err = unizip_uncompress(uncompr, &uncomprLen, compr, comprLen);
    uncompr[uncomprLen] = '\0';
    EXPECT_STREQ(reinterpret_cast<char *>(inputData),
                 reinterpret_cast<char *>(uncompr))
        << "Data mismatch";
    TearDown();
}

void test_DeflateAll(const util_func *uf, int dataLen, char *inputData)
{
    SetUp();
    if (inputData == nullptr) {
        inputData = new char[dataLen * 1024]; // 为dataLen千字节分配内存
        // 生成随机数据
        std::default_random_engine generator(
            static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> distribution(0, 255);
        for (int i = 0; i < dataLen * 1024; i++) {
            inputData[i] = static_cast<char>(distribution(generator));
        }
    }
    uLong len = dataLen * 1024;
    int err;
    c_stream.next_in = (unsigned char *)inputData;
    c_stream.next_out = compr;
    c_stream.avail_in = len;
    c_stream.avail_out = comprLen;
    unizip_deflate(&c_stream, Z_NO_FLUSH);
    unizip_deflate(&c_stream, Z_FINISH);
    ulong total_out = c_stream.total_out;
    compr[total_out] = '\0';
    unizip_deflateEnd(&c_stream);
    //对比原压缩库的压缩函数
    uf->Deflate_fun(inputData, (char *)comprCmp, len, comprLen);
    EXPECT_STREQ(reinterpret_cast<char *>(compr),
                 reinterpret_cast<char *>(comprCmp))
        << "VerifyData mismatch";
    free(comprCmp);
    strcpy((char *)uncompr, "garbage");
    comprLen = total_out;
    unizip_uncompress(uncompr, &uncomprLen, compr, comprLen);
    uncompr[uncomprLen] = '\0';
    EXPECT_STREQ(reinterpret_cast<char *>(inputData),
                 reinterpret_cast<char *>(uncompr))
        << "Data mismatch";
    TearDown();
}

void test_DeflateCopyReset()
{
    SetUp();
    unizip_stream wstream_copy;
    wstream_copy.zalloc = (alloc_func)0;
    wstream_copy.zfree = (free_func)0;
    wstream_copy.opaque = (voidpf)0;
    int err = unizip_deflateInit(&wstream_copy, Z_DEFAULT_COMPRESSION);
    EXPECT_EQ(err, Z_OK) << "deflateInit failed";
    err = unizip_deflateCopy(&wstream_copy, &c_stream);
    EXPECT_EQ(err, Z_OK) << "deflateCopy failed";
    err = unizip_deflateReset(&wstream_copy);
    EXPECT_EQ(err, Z_OK) << "deflateReset failed";
    TearDown();
}

void test_InflateInitEnd(void)
{
    SetUp();
    static unizip_stream temp_stream;
    temp_stream.zalloc = (alloc_func)0;
    temp_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    int err = unizip_inflateInit(&temp_stream);
    EXPECT_EQ(err, Z_OK) << "inflateInit failed";
    err = unizip_inflateEnd(&temp_stream);
    EXPECT_EQ(err, Z_OK) << "inflateEnd failed";
    TearDown();
}

void test_InflateSeg(int dataLen, char *inputData)
{
    SetUp();
    int err;
    if (inputData == nullptr) {
        inputData = new char[dataLen * 1024]; // 为dataLen千字节分配内存
        // 生成随机数据
        std::default_random_engine generator(
            static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> distribution(0, 255);
        for (int i = 0; i < dataLen * 1024; i++) {
            inputData[i] = static_cast<char>(distribution(generator));
        }
    }
    uLong len = dataLen * 1024;
    err = unizip_compress(compr, &comprLen,
                          reinterpret_cast<Bytef *>(inputData), len);
    EXPECT_EQ(err, Z_OK) << "compress failed";
    d_stream.next_in = compr;
    d_stream.next_out = uncompr;
    while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen) {
        d_stream.avail_in = 1;
        d_stream.avail_out = 1;
        err = unizip_inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END)
            break;
        ASSERT_EQ(err, Z_OK) << "inflate failed";
    }
    unizip_inflateEnd(&d_stream);
    inputData[len] = '\0';
    EXPECT_STREQ(reinterpret_cast<char *>(inputData),
                 reinterpret_cast<char *>(uncompr))
        << "Data mismatch";
    TearDown();
}

void test_InflateAll(const util_func *uf, int dataLen, char *inputData)
{
    SetUp();
    int err;
    if (inputData == nullptr) {
        inputData = new char[dataLen * 1024 + 1]; // 为dataLen千字节分配内存
        // 生成随机数据
        std::default_random_engine generator(
            static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> distribution(0, 255);
        for (int i = 0; i < dataLen * 1024; i++) {
            inputData[i] = static_cast<char>(distribution(generator));
            // inputData[i] = 'a';
        }
    }
    uLong len = dataLen * 1024;
    err = unizip_compress(compr, &comprLen,
                          reinterpret_cast<Bytef *>(inputData), len);
    EXPECT_EQ(err, Z_OK) << "compress failed";

    d_stream.next_in = compr;
    d_stream.next_out = uncompr;
    d_stream.avail_in = comprLen;
    d_stream.avail_out = uncomprLen;
    unizip_inflate(&d_stream, Z_NO_FLUSH);
    uncomprLen = d_stream.total_out;
    unizip_inflateEnd(&d_stream);
    uncompr[uncomprLen] = '\0';
    //对比原压缩库的解压函数
    uncomprCmp = (Byte *)calloc(static_cast<uInt>(uncomprLen), 1);
    uf->Inflate_fun((char *)compr, (char *)uncomprCmp, comprLen, uncomprLen);
    EXPECT_STREQ(reinterpret_cast<char *>(uncompr),
                 reinterpret_cast<char *>(uncomprCmp))
        << "VerifyData mismatch";
    free(uncomprCmp);

    inputData[len] = '\0';
    EXPECT_STREQ(reinterpret_cast<char *>(inputData),
                 reinterpret_cast<char *>(uncompr))
        << "Data mismatch";
    TearDown();
}

void test_InflateCopyReset(void)
{
    SetUp();
    unizip_stream wstream_copy;
    wstream_copy.zalloc = (alloc_func)0;
    wstream_copy.zfree = (free_func)0;
    wstream_copy.opaque = (voidpf)0;
    int err = unizip_inflateInit(&wstream_copy);
    EXPECT_EQ(err, Z_OK) << "deflateInit failed";
    err = unizip_inflateCopy(&wstream_copy, &d_stream);
    EXPECT_EQ(err, Z_OK) << "deflateCopy failed";
    err = unizip_inflateReset(&wstream_copy);
    EXPECT_EQ(err, Z_OK) << "deflateReset failed";
    TearDown();
}
