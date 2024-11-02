include_guard()

message(STATUS "Setting toolchain for macOS compilation. Make sure `zig` and `gcc` are installed from homebrew.")

# Run the brew command to get the prefix for GCC
execute_process(
    COMMAND brew --prefix gcc
    OUTPUT_VARIABLE GCC_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_C_COMPILER "${GCC_PREFIX}/bin/gcc-14")
set(CMAKE_CXX_COMPILER "${GCC_PREFIX}/bin/g++-14")

set(CMAKE_AR "${CMAKE_CURRENT_LIST_DIR}/zig_ar.sh")
set(CMAKE_RANLIB "${CMAKE_CURRENT_LIST_DIR}/zig_ranlib.sh")
set(CMAKE_RC "${CMAKE_CURRENT_LIST_DIR}/zig_rc.sh")
