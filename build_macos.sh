#!/bin/bash

# macOS Build Script for OTTO
echo "Building OTTO for macOS..."

# Create build directory
mkdir -p build-macos
cd build-macos

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 \
    -G Xcode

# Build
cmake --build . --config Release --parallel $(sysctl -n hw.ncpu)

echo "macOS build complete! Check build-macos directory for outputs."