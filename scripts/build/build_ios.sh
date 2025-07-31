#!/bin/bash

# iOS Build Script for OTTO (JUCE 8)
# Unified CMake build system that matches ProJucer configuration

set -e

echo "📱 Building OTTO for iOS..."
echo "============================"

# Function to print colored output
print_status() {
    echo -e "\033[1;32m$1\033[0m"
}

print_error() {
    echo -e "\033[1;31m$1\033[0m"
}

print_warning() {
    echo -e "\033[1;33m$1\033[0m"
}

# Detect Mac architecture for simulator builds
HOST_ARCH=$(uname -m)
print_status "🖥️  Host architecture: $HOST_ARCH"

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
        print_status "🍎 Building for iOS Simulator on Apple Silicon Mac"
    else
        IOS_ARCH="x86_64"
        print_status "🔧 Building for iOS Simulator on Intel Mac"
    fi
    BUILD_DIR="Builds/iOS/CMake-Simulator-Release"
    CMAKE_OSX_SYSROOT="iphonesimulator"
else
    IOS_ARCH="arm64"
    BUILD_DIR="Builds/iOS/CMake-Device-Release"
    CMAKE_OSX_SYSROOT="iphoneos"
    print_status "📱 Building for iOS Device"
fi

# Get the directory of this script and change to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
cd "$PROJECT_ROOT"

# Create build directory
mkdir -p "$BUILD_DIR"

# Clean up any previous CMake cache
rm -rf "$BUILD_DIR/CMakeCache.txt" "$BUILD_DIR/CMakeFiles"

print_status "📁 Build directory: $BUILD_DIR"

cd "$BUILD_DIR"

# Set team ID (try environment variable first)
if [ -z "$TEAM_ID" ]; then
    TEAM_ID="${APPLE_TEAM_ID:-YOUR_TEAM_ID}"
fi

print_status "🏗️  Architecture: $IOS_ARCH"
print_status "👤 Team ID: $TEAM_ID"

print_status "⚙️  Configuring CMake..."

# Configure with CMake for iOS using unified settings
if [ "$BUILD_TARGET" = "simulator" ]; then
    print_status "🔧 Configuring for iOS Simulator with SDK: $SIMULATOR_SDK_PATH"
    cmake "$PROJECT_ROOT" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_ARCHITECTURES="$IOS_ARCH" \
        -DCMAKE_OSX_SYSROOT="$CMAKE_OSX_SYSROOT" \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
        -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="$TEAM_ID" \
        -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="iPhone Developer" \
        -DCMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH=NO \
        -DJUCE_ENABLE_AUV3_APP_EXTENSIONS=ON \
        -DJUCE_AUV3_MIDI_INPUT_SUPPORTED=ON \
        -DJUCE_BUILD_EXAMPLES=OFF \
        -DJUCE_BUILD_EXTRAS=OFF \
        -G Xcode
else
    print_status "📱 Configuring for iOS Device"
    cmake "$PROJECT_ROOT" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_ARCHITECTURES="$IOS_ARCH" \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
        -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="$TEAM_ID" \
        -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="iPhone Developer" \
        -DJUCE_ENABLE_AUV3_APP_EXTENSIONS=ON \
        -DJUCE_AUV3_MIDI_INPUT_SUPPORTED=ON \
        -DJUCE_BUILD_EXAMPLES=OFF \
        -DJUCE_BUILD_EXTRAS=OFF \
        -G Xcode
fi

# Check if CMake configuration was successful
if [ $? -ne 0 ]; then
    print_error "❌ CMake configuration failed!"
    exit 1
fi

print_status "🔧 Building OTTO..."

# Build with proper parallel processing
NUM_CORES=$(sysctl -n hw.ncpu)
print_status "🚀 Starting build with $NUM_CORES parallel jobs..."

if [ "$BUILD_TARGET" = "device" ]; then
    print_status "📱 Building for iOS Device - enabling automatic provisioning..."
    cmake --build . --config Release --parallel $NUM_CORES -- -allowProvisioningUpdates
else
    print_status "🖥️  Building for iOS Simulator..."
    cmake --build . --config Release --parallel $NUM_CORES
fi

# Check build result
if [ $? -eq 0 ]; then
    print_status "✅ iOS build complete!"
    echo ""

    # Show build artifacts
    echo "📦 Build artifacts:"
    echo "==================="

    # Look in the expected output directories
    OUTPUT_DIRS=(
        "../Release"
        "Release"
        "../../Release"
    )

    FOUND_ARTIFACTS=false
    for dir in "${OUTPUT_DIRS[@]}"; do
        if [ -d "$dir" ]; then
            echo "🔍 Checking $dir..."
            ARTIFACTS=$(find "$dir" -name "*.app" -o -name "*.appex" 2>/dev/null | head -5)
            if [ -n "$ARTIFACTS" ]; then
                echo "$ARTIFACTS"
                FOUND_ARTIFACTS=true
                break
            fi
        fi
    done

    if [ "$FOUND_ARTIFACTS" = false ]; then
        print_warning "⚠️  No app artifacts found in expected locations"
        echo "🔍 Searching for build outputs..."
        find ../../.. -name "*.app" -o -name "*.appex" 2>/dev/null | head -5
    fi

    echo ""
    print_status "📁 All iOS builds are organized in: Builds/iOS/"

    if [ "$BUILD_TARGET" = "simulator" ]; then
        echo ""
        print_status "🔧 CLion iOS Simulator Setup:"
        echo "  ⚠️  IMPORTANT: iOS simulator builds cannot run directly in CLion!"
        echo ""
        print_status "  For CLion debugging, create a macOS target instead:"
        echo "  1. In CLion: File → New CMake Application Configuration"
        echo "  2. Set Target: OTTO_Standalone (macOS version)"
        echo "  3. Set Executable: path/to/macOS/OTTO.app/Contents/MacOS/OTTO"
        echo ""
        print_status "  To test iOS build, use Xcode iOS Simulator:"
        echo "  1. Open the generated .xcodeproj in Xcode"
        echo "  2. Select iOS Simulator target"
        echo "  3. Run with ⌘+R"
        echo ""
        print_status "  Or use command line simulator:"
        echo "     xcrun simctl boot \"iPhone 15\""
        echo "     xcrun simctl install booted \"/path/to/OTTO.app\""
        echo "     xcrun simctl launch booted com.yourcompany.OTTO"
    fi

    echo ""
    print_status "✨ iOS build completed successfully!"
else
    print_error "❌ iOS build failed!"
    exit 1
fi

echo ""
print_status "💡 Usage Tips:"
echo "  Simulator: $0 --simulator --team-id YOUR_TEAM_ID"
echo "  Device:    $0 --device --team-id YOUR_TEAM_ID"
echo "  Env var:   export APPLE_TEAM_ID=YOUR_TEAM_ID"
echo ""
print_status "💡 CLion Development Tips:"
echo "  - Use macOS target for CLion debugging and development"
echo "  - Use iOS builds only for final testing in Xcode/Simulator"
echo "  - JUCE 8 apps work similarly across macOS/iOS for audio/MIDI"

if [ "$TEAM_ID" = "YOUR_TEAM_ID" ]; then
    print_warning "⚠️  Remember to set your Apple Developer Team ID!"
    echo "   Get it from: https://developer.apple.com/account/"
fi
