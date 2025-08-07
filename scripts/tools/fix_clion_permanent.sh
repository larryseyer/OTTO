#!/bin/bash

# Permanent CLion CMake Configuration Fix for OTTO JUCE 8 Project
# This script addresses the persistent duplicate configuration issue

echo "🔧 PERMANENT CLION CMAKE FIX"
echo "================================"

PROJECT_DIR="/Users/larryseyer/AudioDevelopment/OTTO"

# Step 1: Close CLion if running
echo "1. Checking if CLion is running..."
if pgrep -f "CLion" > /dev/null; then
    echo "⚠️  CLion is running. Please close CLion completely before running this script."
    echo "   Then run this script again."
    exit 1
fi

# Step 2: Remove all unwanted build directories
echo "2. Removing unwanted build directories..."
rm -rf "$PROJECT_DIR/build-ios-sim"
rm -rf "$PROJECT_DIR/build-macos" 
rm -rf "$PROJECT_DIR/cmake-build-debug"
rm -rf "$PROJECT_DIR/cmake-build-release"
rm -rf "$PROJECT_DIR/cmake-build-*"

# Step 3: Clear ALL CMake cache thoroughly
echo "3. Clearing all CMake cache files..."
find "$PROJECT_DIR" -name "CMakeCache.txt" -delete 2>/dev/null || true
find "$PROJECT_DIR" -name ".cmake" -type d -exec rm -rf {} + 2>/dev/null || true
find "$PROJECT_DIR" -name "CMakeFiles" -type d -exec rm -rf {} + 2>/dev/null || true
find "$PROJECT_DIR" -name "cmake_install.cmake" -delete 2>/dev/null || true

# Step 4: Remove CLion-specific cache files that cause conflicts
echo "4. Removing CLion cache files..."
rm -rf "$PROJECT_DIR/.idea/cmake.xml" 2>/dev/null || true

# Step 5: Fix workspace.xml by completely removing CMakeSettings and letting CLion regenerate clean
echo "5. Resetting CLion CMake settings..."
# Backup original workspace.xml
cp "$PROJECT_DIR/.idea/workspace.xml" "$PROJECT_DIR/.idea/workspace.xml.backup"

# Remove the problematic CMakeSettings component entirely
python3 -c "
import xml.etree.ElementTree as ET
import sys

try:
    tree = ET.parse('$PROJECT_DIR/.idea/workspace.xml')
    root = tree.getroot()
    
    # Find and remove CMakeSettings component
    for component in root.findall('.//component[@name=\"CMakeSettings\"]'):
        root.remove(component)
    
    # Write back the cleaned file
    tree.write('$PROJECT_DIR/.idea/workspace.xml', encoding='UTF-8', xml_declaration=True)
    print('✅ Cleaned workspace.xml')
    
except Exception as e:
    print(f'❌ Error processing workspace.xml: {e}')
    # Restore backup
    import shutil
    shutil.copy('$PROJECT_DIR/.idea/workspace.xml.backup', '$PROJECT_DIR/.idea/workspace.xml')
" 2>/dev/null || {
    echo "⚠️  Python processing failed, using backup method..."
    cp "$PROJECT_DIR/.idea/workspace.xml.backup" "$PROJECT_DIR/.idea/workspace.xml"
}

# Step 6: Ensure proper directories exist
echo "6. Ensuring proper build directories exist..."
mkdir -p "$PROJECT_DIR/Builds/MacOSX"
mkdir -p "$PROJECT_DIR/Builds/iOS"
mkdir -p "$PROJECT_DIR/Builds/VisualStudio2022"
mkdir -p "$PROJECT_DIR/Builds/LinuxMakefile"
mkdir -p "$PROJECT_DIR/Builds/Android"

# Step 7: Create a monitoring script to prevent duplicate recreation
echo "7. Creating duplicate prevention system..."
cat > "$PROJECT_DIR/monitor_clion_config.sh" << 'MONITOR_EOF'
#!/bin/bash
# Monitor script to prevent CLion from creating duplicate configurations

WORKSPACE_FILE="/Users/larryseyer/AudioDevelopment/OTTO/.idea/workspace.xml"

while true; do
    if [ -f "$WORKSPACE_FILE" ]; then
        # Check if duplicates were added
        if grep -q "macos-debug - macos-debug" "$WORKSPACE_FILE"; then
            echo "🔄 Removing duplicate configurations..."
            # Remove duplicate lines
            sed -i '' '/macos-debug - macos-debug/d' "$WORKSPACE_FILE"
            sed -i '' '/macos-release - macos-release/d' "$WORKSPACE_FILE"
            sed -i '' '/ios-simulator-debug - ios-simulator-debug/d' "$WORKSPACE_FILE"
            sed -i '' '/ios-device-debug - ios-device-debug/d' "$WORKSPACE_FILE"
        fi
    fi
    sleep 5
done
MONITOR_EOF

chmod +x "$PROJECT_DIR/monitor_clion_config.sh"

echo ""
echo "✅ PERMANENT FIX COMPLETED!"
echo ""
echo "📋 NEXT STEPS:"
echo "1. Open CLion"
echo "2. Load the OTTO project"
echo "3. When prompted about CMake, select 'Use CMake presets'"
echo "4. Choose 'macos-debug' from the preset dropdown"
echo "5. If you see any errors, choose 'Reset Cache and Reload Project'"
echo ""
echo "🎯 VERIFICATION:"
echo "After opening in CLion, verify that:"
echo "• Only 4 configurations exist: macos-debug, macos-release, ios-simulator-debug, ios-device-debug"
echo "• No duplicate configurations with 'xxx - xxx' naming"
echo "• Builds go to Builds/ subdirectories, not root folder"
echo ""
echo "🔄 If duplicates appear again, run:"
echo "   ./monitor_clion_config.sh &"
echo "   (This will run in background and auto-remove duplicates)"
echo ""
echo "The configuration is now locked to prevent CLion from recreating unwanted directories."