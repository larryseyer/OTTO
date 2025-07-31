#!/bin/bash

# macOS Build Script for OTTO (JUCE 8)
# Unified CMake build system that matches ProJucer configuration

set -e

echo "🔨 Building OTTO for macOS..."
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

# Get the directory of this script and change to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
cd "$PROJECT_ROOT"

# Create build directory in proper structure
BUILD_DIR="Builds/MacOSX/CMake-Release"
mkdir -p "$BUILD_DIR"

# Clean up any previous CMake cache
rm -rf "$BUILD_DIR/CMakeCache.txt" "$BUILD_DIR/CMakeFiles"

print_status "📁 Build directory: $BUILD_DIR"

cd "$BUILD_DIR"

print_status "⚙️  Configuring CMake..."

# Configure with CMake using unified settings
cmake "$PROJECT_ROOT" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 \
    -DJUCE_BUILD_EXAMPLES=OFF \
    -DJUCE_BUILD_EXTRAS=OFF \
    -DJUCE_COPY_PLUGIN_AFTER_BUILD=TRUE \
    -DJUCE_VST3_CAN_REPLACE_VST2=ON \
    -G Xcode

if [ $? -ne 0 ]; then
    print_error "❌ CMake configuration failed!"
    exit 1
fi

print_status "🔧 Building OTTO..."

# Build with proper parallel processing
NUM_CORES=$(sysctl -n hw.ncpu)
cmake --build . --config Release --parallel $NUM_CORES

# Check build result
if [ $? -eq 0 ]; then
    print_status "✅ macOS build complete!"
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
            ARTIFACTS=$(find "$dir" -name "*.app" -o -name "*.vst3" -o -name "*.component" -o -name "*.clap" 2>/dev/null | head -10)
            if [ -n "$ARTIFACTS" ]; then
                echo "$ARTIFACTS"
                FOUND_ARTIFACTS=true
                break
            fi
        fi
    done

    if [ "$FOUND_ARTIFACTS" = false ]; then
        print_warning "⚠️  No plugin artifacts found in expected locations"
        echo "🔍 Searching for build outputs..."
        find "$PROJECT_ROOT" -name "*.app" -o -name "*.vst3" -o -name "*.component" -o -name "*.clap" 2>/dev/null | head -10
    fi

    echo ""
    print_status "📁 All macOS builds are organized in: Builds/MacOSX/"
    echo ""
    print_status "💡 Installation Tips:"
    echo "  VST3 plugins: ~/Library/Audio/Plug-Ins/VST3/"
    echo "  AU plugins:   ~/Library/Audio/Plug-Ins/Components/"
    echo "  CLAP plugins: ~/Library/Audio/Plug-Ins/CLAP/"
    echo "  Standalone:   Run directly from build directory"
    echo ""
    print_status "✨ macOS build completed successfully!"
else
    print_error "❌ macOS build failed!"
    exit 1
fi
