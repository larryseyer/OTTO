# 🎵 OTTO - Advanced Drum Machine & Audio Processing Plugin (JUCE 8)

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![JUCE Version](https://img.shields.io/badge/JUCE-8.0.8+-blue)
![Platform Support](https://img.shields.io/badge/platforms-macOS%20%7C%20Windows%20%7C%20Linux%20%7C%20iOS%20%7C%20Android-lightgrey)
![Phase](https://img.shields.io/badge/phase-2.3.x%20complete-success)

OTTO is a powerful **drum machine and sample playback system** with advanced **cross-platform capabilities** built using **JUCE 8**. It features professional-grade audio processing, comprehensive safety systems, and optimized build workflows for modern audio production.

## 🎛️ **Core OTTO Features**

- **8 Independent MIDI Players**: Each with synced MIDI playback to host or standalone, SFZ sample engine, unique sounds and MIDI & audio mixer with multiple outputs and FX for each
- **Pattern Management**: 4x4 grids of MIDI beat patterns organized in groups of 16
- **Smart Fill System**: Automatic and manual drum fills with intelligent pattern generation
- **Professional Audio**: SFZ 2.0 sample playback with effects for each drumkit element (kick, snare, hat, toms, cymbals, etc)
- **Advanced MIDI Integration**: Comprehensive MIDI CC support with real-time parameter control
- **Cross-Platform**: Universal support for Mac, Windows, Linux, iOS, and Android
- **Modern Plugin Formats**: VST3, AU, CLAP, Standalone, and iOS AUv3 versions

### Technical Architecture

OTTO uses these core components:
- **JUCE 8 Framework** - Modern cross-platform audio application framework with CMake build system
- **SFZ Engine** - Industry-standard sample playback
- **MIDI Engine** - Pattern playback and timing
- **Audio Engine** - Mixing, effects, and output
- **High Quality convolution and audio processing FX**
- **User Interface** - Modern, responsive GUI with embedded graphics and fonts
- **Reaper INI style .ini plain text database**

#### Current Build Status ✅
- **Unified Build System**: Single CMakeLists.txt works with both ProJucer and CMake workflows
- **Perfect IDE Integration**: Use ProJucer+Xcode OR CMake+CLion - both produce identical results
- **Cross-Platform Builds**: Supports macOS, Windows, Linux, iOS, Android with consistent `/Builds` structure
- **Plugin Formats**: VST3, AU, and Standalone all building and auto-installing correctly
- **Asset Management**: BinaryData embedded properly, shared between ProJucer and CMake builds

#### Quick Start Building 🚀
```bash
# One-line builds for each platform
./build_macos.sh           # macOS Universal → Builds/MacOSX/Release/
./build_ios.sh --simulator # iOS Simulator → Builds/iOS/Release/
./build_linux.sh           # Linux → Builds/LinuxMakefile/Release/
./build_android.sh          # Android → Builds/Android/Release/
./build_all.sh --all       # All platforms → Organized Builds/ structure

# See BUILD_QUICK_START.md for complete reference
```

#### Organized Build Structure 📁
All build outputs are automatically organized in the `Builds/` directory:
```
Builds/
├── MacOSX/Release/        # macOS Universal Binary (Intel + Apple Silicon)
│   ├── VST3/OTTO.vst3     # VST3 plugin
│   ├── AU/OTTO.component  # Audio Unit plugin
│   └── Standalone/OTTO.app # Standalone application
├── iOS/Release/           # iOS builds
│   └── Standalone/OTTO.app # iOS standalone app
├── Linux/Release/         # Linux builds
├── Android/Release/       # Android builds
└── VisualStudio2022/Release/ # Windows builds
```

OTTO uses 8 MIDI Player Playback engines consisting of:
- **Note events**: Which drum to hit (kick, snare, hi-hat, etc.)
- **Timing**: When each hit occurs (measured in ticks or beats or samples)
- **Velocity**: How hard each hit should be (0-127) modified from 10% to 200% by the "Energy" slider (defaults to 100%)
- **Length**: Total pattern duration (no limit in length)
- **Each MIDI player triggers it's own SFZ Drumkit playback audio engine with mixer and FX
- **Each Player has its own drumkit selected by drop down menu or increment/decrement chevrons
- **Custom MIDI in and MIDI out mappings
- **AI Beat generation capable.

### Audio Processing Flow
1. **MIDI Input** → Pattern selection and timing
2. **Fills Logic** → When the midi fills patterns are to be inserted (every 2,4,8,16 bars at "x" bars from the end of the loop)
3. **SFZ Engine** → Converts MIDI to stereo audio using samples and SFZ mapping
4. **Mixer** → Combines and processes audio channels for each drumkit element with send and return for each
5. **Effects** → Adds reverb, delay, and other processing as send/return and insert for each drum element.
6. **Audio Output** → Sends final audio for each player to speakers/DAW using a multi-output method (from stereo to 32 channels per player engine)

### Preset System
OTTO Stores settings for the program in 'presets'
Each preset remembers current midi drum grove group, beat, fill pattern file, fill frequency, drumkit, energy, swing, volume, mixer and fx settings for each player

SFZ 2.0 (Simple File Format) is an industry-standard format for describing how to play audio samples. An SFZ file is a text file that defines:

- **Samples**: Which audio files to use (WAV, AIFF, etc.)
- **Mapping**: Which MIDI notes trigger which samples
- **Parameters**: Volume, pitch, filtering, envelopes
- **Conditions**: Velocity ranges, round-robin samples

Each player has control over:

1) Reset: bool
2) Transport: enum { Stop, Record, Play, Pause, Punch In, Punch Out, Import, Export, Ignore, Locate Zero, Locate 1, Locate 2, Locate 3, Locate 4 }
3) Recording Mode: enum { Locked=0, Replace, Overdub }
4) Record MIDI Channel: int [1..16]
5) Monitor: bool
6) Point 1–4: float [0..600, e.g. seconds]
7) Point 1–4 Action: enum (same as Transport)
8) Sync Type: enum { Off=0, Host, Trigger }
9) Quantize Amount %: int [0..100]
10) Swing Amount %: int [0..100]
11) Humanize Amount %: int [0..100]
12) Primary Quantize: enum { None, Whole Note, Dotted Half Note, Half Note, Dotted Quarter Note, Quarter Note, Eighth Note, Eighth Note Triplet, Sixteenth Note, Sixteenth Note Triplet, Thirty-Second Note, Thirty-Second Note Triplet, Sixty-Fourth Note, Sixty-Fourth Note Triplet }
13) Secondary Quantize: enum (same range)
14) Quantize Note Off: bool
15) Transpose: int [-24..24]
16) Velocity Shift %: int [-64..64]
17) Timing Shift (Samples): int [-10000..10000]
18) Import / Export: bool triggers
19) Export to Folder (ask for folder)
20) Show Graphics: bool
21) Drum Conversion MIDI Notes: int [0..127] for Kick, Snare, Hat, Ride, Crash, Cowbell in/out
22) Snare -> Stick: bool
23) Hat -> Ride: bool
24) Crash Next Bar, Mute Next Bar, etc.: bool triggers
25) Drum Conversion: bool
26) Sample Position: read-only int
27) Send Crash: bool
28) Power: bool
29) Current Drum Folder: string
30) Energy Level: enum { Asleep=0, Chilled=1, Relaxed=2, Normal=3, Aroused=4, Excited=5, Energetic=6 }

---

## 🚀 **Phase 2.3.x - Advanced Cross-Platform Enhancements**

OTTO has been enhanced with comprehensive **Phase 2.3.x improvements** featuring advanced safety systems, optimized build workflows, and professional-grade cross-platform support.

### 🛡️ **Advanced Safety & Robustness Features**

**Phase 2.2 Foundation Safety:**
- **Memory Safety**: Advanced null-pointer protection and leak detection
- **Real-time Safety**: Buffer validation and underrun/overrun protection  
- **MIDI Safety**: Message validation and device monitoring
- **UI Thread Safety**: Component lifecycle and event validation
- **Error Recovery**: Automatic recovery and graceful degradation

**Phase 2.3.5 Advanced Robustness:**
- **Cross-platform Memory Safety** with smart pointer enforcement
- **Enhanced Audio Processing Safety** with real-time monitoring
- **Advanced MIDI Processing Safety** with timing validation
- **Performance Monitoring** with real-time metrics and alerting
- **Comprehensive Error Recovery** with checkpoint systems

### 🌐 **Supported Platforms & Formats**

| Platform | Formats | Architecture | Build System | Status |
|----------|---------|--------------|--------------|---------|
| **macOS** | Standalone, VST3, AU, CLAP | Universal (Intel + Apple Silicon) | Native Xcode/CMake | ✅ Production Ready |
| **Windows** | Standalone, VST3, CLAP | x64 | VS2022 + Parallels | ✅ Parallels Optimized |
| **Linux** | Standalone, VST3, CLAP | x86_64, ARM64 | Docker + Native | ✅ Docker Ready |
| **iOS** | Standalone, AUv3 | arm64 + Simulator | Xcode Enhanced | ✅ App Store Ready |
| **Android** | Standalone | arm64-v8a, armeabi-v7a | NDK + AAudio | ✅ Play Store Ready |

### 🏗️ **Enhanced Build System**

**Quick Start - Build All Platforms:**
```bash
# Build for all platforms
./build_all_platforms.sh --all --release

# Build specific platforms  
./build_all_platforms.sh --macos --windows --release
./build_all_platforms.sh --ios --android --debug
```

**Individual Platform Builds:**
```bash
# macOS (Universal Binary)
./build_macos.sh --release --test

# Windows (Parallels VM Optimized)
scripts/complete_windows_workflow.bat

# Linux (Docker Containerized)  
./build_linux_docker.sh --release --test

# iOS (App Store Ready with AUv3)
./build_ios_enhanced.sh --release --archive

# Android (Play Store Ready with AAudio)
./build_android.sh --release --clean
```

### 🔧 **Platform-Specific Optimizations**

**Phase 2.3.1 - Windows Enhancement:**
- **Parallels VM Integration** for seamless macOS → Windows development
- **Reaper DAW Integration** for professional plugin testing
- **WASAPI/DirectSound Validation** with advanced error handling
- **Complete Windows Workflow** automation

**Phase 2.3.2 - Docker Linux System:**
- **Multi-stage Containerized Builds** with cross-architecture support
- **Docker Compose Orchestration** for development workflows
- **Enhanced Build Scripts** with comprehensive validation
- **Development Container** with shell access for debugging

**Phase 2.3.3 - iOS Enhancement:**
- **Advanced AUv3 Optimizations** with Metal rendering integration
- **Core Audio Integration** with iOS-specific optimizations
- **App Store Compliance** with automatic code signing
- **Enhanced iOS Build Script** with device/simulator support

**Phase 2.3.4 - Android Enhancement:**
- **AAudio Low-latency Integration** for professional audio performance
- **ARM NEON SIMD Optimizations** for enhanced audio processing
- **Play Store Readiness** with security and privacy compliance
- **Advanced Android Configuration** with manifest generation

### 🧪 **Comprehensive Testing & Validation**

**Validation System:**
```bash
# Run complete Phase 2.3.x validation
./scripts/validate_phase_2_3_x.sh

# Expected result: 55/55 validations passed ✅
```

**Platform-Specific Testing:**
- **Windows**: Reaper DAW integration testing with Parallels VM
- **Linux**: Docker container testing with multi-architecture support
- **iOS**: Simulator and device testing with AUv3 validation
- **Android**: Emulator and device testing with AAudio verification
- **macOS**: Native testing with all plugin formats

### 📊 **Performance Specifications**

| Metric | Specification |
|--------|---------------|
| **Audio Latency** | < 5ms (with appropriate buffer settings) |
| **CPU Usage** | < 5% (typical), < 15% (maximum) |
| **Memory Usage** | < 50MB (typical), < 100MB (maximum) |
| **Sample Rates** | 44.1kHz - 192kHz |
| **Bit Depth** | 16-bit, 24-bit, 32-bit float |
| **MIDI Latency** | < 1ms |

### 🛠️ **Development Environment Setup**

**Prerequisites:**
- **JUCE 8.0.8+** (Framework)
- **CMake 3.22+** (Build System)  
- **C++20 Compiler** (Platform-specific)

**Quick Setup:**
```bash
# macOS Development
brew install cmake
git clone [repository]
cd OTTO
./build_macos.sh --release

# Windows Development (Parallels)
# 1. Set up Windows 11 Parallels VM
# 2. Install Visual Studio 2022
# 3. Run: scripts\complete_windows_workflow.bat

# Linux Development (Docker)
docker-compose -f docker/docker-compose.yml up otto-linux-dev

# iOS Development
# Requires Xcode 14.0+ with iOS SDK
./build_ios_enhanced.sh --simulator --debug

# Android Development  
# Requires Android Studio + NDK 25+
./build_android.sh --debug
```

### 📁 **Enhanced Project Structure**

```
OTTO/
├── Source/                          # Enhanced source code
│   ├── JUCE8Compatibility.h         # JUCE 8 compatibility + safety features
│   ├── ProportionalScalingManager.* # Phase 2.2 proportional scaling
│   ├── EnhancedProportionalEditor.* # Enhanced editor with safety
│   ├── WindowsErrorHandling.h       # Windows-specific error handling
│   └── WindowsAudioValidator.h      # WASAPI/DirectSound validation
├── cmake/                           # Platform-specific CMake configs
│   ├── Windows.cmake                # Phase 2.3.1 Windows optimization
│   ├── iOS-Enhanced.cmake           # Phase 2.3.3 iOS enhancement
│   ├── Android-Enhanced.cmake       # Phase 2.3.4 Android optimization
│   └── AdvancedRobustness.cmake     # Phase 2.3.5 robustness features
├── docker/                          # Phase 2.3.2 Docker infrastructure
│   ├── Dockerfile.linux             # Multi-stage Linux build
│   └── docker-compose.yml           # Development orchestration
├── scripts/                         # Enhanced build and utility scripts
│   ├── complete_windows_workflow.bat # Complete Windows development
│   ├── validate_phase_2_3_x.sh     # Comprehensive validation
│   └── optimize_parallels_windows.bat # Parallels optimization
├── build_*.sh|bat                   # Enhanced platform build scripts
├── CMakeLists.txt                   # Enhanced main CMake configuration
└── CMakePresets.json                # All platform presets
```

### 🏆 **Production Ready Features**

✅ **Complete JUCE 8.0.8+ compatibility** with modern C++20 features  
✅ **Professional-grade safety systems** across all platforms  
✅ **Comprehensive cross-platform support** with optimized workflows  
✅ **Advanced build automation** with validation systems  
✅ **Production-ready quality** with performance optimization  
✅ **Professional audio standards** with low-latency processing  
✅ **Modern plugin format support** including AUv3 and CLAP  
✅ **Enhanced user experience** with robust error handling  

---

## 🎵 **Ready for Professional Drum Production**

OTTO Phase 2.3.x combines the powerful **drum machine capabilities** with **advanced cross-platform technology**, making it the ideal choice for:

- **Professional Music Production** with comprehensive DAW integration
- **Live Performance** with low-latency real-time processing  
- **Cross-Platform Development** with unified build systems
- **Mobile Music Production** with iOS and Android support
- **Plugin Development** with modern safety and robustness features

**Start creating professional drum tracks today with OTTO's advanced drum machine and sample playback system!**

---

*Built with ❤️ using JUCE 8 and modern C++20 • Phase 2.3.x Complete*
