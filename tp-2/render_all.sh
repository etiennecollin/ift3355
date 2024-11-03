#!/bin/bash

# Check that fd and parallel are installed
if [ ! -x "$(command -v fd)" ]; then
    echo "fd is not installed. Please install it."
    exit 1
fi

if [ ! -x "$(command -v parallel)" ]; then
    echo "parallel is not installed. Please install it."
    exit 1
fi

# Render on 20 threads
fd ".*.ray" data/scene --format {/} | parallel -j 20 -n 1 ./build/RAY
