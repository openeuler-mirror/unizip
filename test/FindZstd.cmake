find_path(Zstd_INCLUDE_DIR NAMES zstd.h PATH_SUFFIXES zstd)
find_library(Zstd_LIBRARY NAMES zstd)

if(Zstd_INCLUDE_DIR AND Zstd_LIBRARY)
    set(Zstd_FOUND TRUE)
endif()

if(Zstd_FOUND)
    if(NOT Zstd_FIND_QUIETLY)
        message(STATUS "Found Zstd: ${Zstd_LIBRARY}")
    endif()
else()
    if(Zstd_FIND_REQUIRED)
        message(FATAL_ERROR "Zstd library not found")
    endif()
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Zstd DEFAULT_MSG Zstd_LIBRARY Zstd_INCLUDE_DIR)