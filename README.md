# 🎵 OTTO - Advanced Drum Machine & Audio Processing Plugin (JUCE 8)

![Build Status](https://img.shields.io/badge/build-complete-brightgreen)
![JUCE Version](https://img.shields.io/badge/JUCE-8.0.8+-blue)
![Platform Support](https://img.shields.io/badge/platforms-macOS%20%7C%20Windows%20%7C%20Linux%20%7C%20iOS%20%7C%20Android-lightgrey)
![Status](https://img.shields.io/badge/status-production%20ready-success)

OTTO is a professional **drum machine and sample playback system** with advanced **cross-platform capabilities** built using **JUCE 8**. It features professional-grade audio processing, comprehensive safety systems, and optimized build workflows for modern audio production.

## 🎛️ Core Features

### 8 Independent MIDI Players
Each player is a complete audio production unit featuring:
- **Synchronized MIDI playback** (host sync or standalone)
- **SFZ 2.0 sample engine** with professional sample playback
- **Individual audio mixer** with multiple outputs (stereo to 32 channels)
- **Dedicated effects chain** for each drum element
- **Real-time parameter control** with comprehensive MIDI CC support

### Advanced Pattern Management
- **4×4 grid system** of MIDI beat patterns
- **Organized in groups of 16** for complex song arrangements
- **Smart Fill System** with automatic and manual drum fills
- **AI Beat Generation** for intelligent pattern creation
- **Unlimited pattern duration** support

### Professional Audio Engine
- **SFZ 2.0 sample playback** with industry-standard mapping
- **High-quality convolution** and audio processing effects
- **Send/return and insert effects** for each drum element
- **Multi-output routing** with configurable channel assignment
- **Real-time mixing** with professional-grade audio quality

### Cross-Platform Support
| Platform | Formats | Architecture | Status |
|----------|---------|--------------|---------|
| **macOS** | Standalone, VST3, AU, CLAP | Universal (Intel + Apple Silicon) | ✅ Production Ready |
| **Windows** | Standalone, VST3, CLAP | x64 | ✅ Production Ready |
| **Linux** | Standalone, VST3, CLAP | x86_64, ARM64 | ✅ Production Ready |
| **iOS** | Standalone, AUv3 | arm64 + Simulator | ✅ App Store Ready |
| **Android** | Standalone | arm64-v8a, armeabi-v7a | ✅ Play Store Ready |

## 🚀 Quick Start

### Single Platform Builds
```bash
./build_macos.sh           # macOS Universal → Builds/MacOSX/Release/
./build_ios.sh --simulator # iOS Simulator → Builds/iOS/Release/
./build_linux.sh           # Linux → Builds/LinuxMakefile/Release/
./build_android.sh         # Android → Builds/Android/Release/
./build_windows.bat        # Windows → Builds/VisualStudio2022/Release/
```

### Multi-Platform Build
```bash
./build_all.sh --all       # All platforms → Organized Builds/ structure
```

**See [BUILDRULES.md](BUILDRULES.md) for complete build instructions**

## 🏗️ Technical Architecture

### Core Components
- **JUCE 8 Framework** - Modern cross-platform audio application framework
- **Unified CMake Build System** - Single configuration for all platforms
- **SFZ Engine** - Industry-standard sample playback
- **MIDI Engine** - Pattern playback and host synchronization
- **Audio Engine** - Mixing, effects, and multi-output routing
- **Modern UI** - Responsive GUI with embedded assets and real-time control
- **INI Data System** - Reaper-style plain text database for settings and presets

### Build Structure
```
Builds/
├── MacOSX/Release/        # macOS Universal Binary
│   ├── VST3/OTTO.vst3     # VST3 plugin
│   ├── AU/OTTO.component  # Audio Unit plugin
│   └── Standalone/OTTO.app # Standalone application
├── iOS/Release/           # iOS builds
├── LinuxMakefile/Release/ # Linux builds
├── VisualStudio2022/Release/ # Windows builds
└── Android/Release/       # Android builds
```

## 💾 Data Management

OTTO uses a comprehensive INI-based data system for all settings and presets:

### Preset System
- **Complete State Capture**: MIDI groove groups, patterns, fills, drum kits
- **Audio Settings**: Energy levels, swing, volume, mixer and effects settings
- **Player Configuration**: All 8 players' individual settings
- **Human-Readable Format**: Plain text INI files for easy editing and version control

### Data Organization
```
OTTO_Data/
├── Settings/           # Global application settings
├── Presets/           # User-created plugin presets
├── Patterns/          # MIDI pattern files
├── Kits/             # Drum kit configurations
├── Mix/              # Mixer and effects presets
└── System/           # System configuration files
```

**See [INIRULES.md](INIRULES.md) for complete data management details**

## 🎨 User Interface

### Modern Design System
- **Responsive Layout** with INI-driven positioning (no hardcoded values)
- **Professional UI Components** with proper JUCE 8 integration
- **Embedded Assets** for consistent visual experience
- **Phosphor Icon System** with 6 weight variations
- **Variable Font Support** with professional typography

### Layout Structure
- **Row-based organization** with 6 main interface rows
- **Native HD 720p resolution** (1280×720) for optimal screen compatibility  
- **Proportional scaling** that adapts to different screen sizes
- **Touch-friendly controls** meeting 44px accessibility standards (iOS/Android compliant)
- **Percentage-based responsive design** automatically maintains accessibility across all platforms
- **Real-time parameter feedback** with visual indicators

**See [DESIGNRULES.md](DESIGNRULES.md) for complete UI guidelines**

## 🔧 Player Engine Details

Each of OTTO's 8 players provides comprehensive control:

### Transport & Recording
- **Transport**: Stop, Record, Play, Pause, Punch In/Out, Import/Export
- **Recording Modes**: Locked, Replace, Overdub
- **Locate Points**: 4 configurable position markers
- **MIDI Channel Selection**: Input channels 1-16

### Timing & Groove
- **Sync Types**: Off, Host Sync, Trigger
- **Quantization**: Primary/Secondary with 0-100% amount
- **Swing & Humanize**: 0-100% with musical quantization options
- **Timing Adjustment**: ±10,000 sample precision

### Audio Processing
- **Transpose**: -24 to +24 semitones
- **Velocity Control**: -64 to +64 with Energy slider (10%-200%)
- **Drum Conversion**: Intelligent mapping between drum elements
- **Energy Levels**: 7-level system (Asleep to Energetic)

## 📊 Performance Specifications

| Metric | Specification |
|--------|---------------|
| **Audio Latency** | < 5ms (with appropriate buffer settings) |
| **CPU Usage** | < 5% (typical), < 15% (maximum) |
| **Memory Usage** | < 50MB (typical), < 100MB (maximum) |
| **Sample Rates** | 44.1kHz - 192kHz |
| **Bit Depth** | 16-bit, 24-bit, 32-bit float |
| **MIDI Latency** | < 1ms |

## 🎯 What's Been Completed

### ✅ Phase 2.3.x - Cross-Platform Foundation
- **Unified Build System**: Single CMakeLists.txt works across all platforms
- **Advanced Safety Systems**: Memory protection, real-time safety, error recovery  
- **Comprehensive Platform Support**: All 5 target platforms building successfully
- **Professional Build Workflows**: Automated scripts with proper error handling

### ✅ Phase 3-4 - Core Engine Implementation  
- **8-Player MIDI Engine**: Complete implementation with host sync
- **SFZ Sample Playback**: Full SFZ 2.0 specification support
- **Audio Processing**: High-quality effects and mixing system
- **Pattern Management**: 4×4 grid system with groups and fills

### ✅ Phase 5-6 - Complete INI-Driven Layout System
- **100% INI-Driven Layout**: Eliminated all hardcoded positioning values across all 6 interface rows
- **Complete 6-Row Architecture**: TopBar (8%) → PlayerTabs (15%) → Player Controls (12%) → Pattern Controls (14%) → Main Matrix (45%) → Loop Controls (12%)
- **Responsive Design System**: Percentage-based layout with automatic scaling via `layoutManager.scaled()`
- **Enhanced ColorScheme Integration**: Complete theme propagation to all child components
- **Comprehensive Validation**: Debug-only system validation with performance monitoring (<5ms layout timing)
- **Modern UI Components**: JUCE 8 compliant interface elements with FontManager integration
- **Cross-Platform Accessibility**: 44px minimum touch targets maintained across all platforms
- **Asset Management**: Embedded fonts, graphics, and Phosphor icon systems

### ✅ Production Ready Features
- **Cross-Platform Builds**: Verified working on all target platforms
- **Professional Audio Quality**: Low-latency, high-fidelity audio processing
- **Comprehensive Testing**: Validated across multiple development environments
- **Documentation**: Complete technical documentation and build guides

## 🚧 Development Roadmap

### Near-Term Enhancements
- Enhanced AI beat generation algorithms
- Advanced pattern morphing and variation systems
- Cloud-based preset sharing and collaboration
- Extended SFZ 2.0 features and sample libraries

### Long-Term Vision
- Machine learning-powered drum pattern analysis
- Advanced audio analysis and automatic mixing
- Integration with popular DAW ecosystems
- Mobile-optimized interface enhancements

## 📚 Documentation

- **[REPO.md](REPO.md)** - GitHub repository guidelines and contribution rules
- **[DESIGNRULES.md](DESIGNRULES.md)** - UI design guidelines and component standards
- **[BUILDRULES.md](BUILDRULES.md)** - Complete cross-platform build instructions  
- **[INIRULES.md](INIRULES.md)** - Data storage and INI configuration system
- **[docs/INI_LAYOUT_SYSTEM_COMPLETION.md](docs/INI_LAYOUT_SYSTEM_COMPLETION.md)** - Complete INI-driven layout system implementation details

## 🏆 Key Achievements

- **100% JUCE 8 Compatibility** with modern C++20 features
- **Complete INI-Driven Layout System** - Zero hardcoded values across all 6 interface rows
- **Professional Responsive Design** - Sub-5ms layout timing with automatic cross-platform scaling
- **Perfect Cross-Platform Parity** across all 5 target platforms
- **Enhanced Accessibility Compliance** - 44px minimum touch targets meeting WCAG 2.1 standards
- **Comprehensive Theme System** - Complete ColorScheme integration with runtime theme switching
- **Professional Audio Quality** meeting industry standards
- **Advanced Validation Systems** - Compile-time and runtime layout integrity checks
- **Complete Build Automation** with unified CMake configuration

---

**OTTO** represents a complete, professional-grade drum machine solution built with modern development practices and designed for the future of cross-platform audio production.
