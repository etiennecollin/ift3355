#!/usr/bin/env bash

# Create the build directory if it doesn't exist
mkdir -p build

# Check if -r flag was passed to enable logging
RELEASE=false
if [ "$1" == "-r" ]; then
    echo "Building with release profile"
    RELEASE=true
else
    echo "Building with debug profile"
fi

# Run cmake with the provided target
if $RELEASE; then
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="-O1" -S . -B build
else
    cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build
fi

# Check if cmake ran successfully
if [ $? -eq 0 ]; then
    echo "CMake configuration successful."
else
    echo "CMake configuration failed."
    exit 1
fi

cmake --build build

if [ $? -eq 0 ]; then
    echo "Build successful."
else
    echo "Build failed."
    exit 1
fi
