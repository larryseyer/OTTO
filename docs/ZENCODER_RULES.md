# OTTO Zencoder Rules - AI Assistant Guidelines

**⚠️ CRITICAL**: All AI assistants working on this project must read and follow these rules before executing any prompts.

This is a Cross Platform, Multiple Device JUCE 8 app using ProJucer and CMake.

ISYSTEM DIRECTIVES - CRITICAL TO REMEMBER:
✅ JUCE 8 with ProJucer - Use JUCE 8 methods, protocols, and modern C++ patterns
✅ Unified CMake Build System - Configured for all platforms: Mac OSX Standalone, VST3, AUV3, Clap AU, Windows, Linux, iOS, Android
✅ DO NOT MODIFY BUILD SYSTEM - Work within existing CMake configuration Use our build system by doing this: cd /Users/larryseyer/AudioDevelopment/OTTO && ./scripts/build/build_macos.sh
✅ INI-Driven Layout Priority - All positioning must use INIConfig constants, no hardcoded values
✅ JUCE 8 Fonts/Colors/UI - Use proper JUCE 8 FontManager, ColorScheme integration
✅ All variables and settings must use INI-Driven file system
✅ No backward compatibility required - brand new app with no users yet.

## 📋 MANDATORY WORKFLOW FOR AI ASSISTANTS:

### Before Any Code Changes:
1. **Always check INIConfig.h first** for any layout/positioning work
2. **Use proper namespaces** (INIConfig::LayoutConstants::Row3::, etc.)
3. **Never suggest CMake modifications**
4. **Use FontManager/ColorScheme** for all UI styling
5. **Reference the percentage-based responsive system**
6. **NEVER use 'Magic Numbers' ALWAYS use our INI system

### Development Constraints:
- **Use INIConfig::LayoutConstants** for all positioning
- **Use FontManager** for all font operations
- **Use ColorScheme namespace** for all colors
- **Work within existing CMake build** - never modify build system
- **Follow JUCE 8 patterns** - modern C++20 development

### Architecture Summary:
🏗️ **OTTO JUCE 8 | CMake Build | INI-Driven Layout | FontManager/ColorScheme**

**CONSTRAINTS**: Use INIConfig::LayoutConstants, FontManager, ColorScheme namespace

### Note to Zencoder:
**Use our build system by doing this: cd /Users/larryseyer/AudioDevelopment/OTTO && ./scripts/build/build_macos.sh
