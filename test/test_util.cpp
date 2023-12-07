/******************************************************************************
 * test_util.cpp:test function
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 *
 * Authors:
 * chen-yufan <1109674186@qq.com>
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
#include <cstring>
#include <ctime>
#include <fstream>
#include <gtest/gtest.h>
#include <random>
#include <string>
#include <unistd.h>

unizip_stream c_stream;
unizip_stream d_stream;
uLong uncomprLen = 1048576;
uLong comprLen = 1048576;
Byte *compr = reinterpret_cast<Byte *>(calloc(static_cast<uInt>(comprLen), 1));
Byte *comprCmp = reinterpret_cast<Byte *>(calloc(static_cast<uInt>(comprLen), 1));
Byte *uncompr = reinterpret_cast<Byte *>(calloc(static_cast<uInt>(uncomprLen), 1));
Byte *uncomprCmp = reinterpret_cast<Byte *>(calloc(static_cast<uInt>(uncomprLen), 1));
const int bufferSize = 1048576; // 1MB，根据需要调整大小

void SetUp(int level)
{
    unizip_deflateInit(&c_stream, level);
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
    memset(comprCmp, 0, static_cast<uInt>(comprLen));
    memset(uncomprCmp, 0, static_cast<uInt>(uncomprLen));
    /* unizip_deflateEnd(&c_stream);
    unizip_inflateEnd(&d_stream); */
}

void uadk_changeLib(int compression_flag)
{
    c_stream.compression_flag = compression_flag;
    d_stream.compression_flag = compression_flag;
}

void test_version(void) { EXPECT_STRNE(unizip_Version(), NULL) << "wrong version"; }

void test_DeflateInitEnd(void)
{
    SetUp(Z_DEFAULT_COMPRESSION);
    static unizip_stream temp_stream;
    temp_stream.zalloc = (alloc_func)0;
    temp_stream.zfree = (free_func)0;
    temp_stream.opaque = (voidpf)0;
    int err = unizip_deflateInit(&temp_stream, Z_DEFAULT_COMPRESSION);
    EXPECT_EQ(err, Z_OK) << "deflateInit failed";
    unizip_zError(err);
    err = unizip_deflateEnd(&temp_stream);
    EXPECT_EQ(err, Z_OK) << "deflateEnd failed";
    TearDown();
}

void test_DeflateSeg(const util_func *uf, int noSkipVerify)
{
    for (int dataLen = 1; dataLen <= 100; dataLen *= 10) {
        SetUp(Z_DEFAULT_COMPRESSION);
        uf->SetParamsDeflate_fun(&c_stream);
        char *inputData = new char[dataLen * 1024]; // 为dataLen千字节分配内存
        // 生成随机数据
        std::default_random_engine generator(static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> distribution('a', 'z');
        for (int i = 0; i < dataLen * 1024; i++) {
            inputData[i] = static_cast<char>(distribution(generator));
        }
        uLong len = dataLen * 1024;
        inputData[len] = '\0';
        c_stream.next_in = (unsigned char *)inputData;
        c_stream.next_out = compr;
        uLong boundLen = unizip_compressBound(len);
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
        ulong totalOut = c_stream.total_out;
        ASSERT_GE(boundLen, 0) << "compressBound wrong";

        if (noSkipVerify) {
            uf->Deflate_fun(inputData, (char *)comprCmp, len, comprLen);
            EXPECT_STREQ(reinterpret_cast<char *>(compr), reinterpret_cast<char *>(comprCmp)) << "VerifyData mismatch";
        }

        unizip_deflateEnd(&c_stream);
        strcpy((char *)uncompr, "garbage");
        err = unizip_uncompress(uncompr, &uncomprLen, compr, totalOut);
        uncompr[uncomprLen] = '\0';
        EXPECT_STREQ(reinterpret_cast<char *>(inputData), reinterpret_cast<char *>(uncompr)) << "Data mismatch";
        TearDown();
    }
}

void test_DeflateAll(const util_func *uf, int noSkipVerify)
{
    for (int dataLen = 1; dataLen <= 100; dataLen *= 10) {
        SetUp(Z_DEFAULT_COMPRESSION);
        uf->SetParamsDeflate_fun(&c_stream);
        char *inputData = new char[dataLen * 1024]; // 为dataLen千字节分配内存
        // 生成随机数据
        std::default_random_engine generator(static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> distribution('a', 'z');
        for (int i = 0; i < dataLen * 1024; i++) {
            inputData[i] = static_cast<char>(distribution(generator));
        }
        uLong len = dataLen * 1024;
        inputData[len] = '\0';
        uLong boundLen = unizip_compressBound(len);
        c_stream.next_in = (unsigned char *)inputData;
        c_stream.next_out = compr;
        c_stream.avail_in = len;
        c_stream.avail_out = comprLen;
        unizip_deflate(&c_stream, Z_NO_FLUSH);
        unizip_deflate(&c_stream, Z_FINISH);
        ulong total_out = c_stream.total_out;
        ASSERT_GE(boundLen, 0) << "compressBound wrong";
        compr[total_out] = '\0';
        unizip_deflateEnd(&c_stream);
        // 对比原压缩库的压缩函数
        if(noSkipVerify){
            uf->Deflate_fun(inputData, (char *)comprCmp, len, comprLen);
            EXPECT_STREQ(reinterpret_cast<char *>(compr), reinterpret_cast<char *>(comprCmp)) << "VerifyData mismatch";
        }
        // free(comprCmp);
        strcpy((char *)uncompr, "garbage");
        comprLen = total_out;
        unizip_uncompress(uncompr, &uncomprLen, compr, comprLen);
        uncompr[uncomprLen] = '\0';
        EXPECT_STREQ(reinterpret_cast<char *>(inputData), reinterpret_cast<char *>(uncompr)) << "Data mismatch";
        TearDown();
    }
}

void test_DeflateCopyReset()
{
    SetUp(Z_DEFAULT_COMPRESSION);
    unizip_stream wstream_copy;
    wstream_copy.zalloc = (alloc_func)0;
    wstream_copy.zfree = (free_func)0;
    wstream_copy.opaque = (voidpf)0;
    wstream_copy.compression_flag = 0;
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
    SetUp(Z_DEFAULT_COMPRESSION);
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

void test_InflateSeg(const util_func *uf, int oneMoreByte)
{
    for (int dataLen = 1; dataLen <= 100; dataLen *= 10) {
        SetUp(Z_DEFAULT_COMPRESSION);
        uf->SetParamsInflate_fun(&d_stream);
        int err;
        char *inputData = new char[dataLen * 1024]; // 为dataLen千字节分配内存
        // 生成随机数据
        std::default_random_engine generator(static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> distribution('a', 'z');
        for (int i = 0; i < dataLen * 1024; i++) {
            inputData[i] = static_cast<char>(distribution(generator));
        }

        uLong len = dataLen * 1024;
        err = unizip_compress(compr, &comprLen, reinterpret_cast<Bytef *>(inputData), len);
        EXPECT_EQ(err, Z_OK) << "compress failed";
        d_stream.next_in = compr;
        d_stream.next_out = uncompr;
        while (d_stream.total_out < uncomprLen && d_stream.total_in <= comprLen) {
            d_stream.avail_in = 1024;
            if (comprLen - d_stream.total_in < 1024) { //剩余长度
                d_stream.avail_in = comprLen - d_stream.total_in;
            }
            if (oneMoreByte) {
                ++d_stream.avail_in;
            }
            d_stream.avail_out = 1024;
            err = unizip_inflate(&d_stream, Z_NO_FLUSH);
            if (err == Z_STREAM_END)
                break;
            ASSERT_EQ(err, Z_OK) << "inflate failed";
        }
        unizip_inflateEnd(&d_stream);
        inputData[len] = '\0';
        uf->Inflate_fun((char *)compr, (char *)uncomprCmp, comprLen, uncomprLen);
        EXPECT_STREQ(reinterpret_cast<char *>(uncompr), reinterpret_cast<char *>(uncomprCmp)) << "VerifyData mismatch";

        EXPECT_STREQ(reinterpret_cast<char *>(inputData), reinterpret_cast<char *>(uncompr)) << "Data mismatch";
        TearDown();
    }
}

void test_InflateAll(const util_func *uf)
{
    for (int dataLen = 1; dataLen <= 100; dataLen *= 10) {
        SetUp(Z_DEFAULT_COMPRESSION);
        uf->SetParamsInflate_fun(&d_stream);
        int err;
        char *inputData = new char[dataLen * 1024 + 1]; // 为dataLen千字节分配内存
        // 生成随机数据
        std::default_random_engine generator(static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> distribution('a', 'z');
        for (int i = 0; i < dataLen * 1024; i++) {
            inputData[i] = static_cast<char>(distribution(generator));
            // inputData[i] = 'a';
        }

        uLong len = dataLen * 1024;
        err = unizip_compress(compr, &comprLen, reinterpret_cast<Bytef *>(inputData), len);
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
        uf->Inflate_fun((char *)compr, (char *)uncomprCmp, comprLen, uncomprLen);
        EXPECT_STREQ(reinterpret_cast<char *>(uncompr), reinterpret_cast<char *>(uncomprCmp)) << "VerifyData mismatch";

        inputData[len] = '\0';
        EXPECT_STREQ(reinterpret_cast<char *>(inputData), reinterpret_cast<char *>(uncompr)) << "Data mismatch";
        TearDown();
    }
}

void test_InflateCopyReset(void)
{
    SetUp(Z_DEFAULT_COMPRESSION);
    unizip_stream wstream_copy;
    wstream_copy.zalloc = (alloc_func)0;
    wstream_copy.zfree = (free_func)0;
    wstream_copy.opaque = (voidpf)0;
    wstream_copy.compression_flag = 0;
    int err = unizip_inflateInit(&wstream_copy);
    EXPECT_EQ(err, Z_OK) << "deflateInit failed";
    err = unizip_inflateCopy(&wstream_copy, &d_stream);
    EXPECT_EQ(err, Z_OK) << "deflateCopy failed";
    err = unizip_inflateReset(&wstream_copy);
    EXPECT_EQ(err, Z_OK) << "deflateReset failed";
    TearDown();
}

void test_DeflateInit2(void)
{
    unizip_stream temp_stream;
    temp_stream.zalloc = (alloc_func)0;
    temp_stream.zfree = (free_func)0;
    temp_stream.opaque = (voidpf)0;
    int err = unizip_deflateInit2(&temp_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
    EXPECT_EQ(err, Z_OK) << "deflateInit2 failed";
    err = unizip_deflatePending(&temp_stream, Z_NULL, Z_NULL);
    EXPECT_EQ(err, Z_OK) << "deflatePending failed";
    err = unizip_deflateEnd(&temp_stream);
    EXPECT_EQ(err, Z_OK) << "deflateEnd failed";
    TearDown();
}

void test_InflateInit2(void)
{
    unizip_stream temp_stream;
    temp_stream.zalloc = (alloc_func)0;
    temp_stream.zfree = (free_func)0;
    temp_stream.opaque = (voidpf)0;
    int err = unizip_inflateInit2(&temp_stream, 15);
    EXPECT_EQ(err, Z_OK) << "inflateInit2 failed";
    err = unizip_inflateEnd(&temp_stream);
    EXPECT_EQ(err, Z_OK) << "inflateEnd failed";
    TearDown();
}

void test_DeflateParams(void)
{
    unizip_stream temp_stream; /* compression stream */
    int err;
    comprLen = 10240;
    uncomprLen = 1024;
    for (int i = 0; i < (int)uncomprLen; ++i) {
        uncompr[i] = 'a';
    }
    temp_stream.zalloc = (alloc_func)0;
    temp_stream.zfree = (free_func)0;
    temp_stream.opaque = (voidpf)0;

    err = unizip_deflateInit(&temp_stream, Z_BEST_SPEED);
    EXPECT_EQ(err, Z_OK) << "unizip_deflateInit failed";
    temp_stream.next_out = compr;
    temp_stream.avail_out = (uInt)comprLen;

    temp_stream.next_in = uncompr;
    temp_stream.avail_in = (uInt)uncomprLen;
    err = unizip_deflate(&temp_stream, Z_NO_FLUSH);
    EXPECT_EQ(err, Z_OK) << "unizip_deflate1 failed";

    /* Feed in already compressed data and switch to no compression: */
    unizip_deflateParams(&temp_stream, Z_NO_COMPRESSION, Z_DEFAULT_STRATEGY);
    temp_stream.next_in = compr;
    temp_stream.avail_in = (uInt)uncomprLen / 2;
    err = unizip_deflate(&temp_stream, Z_NO_FLUSH);
    EXPECT_EQ(err, Z_OK) << "unizip_deflate2 failed";

    /* Switch back to compressing mode: */
    unizip_deflateParams(&temp_stream, Z_BEST_COMPRESSION, Z_FILTERED);
    temp_stream.next_in = uncompr;
    temp_stream.avail_in = (uInt)uncomprLen;
    err = unizip_deflate(&temp_stream, Z_NO_FLUSH);
    EXPECT_EQ(err, Z_OK) << "unizip_deflate3 failed";

    err = unizip_deflate(&temp_stream, Z_FINISH);
    EXPECT_EQ(err, Z_STREAM_END) << "unizip_deflate4 failed";

    err = unizip_deflateEnd(&temp_stream);
    EXPECT_EQ(err, Z_OK) << "unizip_deflateEnd failed";
    TearDown();
}

void test_DeflateSetDictionary(void)
{
    unizip_stream temp_stream; /* compression stream */
    int err;

    temp_stream.zalloc = (alloc_func)0;
    temp_stream.zfree = (free_func)0;
    temp_stream.opaque = (voidpf)0;
    static z_const char hello[] = "hello, hello!";
    static const char dictionary[] = "hello";

    err = unizip_deflateInit(&temp_stream, Z_BEST_COMPRESSION);
    EXPECT_EQ(err, Z_OK) << "unizip_deflateInit failed";

    err = unizip_deflateSetDictionary(&temp_stream, (const Bytef *)dictionary, (int)sizeof(dictionary));
    EXPECT_EQ(err, Z_OK) << "deflateSetDictionary failed";

    temp_stream.next_out = compr;
    temp_stream.avail_out = (uInt)comprLen;

    temp_stream.next_in = (z_const unsigned char *)hello;
    temp_stream.avail_in = (uInt)strlen(hello) + 1;

    err = unizip_deflate(&temp_stream, Z_FINISH);
    EXPECT_EQ(err, Z_STREAM_END) << "unizip_deflate4 failed";
    err = unizip_deflateEnd(&temp_stream);
    EXPECT_EQ(err, Z_OK) << "unizip_deflateEnd failed";
}

void test_InflateSetDictionary(void)
{
    int err;
    unizip_stream temp_stream; /* decompression stream */

    strcpy((char *)uncompr, "garbage");

    temp_stream.zalloc = (alloc_func)0;
    temp_stream.zfree = (free_func)0;
    temp_stream.opaque = (voidpf)0;
    static z_const char hello[] = "hello, hello!";
    static const char dictionary[] = "hello";

    temp_stream.next_in = compr;
    temp_stream.avail_in = (uInt)comprLen;

    err = unizip_inflateInit(&temp_stream);
    EXPECT_EQ(err, Z_OK) << "inflateInit failed";

    temp_stream.next_out = uncompr;
    temp_stream.avail_out = (uInt)uncomprLen;
    for (;;) {
        err = unizip_inflate(&temp_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END)
            break;
        if (err == Z_NEED_DICT) {
            err = unizip_inflateSetDictionary(&temp_stream, (const Bytef *)dictionary, (int)sizeof(dictionary));
        }
        EXPECT_EQ(err, Z_OK) << "unizip_inflateSetDictionary failed";
    }

    err = inflateEnd(&temp_stream);
    EXPECT_EQ(err, Z_OK) << "inflateEnd failed";

    if (strcmp((char *)uncompr, hello)) {
        fprintf(stderr, "bad inflate with dict\n");
        exit(1);
    }
    TearDown();
}

void test_DeflateSetHeader(void)
{
    unizip_stream temp_stream;
    temp_stream.zalloc = (alloc_func)0;
    temp_stream.zfree = (free_func)0;
    temp_stream.opaque = (voidpf)0;
    int err = unizip_deflateInit2(&temp_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY);
    EXPECT_EQ(err, Z_OK) << "deflateInit2 failed";
    gz_header header;
    header.text = true;
    header.time = time(NULL);
    header.os = 3;
    err = unizip_deflateSetHeader(&temp_stream, &header);
    EXPECT_EQ(err, Z_OK) << "deflateSetHeader failed";
    unizip_deflateEnd(&temp_stream);
    TearDown();
}

void test_InflateGetHeader(void)
{
    unizip_stream temp_stream;
    temp_stream.zalloc = (alloc_func)0;
    temp_stream.zfree = (free_func)0;
    temp_stream.opaque = (voidpf)0;
    int err = unizip_inflateInit2(&temp_stream, 32);
    EXPECT_EQ(err, Z_OK) << "deflateInit2 failed";
    gz_header header;
    err = unizip_inflateGetHeader(&temp_stream, &header);
    EXPECT_EQ(err, Z_OK) << "inflateGetHeader failed";
    TearDown();
}

void test_compress2(void)
{
    SetUp(Z_DEFAULT_COMPRESSION);
    int err;
    char *inputData = new char[1024]; // 为dataLen千字节分配内存
    // 生成随机数据
    std::default_random_engine generator(static_cast<unsigned>(time(nullptr)));
    std::uniform_int_distribution<int> distribution('a', 'z');
    for (int i = 0; i < 1024; i++) {
        inputData[i] = static_cast<char>(distribution(generator));
    }
    uLong len = 1024;
    err = unizip_compress2(compr, &comprLen, reinterpret_cast<Bytef *>(inputData), len, Z_DEFAULT_COMPRESSION);
    EXPECT_EQ(err, Z_OK) << "compress failed";
    err = unizip_uncompress2(uncompr, &uncomprLen, compr, &comprLen);
    EXPECT_EQ(err, Z_OK) << "uncompress failed";
    EXPECT_STREQ(reinterpret_cast<char *>(inputData), reinterpret_cast<char *>(uncompr)) << "Data mismatch";
}

void test_zError(void)
{
    unizip_Version();
    unizip_zError(0);
}