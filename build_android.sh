#!/bin/bash

# Android Build Script for OTTO
echo "Building OTTO for Android..."

# Check if Android NDK is available
if [ -z "$ANDROID_NDK_ROOT" ]; then
    echo "Error: ANDROID_NDK_ROOT environment variable not set"
    echo "Please set it to your Android NDK installation path"
    exit 1
fi

# Check if Android SDK is available
if [ -z "$ANDROID_HOME" ]; then
    echo "Error: ANDROID_HOME environment variable not set"
    echo "Please set it to your Android SDK installation path"
    exit 1
fi

# Create build directory in proper JUCE 8 structure
BUILD_DIR="Builds/Android/CMake"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake for Android using JUCE 8 conventions
cmake ../../.. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_ANDROID_NDK=$ANDROID_NDK_ROOT \
    -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a \
    -DCMAKE_ANDROID_API=23 \
    -DCMAKE_ANDROID_STL_TYPE=c++_shared

# Build
cmake --build . --config Release --parallel $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Check build result
if [ $? -eq 0 ]; then
    echo "✅ Android build complete!"
    echo ""
    echo "Build artifacts location:"
    find ../Release -name "*.so" -o -name "*.apk" 2>/dev/null | head -10
    echo ""
    echo "📁 All Android builds are now in: Builds/Android/"
    echo "Note: Make sure Android NDK and SDK are properly installed and configured."
else
    echo "❌ Android build failed!"
    exit 1
fi