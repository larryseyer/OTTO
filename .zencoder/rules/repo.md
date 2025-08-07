---
description: Repository Information Overview
alwaysApply: true
---

# OTTO Information

## Summary
OTTO is a professional drum machine and sample playback system with advanced cross-platform capabilities built using JUCE 8. It features 8 independent MIDI players, SFZ 2.0 sample playback, advanced pattern management, and a professional audio engine with multi-output routing.

## Structure
- `/Source/` - C++ source code with row-based UI architecture
- `/JUCE/` - JUCE 8 framework (required external dependency)
- `/Assets/` - GUI assets, fonts, drumkits, and MIDI files
- `/cmake/` - Platform-specific toolchain files
- `/scripts/` - Build and development scripts for all platforms
- `/docs/` - Project documentation
- `/e2e-test/` - End-to-end test suite
- `/docker/` - Docker configuration for Linux builds

## Language & Runtime
**Language**: C++
**Version**: C++20
**Build System**: CMake (3.22+)
**Package Manager**: None (uses CMake for dependency management)

## Dependencies
**Main Dependencies**:
- JUCE 8.0.8+ (audio framework)
- SFZ 2.0 (sample format)

**Platform Dependencies**:
- **macOS**: CoreMIDI, AudioUnit, AudioToolbox, Accelerate, Cocoa
- **Windows**: winmm, ole32, oleaut32, imm32, shlwapi
- **Linux**: ALSA, FreeType, X11, Xext, Xrandr, Xinerama, Xcursor
- **iOS**: CoreMIDI, AudioUnit, UIKit, AVFoundation
- **Android**: OpenSLES, android, log

## Build & Installation
```bash
# Setup JUCE 8 (required)
./scripts/setup/setup_juce8.sh

# Platform-specific builds
./scripts/build/build_macos.sh           # macOS Universal
./scripts/build/build_ios.sh             # iOS
./scripts/build/build_linux.sh           # Linux
./scripts/build/build_android.sh         # Android
./scripts/build/build_windows.bat        # Windows

# Build all platforms
./scripts/build/build_all.sh --all
```

## Docker
**Docker Compose**: `docker/docker-compose.yml`
**Services**:
- `otto-linux-build`: Main Linux build service
- `otto-linux-dev`: Development environment with shell access
- `otto-linux-test`: Testing environment
- `otto-linux-arm64`: ARM64 build service

## Testing
**Framework**: Custom E2E test framework using JUCE
**Test Location**: `/e2e-test/` and `/Source/tests/`
**Test Suites**:
- UI Workflow Tests
- Component Integration Tests
- User Interaction Tests
- State Management Tests

**Run Command**:
```bash
# Build and run E2E tests
cmake --build . --target run_e2e_tests
```

## Application Structure
**Plugin Formats**:
- **macOS**: Standalone, VST3, AU, CLAP
- **Windows**: Standalone, VST3, CLAP
- **Linux**: Standalone, VST3, CLAP
- **iOS**: Standalone, AUv3
- **Android**: Standalone

**Main Components**:
- **PluginProcessor**: Audio processing and MIDI handling
- **PluginEditor**: Main UI container
- **Row-based UI**: 6 main interface rows (TopBar, PlayerTabs, DrumKit, Pattern, Interactive, Loop)
- **SFZ Engine**: Sample playback system
- **MIDI Engine**: Pattern playback and host synchronization
- **INI Data System**: Settings and preset management

## Data Management
**Storage Format**: INI-based data system
**Data Organization**:
- Settings (global application settings)
- Presets (user-created plugin presets)
- Patterns (MIDI pattern files)
- Kits (drum kit configurations)
- Mix (mixer and effects presets)