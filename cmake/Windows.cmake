#==============================================================================
# OTTO Phase 2.3.1 - Windows Build Configuration (JUCE 8)
# Advanced Windows-specific build settings for Visual Studio 2022
#==============================================================================

cmake_minimum_required(VERSION 3.22)

# Ensure we're on Windows
if(NOT WIN32)
    message(FATAL_ERROR "Windows.cmake can only be used on Windows platforms")
endif()

#==============================================================================
# Windows Platform Detection & Configuration
#==============================================================================

# Set Windows-specific variables
set(JUCE_TARGET_PLATFORM "Windows")
set(CMAKE_SYSTEM_VERSION "10.0")

# Windows architecture detection and optimization
if(NOT CMAKE_GENERATOR_PLATFORM)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CMAKE_GENERATOR_PLATFORM "x64")
        set(OTTO_WINDOWS_ARCH "x64")
    else()
        set(CMAKE_GENERATOR_PLATFORM "Win32")
        set(OTTO_WINDOWS_ARCH "Win32")
    endif()
endif()

message(STATUS "Windows Build Configuration:")
message(STATUS "  Platform: ${JUCE_TARGET_PLATFORM}")
message(STATUS "  Architecture: ${OTTO_WINDOWS_ARCH}")
message(STATUS "  System Version: ${CMAKE_SYSTEM_VERSION}")

#==============================================================================
# Visual Studio Configuration
#==============================================================================

# Ensure we're using Visual Studio 2022 or newer
if(CMAKE_GENERATOR MATCHES "Visual Studio")
    # Extract Visual Studio version
    string(REGEX MATCH "Visual Studio ([0-9]+)" VS_VERSION_MATCH ${CMAKE_GENERATOR})
    if(CMAKE_MATCH_1 AND CMAKE_MATCH_1 LESS 17)
        message(WARNING "Visual Studio ${CMAKE_MATCH_1} detected. Visual Studio 2022 (17) or newer recommended for JUCE 8")
    endif()
    
    # Configure Visual Studio specific settings
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Build configurations" FORCE)
    
    # Enable multi-processor compilation
    add_compile_options(/MP)
    
    # Set Windows SDK version
    if(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
        message(STATUS "  Windows SDK: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
    else()
        # Auto-detect latest Windows SDK
        set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "10.0")
        message(STATUS "  Windows SDK: Auto-detected (${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION})")
    endif()
endif()

#==============================================================================
# Windows Audio Driver Configuration
#==============================================================================

# DirectSound and WASAPI configuration
add_compile_definitions(
    JUCE_DIRECTSOUND_ENABLED=1
    JUCE_WASAPI_ENABLED=1
    JUCE_ASIO_ENABLED=1
    JUCE_WINMM_ENABLED=1
)

# Windows audio libraries
set(OTTO_WINDOWS_AUDIO_LIBS
    winmm.lib
    dsound.lib
    dxguid.lib
    ws2_32.lib
)

#==============================================================================
# Windows System Libraries
#==============================================================================

# Core Windows system libraries for JUCE 8
set(OTTO_WINDOWS_SYSTEM_LIBS
    kernel32.lib
    user32.lib
    gdi32.lib
    winspool.lib
    shell32.lib
    ole32.lib
    oleaut32.lib
    uuid.lib
    comdlg32.lib
    advapi32.lib
    imm32.lib
    version.lib
    shlwapi.lib
    wininet.lib
    comctl32.lib
    rpcrt4.lib
)

#==============================================================================
# Windows-Specific Compile Definitions
#==============================================================================

# Windows platform definitions
add_compile_definitions(
    WIN32_LEAN_AND_MEAN=1
    NOMINMAX=1
    _WIN32_WINNT=0x0A00
    WINVER=0x0A00
    _UNICODE=1
    UNICODE=1
    _CRT_SECURE_NO_WARNINGS=1
    _WINSOCK_DEPRECATED_NO_WARNINGS=1
)

# JUCE 8 Windows-specific definitions
add_compile_definitions(
    JUCE_WIN32=1
    JUCE_WINDOWS=1
    JUCE_LITTLE_ENDIAN=1
    JUCE_INTEL=1
)

#==============================================================================
# Windows Compiler Optimizations
#==============================================================================

function(configure_windows_optimizations target)
    # Architecture-specific optimizations
    if(OTTO_WINDOWS_ARCH STREQUAL "x64")
        target_compile_options(${target} PRIVATE
            $<$<CONFIG:Release>:/arch:AVX>
            $<$<CONFIG:Release>:/fp:fast>
            $<$<CONFIG:Release>:/Ot>
            $<$<CONFIG:Release>:/Ox>
            $<$<CONFIG:Release>:/Oy>
        )
        
        target_compile_definitions(${target} PRIVATE
            JUCE_64BIT=1
            _WIN64=1
        )
    else()
        target_compile_options(${target} PRIVATE
            $<$<CONFIG:Release>:/arch:SSE2>
            $<$<CONFIG:Release>:/fp:fast>
        )
        
        target_compile_definitions(${target} PRIVATE
            JUCE_32BIT=1
        )
    endif()
    
    # Debug-specific settings
    target_compile_options(${target} PRIVATE
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Debug>:/Zi>
        $<$<CONFIG:Debug>:/RTC1>
        $<$<CONFIG:Debug>:/MDd>
    )
    
    # Release-specific settings
    target_compile_options(${target} PRIVATE
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/DNDEBUG>
        $<$<CONFIG:Release>:/MD>
        $<$<CONFIG:Release>:/GL>
    )
    
    # Link-time optimizations for Release
    target_link_options(${target} PRIVATE
        $<$<CONFIG:Release>:/LTCG>
        $<$<CONFIG:Release>:/OPT:REF>
        $<$<CONFIG:Release>:/OPT:ICF>
    )
    
    message(STATUS "Windows optimizations configured for: ${target}")
endfunction()

#==============================================================================
# Windows Plugin Format Configuration
#==============================================================================

function(configure_windows_plugin_formats target)
    # Windows supports: Standalone, VST3, CLAP
    set(WINDOWS_FORMATS Standalone VST3 CLAP)
    
    # Configure each format
    foreach(FORMAT ${WINDOWS_FORMATS})
        if(FORMAT STREQUAL "VST3")
            target_compile_definitions(${target} PRIVATE
                JucePlugin_Build_VST3=1
                JUCE_VST3_CAN_REPLACE_VST2=0
                JUCE_VST3_HOST_CALLBACK=1
            )
            
            # VST3 installation path
            set(VST3_INSTALL_PATH "$ENV{PROGRAMFILES}/Common Files/VST3")
            message(STATUS "VST3 installation path: ${VST3_INSTALL_PATH}")
            
        elseif(FORMAT STREQUAL "CLAP")
            target_compile_definitions(${target} PRIVATE
                JucePlugin_Build_CLAP=1
                JUCE_CLAP_SUPPORTS_CUSTOM_FACTORY=1
            )
            
            # CLAP installation path
            set(CLAP_INSTALL_PATH "$ENV{PROGRAMFILES}/Common Files/CLAP")
            message(STATUS "CLAP installation path: ${CLAP_INSTALL_PATH}")
            
        elseif(FORMAT STREQUAL "Standalone")
            target_compile_definitions(${target} PRIVATE
                JucePlugin_Build_Standalone=1
                JUCE_USE_WINRT_MIDI=1
            )
        endif()
    endforeach()
    
    message(STATUS "Windows plugin formats configured: ${WINDOWS_FORMATS}")
endfunction()

#==============================================================================
# Windows Audio Driver Validation
#==============================================================================

function(validate_windows_audio_drivers)
    message(STATUS "Validating Windows audio drivers...")
    
    # Check for DirectSound headers
    find_path(DSOUND_INCLUDE_DIR dsound.h
        PATHS "$ENV{WINDOWSSDKDIR}/Include/*/um"
              "$ENV{ProgramFiles}/Windows Kits/10/Include/*/um"
              "$ENV{ProgramFiles(x86)}/Windows Kits/10/Include/*/um"
    )
    
    if(DSOUND_INCLUDE_DIR)
        message(STATUS "  DirectSound: Found")
        add_compile_definitions(JUCE_DIRECTSOUND_AVAILABLE=1)
    else()
        message(WARNING "  DirectSound: Not found - some audio features may be limited")
        add_compile_definitions(JUCE_DIRECTSOUND_AVAILABLE=0)
    endif()
    
    # Check for WASAPI (always available on Windows 10+)
    if(CMAKE_SYSTEM_VERSION VERSION_GREATER_EQUAL "10.0")
        message(STATUS "  WASAPI: Available (Windows 10+)")
        add_compile_definitions(JUCE_WASAPI_AVAILABLE=1)
    else()
        message(STATUS "  WASAPI: Limited support (Windows < 10)")
        add_compile_definitions(JUCE_WASAPI_AVAILABLE=0)
    endif()
    
    # ASIO SDK check (optional)
    if(DEFINED ASIO_SDK_PATH AND EXISTS "${ASIO_SDK_PATH}")
        message(STATUS "  ASIO SDK: Found at ${ASIO_SDK_PATH}")
        add_compile_definitions(JUCE_ASIO_AVAILABLE=1)
        include_directories(${ASIO_SDK_PATH})
    else()
        message(STATUS "  ASIO SDK: Not configured (ASIO support disabled)")
        add_compile_definitions(JUCE_ASIO_AVAILABLE=0)
    endif()
endfunction()

#==============================================================================
# Windows Error Handling & Debugging
#==============================================================================

function(configure_windows_debugging target)
    # Enhanced error handling for Windows
    target_compile_definitions(${target} PRIVATE
        JUCE_CHECK_MEMORY_LEAKS=1
        JUCE_LOG_ASSERTIONS=1
        JUCE_STRICT_REFCOUNTEDPOINTER=1
    )
    
    # Debug symbol configuration
    target_compile_options(${target} PRIVATE
        $<$<CONFIG:Debug>:/Zi>
        $<$<CONFIG:Debug>:/FS>
    )
    
    # Generate PDB files for Release builds too
    target_compile_options(${target} PRIVATE
        $<$<CONFIG:Release>:/Zi>
    )
    
    target_link_options(${target} PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
        $<$<CONFIG:Release>:/DEBUG>
        $<$<CONFIG:Release>:/PDBALTPATH:%_PDB%>
    )
    
    message(STATUS "Windows debugging configured for: ${target}")
endfunction()

#==============================================================================
# Windows Deployment Configuration
#==============================================================================

function(configure_windows_deployment target)
    # Set Windows application properties
    set_target_properties(${target} PROPERTIES
        WIN32_EXECUTABLE TRUE
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
    
    # Create installer configuration
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        # Configure CPack for Windows installer
        set(CPACK_GENERATOR "WIX;ZIP")
        set(CPACK_PACKAGE_NAME "OTTO")
        set(CPACK_PACKAGE_VENDOR "Automagic Art")
        set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
        set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
        
        # WiX installer configuration
        set(CPACK_WIX_UPGRADE_GUID "12345678-1234-1234-1234-123456789012")
        set(CPACK_WIX_PRODUCT_GUID "87654321-4321-4321-4321-210987654321")
        
        include(CPack)
        message(STATUS "Windows installer configured")
    endif()
endfunction()

#==============================================================================
# Main Windows Configuration Function
#==============================================================================

function(configure_windows_build target)
    message(STATUS "Configuring Windows build for target: ${target}")
    
    # Apply all Windows configurations
    configure_windows_optimizations(${target})
    configure_windows_plugin_formats(${target})
    configure_windows_debugging(${target})
    configure_windows_deployment(${target})
    
    # Link Windows-specific libraries
    target_link_libraries(${target} PRIVATE
        ${OTTO_WINDOWS_SYSTEM_LIBS}
        ${OTTO_WINDOWS_AUDIO_LIBS}
    )
    
    # Validate audio drivers
    validate_windows_audio_drivers()
    
    message(STATUS "Windows build configuration complete for: ${target}")
endfunction()

# Export the main configuration function
set(OTTO_WINDOWS_CMAKE_LOADED TRUE)
message(STATUS "OTTO Windows CMake configuration loaded")