# OTTO - Professional Multi-Platform Drum Machine & Sample Playback System

## Project Overview

**OTTO** is a powerful, professional-grade drum machine and sample playback system built with JUCE 8. It features 8 independent MIDI players, each with its own SFZ-based sample engine, advanced pattern management, and intelligent fill generation. OTTO provides comprehensive audio processing capabilities with per-element effects, professional mixing, and multi-output support across all major platforms.

## Key Features

### Core Functionality
- **8 Independent MIDI Players**: Each with synchronized MIDI playback (host-synced or standalone), dedicated SFZ sample engine, unique sounds, and individual audio/MIDI mixers with multiple outputs and FX
- **Professional Pattern Management**: 4x4 grids of MIDI beat patterns organized in groups of 16, with intelligent fill insertion system
- **Smart Fill System**: Automatic and manual drum fills with configurable timing (every 2, 4, 8, or 16 bars at "x" bars from the end of the loop)
- **SFZ 2.0 Sample Engine**: Industry-standard sample playback with comprehensive effects for each drumkit element (kick, snare, hat, toms, cymbals, etc.)
- **Advanced Audio Processing**: High-quality convolution and audio processing effects with send/return and insert routing for each drum element
- **Intelligent Preset System**: Complete state management including MIDI patterns, drumkits, energy levels, swing, mixer settings, and FX configurations
- **AI Beat Generation**: Intelligent pattern creation and variation capabilities

### Platform Support
- **Cross-Platform Compatibility**: Native support for macOS (Universal Binary), Windows, Linux, iOS, and Android
- **Multiple Plugin Formats**: VST3, AU (Audio Units), CLAP, Standalone application, and iOS AUv3
- **Advanced Build System**: CMake-based cross-platform build system with automated setup scripts

### User Interface
- **Modern Responsive GUI**: Custom look-and-feel with embedded graphics and Phosphor icon fonts
- **Professional Color Schemes**: Multiple themed interfaces optimized for different lighting conditions
- **Embedded Asset Management**: All UI elements, fonts, and graphics embedded in binary for consistent experience
- **Touch-Optimized Interface**: Full support for touch-based interaction on iOS and Android

## Technical Architecture

### Core Framework
- **JUCE 8 Framework**: Modern cross-platform audio application framework with full CMake integration
- **C++17 Standard**: Modern C++ with smart pointer memory management and RAII patterns
- **Modular Design**: Component-based architecture with clear separation of concerns
- **ProJucer Integration**: Project configuration and build system management with CMake support

### Core Engine Components

#### 1. **MIDI Engine** (`MidiEngine.h/cpp`)
- Pattern playback and timing management
- Note event processing (drum mapping, timing, velocity, length)
- Velocity curve processing (Linear, Exponential, Logarithmic, S-Curve, Fixed)
- Advanced timing features: swing, humanization, quantization
- Transport control with multiple sync modes (Off, Host, Trigger)

#### 2. **SFZ Engine** (`SFZEngine.h/cpp`)
- Industry-standard SFZ 2.0 sample format support
- Multi-sample and round-robin playback
- Advanced sample mapping with velocity ranges and conditions
- High-quality sample interpolation and pitch shifting

#### 3. **Audio Processing Pipeline**
**Multi-Engine Architecture**: 8 independent audio processing engines, each capable of 32-channel output
```
MIDI Input → Pattern Selection → Fill Logic → SFZ Engine → Mixer → Effects → Multi-Output
```
- **Fill Logic**: Intelligent insertion of fill patterns based on musical context
- **Multi-Output**: Flexible routing from stereo to 32 channels per player engine

#### 4. **Mixer System** (`Mixer.h/cpp`)
- Individual channel processing for each drum element
- Send/return effects architecture
- Multi-output routing (stereo to 32 channels per player)
- Real-time parameter automation

#### 5. **Preset Management** (`PresetManager.h/cpp`)
- Comprehensive state management
- INI-based configuration system
- Session recall with complete parameter restoration

### User Interface Architecture

#### Core UI Components
- **Main Content Component**: Primary interface container with responsive layout
- **Player Tabs Component**: Multi-player management interface
- **Drum Kit Section**: Sample selection and management
- **Loop Section**: Pattern and timing controls
- **Scene Launcher**: Performance-oriented pattern triggering
- **Settings Panel**: Configuration and preferences management

#### Advanced UI Features
- **Custom Look and Feel**: Modern, professional interface design
- **Responsive Layout Manager**: Adaptive UI scaling and arrangement
- **Pop-up Window System**: Contextual editing interfaces for audio, MIDI, mixer, theme, and performance settings
- **Real-time Visual Feedback**: Comprehensive parameter visualization

## Project Structure

```
OTTO/
├── Source/                 # Main C++ source code
│   ├── PluginProcessor.*   # Main audio processor
│   ├── PluginEditor.*      # Main UI editor
│   ├── MidiEngine.*        # MIDI processing engine
│   ├── SFZEngine.*         # Sample playback engine
│   ├── Mixer.*             # Audio mixing system
│   ├── PresetManager.*     # State management
│   └── UI Components/      # User interface components
├── Assets/                 # Resources and assets
│   ├── GUI/               # UI graphics and themes
│   ├── Fonts/             # Typography resources
│   ├── Drumkits/          # Sample libraries
│   └── MidiFiles/         # Pattern templates
├── Builds/                # Platform-specific build configurations
│   ├── MacOSX/           # Xcode project files
│   ├── iOS/              # iOS-specific builds
│   ├── VisualStudio2022/ # Windows build configuration
│   └── Android/          # Android build system
├── JuceLibraryCode/      # Auto-generated JUCE framework code
├── OTTO.jucer            # Main ProJucer project file
└── README.md             # Project documentation
```

## Current Build Status & Quick Start ✅

### Build Status
- **Graphics & Fonts**: All UI elements, custom fonts, and Phosphor icons properly embedded and displaying
- **Cross-Platform Builds**: CMake-based build system supports macOS, Windows, Linux, iOS, Android
- **Plugin Formats**: VST3, AU, and Standalone all building and installing correctly
- **Asset Management**: Critical UI assets embedded in binary, user content (samples, patterns) still file-based

### Quick Start Building
```bash
# Setup JUCE 8 and dependencies
./setup_juce8.sh

# Build for your platform
./build_macos.sh      # macOS Universal Binary (Intel + Apple Silicon)
./build_ios.sh        # iOS device and simulator
./build_linux.sh      # Linux with dependency management
./build_android.sh    # Android with NDK
./build_windows.bat   # Windows with Visual Studio
./build_all.sh --all  # Cross-platform build orchestration

# See BUILD_INSTRUCTIONS.md for detailed build information
```

## Player Engine Control System

Each of the 8 MIDI players provides comprehensive control over:

### MIDI Playback Features
- **Note Events**: Which drum to hit (kick, snare, hi-hat, etc.)
- **Timing**: When each hit occurs (measured in ticks, beats, or samples)
- **Velocity**: How hard each hit should be (0-127) modified from 10% to 200% by the "Energy" slider (defaults to 100%)
- **Length**: Total pattern duration (no limit in length)
- **Custom MIDI I/O**: Configurable MIDI input and output mappings
- **AI Beat Generation**: Intelligent pattern creation and variation

### Transport & Recording Controls
1. **Reset**: Full player reset (bool)
2. **Transport**: Stop, Record, Play, Pause, Punch In, Punch Out, Import, Export, Ignore, Locate Zero, Locate 1-4
3. **Recording Mode**: Locked=0, Replace, Overdub
4. **Record MIDI Channel**: 1-16
5. **Monitor**: Real-time monitoring (bool)
6. **Locate Points 1–4**: 0-600 seconds
7. **Point Actions**: Same as Transport options
8. **Sync Type**: Off=0, Host, Trigger

### Musical Parameters
9. **Quantize Amount**: 0-100%
10. **Swing Amount**: 0-100%
11. **Humanize Amount**: 0-100%
12. **Primary Quantize**: None, Whole Note, Dotted Half Note, Half Note, Dotted Quarter Note, Quarter Note, Eighth Note, Eighth Note Triplet, Sixteenth Note, Sixteenth Note Triplet, Thirty-Second Note, Thirty-Second Note Triplet, Sixty-Fourth Note, Sixty-Fourth Note Triplet
13. **Secondary Quantize**: Same range as Primary
14. **Quantize Note Off**: bool
15. **Transpose**: -24 to +24 semitones
16. **Velocity Shift**: -64% to +64%
17. **Timing Shift**: -10000 to +10000 samples

### Advanced Features
18. **Import/Export**: Pattern file management (bool triggers)
19. **Export to Folder**: User-selectable export destination
20. **Show Graphics**: Visual feedback display (bool)
21. **Drum Conversion MIDI Notes**: Configurable MIDI notes (0-127) for Kick, Snare, Hat, Ride, Crash, Cowbell in/out
22. **Snare → Stick**: Intelligent drum conversion (bool)
23. **Hat → Ride**: Cymbal conversion (bool)
24. **Performance Triggers**: Crash Next Bar, Mute Next Bar, etc. (bool triggers)
25. **Drum Conversion**: Master conversion enable (bool)
26. **Sample Position**: Real-time playback position (read-only int)
27. **Send Crash**: Crash routing control (bool)
28. **Power**: Player enable/disable (bool)
29. **Current Drum Folder**: Active drumkit path (string)
30. **Energy Level**: Asleep=0, Chilled=1, Relaxed=2, Normal=3, Aroused=4, Excited=5, Energetic=6

## SFZ 2.0 Sample Engine

OTTO uses the industry-standard SFZ 2.0 format for sample playback. Each SFZ file defines:

- **Samples**: Audio files to use (WAV, AIFF, etc.)
- **Mapping**: MIDI note to sample assignments
- **Parameters**: Volume, pitch, filtering, envelopes
- **Conditions**: Velocity ranges, round-robin samples
- **Effects**: Per-sample processing and routing

Each player triggers its own SFZ drumkit playback audio engine with dedicated mixer and effects processing.

### Data Management
- **INI-Based Configuration**: Reaper-style plain text database for settings and presets
- **Asset Management Strategy**: 
  - **Embedded Assets**: UI graphics, fonts, and icons compiled into binary
  - **File-Based Content**: User-customizable drum samples, SFZ definitions, and MIDI patterns
- **State Management**: Comprehensive preset system preserving all player configurations

## Development Information

### Build Configuration
- **Project Type**: Audio Plugin
- **Company**: AudioDev
- **Version**: 1.0.0
- **Plugin Characteristics**: MIDI Effect, Synthesizer, MIDI Input/Output
- **JUCE Version**: 8.x
- **C++ Standard**: Modern C++ (C++17 compatible)

### Supported Plugin Formats
- ✅ VST3 (Primary format)
- ✅ Audio Unit (macOS)
- ✅ Standalone Application
- ✅ AUv3 (iOS)
- ✅ CLAP (enabled)
- ❌ Legacy VST2 (disabled)
- ❌ AAX (disabled)

### Platform-Specific Features
- **macOS**: Universal Binary support (Intel + Apple Silicon), full VST3/AU support
- **Windows**: VST3 support with Visual Studio 2019/2022
- **iOS**: Device and simulator builds with AUv3 support
- **Linux**: Standard distribution support with ALSA/JACK integration
- **Android**: Full NDK support with multiple ABI targets

## Advanced Features

### AI and Automation
- **AI Assistant Panel**: Intelligent pattern generation and suggestion
- **Auto Mix Assistant**: Automatic mixing parameter optimization
- **Pattern Suggestion Engine**: Context-aware rhythm recommendations

### Performance Features
- **Scene Launcher**: Live performance pattern triggering
- **Transport Synchronization**: Seamless DAW integration
- **Real-time Parameter Control**: Low-latency automation support
- **Multi-output Architecture**: Professional mixing workflow support

### Customization and Extensibility
- **Theme System**: Customizable color schemes and visual styles
- **MIDI Mapping**: Flexible controller assignment
- **SFZ Library Support**: Compatible with industry-standard sample libraries
- **Preset Sharing**: Import/export functionality for pattern and setting exchange

## Current Status & Roadmap

### ✅ Completed Features
- **Graphics & Fonts**: All UI elements, custom fonts, and Phosphor icons properly embedded
- **Cross-Platform Builds**: CMake-based build system for all target platforms
- **Plugin Formats**: VST3, AU, and Standalone all building and installing correctly
- **Asset Management**: Critical UI assets embedded, user content still customizable
- **Audio Engine**: Full 8-player SFZ-based sample playback with effects

### 🚧 Current Development
- **Memory Management Improvements**: Migration to smart pointers and RAII patterns
- **Enhanced Error Handling**: Comprehensive exception safety and graceful degradation
- **Performance Optimization**: String handling improvements and audio path optimization (see `EFFICIENCY_REPORT.md`)
- **Extended Testing**: Comprehensive unit test coverage and cross-platform validation

### 🎯 Future Enhancements
- **Advanced AI Features**: Enhanced pattern learning and suggestion algorithms
- **Cloud Integration**: Preset sharing and collaboration features
- **Extended Platform Support**: Additional plugin formats and streaming integration
- **Professional Features**: Advanced mixing console, professional effects suite (see `ENHANCEMENT_ROADMAP.md`)

## Technical Requirements

### Minimum System Requirements
- **macOS**: 10.13+ (Universal Binary supports Intel and Apple Silicon)
- **iOS**: 12.0+ (iPhone and iPad)
- **Windows**: Windows 10 with Visual Studio 2019/2022
- **Linux**: GCC 9+ or Clang 10+ with standard audio libraries
- **Android**: API level 23+ (Android 6.0+)

### Development Prerequisites
- **CMake**: 3.22 or later
- **JUCE 8**: Automatically configured via setup script
- **Platform SDKs**: Native development tools for target platforms
- **Audio Libraries**: ASIO (Windows), CoreAudio (macOS), ALSA/JACK (Linux)

## Performance Optimization

### Code Quality & Performance
- **Memory Safety**: Smart pointer usage throughout, comprehensive leak testing
- **Performance Optimization**: Efficient string handling, optimized audio processing paths
- **Error Handling**: Robust exception handling with graceful degradation
- **Code Standards**: Modern C++17 patterns with JUCE 8 best practices

### Quality Assurance
- **Comprehensive Testing**: Unit tests and memory leak detection
- **Cross-Platform CI**: GitHub Actions integration for automated builds
- **Performance Profiling**: Real-time analysis and optimization
- **Cross-Platform Compatibility**: Extensive testing across all target platforms

## License & Distribution

- **License**: GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for full details
- **Distribution**: Open source with comprehensive build documentation
- **Commercial Use**: GPL v3 compliance required for commercial distribution
- **Asset Licensing**: Embedded UI assets and fonts with appropriate licensing

## Contributing

OTTO welcomes contributions from audio developers, UI/UX designers, and music technology enthusiasts. Key areas for contribution include:

- **Platform Optimization**: Platform-specific audio and MIDI optimizations
- **AI Enhancements**: Pattern generation and learning algorithm improvements
- **UI/UX Improvements**: Interface design and user experience enhancements
- **Documentation**: Build guides, user manuals, and API documentation
- **Testing**: Cross-platform testing and performance benchmarking

## Support & Documentation

- **Build Instructions**: Comprehensive multi-platform build guide in `BUILD_INSTRUCTIONS.md`
- **Change Log**: Detailed development history in `CHANGELOG.md`
- **Performance Analysis**: Efficiency improvements documented in `EFFICIENCY_REPORT.md`
- **Enhancement Roadmap**: Future development plans in `ENHANCEMENT_ROADMAP.md`
- **Asset Management**: Guidelines in `docs/ASSET_MANAGEMENT.md`

---

**OTTO** represents the convergence of professional audio engineering, modern software development practices, and intelligent music creation tools. Built on the robust JUCE 8 framework with comprehensive cross-platform support, OTTO delivers professional-grade drum machine capabilities to musicians, producers, and developers across all major platforms.