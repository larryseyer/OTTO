# OTTO Build Rules & Cross-Platform Instructions

This document provides comprehensive build instructions for OTTO across all 5 supported platforms using the unified CMake build system.

## 🏗️ Build System Overview

OTTO uses a **unified CMake build system** that ensures identical results across all platforms:

- **Single CMakeLists.txt**: One configuration file for all platforms
- **Consistent Output**: Identical plugin behavior regardless of build method
- **Organized Structure**: All builds output to standardized `/Builds` directory
- **Multiple Workflow Support**: Works with ProJucer+Xcode OR CMake+CLion
- **Automated Scripts**: Platform-specific build scripts with error handling

## 📋 Prerequisites

### Core Requirements (All Platforms)
- **JUCE 8.0.8+**: Audio framework (included as submodule)
- **CMake 3.22+**: Build system
- **C++20 Compiler**: Platform-specific toolchain
- **Git**: For submodule management

### Platform-Specific Requirements

#### macOS
- **Xcode 14.0+**: Development environment
- **macOS 10.13+**: Minimum deployment target
- **Apple Developer Account**: For code signing (optional for development)

#### Windows
- **Visual Studio 2022**: Community, Professional, or Enterprise
- **Windows SDK 10.0+**: Platform SDK
- **Windows 10/11**: Target platform

#### Linux
- **GCC 11+** or **Clang 12+**: C++ compiler
- **Development Libraries**: Auto-installed by build script
  - ALSA development headers
  - GTK3 development headers
  - FreeType2 development headers
  - X11 development headers

#### iOS
- **Xcode 14.0+**: iOS development tools
- **iOS SDK 15.0+**: Target SDK
- **Apple Developer Account**: Required for device deployment

#### Android
- **Android Studio**: Development environment
- **Android NDK 25+**: Native development kit
- **Android SDK API 23+**: Target API level
- **Environment Variables**: ANDROID_NDK_ROOT, ANDROID_HOME

## 🚀 Quick Start Builds

### Single Platform Builds

#### macOS (Universal Binary)
```bash
./scripts/build/build_macos.sh
```
**Output**: `Builds/MacOSX/Release/`
- `VST3/OTTO.vst3` - VST3 plugin
- `AU/OTTO.component` - Audio Unit plugin
- `Standalone/OTTO.app` - Standalone application

#### iOS (Device & Simulator)
```bash
# iOS Simulator (auto-detects Mac architecture)
./scripts/build/build_ios.sh --simulator --team-id YOUR_TEAM_ID

# iOS Device
./scripts/build/build_ios.sh --device --team-id YOUR_TEAM_ID
```
**Output**: `Builds/iOS/Release/`
- `Standalone/OTTO.app` - iOS application

#### Linux (Native)
```bash
./scripts/build/build_linux.sh
```
**Output**: `Builds/LinuxMakefile/Release/`
- `VST3/OTTO.vst3` - VST3 plugin
- `Standalone/OTTO` - Standalone application

#### Windows (x64)
```batch
scripts/build/build_windows.bat
```
**Output**: `Builds/VisualStudio2022/Release/`
- `VST3/OTTO.vst3` - VST3 plugin
- `Standalone/OTTO.exe` - Standalone application

#### Android (ARM64/ARM32)
```bash
# Set environment variables first
export ANDROID_NDK_ROOT=/path/to/ndk
export ANDROID_HOME=/path/to/sdk

# Build default (arm64-v8a, API 23)
./scripts/build/build_android.sh

# Custom architecture and API level
./scripts/build/build_android.sh --abi armeabi-v7a --api 28
```
**Output**: `Builds/Android/Release/`
- `libOTTO.so` - Shared library

### Multi-Platform Build
```bash
# Build all platforms
./scripts/build/build_all.sh --all

# Build specific platforms
./scripts/build/build_all.sh --macos --ios --linux
./scripts/build/build_all.sh --windows --android --debug
```

## 🔧 Detailed Build Instructions

### macOS Build Process

#### Prerequisites Setup
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install CMake via Homebrew
brew install cmake

# Clone repository with submodules
git clone --recursive https://github.com/AutomagicArt/OTTO.git
cd OTTO
```

#### Build Commands
```bash
# Release build (recommended)
./scripts/build/build_macos.sh --release

# Debug build
./scripts/build/build_macos.sh --debug

# With testing
./scripts/build/build_macos.sh --release --test

# Clean build
./scripts/build/build_macos.sh --clean --release
```

#### Manual CMake Build
```bash
# Configure
cmake --preset macos-release

# Build
cmake --build --preset macos-release

# Install plugins to system locations
cmake --build build-macos-release --target install
```

### iOS Build Process

#### Prerequisites Setup
```bash
# Get your Team ID
./scripts/setup/get_team_id.sh

# Or manually find it in Xcode:
# Xcode → Preferences → Accounts → Your Apple ID → Team ID
```

#### Build Commands
```bash
# iOS Device build
./scripts/build/build_ios.sh --device --team-id XXXXXXXXXX

# iOS Simulator build
./scripts/build/build_ios.sh --simulator --team-id XXXXXXXXXX

# Archive build for App Store
./scripts/build/build_ios.sh --device --archive --team-id XXXXXXXXXX
```

#### Code Signing
```bash
# The build script automatically handles:
# - Code signing identity detection
# - Provisioning profile selection
# - Universal binary creation (device + simulator)
# - Archive creation for distribution
```

### Windows Build Process

#### Prerequisites Setup
```batch
REM Install Visual Studio 2022 with C++ workload
REM Install CMake (available in Visual Studio Installer)
REM Clone repository
git clone --recursive https://github.com/AutomagicArt/OTTO.git
cd OTTO
```

#### Build Commands
```batch
REM Release build
build_windows.bat --release

REM Debug build
build_windows.bat --debug

REM Specific architecture
build_windows.bat --release --arch x64
build_windows.bat --release --arch Win32
```

#### Manual CMake Build
```batch
REM Configure
cmake --preset windows-release

REM Build
cmake --build --preset windows-release

REM Build specific configuration
cmake --build build-windows-release --config Release
```

### Linux Build Process

#### Prerequisites Setup
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake git

# CentOS/RHEL
sudo yum install gcc-c++ cmake git

# Arch Linux
sudo pacman -S base-devel cmake git

# Clone repository
git clone --recursive https://github.com/AutomagicArt/OTTO.git
cd OTTO
```

#### Build Commands
```bash
# The script auto-installs dependencies
./scripts/build/build_linux.sh --release

# Debug build
./scripts/build/build_linux.sh --debug

# Skip dependency installation
./scripts/build/build_linux.sh --release --skip-deps

# Specific generator
./scripts/build/build_linux.sh --release --generator "Unix Makefiles"
```

#### Docker Build (Alternative)
```bash
# Build using Docker container
./scripts/build/build_linux_docker.sh --release

# Development container with shell access
docker-compose -f docker/docker-compose.yml run otto-linux-dev bash
```

### Android Build Process

#### Prerequisites Setup
```bash
# Install Android Studio
# Install Android NDK via SDK Manager
# Set environment variables

export ANDROID_NDK_ROOT=$HOME/Android/Sdk/ndk/25.0.8775105
export ANDROID_HOME=$HOME/Android/Sdk

# Verify installation
$ANDROID_NDK_ROOT/ndk-build --version
```

#### Build Commands
```bash
# Default build (arm64-v8a, API 23)
./scripts/build/build_android.sh

# Specific architecture
./scripts/build/build_android.sh --abi arm64-v8a
./scripts/build/build_android.sh --abi armeabi-v7a
./scripts/build/build_android.sh --abi x86_64

# Specific API level
./scripts/build/build_android.sh --api 28

# Debug build
./scripts/build/build_android.sh --debug --abi arm64-v8a

# Clean build
./scripts/build/build_android.sh --clean --abi arm64-v8a
```

## 🎯 CMake Presets

OTTO includes comprehensive CMake presets for IDE integration:

### Available Presets
```bash
# macOS
cmake --preset macos-debug
cmake --preset macos-release

# iOS
cmake --preset ios-simulator-debug
cmake --preset ios-device-debug
cmake --preset ios-simulator-release
cmake --preset ios-device-release

# Windows
cmake --preset windows-debug
cmake --preset windows-release

# Linux
cmake --preset linux-debug
cmake --preset linux-release

# Android
cmake --preset android-arm64-debug
cmake --preset android-arm64-release
```

### Using Presets
```bash
# Configure with preset
cmake --preset macos-release

# Build with preset
cmake --build --preset macos-release

# List all presets
cmake --list-presets
```

## 🔍 Build Verification

### Automated Validation
```bash
# Validate all platforms (run after any changes)
./scripts/validate_all_platforms.sh

# Expected output: All platforms ✅ PASS
```

### Manual Testing
```bash
# Test specific platform build
./test_build.sh --platform macos
./test_build.sh --platform ios --simulator
./test_build.sh --platform linux
./test_build.sh --platform windows
./test_build.sh --platform android
```

### Build Artifacts Verification
```bash
# Check macOS plugins
otool -L Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/MacOS/OTTO
codesign -dv Builds/MacOSX/Release/AU/OTTO.component

# Check iOS app
otool -L Builds/iOS/Release/Standalone/OTTO.app/OTTO
codesign -dv Builds/iOS/Release/Standalone/OTTO.app

# Check Linux plugins
ldd Builds/LinuxMakefile/Release/VST3/OTTO.vst3/Contents/x86_64-linux/OTTO.so

# Check Windows plugins
dumpbin /dependents Builds/VisualStudio2022/Release/VST3/OTTO.vst3/Contents/x86_64-win/OTTO.vst3

# Check Android library
readelf -d Builds/Android/Release/lib/arm64-v8a/libOTTO.so
```

## 🐛 Troubleshooting

### Common Build Issues

#### macOS Issues
```bash
# Xcode not found
sudo xcode-select -s /Applications/Xcode.app/Contents/Developer

# CMake not found
brew install cmake

# Code signing errors
# Check Team ID with: ./get_team_id.sh
# Update Apple Developer certificates in Keychain Access
```

#### iOS Issues
```bash
# Simulator architecture issues
# Solution: Build script auto-detects Mac architecture (Intel/Apple Silicon)

# Code signing failures
# Solution: Ensure valid Apple Developer account and certificates
# Run: security find-identity -v -p codesigning
```

#### Windows Issues
```batch
REM Visual Studio not detected
REM Solution: Install Visual Studio 2022 with C++ workload
REM Or specify generator: --generator "Visual Studio 17 2022"

REM CMake not found
REM Solution: Install CMake via Visual Studio Installer or standalone
```

#### Linux Issues
```bash
# Missing dependencies
# Solution: Build script auto-installs, or run manually:
sudo apt install libasound2-dev libgtk-3-dev libfreetype6-dev libx11-dev

# Compiler too old
# Solution: Update to GCC 11+ or Clang 12+
sudo apt install gcc-11 g++-11
```

#### Android Issues
```bash
# NDK not found
export ANDROID_NDK_ROOT=/path/to/your/ndk

# SDK not found
export ANDROID_HOME=/path/to/your/sdk

# CMake toolchain issues
# Solution: Use exact NDK version (25.0.8775105 recommended)
```

### Build Script Options

#### All Scripts Support
```bash
--release        # Release build (default)
--debug          # Debug build
--clean          # Clean before build
--verbose        # Verbose output
--help           # Show help message
```

#### Platform-Specific Options
```bash
# macOS
--test           # Run tests after build
--notarize       # Notarize for distribution

# iOS
--simulator      # Build for simulator
--device         # Build for device
--archive        # Create archive for App Store
--team-id ID     # Apple Developer Team ID

# Windows
--arch x64       # Target architecture (x64/Win32)
--generator NAME # CMake generator

# Linux
--skip-deps      # Skip dependency installation
--generator NAME # CMake generator ("Unix Makefiles", "Ninja")

# Android
--abi ABI        # Target ABI (arm64-v8a, armeabi-v7a, x86, x86_64)
--api LEVEL      # API level (23+)
```

## 📊 Performance Optimization

### Build Performance
```bash
# Parallel builds (adjust for your CPU)
cmake --build build-dir -j 8

# Ninja generator (faster than Make on Linux)
cmake --preset linux-release -G Ninja

# CCache for faster rebuilds
export CMAKE_CXX_COMPILER_LAUNCHER=ccache
```

### Binary Size Optimization
```bash
# Release builds use:
# - Link-time optimization (LTO)
# - Dead code elimination
# - Symbol stripping
# - Optimal compiler flags per platform
```

## 🚢 Distribution

### Plugin Installation

#### macOS
```bash
# Plugins auto-install to:
~/Library/Audio/Plug-Ins/VST3/OTTO.vst3
~/Library/Audio/Plug-Ins/Components/OTTO.component
/Applications/OTTO.app
```

#### Windows
```batch
REM Install to:
%COMMONPROGRAMFILES%\VST3\OTTO.vst3
%PROGRAMFILES%\OTTO\OTTO.exe
```

#### Linux
```bash
# Install to:
~/.vst3/OTTO.vst3
/usr/local/bin/OTTO
```

### Release Builds
```bash
# Create distribution packages
./create_distribution.sh --platform all --sign --notarize

# Individual platform packages
./create_distribution.sh --platform macos --sign --notarize
./create_distribution.sh --platform windows --sign
./create_distribution.sh --platform linux --appimage
```

---

This unified build system ensures **consistent, reliable builds** across all platforms while maintaining the flexibility to use different development environments and workflows.

### Note to Zencoder:
Use our build system by doing this: cd /Users/larryseyer/AudioDevelopment/OTTO && ./scripts/build/build_macos.sh

