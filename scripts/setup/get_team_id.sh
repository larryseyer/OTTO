#!/bin/bash

# Script to help find your Apple Developer Team ID
echo "🔍 Finding your Apple Developer Team ID..."
echo ""

# Method 1: Check Xcode preferences
echo "Method 1: Checking Xcode developer accounts..."
XCODE_ACCOUNTS_PLIST="$HOME/Library/Developer/Xcode/UserData/IDEDeveloperAccounts.plist"
if [ -f "$XCODE_ACCOUNTS_PLIST" ]; then
    echo "✅ Found Xcode accounts file"
    
    # Try to extract team IDs using plutil (safer than defaults)
    TEAM_IDS=$(plutil -extract 'IDEDeveloperAccounts' raw "$XCODE_ACCOUNTS_PLIST" 2>/dev/null | grep -o '"teamID":"[^"]*"' | cut -d'"' -f4 | sort -u)
    
    if [ -n "$TEAM_IDS" ]; then
        echo "🎯 Found Team IDs in Xcode:"
        echo "$TEAM_IDS" | while read -r team_id; do
            echo "   → $team_id"
        done
    else
        echo "⚠️  No team IDs found in Xcode accounts"
    fi
else
    echo "❌ Xcode accounts file not found"
fi

echo ""

# Method 2: Check keychain for development certificates
echo "Method 2: Checking keychain for iOS development certificates..."
DEV_CERTS=$(security find-certificate -a -c "iPhone Developer" -p | openssl x509 -text -noout 2>/dev/null | grep -A1 "Subject:" | grep -o "OU=[A-Z0-9]*" | cut -d'=' -f2 | sort -u)

if [ -n "$DEV_CERTS" ]; then
    echo "🎯 Found Team IDs in development certificates:"
    echo "$DEV_CERTS" | while read -r team_id; do
        echo "   → $team_id"
    done
else
    echo "❌ No iOS development certificates found in keychain"
fi

echo ""

# Method 3: Manual instructions
echo "Method 3: Manual lookup instructions:"
echo "1. Open Xcode"
echo "2. Go to Xcode → Settings → Accounts"
echo "3. Select your Apple ID"
echo "4. Your Team ID will be shown next to your team name"
echo ""
echo "Or visit: https://developer.apple.com/account/#!/membership"
echo ""

# Check if user has any Apple ID certificates at all
APPLE_CERTS=$(security find-certificate -a | grep -c "Apple")
if [ "$APPLE_CERTS" -gt 0 ]; then
    echo "✅ Found $APPLE_CERTS Apple certificates in keychain"
else
    echo "❌ No Apple certificates found - you may need to:"
    echo "   1. Sign in to Xcode with your Apple ID"
    echo "   2. Download development certificates"
    echo "   3. Or sign up for Apple Developer Program"
fi

echo ""
echo "💡 Once you have your Team ID, use it like this:"
echo "   export APPLE_TEAM_ID='YOUR_TEAM_ID_HERE'"
echo "   ./build_ios.sh --device --team-id \$APPLE_TEAM_ID"