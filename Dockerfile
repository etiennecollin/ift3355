# Use the official Alpine base image
FROM alpine:latest

# Install necessary packages for C++ compilation
RUN apk update && \
    apk --no-cache add \
    build-base \
    gdb \
    cmake \
    clang \
    libstdc++ \
    zig \
    bash

# Set up a working directory
WORKDIR /workspace

CMD ["bash"]

