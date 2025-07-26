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

# Create build directory in proper JUCE 8 structure
BUILD_DIR="Builds/Linux/CMake"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake using JUCE 8 conventions
cmake ../../.. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-fPIC"

# Build
cmake --build . --config Release --parallel $(nproc)

# Check build result
if [ $? -eq 0 ]; then
    echo "✅ Linux build complete!"
    echo ""
    echo "Build artifacts location:"
    find ../Release -name "*.so" -o -name "OTTO" 2>/dev/null | head -10
    echo ""
    echo "📁 All Linux builds are now in: Builds/Linux/"
else
    echo "❌ Linux build failed!"
    exit 1
fi