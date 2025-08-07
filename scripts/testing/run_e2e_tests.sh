#!/bin/bash

#==============================================================================
# OTTO E2E Test Runner Script
# Comprehensive End-to-End Testing Automation
#==============================================================================

set -e  # Exit on any error

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
E2E_TEST_DIR="$PROJECT_ROOT/e2e-test"
BUILD_DIR="$E2E_TEST_DIR/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Default configuration
BUILD_TYPE="Release"
RUN_ALL_TESTS=false
RUN_UI_WORKFLOW=false
RUN_COMPONENT_INTEGRATION=false
RUN_USER_INTERACTION=false
RUN_STATE_MANAGEMENT=false
VERBOSE=false
GENERATE_REPORT=false
REPORT_PATH=""
GUI_MODE=false
CLEAN_BUILD=false
PARALLEL_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

#==============================================================================
# Helper Functions
#==============================================================================

print_header() {
    echo -e "${CYAN}"
    echo "=============================================================================="
    echo "  OTTO E2E Test Runner"
    echo "  Comprehensive End-to-End Testing Suite"
    echo "=============================================================================="
    echo -e "${NC}"
}

print_usage() {
    echo -e "${YELLOW}Usage: $0 [OPTIONS]${NC}"
    echo ""
    echo "Test Selection Options:"
    echo "  --all                      Run all E2E test suites"
    echo "  --ui-workflow              Run UI workflow tests only"
    echo "  --component-integration    Run component integration tests only"
    echo "  --user-interaction         Run user interaction tests only"
    echo "  --state-management         Run state management tests only"
    echo ""
    echo "Execution Options:"
    echo "  --gui                      Launch GUI test runner"
    echo "  --verbose, -v              Enable verbose output"
    echo "  --report [path]            Generate XML test report (optional path)"
    echo "  --build-type [type]        Build type: Release (default) or Debug"
    echo "  --clean                    Clean build before running tests"
    echo "  --jobs [n]                 Number of parallel build jobs (default: $PARALLEL_JOBS)"
    echo ""
    echo "Utility Options:"
    echo "  --help, -h                 Show this help message"
    echo "  --version                  Show version information"
    echo ""
    echo "Examples:"
    echo "  $0 --all --verbose                    # Run all tests with verbose output"
    echo "  $0 --ui-workflow --report results.xml # Run UI tests and generate report"
    echo "  $0 --gui                              # Launch GUI test runner"
    echo "  $0 --clean --all --build-type Debug   # Clean build and run all tests in debug"
    echo ""
}

print_version() {
    echo -e "${BLUE}OTTO E2E Test Runner v1.0.0${NC}"
    echo "Built for JUCE 8.0.8+ with CMake 3.22+"
    echo "Supports: macOS, Windows, Linux, iOS, Android"
    echo ""
}

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_step() {
    echo -e "${PURPLE}[STEP]${NC} $1"
}

#==============================================================================
# Validation Functions
#==============================================================================

check_prerequisites() {
    log_step "Checking prerequisites..."
    
    # Check if we're in the right directory
    if [[ ! -f "$PROJECT_ROOT/CMakeLists.txt" ]]; then
        log_error "Not in OTTO project root directory"
        log_error "Expected to find CMakeLists.txt at: $PROJECT_ROOT"
        exit 1
    fi
    
    # Check if E2E test directory exists
    if [[ ! -d "$E2E_TEST_DIR" ]]; then
        log_error "E2E test directory not found: $E2E_TEST_DIR"
        exit 1
    fi
    
    # Check for required files
    local required_files=(
        "$E2E_TEST_DIR/CMakeLists.txt"
        "$E2E_TEST_DIR/E2ETestMain.cpp"
        "$E2E_TEST_DIR/E2ETestRunner.h"
    )
    
    for file in "${required_files[@]}"; do
        if [[ ! -f "$file" ]]; then
            log_error "Required file not found: $file"
            exit 1
        fi
    done
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        log_error "CMake not found. Please install CMake 3.22 or higher."
        exit 1
    fi
    
    # Check CMake version
    local cmake_version=$(cmake --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
    local required_version="3.22.0"
    
    if [[ "$(printf '%s\n' "$required_version" "$cmake_version" | sort -V | head -n1)" != "$required_version" ]]; then
        log_error "CMake version $cmake_version is too old. Required: $required_version or higher."
        exit 1
    fi
    
    # Check for build tools
    if [[ "$OSTYPE" == "darwin"* ]]; then
        if ! command -v xcodebuild &> /dev/null; then
            log_warning "Xcode command line tools not found. Some features may not work."
        fi
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if ! command -v make &> /dev/null && ! command -v ninja &> /dev/null; then
            log_error "No build system found. Please install make or ninja."
            exit 1
        fi
    fi
    
    log_success "Prerequisites check passed"
}

check_juce_version() {
    log_step "Checking JUCE version..."
    
    # Look for JUCE version in CMake cache or find_package
    local juce_version=""
    
    if [[ -f "$BUILD_DIR/CMakeCache.txt" ]]; then
        juce_version=$(grep "JUCE_VERSION" "$BUILD_DIR/CMakeCache.txt" 2>/dev/null | head -n1 | cut -d'=' -f2 || echo "")
    fi
    
    if [[ -z "$juce_version" ]]; then
        log_warning "Could not determine JUCE version. Proceeding with build..."
    else
        log_info "JUCE version: $juce_version"
        
        # Check if version is 8.0.8 or higher
        local required_version="8.0.8"
        if [[ "$(printf '%s\n' "$required_version" "$juce_version" | sort -V | head -n1)" != "$required_version" ]]; then
            log_error "JUCE version $juce_version is too old. Required: $required_version or higher."
            exit 1
        fi
    fi
}

#==============================================================================
# Build Functions
#==============================================================================

clean_build_directory() {
    if [[ "$CLEAN_BUILD" == true ]]; then
        log_step "Cleaning build directory..."
        
        if [[ -d "$BUILD_DIR" ]]; then
            rm -rf "$BUILD_DIR"
            log_success "Build directory cleaned"
        else
            log_info "Build directory doesn't exist, nothing to clean"
        fi
    fi
}

configure_cmake() {
    log_step "Configuring CMake..."
    
    cd "$E2E_TEST_DIR"
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    
    # CMake configuration arguments
    local cmake_args=(
        "-B" "$BUILD_DIR"
        "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    )
    
    # Platform-specific configuration
    if [[ "$OSTYPE" == "darwin"* ]]; then
        cmake_args+=("-DCMAKE_OSX_DEPLOYMENT_TARGET=10.15")
        
        # Use Xcode generator for better IDE integration
        if command -v xcodebuild &> /dev/null; then
            cmake_args+=("-G" "Xcode")
        fi
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Prefer Ninja if available
        if command -v ninja &> /dev/null; then
            cmake_args+=("-G" "Ninja")
        fi
    fi
    
    # Run CMake configuration
    if cmake "${cmake_args[@]}"; then
        log_success "CMake configuration completed"
    else
        log_error "CMake configuration failed"
        exit 1
    fi
    
    # Check JUCE version after configuration
    check_juce_version
}

build_e2e_tests() {
    log_step "Building E2E test application..."
    
    cd "$E2E_TEST_DIR"
    
    # Build arguments
    local build_args=(
        "--build" "$BUILD_DIR"
        "--target" "OTTO_E2E_Tests"
        "--config" "$BUILD_TYPE"
    )
    
    # Add parallel jobs if supported
    if [[ "$PARALLEL_JOBS" -gt 1 ]]; then
        build_args+=("--parallel" "$PARALLEL_JOBS")
    fi
    
    # Run build
    if cmake "${build_args[@]}"; then
        log_success "E2E test application built successfully"
    else
        log_error "Build failed"
        exit 1
    fi
    
    # Verify executable exists
    local executable_path
    if [[ "$OSTYPE" == "darwin"* ]] && [[ -d "$BUILD_DIR/Release/OTTO E2E Tests.app" ]]; then
        executable_path="$BUILD_DIR/Release/OTTO E2E Tests.app/Contents/MacOS/OTTO E2E Tests"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        executable_path="$BUILD_DIR/Release/OTTO_E2E_Tests.exe"
    else
        executable_path="$BUILD_DIR/OTTO_E2E_Tests"
    fi
    
    if [[ ! -f "$executable_path" ]] && [[ ! -d "$BUILD_DIR/Release/OTTO E2E Tests.app" ]]; then
        log_error "E2E test executable not found at expected location"
        log_error "Expected: $executable_path"
        exit 1
    fi
    
    log_success "E2E test executable verified"
}

#==============================================================================
# Test Execution Functions
#==============================================================================

get_executable_path() {
    if [[ "$OSTYPE" == "darwin"* ]] && [[ -d "$BUILD_DIR/Release/OTTO E2E Tests.app" ]]; then
        echo "$BUILD_DIR/Release/OTTO E2E Tests.app/Contents/MacOS/OTTO E2E Tests"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        echo "$BUILD_DIR/Release/OTTO_E2E_Tests.exe"
    else
        echo "$BUILD_DIR/OTTO_E2E_Tests"
    fi
}

run_e2e_tests() {
    log_step "Running E2E tests..."
    
    local executable_path=$(get_executable_path)
    
    # Build command line arguments
    local test_args=()
    
    if [[ "$GUI_MODE" == true ]]; then
        test_args+=("--gui")
    else
        # Add test selection arguments
        if [[ "$RUN_ALL_TESTS" == true ]]; then
            test_args+=("--all")
        else
            if [[ "$RUN_UI_WORKFLOW" == true ]]; then
                test_args+=("--ui-workflow")
            fi
            if [[ "$RUN_COMPONENT_INTEGRATION" == true ]]; then
                test_args+=("--component-integration")
            fi
            if [[ "$RUN_USER_INTERACTION" == true ]]; then
                test_args+=("--user-interaction")
            fi
            if [[ "$RUN_STATE_MANAGEMENT" == true ]]; then
                test_args+=("--state-management")
            fi
        fi
        
        # Add execution options
        if [[ "$VERBOSE" == true ]]; then
            test_args+=("--verbose")
        fi
        
        if [[ "$GENERATE_REPORT" == true ]]; then
            test_args+=("--report")
            if [[ -n "$REPORT_PATH" ]]; then
                test_args+=("$REPORT_PATH")
            fi
        fi
    fi
    
    # Run the tests
    log_info "Executing: \"$executable_path\" ${test_args[*]}"
    echo ""
    
    if "$executable_path" "${test_args[@]}"; then
        echo ""
        log_success "E2E tests completed successfully"
        
        # Show report location if generated
        if [[ "$GENERATE_REPORT" == true ]] && [[ ! "$GUI_MODE" == true ]]; then
            local report_file="$E2E_TEST_DIR/e2e_test_results.xml"
            if [[ -n "$REPORT_PATH" ]]; then
                report_file="$REPORT_PATH"
            fi
            
            if [[ -f "$report_file" ]]; then
                log_info "Test report generated: $report_file"
            fi
        fi
    else
        echo ""
        log_error "E2E tests failed"
        exit 1
    fi
}

#==============================================================================
# Main Execution
#==============================================================================

parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --all)
                RUN_ALL_TESTS=true
                shift
                ;;
            --ui-workflow)
                RUN_UI_WORKFLOW=true
                shift
                ;;
            --component-integration)
                RUN_COMPONENT_INTEGRATION=true
                shift
                ;;
            --user-interaction)
                RUN_USER_INTERACTION=true
                shift
                ;;
            --state-management)
                RUN_STATE_MANAGEMENT=true
                shift
                ;;
            --gui)
                GUI_MODE=true
                shift
                ;;
            --verbose|-v)
                VERBOSE=true
                shift
                ;;
            --report)
                GENERATE_REPORT=true
                if [[ $# -gt 1 ]] && [[ ! "$2" =~ ^-- ]]; then
                    REPORT_PATH="$2"
                    shift
                fi
                shift
                ;;
            --build-type)
                if [[ $# -gt 1 ]]; then
                    BUILD_TYPE="$2"
                    shift
                else
                    log_error "--build-type requires an argument"
                    exit 1
                fi
                shift
                ;;
            --clean)
                CLEAN_BUILD=true
                shift
                ;;
            --jobs)
                if [[ $# -gt 1 ]] && [[ "$2" =~ ^[0-9]+$ ]]; then
                    PARALLEL_JOBS="$2"
                    shift
                else
                    log_error "--jobs requires a numeric argument"
                    exit 1
                fi
                shift
                ;;
            --help|-h)
                print_usage
                exit 0
                ;;
            --version)
                print_version
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                print_usage
                exit 1
                ;;
        esac
    done
    
    # If no specific tests selected and not GUI mode, default to all tests
    if [[ "$GUI_MODE" == false ]] && [[ "$RUN_ALL_TESTS" == false ]] && \
       [[ "$RUN_UI_WORKFLOW" == false ]] && [[ "$RUN_COMPONENT_INTEGRATION" == false ]] && \
       [[ "$RUN_USER_INTERACTION" == false ]] && [[ "$RUN_STATE_MANAGEMENT" == false ]]; then
        RUN_ALL_TESTS=true
    fi
}

main() {
    print_header
    
    # Parse command line arguments
    parse_arguments "$@"
    
    # Show configuration
    log_info "Configuration:"
    log_info "  Project Root: $PROJECT_ROOT"
    log_info "  E2E Test Dir: $E2E_TEST_DIR"
    log_info "  Build Type: $BUILD_TYPE"
    log_info "  Parallel Jobs: $PARALLEL_JOBS"
    log_info "  GUI Mode: $GUI_MODE"
    log_info "  Verbose: $VERBOSE"
    log_info "  Generate Report: $GENERATE_REPORT"
    if [[ -n "$REPORT_PATH" ]]; then
        log_info "  Report Path: $REPORT_PATH"
    fi
    echo ""
    
    # Execute build and test pipeline
    check_prerequisites
    clean_build_directory
    configure_cmake
    build_e2e_tests
    run_e2e_tests
    
    echo ""
    log_success "OTTO E2E Test Runner completed successfully!"
}

# Execute main function with all arguments
main "$@"