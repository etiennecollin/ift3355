include_guard()

set(ZIG_TARGET "aarm64-macos-none")
if(NOT ZIG_TARGET MATCHES "^([a-zZ-Z0-9_]+)-([a-zZ-Z0-9_]+)-([a-zZ-Z0-9_]+)$")
    message(FATAL_ERROR "Expected ZIG_TARGET=<arch>-<os>-<abi>")
endif()

set(ZIG_ARCH ${CMAKE_MATCH_1})
set(ZIG_OS ${CMAKE_MATCH_2})
set(ZIG_ABI ${CMAKE_MATCH_3})

if(ZIG_OS STREQUAL "linux")
    set(CMAKE_SYSTEM_NAME "Linux")
elseif(ZIG_OS STREQUAL "windows")
    set(CMAKE_SYSTEM_NAME "Windows")
elseif(ZIG_OS STREQUAL "macos")
    set(CMAKE_SYSTEM_NAME "Darwin")
else()
    message(WARNING "Unknown OS: ${ZIG_OS}")
endif()

set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR ${ZIG_ARCH})


set(CMAKE_C_COMPILER zig cc)
set(CMAKE_CXX_COMPILER zig c++)
set(CMAKE_C_COMPILER_TARGET ${ZIG_TARGET})
set(CMAKE_CXX_COMPILER_TARGET ${ZIG_TARGET})

set(CMAKE_AR "${CMAKE_CURRENT_LIST_DIR}/zig_ar.sh")
set(CMAKE_RANLIB "${CMAKE_CURRENT_LIST_DIR}/zig_ranlib.sh")
set(CMAKE_RC "${CMAKE_CURRENT_LIST_DIR}/zig_rc.sh")

message(STATUS "Toolchain configured for ${ZIG_TARGET}")
