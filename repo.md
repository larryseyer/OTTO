# OTTO - Professional Drum Machine and Sample Playback System

## Project Overview

**OTTO** is a comprehensive, professional-grade drum machine and sample playback system built with JUCE 8 framework. Developed by Automagic Art, OTTO provides musicians and producers with a powerful, cross-platform audio plugin featuring advanced MIDI pattern management, SFZ 2.0 sample playback, and sophisticated audio processing capabilities.

## Key Features

### Core Functionality
- **8 Independent MIDI Players**: Each with synchronized MIDI playback (host or standalone), dedicated SFZ sample engine, unique sounds, and individual MIDI & audio mixer with multiple outputs and effects
- **Pattern Management System**: 4x4 grids of MIDI beat patterns organized in groups of 16 for comprehensive rhythm programming
- **Smart Fill System**: Automatic and manual drum fills with intelligent timing integration
- **Professional Audio Engine**: SFZ 2.0 compliant sample playback with individual effects processing for each drum kit element (kick, snare, hi-hat, toms, cymbals, etc.)

### Multi-Platform Support
- **Cross-Platform Compatibility**: Native support for macOS, Windows, Linux, and iOS
- **Multiple Plugin Formats**: 
  - VST3 (enabled)
  - Audio Unit (AU) (enabled)
  - Standalone Application (enabled)
  - iOS AUv3 (configurable)
  - CLAP support (planned)

## Technical Architecture

### Framework and Dependencies
- **JUCE Framework 8**: Modern C++ audio application framework
- **ProJucer Integration**: Project configuration and build system management
- **Cross-Platform Build System**: Xcode (macOS/iOS), Visual Studio 2022 (Windows), Android Studio (Android)

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
```
MIDI Input → Pattern Selection → Fills Logic → SFZ Engine → Mixer → Effects → Multi-Output
```

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

## Development Information

### Build Configuration
- **Project Type**: Audio Plugin
- **Company**: Automagic Art
- **Version**: 1.0.0
- **Plugin Characteristics**: MIDI Effect, Synthesizer, MIDI Input/Output
- **JUCE Version**: 8.x
- **C++ Standard**: Modern C++ (C++17/20 compatible)

### Supported Plugin Formats
- ✅ VST3 (Primary format)
- ✅ Audio Unit (macOS)
- ✅ Standalone Application
- ⚪ AUv3 (iOS, configurable)
- ❌ Legacy VST2 (disabled)
- ❌ AAX (disabled)
- ❌ LV2 (disabled)

### Platform-Specific Features
- **macOS**: Full VST3/AU support with Xcode integration
- **Windows**: VST3 support with Visual Studio 2022
- **iOS**: Standalone and AUv3 capabilities
- **Android**: Experimental support (build configuration present)

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

## Performance Optimization

### Identified Improvements
- String processing optimization in state management (see `EFFICIENCY_REPORT.md`)
- Memory management enhancements (detailed in `ENHANCEMENT_ROADMAP.md`)
- Real-time audio processing optimizations

### Quality Assurance
- Comprehensive testing framework integration
- Automated build verification
- Performance profiling and optimization
- Cross-platform compatibility testing

## License

This project is licensed under the **GNU General Public License v3.0** - see the [LICENSE](LICENSE) file for full details.

## Getting Started

### Prerequisites
- JUCE Framework 8.x
- ProJucer
- Platform-specific development tools:
  - **macOS**: Xcode 12+
  - **Windows**: Visual Studio 2022
  - **iOS**: Xcode with iOS SDK
  - **Android**: Android Studio (experimental)

### Building the Project
1. Open `OTTO.jucer` in ProJucer
2. Configure target platforms and plugin formats
3. Generate platform-specific project files
4. Build using the appropriate IDE or build system

### Development Workflow
- Use feature branches for major enhancements
- Run comprehensive testing after modifications
- Follow the enhancement roadmap for systematic improvements
- Maintain cross-platform compatibility

---

**OTTO** represents a sophisticated approach to modern drum machine design, combining professional audio processing with intuitive user experience and extensive customization capabilities. The project demonstrates advanced JUCE framework utilization and provides a solid foundation for continued development and feature enhancement.