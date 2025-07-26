#!/bin/bash

# Script to reset CLion CMake configuration and prevent unwanted build directories
# Run this if CLion continues to create build-ios-sim, build-macos, or cmake-build-debug

echo "Resetting CLion CMake configuration for OTTO project..."

PROJECT_DIR="/Users/larryseyer/AudioDevelopment/OTTO"

# Remove any unwanted build directories that CLion might create
echo "Removing unwanted build directories..."
rm -rf "$PROJECT_DIR/build-ios-sim"
rm -rf "$PROJECT_DIR/build-macos" 
rm -rf "$PROJECT_DIR/cmake-build-debug"
rm -rf "$PROJECT_DIR/cmake-build-release"
rm -rf "$PROJECT_DIR/cmake-build-*"

# Clean CMake cache files throughout the project
echo "Cleaning CMake cache files..."
find "$PROJECT_DIR" -name "CMakeCache.txt" -delete
find "$PROJECT_DIR" -name "CMakeFiles" -type d -exec rm -rf {} + 2>/dev/null || true
find "$PROJECT_DIR" -name "cmake_install.cmake" -delete
find "$PROJECT_DIR" -name "Makefile" -not -path "*/JUCE/*" -delete

# Clear any CLion-specific cache
echo "Clearing CLion caches..."
rm -rf "$PROJECT_DIR/.idea/cmake.xml" 2>/dev/null || true

# Ensure proper directories exist
echo "Ensuring proper build directories exist..."
mkdir -p "$PROJECT_DIR/Builds/MacOSX"
mkdir -p "$PROJECT_DIR/Builds/iOS"
mkdir -p "$PROJECT_DIR/Builds/VisualStudio2022"
mkdir -p "$PROJECT_DIR/Builds/LinuxMakefile"
mkdir -p "$PROJECT_DIR/Builds/Android"

echo "Reset complete!"
echo ""
echo "Next steps:"
echo "1. Close CLion completely"
echo "2. Reopen CLion and load the project"
echo "3. When CLion asks to reload CMake, choose 'Reset Cache and Reload Project'"
echo "4. Select one of the preset configurations (macos-debug, macos-release, etc.)"
echo ""
echo "All builds should now go to the Builds/ directory structure."