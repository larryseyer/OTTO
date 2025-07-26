# OTTO - Quick Build Guide

## 🚀 Quick Start

### macOS (Universal Binary)
```bash
./build_macos.sh
```
**Output:** `Builds/MacOSX/Release/` → VST3, AU, Standalone

### iOS 
```bash
# For Simulator (auto-detects Mac architecture)
./build_ios.sh --simulator --team-id YOUR_TEAM_ID

# For Device
./build_ios.sh --device --team-id YOUR_TEAM_ID
```
**Output:** `Builds/iOS/Release/` → iOS App

### Linux
```bash
./build_linux.sh
```
**Output:** `Builds/LinuxMakefile/Release/` → VST3, Standalone

### Windows
```batch
build_windows.bat
```
**Output:** `Builds/VisualStudio2022/Release/` → VST3, Standalone

### Android
```bash
# Set environment first
export ANDROID_NDK_ROOT=/path/to/ndk
export ANDROID_HOME=/path/to/sdk

./build_android.sh
```
**Output:** `Builds/Android/Release/` → Shared Library (.so)

### All Platforms
```bash
./build_all.sh --all
```

## ⚡ Prerequisites

| Platform | Requirements |
|----------|-------------|
| **macOS** | Xcode, CMake |
| **iOS** | Xcode, Apple Developer Account |
| **Linux** | GCC/Clang, CMake, GTK3, ALSA |
| **Windows** | Visual Studio 2019/2022, CMake |
| **Android** | Android NDK, Android SDK |

## 🎯 IDE Integration

### CLion / CMake IDEs
1. Open project root
2. Select CMake preset:
   - `macos-release`
   - `ios-simulator-debug`
   - `linux-release`
   - `windows-release`
3. Build with CMake

### Xcode
1. Open `Builds/MacOSX/OTTO.xcodeproj` (ProJucer)
2. Or use CMake-generated project in `Builds/MacOSX/CMake-Release/`

### Visual Studio
1. Open `Builds/VisualStudio2022/OTTO.sln` (ProJucer)
2. Or use CMake-generated project

## 📁 Build Outputs

All builds are organized in `/Builds` subdirectories:

```
Builds/
├── MacOSX/Release/     → macOS plugins
├── iOS/Release/        → iOS apps
├── LinuxMakefile/Release/ → Linux plugins
├── VisualStudio2022/Release/ → Windows plugins
└── Android/Release/    → Android libraries
```

## ✅ Verification

### macOS Success
```
✅ macOS build complete!
📦 Build artifacts:
../Release/VST3/OTTO.vst3
../Release/Standalone/OTTO.app
../Release/AU/OTTO.component
```

### Plugin Installation
- **VST3**: Auto-installed to system plugin directories
- **AU**: Auto-installed to `~/Library/Audio/Plug-Ins/Components/`
- **Standalone**: Run directly from build directory

## 🔧 Troubleshooting

### Common Issues

**macOS**: Xcode command line tools missing
```bash
xcode-select --install
```

**iOS**: Team ID required
```bash
# Get your Team ID from https://developer.apple.com/account/
export APPLE_TEAM_ID=YOUR_TEAM_ID
```

**Linux**: Missing dependencies
```bash
# Ubuntu/Debian
sudo apt-get update && sudo apt-get install build-essential cmake libasound2-dev libgtk-3-dev

# CentOS/RHEL
sudo yum install gcc-c++ cmake alsa-lib-devel gtk3-devel

# Arch
sudo pacman -S gcc cmake alsa-lib gtk3
```

**Windows**: Visual Studio not found
- Install Visual Studio 2019+ with C++ development tools

**Android**: NDK/SDK not set
```bash
export ANDROID_NDK_ROOT=/path/to/android-ndk
export ANDROID_HOME=/path/to/android-sdk
```

## 🎵 Using Your Plugin

### DAW Testing
1. Build succeeds → plugins auto-install
2. Open your DAW (Logic Pro, Reaper, etc.)
3. Rescan plugins
4. Find "OTTO" by "Automagic Art"

### Standalone Testing
- **macOS**: Double-click `OTTO.app`
- **Linux**: Run `./OTTO` from build directory
- **Windows**: Run `OTTO.exe` from build directory

## 🔄 Clean Build
```bash
# Remove all CMake build directories
rm -rf Builds/*/CMake-*

# Full clean (keeps ProJucer files)
git clean -xdf --exclude="Builds/*/OTTO.*" --exclude="Builds/*/*.plist"
```

Ready to build! 🎶