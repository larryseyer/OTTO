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

## Project Structure ✅

**⚠️ CRITICAL FOR ALL AI AGENTS**: When debugging, ALWAYS check ALL these folders for missing methods or files:

### Main Source Structure:
```
/Source - Main source files (PluginEditor, PluginProcessor, Row Components, etc.)
/Source/UI - User interface components
  /Source/UI/Touch - Touch interface handling
  /Source/UI/Layout - Layout management components
  /Source/UI/Themes - Theme and visual styling
  /Source/UI/Controls - Custom UI controls
  /Source/UI/Feedback - User feedback systems
  /Source/UI/Responsive - Responsive design components
  /Source/UI/Visualizations - Audio visualizations
/Source/UX - User experience enhancements
/Source/Animation - Animation and transitions
/Source/DragDrop - Drag and drop functionality
/Source/Performance - Performance optimization
/Source/tests - Test suite
```

### 🚨 DEBUGGING MANDATE FOR AI AGENTS:
**NEVER waste time debugging because you didn't check deep enough into the file structure!**

1. **ALWAYS search ALL subdirectories** when looking for methods or components
2. **Check PluginEditor.cpp and PluginProcessor.cpp** - they may include references to missing files
3. **Examine all Row Components** (Row1Component.cpp through Row6Component.cpp)
4. **Search the entire /Source tree** before concluding a file doesn't exist
5. **Consider that files may be commented at the top** of main files even if not actively used

## 📋 MANDATORY WORKFLOW FOR AI ASSISTANTS:

### Before Any Code Changes:
1. **Always check INIConfig.h first** for any layout/positioning work
2. **Use proper namespaces** (INIConfig::LayoutConstants::Row3::, etc.)
3. **Never suggest CMake modifications**
4. **Use FontManager/ColorScheme** for all UI styling
5. **Reference the percentage-based responsive system**
6. **NEVER use 'Magic Numbers' ALWAYS use our INI system
7. **SEARCH ALL SOURCE SUBDIRECTORIES** before concluding files don't exist

### File Organization Best Practice:
**Consider including ALL subdirectory files in PluginEditor.h and PluginProcessor.h** (even if commented) to ensure AI agents can find them during debugging. This prevents wasted time searching for "missing" files that actually exist in subdirectories.

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
