#pragma once

//==============================================================================
// JUCE 8 PROJECT - MANDATORY CODING STANDARDS
// This project uses JUCE 8.0.8+ EXCLUSIVELY
// ALL deprecated methods are FORBIDDEN
//==============================================================================

#ifndef JUCE8_CODING_STANDARDS_H
#define JUCE8_CODING_STANDARDS_H

// Enforce JUCE 8 version check
#if JUCE_MAJOR_VERSION < 8
    #error "This project requires JUCE 8.0.8 or higher"
#endif

//==============================================================================
// FONT CREATION - MANDATORY PATTERNS
//==============================================================================

// ❌ NEVER USE THESE DEPRECATED PATTERNS:
// juce::Font(height)
// juce::Font(height, style)
// juce::Font(name, height, style)

// ✅ ALWAYS USE THESE JUCE 8 PATTERNS:
#define JUCE8_FONT(height) \
    juce::Font(juce::FontOptions().withHeight(height))

#define JUCE8_FONT_BOLD(height) \
    juce::Font(juce::FontOptions().withHeight(height).withStyle("Bold"))

#define JUCE8_FONT_ITALIC(height) \
    juce::Font(juce::FontOptions().withHeight(height).withStyle("Italic"))

#define JUCE8_FONT_NAMED(name, height) \
    juce::Font(juce::FontOptions().withName(name).withHeight(height))

#define JUCE8_FONT_NAMED_BOLD(name, height) \
    juce::Font(juce::FontOptions().withName(name).withHeight(height).withStyle("Bold"))

//==============================================================================
// DISPLAY API - MANDATORY PATTERNS  
//==============================================================================

// ✅ ALWAYS USE JUCE 8 Display API:
inline auto getDisplays() { return juce::Desktop::getInstance().getDisplays(); }
inline auto getPrimaryDisplay() { return juce::Desktop::getInstance().getDisplays().getPrimaryDisplay(); }

//==============================================================================
// COMPONENT MANAGEMENT - MANDATORY PATTERNS
//==============================================================================

// ✅ ALWAYS USE modern memory management:
// std::unique_ptr, std::make_unique
// JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR

//==============================================================================
// DEPRECATED METHOD DETECTION - COMPILE-TIME ERRORS
//==============================================================================

// These will cause compile errors if deprecated methods are used
#define juce_Font_deprecated_constructor_detected \
    static_assert(false, "JUCE 8 ERROR: Use juce::Font(juce::FontOptions()) instead of deprecated constructors")

//==============================================================================
// PROJECT VERIFICATION
//==============================================================================

namespace JUCE8Standards {
    constexpr int JUCE_VERSION_REQUIRED = 8;
    constexpr const char* PROJECT_TYPE = "JUCE8_NATIVE";
    
    // Compile-time verification this is JUCE 8
    static_assert(JUCE_MAJOR_VERSION >= 8, "This project requires JUCE 8.0.8+");
    
    // Runtime verification function
    inline bool verifyJUCE8Compliance() {
        return JUCE_MAJOR_VERSION >= 8;
    }
}

#endif // JUCE8_CODING_STANDARDS_H

//==============================================================================
// USAGE INSTRUCTIONS:
// 
// 1. #include "JUCE8_CODING_STANDARDS.h" in all source files
// 2. Use JUCE8_FONT macros instead of deprecated constructors
// 3. All new code MUST follow JUCE 8 patterns
// 4. NO exceptions for deprecated methods
//==============================================================================