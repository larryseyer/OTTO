#!/bin/bash

# Linux Build Script for OTTO (JUCE 8)
# Unified CMake build system that matches ProJucer configuration

set -e

echo "🐧 Building OTTO for Linux..."
echo "============================="

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

# Check for required dependencies and install if needed
print_status "🔍 Checking dependencies..."

MISSING_DEPS=()

# Check for essential build tools
if ! command -v cmake &> /dev/null; then
    MISSING_DEPS+=("cmake")
fi

if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    MISSING_DEPS+=("build-essential")
fi

# Check for JUCE dependencies
if ! pkg-config --exists alsa; then
    MISSING_DEPS+=("libasound2-dev")
fi

if ! pkg-config --exists gtk+-3.0; then
    MISSING_DEPS+=("libgtk-3-dev")
fi

if ! pkg-config --exists freetype2; then
    MISSING_DEPS+=("libfreetype6-dev")
fi

if ! pkg-config --exists x11; then
    MISSING_DEPS+=("libx11-dev")
fi

# Install missing dependencies
if [ ${#MISSING_DEPS[@]} -gt 0 ]; then
    print_warning "📦 Missing dependencies: ${MISSING_DEPS[*]}"

    if command -v apt-get &> /dev/null; then
        print_status "📥 Installing dependencies with apt-get..."
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            libasound2-dev \
            libgtk-3-dev \
            libwebkit2gtk-4.0-dev \
            libfreetype6-dev \
            libcurl4-openssl-dev \
            libx11-dev \
            libxext-dev \
            libxrandr-dev \
            libxinerama-dev \
            libxcursor-dev \
            libjack-jackd2-dev \
            lv2-dev \
            pkg-config
    elif command -v yum &> /dev/null; then
        print_status "📥 Installing dependencies with yum..."
        sudo yum install -y \
            gcc-c++ \
            cmake \
            alsa-lib-devel \
            gtk3-devel \
            webkit2gtk3-devel \
            freetype-devel \
            libcurl-devel \
            libX11-devel \
            libXext-devel \
            libXrandr-devel \
            libXinerama-devel \
            libXcursor-devel \
            jack-audio-connection-kit-devel \
            lv2-devel \
            pkgconfig
    elif command -v pacman &> /dev/null; then
        print_status "📥 Installing dependencies with pacman..."
        sudo pacman -S --needed \
            gcc \
            cmake \
            alsa-lib \
            gtk3 \
            webkit2gtk \
            freetype2 \
            curl \
            libx11 \
            libxext \
            libxrandr \
            libxinerama \
            libxcursor \
            jack \
            lv2 \
            pkgconf
    else
        print_error "❌ Unsupported package manager. Please install dependencies manually:"
        echo "  - build-essential/gcc-c++/gcc"
        echo "  - cmake"
        echo "  - alsa development headers"
        echo "  - GTK3 development headers"
        echo "  - freetype development headers"
        echo "  - X11 development headers"
        echo "  - JACK development headers"
        echo "  - LV2 development headers"
        exit 1
    fi
else
    print_status "✅ All dependencies are installed"
fi

# Get the directory of this script and change to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
cd "$PROJECT_ROOT"

# Create build directory in proper unified structure
BUILD_DIR="Builds/LinuxMakefile/CMake-Release"
mkdir -p "$BUILD_DIR"

# Clean up any previous CMake cache
rm -rf "$BUILD_DIR/CMakeCache.txt" "$BUILD_DIR/CMakeFiles"

print_status "📁 Build directory: $BUILD_DIR"

cd "$BUILD_DIR"

print_status "⚙️  Configuring CMake..."

# Configure with CMake using unified settings
cmake "$PROJECT_ROOT" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-fPIC -O3" \
    -DJUCE_BUILD_EXAMPLES=OFF \
    -DJUCE_BUILD_EXTRAS=OFF \
    -DJUCE_COPY_PLUGIN_AFTER_BUILD=TRUE \
    -G "Unix Makefiles"

if [ $? -ne 0 ]; then
    print_error "❌ CMake configuration failed!"
    exit 1
fi

print_status "🔧 Building OTTO..."

# Build with proper parallel processing
NUM_CORES=$(nproc)
print_status "🚀 Starting build with $NUM_CORES parallel jobs..."

cmake --build . --config Release --parallel $NUM_CORES

# Check build result
if [ $? -eq 0 ]; then
    print_status "✅ Linux build complete!"
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
            ARTIFACTS=$(find "$dir" -name "*.so" -o -name "OTTO" -o -name "*.vst3" -o -name "*.clap" -o -name "*.lv2" 2>/dev/null | head -10)
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
        find "$PROJECT_ROOT" -name "*.so" -o -name "OTTO" -o -name "*.vst3" -o -name "*.clap" -o -name "*.lv2" 2>/dev/null | head -10
    fi

    echo ""
    print_status "📁 All Linux builds are organized in: Builds/LinuxMakefile/"
    echo ""
    print_status "✨ Linux build completed successfully!"

    # Show installation instructions
    echo ""
    print_status "💡 Installation Tips:"
    echo "  VST3 plugins: ~/.vst3/ or /usr/lib/vst3/"
    echo "  CLAP plugins: ~/.clap/ or /usr/lib/clap/"
    echo "  LV2 plugins:  ~/.lv2/ or /usr/lib/lv2/"
    echo "  Standalone:   Run directly from build directory"

else
    print_error "❌ Linux build failed!"
    exit 1
fi
