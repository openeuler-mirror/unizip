find_path(LZ4_INCLUDE_DIRS NAMES lz4.h PATH /usr/include/)
find_library(LZ4_LIBRARIES NAMES liblz4.so)
message(STATUS "Found LZ4: ${LZ4_LIBRARIES}")

if(LZ4_INCLUDE_DIRS AND LZ4_LIBRARIES)
    set(LZ4_FOUND TRUE)
endif()

if(LZ4_FOUND)
    set(LZ4_LIBRARIES)
    message(STATUS "Found lZ4: ${LZ4_INCLUDE_DIRS}")
else()
    set(LZ4_DEFINITIONS -LZ4_NOTFOUND)
    message(STATUS "LZ4 library not found.")
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LZ4 DEFAULT_MSG LZ4_INCLUDE_DIRS LZ4_LIBRARIES)

mark_as_advanced(LZ4_INCLUDE_DIRS LZ4_LIBRARIES)