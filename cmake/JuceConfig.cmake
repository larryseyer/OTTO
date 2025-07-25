# JUCE 8 Configuration for OTTO
# This file contains JUCE 8 specific settings and compatibility configurations

# JUCE 8 requires CMake 3.22+
cmake_minimum_required(VERSION 3.22)

# Set JUCE 8 specific compile definitions
function(configure_juce8_target target_name)
    target_compile_definitions(${target_name} PRIVATE
        # JUCE 8 specific definitions
        JUCE_STRICT_REFCOUNTEDPOINTER=1
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        
        # Audio plugin settings
        JucePlugin_Build_VST=0
        JucePlugin_Build_VST3=1
        JucePlugin_Build_AU=1
        JucePlugin_Build_Standalone=1
        JucePlugin_Build_AAX=0
        JucePlugin_Build_Unity=0
        
        # Plugin characteristics
        JucePlugin_IsSynth=1
        JucePlugin_WantsMidiInput=1
        JucePlugin_ProducesMidiOutput=1
        JucePlugin_EditorRequiresKeyboardFocus=0
        
        # Platform specific
        $<$<PLATFORM_ID:Darwin>:JUCE_MAC=1>
        $<$<PLATFORM_ID:iOS>:JUCE_IOS=1>
        $<$<PLATFORM_ID:Windows>:JUCE_WINDOWS=1>
        $<$<PLATFORM_ID:Linux>:JUCE_LINUX=1>
        $<$<PLATFORM_ID:Android>:JUCE_ANDROID=1>
    )
    
    # JUCE 8 compiler flags with platform-specific optimizations
    target_compile_options(${target_name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -O3>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic -O3>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /O2 /arch:AVX2>
        
        $<$<PLATFORM_ID:Darwin>:-Wno-deprecated-declarations -fno-objc-arc>
        $<$<PLATFORM_ID:iOS>:-Wno-deprecated-declarations -fno-objc-arc>
        $<$<PLATFORM_ID:Linux>:-fPIC -msse4.2 -mavx -pthread>
        $<$<PLATFORM_ID:Windows>:/arch:SSE2>
        $<$<PLATFORM_ID:Android>:-mfpu=neon -ffast-math>
    )
    
    target_compile_definitions(${target_name} PRIVATE
        $<$<PLATFORM_ID:Darwin>:JUCE_COREAUDIO_ENABLED=1>
        $<$<PLATFORM_ID:iOS>:JUCE_IOS_AUDIO_ENABLED=1>
        $<$<PLATFORM_ID:Windows>:JUCE_ASIO_ENABLED=1 JUCE_DIRECTSOUND_ENABLED=1>
        $<$<PLATFORM_ID:Linux>:JUCE_ALSA_ENABLED=1 JUCE_JACK_ENABLED=1>
        $<$<PLATFORM_ID:Android>:JUCE_OPENSLES_ENABLED=1>
    )
    
    # Set properties for JUCE 8
    set_target_properties(${target_name} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
    
    # Platform-specific properties
    if(APPLE)
        set_target_properties(${target_name} PROPERTIES
            XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC NO
            XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES
        )
        if(IOS)
            set_target_properties(${target_name} PROPERTIES
                XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
            )
        endif()
    endif()
endfunction()

# Function to add JUCE 8 modules with proper configuration
function(add_juce8_modules target_name)
    target_link_libraries(${target_name} PRIVATE
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
endfunction()

# Function to setup platform-specific libraries for JUCE 8
function(setup_platform_libraries target_name)
    if(APPLE)
        if(IOS)
            target_link_libraries(${target_name} PRIVATE
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
        else() # macOS
            target_link_libraries(${target_name} PRIVATE
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
        endif()
    elseif(WIN32)
        target_link_libraries(${target_name} PRIVATE
            winmm
            ole32
            ws2_32
            wininet
            version
            kernel32
            user32
            gdi32
            winspool
            shell32
            oleaut32
            uuid
            comdlg32
            advapi32
        )
    elseif(UNIX AND NOT APPLE AND NOT ANDROID)
        # Linux
        find_package(PkgConfig REQUIRED)
        
        pkg_check_modules(ALSA REQUIRED alsa)
        pkg_check_modules(FREETYPE REQUIRED freetype2)
        pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
        
        target_link_libraries(${target_name} PRIVATE
            ${ALSA_LIBRARIES}
            ${FREETYPE_LIBRARIES}
            ${GTK3_LIBRARIES}
            pthread
            dl
            rt
        )
        
        target_include_directories(${target_name} PRIVATE
            ${ALSA_INCLUDE_DIRS}
            ${FREETYPE_INCLUDE_DIRS}
            ${GTK3_INCLUDE_DIRS}
        )
    elseif(ANDROID)
        target_link_libraries(${target_name} PRIVATE
            android
            log
            OpenSLES
            GLESv2
            EGL
        )
    endif()
endfunction()

# Function to configure JUCE 8 plugin formats
function(configure_plugin_formats target_name)
    # Generate JUCE AppConfig.h
    juce_generate_juce_header(${target_name})
endfunction()
