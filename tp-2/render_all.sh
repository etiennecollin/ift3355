#!/usr/bin/env bash

# Function to display an error message and exit
function error_exit {
    echo "$1" >&2
    exit 1
}

# Check that fd is installed
if [ ! -x "$(command -v fd)" ]; then
    error_exit "Error: 'fd' is not installed. Please install it."
fi

# Check if -l flag was passed to enable logging
LOGGING=false
if [ "$1" == "-l" ]; then
    echo "Logging enabled."
    echo "Logs will be saved in ./logs/ directory"
    mkdir -p logs
    LOGGING=true
fi

# Render in parallel
if $LOGGING; then
    time fd -e ray -d 1 "." ./data/scene -x sh -c "./build/RAY '{/}' > './logs/{/.}.txt'"
else

    time fd -e ray -d 1 "." ./data/scene -x ./build/RAY "{/}"
fi
