#!/usr/bin/env bash

# Create a symlink to the data directory in the zig-out/bin/ directory
mkdir -p ./zig-out/bin/
cd ./zig-out/bin/
ln -s ../../data/ ./data
echo "Created symlink: data directory is now available to the executable."
echo "If you encounter an error saying the scene files cannot be found when executing the raytracer, run this script again."
