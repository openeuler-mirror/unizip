# lcr: utils library for iSula
#
# Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
#
# Authors:
# Haozi007 <liuhao27@huawei.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#

set(check_dep_libs
    ${LIBZLIB_LIBRARY}
    ${LIBWD_LIBRARY}
    ${LIBLZ4_LIBRARY}
    ${LIBBROTLI_LIBRARY}
    ${LIBSNAPPY_LIBRARY}
    ${LIBBZIP2_LIBRARY}
    ${LIBXZ_LIBRARY}
    )

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
set(wd_lib wd)
endif()

list(REMOVE_DUPLICATES check_dep_libs)

# get top source files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} topsrcs)
file(GLOB_RECURSE ALL_C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
# file(GLOB_RECURSE EXCLUDE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/7zip/*.c")
# list(REMOVE_ITEM ALL_C_FILES ${EXCLUDE_FILES})
# file(GLOB_RECURSE EXCLUDE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/uadk/*.c")
# list(REMOVE_ITEM ALL_C_FILES ${EXCLUDE_FILES})
message("--  Get top srcs: " ${ALL_C_FILES})

if (ENABLE_LIBUNIZIP)
    # set libunizip library
    add_library(libunizip SHARED ${ALL_C_FILES})

    set(libunizip_incs
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}/conf
        ${check_incs}
        ${CMAKE_CURRENT_SOURCE_DIR}/xz
        ${CMAKE_CURRENT_SOURCE_DIR}/snappy
        ${CMAKE_CURRENT_SOURCE_DIR}/gzip
        ${CMAKE_CURRENT_SOURCE_DIR}/bzip2
        ${CMAKE_CURRENT_SOURCE_DIR}/zlib
        ${CMAKE_CURRENT_SOURCE_DIR}/zstd
        ${CMAKE_CURRENT_SOURCE_DIR}/7zip
        ${CMAKE_CURRENT_SOURCE_DIR}/brotli
        ${CMAKE_CURRENT_SOURCE_DIR}/lz4
        ${CMAKE_CURRENT_SOURCE_DIR}/uadk
    )

    # set libunizip include dirs
    target_include_directories(libunizip
        PUBLIC ${libunizip_incs}
        )

    # set libunizip compile flags
    set_target_properties(libunizip PROPERTIES PREFIX "")
    target_link_libraries(libunizip ${check_dep_libs} z lz4 brotlidec brotlicommon brotlienc snappy bz2 lzma zstd 7lzma ${wd_lib})

    if (CMAKE_TOOLCHAIN_FILE)
        target_link_directories(libunizip PUBLIC ${CMAKE_LIBRARY_PATH})
        target_link_libraries(libunizip ${EXTRAL_LINK_LIBS})
    endif()


    add_library(libunizip_s STATIC ${ALL_C_FILES})
    target_include_directories(libunizip_s
        PUBLIC ${libunizip_incs}
        )
    set_target_properties(libunizip_s PROPERTIES PREFIX "")
    set_target_properties(libunizip_s PROPERTIES OUTPUT_NAME libunizip)

    # install all files
    install(TARGETS libunizip
        LIBRARY DESTINATION ${LIB_INSTALL_DIR_DEFAULT} PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    install(TARGETS libunizip_s
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR_DEFAULT} PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/unizip_struct.h ${CMAKE_CURRENT_SOURCE_DIR}/unizip_adapt.h DESTINATION ${INC_INSTALL_DIR_DEFAULT})
endif()

