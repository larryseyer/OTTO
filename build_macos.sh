#!/bin/bash

# macOS Build Script for OTTO (JUCE 8)
echo "Building OTTO for macOS..."

# Create build directory in proper JUCE 8 structure
BUILD_DIR="Builds/MacOSX/CMake"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake using JUCE 8 conventions
cmake ../../.. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 \
    -G Xcode

# Build
cmake --build . --config Release --parallel $(sysctl -n hw.ncpu)

# Check build result
if [ $? -eq 0 ]; then
    echo "✅ macOS build complete!"
    echo ""
    echo "Build artifacts location:"
    find ../Release -name "*.app" -o -name "*.vst3" -o -name "*.component" 2>/dev/null | head -10
    echo ""
    echo "📁 All macOS builds are now in: Builds/MacOSX/"
else
    echo "❌ macOS build failed!"
    exit 1
fi