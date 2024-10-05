#!/bin/bash

# Get the target from the first argument
TARGET="aarch64-macos" # or x86_64-windows

# Create the build directory if it doesn't exist
mkdir -p build

# Run cmake with the provided target
cmake -S . -B build -DTARGET=$TARGET

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
