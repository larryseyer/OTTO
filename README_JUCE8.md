# 🚨 JUCE 8 PROJECT - STRICT COMPLIANCE REQUIRED 🚨

## ⚠️ CRITICAL: This Project Uses JUCE 8.0.8+ EXCLUSIVELY

**ALL code in this project MUST use JUCE 8 compatible methods ONLY.**
**Deprecated JUCE methods are STRICTLY FORBIDDEN.**

---

## 📋 MANDATORY JUCE 8 Patterns

### ✅ FONTS - Always Use FontOptions
```cpp
// ❌ NEVER USE (Deprecated):
juce::Font font(14.0f);
juce::Font font(14.0f, juce::Font::bold);

// ✅ ALWAYS USE (JUCE 8):
juce::Font font(juce::FontOptions().withHeight(14.0f));
juce::Font font(juce::FontOptions().withHeight(14.0f).withStyle("Bold"));

// ✅ OR USE PROJECT MACROS:
JUCE8_FONT(14.0f)
JUCE8_FONT_BOLD(14.0f)
JUCE8_CREATE_FONT(14.0f)
```

### ✅ DISPLAYS - Use Modern Display API
```cpp
// ✅ CORRECT JUCE 8:
auto displays = juce::Desktop::getInstance().getDisplays();
auto primary = displays.getPrimaryDisplay();
```

### ✅ COLORS - Use Current Color Names
```cpp
// ✅ JUCE 8 Colors:
juce::Colours::lightgrey  // (correct spelling)
juce::Colours::darkgrey   // (correct spelling)
```

### ✅ COMPONENTS - Modern Memory Management
```cpp
// ✅ ALWAYS USE:
std::unique_ptr<Component> component = std::make_unique<Component>();
JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyClass)
```

---

## 🔧 Build Configuration

### Platforms Supported:
- ✅ **macOS**: Standalone, VST3, AU, AUv3
- ✅ **Windows**: Standalone, VST3, CLAP  
- ✅ **Linux**: Standalone, VST3, CLAP
- ✅ **iOS**: Standalone, AUv3
- ✅ **Android**: Standalone

### CMake Build:
```bash
cd /Users/larryseyer/AudioDevelopment/OTTO
cmake --build build_test
```

---

## 🚫 STRICTLY FORBIDDEN

### ❌ DO NOT USE:
- `juce::Font(float)` constructors
- `juce::Font(float, int)` constructors  
- Any deprecated JUCE methods
- Old Display API methods
- Legacy component patterns

---

## 📁 Project Markers

The following files mark this as a JUCE 8 project:
- `.juce8-project` - Project type marker
- `JUCE8_CODING_STANDARDS.h` - Mandatory patterns
- `JUCE8Compatibility.h` - Compatibility layer
- `CMakeLists.txt` - Build system with JUCE 8 enforcement

---

## ⚡ Quick Reference

**Include in all source files:**
```cpp
#include "JUCE8_CODING_STANDARDS.h"
#include "JUCE8Compatibility.h"
```

**Use project macros:**
```cpp
JUCE8_FONT(height)
JUCE8_FONT_BOLD(height)  
JUCE8_CREATE_FONT(height)
```

---

## 🎯 ZERO TOLERANCE POLICY

**This project has ZERO TOLERANCE for deprecated JUCE methods.**
**All code must be JUCE 8 compliant or it will be rejected.**

---

*Project Type: JUCE8_NATIVE*  
*JUCE Version: 8.0.8+*  
*Compliance Level: STRICT*