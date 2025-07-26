# iOS Device CMake Toolchain for JUCE 8

set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_SYSTEM_VERSION 12.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0)

# iOS Device is always arm64
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_OSX_SYSROOT "iphoneos")

message(STATUS "Building for iOS Device (arm64)")

# iOS Device specific settings
set(CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2") # iPhone and iPad
set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH NO)
set(CMAKE_XCODE_ATTRIBUTE_VALID_ARCHS "arm64")

# JUCE 8 specific iOS settings
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "12.0")
set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE NO)
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")

# Compiler and linker flags
set(CMAKE_CXX_FLAGS_INIT "-stdlib=libc++")
set(CMAKE_C_FLAGS_INIT "")

# Print configuration summary
message(STATUS "iOS Device Configuration:")
message(STATUS "  Target Architecture: ${CMAKE_OSX_ARCHITECTURES}")
message(STATUS "  SDK Root: ${CMAKE_OSX_SYSROOT}")
message(STATUS "  Deployment Target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")