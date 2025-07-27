#!/bin/bash

# OTTO JUCE 8 Multi-Platform Build Script
# Builds for Mac OSX Standalone, VST3, AUv3, CLAP AU, Windows, Linux, iOS, and Android

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Build configuration
BUILD_TYPE="${1:-Release}"
PROJECT_DIR=$(pwd)
BUILD_BASE_DIR="$PROJECT_DIR/Builds"

echo -e "${BLUE}OTTO JUCE 8 Multi-Platform Build Script${NC}"
echo "======================================"
echo "Build Type: $BUILD_TYPE"
echo "Project Directory: $PROJECT_DIR"
echo "Build Base Directory: $BUILD_BASE_DIR"
echo ""

# Function to print status
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to build for a specific platform
build_platform() {
    local platform=$1
    local cmake_args=$2
    local build_dir="$BUILD_BASE_DIR/$platform"
    
    print_status "Building for $platform..."
    
    # Create build directory
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Configure with CMake
    print_status "Configuring CMake for $platform..."
    cmake "$PROJECT_DIR" \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        $cmake_args
    
    # Build
    print_status "Compiling for $platform..."
    cmake --build . --config "$BUILD_TYPE" --parallel $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    print_status "$platform build completed successfully!"
    cd "$PROJECT_DIR"
}

# Check if we're on macOS
if [[ "$OSTYPE" == "darwin"* ]]; then
    print_status "Detected macOS - Building native targets"
    
    # macOS Standalone, VST3, AU, CLAP
    build_platform "macOS" "-DCMAKE_OSX_ARCHITECTURES=\"arm64;x86_64\" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13"
    
    # iOS (requires iOS development environment)
    if xcodebuild -showsdks | grep -q iphoneos; then
        print_status "iOS SDK detected - Building iOS targets"
        
        # iOS Device
        build_platform "iOS_Device" "-DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphoneos -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0"
        
        # iOS Simulator
        build_platform "iOS_Simulator" "-DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphonesimulator -DCMAKE_OSX_ARCHITECTURES=\"arm64;x86_64\" -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0"
    else
        print_warning "iOS SDK not found - skipping iOS builds"
    fi
    
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    print_status "Detected Linux - Building Linux targets"
    
    # Linux Standalone, VST3, CLAP
    build_platform "Linux" "-DCMAKE_CXX_FLAGS=\"-fPIC\""
    
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
    print_status "Detected Windows - Building Windows targets"
    
    # Windows Standalone, VST3, CLAP
    build_platform "Windows" "-G \"Visual Studio 17 2022\" -A x64"
    
else
    print_error "Unsupported operating system: $OSTYPE"
    exit 1
fi

# Cross-compilation builds (advanced)
if command -v docker &> /dev/null; then
    print_status "Docker detected - offering cross-compilation builds"
    
    # Android build (requires Android NDK)
    if [ -n "$ANDROID_NDK_ROOT" ]; then
        print_status "Android NDK detected - Building Android targets"
        build_platform "Android" "-DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a -DCMAKE_ANDROID_NDK=$ANDROID_NDK_ROOT -DCMAKE_ANDROID_STL_TYPE=c++_shared -DCMAKE_ANDROID_API=23"
    else
        print_warning "Android NDK not found (set ANDROID_NDK_ROOT) - skipping Android builds"
    fi
    
    # Windows cross-compilation from Linux/macOS (requires MinGW)
    if [[ "$OSTYPE" != "msys" && "$OSTYPE" != "cygwin" && "$OSTYPE" != "win32" ]]; then
        if command -v x86_64-w64-mingw32-gcc &> /dev/null; then
            print_status "MinGW detected - Building Windows targets via cross-compilation"
            build_platform "Windows_CrossCompile" "-DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++"
        else
            print_warning "MinGW not found - skipping Windows cross-compilation"
        fi
    fi
else
    print_warning "Docker not found - skipping advanced cross-compilation options"
fi

# Build summary
echo ""
echo -e "${BLUE}Build Summary${NC}"
echo "============="

successful_builds=0
total_builds=0

for build_dir in "$BUILD_BASE_DIR"/*; do
    if [ -d "$build_dir" ]; then
        platform=$(basename "$build_dir")
        total_builds=$((total_builds + 1))
        
        # Check if build was successful (look for compiled artifacts)
        if find "$build_dir" -name "OTTO*" -type f | grep -E '\.(app|vst3|component|clap|exe|so|dylib)$' > /dev/null 2>&1; then
            print_status "$platform: SUCCESS"
            successful_builds=$((successful_builds + 1))
        else
            print_error "$platform: FAILED"
        fi
    fi
done

echo ""
echo -e "${GREEN}Completed: $successful_builds/$total_builds builds successful${NC}"

# Installation notes
echo ""
echo -e "${BLUE}Installation Notes${NC}"
echo "=================="
echo "macOS:"
echo "  - Standalone: $BUILD_BASE_DIR/macOS/OTTO_artefacts/$BUILD_TYPE/Standalone/OTTO.app"
echo "  - VST3: $BUILD_BASE_DIR/macOS/OTTO_artefacts/$BUILD_TYPE/VST3/OTTO.vst3"
echo "  - AU: $BUILD_BASE_DIR/macOS/OTTO_artefacts/$BUILD_TYPE/AU/OTTO.component"
echo "  - CLAP: $BUILD_BASE_DIR/macOS/OTTO_artefacts/$BUILD_TYPE/CLAP/OTTO.clap"
echo ""
echo "iOS:"
echo "  - AUv3: $BUILD_BASE_DIR/iOS_Device/OTTO_artefacts/$BUILD_TYPE/AUv3/OTTO.appex"
echo ""
echo "Windows:"
echo "  - Standalone: $BUILD_BASE_DIR/Windows/OTTO_artefacts/$BUILD_TYPE/Standalone/OTTO.exe"
echo "  - VST3: $BUILD_BASE_DIR/Windows/OTTO_artefacts/$BUILD_TYPE/VST3/OTTO.vst3"
echo ""
echo "Linux:"
echo "  - Standalone: $BUILD_BASE_DIR/Linux/OTTO_artefacts/$BUILD_TYPE/Standalone/OTTO"
echo "  - VST3: $BUILD_BASE_DIR/Linux/OTTO_artefacts/$BUILD_TYPE/VST3/OTTO.vst3"
echo ""
echo "Android:"
echo "  - APK: $BUILD_BASE_DIR/Android/OTTO_artefacts/$BUILD_TYPE/OTTO.apk"
echo ""

echo -e "${GREEN}Multi-platform build process completed!${NC}"

# Additional platform-specific instructions
echo ""
echo -e "${BLUE}Platform-Specific Instructions${NC}"
echo "==============================="
echo ""
echo "macOS Plugin Installation:"
echo "  sudo cp -R $BUILD_BASE_DIR/macOS/OTTO_artefacts/$BUILD_TYPE/VST3/OTTO.vst3 /Library/Audio/Plug-Ins/VST3/"
echo "  sudo cp -R $BUILD_BASE_DIR/macOS/OTTO_artefacts/$BUILD_TYPE/AU/OTTO.component /Library/Audio/Plug-Ins/Components/"
echo "  sudo cp -R $BUILD_BASE_DIR/macOS/OTTO_artefacts/$BUILD_TYPE/CLAP/OTTO.clap /Library/Audio/Plug-Ins/CLAP/"
echo ""
echo "Windows Plugin Installation:"
echo "  Copy OTTO.vst3 to: C:\\Program Files\\Common Files\\VST3\\"
echo "  Copy OTTO.clap to: C:\\Program Files\\Common Files\\CLAP\\"
echo ""
echo "Linux Plugin Installation:"
echo "  cp -R OTTO.vst3 ~/.vst3/"
echo "  cp -R OTTO.clap ~/.clap/"
echo ""
echo "iOS Installation:"
echo "  Install via Xcode or TestFlight for AUv3 extension"
echo ""
echo "Android Installation:"
echo "  adb install OTTO.apk"
echo ""

exit 0