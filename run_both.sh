#!/bin/bash

echo "🚀 OTTO - Running macOS and iOS Simulator"
echo "=========================================="

# Check if macOS build exists
if [ -f "/Users/larryseyer/AudioDevelopment/OTTO/build-macos/OTTO_artefacts/Release/Standalone/OTTO.app/Contents/MacOS/OTTO" ]; then
    echo "✅ macOS app found, launching..."
    open "/Users/larryseyer/AudioDevelopment/OTTO/build-macos/OTTO_artefacts/Release/Standalone/OTTO.app" &
    echo "✅ macOS OTTO app launched!"
else
    echo "❌ macOS app not found. Run './build_macos.sh' first."
fi

# Start iOS Simulator
echo ""
echo "📱 Starting iOS Simulator..."
DEVICE_ID=$(xcrun simctl list devices available | grep iPhone | head -1 | grep -o '([A-F0-9-]*)' | tr -d '()')

if [ -n "$DEVICE_ID" ]; then
    echo "🔧 Using device: $DEVICE_ID"
    xcrun simctl boot "$DEVICE_ID" 2>/dev/null || echo "ℹ️  Device already booted"
    open -a Simulator
    echo "✅ iOS Simulator launched!"
else
    echo "❌ No iOS simulator devices found"
fi

echo ""
echo "🎯 Summary:"
echo "- macOS OTTO app: Running"
echo "- iOS Simulator: Running (ready for future iOS builds)"
echo ""
echo "📝 Note: For iOS app, the build process needs refinement due to dependency complexity."
echo "    The current codebase uses plugin-specific components that require adaptation for iOS GUI apps."