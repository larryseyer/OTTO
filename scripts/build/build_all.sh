#!/bin/bash

# Master Build Script for OTTO - All Platforms
echo "OTTO Multi-Platform Build Script"
echo "================================="

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

# Get the directory of this script and change to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
cd "$PROJECT_ROOT"

# Check if we're on macOS for iOS/macOS builds
if [[ "$OSTYPE" == "darwin"* ]]; then
    CAN_BUILD_APPLE=true
else
    CAN_BUILD_APPLE=false
fi

# Parse command line arguments
BUILD_MACOS=false
BUILD_IOS=false
BUILD_WINDOWS=false
BUILD_LINUX=false
BUILD_ANDROID=false
BUILD_ALL=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --macos)
            BUILD_MACOS=true
            shift
            ;;
        --ios)
            BUILD_IOS=true
            shift
            ;;
        --windows)
            BUILD_WINDOWS=true
            shift
            ;;
        --linux)
            BUILD_LINUX=true
            shift
            ;;
        --android)
            BUILD_ANDROID=true
            shift
            ;;
        --all)
            BUILD_ALL=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--macos] [--ios] [--windows] [--linux] [--android] [--all]"
            exit 1
            ;;
    esac
done

# If no specific platform was chosen, show help
if [ "$BUILD_ALL" = false ] && [ "$BUILD_MACOS" = false ] && [ "$BUILD_IOS" = false ] && [ "$BUILD_WINDOWS" = false ] && [ "$BUILD_LINUX" = false ] && [ "$BUILD_ANDROID" = false ]; then
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  --macos     Build for macOS (Standalone, VST3, AU, CLAP)"
    echo "  --ios       Build for iOS (Standalone, AUv3)"
    echo "  --windows   Build for Windows (Standalone, VST3, CLAP)"
    echo "  --linux     Build for Linux (Standalone, VST3, CLAP, LV2)"
    echo "  --android   Build for Android (Standalone)"
    echo "  --all       Build for all supported platforms"
    echo ""
    echo "Examples:"
    echo "  $0 --macos --ios    # Build for Apple platforms"
    echo "  $0 --all            # Build for all platforms"
    exit 0
fi

# Set BUILD_ALL flags
if [ "$BUILD_ALL" = true ]; then
    BUILD_LINUX=true
    BUILD_ANDROID=true
    if [ "$CAN_BUILD_APPLE" = true ]; then
        BUILD_MACOS=true
        BUILD_IOS=true
    fi
    # Note: Windows cross-compilation from macOS/Linux is complex, so we skip it in --all
    print_warning "Note: Windows build skipped in --all mode. Use --windows specifically if needed."
fi

# Build for macOS
if [ "$BUILD_MACOS" = true ]; then
    if [ "$CAN_BUILD_APPLE" = true ]; then
        print_status "Building for macOS..."
        chmod +x scripts/build/build_macos.sh
        ./scripts/build/build_macos.sh
        if [ $? -eq 0 ]; then
            print_status "✓ macOS build successful"
        else
            print_error "✗ macOS build failed"
        fi
    else
        print_error "Cannot build for macOS on this platform"
    fi
    echo ""
fi

# Build for iOS
if [ "$BUILD_IOS" = true ]; then
    if [ "$CAN_BUILD_APPLE" = true ]; then
        print_status "Building for iOS..."
        chmod +x scripts/build/build_ios.sh
        ./scripts/build/build_ios.sh
        if [ $? -eq 0 ]; then
            print_status "✓ iOS build successful"
        else
            print_error "✗ iOS build failed"
        fi
    else
        print_error "Cannot build for iOS on this platform"
    fi
    echo ""
fi

# Build for Windows
if [ "$BUILD_WINDOWS" = true ]; then
    print_status "Building for Windows..."
    if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
        cmd //c scripts/build/build_windows.bat
    else
        print_warning "Cross-compiling for Windows from $OSTYPE..."
        # This would require MinGW-w64 or similar setup
        mkdir -p Builds/VisualStudio2022/CMake-Cross
        cd Builds/VisualStudio2022/CMake-Cross
        cmake "$PROJECT_ROOT" \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_TOOLCHAIN_FILE="$PROJECT_ROOT/cmake/mingw-w64-toolchain.cmake"
        cmake --build . --config Release --parallel $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
        cd "$PROJECT_ROOT"
    fi
    if [ $? -eq 0 ]; then
        print_status "✓ Windows build successful"
    else
        print_error "✗ Windows build failed"
    fi
    echo ""
fi

# Build for Linux
if [ "$BUILD_LINUX" = true ]; then
    print_status "Building for Linux..."
    chmod +x scripts/build/build_linux.sh
    ./scripts/build/build_linux.sh
    if [ $? -eq 0 ]; then
        print_status "✓ Linux build successful"
    else
        print_error "✗ Linux build failed"
    fi
    echo ""
fi

# Build for Android
if [ "$BUILD_ANDROID" = true ]; then
    print_status "Building for Android..."
    chmod +x scripts/build/build_android.sh
    ./scripts/build/build_android.sh
    if [ $? -eq 0 ]; then
        print_status "✓ Android build successful"
    else
        print_error "✗ Android build failed"
    fi
    echo ""
fi

print_status "Build process complete!"
print_status "📁 All build outputs are now organized in the Builds/ directory:"
echo ""
echo "Build outputs by platform:"
[ -d "Builds/MacOSX/Release" ] && echo "  📁 macOS: Builds/MacOSX/ (Standalone, VST3, AU, CLAP)"
[ -d "Builds/iOS/Release" ] && echo "  📁 iOS: Builds/iOS/ (Standalone, AUv3)"
[ -d "Builds/Linux/Release" ] && echo "  📁 Linux: Builds/Linux/ (Standalone, VST3, CLAP, LV2)"
[ -d "Builds/Android/Release" ] && echo "  📁 Android: Builds/Android/ (Standalone)"
[ -d "Builds/VisualStudio2022/Release" ] && echo "  📁 Windows: Builds/VisualStudio2022/ (Standalone, VST3, CLAP)"
