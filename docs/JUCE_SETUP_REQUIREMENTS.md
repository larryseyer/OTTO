# 🎵 JUCE 8 Setup Requirements for OTTO

## 📋 **Overview**

OTTO is built using **JUCE 8**, a powerful cross-platform C++ framework for audio applications. JUCE is dual-licensed under AGPLv3 and commercial licenses.

## ⚖️ **JUCE Licensing Information**

### **Open Source Projects (AGPLv3)**
- ✅ **Free to use** for open source projects
- ✅ **No revenue restrictions** for AGPLv3 compliance
- ✅ **Source code must be available** under AGPLv3
- ✅ **OTTO complies** with AGPLv3 requirements

### **Commercial License**
- 💰 **Required** if you want to keep your source code proprietary
- 💰 **Revenue limits** apply for different license tiers
- 📄 Visit [JUCE Licensing](https://juce.com/get-juce) for details

## 🚀 **Quick Setup**

### **Option 1: Automatic Setup (Recommended)**
```bash
# Run our automated JUCE 8 setup script
./scripts/setup/setup_juce8.sh
```

### **Option 2: Manual Setup**
```bash
# Clone JUCE 8 from official repository
git clone --branch master https://github.com/juce-framework/JUCE.git
```

## 📁 **Directory Structure**

After setup, your project should look like this:
```
OTTO/
├── JUCE/                          # JUCE 8 Framework (downloaded separately)
│   ├── CMakeLists.txt
│   ├── modules/
│   └── extras/
├── scripts/                       # Build and setup scripts (tracked in git)
│   ├── build/
│   │   ├── build_macos.sh
│   │   ├── build_ios.sh
│   │   ├── build_linux.sh
│   │   ├── build_android.sh
│   │   └── build_windows.bat
│   └── setup/
│       └── setup_juce8.sh
├── Source/                        # OTTO source code
├── CMakeLists.txt                 # Main CMake configuration
└── OTTO.jucer                     # ProJucer project file
```

## 🔧 **Platform-Specific Requirements**

### **macOS**
```bash
# Prerequisites
xcode-select --install
brew install cmake pkgconf

# JUCE Setup
./scripts/setup/setup_juce8.sh

# Build OTTO
./scripts/build/build_macos.sh
```

### **Windows**
```batch
# Prerequisites (install manually):
# - Visual Studio 2022 with C++ workload
# - CMake 3.21+
# - Git

# JUCE Setup
git clone https://github.com/juce-framework/JUCE.git

# Build OTTO
scripts\build\build_windows.bat
```

### **Linux**
```bash
# Prerequisites (Ubuntu/Debian)
sudo apt update
sudo apt install build-essential cmake git pkg-config
sudo apt install libasound2-dev libx11-dev libxext-dev libxrandr-dev libxss-dev libxi-dev libxrender-dev libxcomposite-dev libxdamage-dev libxcursor-dev libxinerama-dev libfreetype6-dev libfontconfig1-dev

# JUCE Setup
./scripts/setup/setup_juce8.sh

# Build OTTO
./scripts/build/build_linux.sh
```

### **iOS**
```bash
# Prerequisites
# - macOS with Xcode 15+
# - Valid Apple Developer account for device builds

# Get your Team ID
./scripts/setup/get_team_id.sh

# JUCE Setup
./scripts/setup/setup_juce8.sh

# Build for Simulator
./scripts/build/build_ios.sh --simulator

# Build for Device (requires Team ID)
export APPLE_TEAM_ID="YOUR_TEAM_ID"
./scripts/build/build_ios.sh --device
```

### **Android**
```bash
# Prerequisites
# - Android Studio with NDK
# - CMake 3.21+

# Set environment variables
export ANDROID_NDK_ROOT=/path/to/android/ndk
export ANDROID_SDK_ROOT=/path/to/android/sdk

# JUCE Setup
./scripts/setup/setup_juce8.sh

# Build OTTO
./scripts/build/build_android.sh
```

## 🏗️ **Multi-Platform Build**

```bash
# Build for all supported platforms
./scripts/build/build_all.sh --all

# Build for specific platforms
./scripts/build/build_all.sh --macos --ios
./scripts/build/build_all.sh --linux --android
```

## 📝 **JUCE Version Compatibility**

- **Required**: JUCE 8.0.0 or later
- **Tested with**: JUCE 8.0.4+
- **Compatibility**: C++20 standard required

## 🚨 **Important Notes**

### **Why JUCE is Not Included**
- 📦 **Repository size**: JUCE is ~500MB and would bloat the repository
- ⚖️ **Licensing clarity**: Users should get JUCE directly from official source
- 🔄 **Updates**: Users get latest JUCE updates directly
- 🎯 **Best practice**: Industry standard for JUCE-based projects

### **Build Script Availability**
- ✅ **All build scripts are included** in this repository
- ✅ **Setup scripts automate** JUCE installation
- ✅ **Cross-platform support** for all major platforms
- ✅ **Professional build system** with CMake integration

## 🔍 **Verification**

After setup, verify your installation:
```bash
# Check JUCE installation
ls -la JUCE/
ls -la JUCE/modules/

# Test build (macOS example)
./scripts/build/build_macos.sh

# Verify build outputs
ls -la Builds/MacOSX/Release/
```

## 📚 **Additional Resources**

- 📖 [JUCE Documentation](https://docs.juce.com/)
- 🌐 [JUCE Website](https://juce.com/)
- 💬 [JUCE Forum](https://forum.juce.com/)
- 📦 [JUCE GitHub](https://github.com/juce-framework/JUCE)
- ⚖️ [JUCE Licensing](https://juce.com/get-juce)

## 🆘 **Troubleshooting**

### **JUCE Not Found**
```bash
# Ensure JUCE directory exists and contains CMakeLists.txt
ls JUCE/CMakeLists.txt

# Re-run setup if needed
./scripts/setup/setup_juce8.sh
```

### **Build Failures**
```bash
# Clean and retry
./scripts/build/cleanup_old_builds.sh
./scripts/build/build_macos.sh  # or your platform
```

### **Missing Dependencies**
```bash
# Run setup script which checks dependencies
./scripts/setup/setup_juce8.sh
```

---

**📄 License Compliance**: OTTO is distributed under AGPLv3, compatible with JUCE's AGPLv3 option.  
**🔄 Setup Status**: Automated setup scripts ensure proper JUCE 8 installation and configuration.  
**🚀 Ready to Build**: Once JUCE is installed, all platforms supported with included build scripts.