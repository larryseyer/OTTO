cmake_minimum_required(VERSION 3.22)

#==============================================================================
# JUCE 8 PROJECT - MANDATORY JUCE 8.0.8+ COMPATIBILITY
# This project EXCLUSIVELY uses JUCE 8 methods and APIs
# ALL deprecated JUCE methods are FORBIDDEN
#==============================================================================

# Set project name and version to match ProJucer
project(OTTO VERSION 1.0.0)

# Enforce JUCE 8 requirement
set(JUCE_MINIMUM_VERSION "8.0.8")
add_compile_definitions(
    JUCE_MAJOR_VERSION_REQUIRED=8
    JUCE8_PROJECT=1
)

# Set C++ standard for JUCE 8
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# JUCE 8 specific settings
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Platform detection and configuration
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(JUCE_TARGET_PLATFORM "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
    set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "12.0")
    
    # Phase 2.3.3 - Load enhanced iOS configuration
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/iOS-Enhanced.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/iOS-Enhanced.cmake)
        message(STATUS "Phase 2.3.3: Enhanced iOS configuration loaded")
    endif()
elseif(APPLE)
    set(JUCE_TARGET_PLATFORM "macOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
elseif(ANDROID)
    set(JUCE_TARGET_PLATFORM "Android")
    
    # Phase 2.3.4 - Load enhanced Android configuration
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Android-Enhanced.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Android-Enhanced.cmake)
        message(STATUS "Phase 2.3.4: Enhanced Android configuration loaded")
    endif()
elseif(WIN32)
    set(JUCE_TARGET_PLATFORM "Windows")
    set(CMAKE_SYSTEM_VERSION "10.0")
    # Load Windows-specific configuration
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Windows.cmake)
    
    # Phase 2.3.1 - Load Windows Parallels optimization
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/WindowsParallelsOptimization.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/WindowsParallelsOptimization.cmake)
        message(STATUS "Phase 2.3.1: Windows Parallels optimization loaded")
    endif()
    
    # Phase 2.3.1 - Load Reaper testing integration
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ReaperTestingIntegration.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ReaperTestingIntegration.cmake)
        message(STATUS "Phase 2.3.1: Reaper testing integration loaded")
    endif()
elseif(UNIX)
    set(JUCE_TARGET_PLATFORM "Linux")
endif()

# Phase 2.3.5 - Load advanced robustness features (all platforms)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/AdvancedRobustness.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AdvancedRobustness.cmake)
    message(STATUS "Phase 2.3.5: Advanced robustness features loaded")
endif()

# Auto-detect architecture for Apple platforms if not specified
if(APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
    execute_process(
        COMMAND uname -m
        OUTPUT_VARIABLE HOST_ARCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        if(CMAKE_OSX_SYSROOT STREQUAL "iphonesimulator")
            if(HOST_ARCH STREQUAL "arm64")
                set(CMAKE_OSX_ARCHITECTURES "arm64")
                message(STATUS "Auto-detected iOS Simulator architecture: arm64 (Apple Silicon)")
            else()
                set(CMAKE_OSX_ARCHITECTURES "x86_64")
                message(STATUS "Auto-detected iOS Simulator architecture: x86_64 (Intel)")
            endif()
        else()
            set(CMAKE_OSX_ARCHITECTURES "arm64")
            message(STATUS "iOS Device architecture: arm64")
        endif()
    else()
        set(CMAKE_OSX_ARCHITECTURES "${HOST_ARCH}")
        message(STATUS "Auto-detected macOS architecture: ${HOST_ARCH}")
    endif()
endif()

# JUCE Configuration
set(JUCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/JUCE" CACHE PATH "Path to JUCE")

# Check if JUCE is available as subdirectory
if(EXISTS "${JUCE_PATH}/CMakeLists.txt")
    add_subdirectory(${JUCE_PATH} JUCE)
    message(STATUS "Using JUCE from: ${JUCE_PATH}")
else()
    # Try to find system-installed JUCE
    find_package(juce CONFIG REQUIRED)
    message(STATUS "Using system-installed JUCE")
endif()

# Define plugin formats based on platform for JUCE 8
set(OTTO_FORMATS)

# Platform-specific format configuration
if(JUCE_TARGET_PLATFORM STREQUAL "macOS")
    list(APPEND OTTO_FORMATS Standalone VST3 AU CLAP)
elseif(JUCE_TARGET_PLATFORM STREQUAL "iOS")
    list(APPEND OTTO_FORMATS Standalone AUv3)
elseif(JUCE_TARGET_PLATFORM STREQUAL "Windows")
    list(APPEND OTTO_FORMATS Standalone VST3 CLAP)
elseif(JUCE_TARGET_PLATFORM STREQUAL "Linux")
    list(APPEND OTTO_FORMATS Standalone VST3 CLAP)
elseif(JUCE_TARGET_PLATFORM STREQUAL "Android")
    list(APPEND OTTO_FORMATS Standalone)
else()
    # Fallback for unknown platforms
    list(APPEND OTTO_FORMATS Standalone)
endif()

message(STATUS "Building OTTO for ${JUCE_TARGET_PLATFORM} with formats: ${OTTO_FORMATS}")

# Create the plugin target with JUCE 8 configuration
juce_add_plugin(OTTO
    # Company and product info
    COMPANY_NAME "Automagic Art"
    PLUGIN_MANUFACTURER_CODE "OTTO"
    PLUGIN_CODE "OTTO"
    FORMATS ${OTTO_FORMATS}
    PRODUCT_NAME "OTTO"
    DESCRIPTION "OTTO - Proportional Scaling Audio Interface"
    BUNDLE_ID "com.automagicart.OTTO"
    
    # Plugin characteristics
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    
    # JUCE 8 specific plugin characteristics
    PLUGIN_CHARACTERISTICS_VALUE "pluginIsMidiEffectPlugin,pluginIsSynth,pluginProducesMidiOut,pluginWantsMidiIn"
    
    # VST3 specific (JUCE 8)
    VST3_CATEGORIES "Fx" "Dynamics" "Tools" "Synth"
    
    # AU specific (JUCE 8)
    AU_MAIN_TYPE "kAudioUnitType_MusicDevice"
    AU_SANDBOX_SAFE TRUE
    
    # AUv3 specific for iOS (JUCE 8)
    $<$<IN_LIST:AUv3,${OTTO_FORMATS}>:
        AUV3_CATEGORY_GENERATOR
        REQUIRES_FULL_SCREEN FALSE
        BACKGROUND_AUDIO_ENABLED FALSE
        BACKGROUND_BLE_ENABLED FALSE
        INTER_APP_AUDIO_ENABLED FALSE
    >
    
    # iOS specific settings (JUCE 8)
    $<$<STREQUAL:${JUCE_TARGET_PLATFORM},iOS>:
        IPHONE_SCREEN_ORIENTATIONS UIInterfaceOrientationPortrait UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight
        IPAD_SCREEN_ORIENTATIONS UIInterfaceOrientationPortrait UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight UIInterfaceOrientationPortraitUpsideDown
        TARGETED_DEVICE_FAMILY "1,2"
        IOS_DEPLOYMENT_TARGET "12.0"
        MICROPHONE_PERMISSION_ENABLED TRUE
        MICROPHONE_PERMISSION_TEXT "OTTO needs microphone access for audio input"
        CAMERA_PERMISSION_ENABLED FALSE
        BLUETOOTH_PERMISSION_ENABLED FALSE
    >
    
    # Android specific (JUCE 8)
    $<$<STREQUAL:${JUCE_TARGET_PLATFORM},Android>:
        ANDROID_MIN_SDK_VERSION 23
        ANDROID_TARGET_SDK_VERSION 34
        ANDROID_ACTIVITY_CLASS_NAME "com.automagicart.otto.OTTOActivity"
        ANDROID_INTERNET_NEEDED FALSE
        ANDROID_MIC_NEEDED TRUE
        ANDROID_BLUETOOTH_NEEDED FALSE
        ANDROID_EXTERNAL_READ_NEEDED FALSE
        ANDROID_EXTERNAL_WRITE_NEEDED FALSE
    >
    
    # Windows specific (JUCE 8)
    $<$<STREQUAL:${JUCE_TARGET_PLATFORM},Windows>:
        COMPANY_WEBSITE "https://automagicart.com"
        COMPANY_EMAIL "support@automagicart.com"
        WIN32_EXECUTABLE_VERSION "1.0.0.0"
    >
)

# Collect all source files
file(GLOB_RECURSE SOURCE_FILES "Source/*.cpp")
file(GLOB_RECURSE HEADER_FILES "Source/*.h")

# Exclude test files
list(FILTER SOURCE_FILES EXCLUDE REGEX "Source/tests/.*")

# Ensure our new proportional scaling files are included
set(PROPORTIONAL_SCALING_SOURCES
    "Source/ProportionalScalingManager.cpp"
    "Source/ProportionalScalingDemo.cpp"
    "Source/EnhancedProportionalEditor.cpp"
    "Source/ProportionalPluginEditor.cpp"
)

set(PROPORTIONAL_SCALING_HEADERS
    "Source/ProportionalScalingManager.h"
    "Source/ProportionalScalingDemo.h"
    "Source/EnhancedProportionalEditor.h"
    "Source/ProportionalPluginEditor.h"
    "Source/JUCE8Compatibility.h"
)

# Add proportional scaling sources explicitly
list(APPEND SOURCE_FILES ${PROPORTIONAL_SCALING_SOURCES})
list(APPEND HEADER_FILES ${PROPORTIONAL_SCALING_HEADERS})

# Add sources to target
target_sources(OTTO PRIVATE ${SOURCE_FILES})

# Include directories
target_include_directories(OTTO PRIVATE
    Source
    JuceLibraryCode
    Assets
)

# Link JUCE 8 modules with plugin client support
target_link_libraries(OTTO PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_plugin_client
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_opengl
)

# Platform-specific additional libraries for JUCE 8
if(JUCE_TARGET_PLATFORM STREQUAL "iOS")
    target_link_libraries(OTTO PRIVATE
        "-framework CoreMIDI"
        "-framework AudioUnit"
        "-framework AudioToolbox"
        "-framework Accelerate"
        "-framework QuartzCore"
        "-framework CoreGraphics"
        "-framework UIKit"
        "-framework Foundation"
        "-framework AVFoundation"
    )
elseif(JUCE_TARGET_PLATFORM STREQUAL "macOS")
    target_link_libraries(OTTO PRIVATE
        "-framework CoreMIDI"
        "-framework AudioUnit"
        "-framework AudioToolbox" 
        "-framework Accelerate"
        "-framework QuartzCore"
        "-framework CoreGraphics"
        "-framework Cocoa"
        "-framework Foundation"
        "-framework AVFoundation"
        "-framework CoreAudio"
    )
elseif(JUCE_TARGET_PLATFORM STREQUAL "Windows")
    # Apply advanced Windows configuration
    configure_windows_build(OTTO)
    
    # Phase 2.3.1 - Apply Parallels VM optimization
    if(COMMAND configure_parallels_build)
        configure_parallels_build(OTTO)
    endif()
    
    # Phase 2.3.1 - Apply Reaper testing integration
    if(COMMAND configure_reaper_testing)
        configure_reaper_testing(OTTO)
    endif()
    
    # Legacy libraries (kept for compatibility)
    target_link_libraries(OTTO PRIVATE
        winmm.lib
        ole32.lib
        oleaut32.lib
        imm32.lib
        version.lib
        shlwapi.lib
        wininet.lib
        ws2_32.lib
    )
elseif(JUCE_TARGET_PLATFORM STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ALSA REQUIRED alsa)
    pkg_check_modules(FREETYPE REQUIRED freetype2)
    pkg_check_modules(X11 REQUIRED x11)
    pkg_check_modules(XEXT REQUIRED xext)
    pkg_check_modules(XRANDR REQUIRED xrandr)
    pkg_check_modules(XINERAMA REQUIRED xinerama)
    pkg_check_modules(XCURSOR REQUIRED xcursor)
    
    target_link_libraries(OTTO PRIVATE
        ${ALSA_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        ${X11_LIBRARIES}
        ${XEXT_LIBRARIES}
        ${XRANDR_LIBRARIES}
        ${XINERAMA_LIBRARIES}
        ${XCURSOR_LIBRARIES}
        pthread
        rt
        dl
    )
    
    target_include_directories(OTTO PRIVATE
        ${ALSA_INCLUDE_DIRS}
        ${FREETYPE_INCLUDE_DIRS}
        ${X11_INCLUDE_DIRS}
    )
elseif(JUCE_TARGET_PLATFORM STREQUAL "Android")
    target_link_libraries(OTTO PRIVATE
        android
        log
        OpenSLES
    )
endif()

# JUCE 8 compile definitions matching ProJucer JUCEOPTIONS
target_compile_definitions(OTTO PRIVATE
    # JUCE global header definition (required for JUCE 8)
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
    
    # Build type definitions
    $<$<CONFIG:Debug>:DEBUG=1>
    $<$<CONFIG:Release>:NDEBUG=1>
    
    # JUCE 8 specific definitions
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_ALLOW_STATIC_NULL_VARIABLES=0
    JUCE_LOG_ASSERTIONS=0
    JUCE_CHECK_MEMORY_LEAKS=1
    JUCE_DONT_DECLARE_PROJECTINFO=1
    
    # OpenGL and graphics optimizations for JUCE 8
    JUCE_OPENGL=1
    JUCE_OPENGL3=1
    JUCE_USE_OPENGL_SHADERS=1
    
    # Plugin format controls (dynamic based on platform)
    JucePlugin_Build_VST=0
    $<$<IN_LIST:VST3,${OTTO_FORMATS}>:JucePlugin_Build_VST3=1>
    $<$<IN_LIST:AU,${OTTO_FORMATS}>:JucePlugin_Build_AU=1>
    $<$<IN_LIST:AUv3,${OTTO_FORMATS}>:JucePlugin_Build_AUv3=1>
    $<$<IN_LIST:CLAP,${OTTO_FORMATS}>:JucePlugin_Build_CLAP=1>
    $<$<IN_LIST:Standalone,${OTTO_FORMATS}>:JucePlugin_Build_Standalone=1>
    JucePlugin_Build_AAX=0
    JucePlugin_Build_Unity=0
    JucePlugin_Build_LV2=0
    
    # Plugin characteristics matching ProJucer
    JucePlugin_IsSynth=1
    JucePlugin_WantsMidiInput=1
    JucePlugin_ProducesMidiOutput=1
    JucePlugin_IsMidiEffect=1
    
    # Platform-specific audio backend definitions
    $<$<STREQUAL:${JUCE_TARGET_PLATFORM},macOS>:JUCE_COREAUDIO_ENABLED=1>
    $<$<STREQUAL:${JUCE_TARGET_PLATFORM},iOS>:JUCE_IOS_AUDIO_ENABLED=1>
    $<$<STREQUAL:${JUCE_TARGET_PLATFORM},Windows>:JUCE_ASIO_ENABLED=1 JUCE_DIRECTSOUND_ENABLED=1>
    $<$<STREQUAL:${JUCE_TARGET_PLATFORM},Linux>:JUCE_ALSA_ENABLED=1 JUCE_JACK_ENABLED=1>
    $<$<STREQUAL:${JUCE_TARGET_PLATFORM},Android>:JUCE_OPENSLES_ENABLED=1>
)

# Compiler flags for optimal performance
target_compile_options(OTTO PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -O3>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -O3>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /O2>
    
    # Platform-specific optimizations
    $<$<AND:$<STREQUAL:${JUCE_TARGET_PLATFORM},macOS>,$<STREQUAL:${CMAKE_OSX_ARCHITECTURES},x86_64>>:-msse4.2 -mavx>
    $<$<STREQUAL:${JUCE_TARGET_PLATFORM},Linux>:-fPIC -msse4.2 -mavx -pthread>
    $<$<STREQUAL:${JUCE_TARGET_PLATFORM},Windows>:/arch:SSE2>
    $<$<STREQUAL:${JUCE_TARGET_PLATFORM},Android>:-ffast-math>
)

# Platform-specific system libraries
if(JUCE_TARGET_PLATFORM STREQUAL "macOS")
    target_link_libraries(OTTO PRIVATE
        "-framework AudioToolbox"
        "-framework Carbon"
        "-framework Cocoa"
        "-framework CoreAudio"
        "-framework CoreFoundation"
        "-framework CoreMIDI"
        "-framework IOKit"
        "-framework QuartzCore"
        "-framework Accelerate"
    )
elseif(JUCE_TARGET_PLATFORM STREQUAL "iOS")
    target_link_libraries(OTTO PRIVATE
        "-framework AudioToolbox"
        "-framework AVFoundation"
        "-framework CoreAudio"
        "-framework CoreFoundation"
        "-framework CoreMIDI"
        "-framework Foundation"
        "-framework QuartzCore"
        "-framework UIKit"
        "-framework Accelerate"
    )
elseif(JUCE_TARGET_PLATFORM STREQUAL "Windows")
    # Additional Windows system libraries are handled by configure_windows_build()
    # Legacy support maintained for manual library linking if needed
    target_link_libraries(OTTO PRIVATE
        winmm ole32 ws2_32 wininet version kernel32 user32 gdi32 winspool
        shell32 oleaut32 uuid comdlg32 advapi32
    )
elseif(JUCE_TARGET_PLATFORM STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ALSA REQUIRED alsa)
    pkg_check_modules(FREETYPE REQUIRED freetype2)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    
    target_link_libraries(OTTO PRIVATE
        ${ALSA_LIBRARIES} ${FREETYPE_LIBRARIES} ${GTK3_LIBRARIES}
        pthread dl rt
    )
    
    target_include_directories(OTTO PRIVATE
        ${ALSA_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${GTK3_INCLUDE_DIRS}
    )
elseif(JUCE_TARGET_PLATFORM STREQUAL "Android")
    target_link_libraries(OTTO PRIVATE
        android log OpenSLES GLESv2 EGL
    )
    
    # Phase 2.3.4 - Apply Android-specific optimizations
    if(COMMAND configure_enhanced_android_build)
        configure_enhanced_android_build(OTTO)
    endif()
endif()

# Phase 2.3.5 - Apply advanced robustness features to all platforms
if(COMMAND configure_advanced_robustness_features)
    configure_advanced_robustness_features(OTTO)
    message(STATUS "✅ Phase 2.3.5: Advanced robustness features applied to OTTO target")
endif()

# Phase 2.3.3 - Apply iOS-specific optimizations (when building for iOS)
if(JUCE_TARGET_PLATFORM STREQUAL "iOS" AND COMMAND configure_enhanced_ios_build)
    configure_enhanced_ios_build(OTTO)
    message(STATUS "✅ Phase 2.3.3: Enhanced iOS configuration applied to OTTO target")
endif()

# Handle BinaryData files (generated by ProJucer)
file(GLOB BINARY_DATA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/BinaryData*.cpp")
if(BINARY_DATA_FILES)
    target_sources(OTTO PRIVATE ${BINARY_DATA_FILES})
    list(LENGTH BINARY_DATA_FILES BINARY_DATA_COUNT)
    message(STATUS "Using ${BINARY_DATA_COUNT} BinaryData files with embedded assets")
else()
    message(WARNING "BinaryData files not found - assets may not be available")
endif()

# Set build output directories to match ProJucer structure
# All builds go into organized Builds folder structure
if(JUCE_TARGET_PLATFORM STREQUAL "macOS")
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Builds/MacOSX")
elseif(JUCE_TARGET_PLATFORM STREQUAL "iOS")
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Builds/iOS")
elseif(JUCE_TARGET_PLATFORM STREQUAL "Windows")
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Builds/VisualStudio2022")
elseif(JUCE_TARGET_PLATFORM STREQUAL "Linux")
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Builds/LinuxMakefile")
elseif(JUCE_TARGET_PLATFORM STREQUAL "Android")
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Builds/Android")
else()
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Builds/CMake")
endif()

# Set output directories for all build configurations
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} CONFIG_UPPER)
    set_target_properties(OTTO PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${OUTPUT_DIR}/${config}"
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${OUTPUT_DIR}/${config}"
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${OUTPUT_DIR}/${config}"
    )
endforeach()

# Fallback for single-config generators
set_target_properties(OTTO PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/Release"
    LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}/Release"
    ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/Release"
)

# iOS Code Signing Configuration
if(JUCE_TARGET_PLATFORM STREQUAL "iOS")
    # Set up code signing for all OTTO targets
    foreach(target OTTO_Standalone OTTO_AU OTTO_VST3)
        if(TARGET ${target})
            set_target_properties(${target} PROPERTIES
                XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic"
                XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM}"
                XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.automagicart.OTTO"
                XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
                XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER ""
                XCODE_ATTRIBUTE_ENABLE_BITCODE NO
                XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH NO
                XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "12.0"
                XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
                XCODE_ATTRIBUTE_SUPPORTS_MACCATALYST NO
            )
        endif()
    endforeach()
    
    message(STATUS "iOS Code Signing configured for automatic signing")
    message(STATUS "Bundle ID: com.automagicart.OTTO")
    message(STATUS "Development Team: ${CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM}")
endif()

# macOS specific properties
if(JUCE_TARGET_PLATFORM STREQUAL "macOS")
    set_target_properties(OTTO PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC NO
        XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES
        XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.13"
    )
endif()

# Enable copy plugin step for easier testing
juce_enable_copy_plugin_step(OTTO)

# Generate JUCE header
juce_generate_juce_header(OTTO)

# Print build configuration summary
message(STATUS "OTTO Build Configuration Summary:")
message(STATUS "  Platform: ${JUCE_TARGET_PLATFORM}")
message(STATUS "  Output Directory: ${OUTPUT_DIR}")
message(STATUS "  Architecture: ${CMAKE_OSX_ARCHITECTURES}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Generator: ${CMAKE_GENERATOR}")