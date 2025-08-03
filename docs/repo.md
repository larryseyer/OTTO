# 🎵 OTTO Repository Information

## 📋 Repository Overview

**OTTO** is a professional-grade, cross-platform drum machine and audio processing plugin built with **JUCE 8**. This repository contains a complete audio production system designed for modern music creation with advanced pattern management, sample playback, and real-time audio processing capabilities.

### 🏗️ Project Architecture

- **Framework**: JUCE 8.0.8+ (Modern C++20)
- **Build System**: Unified CMake configuration for all platforms
- **License**: GNU General Public License v3.0
- **Development Status**: Production Ready
- **Company**: Automagic Art

## 🎯 Core Features

### 8-Player MIDI Engine
- **Independent Players**: Each with dedicated MIDI playback, SFZ sample engine, and audio mixer
- **Pattern Management**: 4×4 grid system with groups and fills
- **Real-time Control**: Comprehensive MIDI CC support and parameter automation
- **Host Synchronization**: Seamless integration with DAW environments

### Professional Audio Processing
- **SFZ 2.0 Engine**: Industry-standard sample playback with advanced mapping
- **Multi-output Routing**: Configurable channel assignment (stereo to 32 channels)
- **Effects Chain**: Individual effects processing for each drum element
- **High-quality Audio**: Low-latency processing with professional-grade quality

### Cross-Platform Support
| Platform | Formats | Architecture | Status |
|----------|---------|--------------|---------|
| **macOS** | Standalone, VST3, AU, CLAP | Universal (Intel + Apple Silicon) | ✅ Production Ready |
| **Windows** | Standalone, VST3, CLAP | x64 | ✅ Production Ready |
| **Linux** | Standalone, VST3, CLAP | x86_64, ARM64 | ✅ Production Ready |
| **iOS** | Standalone, AUv3 | arm64 + Simulator | ✅ App Store Ready |
| **Android** | Standalone | arm64-v8a, armeabi-v7a | ✅ Play Store Ready |

## 📁 Repository Structure

### Root Directory
```
OTTO/
├── Source/                     # C++ source code and headers
├── Assets/                     # Embedded resources (fonts, graphics, samples)
├── docs/                       # Comprehensive documentation
├── scripts/                    # Build and development automation
├── cmake/                      # Platform-specific CMake configurations
├── JUCE/                       # JUCE 8 framework (git submodule)
├── Builds/                     # Generated build outputs
├── e2e-test/                   # End-to-end testing suite
├── docker/                     # Docker containerization
└── CMakeLists.txt              # Main CMake configuration
```

### Source Code Organization
```
Source/
├── Core Components/
│   ├── MainContentComponent.*     # Main UI container (6-row layout)
│   ├── PluginProcessor.*          # Audio processing engine
│   ├── PluginEditor.*             # Plugin interface
│   └── StateManager.*             # Application state management
├── UI Components/
│   ├── TopBarComponent.*          # Transport and global controls
│   ├── PlayerTabsComponent.*      # 8-player selection interface
│   ├── DrumKitSectionComponent.*  # Kit selection and controls
│   ├── SceneLauncherComponent.*   # Pattern trigger interface
│   ├── LoopSectionComponent.*     # Timeline controls
│   └── MainContentComponent*Section.* # Left/right panel components
├── Audio Engine/
│   ├── SFZEngine.*                # Sample playback engine
│   ├── MidiEngine.*               # MIDI pattern processing
│   ├── Mixer.*                    # Audio mixing and routing
│   └── SFZVoice.*                 # Voice allocation system
├── Data Management/
│   ├── INIConfig.h                # Central configuration constants
│   ├── INIDataManager.*           # INI file I/O operations
│   ├── PresetManager.*            # Preset loading/saving
│   └── PatternManager.*           # MIDI pattern management
├── UI Systems/
│   ├── FontManager.*              # Professional font management
│   ├── ColorScheme.*              # Theme and color system
│   ├── CustomLookAndFeel.*        # JUCE 8 UI styling
│   └── ResponsiveLayoutManager.*  # Proportional scaling system
└── Platform Support/
    ├── JUCE8Compatibility.h       # JUCE 8 compliance enforcement
    ├── WindowsErrorHandling.*     # Windows-specific error handling
    └── PlatformAwareLookAndFeel.* # Platform-specific UI adaptations
```

### Documentation Structure
```
docs/
├── Core Documentation/
│   ├── README.md                  # Documentation index
│   ├── REPO.md                    # Repository guidelines
│   ├── CHANGELOG.md               # Project changelog
│   └── BUILDRULES.md              # Build system instructions
├── Development Guidelines/
│   ├── ZENCODER_RULES.md          # AI assistant guidelines
│   ├── DESIGNRULES.md             # UI design standards
│   ├── INIRULES.md                # Data management system
│   └── JUCE_SETUP_INSTRUCTIONS.md # JUCE 8 setup guide
├── Implementation Reports/
│   ├── INI_LAYOUT_SYSTEM_COMPLETION.md # Layout system implementation
│   ├── PHOSPHOR_SCALING_FIX_COMPLETED.md # Icon scaling fixes
│   ├── MENU_SELECTION_FIX.md      # Font compatibility solutions
│   └── SCRIPT_ORGANIZATION_COMPLETE.md # Build script organization
└── Technical Analysis/
    ├── MINIMUM_SYSTEM_REQUIREMENTS.md # System requirements
    ├── CURRENT_SYSTEM_ANALYSIS.md # Architecture analysis
    └── PHASE_*_COMPLETION_REPORT.md # Development phase reports
```

### Build System Structure
```
scripts/
├── build/                      # Platform-specific build scripts
│   ├── build_macos.sh          # macOS Universal builds
│   ├── build_ios.sh            # iOS Device & Simulator
│   ├── build_linux.sh          # Linux native builds
│   ├── build_android.sh        # Android ARM builds
│   ├── build_windows.bat       # Windows x64 builds
│   └── build_all.sh            # Multi-platform orchestrator
├── setup/                      # Environment configuration
│   ├── setup_juce8.sh          # JUCE 8 automatic setup
│   └── get_team_id.sh          # iOS development setup
├── tools/                      # Development utilities
│   ├── fix_clion_permanent.sh  # CLion IDE configuration
│   └── reset_clion_cmake.sh    # CMake cache management
├── testing/                    # Testing automation
│   └── validate_phase_2_3_x.sh # Build validation
└── development/                # Development convenience
    ├── go.sh                   # Quick navigation
    └── gonocomments.sh         # Code analysis
```

## 🎨 Design System

### INI-Driven Layout Architecture
OTTO implements a **100% INI-driven layout system** with zero hardcoded values:

- **6-Row Interface**: TopBar (8%) → PlayerTabs (15%) → Player Controls (12%) → Pattern Controls (14%) → Main Matrix (45%) → Loop Controls (12%)
- **Responsive Design**: Percentage-based calculations relative to base resolution (1024×768)
- **Accessibility Compliance**: 44px minimum touch targets meeting WCAG 2.1 standards
- **Cross-Platform Consistency**: Identical appearance across all supported platforms

### Professional UI Components
- **FontManager**: Embedded professional fonts (Roboto, Montserrat, Orbitron, Open Sans)
- **Phosphor Icon System**: 6 weight variations for comprehensive iconography
- **ColorScheme**: Centralized theme management with runtime switching
- **CustomLookAndFeel**: JUCE 8 compliant styling system

### Layout Constants Structure
```cpp
namespace INIConfig::LayoutConstants {
    // Interface Dimensions
    constexpr int defaultInterfaceWidth = 1024;
    constexpr int defaultInterfaceHeight = 768;
    
    // Row Heights (percentages)
    constexpr float row1HeightPercent = 8.0f;   // Top Bar
    constexpr float row2HeightPercent = 15.0f;  // Player Tabs
    constexpr float row3HeightPercent = 12.0f;  // Player Controls
    constexpr float row4HeightPercent = 14.0f;  // Pattern Controls
    constexpr float row5HeightPercent = 45.0f;  // Main Matrix
    constexpr float row6HeightPercent = 12.0f;  // Loop Controls
}
```

## 🔧 Technical Specifications

### JUCE 8 Compliance
- **Mandatory JUCE 8.0.8+**: Enforced at compile-time with version checks
- **Modern C++20**: Advanced language features and patterns
- **Deprecated Method Detection**: Compile-time errors for deprecated JUCE methods
- **Font Creation Standards**: JUCE 8 FontOptions API exclusively

### Build System Features
- **Unified CMake**: Single configuration for all platforms
- **Advanced Safety Systems**: Memory protection and error recovery
- **Platform-Specific Optimizations**: Windows Parallels VM, iOS Enhanced, Android Enhanced
- **Automated Testing**: Comprehensive validation and E2E test suites

### Performance Specifications
| Metric | Specification |
|--------|---------------|
| **Audio Latency** | < 5ms (with appropriate buffer settings) |
| **CPU Usage** | < 5% (typical), < 15% (maximum) |
| **Memory Usage** | < 50MB (typical), < 100MB (maximum) |
| **Layout Timing** | < 5ms (responsive layout calculations) |
| **Sample Rates** | 44.1kHz - 192kHz |
| **Bit Depth** | 16-bit, 24-bit, 32-bit float |

## 💾 Data Management System

### INI-Based Configuration
OTTO uses a comprehensive INI system for all settings and presets:

```
OTTO_Data/
├── Settings/           # Global application settings
├── Presets/           # User-created plugin presets
├── Patterns/          # MIDI pattern files
├── Kits/             # Drum kit configurations
├── Mix/              # Mixer and effects presets
└── System/           # System configuration files
```

### Key Features
- **Human-Readable Format**: Plain text INI files for easy editing
- **Version Control Friendly**: Text-based format for Git tracking
- **Complete State Capture**: All application state preserved
- **Cross-Platform Compatibility**: Consistent data format across platforms

## 🚀 Development Workflow

### Quick Start Commands
```bash
# Setup JUCE 8 environment
./scripts/setup/setup_juce8.sh

# Single platform builds
./scripts/build/build_macos.sh           # macOS Universal
./scripts/build/build_ios.sh --simulator # iOS Simulator
./scripts/build/build_linux.sh           # Linux native
./scripts/build/build_android.sh         # Android ARM
./scripts/build/build_windows.bat        # Windows x64

# Multi-platform build
./scripts/build/build_all.sh --all       # All platforms
```

### Development Constraints
- ✅ **JUCE 8 + CMake**: Modern C++20 patterns, unified build system
- ✅ **INI-Driven Layout Priority**: All positioning uses `INIConfig::LayoutConstants`
- ✅ **FontManager/ColorScheme**: All UI styling through proper JUCE 8 systems
- ❌ **No CMake Modifications**: Work within existing build configuration
- ❌ **No Hardcoded Values**: Everything must be configurable via INI system

## 🤖 AI Assistant Guidelines

### Critical Requirements
Before working on this project, AI assistants must:

1. **Read [docs/ZENCODER_RULES.md](docs/ZENCODER_RULES.md)** - System constraints and development guidelines
2. **Review [docs/DESIGNRULES.md](docs/DESIGNRULES.md)** - INI-driven layout requirements
3. **Understand [docs/BUILDRULES.md](docs/BUILDRULES.md)** - Build system configuration
4. **Follow [docs/INIRULES.md](docs/INIRULES.md)** - Data management system

### Key Development Patterns
```cpp
// ✅ CORRECT - INI-driven layout
auto buttonWidth = static_cast<int>(getWidth() * 
    INIConfig::LayoutConstants::buttonWidthPercent / 100.0f);

// ❌ WRONG - Hardcoded values
auto buttonWidth = 120; // Never do this!

// ✅ CORRECT - JUCE 8 font creation
auto font = JUCE8_FONT(14.0f);

// ❌ WRONG - Deprecated JUCE font constructor
auto font = juce::Font(14.0f); // Compile error!
```

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

## 📊 Project Status

### Completed Phases
- ✅ **Phase 2.3.x**: Cross-Platform Foundation
- ✅ **Phase 3-4**: Core Engine Implementation
- ✅ **Phase 5-6**: Complete INI-Driven Layout System
- ✅ **Production Ready**: All platforms building and validated

### Current State
- **Build Status**: ✅ Complete - All platforms building successfully
- **Documentation**: ✅ Comprehensive - All systems documented
- **Testing**: ✅ Validated - E2E test suite implemented
- **Distribution**: ✅ Ready - Open source distribution prepared

### Recent Improvements
- **Menu Selection Fix**: Resolved character corruption in preset menus
- **Font System Enhancement**: Reduced preset font sizes by 40% for better readability
- **Phosphor Icon Scaling**: Fixed icon scaling across all platforms
- **Script Organization**: Complete reorganization of build scripts into logical categories

## 🔗 Important Links

- **Main Documentation**: [README.md](README.md)
- **Build Instructions**: [docs/BUILDRULES.md](docs/BUILDRULES.md)
- **Design Guidelines**: [docs/DESIGNRULES.md](docs/DESIGNRULES.md)
- **AI Assistant Rules**: [docs/ZENCODER_RULES.md](docs/ZENCODER_RULES.md)
- **Data Management**: [docs/INIRULES.md](docs/INIRULES.md)
- **JUCE 8 Setup**: [docs/JUCE_SETUP_INSTRUCTIONS.md](docs/JUCE_SETUP_INSTRUCTIONS.md)

---

**OTTO** represents a complete, professional-grade drum machine solution built with modern development practices and designed for the future of cross-platform audio production. The repository demonstrates best practices in JUCE 8 development, responsive UI design, and comprehensive build automation.