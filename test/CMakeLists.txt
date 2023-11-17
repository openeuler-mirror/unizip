
project(unizip)

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

add_library(7zip OBJECT IMPORTED)
set_property(TARGET 7zip PROPERTY IMPORTED_LOCATION /usr/lib64/7z)




set( TESTS_UTILS_SRCS
    ${CMAKE_SOURCE_DIR}/src/unizip_struct.h
    ${CMAKE_SOURCE_DIR}/src/unizip_adapt.h
    ${CMAKE_SOURCE_DIR}/src/unizip_adapt.c
    # ${CMAKE_SOURCE_DIR}/src/xz/unizip_algorithm_xz.c
    # ${CMAKE_SOURCE_DIR}/src/xz/unizip_algorithm_xz.h
    # ${CMAKE_SOURCE_DIR}/src/xz/unizip_callback_xz.c
    # ${CMAKE_SOURCE_DIR}/src/xz/unizip_callback_xz.h
    # ${CMAKE_SOURCE_DIR}/src/gzip/unizip_callback_gzip.h
    # ${CMAKE_SOURCE_DIR}/src/gzip/unizip_callback_gzip.c
    ${CMAKE_SOURCE_DIR}/src/zlib/unizip_callback_zlib.h
    ${CMAKE_SOURCE_DIR}/src/zlib/unizip_callback_zlib.c
    # ${CMAKE_SOURCE_DIR}/src/bzip2/unizip_callback_bzip2.h
    # ${CMAKE_SOURCE_DIR}/src/bzip2/unizip_callback_bzip2.c
    # ${CMAKE_SOURCE_DIR}/src/bzip2/unizip_algorithm_bzip2.h
    # ${CMAKE_SOURCE_DIR}/src/bzip2/unizip_algorithm_bzip2.c
    # ${CMAKE_SOURCE_DIR}/src/zstd/unizip_algorithm_zstd.c
    # ${CMAKE_SOURCE_DIR}/src/zstd/unizip_algorithm_zstd.h
    # ${CMAKE_SOURCE_DIR}/src/zstd/unizip_callback_zstd.c
    # ${CMAKE_SOURCE_DIR}/src/zstd/unizip_callback_zstd.h
    # ${CMAKE_SOURCE_DIR}/src/snappy/unizip_algorithm_snappy.c
    # ${CMAKE_SOURCE_DIR}/src/snappy/unizip_algorithm_snappy.h
    # ${CMAKE_SOURCE_DIR}/src/snappy/unizip_callback_snappy.c
    # ${CMAKE_SOURCE_DIR}/src/snappy/unizip_callback_snappy.h
    # ${CMAKE_SOURCE_DIR}/src/brotli/unizip_algorithm_brotli.c
    # ${CMAKE_SOURCE_DIR}/src/brotli/unizip_algorithm_brotli.h
    # ${CMAKE_SOURCE_DIR}/src/brotli/unizip_callback_brotli.c
    # ${CMAKE_SOURCE_DIR}/src/brotli/unizip_callback_brotli.h
    # ${CMAKE_SOURCE_DIR}/src/uadk/unizip_callback_uadk.h
    # ${CMAKE_SOURCE_DIR}/src/uadk/unizip_callback_uadk.c
    # ${CMAKE_SOURCE_DIR}/src/uadk/unizip_algorithm_uadk.h
    # ${CMAKE_SOURCE_DIR}/src/uadk/unizip_algorithm_uadk.c
    # ${CMAKE_SOURCE_DIR}/src/lz4/unizip_algorithm_lz4.c
    # ${CMAKE_SOURCE_DIR}/src/lz4/unizip_algorithm_lz4.h
    # ${CMAKE_SOURCE_DIR}/src/lz4/unizip_callback_lz4.c
    # ${CMAKE_SOURCE_DIR}/src/lz4/unizip_callback_lz4.h
    ${CMAKE_SOURCE_DIR}/test/test_util.h
    ${CMAKE_SOURCE_DIR}/test/test_util.cpp
)

macro(_DEFINE_NEW_TEST)
    add_executable(${ARGV0}
        ${TESTS_UTILS_SRCS}
        main.cpp
        ${ARGV0}.cpp
        )

    target_link_libraries(${ARGV0}
        ${GTEST_LIBRARY}
        pthread
        ${LIBLZMA_LIBRARIES} 
        ${GTEST_BOTH_LIBRARIES} 
        ${ZLIB_LIBRARIES} 
        ${BZIP2_LIBRARIES} 
        ${SNAPPY_LIBRARIES} 
        ${LIBWD_LIBRARY}  #uadk
        libzstd.so 
        ${LZ4_LIBRARIES}
        ${BROTLI_LIBRARIES}
        snappy
        gpg-error
        gmp
        )

    target_include_directories(${ARGV0} PUBLIC
        ${GTEST_INCLUDE_DIR}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC ${CMAKE_SOURCE_DIR}/src
        # PUBLIC ${CMAKE_SOURCE_DIR}/src/xz
        # PUBLIC ${CMAKE_SOURCE_DIR}/src/snappy
        # PUBLIC ${CMAKE_SOURCE_DIR}/src/gzip
        # PUBLIC ${CMAKE_SOURCE_DIR}/src/bzip2
        PUBLIC ${CMAKE_SOURCE_DIR}/src/zlib
        # PUBLIC ${CMAKE_SOURCE_DIR}/src/zstd
        # PUBLIC ${CMAKE_SOURCE_DIR}/src/7zip
        # PUBLIC ${CMAKE_SOURCE_DIR}/src/brotli
        # PUBLIC ${CMAKE_SOURCE_DIR}/src/lz4
        #PUBLIC ${CMAKE_SOURCE_DIR}/src/uadk
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
#_DEFINE_NEW_TEST(bzip2_util bzip2_testcases)
#_DEFINE_NEW_TEST(zstd_util zstd_testcases)
#_DEFINE_NEW_TEST(xz_util Xz_testcases)
#_DEFINE_NEW_TEST(snappy_util snappy_testcases)
#_DEFINE_NEW_TEST(lz4_util Lz4_testcases)

# mock test for run lcov to generate html
add_executable(mock_ut ${TESTS_UTILS_SRCS} main.cpp)
target_include_directories(mock_ut PUBLIC
    ${GTEST_INCLUDE_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src
    # PUBLIC ${CMAKE_SOURCE_DIR}/src/xz
    # PUBLIC ${CMAKE_SOURCE_DIR}/src/snappy
    # PUBLIC ${CMAKE_SOURCE_DIR}/src/gzip
    # PUBLIC ${CMAKE_SOURCE_DIR}/src/bzip2
    PUBLIC ${CMAKE_SOURCE_DIR}/src/zlib
    # PUBLIC ${CMAKE_SOURCE_DIR}/src/zstd
    # PUBLIC ${CMAKE_SOURCE_DIR}/src/7zip
    # PUBLIC ${CMAKE_SOURCE_DIR}/src/brotli
    # PUBLIC ${CMAKE_SOURCE_DIR}/src/lz4
    #PUBLIC ${CMAKE_SOURCE_DIR}/src/uadk #uadk
    /usr/local/include/uadk
    )
target_link_libraries(mock_ut
    ${GTEST_LIBRARY}
    ${LIBLZMA_LIBRARIES} 
    ${GTEST_BOTH_LIBRARIES} 
    ${ZLIB_LIBRARIES} 
    ${BZIP2_LIBRARIES} 
    ${SNAPPY_LIBRARIES} 
    ${LIBWD_LIBRARY}
    ${LZ4_LIBRARIES}
    libzstd.so 
    snappy
    pthread
    ${BROTLI_LIBRARIES}
    )
add_dependencies(mock_ut zlib_util)

IF(ENABLE_GCOV)
    add_custom_target(coverage
        COMMAND lcov --directory . --zerocounters --rc lcov_branch_coverage=1
        COMMAND lcov -c -i -d . -o coverage.base --rc lcov_branch_coverage=1

        COMMAND ctest

        COMMAND lcov --directory . --capture --output-file coverage.info --rc lcov_branch_coverage=1
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

