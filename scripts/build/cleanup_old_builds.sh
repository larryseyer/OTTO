#!/bin/bash

# OTTO Build Cleanup Script
# Safely removes old scattered build directories and files after consolidation

echo "OTTO Build Directory Cleanup"
echo "============================"
echo ""

# Function to print colored output
print_status() {
    echo -e "\033[1;32m$1\033[0m"
}

print_warning() {
    echo -e "\033[1;33m$1\033[0m"
}

print_error() {
    echo -e "\033[1;31m$1\033[0m"
}

# Safety check - ensure we're in the right directory
if [ ! -f "CMakeLists.txt" ] || [ ! -d "Builds" ] || [ ! -d "Source" ]; then
    print_error "Error: This script must be run from the OTTO project root directory"
    print_error "Expected files/directories: CMakeLists.txt, Builds/, Source/"
    exit 1
fi

print_status "Checking for successful build consolidation..."

# Verify new build structure exists and has content
NEW_BUILDS_EXIST=false
if [ -d "Builds/MacOSX/Release" ] && [ "$(ls -A Builds/MacOSX/Release 2>/dev/null)" ]; then
    print_status "✓ Found macOS builds in Builds/MacOSX/"
    NEW_BUILDS_EXIST=true
fi

if [ -d "Builds/iOS/Release" ] && [ "$(ls -A Builds/iOS/Release 2>/dev/null)" ]; then
    print_status "✓ Found iOS builds in Builds/iOS/"
    NEW_BUILDS_EXIST=true
fi

if [ "$NEW_BUILDS_EXIST" = false ]; then
    print_error "No consolidated builds found in Builds/ directory!"
    print_error "Please run a successful build first before cleanup."
    echo ""
    echo "Try: ./scripts/build/build_macos.sh or ./scripts/build/build_all.sh --macos"
    exit 1
fi

echo ""
print_warning "The following old build directories will be removed:"
echo ""

# List old build directories that will be removed
OLD_DIRS=()
for dir in "build-macos" "build-ios" "build-ios-device" "build-ios-sim" "build-ios-simulator" "build-linux" "build-windows" "build-android" "cmake-build-debug" "cmake-build-release"; do
    if [ -d "$dir" ]; then
        echo "  📁 $dir"
        OLD_DIRS+=("$dir")
    fi
done

# List old build files that will be removed
OLD_FILES=()
for file in "test_simple.cpp"; do
    if [ -f "$file" ]; then
        echo "  📄 $file"
        OLD_FILES+=("$file")
    fi
done

if [ ${#OLD_DIRS[@]} -eq 0 ] && [ ${#OLD_FILES[@]} -eq 0 ]; then
    print_status "✅ No old build directories found - cleanup not needed!"
    exit 0
fi

echo ""
print_warning "This will permanently delete the above directories and files."
print_warning "Make sure your builds work correctly from the Builds/ directory first!"
echo ""

# Interactive confirmation
read -p "Continue with cleanup? (y/N): " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    print_status "Cleanup cancelled."
    exit 0
fi

echo ""
print_status "Starting cleanup..."

# Remove old directories
for dir in "${OLD_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        print_status "Removing directory: $dir"
        rm -rf "$dir"
        if [ $? -eq 0 ]; then
            print_status "  ✓ Removed $dir"
        else
            print_error "  ✗ Failed to remove $dir"
        fi
    fi
done

# Remove old files
for file in "${OLD_FILES[@]}"; do
    if [ -f "$file" ]; then
        print_status "Removing file: $file"
        rm -f "$file"
        if [ $? -eq 0 ]; then
            print_status "  ✓ Removed $file"
        else
            print_error "  ✗ Failed to remove $file"
        fi
    fi
done

echo ""
print_status "✅ Cleanup complete!"
echo ""
print_status "📁 Organized build structure:"
echo ""
echo "Current build directories in Builds/:"
find Builds/ -name "Release" -type d 2>/dev/null | sort
echo ""
echo "Build artifacts by platform:"
[ -d "Builds/MacOSX/Release" ] && echo "  📁 macOS: Builds/MacOSX/Release/ ($(find Builds/MacOSX/Release -name "*.app" -o -name "*.vst3" -o -name "*.component" | wc -l | tr -d ' ') artifacts)"
[ -d "Builds/iOS/Release" ] && echo "  📁 iOS: Builds/iOS/Release/ ($(find Builds/iOS/Release -name "*.app" -o -name "*.appex" | wc -l | tr -d ' ') artifacts)"
[ -d "Builds/Linux/Release" ] && echo "  📁 Linux: Builds/Linux/Release/ ($(find Builds/Linux/Release -name "*.so" -o -name "OTTO" | wc -l | tr -d ' ') artifacts)"
[ -d "Builds/Android/Release" ] && echo "  📁 Android: Builds/Android/Release/ ($(find Builds/Android/Release -name "*.so" -o -name "*.apk" | wc -l | tr -d ' ') artifacts)"
[ -d "Builds/VisualStudio2022/Release" ] && echo "  📁 Windows: Builds/VisualStudio2022/Release/ ($(find Builds/VisualStudio2022/Release -name "*.dll" -o -name "*.exe" -o -name "*.vst3" | wc -l | tr -d ' ') artifacts)"

echo ""
print_status "🎉 OTTO build directories are now properly organized!"
print_status "All future builds will automatically use the Builds/ folder structure."