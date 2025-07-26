# iOS Simulator Toolchain File for JUCE 8 OTTO Project
# Ensures builds go to organized Builds/iOS directory structure

set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_SYSTEM_VERSION 12.0)
set(CMAKE_OSX_SYSROOT iphonesimulator)
set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")

# Auto-detect simulator architecture based on host
execute_process(
    COMMAND uname -m
    OUTPUT_VARIABLE HOST_ARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(HOST_ARCH STREQUAL "arm64")
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    message(STATUS "iOS Simulator Toolchain: Using arm64 for Apple Silicon")
else()
    set(CMAKE_OSX_ARCHITECTURES "x86_64") 
    message(STATUS "iOS Simulator Toolchain: Using x86_64 for Intel")
endif()

# Ensure builds go to the proper Builds directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Builds/iOS)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Builds/iOS)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Builds/iOS)

# iOS Simulator specific settings
set(CMAKE_XCODE_ATTRIBUTE_SDKROOT iphonesimulator)
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "12.0")
set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH NO)
set(CMAKE_XCODE_ATTRIBUTE_VALID_ARCHS "${CMAKE_OSX_ARCHITECTURES}")

message(STATUS "iOS Simulator toolchain configured for architecture: ${CMAKE_OSX_ARCHITECTURES}")