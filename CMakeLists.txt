cmake_minimum_required(VERSION 3.22)

# Set project name and version to match ProJucer
project(OTTO VERSION 1.0.0)

# Set C++ standard for JUCE 8
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(JUCE_TARGET_PLATFORM "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
    set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "12.0")
elseif(APPLE)
    set(JUCE_TARGET_PLATFORM "macOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
elseif(ANDROID)
    set(JUCE_TARGET_PLATFORM "Android")
elseif(WIN32)
    set(JUCE_TARGET_PLATFORM "Windows")
    set(CMAKE_SYSTEM_VERSION "10.0")
elseif(UNIX)
    set(JUCE_TARGET_PLATFORM "Linux")
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

# Create the plugin target with settings matching ProJucer file
juce_add_plugin(OTTO
    # Company and product info to match ProJucer
    COMPANY_NAME "Automagic Art"
    PLUGIN_MANUFACTURER_CODE "OTTO"
    PLUGIN_CODE "OTTO"
    FORMATS AU VST3 CLAP Standalone
    PRODUCT_NAME "OTTO"
    DESCRIPTION "OTTO."
    BUNDLE_ID "com.automagicart.OTTO"
    
    # Plugin characteristics matching ProJucer
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    
    # Plugin characteristics value from ProJucer
    PLUGIN_CHARACTERISTICS_VALUE "pluginIsMidiEffectPlugin,pluginIsSynth,pluginProducesMidiOut,pluginWantsMidiIn"
    
    # iOS specific settings
    $<$<STREQUAL:${JUCE_TARGET_PLATFORM},iOS>:
        IPHONE_SCREEN_ORIENTATIONS UIInterfaceOrientationPortrait UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight
        REQUIRES_FULL_SCREEN TRUE
        TARGETED_DEVICE_FAMILY "1,2"
    >
    
    # Android specific  
    $<$<STREQUAL:${JUCE_TARGET_PLATFORM},Android>:
        ANDROID_MIN_SDK_VERSION 23
        ANDROID_TARGET_SDK_VERSION 33
    >
)

# Collect all source files
file(GLOB_RECURSE SOURCE_FILES "Source/*.cpp")
file(GLOB_RECURSE HEADER_FILES "Source/*.h")

# Exclude test files
list(FILTER SOURCE_FILES EXCLUDE REGEX "Source/tests/.*")

# Add sources to target
target_sources(OTTO PRIVATE ${SOURCE_FILES})

# Include directories
target_include_directories(OTTO PRIVATE
    Source
    JuceLibraryCode
    Assets
)

# Link JUCE modules
target_link_libraries(OTTO PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# JUCE 8 compile definitions matching ProJucer JUCEOPTIONS
target_compile_definitions(OTTO PRIVATE
    # JUCE 8 specific definitions
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    
    # Plugin format controls
    JucePlugin_Build_VST=0
    JucePlugin_Build_VST3=1
    JucePlugin_Build_AU=1
    JucePlugin_Build_CLAP=1
    JucePlugin_Build_Standalone=1
    JucePlugin_Build_AAX=0
    JucePlugin_Build_Unity=0
    
    # Plugin characteristics matching ProJucer
    JucePlugin_IsSynth=1
    JucePlugin_WantsMidiInput=1
    JucePlugin_ProducesMidiOutput=1
    JucePlugin_EditorRequiresKeyboardFocus=0
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