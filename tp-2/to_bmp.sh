#!/usr/bin/env bash

# Check if ImageMagick is installed
if ! command -v magick &>/dev/null; then
    echo "ImageMagick is not installed"
    exit 1
fi

# Check if at least one argument is passed
if [ $# -lt 1 ]; then
    echo "Usage: ./script.sh <extra magick args> <img path>"
    exit 1
fi

# Get the image path (the last argument)
IMG_PATH="${!#}"

# Get all the extra arguments (ImageMagick options)
MAGICK_ARGS="${@:1:$(($# - 1))}"

# Get basename without extension
FILENAME_EXT=$(basename "$IMG_PATH")
FILENAME="${FILENAME_EXT%.*}"
echo "Converting $FILENAME_EXT to $FILENAME.bmp"

# Convert a file to a 24bit bmp file
magick "$IMG_PATH" -depth 8 -alpha off -format BGR24 -define bmp:format=bmp3 -compress none $MAGICK_ARGS "./data/assets/texture/$FILENAME.bmp"
