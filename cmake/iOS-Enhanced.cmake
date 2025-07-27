#==============================================================================
# OTTO Phase 2.3.3 - Enhanced iOS Build Configuration (JUCE 8)
# Advanced iOS-specific optimizations and AUv3 integration
# Maintains all Phase 2.2 safety features while adding iOS-specific enhancements
#==============================================================================

cmake_minimum_required(VERSION 3.22)

# iOS platform validation
if(NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    message(FATAL_ERROR "iOS-Enhanced.cmake can only be used for iOS builds")
endif()

message(STATUS "🍎 OTTO Phase 2.3.3 - Enhanced iOS Configuration")
message(STATUS "================================================")

#==============================================================================
# iOS Version and Deployment Configuration
#==============================================================================

# Set minimum iOS version for optimal JUCE 8 compatibility
set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "iOS Deployment Target")
set(IPHONEOS_DEPLOYMENT_TARGET "12.0" CACHE STRING "iOS Deployment Target")

# iOS-specific compiler definitions
add_compile_definitions(
    # iOS platform identification
    OTTO_IOS_BUILD=1
    JUCE_IOS_ENHANCED=1
    
    # Audio Unit v3 optimizations
    OTTO_AUV3_OPTIMIZED=1
    
    # iOS memory management
    JUCE_IOS_MEMORY_OPTIMIZED=1
    
    # Metal rendering support
    JUCE_METAL_RENDERER=1
    
    # iOS-specific features
    JUCE_IOS_BACKGROUND_AUDIO=1
    JUCE_IOS_INTER_APP_AUDIO=1
)

message(STATUS "✅ iOS deployment target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")

#==============================================================================
# iOS Architecture Configuration
#==============================================================================

# Auto-detect architecture if not specified
if(NOT CMAKE_OSX_ARCHITECTURES)
    if(CMAKE_OSX_SYSROOT MATCHES "simulator")
        # iOS Simulator - detect host architecture
        execute_process(
            COMMAND uname -m
            OUTPUT_VARIABLE HOST_ARCHITECTURE
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        
        if(HOST_ARCHITECTURE STREQUAL "arm64")
            set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "iOS Simulator ARM64")
            message(STATUS "🔧 iOS Simulator (Apple Silicon): arm64")
        else()
            set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "iOS Simulator Intel")
            message(STATUS "🔧 iOS Simulator (Intel): x86_64")
        endif()
    else()
        # iOS Device - always arm64
        set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "iOS Device ARM64")
        message(STATUS "🔧 iOS Device: arm64")
    endif()
endif()

#==============================================================================
# iOS Audio Configuration
#==============================================================================

function(configure_ios_audio_optimizations target)
    message(STATUS "🎵 Configuring iOS audio optimizations for: ${target}")
    
    # iOS-specific audio definitions
    target_compile_definitions(${target} PRIVATE
        # Core Audio optimizations
        JUCE_IOS_AUDIO_OPTIMIZED=1
        JUCE_USE_CORE_AUDIO_FORMAT=1
        
        # Audio Unit v3 specific
        JUCE_AUV3_MIDI_ONLY=0
        JUCE_AUV3_VIEW_CONFIG_SUPPORTED=1
        
        # iOS audio session management
        JUCE_IOS_AUDIO_SESSION_OPTIMIZED=1
        
        # Buffer size optimization for iOS
        JUCE_IOS_DEFAULT_BUFFER_SIZE=256
        
        # Sample rate optimization
        JUCE_IOS_PREFERRED_SAMPLE_RATE=48000
    )
    
    # iOS-specific compiler flags
    target_compile_options(${target} PRIVATE
        # Optimize for iOS performance
        -ffast-math
        -fno-finite-math-only  # Avoid issues with NaN/Inf
        
        # iOS-specific optimizations
        -mios-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}
        
        # ARC (Automatic Reference Counting) support
        -fobjc-arc
        
        # Optimize for mobile battery life
        -Os  # Optimize for size (important for mobile)
    )
    
    message(STATUS "  ✅ iOS audio optimizations applied")
endfunction()

#==============================================================================
# iOS UI and Graphics Configuration
#==============================================================================

function(configure_ios_ui_optimizations target)
    message(STATUS "📱 Configuring iOS UI optimizations for: ${target}")
    
    # iOS UI definitions
    target_compile_definitions(${target} PRIVATE
        # Metal rendering
        JUCE_METAL=1
        JUCE_OPENGL_ES=1
        
        # iOS-specific UI features
        JUCE_IOS_UI_OPTIMIZED=1
        JUCE_IOS_RETINA_SUPPORT=1
        JUCE_IOS_SAFE_AREA_SUPPORT=1
        
        # Touch interface optimizations
        JUCE_IOS_MULTITOUCH=1
        JUCE_IOS_GESTURE_RECOGNITION=1
        
        # Screen orientation support
        JUCE_IOS_ORIENTATION_SUPPORT=1
    )
    
    # iOS-specific frameworks
    target_link_libraries(${target} PRIVATE
        "-framework Metal"
        "-framework MetalKit"
        "-framework QuartzCore"
        "-framework CoreGraphics"
        "-framework UIKit"
        "-framework Foundation"
        "-framework CoreFoundation"
        "-framework AVFoundation"
        "-framework AudioToolbox"
        "-framework CoreAudio"
        "-framework CoreMIDI"
        "-framework Accelerate"
    )
    
    message(STATUS "  ✅ iOS UI optimizations applied")
endfunction()

#==============================================================================
# iOS Performance Optimization
#==============================================================================

function(configure_ios_performance_optimizations target)
    message(STATUS "⚡ Configuring iOS performance optimizations for: ${target}")
    
    # Performance-related definitions
    target_compile_definitions(${target} PRIVATE
        # Memory optimization
        JUCE_IOS_MEMORY_POOL_SIZE=4194304  # 4MB
        JUCE_IOS_CACHE_OPTIMIZATION=1
        
        # CPU optimization
        JUCE_IOS_NEON_OPTIMIZED=1  # ARM NEON SIMD
        JUCE_IOS_POWER_EFFICIENT=1
        
        # Background processing
        JUCE_IOS_BACKGROUND_PROCESSING=1
        JUCE_IOS_AUDIO_BACKGROUND_SUPPORT=1
    )
    
    # Link-time optimizations
    target_link_options(${target} PRIVATE
        # Link-time optimization
        $<$<CONFIG:Release>:-flto>
        
        # Dead code stripping
        -dead_strip
        
        # iOS-specific linker flags
        -Wl,-ios_version_min,${CMAKE_OSX_DEPLOYMENT_TARGET}
    )
    
    message(STATUS "  ✅ iOS performance optimizations applied")
endfunction()

#==============================================================================
# iOS Security and Privacy Configuration
#==============================================================================

function(configure_ios_security_features target)
    message(STATUS "🔐 Configuring iOS security features for: ${target}")
    
    # Security definitions
    target_compile_definitions(${target} PRIVATE
        # iOS security features
        JUCE_IOS_SECURE_CODING=1
        JUCE_IOS_KEYCHAIN_SUPPORT=1
        
        # Privacy and permissions
        JUCE_IOS_MICROPHONE_PERMISSION=1
        JUCE_IOS_PRIVACY_COMPLIANT=1
        
        # Code signing and validation
        JUCE_IOS_CODE_SIGNING_REQUIRED=1
    )
    
    # Security frameworks
    target_link_libraries(${target} PRIVATE
        "-framework Security"
        "-framework LocalAuthentication"
    )
    
    message(STATUS "  ✅ iOS security features applied")
endfunction()

#==============================================================================
# AUv3 Specific Configuration
#==============================================================================

function(configure_auv3_optimizations target)
    message(STATUS "🎛️  Configuring AUv3 optimizations for: ${target}")
    
    # AUv3-specific definitions
    target_compile_definitions(${target} PRIVATE
        # AUv3 features
        JUCE_AUV3_ENHANCED=1
        JUCE_AUV3_MIDI_SUPPORT=1
        JUCE_AUV3_PARAMETER_SUPPORT=1
        JUCE_AUV3_PRESET_SUPPORT=1
        
        # Inter-app audio
        JUCE_IOS_INTER_APP_AUDIO_ENABLED=1
        
        # AUv3 UI features
        JUCE_AUV3_VIEW_RESIZING=1
        JUCE_AUV3_CUSTOM_UI=1
    )
    
    # AUv3 frameworks
    target_link_libraries(${target} PRIVATE
        "-framework AudioUnit"
        "-framework AVFoundation"
        "-framework CoreAudioKit"
    )
    
    message(STATUS "  ✅ AUv3 optimizations applied")
endfunction()

#==============================================================================
# iOS App Store Configuration
#==============================================================================

function(configure_ios_app_store_compliance target)
    message(STATUS "🏪 Configuring iOS App Store compliance for: ${target}")
    
    # App Store compliance definitions
    target_compile_definitions(${target} PRIVATE
        # App Store requirements
        JUCE_IOS_APP_STORE_COMPLIANT=1
        JUCE_IOS_PRIVACY_MANIFEST=1
        
        # No private APIs
        JUCE_IOS_NO_PRIVATE_APIS=1
        
        # Accessibility support
        JUCE_IOS_ACCESSIBILITY_SUPPORT=1
    )
    
    # Ensure proper code signing configuration
    set_target_properties(${target} PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "$ENV{APPLE_DEVELOPMENT_TEAM}"
        XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER ""
        XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic"
    )
    
    message(STATUS "  ✅ App Store compliance configured")
endfunction()

#==============================================================================
# iOS Testing and Debugging Configuration
#==============================================================================

function(configure_ios_testing_features target)
    message(STATUS "🧪 Configuring iOS testing features for: ${target}")
    
    # Debug and testing definitions
    target_compile_definitions(${target} PRIVATE
        $<$<CONFIG:Debug>:
            JUCE_IOS_DEBUG_ENABLED=1
            JUCE_IOS_CONSOLE_LOGGING=1
            JUCE_IOS_PERFORMANCE_PROFILING=1
        >
        
        $<$<CONFIG:Release>:
            JUCE_IOS_RELEASE_OPTIMIZED=1
            JUCE_IOS_CRASH_REPORTING=1
        >
    )
    
    # Debug frameworks (Debug builds only)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(${target} PRIVATE
            "-framework OSLog"
        )
    endif()
    
    message(STATUS "  ✅ iOS testing features configured")
endfunction()

#==============================================================================
# Main iOS Configuration Function
#==============================================================================

function(configure_enhanced_ios_build target)
    message(STATUS "🍎 Configuring enhanced iOS build for: ${target}")
    
    # Apply all iOS optimizations
    configure_ios_audio_optimizations(${target})
    configure_ios_ui_optimizations(${target})
    configure_ios_performance_optimizations(${target})
    configure_ios_security_features(${target})
    configure_ios_testing_features(${target})
    configure_ios_app_store_compliance(${target})
    
    # Apply AUv3 optimizations if building AUv3
    get_target_property(TARGET_TYPE ${target} TYPE)
    if(TARGET_TYPE STREQUAL "MODULE_LIBRARY")
        # This is likely an AUv3 target
        configure_auv3_optimizations(${target})
    endif()
    
    message(STATUS "✅ Enhanced iOS configuration complete for: ${target}")
endfunction()

# Export the configuration
set(OTTO_IOS_ENHANCED_CMAKE_LOADED TRUE)
message(STATUS "OTTO Enhanced iOS configuration loaded")