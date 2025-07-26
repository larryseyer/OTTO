# iOS Simulator CMake Toolchain for JUCE 8
# Automatically detects Apple Silicon vs Intel Mac architecture

set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_SYSTEM_VERSION 12.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0)

# Detect host architecture
execute_process(
    COMMAND uname -m
    OUTPUT_VARIABLE HOST_ARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set simulator architecture based on host
if(HOST_ARCH STREQUAL "arm64")
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    message(STATUS "Building for iOS Simulator on Apple Silicon Mac (arm64)")
else()
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
    message(STATUS "Building for iOS Simulator on Intel Mac (x86_64)")
endif()

# Set iOS Simulator SDK
set(CMAKE_OSX_SYSROOT "iphonesimulator")

# Find and set the simulator SDK path for DYLD_ROOT_PATH
execute_process(
    COMMAND xcrun --show-sdk-path --sdk iphonesimulator
    OUTPUT_VARIABLE IOS_SIMULATOR_SDK_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(IOS_SIMULATOR_SDK_PATH)
    message(STATUS "iOS Simulator SDK Path: ${IOS_SIMULATOR_SDK_PATH}")
    set(CMAKE_XCODE_ATTRIBUTE_DYLD_ROOT_PATH "${IOS_SIMULATOR_SDK_PATH}")
endif()

# iOS Simulator specific settings
set(CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2") # iPhone and iPad
set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH NO)
set(CMAKE_XCODE_ATTRIBUTE_VALID_ARCHS "${CMAKE_OSX_ARCHITECTURES}")

# JUCE 8 specific iOS settings
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "12.0")
set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE NO)
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")

# Set environment variables for runtime
set(CMAKE_XCODE_ATTRIBUTE_DYLD_FRAMEWORK_PATH "${IOS_SIMULATOR_SDK_PATH}/System/Library/Frameworks")

# Compiler and linker flags
set(CMAKE_CXX_FLAGS_INIT "-stdlib=libc++")
set(CMAKE_C_FLAGS_INIT "")

# Print configuration summary
message(STATUS "iOS Simulator Configuration:")
message(STATUS "  Host Architecture: ${HOST_ARCH}")
message(STATUS "  Target Architecture: ${CMAKE_OSX_ARCHITECTURES}")
message(STATUS "  SDK Root: ${CMAKE_OSX_SYSROOT}")
message(STATUS "  Deployment Target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")