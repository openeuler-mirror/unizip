message("${BoldGreen}---- Selected options begin ----${ColourReset}")

option(ENABLE_GCOV "set unizip gcov option" OFF)
if (ENABLE_GCOV STREQUAL "ON")
    message("${Green}--  Enable gcov${ColourReset}")
endif()

option(ENABLE_LIBUNIZIP "enable unizip libunizip option" ON)
if (ENABLE_LIBUNIZIP STREQUAL "ON")
    add_definitions(-DENABLE_LIBUNIZIP=1)
    set(ENABLE_LIBUNIZIP 1)
    message("${Green}--  Enable libunizip${ColourReset}")
endif()

message("${BoldGreen}---- Selected options end ----${ColourReset}")
