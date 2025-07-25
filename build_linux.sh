#!/bin/bash

# Linux Build Script for OTTO
echo "Building OTTO for Linux..."

# Install dependencies (Ubuntu/Debian)
if command -v apt-get &> /dev/null; then
    echo "Installing dependencies..."
    sudo apt-get update
    sudo apt-get install -y \
        build-essential \
        cmake \
        libasound2-dev \
        libgtk-3-dev \
        libwebkit2gtk-4.0-dev \
        libfreetype6-dev \
        libcurl4-openssl-dev \
        pkg-config
fi

# Create build directory
mkdir -p build-linux
cd build-linux

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-fPIC"

# Build
cmake --build . --config Release --parallel $(nproc)

echo "Linux build complete! Check build-linux directory for outputs."