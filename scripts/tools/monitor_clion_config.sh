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
