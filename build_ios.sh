#!/bin/bash

# iOS Build Script for OTTO
echo "Building OTTO for iOS..."

# Create build directory
mkdir -p build-ios
cd build-ios

# Configure with CMake for iOS Simulator
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
    -DCMAKE_OSX_SYSROOT=iphonesimulator \
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="Apple Development" \
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED=YES \
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED=YES \
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE="Automatic" \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="" \
    -G Xcode

# Build for simulator with automatic signing
cmake --build . --config Release --parallel $(sysctl -n hw.ncpu) -- -sdk iphonesimulator CODE_SIGNING_REQUIRED=NO CODE_SIGNING_ALLOWED=NO

echo "iOS build complete! Check build-ios directory for outputs."
echo "App built for iOS Simulator without code signing requirements."