#!/bin/bash

# iOS Build Script for OTTO
echo "Building OTTO for iOS..."

# Create build directory
mkdir -p build-ios
cd build-ios

# Configure with CMake for iOS
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_ARCHITECTURES="arm64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="YOUR_TEAM_ID" \
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="iPhone Developer" \
    -G Xcode

# Build
cmake --build . --config Release --parallel $(sysctl -n hw.ncpu)

echo "iOS build complete! Check build-ios directory for outputs."
echo "Note: You'll need to set your Apple Developer Team ID in the script."