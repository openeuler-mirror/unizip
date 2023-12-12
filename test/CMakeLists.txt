
project(unizip)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")
enable_testing()
find_package(GTest REQUIRED)
find_package(ZLIB REQUIRED)
find_package(LibLZMA REQUIRED)
find_package(BZip2 REQUIRED)
find_package(Zstd REQUIRED)
find_package(Snappy REQUIRED)
find_package(Brotli REQUIRED)
find_package(LZ4 REQUIRED)
# uadk
# find_library(LIBWD_LIBRARY wd
# 	HINTS ${PC_LIBWD_LIBDIR} ${PC_LIBWD_LIBRARY_DIRS})
# _CHECK(LIBWD_LIBRARY "LIBWD_LIBRARY-NOTFOUND" "libwd.so")

macro(_DEFINE_NEW_TEST)
    add_executable(${ARGV0}
        main.cpp
        ${ARGV0}.cpp
        test_util.cpp
        zlib_func.cpp
        )

    target_link_libraries(${ARGV0}
        ${GTEST_LIBRARY}
        ${CMAKE_CURRENT_SOURCE_DIR}/../build/src/libunizip.so
        pthread
        gpg-error
        gmp
        z lz4 brotlidec brotlicommon brotlienc snappy bz2 lzma zstd ${LIB7LZMA_LIBRARY}
        )

    target_include_directories(${ARGV0} PUBLIC
        ${GTEST_INCLUDE_DIR}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC ${CMAKE_SOURCE_DIR}/src
        PUBLIC ${CMAKE_SOURCE_DIR}/src/xz
        PUBLIC ${CMAKE_SOURCE_DIR}/src/snappy
        PUBLIC ${CMAKE_SOURCE_DIR}/src/gzip
        PUBLIC ${CMAKE_SOURCE_DIR}/src/bzip2
        PUBLIC ${CMAKE_SOURCE_DIR}/src/zlib
        PUBLIC ${CMAKE_SOURCE_DIR}/src/zstd
        PUBLIC ${CMAKE_SOURCE_DIR}/src/7zip
        PUBLIC ${CMAKE_SOURCE_DIR}/src/brotli
        PUBLIC ${CMAKE_SOURCE_DIR}/src/lz4
        PUBLIC ${CMAKE_SOURCE_DIR}/src/uadk
        /usr/local/include/uadk
        )
        
    add_test(
        NAME ${ARGV1}
        COMMAND ${ARGV0}
        --gtest_output=xml:${ARGV1}-Results.xml
    )
endmacro()

# --------------- testcase add here -----------------

# fuzz test
option(ENABLE_FUZZ "set unizip fuzz option" OFF)
if (ENABLE_FUZZ)
    add_subdirectory(fuzz)
endif()

#   api testcase
_DEFINE_NEW_TEST(zlib_util Zlib_testcases)
_DEFINE_NEW_TEST(gzip_util gzip_testcases)
_DEFINE_NEW_TEST(bzip2_util bzip2_testcases)
_DEFINE_NEW_TEST(zstd_util zstd_testcases)
_DEFINE_NEW_TEST(xz_util Xz_testcases)
_DEFINE_NEW_TEST(snappy_util snappy_testcases)
_DEFINE_NEW_TEST(lz4_util Lz4_testcases)
_DEFINE_NEW_TEST(brotli_util Brotli_testcases)
_DEFINE_NEW_TEST(7zip_util 7zip_testcases)
_DEFINE_NEW_TEST(uadk_util uadk_testcases)

# mock test for run lcov to generate html
add_executable(mock_ut main.cpp)
target_include_directories(mock_ut PUBLIC
    ${GTEST_INCLUDE_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src
    PUBLIC ${CMAKE_SOURCE_DIR}/src/xz
    PUBLIC ${CMAKE_SOURCE_DIR}/src/snappy
    PUBLIC ${CMAKE_SOURCE_DIR}/src/gzip
    PUBLIC ${CMAKE_SOURCE_DIR}/src/bzip2
    PUBLIC ${CMAKE_SOURCE_DIR}/src/zlib
    PUBLIC ${CMAKE_SOURCE_DIR}/src/zstd
    PUBLIC ${CMAKE_SOURCE_DIR}/src/7zip
    PUBLIC ${CMAKE_SOURCE_DIR}/src/brotli
    PUBLIC ${CMAKE_SOURCE_DIR}/src/lz4
    PUBLIC ${CMAKE_SOURCE_DIR}/src/uadk #uadk
    /usr/local/include/uadk
    )
target_link_libraries(mock_ut ${GTEST_LIBRARY}
    pthread
    )
add_dependencies(mock_ut
                zlib_util
                bzip2_util
                zstd_util
                xz_util
                snappy_util
                lz4_util
                uadk_util
                brotli_util)

IF(ENABLE_GCOV)
    add_custom_target(coverage
        COMMAND lcov --directory . --zerocounters --rc lcov_branch_coverage=1
        COMMAND lcov -c -i -d . -o coverage.base --rc lcov_branch_coverage=1

        COMMAND ctest

        COMMAND lcov --directory ${CMAKE_SOURCE_DIR} --capture --output-file coverage.info --rc lcov_branch_coverage=1
        COMMAND lcov -a coverage.base -a coverage.info --output-file coverage.total --rc lcov_branch_coverage=1
        COMMAND lcov --remove coverage.total '/usr/*' '*/json/*' --output-file ${PROJECT_BINARY_DIR}/coverage.info.cleaned --rc lcov_branch_coverage=1
        COMMAND genhtml -o coverage ${PROJECT_BINARY_DIR}/coverage.info.cleaned --branch-coverage --rc lcov_branch_coverage=1
        COMMAND ${CMAKE_COMMAND} -E remove coverage.base coverage.total ${PROJECT_BINARY_DIR}/coverage.info.cleaned

        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMENT "generating report..."
    )

    add_custom_command(TARGET coverage POST_BUILD
        COMMAND ;
        COMMENT "open .test/coverage/index.html in browser to view the coverage analysis report."
    )
ENDIF(ENABLE_GCOV)
# --------------- testcase add finish -----------------

