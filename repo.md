# OTTO - Professional Drum Machine & Sample Playback System

[![JUCE](https://img.shields.io/badge/JUCE-8.0+-blue.svg)](https://juce.com/)
[![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Windows%20%7C%20Linux%20%7C%20iOS%20%7C%20Android-lightgrey.svg)](#platform-support)
[![License](https://img.shields.io/badge/license-Commercial-green.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](#build-system)

## Overview

**OTTO** is a powerful, professional-grade drum machine and sample playback system designed for modern music production. Built with JUCE 8 and featuring a unified CMake build system, OTTO delivers consistent performance across all major platforms while maintaining the flexibility of both plugin and standalone operation.

### Key Features

- **🎵 8 Independent MIDI Players**: Each with synchronized MIDI playback (host sync or standalone), dedicated SFZ sample engine, unique sounds, and individual audio mixer with multiple outputs and effects
- **🎛️ Advanced Pattern Management**: 4×4 grids of MIDI beat patterns organized in groups of 16 for complex song arrangements
- **🤖 Smart Fill System**: Automatic and manual drum fills with intelligent pattern variations
- **🎚️ Professional Audio Engine**: SFZ 2.0 sample playback with dedicated effects for each drum kit element (kick, snare, hi-hat, toms, cymbals, etc.)
- **🌐 True Cross-Platform**: Native support for macOS, Windows, Linux, iOS, and Android
- **🔌 Universal Plugin Support**: VST3, AU, AUv3, CLAP, and Standalone versions
- **🎨 Modern Interface**: Responsive GUI with embedded graphics, custom fonts, and Phosphor icon system
- **💾 Flexible Storage**: Reaper-style INI plain text database for easy preset management
- **🤖 AI Beat Generation**: Intelligent pattern creation and suggestion capabilities

## Technical Architecture

### Core Components

- **JUCE 8 Framework**: Modern cross-platform audio application framework with unified CMake build system
- **SFZ Engine**: Industry-standard sample playback with full SFZ 2.0 specification support
- **MIDI Engine**: Precision pattern playback, quantization, and host synchronization
- **Audio Processing**: High-quality convolution and audio processing effects with individual channel routing
- **User Interface**: Modern, responsive GUI with embedded assets and real-time parameter control
- **Asset Management**: Hybrid system with embedded UI assets and file-based user content

### Audio Processing Flow

```
MIDI Input → Pattern Selection & Timing → Fill Logic Insertion → 
SFZ Engine (MIDI to Audio) → Individual Mixers → Effects Processing → 
Multi-Output Routing (Stereo to 32 channels per player) → Audio Output
```

### Plugin Characteristics

- **Type**: Synthesizer with MIDI Effects capabilities
- **MIDI**: Full input/output support with custom channel mapping
- **Audio**: Multi-output support (up to 32 channels per player engine)
- **Formats**: VST3, AU, AUv3 (iOS), CLAP, Standalone
- **Company**: Automagic Art
- **Bundle ID**: com.automagicart.OTTO

## Build System

### Unified CMake Architecture ✅

OTTO features a **revolutionary unified build system** that ensures identical results whether using ProJucer+Xcode or CMake+CLion workflows:

- **Single Source of Truth**: One `CMakeLists.txt` for all platforms and build configurations
- **Perfect IDE Integration**: Seamlessly supports both ProJucer/Xcode and CMake/CLion workflows
- **Consistent Output Structure**: All builds organized in standardized `/Builds` directory
- **Cross-Platform Compatibility**: Supports all target platforms with platform-specific optimizations

### Platform Support

| Platform | Architecture | Formats | Status |
|----------|-------------|---------|--------|
| **macOS** | Universal (Intel + Apple Silicon) | VST3, AU, Standalone | ✅ Full Support |
| **iOS** | arm64, Simulator (x86_64/arm64) | AUv3, Standalone | ✅ Full Support |
| **Windows** | x64, x86 | VST3, Standalone | ✅ Full Support |
| **Linux** | x86_64, ARM | VST3, Standalone | ✅ Full Support |
| **Android** | arm64-v8a, armeabi-v7a, x86, x86_64 | Shared Library | ✅ Full Support |

### Quick Start Building 🚀

```bash
# Platform-specific builds
./build_macos.sh           # macOS Universal → Builds/MacOSX/Release/
./build_ios.sh --simulator # iOS Simulator → Builds/iOS/Release/
./build_linux.sh           # Linux → Builds/LinuxMakefile/Release/
./build_android.sh         # Android → Builds/Android/Release/
./build_windows.bat        # Windows → Builds/VisualStudio2022/Release/

# Multi-platform build
./build_all.sh --all       # All platforms → Organized Builds/ structure

# See BUILD_QUICK_START.md for complete reference
```

### Build Output Structure 📁

All build outputs are automatically organized in the `Builds/` directory:
```
Builds/
├── MacOSX/Release/              # macOS Universal Binary (Intel + Apple Silicon)
│   ├── VST3/OTTO.vst3          # VST3 plugin
│   ├── AU/OTTO.component       # Audio Unit plugin  
│   └── Standalone/OTTO.app     # Standalone application
├── iOS/Release/                 # iOS builds
│   ├── Standalone/OTTO.app     # iOS standalone app
│   └── AUv3/OTTO.appex        # AUv3 plugin extension
├── LinuxMakefile/Release/       # Linux builds
├── Android/Release/             # Android builds
└── VisualStudio2022/Release/   # Windows builds
```

## Player Engine Architecture

Each of OTTO's 8 MIDI players is a complete audio production unit featuring:

### MIDI Processing
- **Note Events**: Drum mapping (kick, snare, hi-hat, toms, cymbals, etc.)
- **Precision Timing**: Sample-accurate timing with host synchronization
- **Velocity Control**: Full 0-127 range with Energy slider modification (10%-200%)
- **Pattern Length**: Unlimited pattern duration support
- **Custom MIDI Mapping**: Configurable input/output channel routing
- **AI Beat Generation**: Intelligent pattern creation and suggestion capabilities

### Audio Engine
- **SFZ Sample Playback**: Industry-standard sample triggering and playback
- **Individual Mixing**: Dedicated mixer and effects chain per player
- **Multi-Output Routing**: Configurable output routing (stereo to 32 channels)
- **Real-time Effects**: Send/return and insert effects for each drum element

### Player Controls

Each player provides comprehensive control over:

1. **Transport Controls**: Stop, Record, Play, Pause, Punch In/Out, Import/Export, Locate Points
2. **Recording**: Locked, Replace, or Overdub modes with MIDI channel selection (1-16)
3. **Synchronization**: Off, Host Sync, or Trigger sync modes
4. **Quantization**: Primary/Secondary quantize with 0-100% amount control
5. **Groove Processing**: Swing (0-100%), Humanize (0-100%) with musical quantization
6. **MIDI Processing**: Transpose (-24 to +24), Velocity Shift (-64 to +64), Timing Shift (±10000 samples)
7. **Drum Conversion**: Intelligent drum mapping with note conversion (0-127 for each element)
8. **Real-time Control**: Sample position monitoring, crash/mute triggers, power control
9. **Energy Levels**: 7-level system (Asleep=0, Chilled=1, Relaxed=2, Normal=3, Aroused=4, Excited=5, Energetic=6)

## SFZ 2.0 Integration

OTTO uses the industry-standard SFZ 2.0 (Simple File Format) for sample playback. An SFZ file is a text file that defines:

- **Samples**: Which audio files to use (WAV, AIFF, etc.)
- **Mapping**: Which MIDI notes trigger which samples
- **Parameters**: Volume, pitch, filtering, envelopes
- **Conditions**: Velocity ranges, round-robin samples
- **Advanced Features**: Full SFZ 2.0 specification support

## Asset Management System

OTTO uses a sophisticated hybrid asset management approach:

### Embedded Assets (BinaryData)
- **UI Graphics**: Button080.png, Button100.png, Slider graphics, Splash screen, OTTO icons
- **Typography**: Complete font families (Roboto, Montserrat, Orbitron, Playfair Display, Open Sans)
- **Icon System**: Full Phosphor icon font collection (6 weights: Thin, Light, Regular, Bold, Fill, Duotone)

### File-Based Assets (User Customizable)
- **Drum Samples**: WAV files organized by kit in `Assets/Drumkits/`
- **SFZ Definitions**: Drum kit mappings in `Assets/Drumkits/`
- **MIDI Patterns**: Beat patterns in `Assets/MidiFiles/`
- **User Presets**: Custom plugin settings and configurations

## Preset System

OTTO's preset system provides comprehensive state management:

- **Complete State Capture**: Current MIDI groove group, beat patterns, fill patterns, fill frequency
- **Audio Settings**: Drum kit selection, energy levels, swing, volume, mixer settings
- **Effects State**: Individual effect settings for each player and drum element
- **INI Format**: Human-readable plain text format for easy editing and version control

## Development Workflow

### Option 1: ProJucer + Native IDEs
1. Use ProJucer for project management and configuration
2. Open generated .xcodeproj/.sln files in Xcode/Visual Studio
3. Build and debug using native IDE tools

### Option 2: CMake + Modern IDEs
1. Open project in CMake-compatible IDE (CLion, VSCode, etc.)
2. Select appropriate CMake preset for target platform
3. Build and debug using CMake tools and integrated debugging

### Option 3: Command Line + CI/CD
1. Use automated build scripts for continuous integration
2. Multi-platform builds with consistent output structure
3. Perfect for release workflows and automated testing

## File Structure

```
OTTO/
├── Assets/                      # User-customizable content
│   ├── Drumkits/               # Drum sample libraries and SFZ files
│   ├── Fonts/                  # Typography resources
│   ├── GUI/                    # UI graphics and themes
│   ├── MidiFiles/              # MIDI pattern libraries
│   └── Docs/                   # Documentation assets
├── Builds/                      # Build output directory
│   ├── MacOSX/                 # macOS builds
│   ├── iOS/                    # iOS builds  
│   ├── LinuxMakefile/          # Linux builds
│   ├── VisualStudio2022/       # Windows builds
│   └── Android/                # Android builds
├── Source/                      # C++ source code
│   ├── PluginProcessor.h/cpp   # Main audio processor
│   ├── PluginEditor.h/cpp      # UI editor component
│   ├── MidiEngine.h/cpp        # MIDI processing engine
│   ├── SFZEngine.h/cpp         # SFZ sample playback
│   ├── Mixer.h/cpp             # Audio mixing and routing
│   ├── FontManager.h/cpp       # Font loading and management
│   ├── CustomLookAndFeel.h/cpp # UI theming and graphics
│   ├── PresetManager.h/cpp     # State management
│   ├── StateManager.h/cpp      # Application state handling
│   ├── ColorScheme.h/cpp       # UI color management
│   ├── PopupWindows*.h/cpp     # Contextual UI dialogs
│   ├── MainContentComponent*.h/cpp # Primary UI components
│   ├── PlayerTabsComponent.h/cpp # Multi-player interface
│   ├── DrumKitSectionComponent.h/cpp # Drum kit selection
│   ├── LoopSectionComponent.h/cpp # Pattern controls
│   ├── SceneLauncherComponent.h/cpp # Performance interface
│   ├── SettingsPanel.h/cpp     # Configuration UI
│   ├── AIAssistantPanel.h/cpp  # AI features interface
│   ├── AutoMixAssistant.h/cpp  # Automatic mixing
│   ├── CommunityBrowser.h/cpp  # Preset sharing
│   ├── CollaborationPanel.h/cpp # User collaboration
│   ├── UtilityComponents.h/cpp # Helper UI elements
│   ├── ErrorHandling.h         # Error management
│   ├── INIConfig.h             # Configuration management
│   ├── INIDataManager.h/cpp    # Data persistence
│   ├── INIUtils.h              # INI file utilities
│   ├── MidiFileManager.h/cpp   # MIDI file handling
│   ├── PatternManager.h/cpp    # Pattern organization
│   ├── PatternSuggestionEngine.h/cpp # AI pattern generation
│   ├── TopBarComponent.h/cpp   # Main toolbar
│   ├── SFZVoice.h/cpp         # SFZ voice implementation
│   ├── SFZVoiceAllocator.h/cpp # Voice management
│   ├── ComponentState.h        # UI state management
│   ├── MidiAnalysisTypes.h     # MIDI analysis structures
│   ├── LatencyCompensation.h   # Audio timing correction
│   ├── PerformanceOptimizations.h # Performance enhancements
│   ├── AdvancedAILearning.h/cpp # Machine learning features
│   ├── ResponsiveLayoutManager.h/cpp # Adaptive UI layout
│   ├── CloudServiceManager.h   # Cloud integration
│   └── tests/                  # Unit tests
├── JuceLibraryCode/            # Generated JUCE integration
│   ├── BinaryData.h            # Asset declarations
│   ├── BinaryData.cpp          # Embedded asset data (part 1)
│   ├── BinaryData2.cpp         # Embedded asset data (part 2)
│   ├── BinaryData3.cpp         # Embedded asset data (part 3)
│   ├── BinaryData4.cpp         # Embedded asset data (part 4)
│   ├── BinaryData5.cpp         # Embedded asset data (part 5)
│   └── JuceHeader.h           # JUCE module includes
├── cmake/                       # CMake configuration files
│   ├── iOS-Device.cmake        # iOS device build configuration
│   └── iOS-Simulator.cmake     # iOS simulator build configuration
├── docs/                        # Comprehensive documentation
│   └── ASSET_MANAGEMENT.md     # Asset system documentation
├── JUCE/                        # JUCE 8 submodule
├── vscode-build-debug/         # VSCode build artifacts
├── .zencoder/                  # Zencoder AI assistant files
├── CMakeLists.txt              # Main build configuration
├── CMakePresets.json           # IDE integration presets
├── build_*.sh/bat              # Platform-specific build scripts
├── setup_juce8.sh             # JUCE 8 setup automation
├── BUILD_SYSTEM.md             # Build system documentation
├── BUILD_INSTRUCTIONS.md       # Detailed build guide
├── BUILD_QUICK_START.md        # Quick start guide
├── BUILD_CONSOLIDATION_SUMMARY.md # Build system summary
├── CHANGELOG.md                # Version history
├── COMMIT_MESSAGE.md           # Git commit guidelines
├── EFFICIENCY_REPORT.md        # Performance analysis
├── ENHANCEMENT_ROADMAP.md      # Future development plans
└── LICENSE                     # Software license
```

## System Requirements

### Development Requirements
- **CMake**: 3.22 or later
- **C++ Standard**: C++17 (required for JUCE 8)
- **JUCE**: 8.0+ (included as submodule)

### Platform-Specific Requirements

#### macOS
- **Xcode**: 12.0 or later
- **macOS**: 10.13 or later (deployment target)
- **Architectures**: Universal Binary (Intel + Apple Silicon)

#### iOS
- **iOS SDK**: 12.0 or later
- **Xcode**: 12.0 or later with iOS development tools
- **Code Signing**: Apple Developer account for device deployment

#### Windows
- **Visual Studio**: 2019 or 2022 (Community/Professional/Enterprise)
- **Windows SDK**: 10.0 or later
- **Target**: Windows 10/11 (x64/x86)

#### Linux
- **GCC**: 7.0+ or **Clang**: 6.0+
- **Dependencies**: Auto-installed for Ubuntu/Debian, CentOS/RHEL, Arch Linux
- **Libraries**: ALSA, GTK3, FreeType2, X11

#### Android
- **Android NDK**: r21 or later
- **Android SDK**: API Level 23+ (configurable)
- **CMake**: Android CMake tools
- **Architectures**: arm64-v8a, armeabi-v7a, x86, x86_64

## JUCE 8 Compliance

OTTO leverages all modern JUCE 8 features and best practices:

- **Modern CMake Integration**: Native JUCE 8 CMake support
- **Proper Module System**: Clean module linking and dependency management
- **Platform Optimization**: Platform-specific compiler flags and optimizations
- **Asset Management**: Modern BinaryData handling with memory-based loading
- **Plugin Architecture**: Latest plugin format specifications and APIs
- **Code Signing**: Automated code signing and notarization support

## Advanced Features

### AI and Automation
- **AI Assistant Panel**: Intelligent pattern generation and suggestion
- **Auto Mix Assistant**: Automatic mixing parameter optimization
- **Pattern Suggestion Engine**: Context-aware rhythm recommendations
- **Advanced AI Learning**: Machine learning-based pattern analysis

### Performance Features
- **Scene Launcher**: Live performance pattern triggering
- **Transport Synchronization**: Seamless DAW integration
- **Real-time Parameter Control**: Low-latency automation support
- **Multi-output Architecture**: Professional mixing workflow support
- **Latency Compensation**: Automatic timing correction
- **Performance Optimizations**: CPU and memory usage optimization

### Customization and Extensibility
- **Theme System**: Customizable color schemes and visual styles
- **MIDI Mapping**: Flexible controller assignment
- **SFZ Library Support**: Compatible with industry-standard sample libraries
- **Preset Sharing**: Import/export functionality for pattern and setting exchange
- **Community Browser**: Online preset and pattern sharing
- **Collaboration Panel**: Real-time collaboration features
- **Cloud Service Manager**: Cloud-based preset synchronization

### User Interface Features
- **Responsive Layout Manager**: Adaptive UI scaling and arrangement
- **Custom Group Manager**: Flexible pattern organization
- **Multiple Popup Systems**: Contextual editors for Audio, MIDI, Mixer, Theme, Backup, Editor, Groove, Settings, and Performance
- **Top Bar Component**: Main application controls and status
- **Utility Components**: Helper UI elements and widgets

## Performance Optimization

OTTO is optimized for professional audio production:

- **Low-latency Audio Processing**: Optimized for real-time performance
- **Efficient Memory Usage**: Smart pointer management and RAII patterns
- **Multi-threaded Architecture**: Separate threads for audio, MIDI, and UI
- **Cache-Friendly Design**: Optimized data structures for performance
- **Platform-specific Optimizations**: SSE/AVX instructions where available

## Testing and Quality Assurance

- **Unit Testing**: Comprehensive test coverage for core components
- **Cross-platform Testing**: Validation on all supported platforms
- **Memory Safety**: Valgrind and AddressSanitizer integration
- **Performance Profiling**: Continuous performance monitoring
- **Audio Quality Testing**: THD+N and frequency response validation

## Contributing

OTTO follows modern C++ and JUCE best practices:

1. **Code Style**: Follow JUCE coding conventions
2. **JUCE 8 APIs**: Use modern JUCE 8 methods and patterns
3. **Cross-Platform**: Ensure all changes work across supported platforms
4. **Testing**: Test on multiple platforms before submitting
5. **Documentation**: Update relevant documentation for changes

## License

OTTO is commercial software developed by Automagic Art. See [LICENSE](LICENSE) for full license terms.

## Support & Documentation

- **Build Instructions**: [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)
- **Quick Start Guide**: [BUILD_QUICK_START.md](BUILD_QUICK_START.md)  
- **Build System Details**: [BUILD_SYSTEM.md](BUILD_SYSTEM.md)
- **Asset Management**: [docs/ASSET_MANAGEMENT.md](docs/ASSET_MANAGEMENT.md)
- **Changelog**: [CHANGELOG.md](CHANGELOG.md)
- **Enhancement Roadmap**: [ENHANCEMENT_ROADMAP.md](ENHANCEMENT_ROADMAP.md)
- **Efficiency Report**: [EFFICIENCY_REPORT.md](EFFICIENCY_REPORT.md)

---

**OTTO** represents the next generation of drum machine technology, combining the power and flexibility of modern audio processing with the reliability and performance of JUCE 8's cross-platform framework. Whether you're producing in your home studio or performing live, OTTO delivers the professional-grade drum sounds and features you need to create exceptional music.