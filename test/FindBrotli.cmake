# - Try to find Brotli
# Once done this will define
#
#  BROTLI_FOUND - System has Brotli
#  BROTLI_INCLUDE_DIRS - The Brotli include directories
#  BROTLI_LIBRARIES - The libraries needed to use Brotli
#  BROTLI_DEFINITIONS - Compiler switches required for using Brotli

find_path(BROTLI_INCLUDE_DIRS NAMES decode.h PATHS /usr/include/brotli)
find_library(BROTLICOMMON_LIBRARY NAMES brotlicommon)
find_library(BROTLIDEC_LIBRARY NAMES brotlidec)
find_library(BROTLIENC_LIBRARY NAMES brotlienc)
set(BROTLI_LIBRARIES "")
if(BROTLICOMMON_LIBRARY)
    list(APPEND BROTLI_LIBRARIES ${BROTLICOMMON_LIBRARY})
endif()

if(BROTLIDEC_LIBRARY)
    list(APPEND BROTLI_LIBRARIES ${BROTLIDEC_LIBRARY})
endif()

if(BROTLIENC_LIBRARY)
    list(APPEND BROTLI_LIBRARIES ${BROTLIENC_LIBRARY})
endif()
message(STATUS "Found Brotli: ${BROTLI_LIBRARIES}")
if(BROTLI_INCLUDE_DIRS AND BROTLI_LIBRARIES)
    set(BROTLI_FOUND TRUE)
endif()

if(BROTLI_FOUND)
    set(BROTLI_DEFINITIONS)
    message(STATUS "Found Brotli: ${BROTLI_INCLUDE_DIRS}")
else()
    set(BROTLI_DEFINITIONS -DBROTLI_NOTFOUND)
    message(STATUS "Brotli library not found.")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Brotli DEFAULT_MSG
    BROTLI_INCLUDE_DIRS BROTLI_LIBRARIES)

mark_as_advanced(BROTLI_INCLUDE_DIRS BROTLI_LIBRARIES)
