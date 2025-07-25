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

# Create build directory
mkdir -p build-android
cd build-android

# Configure with CMake for Android
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_ANDROID_NDK=$ANDROID_NDK_ROOT \
    -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a \
    -DCMAKE_ANDROID_API=23 \
    -DCMAKE_ANDROID_STL_TYPE=c++_shared

# Build
cmake --build . --config Release --parallel $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "Android build complete! Check build-android directory for outputs."
echo "Note: Make sure Android NDK and SDK are properly installed and configured."