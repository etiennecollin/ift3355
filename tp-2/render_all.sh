#!/bin/bash

# Check that fd and parallel are installed
if [ ! -x "$(command -v fd)" ]; then
    echo "fd is not installed. Please install it."
    exit 1
fi

mkdir -p logs

# Render in parallel
# time fd -e ray -p ./data/scene -x sh -c "./build/RAY '{/}' > './logs/{/.}.txt'"
time fd -e ray -p ./data/scene -x sh -c ./build/RAY "{/}"
