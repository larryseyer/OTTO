#!/bin/bash

# iOS Build Script for OTTO (JUCE 8)
echo "Building OTTO for iOS..."

# Detect Mac architecture for simulator builds
HOST_ARCH=$(uname -m)
echo "Host architecture: $HOST_ARCH"

# Parse command line arguments
BUILD_TARGET="device"
SIMULATOR_SDK_PATH=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --simulator)
            BUILD_TARGET="simulator"
            shift
            ;;
        --device)
            BUILD_TARGET="device"
            shift
            ;;
        --team-id)
            TEAM_ID="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--simulator|--device] [--team-id YOUR_TEAM_ID]"
            exit 1
            ;;
    esac
done

# Set architecture and SDK based on target
if [ "$BUILD_TARGET" = "simulator" ]; then
    SIMULATOR_SDK_PATH=$(xcrun --show-sdk-path --sdk iphonesimulator)
    if [ "$HOST_ARCH" = "arm64" ]; then
        IOS_ARCH="arm64"
        echo "Building for iOS Simulator on Apple Silicon Mac"
    else
        IOS_ARCH="x86_64"
        echo "Building for iOS Simulator on Intel Mac"
    fi
    BUILD_DIR="build-ios-simulator"
    CMAKE_OSX_SYSROOT="iphonesimulator"
else
    IOS_ARCH="arm64"
    BUILD_DIR="build-ios-device"
    CMAKE_OSX_SYSROOT="iphoneos"
    echo "Building for iOS Device"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Set team ID (try environment variable first)
if [ -z "$TEAM_ID" ]; then
    TEAM_ID="${APPLE_TEAM_ID:-YOUR_TEAM_ID}"
fi

echo "Using architecture: $IOS_ARCH"
echo "Using Team ID: $TEAM_ID"

# Configure with CMake for iOS
if [ "$BUILD_TARGET" = "simulator" ]; then
    echo "Configuring for iOS Simulator with SDK: $SIMULATOR_SDK_PATH"
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_ARCHITECTURES="$IOS_ARCH" \
        -DCMAKE_OSX_SYSROOT="$CMAKE_OSX_SYSROOT" \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
        -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="$TEAM_ID" \
        -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="iPhone Developer" \
        -DCMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH=NO \
        -G Xcode
else
    echo "Configuring for iOS Device"
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_ARCHITECTURES="$IOS_ARCH" \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
        -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="$TEAM_ID" \
        -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="iPhone Developer" \
        -G Xcode
fi

# Check if CMake configuration was successful
if [ $? -ne 0 ]; then
    echo "❌ CMake configuration failed!"
    exit 1
fi

# Build
echo "Starting build with $(sysctl -n hw.ncpu) parallel jobs..."
if [ "$BUILD_TARGET" = "device" ]; then
    echo "Building for iOS Device - enabling automatic provisioning..."
    cmake --build . --config Release --parallel $(sysctl -n hw.ncpu) -- -allowProvisioningUpdates
else
    echo "Building for iOS Simulator..."
    cmake --build . --config Release --parallel $(sysctl -n hw.ncpu)
fi

# Check build result
if [ $? -eq 0 ]; then
    echo "✅ iOS build complete! Check $BUILD_DIR directory for outputs."
    echo ""
    echo "Build artifacts location:"
    find . -name "*.app" -o -name "*.appex" 2>/dev/null | head -5
    
    if [ "$BUILD_TARGET" = "simulator" ]; then
        echo ""
        echo "🔧 To run in CLion iOS Simulator:"
        echo "1. Set DYLD_ROOT_PATH=$SIMULATOR_SDK_PATH"
        echo "2. Use architecture: $IOS_ARCH"
    fi
else
    echo "❌ iOS build failed!"
    exit 1
fi

echo ""
echo "💡 Tips:"
echo "- For simulator: $0 --simulator --team-id YOUR_TEAM_ID"
echo "- For device: $0 --device --team-id YOUR_TEAM_ID"
echo "- Set APPLE_TEAM_ID environment variable to avoid specifying --team-id"
if [ "$TEAM_ID" = "YOUR_TEAM_ID" ]; then
    echo "⚠️  Remember to set your Apple Developer Team ID!"
fi