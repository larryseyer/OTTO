# OTTO Repository Guidelines

This document outlines the GitHub repository management guidelines, contribution standards, and development workflow for the OTTO project.

## 🏷️ Repository Information

- **Repository Name**: OTTO
- **Description**: Professional Drum Machine & Sample Playback System
- **Company**: Automagic Art
- **License**: Commercial
- **Primary Language**: C++20
- **Framework**: JUCE 8.0.8+

## 📁 Repository Structure

### Core Directories
```
OTTO/
├── Source/                 # C++ source code (JUCE 8 compliant)
├── Assets/                 # User-customizable content (samples, patterns)
├── Builds/                 # Build output directory (auto-generated)
├── cmake/                  # Platform-specific CMake configurations
├── JuceLibraryCode/        # Generated JUCE integration files
├── scripts/                # Build and utility scripts
└── docs/                   # Technical documentation
```

### Documentation Files
- `README.md` - Main project overview and quick start
- `REPO.md` - This file (repository guidelines)
- `DESIGNRULES.md` - UI design guidelines and standards
- `BUILDRULES.md` - Cross-platform build instructions
- `INIRULES.md` - Data storage and configuration system

## 🔄 Git Workflow

### Branch Strategy
- **`main`** - Production-ready code, always stable
- **`develop`** - Integration branch for new features
- **`feature/*`** - Feature development branches
- **`hotfix/*`** - Critical bug fixes for production
- **`release/*`** - Release preparation branches

### Commit Standards

#### Commit Message Format
```
type(scope): description

[optional body]

[optional footer]
```

#### Commit Types
- `feat` - New feature implementation
- `fix` - Bug fixes
- `docs` - Documentation updates
- `style` - Code formatting, no functional changes
- `refactor` - Code restructuring without feature changes
- `test` - Adding or updating tests
- `build` - Build system changes
- `ci` - CI/CD configuration changes

#### Examples
```bash
feat(midi): add host synchronization for player engines
fix(ui): resolve button layout positioning on iOS
docs(build): update cross-platform build instructions
refactor(sfz): optimize sample loading performance
```

### Pull Request Guidelines

#### PR Title Format
```
[Type] Brief description of changes
```

#### PR Requirements
- [ ] All builds pass (5 platforms)
- [ ] No hardcoded values introduced (INI-driven only)
- [ ] JUCE 8 compatibility maintained
- [ ] Documentation updated if needed
- [ ] Manual testing completed

#### PR Template
```markdown
## Description
Brief description of changes and motivation.

## Type of Change
- [ ] Bug fix (non-breaking change)
- [ ] New feature (non-breaking change)
- [ ] Breaking change (fix/feature causing existing functionality to change)
- [ ] Documentation update

## Testing
- [ ] macOS build tested
- [ ] Windows build tested  
- [ ] Linux build tested
- [ ] iOS build tested
- [ ] Android build tested

## Checklist
- [ ] Code follows JUCE 8 standards
- [ ] No hardcoded values added
- [ ] INI configuration used for all constants
- [ ] Self-review completed
- [ ] Documentation updated
```

## 🛡️ Code Standards

### JUCE 8 Compliance
- **Modern C++20**: Use latest language features
- **Smart Pointers**: Prefer std::unique_ptr and std::shared_ptr
- **RAII**: Resource Acquisition Is Initialization principles
- **Exception Safety**: Proper error handling and recovery
- **Thread Safety**: Proper synchronization for real-time audio

### INI-Driven Development
- **No Hardcoded Values**: All layout constants must use INIConfig
- **Proportional Sizing**: Use percentage-based measurements
- **Responsive Design**: Components must adapt to screen sizes
- **Configuration Centralization**: All settings in INI system

### Code Style
```cpp
// Class naming: PascalCase
class PlayerTabsComponent;

// Method naming: camelCase
void calculatePlayerButtonLayout();

// Constants: Use INIConfig namespace
auto buttonWidth = INIConfig::LayoutConstants::defaultButtonWidth;

// Modern C++20 features
auto [x, y, width, height] = calculateBounds();
```

## 🏗️ Build Requirements

### Supported Platforms
| Platform | Architecture | Status | Required Tools |
|----------|-------------|---------|----------------|
| macOS | Universal (Intel + ARM) | ✅ Active | Xcode 14+, CMake 3.22+ |
| Windows | x64 | ✅ Active | VS2022, CMake 3.22+ |
| Linux | x86_64, ARM64 | ✅ Active | GCC 11+, CMake 3.22+ |
| iOS | arm64, Simulator | ✅ Active | Xcode 14+, iOS SDK 15+ |
| Android | arm64-v8a, armeabi-v7a | ✅ Active | NDK 25+, API 23+ |

### Pre-commit Checks
```bash
# Required before any commit
./validate_all_platforms.sh  # Must pass on all 5 platforms
./check_hardcoded_values.sh  # Must find zero violations
./verify_ini_compliance.sh   # Must pass INI configuration checks
```

## 📊 Quality Assurance

### Testing Requirements
- **Build Testing**: All 5 platforms must build successfully
- **Runtime Testing**: Basic functionality verified on each platform
- **Memory Testing**: No leaks or undefined behavior
- **Audio Testing**: Real-time performance validated
- **UI Testing**: Responsive layout across different screen sizes

### Performance Standards
- **Build Time**: < 5 minutes per platform
- **Binary Size**: < 50MB total (all formats)
- **Startup Time**: < 2 seconds on all platforms
- **Audio Latency**: < 5ms typical, < 10ms maximum
- **CPU Usage**: < 5% typical, < 15% maximum

## 🔐 Security & Privacy

### Data Handling
- **Local Storage**: All user data stored locally in INI format
- **No Telemetry**: No data collection or transmission
- **Privacy First**: User content never uploaded without explicit consent
- **Secure Coding**: Buffer overflow protection, input validation

### Code Security
- **Memory Safety**: Smart pointers, bounds checking
- **Input Validation**: All user inputs sanitized
- **File System**: Secure path handling, permission checks
- **Threading**: Proper synchronization, race condition prevention

## 🚀 Release Process

### Version Numbering
- **Format**: MAJOR.MINOR.PATCH (SemVer 2.0)
- **Major**: Breaking changes, major new features
- **Minor**: New features, backward compatible
- **Patch**: Bug fixes, small improvements

### Release Checklist
- [ ] All automated tests pass
- [ ] Manual testing on all 5 platforms
- [ ] Documentation updated
- [ ] CHANGELOG.md updated
- [ ] Version numbers updated
- [ ] Release notes prepared
- [ ] Build artifacts generated and signed

### Distribution
- **macOS**: Notarized .pkg installer + individual plugins
- **Windows**: Signed installer + individual plugins
- **Linux**: AppImage + repository packages
- **iOS**: App Store submission (TestFlight for beta)
- **Android**: Play Store submission (Internal testing for beta)

## 🤝 Contributing

### Getting Started
1. Fork the repository
2. Create feature branch from `develop`
3. Set up development environment
4. Make changes following code standards
5. Test on all supported platforms
6. Submit pull request

### Development Environment Setup
```bash
# Clone repository
git clone https://github.com/AutomagicArt/OTTO.git
cd OTTO

# Initialize JUCE submodule
git submodule update --init --recursive

# Set up development environment
./setup_development.sh

# Test build on current platform
./build_$(uname -s | tr '[:upper:]' '[:lower:]').sh
```

### Issue Reporting
- **Bug Reports**: Use bug report template with reproduction steps
- **Feature Requests**: Use feature request template with clear use cases
- **Questions**: Use discussions for general questions
- **Security Issues**: Report privately via email

## 📝 Documentation Standards

### Code Documentation
- **Header Comments**: All public APIs documented
- **Inline Comments**: Complex logic explained
- **Examples**: Usage examples for public interfaces
- **Doxygen**: Standard documentation format

### User Documentation
- **Clear Instructions**: Step-by-step guides
- **Visual Aids**: Screenshots and diagrams where helpful
- **Cross-Platform**: Instructions for all supported platforms
- **Up-to-Date**: Documentation updated with every release

## 🔍 Monitoring & Analytics

### Build Metrics
- **Success Rate**: Track build success across platforms
- **Build Time**: Monitor build performance trends
- **Binary Size**: Track size growth over time
- **Test Coverage**: Maintain code coverage metrics

### Repository Health
- **Issue Response Time**: Aim for < 24 hours
- **PR Review Time**: Aim for < 48 hours
- **Code Review Coverage**: All PRs require review
- **Automated Checks**: Pre-commit hooks and CI/CD validation

## 📞 Support & Communication

### Community Channels
- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: General questions and community support
- **Documentation**: Primary source for technical information
- **Email**: Direct contact for security issues or commercial inquiries

### Response Times
- **Critical Issues**: < 4 hours
- **Bug Reports**: < 24 hours
- **Feature Requests**: < 48 hours
- **General Questions**: < 72 hours

---

This repository follows professional development practices with a focus on quality, cross-platform compatibility, and modern C++ development standards using JUCE 8.