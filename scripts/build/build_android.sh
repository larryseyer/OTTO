#!/bin/bash

# Android Build Script for OTTO (JUCE 8)
# Unified CMake build system that matches ProJucer configuration

set -e

echo "🤖 Building OTTO for Android..."
echo "==============================="

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

# Check if Android NDK is available
if [ -z "$ANDROID_NDK_ROOT" ]; then
    print_error "❌ ANDROID_NDK_ROOT environment variable not set"
    echo "📱 Please install Android NDK and set the path:"
    echo "   export ANDROID_NDK_ROOT=/path/to/android-ndk"
    echo "   Download from: https://developer.android.com/ndk/downloads"
    exit 1
fi

# Check if Android SDK is available
if [ -z "$ANDROID_HOME" ]; then
    print_error "❌ ANDROID_HOME environment variable not set"
    echo "📱 Please install Android SDK and set the path:"
    echo "   export ANDROID_HOME=/path/to/android-sdk"
    echo "   Download from: https://developer.android.com/studio"
    exit 1
fi

print_status "✅ Android NDK: $ANDROID_NDK_ROOT"
print_status "✅ Android SDK: $ANDROID_HOME"

# Parse command line arguments for architecture
ANDROID_ABI="arm64-v8a"
ANDROID_API="23"

while [[ $# -gt 0 ]]; do
    case $1 in
        --abi)
            ANDROID_ABI="$2"
            shift 2
            ;;
        --api)
            ANDROID_API="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--abi arm64-v8a|armeabi-v7a|x86|x86_64] [--api API_LEVEL]"
            exit 1
            ;;
    esac
done

print_status "🏗️  Architecture: $ANDROID_ABI"
print_status "📱 API Level: $ANDROID_API"

# Get the directory of this script and change to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
cd "$PROJECT_ROOT"

# Create build directory in proper unified structure
BUILD_DIR="Builds/Android/CMake-Release-$ANDROID_ABI"
mkdir -p "$BUILD_DIR"

# Clean up any previous CMake cache
rm -rf "$BUILD_DIR/CMakeCache.txt" "$BUILD_DIR/CMakeFiles"

print_status "📁 Build directory: $BUILD_DIR"

cd "$BUILD_DIR"

print_status "⚙️  Configuring CMake for Android..."

# Configure with CMake for Android using unified settings
cmake "$PROJECT_ROOT" \
   -DCMAKE_BUILD_TYPE=Release \
   -DCMAKE_SYSTEM_NAME=Android \
   -DCMAKE_ANDROID_NDK="$ANDROID_NDK_ROOT" \
   -DCMAKE_ANDROID_ARCH_ABI="$ANDROID_ABI" \
   -DCMAKE_ANDROID_API="$ANDROID_API" \
   -DCMAKE_ANDROID_STL_TYPE=c++_shared \
   -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang \
   -DJUCE_BUILD_EXAMPLES=OFF \
   -DJUCE_BUILD_EXTRAS=OFF \
   -DJUCE_ENABLE_MODULE_SOURCE_GROUPS=ON \
   -G "Unix Makefiles"

if [ $? -ne 0 ]; then
   print_error "❌ CMake configuration failed!"
   exit 1
fi

print_status "🔧 Building OTTO..."

# Build with proper parallel processing
if command -v nproc &> /dev/null; then
   NUM_CORES=$(nproc)
elif command -v sysctl &> /dev/null; then
   NUM_CORES=$(sysctl -n hw.ncpu)
else
   NUM_CORES=4
fi

print_status "🚀 Starting build with $NUM_CORES parallel jobs..."

cmake --build . --config Release --parallel $NUM_CORES

# Check build result
if [ $? -eq 0 ]; then
   print_status "✅ Android build complete!"
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
           ARTIFACTS=$(find "$dir" -name "*.so" -o -name "*.apk" -o -name "libOTTO.so" 2>/dev/null | head -10)
           if [ -n "$ARTIFACTS" ]; then
               echo "$ARTIFACTS"
               FOUND_ARTIFACTS=true
               break
           fi
       fi
   done

   if [ "$FOUND_ARTIFACTS" = false ]; then
       print_warning "⚠️  No Android artifacts found in expected locations"
       echo "🔍 Searching for build outputs..."
       find "$PROJECT_ROOT" -name "*.so" -o -name "*.apk" 2>/dev/null | head -10
   fi

   echo ""
   print_status "📁 All Android builds are organized in: Builds/Android/"
   echo ""
   print_status "💡 Next Steps:"
   echo "  1. Copy .so files to your Android app's jniLibs/$ANDROID_ABI/ folder"
   echo "  2. Or use the ProJucer Android exporter for full app generation"
   echo ""
   print_status "✨ Android build completed successfully!"

else
   print_error "❌ Android build failed!"
   echo ""
   echo "📋 Troubleshooting tips:"
   echo "  - Ensure Android NDK and SDK are properly installed"
   echo "  - Check that API level $ANDROID_API is available"
   echo "  - Try different architecture: --abi armeabi-v7a"
   exit 1
fi

