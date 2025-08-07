#!/bin/bash

#==============================================================================
# OTTO Phase 2.3.3 - Enhanced iOS Build Script (JUCE 8)
# Advanced iOS build system with simulator and device support
# Maintains all Phase 2.2 safety features while adding iOS-specific optimizations
#==============================================================================

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

echo -e "${CYAN}🍎 Building OTTO for iOS - Phase 2.3.3...${NC}"
echo "==========================================="
echo -e "${GREEN}🛡️  Enhanced robustness and safety features enabled${NC}"
echo -e "${BLUE}📱 Advanced iOS and AUv3 optimizations${NC}"
echo -e "${GREEN}🎯 JUCE 8 native iOS integration${NC}"

#==============================================================================
# Configuration and argument parsing
#==============================================================================

BUILD_TYPE="Release"
TARGET_PLATFORM="simulator"  # simulator or device
ARCHITECTURE=""  # auto-detect if not specified
CLEAN_BUILD=false
ARCHIVE_BUILD=false
CODESIGN_IDENTITY=""
DEVELOPMENT_TEAM=""

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --simulator)
            TARGET_PLATFORM="simulator"
            shift
            ;;
        --device)
            TARGET_PLATFORM="device"
            shift
            ;;
        --arm64)
            ARCHITECTURE="arm64"
            shift
            ;;
        --x86_64)
            ARCHITECTURE="x86_64"
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --archive)
            ARCHIVE_BUILD=true
            TARGET_PLATFORM="device"  # Archive requires device build
            shift
            ;;
        --codesign)
            CODESIGN_IDENTITY="$2"
            shift 2
            ;;
        --team)
            DEVELOPMENT_TEAM="$2"
            shift 2
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "OPTIONS:"
            echo "  --debug          Build in Debug mode (default: Release)"
            echo "  --release        Build in Release mode"
            echo "  --simulator      Build for iOS Simulator (default)"
            echo "  --device         Build for iOS Device"
            echo "  --arm64          Force ARM64 architecture"
            echo "  --x86_64         Force x86_64 architecture (simulator only)"
            echo "  --clean          Clean previous builds"
            echo "  --archive        Create archive for App Store (implies --device)"
            echo "  --codesign ID    Code signing identity"
            echo "  --team TEAM      Development team ID"
            echo "  --help           Show this help message"
            echo ""
            echo "ENVIRONMENT VARIABLES:"
            echo "  APPLE_DEVELOPMENT_TEAM    Development team ID"
            echo "  IOS_CODE_SIGN_IDENTITY    Code signing identity"
            exit 0
            ;;
        *)
            echo -e "${RED}❌ Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Use environment variables if not provided
if [[ -z "$DEVELOPMENT_TEAM" && -n "$APPLE_DEVELOPMENT_TEAM" ]]; then
    DEVELOPMENT_TEAM="$APPLE_DEVELOPMENT_TEAM"
fi

if [[ -z "$CODESIGN_IDENTITY" && -n "$IOS_CODE_SIGN_IDENTITY" ]]; then
    CODESIGN_IDENTITY="$IOS_CODE_SIGN_IDENTITY"
fi

echo -e "${BLUE}🔧 Configuration:${NC}"
echo -e "${GREEN}  Build Type: ${BUILD_TYPE}${NC}"
echo -e "${GREEN}  Target Platform: ${TARGET_PLATFORM}${NC}"
echo -e "${GREEN}  Architecture: ${ARCHITECTURE:-auto-detect}${NC}"
echo -e "${GREEN}  Clean Build: ${CLEAN_BUILD}${NC}"
echo -e "${GREEN}  Archive Build: ${ARCHIVE_BUILD}${NC}"
echo -e "${GREEN}  Development Team: ${DEVELOPMENT_TEAM:-not set}${NC}"

#==============================================================================
# Environment validation
#==============================================================================

validate_ios_environment() {
    echo -e "${CYAN}🔍 Validating iOS build environment...${NC}"
    
    # Check for Xcode
    if ! command -v xcodebuild &> /dev/null; then
        echo -e "${RED}❌ Xcode not found! Please install Xcode from the App Store${NC}"
        exit 1
    fi
    
    # Check Xcode version
    XCODE_VERSION=$(xcodebuild -version | head -n1 | sed 's/Xcode //')
    echo -e "${GREEN}  ✅ Xcode version: ${XCODE_VERSION}${NC}"
    
    # Check for iOS SDKs
    if $TARGET_PLATFORM == "simulator"; then
        if ! xcodebuild -showsdks | grep -q "iphonesimulator"; then
            echo -e "${RED}❌ iOS Simulator SDK not found!${NC}"
            exit 1
        fi
        echo -e "${GREEN}  ✅ iOS Simulator SDK available${NC}"
    else
        if ! xcodebuild -showsdks | grep -q "iphoneos"; then
            echo -e "${RED}❌ iOS Device SDK not found!${NC}"
            exit 1
        fi
        echo -e "${GREEN}  ✅ iOS Device SDK available${NC}"
    fi
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        echo -e "${RED}❌ CMake not found! Please install CMake${NC}"
        exit 1
    fi
    
    CMAKE_VERSION=$(cmake --version | head -n1 | sed 's/cmake version //')
    echo -e "${GREEN}  ✅ CMake version: ${CMAKE_VERSION}${NC}"
    
    # Check JUCE installation
    if [ -d "JUCE" ]; then
        echo -e "${GREEN}  ✅ JUCE found in local directory${NC}"
    else
        echo -e "${YELLOW}  ⚠️  JUCE not found locally - will be handled by build system${NC}"
    fi
    
    # Validate development team for device builds
    if [[ "$TARGET_PLATFORM" == "device" && -z "$DEVELOPMENT_TEAM" ]]; then
        echo -e "${YELLOW}  ⚠️  Development team not set - may cause code signing issues${NC}"
        echo -e "${CYAN}  💡 Set APPLE_DEVELOPMENT_TEAM environment variable or use --team${NC}"
    fi
}

#==============================================================================
# Architecture detection
#==============================================================================

detect_architecture() {
    if [[ -n "$ARCHITECTURE" ]]; then
        echo -e "${GREEN}  Using specified architecture: ${ARCHITECTURE}${NC}"
        return
    fi
    
    if [[ "$TARGET_PLATFORM" == "simulator" ]]; then
        # Detect host architecture for simulator
        HOST_ARCH=$(uname -m)
        if [[ "$HOST_ARCH" == "arm64" ]]; then
            ARCHITECTURE="arm64"
            echo -e "${GREEN}  Auto-detected iOS Simulator architecture: arm64 (Apple Silicon)${NC}"
        else
            ARCHITECTURE="x86_64"
            echo -e "${GREEN}  Auto-detected iOS Simulator architecture: x86_64 (Intel)${NC}"
        fi
    else
        # iOS device is always arm64
        ARCHITECTURE="arm64"
        echo -e "${GREEN}  iOS Device architecture: arm64${NC}"
    fi
}

#==============================================================================
# Clean build function
#==============================================================================

clean_ios_builds() {
    if $CLEAN_BUILD; then
        echo -e "${CYAN}🧹 Cleaning previous iOS builds...${NC}"
        
        # Clean build directories
        rm -rf "Builds/iOS"
        
        # Clean Xcode derived data for OTTO
        if [ -d ~/Library/Developer/Xcode/DerivedData ]; then
            find ~/Library/Developer/Xcode/DerivedData -name "*OTTO*" -type d -exec rm -rf {} + 2>/dev/null || true
        fi
        
        echo -e "${GREEN}  ✅ iOS builds cleaned${NC}"
    fi
}

#==============================================================================
# CMake configuration function
#==============================================================================

configure_ios_cmake() {
    echo -e "${CYAN}⚙️  Configuring CMake for iOS...${NC}"
    
    # Determine build directory and CMake preset
    if [[ "$TARGET_PLATFORM" == "simulator" ]]; then
        if [[ "$BUILD_TYPE" == "Debug" ]]; then
            BUILD_DIR="Builds/iOS/CMake-Simulator-Debug"
            CMAKE_PRESET="ios-simulator-debug"
        else
            BUILD_DIR="Builds/iOS/CMake-Simulator-Release"
            # Create release simulator preset if it doesn't exist
            CMAKE_PRESET=""
        fi
        SDK_NAME="iphonesimulator"
    else
        if [[ "$BUILD_TYPE" == "Debug" ]]; then
            BUILD_DIR="Builds/iOS/CMake-Device-Debug"
            CMAKE_PRESET="ios-device-debug"
        else
            BUILD_DIR="Builds/iOS/CMake-Device-Release"
            # Create release device preset if it doesn't exist
            CMAKE_PRESET=""
        fi
        SDK_NAME="iphoneos"
    fi
    
    echo -e "${BLUE}📁 Build directory: ${BUILD_DIR}${NC}"
    
    # Create build directory
    mkdir -p "${BUILD_DIR}"
    
    # Configure CMake
    CMAKE_ARGS=(
        -B "${BUILD_DIR}"
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
        -DCMAKE_SYSTEM_NAME=iOS
        -DCMAKE_OSX_SYSROOT="${SDK_NAME}"
        -DCMAKE_OSX_ARCHITECTURES="${ARCHITECTURE}"
        -DCMAKE_OSX_DEPLOYMENT_TARGET="12.0"
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        -DJUCE8_PROJECT=ON
        -DJUCE_MAJOR_VERSION_REQUIRED=8
        -DOTTO_IOS_ENHANCED_BUILD=ON
        -DOTTO_ENABLE_ALL_SAFETY_FEATURES=ON
        -G "Xcode"
    )
    
    # Add development team if specified
    if [[ -n "$DEVELOPMENT_TEAM" ]]; then
        CMAKE_ARGS+=(
            -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="${DEVELOPMENT_TEAM}"
        )
    fi
    
    # Add code signing identity if specified
    if [[ -n "$CODESIGN_IDENTITY" ]]; then
        CMAKE_ARGS+=(
            -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="${CODESIGN_IDENTITY}"
        )
    fi
    
    echo -e "${CYAN}🔧 Running CMake configuration...${NC}"
    if cmake "${CMAKE_ARGS[@]}"; then
        echo -e "${GREEN}  ✅ CMake configuration successful${NC}"
    else
        echo -e "${RED}  ❌ CMake configuration failed${NC}"
        exit 1
    fi
}

#==============================================================================
# Build function
#==============================================================================

build_ios_project() {
    echo -e "${CYAN}🏗️  Building iOS project...${NC}"
    
    # Build with xcodebuild
    XCODEBUILD_ARGS=(
        -project "${BUILD_DIR}/OTTO.xcodeproj"
        -scheme "OTTO_Standalone"
        -configuration "${BUILD_TYPE}"
    )
    
    if [[ "$TARGET_PLATFORM" == "simulator" ]]; then
        XCODEBUILD_ARGS+=(
            -destination "platform=iOS Simulator,name=iPhone 14,OS=latest"
        )
    else
        XCODEBUILD_ARGS+=(
            -destination "generic/platform=iOS"
        )
    fi
    
    if $ARCHIVE_BUILD; then
        echo -e "${PURPLE}📦 Creating archive for App Store...${NC}"
        XCODEBUILD_ARGS+=(
            archive
            -archivePath "${BUILD_DIR}/OTTO.xcarchive"
        )
    else
        XCODEBUILD_ARGS+=(
            build
        )
    fi
    
    echo -e "${CYAN}🚀 Starting Xcode build...${NC}"
    if xcodebuild "${XCODEBUILD_ARGS[@]}"; then
        echo -e "${GREEN}✅ iOS build completed successfully${NC}"
    else
        echo -e "${RED}❌ iOS build failed${NC}"
        exit 1
    fi
}

#==============================================================================
# Validation function
#==============================================================================

validate_build_artifacts() {
    echo -e "${CYAN}🔍 Validating build artifacts...${NC}"
    
    # Look for build products
    if [[ "$TARGET_PLATFORM" == "simulator" ]]; then
        BUILD_PRODUCTS_DIR="${BUILD_DIR}/Build/Products/${BUILD_TYPE}-iphonesimulator"
    else
        BUILD_PRODUCTS_DIR="${BUILD_DIR}/Build/Products/${BUILD_TYPE}-iphoneos"
    fi
    
    if [ -d "$BUILD_PRODUCTS_DIR" ]; then
        echo -e "${GREEN}  ✅ Build products directory found${NC}"
        
        # List build artifacts
        find "$BUILD_PRODUCTS_DIR" -name "*.app" -o -name "*.appex" | while read artifact; do
            echo -e "${GREEN}    📱 $(basename "$artifact")${NC}"
            
            # Show basic info
            if [[ "$artifact" == *.app ]]; then
                INFO_PLIST="$artifact/Info.plist"
                if [ -f "$INFO_PLIST" ]; then
                    BUNDLE_ID=$(plutil -p "$INFO_PLIST" | grep CFBundleIdentifier | sed 's/.*=> "\(.*\)"/\1/')
                    VERSION=$(plutil -p "$INFO_PLIST" | grep CFBundleShortVersionString | sed 's/.*=> "\(.*\)"/\1/')
                    echo -e "${BLUE}      Bundle ID: ${BUNDLE_ID}${NC}"
                    echo -e "${BLUE}      Version: ${VERSION}${NC}"
                fi
            fi
        done
    else
        echo -e "${YELLOW}  ⚠️  Build products directory not found${NC}"
    fi
    
    # Check for archive if created
    if $ARCHIVE_BUILD; then
        ARCHIVE_PATH="${BUILD_DIR}/OTTO.xcarchive"
        if [ -d "$ARCHIVE_PATH" ]; then
            echo -e "${GREEN}  ✅ Archive created: $(basename "$ARCHIVE_PATH")${NC}"
            
            # Show archive info
            INFO_PLIST="$ARCHIVE_PATH/Info.plist"
            if [ -f "$INFO_PLIST" ]; then
                ARCHIVE_DATE=$(plutil -p "$INFO_PLIST" | grep CreationDate | sed 's/.*=> "\(.*\)"/\1/')
                echo -e "${BLUE}    Created: ${ARCHIVE_DATE}${NC}"
            fi
        else
            echo -e "${YELLOW}  ⚠️  Archive not found${NC}"
        fi
    fi
}

#==============================================================================
# Main execution
#==============================================================================

main() {
    # Validate environment
    validate_ios_environment
    
    # Detect architecture
    detect_architecture
    
    # Clean if requested
    clean_ios_builds
    
    # Configure CMake
    configure_ios_cmake
    
    # Build project
    build_ios_project
    
    # Validate results
    validate_build_artifacts
    
    # Display results
    echo ""
    echo -e "${GREEN}✨ OTTO Phase 2.3.3 iOS Build Complete!${NC}"
    echo "========================================"
    echo ""
    echo -e "${CYAN}📋 Build Summary:${NC}"
    echo -e "${GREEN}  ✅ Platform: iOS${NC}"
    echo -e "${GREEN}  ✅ Target: ${TARGET_PLATFORM}${NC}"
    echo -e "${GREEN}  ✅ Architecture: ${ARCHITECTURE}${NC}"
    echo -e "${GREEN}  ✅ Build Type: ${BUILD_TYPE}${NC}"
    if $ARCHIVE_BUILD; then
        echo -e "${GREEN}  ✅ Archive: Created${NC}"
    fi
    
    echo ""
    echo -e "${CYAN}💡 Next Steps:${NC}"
    if [[ "$TARGET_PLATFORM" == "simulator" ]]; then
        echo -e "${GREEN}  1. Test app in iOS Simulator${NC}"
        echo -e "${GREEN}  2. Use Instruments for performance profiling${NC}"
    else
        echo -e "${GREEN}  1. Test app on physical iOS device${NC}"
        if $ARCHIVE_BUILD; then
            echo -e "${GREEN}  2. Upload archive to App Store Connect${NC}"
            echo -e "${GREEN}  3. Submit for TestFlight or App Store review${NC}"
        else
            echo -e "${GREEN}  2. Create archive with --archive for distribution${NC}"
        fi
    fi
    echo -e "${GREEN}  3. Report any issues for Phase 2.3.4${NC}"
    echo ""
    echo -e "${GREEN}🍎 Happy iOS development!${NC}"
}

# Execute main function
main "$@"