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

set(check_zlib_libs
    ${LIBZLIB_LIBRARY}
    ${LIBWD_LIBRARY}
    )

list(REMOVE_DUPLICATES check_zlib_libs)

# get top source files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} topsrcs)
list(REMOVE_ITEM topsrcs "${CMAKE_CURRENT_SOURCE_DIR}/utils.c")
message("--  Get top srcs: " ${topsrcs})

if (ENABLE_LIBUNIZIP)
    # set libunizip library
    add_library(libunizip SHARED ${topsrcs})

    set(libunizip_incs
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}/conf
        ${check_incs}
        ${CMAKE_CURRENT_SOURCE_DIR}/zlib
    )

    # set libunizip include dirs
    target_include_directories(libunizip
        PUBLIC ${libunizip_incs}
        )

    # set libunizip compile flags
    set_target_properties(libunizip PROPERTIES PREFIX "")
    target_link_libraries(libunizip ${check_zlib_libs} z)

    if (CMAKE_TOOLCHAIN_FILE)
        target_link_directories(libunizip PUBLIC ${CMAKE_LIBRARY_PATH})
        target_link_libraries(libunizip ${EXTRAL_LINK_LIBS})
    endif()


    add_library(libunizip_s STATIC ${topsrcs})
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
endif()

