#!/usr/bin/env bash

BUILD_DIR="$1"

# Create the build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="-O1" -DCMAKE_CXX_FLAGS="-$2" -S . -B "$BUILD_DIR"

# Check if cmake ran successfully
if [ $? -eq 0 ]; then
    echo "CMake configuration successful."
else
    echo "CMake configuration failed."
    exit 1
fi

cmake --build "$BUILD_DIR"

if [ $? -eq 0 ]; then
    echo "Build successful."
else
    echo "Build failed."
    exit 1
fi

# cat profiles.txt | parallel -j20 -n1 ./build_test.sh "./build/{}" "{}"
# fd -t l -d 1 "data" ./build/* -x rm {}
# fd -t d -d 1 "." ./build/ -x cp -r ./data {}
# fd -t d -d 1 "." ./build/ -x sh -c "cd {} && ./RAY all_at_once.ray"
# fd -e bmp -I -p "/output/all_at_once/color\." ./build/ -x open {}
