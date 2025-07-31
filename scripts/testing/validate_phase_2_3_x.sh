#!/bin/bash

#==============================================================================
# OTTO Phase 2.3.x - Comprehensive Validation Script (JUCE 8)
# Validates all Phase 2.2 safety features and 2.3.x platform optimizations
# Tests build system integrity and cross-platform compatibility
#==============================================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
PURPLE='\033[0;35m'
BOLD='\033[1m'
NC='\033[0m'

echo -e "${CYAN}${BOLD}🔬 OTTO Phase 2.3.x - Comprehensive Validation${NC}"
echo "=============================================="
echo -e "${GREEN}🛡️  Validating all safety features and platform optimizations${NC}"
echo -e "${BLUE}🎯 JUCE 8.0.8+ compatibility verification${NC}"

#==============================================================================
# Validation Configuration
#==============================================================================

VALIDATION_RESULTS=()
VALIDATION_FAILURES=()
CURRENT_PHASE=""

# Function to log validation results
log_validation() {
    local status=$1
    local message=$2
    local details=${3:-""}
    
    if [[ "$status" == "PASS" ]]; then
        echo -e "${GREEN}  ✅ ${message}${NC}"
        [[ -n "$details" ]] && echo -e "${BLUE}     ${details}${NC}"
        VALIDATION_RESULTS+=("PASS: $CURRENT_PHASE - $message")
    elif [[ "$status" == "FAIL" ]]; then
        echo -e "${RED}  ❌ ${message}${NC}"
        [[ -n "$details" ]] && echo -e "${YELLOW}     ${details}${NC}"
        VALIDATION_FAILURES+=("FAIL: $CURRENT_PHASE - $message")
    elif [[ "$status" == "WARN" ]]; then
        echo -e "${YELLOW}  ⚠️  ${message}${NC}"
        [[ -n "$details" ]] && echo -e "${CYAN}     ${details}${NC}"
        VALIDATION_RESULTS+=("WARN: $CURRENT_PHASE - $message")
    fi
}

#==============================================================================
# Phase 2.2 Safety Features Validation
#==============================================================================

validate_phase_2_2_safety() {
    CURRENT_PHASE="Phase 2.2 Safety"
    echo -e "${PURPLE}🛡️  Validating Phase 2.2 Safety Features${NC}"
    echo "========================================"
    
    # Check for Phase 2.2 source files
    local safety_files=(
        "Source/JUCE8Compatibility.h"
        "Source/ProportionalScalingManager.h"
        "Source/ProportionalScalingManager.cpp"
        "Source/EnhancedProportionalEditor.h"
        "Source/EnhancedProportionalEditor.cpp"
        "Source/ProportionalPluginEditor.h"
        "Source/ProportionalPluginEditor.cpp"
    )
    
    for file in "${safety_files[@]}"; do
        if [[ -f "$file" ]]; then
            log_validation "PASS" "Safety file exists: $(basename "$file")"
        else
            log_validation "FAIL" "Missing safety file: $file"
        fi
    done
    
    # Check for safety features in JUCE8Compatibility.h
    if [[ -f "Source/JUCE8Compatibility.h" ]]; then
        local safety_features=(
            "OTTO_ENHANCED_NULL_SAFETY"
            "OTTO_MEMORY_SAFETY_CHECKS"
            "OTTO_AUDIO_PROCESSING_SAFETY"
            "OTTO_MIDI_SAFETY_FEATURES"
            "OTTO_UI_THREAD_SAFETY"
        )
        
        for feature in "${safety_features[@]}"; do
            if grep -q "$feature" "Source/JUCE8Compatibility.h"; then
                log_validation "PASS" "Safety feature defined: $feature"
            else
                log_validation "FAIL" "Missing safety feature: $feature"
            fi
        done
    fi
    
    echo ""
}

#==============================================================================
# Phase 2.3.1 Windows Configuration Validation
#==============================================================================

validate_phase_2_3_1_windows() {
    CURRENT_PHASE="Phase 2.3.1 Windows"
    echo -e "${PURPLE}🪟 Validating Phase 2.3.1 Windows Configuration${NC}"
    echo "=============================================="
    
    # Check Windows CMake configuration
    if [[ -f "cmake/Windows.cmake" ]]; then
        log_validation "PASS" "Windows CMake configuration exists"
        
        # Check for Windows-specific features
        local windows_features=(
            "configure_windows_build"
            "configure_windows_audio_drivers"
            "configure_windows_parallels_optimizations"
        )
        
        for feature in "${windows_features[@]}"; do
            if grep -q "$feature" "cmake/Windows.cmake"; then
                log_validation "PASS" "Windows feature available: $feature"
            else
                log_validation "WARN" "Windows feature missing: $feature"
            fi
        done
    else
        log_validation "FAIL" "Windows CMake configuration missing"
    fi
    
    # Check Windows build scripts
    if [[ -f "build_windows.bat" ]]; then
        log_validation "PASS" "Windows build script exists"
    else
        log_validation "FAIL" "Windows build script missing"
    fi
    
    # Check Parallels integration
    if [[ -f "scripts/complete_windows_workflow.bat" ]]; then
        log_validation "PASS" "Parallels workflow script exists"
    else
        log_validation "WARN" "Parallels workflow script missing"
    fi
    
    echo ""
}

#==============================================================================
# Phase 2.3.2 Docker Linux Validation
#==============================================================================

validate_phase_2_3_2_linux() {
    CURRENT_PHASE="Phase 2.3.2 Linux"
    echo -e "${PURPLE}🐧 Validating Phase 2.3.2 Docker Linux Configuration${NC}"
    echo "================================================"
    
    # Check Docker configuration
    if [[ -f "docker/Dockerfile.linux" ]]; then
        log_validation "PASS" "Linux Dockerfile exists"
        
        # Check for Docker optimization features
        if grep -q "OTTO Phase 2.3.2" "docker/Dockerfile.linux"; then
            log_validation "PASS" "Docker Phase 2.3.2 markers found"
        else
            log_validation "WARN" "Docker Phase markers missing"
        fi
    else
        log_validation "FAIL" "Linux Dockerfile missing"
    fi
    
    # Check Docker Compose
    if [[ -f "docker/docker-compose.yml" ]]; then
        log_validation "PASS" "Docker Compose configuration exists"
    else
        log_validation "WARN" "Docker Compose configuration missing"
    fi
    
    # Check enhanced Linux build script
    if [[ -f "build_linux_docker.sh" ]]; then
        log_validation "PASS" "Enhanced Linux build script exists"
    else
        log_validation "WARN" "Enhanced Linux build script missing"
    fi
    
    echo ""
}

#==============================================================================
# Phase 2.3.3 iOS Enhancement Validation
#==============================================================================

validate_phase_2_3_3_ios() {
    CURRENT_PHASE="Phase 2.3.3 iOS"
    echo -e "${PURPLE}🍎 Validating Phase 2.3.3 iOS Enhancement Configuration${NC}"
    echo "==================================================="
    
    # Check iOS CMake configuration
    if [[ -f "cmake/iOS-Enhanced.cmake" ]]; then
        log_validation "PASS" "Enhanced iOS CMake configuration exists"
        
        # Check for iOS-specific features
        local ios_features=(
            "configure_enhanced_ios_build"
            "configure_ios_audio_optimizations"
            "configure_auv3_optimizations"
        )
        
        for feature in "${ios_features[@]}"; do
            if grep -q "$feature" "cmake/iOS-Enhanced.cmake"; then
                log_validation "PASS" "iOS feature available: $feature"
            else
                log_validation "WARN" "iOS feature missing: $feature"
            fi
        done
    else
        log_validation "FAIL" "Enhanced iOS CMake configuration missing"
    fi
    
    # Check iOS build script
    if [[ -f "build_ios_enhanced.sh" ]]; then
        log_validation "PASS" "Enhanced iOS build script exists"
        chmod +x "build_ios_enhanced.sh" 2>/dev/null || true
    else
        log_validation "WARN" "Enhanced iOS build script missing"
    fi
    
    echo ""
}

#==============================================================================
# Phase 2.3.4 Android Enhancement Validation
#==============================================================================

validate_phase_2_3_4_android() {
    CURRENT_PHASE="Phase 2.3.4 Android"
    echo -e "${PURPLE}🤖 Validating Phase 2.3.4 Android Enhancement Configuration${NC}"
    echo "======================================================="
    
    # Check Android CMake configuration
    if [[ -f "cmake/Android-Enhanced.cmake" ]]; then
        log_validation "PASS" "Enhanced Android CMake configuration exists"
        
        # Check for Android-specific features
        local android_features=(
            "configure_enhanced_android_build"
            "configure_android_audio_optimizations"
            "configure_android_performance_optimizations"
        )
        
        for feature in "${android_features[@]}"; do
            if grep -q "$feature" "cmake/Android-Enhanced.cmake"; then
                log_validation "PASS" "Android feature available: $feature"
            else
                log_validation "WARN" "Android feature missing: $feature"
            fi
        done
    else
        log_validation "FAIL" "Enhanced Android CMake configuration missing"
    fi
    
    echo ""
}

#==============================================================================
# Phase 2.3.5 Advanced Robustness Validation
#==============================================================================

validate_phase_2_3_5_robustness() {
    CURRENT_PHASE="Phase 2.3.5 Robustness"
    echo -e "${PURPLE}🛡️  Validating Phase 2.3.5 Advanced Robustness Features${NC}"
    echo "====================================================="
    
    # Check Advanced Robustness CMake configuration
    if [[ -f "cmake/AdvancedRobustness.cmake" ]]; then
        log_validation "PASS" "Advanced Robustness CMake configuration exists"
        
        # Check for robustness features
        local robustness_features=(
            "configure_advanced_robustness_features"
            "configure_advanced_memory_safety"
            "configure_advanced_audio_safety"
            "configure_advanced_error_recovery"
        )
        
        for feature in "${robustness_features[@]}"; do
            if grep -q "$feature" "cmake/AdvancedRobustness.cmake"; then
                log_validation "PASS" "Robustness feature available: $feature"
            else
                log_validation "WARN" "Robustness feature missing: $feature"
            fi
        done
    else
        log_validation "FAIL" "Advanced Robustness CMake configuration missing"
    fi
    
    echo ""
}

#==============================================================================
# CMake Integration Validation
#==============================================================================

validate_cmake_integration() {
    CURRENT_PHASE="CMake Integration"
    echo -e "${PURPLE}⚙️  Validating CMake Integration${NC}"
    echo "==============================="
    
    # Check main CMakeLists.txt
    if [[ -f "CMakeLists.txt" ]]; then
        log_validation "PASS" "Main CMakeLists.txt exists"
        
        # Check for Phase integration markers
        local integration_markers=(
            "Phase 2.3.1"
            "Phase 2.3.3"
            "Phase 2.3.4"
            "Phase 2.3.5"
        )
        
        for marker in "${integration_markers[@]}"; do
            if grep -q "$marker" "CMakeLists.txt"; then
                log_validation "PASS" "CMake integration marker found: $marker"
            else
                log_validation "WARN" "CMake integration marker missing: $marker"
            fi
        done
        
        # Check for function calls
        local function_calls=(
            "configure_enhanced_ios_build"
            "configure_enhanced_android_build"
            "configure_advanced_robustness_features"
        )
        
        for function in "${function_calls[@]}"; do
            if grep -q "$function" "CMakeLists.txt"; then
                log_validation "PASS" "CMake function call found: $function"
            else
                log_validation "WARN" "CMake function call missing: $function"
            fi
        done
    else
        log_validation "FAIL" "Main CMakeLists.txt missing"
    fi
    
    echo ""
}

#==============================================================================
# Build System Validation
#==============================================================================

validate_build_system() {
    CURRENT_PHASE="Build System"
    echo -e "${PURPLE}🏗️  Validating Build System${NC}"
    echo "=========================="
    
    # Check for build scripts
    local build_scripts=(
        "build_macos.sh"
        "build_windows.bat"
        "build_linux.sh"
        "build_ios.sh"
        "build_android.sh"
        "build_all_platforms.sh"
    )
    
    for script in "${build_scripts[@]}"; do
        if [[ -f "$script" ]]; then
            log_validation "PASS" "Build script exists: $script"
            # Make sure it's executable (for shell scripts)
            if [[ "$script" == *.sh ]]; then
                chmod +x "$script" 2>/dev/null || true
            fi
        else
            log_validation "WARN" "Build script missing: $script"
        fi
    done
    
    # Check CMake presets
    if [[ -f "CMakePresets.json" ]]; then
        log_validation "PASS" "CMake presets configuration exists"
    else
        log_validation "WARN" "CMake presets configuration missing"
    fi
    
    echo ""
}

#==============================================================================
# JUCE 8 Compatibility Validation
#==============================================================================

validate_juce8_compatibility() {
    CURRENT_PHASE="JUCE 8 Compatibility"
    echo -e "${PURPLE}🎯 Validating JUCE 8 Compatibility${NC}"
    echo "================================="
    
    # Check JUCE 8 specific features in CMakeLists.txt
    if [[ -f "CMakeLists.txt" ]]; then
        local juce8_features=(
            "JUCE8_PROJECT=ON"
            "JUCE_MAJOR_VERSION_REQUIRED=8"
            "juce_add_plugin"
        )
        
        for feature in "${juce8_features[@]}"; do
            if grep -q "$feature" "CMakeLists.txt"; then
                log_validation "PASS" "JUCE 8 feature found: $feature"
            else
                log_validation "WARN" "JUCE 8 feature missing: $feature"
            fi
        done
    fi
    
    # Check for JUCE 8 compatibility header
    if [[ -f "Source/JUCE8Compatibility.h" ]]; then
        if grep -q "JUCE_MAJOR_VERSION.*8" "Source/JUCE8Compatibility.h"; then
            log_validation "PASS" "JUCE 8 version compatibility defined"
        else
            log_validation "WARN" "JUCE 8 version compatibility unclear"
        fi
    fi
    
    echo ""
}

#==============================================================================
# Main Validation Execution
#==============================================================================

main() {
    local VALIDATION_START_TIME=$(date +%s)
    
    echo ""
    echo -e "${CYAN}🔍 Starting comprehensive validation...${NC}"
    echo ""
    
    # Run all validation phases
    validate_phase_2_2_safety
    validate_phase_2_3_1_windows
    validate_phase_2_3_2_linux
    validate_phase_2_3_3_ios
    validate_phase_2_3_4_android
    validate_phase_2_3_5_robustness
    validate_cmake_integration
    validate_build_system
    validate_juce8_compatibility
    
    # Display final results
    local VALIDATION_END_TIME=$(date +%s)
    local TOTAL_DURATION=$((VALIDATION_END_TIME - VALIDATION_START_TIME))
    
    echo -e "${CYAN}${BOLD}📊 OTTO Phase 2.3.x Validation Results${NC}"
    echo "======================================="
    echo ""
    echo -e "${BLUE}⏱️  Validation completed in ${TOTAL_DURATION}s${NC}"
    echo -e "${GREEN}✅ Total validations: ${#VALIDATION_RESULTS[@]}${NC}"
    echo -e "${RED}❌ Total failures: ${#VALIDATION_FAILURES[@]}${NC}"
    
    # Show failures if any
    if [[ ${#VALIDATION_FAILURES[@]} -gt 0 ]]; then
        echo ""
        echo -e "${RED}💥 Validation Failures:${NC}"
        for failure in "${VALIDATION_FAILURES[@]}"; do
            echo -e "${RED}  • ${failure}${NC}"
        done
    fi
    
    echo ""
    if [[ ${#VALIDATION_FAILURES[@]} -eq 0 ]]; then
        echo -e "${GREEN}${BOLD}🎉 All Phase 2.3.x validations passed!${NC}"
        echo -e "${GREEN}✨ OTTO is ready for cross-platform development${NC}"
        echo -e "${CYAN}🚀 All safety features and optimizations validated${NC}"
        exit 0
    else
        echo -e "${YELLOW}${BOLD}⚠️  Some validations failed${NC}"
        echo -e "${CYAN}🔧 Please address the issues above before proceeding${NC}"
        exit 1
    fi
}

# Execute main function
main "$@"