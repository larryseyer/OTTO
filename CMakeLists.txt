cmake_minimum_required(VERSION 3.22)

# Set project name and version
project(OTTO VERSION 1.0.0)

# Set C++ standard for JUCE 8
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include JUCE 8 configuration
include(cmake/JuceConfig.cmake)

# Platform-specific settings
if(APPLE)
    if(IOS)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
        set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "12.0")
    else()
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
    endif()
elseif(WIN32)
    set(CMAKE_SYSTEM_VERSION "10.0")
endif()

# Find JUCE - adjust path as needed
find_package(PkgConfig REQUIRED)

# If JUCE is installed via package manager or system
# find_package(juce CONFIG REQUIRED)

# If JUCE is a submodule or local installation
set(JUCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/JUCE" CACHE PATH "Path to JUCE")
if(EXISTS "${JUCE_PATH}/CMakeLists.txt")
    add_subdirectory(${JUCE_PATH} JUCE)
else()
    # Fallback to system JUCE installation
    find_package(juce CONFIG REQUIRED)
endif()

# Create the plugin target
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    # For iOS, create a simple app instead of a plugin
    juce_add_gui_app(OTTO
        COMPANY_NAME "YourCompany"  
        PRODUCT_NAME "OTTO"
        VERSION "1.0.0"
        BUNDLE_ID "com.yourcompany.otto"
        IPHONE_SCREEN_ORIENTATIONS UIInterfaceOrientationPortrait UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight
        REQUIRES_FULL_SCREEN TRUE
    )
else()
    # For other platforms, create the plugin
    juce_add_plugin(OTTO
        COMPANY_NAME "YourCompany"
        PLUGIN_MANUFACTURER_CODE "Manu"
        PLUGIN_CODE "Otto"
        FORMATS AU VST3 Standalone
        PRODUCT_NAME "OTTO"
        DESCRIPTION "OTTO Audio Plugin"
        
        # Plugin characteristics
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT TRUE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        
        # Android specific  
        $<$<PLATFORM_ID:Android>:
            ANDROID_MIN_SDK_VERSION 23
            ANDROID_TARGET_SDK_VERSION 33
        >
    )
endif()

# Source files - only include existing files
file(GLOB_RECURSE SOURCE_FILES
    "Source/*.cpp"
)

# Exclude test files from main build
list(FILTER SOURCE_FILES EXCLUDE REGEX "Source/tests/.*")

# For iOS, exclude plugin-specific files and include iOS main
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    list(FILTER SOURCE_FILES EXCLUDE REGEX "Source/PluginProcessor\\.cpp$")
    list(FILTER SOURCE_FILES EXCLUDE REGEX "Source/PluginEditor\\.cpp$")
    # Add iOS-specific main file
    list(APPEND SOURCE_FILES "Source/Main_iOS.cpp")
else()
    # For non-iOS platforms, exclude the iOS main file
    list(FILTER SOURCE_FILES EXCLUDE REGEX "Source/Main_iOS\\.cpp$")
endif()

# Add sources to target
target_sources(OTTO PRIVATE ${SOURCE_FILES})

# Include JuceLibraryCode for header access
target_include_directories(OTTO PRIVATE JuceLibraryCode)

# Include directories
target_include_directories(OTTO PRIVATE
    Source
    JuceLibraryCode
    Assets
)

# Apply JUCE 8 configuration
configure_juce8_target(OTTO)
add_juce8_modules(OTTO)
setup_platform_libraries(OTTO)
configure_plugin_formats(OTTO)

# Use existing BinaryData files (ProJucer generated - may be split into multiple files)
file(GLOB BINARY_DATA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/BinaryData*.cpp")
if(BINARY_DATA_FILES)
    target_sources(OTTO PRIVATE ${BINARY_DATA_FILES})
    list(LENGTH BINARY_DATA_FILES BINARY_DATA_COUNT)
    message(STATUS "Using ${BINARY_DATA_COUNT} BinaryData files with embedded assets")
else()
    message(WARNING "BinaryData files not found - assets may not be available")
endif()

# Copy plugin after build for easier testing (only for plugins, not iOS apps)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    juce_enable_copy_plugin_step(OTTO)
endif()