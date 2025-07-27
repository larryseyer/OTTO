#!/bin/bash

#==============================================================================
# OTTO Phase 2.3.2 - Enhanced Linux Build Script (JUCE 8)
# Docker-based build system with native fallback
# Maintains all Phase 2.2 safety features while adding Linux-specific optimizations
#==============================================================================

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${CYAN}🐧 Building OTTO for Linux - Phase 2.3.2...${NC}"
echo "============================================="
echo -e "${GREEN}🛡️  Enhanced robustness and safety features enabled${NC}"
echo -e "${BLUE}🐳 Docker-optimized build system${NC}"
echo -e "${GREEN}🎯 JUCE 8 native Linux integration${NC}"

#==============================================================================
# Configuration and argument parsing
#==============================================================================

BUILD_TYPE="Release"
USE_DOCKER=true
ARCHITECTURE="x86_64"
CLEAN_BUILD=false
RUN_TESTS=false

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
        --native)
            USE_DOCKER=false
            shift
            ;;
        --docker)
            USE_DOCKER=true
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
        --test)
            RUN_TESTS=true
            shift
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "OPTIONS:"
            echo "  --debug          Build in Debug mode (default: Release)"
            echo "  --release        Build in Release mode"
            echo "  --native         Use native build (no Docker)"
            echo "  --docker         Use Docker build (default)"
            echo "  --arm64          Build for ARM64 architecture"
            echo "  --x86_64         Build for x86_64 architecture (default)"
            echo "  --clean          Clean previous builds"
            echo "  --test           Run tests after build"
            echo "  --help           Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}❌ Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

echo -e "${BLUE}🔧 Configuration:${NC}"
echo -e "${GREEN}  Build Type: ${BUILD_TYPE}${NC}"
echo -e "${GREEN}  Architecture: ${ARCHITECTURE}${NC}"
echo -e "${GREEN}  Use Docker: ${USE_DOCKER}${NC}"
echo -e "${GREEN}  Clean Build: ${CLEAN_BUILD}${NC}"
echo -e "${GREEN}  Run Tests: ${RUN_TESTS}${NC}"

#==============================================================================
# Environment validation
#==============================================================================

validate_environment() {
    echo -e "${CYAN}🔍 Validating build environment...${NC}"
    
    # Check for required tools
    if $USE_DOCKER; then
        if ! command -v docker &> /dev/null; then
            echo -e "${RED}❌ Docker not found! Please install Docker or use --native${NC}"
            exit 1
        fi
        
        if ! command -v docker-compose &> /dev/null; then
            echo -e "${RED}❌ Docker Compose not found! Please install Docker Compose${NC}"
            exit 1
        fi
        
        echo -e "${GREEN}  ✅ Docker environment validated${NC}"
    else
        # Native build validation
        if ! command -v cmake &> /dev/null; then
            echo -e "${RED}❌ CMake not found! Please install CMake${NC}"
            exit 1
        fi
        
        if ! command -v gcc &> /dev/null && ! command -v clang &> /dev/null; then
            echo -e "${RED}❌ No C++ compiler found! Please install GCC or Clang${NC}"
            exit 1
        fi
        
        # Check for essential Linux development packages
        if ! pkg-config --exists alsa; then
            echo -e "${YELLOW}⚠️  ALSA development libraries not found${NC}"
            echo -e "${CYAN}💡 Install with: sudo apt-get install libasound2-dev${NC}"
        fi
        
        echo -e "${GREEN}  ✅ Native build environment validated${NC}"
    fi
    
    # Check JUCE installation
    if [ -d "JUCE" ]; then
        echo -e "${GREEN}  ✅ JUCE found in local directory${NC}"
    else
        echo -e "${YELLOW}  ⚠️  JUCE not found locally - will be handled by build system${NC}"
    fi
}

#==============================================================================
# Clean build function
#==============================================================================

clean_builds() {
    if $CLEAN_BUILD; then
        echo -e "${CYAN}🧹 Cleaning previous builds...${NC}"
        
        if $USE_DOCKER; then
            # Clean Docker builds
            rm -rf "Builds/LinuxDocker"
            rm -rf "Builds/LinuxDocker-ARM64"
            
            # Clean Docker containers and images
            docker-compose -f docker/docker-compose.yml down --rmi all --volumes --remove-orphans 2>/dev/null || true
            
            echo -e "${GREEN}  ✅ Docker builds cleaned${NC}"
        else
            # Clean native builds
            rm -rf "Builds/LinuxMakefile"
            rm -rf "Builds/LinuxNinja"
            
            echo -e "${GREEN}  ✅ Native builds cleaned${NC}"
        fi
    fi
}

#==============================================================================
# Docker build function
#==============================================================================

build_with_docker() {
    echo -e "${CYAN}🐳 Building OTTO with Docker...${NC}"
    
    # Create cache directories
    mkdir -p .docker-cache/juce-x86_64
    mkdir -p .docker-cache/juce-arm64
    
    # Create output directories
    if [ "$ARCHITECTURE" == "arm64" ]; then
        mkdir -p "Builds/LinuxDocker-ARM64"
        OUTPUT_DIR="Builds/LinuxDocker-ARM64"
        SERVICE_NAME="otto-linux-arm64"
    else
        mkdir -p "Builds/LinuxDocker"
        OUTPUT_DIR="Builds/LinuxDocker"
        SERVICE_NAME="otto-linux-build"
    fi
    
    echo -e "${BLUE}📁 Output directory: ${OUTPUT_DIR}${NC}"
    
    # Set environment variables for Docker Compose
    export BUILD_TYPE
    export ARCHITECTURE
    
    # Build and run the appropriate service
    echo -e "${CYAN}🏗️  Starting Docker build process...${NC}"
    
    if docker-compose -f docker/docker-compose.yml build ${SERVICE_NAME}; then
        echo -e "${GREEN}  ✅ Docker image built successfully${NC}"
    else
        echo -e "${RED}  ❌ Docker image build failed${NC}"
        exit 1
    fi
    
    # Run the build
    if docker-compose -f docker/docker-compose.yml run --rm ${SERVICE_NAME}; then
        echo -e "${GREEN}✅ Docker build completed successfully${NC}"
    else
        echo -e "${RED}❌ Docker build failed${NC}"
        exit 1
    fi
    
    # Run tests if requested
    if $RUN_TESTS; then
        echo -e "${CYAN}🧪 Running Docker tests...${NC}"
        if docker-compose -f docker/docker-compose.yml run --rm otto-linux-test; then
            echo -e "${GREEN}✅ Docker tests passed${NC}"
        else
            echo -e "${YELLOW}⚠️  Some Docker tests failed${NC}"
        fi
    fi
    
    return 0
}

#==============================================================================
# Main execution
#==============================================================================

main() {
    # Validate environment
    validate_environment
    
    # Clean if requested
    clean_builds
    
    # Execute build
    if $USE_DOCKER; then
        build_with_docker
    else
        echo -e "${YELLOW}⚠️  Native build not implemented in this version${NC}"
        echo -e "${CYAN}💡 Use the original build_linux.sh for native builds${NC}"
        exit 1
    fi
    
    # Display results
    echo ""
    echo -e "${GREEN}✨ OTTO Phase 2.3.2 Linux Build Complete!${NC}"
    echo "==========================================="
    echo ""
    echo -e "${CYAN}📋 Build Summary:${NC}"
    echo -e "${GREEN}  ✅ Platform: Linux${NC}"
    echo -e "${GREEN}  ✅ Architecture: ${ARCHITECTURE}${NC}"
    echo -e "${GREEN}  ✅ Build Type: ${BUILD_TYPE}${NC}"
    echo -e "${GREEN}  ✅ Method: $(if $USE_DOCKER; then echo 'Docker'; else echo 'Native'; fi)${NC}"
    
    # Find and list build artifacts
    echo ""
    echo -e "${CYAN}📦 Build Artifacts:${NC}"
    
    if [ "$ARCHITECTURE" == "arm64" ]; then
        SEARCH_DIR="Builds/LinuxDocker-ARM64"
    else
        SEARCH_DIR="Builds/LinuxDocker"
    fi
    
    if [ -d "$SEARCH_DIR" ]; then
        find "$SEARCH_DIR" -name "*.so" -o -name "*.vst3" -o -name "*.clap" -o -name "OTTO" -type f 2>/dev/null | while read file; do
            echo -e "${GREEN}  📄 ${file}${NC}"
            ls -la "$file" | awk '{print "      Size: " $5 " bytes, Modified: " $6 " " $7 " " $8}'
        done
    else
        echo -e "${YELLOW}  ⚠️  Build directory not found: ${SEARCH_DIR}${NC}"
    fi
    
    echo ""
    echo -e "${CYAN}💡 Next Steps:${NC}"
    echo -e "${GREEN}  1. Test plugins in your favorite Linux DAW${NC}"
    echo -e "${GREEN}  2. Install plugins to system directories if needed${NC}"
    echo -e "${GREEN}  3. Report any issues for Phase 2.3.3${NC}"
    echo ""
    echo -e "${GREEN}🎵 Happy Linux audio development!${NC}"
}

# Execute main function
main "$@"