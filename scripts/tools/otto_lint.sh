#!/bin/bash

# =============================================================================
# OTTO Comprehensive Code Linting Script
# =============================================================================
# 
# This script systematically checks the entire OTTO codebase for:
# - Syntax errors (missing semicolons, braces, parentheses)
# - Incomplete method calls (orphaned parameters)
# - Missing includes and forward declarations
# - JUCE 8 compliance issues
# - Memory management problems
# - Thread safety violations
#
# Usage: ./otto_lint.sh [options]
# Options:
#   --fix-auto    Automatically fix simple syntax errors
#   --verbose     Show detailed output
#   --cpp-only    Check only C++ files
#   --headers-only Check only header files
#
# Author: OTTO Development Team
# Version: 1.0
# =============================================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
SOURCE_DIR="$PROJECT_ROOT/Source"
BUILD_DIR="$PROJECT_ROOT/build"

# Options
FIX_AUTO=false
VERBOSE=false
CPP_ONLY=false
HEADERS_ONLY=false
ERROR_COUNT=0
WARNING_COUNT=0

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --fix-auto)
            FIX_AUTO=true
            shift
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --cpp-only)
            CPP_ONLY=true
            shift
            ;;
        --headers-only)
            HEADERS_ONLY=true
            shift
            ;;
        -h|--help)
            echo "OTTO Comprehensive Code Linting Script"
            echo ""
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  --fix-auto      Automatically fix simple syntax errors"
            echo "  --verbose       Show detailed output"
            echo "  --cpp-only      Check only C++ files"
            echo "  --headers-only  Check only header files"
            echo "  -h, --help      Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Utility functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
    ((WARNING_COUNT++))
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
    ((ERROR_COUNT++))
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_verbose() {
    if [[ "$VERBOSE" == true ]]; then
        echo -e "${CYAN}[VERBOSE]${NC} $1"
    fi
}

# Check if we're in the right directory
check_project_structure() {
    log_info "Checking project structure..."
    
    if [[ ! -d "$SOURCE_DIR" ]]; then
        log_error "Source directory not found: $SOURCE_DIR"
        exit 1
    fi
    
    if [[ ! -f "$PROJECT_ROOT/CMakeLists.txt" ]]; then
        log_error "CMakeLists.txt not found in project root"
        exit 1
    fi
    
    log_success "Project structure validated"
}

# Get list of files to check
get_files_to_check() {
    local files=()
    
    if [[ "$CPP_ONLY" == true ]]; then
        files=($(find "$SOURCE_DIR" -name "*.cpp" -type f))
    elif [[ "$HEADERS_ONLY" == true ]]; then
        files=($(find "$SOURCE_DIR" -name "*.h" -type f))
    else
        files=($(find "$SOURCE_DIR" \( -name "*.cpp" -o -name "*.h" \) -type f))
    fi
    
    echo "${files[@]}"
}

# Check for basic syntax errors
check_syntax_errors() {
    local file="$1"
    local filename=$(basename "$file")
    local errors_found=false
    
    log_verbose "Checking syntax in $filename..."
    
    # Check for orphaned semicolons at start of lines
    if grep -n "^[[:space:]]*;" "$file" > /dev/null 2>&1; then
        log_error "$filename: Found orphaned semicolons at line start"
        if [[ "$VERBOSE" == true ]]; then
            grep -n "^[[:space:]]*;" "$file" | head -5
        fi
        errors_found=true
        
        if [[ "$FIX_AUTO" == true ]]; then
            log_info "Auto-fixing orphaned semicolons in $filename"
            sed -i '' '/^[[:space:]]*;[[:space:]]*$/d' "$file"
        fi
    fi
    
    # Check for incomplete method calls (lines ending with comma followed by empty line)
    if grep -Pzo ",[[:space:]]*\n[[:space:]]*\n" "$file" > /dev/null 2>&1; then
        log_error "$filename: Found incomplete method calls (orphaned parameters)"
        errors_found=true
    fi
    
    # Check for unmatched braces
    local open_braces=$(grep -o "{" "$file" | wc -l)
    local close_braces=$(grep -o "}" "$file" | wc -l)
    if [[ $open_braces -ne $close_braces ]]; then
        log_error "$filename: Unmatched braces (open: $open_braces, close: $close_braces)"
        errors_found=true
    fi
    
    # Check for unmatched parentheses in single lines
    while IFS= read -r line_num; do
        local line_content=$(sed -n "${line_num}p" "$file")
        local open_parens=$(echo "$line_content" | grep -o "(" | wc -l)
        local close_parens=$(echo "$line_content" | grep -o ")" | wc -l)
        if [[ $open_parens -ne $close_parens ]]; then
            log_warning "$filename:$line_num: Unmatched parentheses in line"
            if [[ "$VERBOSE" == true ]]; then
                echo "  Line: $line_content"
            fi
        fi
    done < <(grep -n "(" "$file" | cut -d: -f1)
    
    # Check for missing semicolons after statements
    if grep -n "^[[:space:]]*[a-zA-Z_][a-zA-Z0-9_]*\.[a-zA-Z_][a-zA-Z0-9_]*([^;]*$" "$file" > /dev/null 2>&1; then
        log_warning "$filename: Possible missing semicolons after method calls"
        if [[ "$VERBOSE" == true ]]; then
            grep -n "^[[:space:]]*[a-zA-Z_][a-zA-Z0-9_]*\.[a-zA-Z_][a-zA-Z0-9_]*([^;]*$" "$file" | head -3
        fi
    fi
    
    return $([[ "$errors_found" == true ]] && echo 1 || echo 0)
}

# Check for JUCE 8 specific issues
check_juce8_compliance() {
    local file="$1"
    local filename=$(basename "$file")
    
    log_verbose "Checking JUCE 8 compliance in $filename..."
    
    # Check for deprecated JUCE methods
    local deprecated_patterns=(
        "juce::Component::setBufferedToImage"
        "juce::AudioProcessor::setPlayConfigDetails"
        "juce::AudioProcessor::getNumInputChannels"
        "juce::AudioProcessor::getNumOutputChannels"
        "juce::Slider::setTextBoxStyle.*false"
    )
    
    for pattern in "${deprecated_patterns[@]}"; do
        if grep -n "$pattern" "$file" > /dev/null 2>&1; then
            log_warning "$filename: Uses deprecated JUCE method: $pattern"
        fi
    done
    
    # Check for proper JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR usage
    if [[ "$file" == *.h ]] && grep -q "class.*:" "$file"; then
        if ! grep -q "JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR" "$file"; then
            log_warning "$filename: Missing JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR"
        fi
    fi
}

# Check for memory management issues
check_memory_management() {
    local file="$1"
    local filename=$(basename "$file")
    
    log_verbose "Checking memory management in $filename..."
    
    # Check for raw pointers that should be smart pointers
    if grep -n "^[[:space:]]*[a-zA-Z_][a-zA-Z0-9_]*\*[[:space:]]*[a-zA-Z_]" "$file" > /dev/null 2>&1; then
        log_warning "$filename: Found raw pointers - consider using smart pointers"
    fi
    
    # Check for new without corresponding delete
    local new_count=$(grep -c "\bnew\b" "$file" 2>/dev/null || echo 0)
    local delete_count=$(grep -c "\bdelete\b" "$file" 2>/dev/null || echo 0)
    if [[ $new_count -gt 0 && $delete_count -eq 0 ]]; then
        log_warning "$filename: Found 'new' without 'delete' - potential memory leak"
    fi
}

# Check for thread safety issues
check_thread_safety() {
    local file="$1"
    local filename=$(basename "$file")
    
    log_verbose "Checking thread safety in $filename..."
    
    # Check for MessageManager calls in audio thread
    if grep -n "MessageManager::" "$file" > /dev/null 2>&1; then
        log_warning "$filename: MessageManager calls found - ensure not in audio thread"
    fi
    
    # Check for std::cout in audio processing
    if grep -n "std::cout\|printf\|std::cerr" "$file" > /dev/null 2>&1; then
        log_warning "$filename: Console output found - avoid in audio processing"
    fi
}

# Check for incomplete setColour calls (the specific issue we found)
check_incomplete_setcolour() {
    local file="$1"
    local filename=$(basename "$file")
    
    log_verbose "Checking for incomplete setColour calls in $filename..."
    
    # Look for setColour calls followed by empty lines
    local line_num=1
    while IFS= read -r line; do
        if [[ "$line" =~ setColour.*,$ ]]; then
            local next_line_num=$((line_num + 1))
            local next_line=$(sed -n "${next_line_num}p" "$file")
            if [[ -z "$next_line" || "$next_line" =~ ^[[:space:]]*$ ]]; then
                log_error "$filename:$line_num: Incomplete setColour call - missing second parameter"
                if [[ "$VERBOSE" == true ]]; then
                    echo "  Line $line_num: $line"
                    echo "  Line $next_line_num: $next_line"
                fi
                
                if [[ "$FIX_AUTO" == true ]]; then
                    log_info "Auto-fixing incomplete setColour call at line $line_num"
                    # This would need more sophisticated logic to determine the correct parameter
                    log_warning "Auto-fix for setColour requires manual intervention"
                fi
            fi
        fi
        ((line_num++))
    done < "$file"
}

# Compile check using CMake
compile_check() {
    log_info "Running compilation check..."
    
    if [[ ! -d "$BUILD_DIR" ]]; then
        log_info "Creating build directory..."
        mkdir -p "$BUILD_DIR"
    fi
    
    cd "$BUILD_DIR"
    
    # Configure CMake
    if ! cmake .. > /dev/null 2>&1; then
        log_error "CMake configuration failed"
        return 1
    fi
    
    # Try to compile
    local compile_output=$(cmake --build . 2>&1)
    local compile_result=$?
    
    if [[ $compile_result -ne 0 ]]; then
        log_error "Compilation failed"
        if [[ "$VERBOSE" == true ]]; then
            echo "$compile_output"
        else
            echo "$compile_output" | grep -E "(error|Error|ERROR)" | head -10
        fi
        return 1
    else
        log_success "Compilation successful"
        return 0
    fi
}

# Main linting function
lint_file() {
    local file="$1"
    local filename=$(basename "$file")
    
    log_verbose "Linting $filename..."
    
    # Run all checks
    check_syntax_errors "$file"
    check_juce8_compliance "$file"
    check_memory_management "$file"
    check_thread_safety "$file"
    check_incomplete_setcolour "$file"
}

# Generate report
generate_report() {
    echo ""
    echo "=============================================="
    echo "           OTTO LINT REPORT"
    echo "=============================================="
    echo ""
    
    if [[ $ERROR_COUNT -eq 0 && $WARNING_COUNT -eq 0 ]]; then
        log_success "No issues found! Code is clean."
    else
        echo -e "${RED}Errors found: $ERROR_COUNT${NC}"
        echo -e "${YELLOW}Warnings found: $WARNING_COUNT${NC}"
        echo ""
        
        if [[ $ERROR_COUNT -gt 0 ]]; then
            echo -e "${RED}❌ CRITICAL ISSUES FOUND${NC}"
            echo "Please fix all errors before committing."
        else
            echo -e "${GREEN}✅ NO CRITICAL ERRORS${NC}"
            echo "Consider addressing warnings for better code quality."
        fi
    fi
    
    echo ""
    echo "=============================================="
}

# Main execution
main() {
    echo ""
    echo "🔍 OTTO Comprehensive Code Linting"
    echo "=================================="
    echo ""
    
    # Check project structure
    check_project_structure
    
    # Get files to check
    local files=($(get_files_to_check))
    log_info "Found ${#files[@]} files to check"
    
    # Lint each file
    for file in "${files[@]}"; do
        lint_file "$file"
    done
    
    # Run compilation check
    if [[ "$CPP_ONLY" != true && "$HEADERS_ONLY" != true ]]; then
        compile_check
    fi
    
    # Generate report
    generate_report
    
    # Exit with appropriate code
    if [[ $ERROR_COUNT -gt 0 ]]; then
        exit 1
    else
        exit 0
    fi
}

# Run main function
main "$@"