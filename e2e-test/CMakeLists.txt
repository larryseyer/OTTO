# OTTO E2E Tests CMake Configuration
# Comprehensive End-to-End Testing Suite

cmake_minimum_required(VERSION 3.22)

# Project definition
project(OTTO_E2E_Tests VERSION 1.0.0)

# Ensure JUCE 8 compliance
if(NOT DEFINED JUCE_VERSION)
    message(FATAL_ERROR "JUCE version not defined. This project requires JUCE 8.0.8+")
endif()

if(JUCE_VERSION VERSION_LESS "8.0.8")
    message(FATAL_ERROR "This project requires JUCE 8.0.8 or higher. Found: ${JUCE_VERSION}")
endif()

message(STATUS "Building OTTO E2E Tests with JUCE ${JUCE_VERSION}")

# Set C++ standard (JUCE 8 requirement)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JUCE
find_package(JUCE CONFIG REQUIRED)

# Include parent project's source directory for access to OTTO components
set(OTTO_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Source")
set(OTTO_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

# Verify OTTO source files exist
if(NOT EXISTS "${OTTO_SOURCE_DIR}/INIConfig.h")
    message(FATAL_ERROR "OTTO source files not found. Expected at: ${OTTO_SOURCE_DIR}")
endif()

message(STATUS "OTTO source directory: ${OTTO_SOURCE_DIR}")

# E2E Test executable
juce_add_gui_app(OTTO_E2E_Tests
    PRODUCT_NAME "OTTO E2E Tests"
    VERSION "1.0.0"
    COMPANY_NAME "OTTO Audio"
    COMPANY_WEBSITE "https://github.com/larryseyer/OTTO"
    COMPANY_EMAIL "support@otto-audio.com"
    
    # Bundle identifier for macOS/iOS
    BUNDLE_ID "com.otto-audio.e2e-tests"
    
    # Icon (use OTTO's icon if available)
    # ICON_BIG "${OTTO_ROOT_DIR}/Resources/icon.png"
    # ICON_SMALL "${OTTO_ROOT_DIR}/Resources/icon_small.png"
    
    # Microphone permission for audio testing
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "OTTO E2E Tests need microphone access for audio testing"
    
    # File access for preset testing
    FILE_SHARING_ENABLED TRUE
    DOCUMENT_BROWSER_ENABLED TRUE
)

# E2E Test source files
target_sources(OTTO_E2E_Tests PRIVATE
    # Main E2E test application
    E2ETestMain.cpp
    
    # Test suite headers (header-only)
    UIWorkflowTests.h
    ComponentIntegrationTests.h
    UserInteractionTests.h
    StateManagementE2ETests.h
    E2ETestRunner.h
    
    # Include OTTO source files needed for testing
    ${OTTO_SOURCE_DIR}/INIConfig.h
    ${OTTO_SOURCE_DIR}/ComponentState.h
    ${OTTO_SOURCE_DIR}/INIDataManager.h
    ${OTTO_SOURCE_DIR}/StateManager.h
    ${OTTO_SOURCE_DIR}/UtilityComponents.h
    ${OTTO_SOURCE_DIR}/CustomLookAndFeel.h
    
    # Include main OTTO components for integration testing
    ${OTTO_SOURCE_DIR}/PluginProcessor.cpp
    ${OTTO_SOURCE_DIR}/PluginProcessor.h
    ${OTTO_SOURCE_DIR}/PluginEditor.cpp
    ${OTTO_SOURCE_DIR}/PluginEditor.h
    ${OTTO_SOURCE_DIR}/MainContentComponent.cpp
    ${OTTO_SOURCE_DIR}/MainContentComponent.h
)

# Include directories
target_include_directories(OTTO_E2E_Tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OTTO_SOURCE_DIR}
    ${OTTO_ROOT_DIR}
)

# Compiler definitions
target_compile_definitions(OTTO_E2E_Tests PRIVATE
    # JUCE app config
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:OTTO_E2E_Tests,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:OTTO_E2E_Tests,JUCE_VERSION>"
    
    # E2E test specific definitions
    OTTO_E2E_TESTS=1
    OTTO_TESTING_MODE=1
    
    # Platform-specific definitions
    $<$<PLATFORM_ID:Darwin>:JUCE_MAC=1>
    $<$<PLATFORM_ID:Windows>:JUCE_WINDOWS=1>
    $<$<PLATFORM_ID:Linux>:JUCE_LINUX=1>
    $<$<PLATFORM_ID:iOS>:JUCE_IOS=1>
    $<$<PLATFORM_ID:Android>:JUCE_ANDROID=1>
)

# Link JUCE libraries
target_link_libraries(OTTO_E2E_Tests PRIVATE
    # Core JUCE modules
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    
    # Audio modules for testing audio components
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    
    # Additional modules for comprehensive testing
    juce::juce_opengl
    
    # JUCE recommended flags
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# Platform-specific configurations
if(APPLE)
    # macOS/iOS specific settings
    set_target_properties(OTTO_E2E_Tests PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.otto-audio.e2e-tests"
        MACOSX_BUNDLE_BUNDLE_NAME "OTTO E2E Tests"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
        MACOSX_BUNDLE_COPYRIGHT "© 2025 OTTO Audio"
    )
    
    # macOS deployment target
    if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "macOS deployment target")
    endif()
    
    # iOS specific settings
    if(IOS)
        set_target_properties(OTTO_E2E_Tests PROPERTIES
            XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"  # iPhone and iPad
            XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "13.0"
            XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
        )
    endif()
    
elseif(WIN32)
    # Windows specific settings
    set_target_properties(OTTO_E2E_Tests PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # Windows version info
    target_compile_definitions(OTTO_E2E_Tests PRIVATE
        JUCE_APP_VERSION="1.0.0"
        JUCE_APP_VERSION_HEX=0x10000
    )
    
elseif(UNIX AND NOT APPLE)
    # Linux specific settings
    target_link_libraries(OTTO_E2E_Tests PRIVATE
        pthread
        dl
    )
    
    # Find required Linux packages
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    pkg_check_modules(WEBKIT2 REQUIRED webkit2gtk-4.0)
    
    target_include_directories(OTTO_E2E_Tests PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(OTTO_E2E_Tests PRIVATE ${GTK3_LIBRARIES})
    
endif()

# Compiler-specific settings
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(OTTO_E2E_Tests PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
        -Wno-missing-field-initializers
    )
    
    # Release optimizations
    target_compile_options(OTTO_E2E_Tests PRIVATE
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-DNDEBUG>
    )
    
    # Debug settings
    target_compile_options(OTTO_E2E_Tests PRIVATE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Debug>:-DDEBUG>
    )
    
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(OTTO_E2E_Tests PRIVATE
        /W4
        /permissive-
        /Zc:__cplusplus
    )
    
    # Release optimizations
    target_compile_options(OTTO_E2E_Tests PRIVATE
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/DNDEBUG>
    )
    
    # Debug settings
    target_compile_options(OTTO_E2E_Tests PRIVATE
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Debug>:/DDEBUG>
    )
endif()

# Custom target for running E2E tests
add_custom_target(run_e2e_tests
    COMMAND $<TARGET_FILE:OTTO_E2E_Tests> --all --verbose
    DEPENDS OTTO_E2E_Tests
    COMMENT "Running OTTO E2E Tests"
    VERBATIM
)

# Custom target for running specific test suites
add_custom_target(run_ui_workflow_tests
    COMMAND $<TARGET_FILE:OTTO_E2E_Tests> --ui-workflow --verbose
    DEPENDS OTTO_E2E_Tests
    COMMENT "Running UI Workflow Tests"
    VERBATIM
)

add_custom_target(run_component_integration_tests
    COMMAND $<TARGET_FILE:OTTO_E2E_Tests> --component-integration --verbose
    DEPENDS OTTO_E2E_Tests
    COMMENT "Running Component Integration Tests"
    VERBATIM
)

add_custom_target(run_user_interaction_tests
    COMMAND $<TARGET_FILE:OTTO_E2E_Tests> --user-interaction --verbose
    DEPENDS OTTO_E2E_Tests
    COMMENT "Running User Interaction Tests"
    VERBATIM
)

add_custom_target(run_state_management_tests
    COMMAND $<TARGET_FILE:OTTO_E2E_Tests> --state-management --verbose
    DEPENDS OTTO_E2E_Tests
    COMMENT "Running State Management Tests"
    VERBATIM
)

# Custom target for generating test reports
add_custom_target(generate_e2e_report
    COMMAND $<TARGET_FILE:OTTO_E2E_Tests> --all --report e2e_test_results.xml --verbose
    DEPENDS OTTO_E2E_Tests
    COMMENT "Generating E2E Test Report"
    VERBATIM
)

# Installation settings
install(TARGETS OTTO_E2E_Tests
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "OTTO_E2E_Tests")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OTTO End-to-End Test Suite")
set(CPACK_PACKAGE_VENDOR "OTTO Audio")
set(CPACK_PACKAGE_CONTACT "support@otto-audio.com")

if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_FORMAT "UDBZ")
    set(CPACK_DMG_VOLUME_NAME "OTTO E2E Tests")
elseif(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "OTTO E2E Tests")
    set(CPACK_NSIS_PACKAGE_NAME "OTTO E2E Tests")
elseif(UNIX)
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "OTTO Audio <support@otto-audio.com>")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

include(CPack)

# Print configuration summary
message(STATUS "")
message(STATUS "OTTO E2E Tests Configuration Summary:")
message(STATUS "=====================================")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "JUCE Version: ${JUCE_VERSION}")
message(STATUS "Target Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "OTTO Source Dir: ${OTTO_SOURCE_DIR}")
message(STATUS "")
message(STATUS "Available targets:")
message(STATUS "  OTTO_E2E_Tests           - Build E2E test application")
message(STATUS "  run_e2e_tests           - Run all E2E tests")
message(STATUS "  run_ui_workflow_tests   - Run UI workflow tests only")
message(STATUS "  run_component_integration_tests - Run component integration tests only")
message(STATUS "  run_user_interaction_tests - Run user interaction tests only")
message(STATUS "  run_state_management_tests - Run state management tests only")
message(STATUS "  generate_e2e_report     - Generate XML test report")
message(STATUS "")
message(STATUS "Usage examples:")
message(STATUS "  cmake --build . --target OTTO_E2E_Tests")
message(STATUS "  cmake --build . --target run_e2e_tests")
message(STATUS "  cmake --build . --target generate_e2e_report")
message(STATUS "")