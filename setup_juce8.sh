#!/bin/bash

# JUCE 8 Setup Script for OTTO
echo "Setting up JUCE 8 for OTTO development..."

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we have git
if ! command -v git &> /dev/null; then
    print_error "Git is required but not installed. Please install Git first."
    exit 1
fi

# Check if we have CMake
if ! command -v cmake &> /dev/null; then
    print_error "CMake is required but not installed."
    if [[ "$OSTYPE" == "darwin"* ]]; then
        print_status "You can install CMake via Homebrew: brew install cmake"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        print_status "You can install CMake via apt: sudo apt-get install cmake"
    fi
    exit 1
fi

# Check CMake version
CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
CMAKE_MAJOR=$(echo $CMAKE_VERSION | cut -d'.' -f1)
CMAKE_MINOR=$(echo $CMAKE_VERSION | cut -d'.' -f2)

if [ $CMAKE_MAJOR -lt 3 ] || ([ $CMAKE_MAJOR -eq 3 ] && [ $CMAKE_MINOR -lt 22 ]); then
    print_error "CMake 3.22+ is required for JUCE 8. Current version: $CMAKE_VERSION"
    exit 1
fi

print_status "CMake version $CMAKE_VERSION is compatible with JUCE 8"

# Check if JUCE directory exists
if [ -d "JUCE" ]; then
    print_warning "JUCE directory already exists. Checking if it's JUCE 8..."
    cd JUCE
    if git log -1 --format="%H" | grep -q ""; then
        print_status "JUCE directory found, pulling latest changes..."
        git pull
    else
        print_warning "JUCE directory exists but may not be a git repository"
    fi
    cd ..
else
    print_status "Cloning JUCE 8 from GitHub..."
    git clone https://github.com/juce-framework/JUCE.git
    
    if [ $? -ne 0 ]; then
        print_error "Failed to clone JUCE repository"
        exit 1
    fi
fi

# Check JUCE version
cd JUCE
JUCE_VERSION_FILE="modules/juce_core/juce_core.h"
if [ -f "$JUCE_VERSION_FILE" ]; then
    JUCE_MAJOR=$(grep "JUCE_MAJOR_VERSION" $JUCE_VERSION_FILE | head -1 | awk '{print $3}')
    JUCE_MINOR=$(grep "JUCE_MINOR_VERSION" $JUCE_VERSION_FILE | head -1 | awk '{print $3}')
    print_status "JUCE version detected: $JUCE_MAJOR.$JUCE_MINOR"
    
    if [ $JUCE_MAJOR -lt 8 ]; then
        print_warning "JUCE version is older than 8. Checking out latest stable release..."
        git checkout master
        git pull
    fi
else
    print_warning "Could not detect JUCE version from headers"
fi
cd ..

# Platform-specific dependency installation
if [[ "$OSTYPE" == "darwin"* ]]; then
    print_status "Detected macOS. Checking for Xcode..."
    if ! xcode-select -p &> /dev/null; then
        print_error "Xcode Command Line Tools not found. Please install with: xcode-select --install"
        exit 1
    fi
    
    # Check for Homebrew
    if command -v brew &> /dev/null; then
        print_status "Installing additional dependencies via Homebrew..."
        brew install pkg-config
    else
        print_warning "Homebrew not found. Some dependencies may need manual installation."
    fi
    
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    print_status "Detected Linux. Installing dependencies..."
    
    # Detect package manager
    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            libasound2-dev \
            libgtk-3-dev \
            libwebkit2gtk-4.0-dev \
            libfreetype6-dev \
            libcurl4-openssl-dev \
            pkg-config \
            libjack-jackd2-dev
    elif command -v yum &> /dev/null; then
        sudo yum groupinstall -y "Development Tools"
        sudo yum install -y \
            alsa-lib-devel \
            gtk3-devel \
            webkit2gtk3-devel \
            freetype-devel \
            libcurl-devel \
            pkgconfig \
            jack-audio-connection-kit-devel
    elif command -v pacman &> /dev/null; then
        sudo pacman -S --needed \
            base-devel \
            alsa-lib \
            gtk3 \
            webkit2gtk \
            freetype2 \
            curl \
            pkgconf \
            jack
    else
        print_warning "Unknown package manager. Please install dependencies manually."
    fi
fi

# Create cmake directory if it doesn't exist
mkdir -p cmake

print_status "JUCE 8 setup complete!"
print_status "You can now build OTTO using the platform-specific build scripts:"
print_status "  - ./build_macos.sh    (macOS)"
print_status "  - ./build_ios.sh      (iOS)"
print_status "  - ./build_linux.sh    (Linux)"
print_status "  - ./build_android.sh  (Android - requires Android NDK/SDK)"
print_status "  - ./build_all.sh --all (All supported platforms)"

# Make build scripts executable
chmod +x build_*.sh

print_status "All build scripts are now executable."