# OTTO Multi-Platform Build Instructions

This document provides comprehensive instructions for building OTTO on all supported platforms using JUCE 8.

## Quick Start

1. **Setup JUCE 8 and dependencies:**
   ```bash
   ./setup_juce8.sh
   ```

2. **Build for your current platform:**
   ```bash
   # macOS
   ./build_macos.sh
   
   # iOS Simulator (macOS only)
   ./build_ios.sh --simulator
   
   # iOS Device (requires Apple Developer Team ID)
   ./build_ios.sh --device --team-id YOUR_TEAM_ID
   
   # Linux
   ./build_linux.sh
   
   # Android (requires Android NDK/SDK)
   ./build_android.sh
   
   # All platforms
   ./build_all.sh --all
   ```

## Supported Platforms

| Platform | Architecture | Build System | Status |
|----------|-------------|--------------|--------|
| macOS    | x86_64, arm64 | Xcode/CMake | ✅ |
| iOS      | arm64       | Xcode/CMake | ✅ |
| Windows  | x64         | Visual Studio/CMake | ✅ |
| Linux    | x86_64      | Make/CMake  | ✅ |
| Android  | arm64-v8a   | CMake/NDK   | ✅ |

## Prerequisites

### All Platforms
- CMake 3.22 or later
- Git
- JUCE 8 (automatically downloaded by setup script)

### macOS / iOS
- Xcode 13+ with Command Line Tools
- macOS 10.15+ (for building)
- iOS 12.0+ (for target)

### Windows
- Visual Studio 2019/2022 with C++ workload
- Windows 10 SDK

### Linux
- GCC 9+ or Clang 10+
- Development packages:
  ```bash
  # Ubuntu/Debian
  sudo apt-get install build-essential libasound2-dev libgtk-3-dev libwebkit2gtk-4.0-dev libfreetype6-dev libcurl4-openssl-dev pkg-config libjack-jackd2-dev
  
  # CentOS/RHEL/Fedora
  sudo yum groupinstall "Development Tools"
  sudo yum install alsa-lib-devel gtk3-devel webkit2gtk3-devel freetype-devel libcurl-devel pkgconfig jack-audio-connection-kit-devel
  ```

### Android
- Android Studio with NDK (r21+)
- Android SDK (API level 23+)
- Set environment variables:
  ```bash
  export ANDROID_HOME=/path/to/android-sdk
  export ANDROID_NDK_ROOT=/path/to/android-ndk
  ```

## Build Outputs

### Plugin Formats
- **VST3**: Modern cross-platform plugin format
- **AU**: Audio Units (macOS/iOS only)
- **Standalone**: Independent application

### Output Locations
```
build-{platform}/
├── OTTO_artefacts/
│   ├── Release/
│   │   ├── VST3/OTTO.vst3
│   │   ├── AU/OTTO.component (macOS only)
│   │   └── Standalone/OTTO.app
```

## IDE-Specific Instructions

### CLion Configuration

#### iOS Simulator Setup for Apple Silicon Macs

1. **Create iOS Simulator CMake Profile:**
   - Go to `File > Settings > Build, Execution, Deployment > CMake`
   - Add new profile: `iOS-Simulator`
   - Set the following CMake options:
   ```
   -DCMAKE_BUILD_TYPE=Debug
   -DCMAKE_SYSTEM_NAME=iOS
   -DCMAKE_OSX_ARCHITECTURES=arm64
   -DCMAKE_OSX_SYSROOT=iphonesimulator
   -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0
   ```

2. **Environment Variables:**
   Set the following environment variables in CLion:
   ```bash
   DYLD_ROOT_PATH=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk
   DYLD_FRAMEWORK_PATH=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk/System/Library/Frameworks
   ```

3. **For Intel Macs (Legacy):**
   Use `x86_64` architecture instead:
   ```
   -DCMAKE_OSX_ARCHITECTURES=x86_64
   ```

4. **macOS Profile:**
   - Profile name: `macOS-Debug`
   - CMake options:
   ```
   -DCMAKE_BUILD_TYPE=Debug
   -DCMAKE_OSX_ARCHITECTURES=arm64
   -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13
   ```

#### Runtime Configuration

To resolve simulator runtime errors, set up run configuration with:
1. **Working directory:** Project root
2. **Environment variables:**
   ```
   DYLD_ROOT_PATH=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk
   ```

#### Architecture Detection Script

Add this to your CLion build configuration to auto-detect architecture:
```bash
#!/bin/bash
if [[ $(uname -m) == "arm64" ]]; then
    export IOS_ARCH="arm64"
else
    export IOS_ARCH="x86_64"
fi
echo "Building for iOS Simulator architecture: $IOS_ARCH"
```

#### Using CMake Toolchain Files

For easier CLion configuration, use the provided toolchain files:

1. **iOS Simulator with auto-detection:**
   ```
   -DCMAKE_TOOLCHAIN_FILE=cmake/iOS-Simulator.cmake
   ```

2. **iOS Device:**
   ```
   -DCMAKE_TOOLCHAIN_FILE=cmake/iOS-Device.cmake
   ```

#### CLion Run/Debug Configuration

1. **Create iOS Simulator run configuration:**
   - Target: Select your OTTO target
   - Executable: Path to generated .app bundle
   - Working directory: Project root
   - Environment variables:
     ```
     DYLD_ROOT_PATH=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk
     DYLD_FRAMEWORK_PATH=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk/System/Library/Frameworks
     ```

2. **Testing different architectures:**
   - Apple Silicon Mac: Use arm64 architecture
   - Intel Mac: Use x86_64 architecture
   - The toolchain files automatically detect the correct architecture

## iOS Device Builds & Code Signing

### Apple Developer Requirements

iOS device builds require a valid Apple Developer account and proper code signing setup:

1. **Get your Apple Developer Team ID:**
   ```bash
   ./get_team_id.sh
   ```

2. **Set your Team ID as environment variable:**
   ```bash
   export APPLE_TEAM_ID='YOUR_ACTUAL_TEAM_ID'
   ```

3. **Sign in to Xcode:**
   - Open Xcode → Settings → Accounts
   - Add your Apple ID
   - Download development certificates

### iOS Build Commands

```bash
# iOS Simulator (no signing required) - Auto-detects architecture
./build_ios.sh --simulator

# iOS Device (requires Apple Developer Team ID)
./build_ios.sh --device --team-id $APPLE_TEAM_ID

# Or with explicit Team ID
./build_ios.sh --device --team-id ABC123XYZ9
```

### Troubleshooting Code Signing

If you get provisioning profile errors:

1. **Verify your Team ID is correct:**
   ```bash
   ./get_team_id.sh
   ```

2. **Check Xcode accounts:**
   - Xcode → Settings → Accounts
   - Make sure your Apple ID is signed in
   - Download manual profiles if needed

3. **Clean and rebuild with correct Team ID:**
   ```bash
   rm -rf build-ios-device
   ./build_ios.sh --device --team-id YOUR_ACTUAL_TEAM_ID
   ```

4. **Alternative: Use Xcode for manual signing:**
   - Open `build-ios-device/OTTO.xcodeproj` in Xcode
   - Select OTTO_Standalone target
   - Go to Signing & Capabilities tab
   - Set your Team and Bundle Identifier
   - Build manually (⌘+B)

### Bundle Identifier
The app uses bundle ID: `com.audiodev.OTTO`

You can modify this in `CMakeLists.txt` if needed:
```cmake
BUNDLE_ID "com.yourcompany.OTTO"
```

## Advanced Build Options

### Custom JUCE Installation
If you have JUCE installed elsewhere:
```bash
cmake -DJUCE_PATH=/path/to/juce ..
```

### Debug Builds
```bash
# Add to any build script
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

### Architecture-Specific Builds

#### macOS Universal Binary
```bash
cmake .. -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
```

#### iOS Device vs Simulator
```bash
# Device
cmake .. -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_ARCHITECTURES=arm64

# Simulator (Apple Silicon Macs)
cmake .. -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_SYSROOT=iphonesimulator

# Simulator (Intel Macs - Legacy)
cmake .. -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_OSX_SYSROOT=iphonesimulator
```

#### Android ABIs
```bash
# ARM64 (default)
cmake .. -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a

# ARM32
cmake .. -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a

# x86_64 (emulator)
cmake .. -DCMAKE_ANDROID_ARCH_ABI=x86_64
```

## Troubleshooting

### Common Issues

1. **JUCE not found**
   - Run `./setup_juce8.sh` first
   - Check that `JUCE/` directory exists in project root

2. **CMake version too old**
   - Install CMake 3.22+: `brew install cmake` (macOS) or download from cmake.org

3. **Missing dependencies (Linux)**
   - Run the setup script which installs most dependencies
   - For exotic distributions, install equivalent packages manually

4. **Android build fails**
   - Ensure `ANDROID_HOME` and `ANDROID_NDK_ROOT` are set correctly
   - Verify NDK version is r21 or later

5. **iOS code signing issues**
   - Set your Apple Developer Team ID in `build_ios.sh`
   - Ensure proper certificates are installed

### Build Cache Issues
```bash
# Clean all build directories
rm -rf build-*

# Clean CMake cache
find . -name "CMakeCache.txt" -delete
find . -name "CMakeFiles" -type d -exec rm -rf {} +
```

### Verbose Build Output
Add `-v` or `--verbose` to cmake build commands:
```bash
cmake --build . --config Release --verbose
```

## Cross-Compilation

### Windows from macOS/Linux
Requires MinGW-w64 toolchain (experimental):
```bash
# Install mingw-w64
brew install mingw-w64  # macOS
# or
sudo apt-get install mingw-w64  # Linux

# Build
mkdir build-windows-cross
cd build-windows-cross
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-w64-toolchain.cmake
cmake --build .
```

## Performance Optimization

### Release Builds
Always use Release configuration for distribution:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Link-Time Optimization (LTO)
```bash
cmake .. -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
```

### Native Architecture Optimization
```bash
cmake .. -DCMAKE_CXX_FLAGS="-march=native -mtune=native"
```

## Testing

Run unit tests after building:
```bash
# From project root
make -f Makefile.tests run
```

## Continuous Integration

The build system is designed to work with CI/CD platforms:

### GitHub Actions Example
```yaml
- name: Setup JUCE 8
  run: ./setup_juce8.sh

- name: Build for macOS
  run: ./build_macos.sh
```

### Docker Linux Builds
```dockerfile
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y build-essential cmake git
# ... install other dependencies
COPY . /workspace
WORKDIR /workspace
RUN ./setup_juce8.sh && ./build_linux.sh
```

## Support

For build issues:
1. Check this documentation
2. Verify all prerequisites are installed
3. Try cleaning build directories
4. Check JUCE documentation for platform-specific requirements

## Contributing

When adding new features:
1. Update source file lists in `CMakeLists.txt`
2. Test builds on all target platforms
3. Update this documentation if needed