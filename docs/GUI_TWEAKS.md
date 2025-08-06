# GUI_TWEAKS.md - Comprehensive Multi-Stage Prompt for OTTO GUI Fixes

## Overview
This document provides detailed, comprehensive multi-stage prompts to fix GUI issues across all 6 rows of the OTTO drum machine interface. Each section includes specific code analysis, file locations, and implementation guidance.

**Repository:** larryseyer/OTTO  
**Target Files:** Row1Component through Row6Component and supporting systems  
**Approach:** Row-by-row systematic fixes with detailed implementation steps

---

## Row 1: Top Bar Component Issues

### Current Implementation Analysis
- **File:** `Source/Row1Component.h` and `Source/Row1Component.cpp`
- **Class:** `Row1Component` extends `OTTO::UI::Layout::ResponsiveComponent`
- **Key Components:** PhosphorIconButton instances for gear, chevrons, play controls
- **Layout System:** Uses `INIConfig::LayoutConstants::Row1` namespace for positioning

### Issue 1.1: Gear Icon Does Not Bring Up Settings Window
**Current State:** 
- Gear button exists as `PhosphorIconButton gearButton` (line 88 in Row1Component.h)
- Has callback `std::function<void()> onGearButtonClicked` (line 43)
- Button setup in `setupTopBarComponents()` method

**Required Fix:**
```cpp
// In Row1Component.cpp setupTopBarComponents() method
gearButton.onClick = [this]() {
    if (onGearButtonClicked) {
        onGearButtonClicked();
    }
    // Alternative: Direct popup creation
    showSettingsWindow();
};

// Add method to Row1Component class
void Row1Component::showSettingsWindow() {
    auto settingsWindow = std::make_unique<SettingsWindow>(colorScheme, fontManager);
    settingsWindow->setSize(600, 400);
    settingsWindow->centreWithSize(600, 400);
    settingsWindow->setVisible(true);
    settingsWindow->toFront(true);
}
```

**Files to Modify:**
1. `Source/Row1Component.h` - Add `showSettingsWindow()` declaration
2. `Source/Row1Component.cpp` - Implement gear button callback and settings window
3. `Source/PopupWindowsSettings.h/cpp` - Ensure SettingsWindow class exists and is properly integrated

### Issue 1.2: Left Chevron Does Not Decrement Preset Menu
**Current State:**
- Left chevron exists as `PhosphorIconButton leftChevronButton` (line 93)
- Method `handlePresetChevrons(bool isRight)` exists (line 147)

**Required Fix:**
```cpp
// In setupTopBarComponents() method
leftChevronButton.onClick = [this]() {
    handlePresetChevrons(false); // false = left/previous
};

// Ensure handlePresetChevrons properly decrements
void Row1Component::handlePresetChevrons(bool isRight) {
    auto allPresets = getAllPresetNames();
    int currentIndex = getPresetIndexFromName(currentPresetName);
    
    if (isRight) {
        currentIndex = (currentIndex + 1) % allPresets.size();
    } else {
        currentIndex = (currentIndex - 1 + allPresets.size()) % allPresets.size();
    }
    
    setPresetSelection(currentIndex);
    updatePresetDisplayToggle();
}
```

### Issue 1.3: Preset Menu Click Behavior (Label/Menu Toggle)
**Current State:**
- Has `togglePresetDisplay()` method (line 150)
- Uses `showingPresetLabel` boolean state (line 125)
- Method `updatePresetDisplayToggle()` exists (line 153)

**Required Fix:**
```cpp
// In setupTopBarComponents() method for presetsMenu
presetsMenu.onClick = [this]() {
    if (showingPresetLabel) {
        // First click: hide label, show menu
        showingPresetLabel = false;
        presetDisplayLabel.setVisible(false);
        presetsMenu.setVisible(true);
        presetsMenu.showPopup(); // Show dropdown immediately
    } else {
        // Second click or selection: show label, hide menu
        showingPresetLabel = true;
        presetDisplayLabel.setVisible(true);
        presetsMenu.setVisible(false);
    }
};

// Ensure preset selection updates label
presetsMenu.onChange = [this]() {
    int selectedId = presetsMenu.getSelectedId();
    if (selectedId > 0) {
        handlePresetMenuSelection(selectedId);
        showingPresetLabel = true;
        presetDisplayLabel.setVisible(true);
        presetsMenu.setVisible(false);
    }
};
```

### Issue 1.4: Right Chevron Does Not Increment Preset Menu
**Required Fix:**
```cpp
// In setupTopBarComponents() method
rightChevronButton.onClick = [this]() {
    handlePresetChevrons(true); // true = right/next
};
```

### Issue 1.5: Tap Tempo Icon Positioning
**Current State:**
- Position defined in `INIConfig::LayoutConstants::Row1::tapTempoX` (line 1390 in INIConfig.h)
- Should stick to left edge of record icon

**Required Fix:**
```cpp
// In INIConfig.h Row1 namespace, update tapTempoX calculation
constexpr int tapTempoX = recordX - iconButtonSize - topBarIconSpacing; // Position relative to record button

// In Row1Component.cpp resized() method
tapTempoButton.setBounds(
    layoutManager.scaled(INIConfig::LayoutConstants::Row1::tapTempoX),
    layoutManager.scaled(INIConfig::LayoutConstants::Row1::iconY),
    layoutManager.scaled(INIConfig::LayoutConstants::Row1::iconButtonSize),
    layoutManager.scaled(INIConfig::LayoutConstants::Row1::iconButtonSize)
);
```

### Issue 1.6: OTTO Logo Click Should Bring Up Splash Screen
**Current State:**
- OTTO label exists as `juce::Label ottoLabel` (line 103)
- No click handler currently implemented

**Required Fix:**
```cpp
// In setupTopBarComponents() method
ottoLabel.setMouseCursor(juce::MouseCursor::PointingHandCursor);
ottoLabel.addMouseListener(this, false);

// In Row1Component.cpp mouseDown method
void Row1Component::mouseDown(const juce::MouseEvent& event) {
    if (event.eventComponent == &ottoLabel) {
        showSplashScreen();
    }
    // Handle other mouse events...
}

// Add method to show splash screen
void Row1Component::showSplashScreen() {
    auto splashWindow = std::make_unique<SplashWindow>(colorScheme, fontManager);
    splashWindow->setSize(400, 300);
    splashWindow->centreWithSize(400, 300);
    splashWindow->setVisible(true);
    splashWindow->toFront(true);
    
    // Auto-close after 3 seconds
    juce::Timer::callAfterDelay(3000, [splashWindow = splashWindow.get()]() {
        if (splashWindow != nullptr) {
            splashWindow->setVisible(false);
        }
    });
}
```

---

## Row 2: Player Selector Buttons Issues

### Current Implementation Analysis
- **File:** `Source/Row2Component.h` and `Source/Row2Component.cpp`
- **Class:** `Row2Component` extends `OTTO::UI::Layout::ResponsiveComponent`
- **Key Components:** `juce::TextButton tabs[INIConfig::Defaults::MAX_PLAYERS]` (line 58)
- **Layout:** Uses `INIConfig::LayoutConstants::Row2` namespace

### Issue 2.1: Player Selection Line Not Centered
**Current State:**
- Selection indication likely handled in `updateTabVisuals()` method (line 72)
- Tab positioning uses percentage-based layout from INIConfig

**Required Fix:**
```cpp
// In Row2Component.cpp updateTabVisuals() method
void Row2Component::updateTabVisuals() {
    for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
        auto& tab = tabs[i];
        
        if (i == selectedTab) {
            // Center the selection indicator
            auto tabBounds = tab.getBounds();
            int indicatorWidth = tabBounds.getWidth() * 0.8f; // 80% of tab width
            int indicatorX = tabBounds.getX() + (tabBounds.getWidth() - indicatorWidth) / 2;
            int indicatorY = tabBounds.getBottom() - 3; // 3px from bottom
            
            // Draw centered selection line
            tab.setColour(juce::TextButton::buttonColourId, 
                         colorScheme.getColor(ColorScheme::ColorRole::Accent));
        } else {
            tab.setColour(juce::TextButton::buttonColourId, 
                         colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        }
    }
}
```

### Issue 2.2: Font Size Should Be Twice as Large
**Current State:**
- Font sizing likely handled in `setupTabs()` method (line 71)
- Uses FontManager for consistent typography

**Required Fix:**
```cpp
// In Row2Component.cpp setupTabs() method
void Row2Component::setupTabs() {
    for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
        auto& tab = tabs[i];
        
        // Double the current font size
        float currentFontSize = layoutManager.scaled(INIConfig::LayoutConstants::Row2::tabFontSize);
        float newFontSize = currentFontSize * 2.0f;
        
        tab.setFont(fontManager.getFont(FontManager::FontRole::Header, newFontSize));
        tab.setComponentID(juce::String("player_tab_") + juce::String(i + 1));
        
        // Ensure text fits with larger font
        tab.setSize(
            layoutManager.scaled(INIConfig::LayoutConstants::Row2::tabWidth),
            layoutManager.scaled(INIConfig::LayoutConstants::Row2::tabHeight)
        );
    }
}
```

### Issue 2.3: Button Text Color - Missing ColorScheme Definition
**Current State:**
- User reports `ColorScheme::ColorScheme() currentThemeSettings.buttonTextColor` is not defined
- Should be "#000000" (black)

**Required Fix:**
```cpp
// In Source/ColorScheme.h - Add to ThemeSettings struct
struct ThemeSettings {
    // ... existing color definitions ...
    juce::String buttonTextColor = "#000000";  // Add this line
    // ... rest of struct ...
};

// In Source/ColorScheme.cpp - Add to setDefaults() method
void ThemeSettings::setDefaults() {
    // ... existing defaults ...
    buttonTextColor = "#000000";  // Black text for buttons
    // ... rest of defaults ...
}

// In Row2Component.cpp setupTabs() method
void Row2Component::setupTabs() {
    for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
        auto& tab = tabs[i];
        
        // Apply button text color from theme
        tab.setColour(juce::TextButton::textColourOffId, 
                     juce::Colour::fromString(colorScheme.getCurrentThemeSettings().buttonTextColor));
        tab.setColour(juce::TextButton::textColourOnId, 
                     juce::Colour::fromString(colorScheme.getCurrentThemeSettings().buttonTextColor));
    }
}
```

### Issue 2.4: Ensure ColorScheme Values Saved in INI System
**Required Fix:**
```cpp
// In Source/ColorScheme.cpp saveStates() method - ensure buttonTextColor is saved
void ColorScheme::saveStates(ComponentState& state) const {
    state.themeSettings = currentThemeSettings;
    state.themeSettings.buttonTextColor = currentThemeSettings.buttonTextColor;
    state.globalSettings.currentThemeName = currentThemeName;
}

// In Source/ColorScheme.cpp loadStates() method - ensure buttonTextColor is loaded
void ColorScheme::loadStates(const ComponentState& state) {
    if (state.themeSettings.isValid()) {
        currentThemeSettings = state.themeSettings;
        // Ensure buttonTextColor has default if not set
        if (currentThemeSettings.buttonTextColor.isEmpty()) {
            currentThemeSettings.buttonTextColor = "#000000";
        }
        notifyListeners();
    }
}
```

---

## Row 3: DrumKit Controls Issues

### Current Implementation Analysis
- **File:** `Source/Row3Component.h` and `Source/Row3Component.cpp`
- **Class:** `Row3Component` extends `OTTO::UI::Layout::ResponsiveComponent`
- **Key Components:** Edit, chevron, mixer, mute buttons, dropdown, player number label
- **Layout:** Uses `INIConfig::LayoutConstants::Row3` namespace

### Issue 3.1: Remove Number "1" Display
**Current State:**
- Player number shown in `juce::Label playerNumberLabel` (line 81 in Row3Component.h)
- Updated in `updatePlayerNumberDisplay()` method (line 96)

**Required Fix:**
```cpp
// In Row3Component.cpp setupDrumKitComponents() method
void Row3Component::setupDrumKitComponents() {
    // Comment out or remove player number label setup
    // playerNumberLabel.setComponentID("player_number_label");
    // playerNumberLabel.setJustificationType(juce::Justification::centred);
    // addAndMakeVisible(playerNumberLabel);  // Remove this line
    
    // Continue with other component setup...
}

// In resized() method, remove playerNumberLabel bounds setting
void Row3Component::resized() {
    // Remove or comment out:
    // playerNumberLabel.setBounds(...);
}

// In updatePlayerNumberDisplay() method
void Row3Component::updatePlayerNumberDisplay() {
    // Remove or comment out the content:
    // playerNumberLabel.setText(juce::String(currentPlayerIndex + 1), juce::dontSendNotification);
}
```

### Issue 3.2: Edit Icon Does Not Bring Up Drumkit Edit Window
**Current State:**
- Edit button exists as `PhosphorIconButton drumKitEditButton` (line 72)
- Has `handleEditButtonClick()` method (line 101)

**Required Fix:**
```cpp
// In Row3Component.cpp handleEditButtonClick() method
void Row3Component::handleEditButtonClick() {
    bool newEditMode = !editMode;
    setEditMode(newEditMode);
    
    if (newEditMode) {
        // Show drumkit edit window
        showDrumKitEditWindow();
    }
    
    if (onEditModeChanged) {
        onEditModeChanged(newEditMode);
    }
}

// Add method to show drumkit edit window
void Row3Component::showDrumKitEditWindow() {
    auto editWindow = std::make_unique<DrumKitEditorWindow>(
        currentDrumKitName, 
        colorScheme, 
        fontManager,
        midiEngine
    );
    editWindow->setSize(800, 600);
    editWindow->centreWithSize(800, 600);
    editWindow->setVisible(true);
    editWindow->toFront(true);
    
    // Set callback for when drumkit is modified
    editWindow->onDrumKitChanged = [this](const juce::String& newKitName) {
        setDrumKitName(newKitName);
        if (onDrumKitChanged) {
            onDrumKitChanged(newKitName);
        }
    };
}
```

### Issue 3.3: Left Chevron Does Not Decrement Drumkit Menu
**Current State:**
- Left chevron exists as `PhosphorIconButton drumKitLeftChevron` (line 73)
- Has `handleDrumKitChevrons(bool isRight)` method (line 100)

**Required Fix:**
```cpp
// In setupDrumKitComponents() method
drumKitLeftChevron.onClick = [this]() {
    handleDrumKitChevrons(false); // false = left/previous
};

// Ensure handleDrumKitChevrons properly decrements
void Row3Component::handleDrumKitChevrons(bool isRight) {
    // Get list of available drumkits
    auto drumKitList = getDrumKitList();
    int currentIndex = drumKitList.indexOf(currentDrumKitName);
    
    if (currentIndex == -1) currentIndex = 0;
    
    if (isRight) {
        currentIndex = (currentIndex + 1) % drumKitList.size();
    } else {
        currentIndex = (currentIndex - 1 + drumKitList.size()) % drumKitList.size();
    }
    
    setDrumKitName(drumKitList[currentIndex]);
    updateDrumKitDisplayToggle();
}
```

### Issue 3.4: Drumkit Menu Label Does Not Return When Item Selected
**Current State:**
- Uses `showingDrumKitLabel` boolean (line 86)
- Has `toggleDrumKitDisplay()` method (line 99)

**Required Fix:**
```cpp
// In setupDrumKitComponents() method for drumKitDropdown
drumKitDropdown.onChange = [this]() {
    int selectedId = drumKitDropdown.getSelectedId();
    if (selectedId > 0) {
        handleDrumKitDropdownChange();
        
        // Always return to label view after selection
        showingDrumKitLabel = true;
        drumKitSelectedLabel.setVisible(true);
        drumKitDropdown.setVisible(false);
        updateDrumKitDisplayToggle();
    }
};

// In toggleDrumKitDisplay() method
void Row3Component::toggleDrumKitDisplay() {
    showingDrumKitLabel = !showingDrumKitLabel;
    
    if (showingDrumKitLabel) {
        drumKitSelectedLabel.setVisible(true);
        drumKitDropdown.setVisible(false);
        drumKitSelectedLabel.setText(currentDrumKitName, juce::dontSendNotification);
    } else {
        drumKitSelectedLabel.setVisible(false);
        drumKitDropdown.setVisible(true);
        drumKitDropdown.showPopup();
    }
}
```

### Issue 3.5: Right Chevron Does Not Increment Drumkit Menu
**Required Fix:**
```cpp
// In setupDrumKitComponents() method
drumKitRightChevron.onClick = [this]() {
    handleDrumKitChevrons(true); // true = right/next
};
```

### Issue 3.6: Mixer Icon Does Not Bring Up Mixer Window
**Current State:**
- Mixer button exists as `PhosphorIconButton drumKitMixerButton` (line 76)
- Has `handleMixerButtonClick()` method (line 103)

**Required Fix:**
```cpp
// In handleMixerButtonClick() method
void Row3Component::handleMixerButtonClick() {
    showMixerWindow();
    
    if (onMixerRequested) {
        onMixerRequested();
    }
}

// Add method to show mixer window
void Row3Component::showMixerWindow() {
    auto mixerWindow = std::make_unique<MixerWindow>(
        mixer,
        currentPlayerIndex,
        colorScheme,
        fontManager
    );
    mixerWindow->setSize(1000, 700);
    mixerWindow->centreWithSize(1000, 700);
    mixerWindow->setVisible(true);
    mixerWindow->toFront(true);
}
```

### Issue 3.7: Mute Icon Toggles But Doesn't Mute MIDI Signal
**Current State:**
- Mute button exists as `PhosphorIconButton drumKitMuteButton` (line 75)
- Has `handleMuteButtonClick()` method (line 102)

**Required Fix:**
```cpp
// In handleMuteButtonClick() method
void Row3Component::handleMuteButtonClick() {
    bool newMuteState = !drumKitMuteButton.getToggleState();
    drumKitMuteButton.setToggleState(newMuteState, juce::dontSendNotification);
    
    // Actually mute the MIDI signal to SFZ player
    midiEngine.setPlayerMuted(currentPlayerIndex, newMuteState);
    
    updateMuteButtonState();
    
    if (onMuteToggled) {
        onMuteToggled(newMuteState);
    }
}

// Ensure MidiEngine has setPlayerMuted method
// In MidiEngine.h add:
void setPlayerMuted(int playerIndex, bool muted);

// In MidiEngine.cpp implement:
void MidiEngine::setPlayerMuted(int playerIndex, bool muted) {
    if (playerIndex >= 0 && playerIndex < MAX_PLAYERS) {
        players[playerIndex].setMuted(muted);
    }
}
```

### Issue 3.8: All Icons Should Be Centered to App Window
**Required Fix:**
```cpp
// In Row3Component.cpp resized() method
void Row3Component::resized() {
    ResponsiveComponent::resized();
    
    auto bounds = getRowBounds();
    int totalWidth = bounds.getWidth();
    
    // Calculate total width needed for all components
    int buttonSize = layoutManager.scaled(INIConfig::LayoutConstants::Row3::buttonSize);
    int spacing = layoutManager.scaled(INIConfig::LayoutConstants::Row3::buttonSpacing);
    int dropdownWidth = layoutManager.scaled(INIConfig::LayoutConstants::Row3::dropdownWidth);
    
    int totalComponentsWidth = (5 * buttonSize) + (4 * spacing) + dropdownWidth;
    int startX = (totalWidth - totalComponentsWidth) / 2; // Center all components
    
    // Position components from center outward
    int currentX = startX;
    
    drumKitEditButton.setBounds(currentX, bounds.getY() + spacing, buttonSize, buttonSize);
    currentX += buttonSize + spacing;
    
    drumKitLeftChevron.setBounds(currentX, bounds.getY() + spacing, buttonSize, buttonSize);
    currentX += buttonSize + spacing;
    
    // Continue for all components...
}
```

---

## Row 4: Pattern Group Controls Issues

### Current Implementation Analysis
- **File:** `Source/Row4Component.h` and `Source/Row4Component.cpp`
- **Class:** `Row4Component` extends `OTTO::UI::Layout::ResponsiveComponent`
- **Key Components:** Pattern group controls and labels (moved from Row 5)
- **Layout:** Uses `INIConfig::LayoutConstants::Row4` namespace

### Issue 4.1: Pattern Group Edit Icon Does Not Bring Up Editor
**Current State:**
- Edit button exists as `PhosphorIconButton patternGroupEditButton` (line 68)
- Has `setPatternGroupEditMode(bool editMode)` method (line 39)

**Required Fix:**
```cpp
// In setupPatternGroupComponents() method
patternGroupEditButton.onClick = [this]() {
    bool newEditMode = !isPatternGroupEditMode();
    setPatternGroupEditMode(newEditMode);
    
    if (newEditMode) {
        showPatternGroupEditor();
    }
};

// Add method to show pattern group editor
void Row4Component::showPatternGroupEditor() {
    auto editorWindow = std::make_unique<PatternGroupEditorWindow>(
        getCurrentPatternGroupIndex(),
        iniDataManager,
        colorScheme,
        fontManager
    );
    editorWindow->setSize(900, 650);
    editorWindow->centreWithSize(900, 650);
    editorWindow->setVisible(true);
    editorWindow->toFront(true);
    
    // Set callback for pattern group changes
    editorWindow->onPatternGroupChanged = [this](int newIndex) {
        setCurrentPatternGroupIndex(newIndex);
        animatePatternGroupChange(newIndex);
    };
}
```

### Issue 4.2: Left Chevron Does Not Decrement Pattern Group Menu
**Current State:**
- Left chevron exists as `PhosphorIconButton patternGroupLeftChevron` (line 69)
- Has `navigatePatternGroup(bool isNext)` method (line 41)

**Required Fix:**
```cpp
// In setupPatternGroupComponents() method
patternGroupLeftChevron.onClick = [this]() {
    navigatePatternGroup(false); // false = previous
};

// Ensure navigatePatternGroup properly decrements
void Row4Component::navigatePatternGroup(bool isNext) {
    int currentIndex = getCurrentPatternGroupIndex();
    int maxGroups = getMaxPatternGroups(); // Get from INI or config
    
    if (isNext) {
        currentIndex = (currentIndex + 1) % maxGroups;
    } else {
        currentIndex = (currentIndex - 1 + maxGroups) % maxGroups;
    }
    
    setCurrentPatternGroupIndex(currentIndex);
    animatePatternGroupChange(currentIndex);
    updatePatternGroupButtonStates();
}
```

### Issue 4.3: Pattern Group Dropdown Does Nothing
**Current State:**
- Dropdown exists as `HierarchicalComboBox patternGroupDropdown` (line 70)
- Should show pattern groups

**Required Fix:**
```cpp
// In setupPatternGroupComponents() method
patternGroupDropdown.onChange = [this]() {
    int selectedId = patternGroupDropdown.getSelectedId();
    if (selectedId > 0) {
        setCurrentPatternGroupIndex(selectedId - 1); // Convert to 0-based index
        animatePatternGroupChange(selectedId - 1);
        updatePatternGroupButtonStates();
    }
};

// Add method to populate dropdown
void Row4Component::populatePatternGroupDropdown() {
    patternGroupDropdown.clear();
    
    // Load pattern groups from INI data
    if (iniDataManager) {
        auto patternGroups = iniDataManager->getPatternGroups();
        
        for (int i = 0; i < patternGroups.size(); ++i) {
            patternGroupDropdown.addItem(patternGroups[i].name, i + 1);
        }
    } else {
        // Default pattern groups if no INI data
        for (int i = 0; i < 16; ++i) {
            patternGroupDropdown.addItem(juce::String("Group ") + juce::String(i + 1), i + 1);
        }
    }
    
    patternGroupDropdown.setSelectedId(getCurrentPatternGroupIndex() + 1);
}
```

### Issue 4.4: Pattern Group Label Visibility Toggle
**Current State:**
- Should handle label/menu visibility similar to preset system

**Required Fix:**
```cpp
// Add state variable to Row4Component.h
bool showingPatternGroupLabel = true;

// In setupPatternGroupComponents() method
patternGroupDropdown.onClick = [this]() {
    if (showingPatternGroupLabel) {
        // First click: hide label, show menu
        showingPatternGroupLabel = false;
        // Hide label component if exists
        patternGroupDropdown.showPopup();
    } else {
        // Second click: show label, hide menu
        showingPatternGroupLabel = true;
        // Show label component if exists
    }
};

// Add toggle method
void Row4Component::togglePatternGroupDisplay() {
    showingPatternGroupLabel = !showingPatternGroupLabel;
    updatePatternGroupDisplayToggle();
}

void Row4Component::updatePatternGroupDisplayToggle() {
    // Update visibility based on showingPatternGroupLabel state
    // Implementation depends on whether you have a separate label component
}
```

### Issue 4.5: Right Chevron Does Not Increment Pattern Group Menu
**Required Fix:**
```cpp
// In setupPatternGroupComponents() method
patternGroupRightChevron.onClick = [this]() {
    navigatePatternGroup(true); // true = next
};
```

### Issue 4.6: Favorites Icon Does Not Toggle/Mark Current Group
**Current State:**
- Favorite button exists as `PhosphorIconButton patternGroupFavoriteButton` (line 72)
- Has `togglePatternGroupFavorite()` method (line 42)

**Required Fix:**
```cpp
// In setupPatternGroupComponents() method
patternGroupFavoriteButton.onClick = [this]() {
    togglePatternGroupFavorite();
};

// Implement togglePatternGroupFavorite method
void Row4Component::togglePatternGroupFavorite() {
    int currentIndex = getCurrentPatternGroupIndex();
    bool isFavorite = isPatternGroupFavorite(currentIndex);
    
    setPatternGroupFavorite(currentIndex, !isFavorite);
    updateFavoriteButtonState();
    
    // Save to INI data
    if (iniDataManager) {
        iniDataManager->setPatternGroupFavorite(currentIndex, !isFavorite);
    }
}

// Add helper methods
bool Row4Component::isPatternGroupFavorite(int index) {
    if (iniDataManager) {
        return iniDataManager->isPatternGroupFavorite(index);
    }
    return false;
}

void Row4Component::setPatternGroupFavorite(int index, bool favorite) {
    if (iniDataManager) {
        iniDataManager->setPatternGroupFavorite(index, favorite);
    }
}

void Row4Component::updateFavoriteButtonState() {
    bool isFavorite = isPatternGroupFavorite(getCurrentPatternGroupIndex());
    patternGroupFavoriteButton.setToggleState(isFavorite, juce::dontSendNotification);
    
    // Update visual appearance
    if (isFavorite) {
        patternGroupFavoriteButton.setColour(juce::TextButton::buttonColourId,
                                           colorScheme.getColor(ColorScheme::ColorRole::Accent));
    } else {
        patternGroupFavoriteButton.setColour(juce::TextButton::buttonColourId,
                                           colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
    }
}
```

### Issue 4.7: Labels Too Small - Need Playfair Display Font 2x Larger
**Current State:**
- Labels exist: togglesLabel, fillsLabel, swingLabel, energyLabel, volumeLabel (lines 73-77)
- Currently using default font sizing

**Required Fix:**
```cpp
// In setupLabels() method
void Row4Component::setupLabels() {
    // Get Playfair Display font at 2x size
    float baseFontSize = layoutManager.scaled(INIConfig::LayoutConstants::Row4::labelFontSize);
    float enlargedFontSize = baseFontSize * 2.0f;
    
    // Setup each label with Playfair Display font
    auto setupLabel = [this, enlargedFontSize](juce::Label& label, const juce::String& text) {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setFont(fontManager.getFont(FontManager::FontRole::PlayfairDisplay, enlargedFontSize));
        label.setColour(juce::Label::textColourId, 
                       colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
        addAndMakeVisible(label);
    };
    
    setupLabel(togglesLabel, "TOGGLES");
    setupLabel(fillsLabel, "FILLS");
    setupLabel(swingLabel, "SWING");
    setupLabel(energyLabel, "ENERGY");
    setupLabel(volumeLabel, "VOLUME");
}

// Ensure FontManager supports Playfair Display
// In FontManager.h add enum value:
enum class FontRole {
    Header,
    Body,
    Caption,
    PlayfairDisplay  // Add this
};

// In FontManager.cpp implement Playfair Display loading
juce::Font FontManager::getFont(FontRole role, float size) {
    switch (role) {
        case FontRole::PlayfairDisplay:
            return juce::Font(juce::Typeface::createSystemTypefaceFor(
                BinaryData::PlayfairDisplay_ttf, 
                BinaryData::PlayfairDisplay_ttfSize
            )).withHeight(size);
        // ... other cases
    }
}
```

---

## Row 5: Interactive Controls Issues

### Current Implementation Analysis
- **File:** `Source/Row5Component.h` and `Source/Row5Component.cpp`
- **Class:** `Row5Component` extends `OTTO::UI::Layout::ResponsiveComponent`
- **Key Components:** 4x4 drum matrix, toggle buttons, fill buttons, sliders
- **Layout:** Left section (60% width) for drum matrix, right section (39.5% width) for controls

### Issue 5.1: 4x4 Matrix Buttons Too Small - Need More Room
**Current State:**
- Drum buttons array: `juce::TextButton drumButtons[INIConfig::Audio::NUM_DRUM_PADS]` (line 163)
- Currently positioned in left section with 60% width

**Required Fix:**
```cpp
// In INIConfig.h LayoutConstants::Row5 namespace, increase drum matrix allocation
namespace Row5 {
    // Increase drum matrix width from 60% to 75%
    constexpr float drumMatrixWidthPercent = 75.0f;
    constexpr float controlsWidthPercent = 24.0f; // Reduce controls area
    constexpr float separatorWidthPercent = 1.0f;
    
    // Increase individual drum button size
    constexpr float drumButtonSizePercent = 16.0f; // Increase from current size
    constexpr float drumButtonSpacingPercent = 2.0f;
}

// In Row5Component.cpp setupDrumGrid() method
void Row5Component::setupDrumGrid() {
    int matrixWidth = getWidth() * (INIConfig::LayoutConstants::Row5::drumMatrixWidthPercent / 100.0f);
    int buttonSize = matrixWidth * (INIConfig::LayoutConstants::Row5::drumButtonSizePercent / 100.0f);
    int spacing = matrixWidth * (INIConfig::LayoutConstants::Row5::drumButtonSpacingPercent / 100.0f);
    
    // Calculate grid positioning to center within left section
    int gridTotalWidth = (4 * buttonSize) + (3 * spacing);
    int gridTotalHeight = (4 * buttonSize) + (3 * spacing);
    int startX = (matrixWidth - gridTotalWidth) / 2;
    int startY = (getHeight() - gridTotalHeight) / 2;
    
    // Position 4x4 grid with larger buttons
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            int index = row * 4 + col;
            int x = startX + col * (buttonSize + spacing);
            int y = startY + row * (buttonSize + spacing);
            
            drumButtons[index].setBounds(x, y, buttonSize, buttonSize);
        }
    }
}
```

### Issue 5.2: MIDI Pattern Filename Buttons Font Size 2x Increase
**Required Fix:**
```cpp
// In setupDrumGrid() method
void Row5Component::setupDrumGrid() {
    float baseFontSize = layoutManager.scaled(INIConfig::LayoutConstants::Row5::drumButtonFontSize);
    float enlargedFontSize = baseFontSize * 2.0f;
    
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        auto& button = drumButtons[i];
        
        // Set larger font
        button.setFont(fontManager.getFont(FontManager::FontRole::Header, enlargedFontSize));
        
        // Ensure button is large enough for larger text
        button.setSize(buttonSize, buttonSize); // Use calculated larger buttonSize
    }
}
```

### Issue 5.3: Show MIDI Pattern Filenames Instead of Numbers
**Current State:**
- Buttons currently show numbers
- Should load from `/Assets/MidiFiles/Grooves` directory
- Found files: "Basic House.MID", "70's Rock.MID", "Fusion Funk.MID", etc.

**Required Fix:**
```cpp
// Add method to load MIDI filenames
juce::StringArray Row5Component::loadMidiPatternFilenames() {
    juce::StringArray filenames;
    
    // Load from Assets/MidiFiles/Grooves directory
    juce::File groovesDir = juce::File::getCurrentWorkingDirectory()
                           .getChildFile("Assets")
                           .getChildFile("MidiFiles")
                           .getChildFile("Grooves");
    
    if (groovesDir.exists()) {
        auto midiFiles = groovesDir.findChildFiles(juce::File::findFiles, false, "*.mid;*.MID");
        
        for (auto& file : midiFiles) {
            // Remove extension and add to list
            juce::String filename = file.getFileNameWithoutExtension();
            filenames.add(filename);
        }
    }
    
    // If no files found, use default names
    if (filenames.isEmpty()) {
        filenames.add("Basic House");
        filenames.add("70's Rock");
        filenames.add("Fusion Funk");
        filenames.add("Hip Hop Funk");
        // ... add more defaults
    }
    
    return filenames;
}

// In setupDrumGrid() method
void Row5Component::setupDrumGrid() {
    auto midiFilenames = loadMidiPatternFilenames();
    
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        auto& button = drumButtons[i];
        
        // Set MIDI filename as button text
        if (i < midiFilenames.size()) {
            button.setButtonText(midiFilenames[i]);
            assignedMidiFiles[i] = midiFilenames[i] + ".mid";
        } else {
            button.setButtonText("Empty");
            assignedMidiFiles[i] = "";
        }
        
        // Set larger font
        float enlargedFontSize = baseFontSize * 2.0f;
        button.setFont(fontManager.getFont(FontManager::FontRole::Header, enlargedFontSize));
    }
}

// Add method to group by pattern groups (future enhancement)
void Row5Component::loadMidiFilesByPatternGroup(int patternGroupIndex) {
    // Implementation to filter MIDI files by pattern group
    // This would integrate with the pattern group system from Row4
}
```

### Issue 5.4: Toggle and Fill Buttons Should Be Arranged Vertically
**Current State:**
- Toggle buttons: `juce::TextButton toggleButtons[INIConfig::UI::MAX_TOGGLE_STATES]` (line 167)
- Fill buttons: `juce::TextButton fillButtons[INIConfig::UI::MAX_FILL_STATES]` (line 168)
- Currently arranged horizontally

**Required Fix:**
```cpp
// In setupControlButtons() method
void Row5Component::setupControlButtons() {
    int controlsStartX = getWidth() * (INIConfig::LayoutConstants::Row5::drumMatrixWidthPercent / 100.0f);
    int controlsWidth = getWidth() * (INIConfig::LayoutConstants::Row5::controlsWidthPercent / 100.0f);
    
    // Vertical arrangement for toggle buttons
    int toggleButtonHeight = 30;
    int toggleButtonSpacing = 5;
    int toggleStartY = 20;
    
    for (int i = 0; i < INIConfig::UI::MAX_TOGGLE_STATES; ++i) {
        auto& button = toggleButtons[i];
        
        int y = toggleStartY + i * (toggleButtonHeight + toggleButtonSpacing);
        button.setBounds(controlsStartX + 10, y, controlsWidth - 20, toggleButtonHeight);
        button.setButtonText(juce::String("Toggle ") + juce::String(i + 1));
        addAndMakeVisible(button);
    }
    
    // Vertical arrangement for fill buttons
    int fillButtonHeight = 30;
    int fillButtonSpacing = 5;
    int fillStartY = toggleStartY + (INIConfig::UI::MAX_TOGGLE_STATES * (toggleButtonHeight + toggleButtonSpacing)) + 20;
    
    for (int i = 0; i < INIConfig::UI::MAX_FILL_STATES; ++i) {
        auto& button = fillButtons[i];
        
        int y = fillStartY + i * (fillButtonHeight + fillButtonSpacing);
        button.setBounds(controlsStartX + 10, y, controlsWidth - 20, fillButtonHeight);
        button.setButtonText(juce::String("Fill ") + juce::String(i + 1));
        addAndMakeVisible(button);
    }
}
```

### Issue 5.5: Sliders Should Be Vertical, Not Horizontal
**Current State:**
- Sliders: swingSlider, energySlider, volumeSlider (lines 169-171)
- Currently horizontal orientation

**Required Fix:**
```cpp
// In setupSliders() method
void Row5Component::setupSliders() {
    int controlsStartX = getWidth() * (INIConfig::LayoutConstants::Row5::drumMatrixWidthPercent / 100.0f);
    int controlsWidth = getWidth() * (INIConfig::LayoutConstants::Row5::controlsWidthPercent / 100.0f);
    
    // Calculate vertical slider positioning
    int sliderWidth = 40;
    int sliderHeight = 120;
    int sliderSpacing = (controlsWidth - (3 * sliderWidth)) / 4; // Even spacing
    
    // Position sliders vertically in bottom section
    int slidersStartY = getHeight() - sliderHeight - 40; // 40px from bottom
    
    // Swing slider
    swingSlider.setSliderStyle(juce::Slider::LinearVertical);
    swingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    swingSlider.setBounds(controlsStartX + sliderSpacing, slidersStartY, sliderWidth, sliderHeight);
    swingSlider.setRange(0.0, 100.0, 1.0);
    swingSlider.setValue(50.0);
    addAndMakeVisible(swingSlider);
    
    // Energy slider
    energySlider.setSliderStyle(juce::Slider::LinearVertical);
    energySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    energySlider.setBounds(controlsStartX + sliderSpacing + sliderWidth + sliderSpacing, slidersStartY, sliderWidth, sliderHeight);
    energySlider.setRange(0.0, 100.0, 1.0);
    energySlider.setValue(75.0);
    addAndMakeVisible(energySlider);
    
    // Volume slider
    volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeSlider.setBounds(controlsStartX + sliderSpacing + (2 * sliderWidth) + (2 * sliderSpacing), slidersStartY, sliderWidth, sliderHeight);
    volumeSlider.setRange(0.0, 100.0, 1.0);
    volumeSlider.setValue(80.0);
    addAndMakeVisible(volumeSlider);
}
```

---

## Row 6: Loop Controls Issues

### Current Implementation Analysis
- **File:** `Source/Row6Component.h` and `Source/Row6Component.cpp`
- **Class:** `Row6Component` extends `OTTO::UI::Layout::ResponsiveComponent`
- **Key Components:** Loop start/end labels, horizontal slider
- **Layout:** Uses `INIConfig::LayoutConstants::Row6` namespace

### Issue 6.1: Remove Duplicate "LOOP START" and "LOOP END" Labels
**Current State:**
- Start label: `juce::Label startLabel` (line 46)
- End label: `juce::Label endLabel` (line 47)
- User reports seeing duplicates

**Required Fix:**
```cpp
// In Row6Component.cpp setupLoopComponents() method
void Row6Component::setupLoopComponents() {
    // Ensure only one start label
    startLabel.setComponentID("loop_start_label");
    startLabel.setText("LOOP START", juce::dontSendNotification);
    startLabel.setJustificationType(juce::Justification::centredLeft);
    startLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, 
                      layoutManager.scaled(INIConfig::LayoutConstants::Row6::labelFontSize)));
    startLabel.setColour(juce::Label::textColourId, 
                        colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    addAndMakeVisible(startLabel);
    
    // Ensure only one end label
    endLabel.setComponentID("loop_end_label");
    endLabel.setText("LOOP END", juce::dontSendNotification);
    endLabel.setJustificationType(juce::Justification::centredRight);
    endLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, 
                    layoutManager.scaled(INIConfig::LayoutConstants::Row6::labelFontSize)));
    endLabel.setColour(juce::Label::textColourId, 
                      colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    addAndMakeVisible(endLabel);
    
    // Remove any duplicate label creation code
    // Check for any other label components that might be duplicates
}
```

### Issue 6.2: Remove Duplicate Horizontal Slider
**Current State:**
- Loop slider: `ScaledSlider loopSlider` (line 48)
- User reports seeing two sliders

**Required Fix:**
```cpp
// In setupLoopComponents() method - ensure only one slider
void Row6Component::setupLoopComponents() {
    // ... label setup above ...
    
    // Ensure only one loop slider exists
    loopSlider.setComponentID("loop_position_slider");
    loopSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    loopSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    loopSlider.setRange(0.0, 100.0, 0.1);
    loopSlider.setValue(0.0);
    loopSlider.onValueChange = [this]() {
        if (onLoopPositionChanged) {
            onLoopPositionChanged(static_cast<float>(loopSlider.getValue()));
        }
    };
    addAndMakeVisible(loopSlider);
    
    // Remove any duplicate slider creation code
    // Check for any other slider components that might be duplicates
}
```

### Issue 6.3: Slider Should Travel Full Width of App Window
**Current State:**
- Slider width likely constrained to row bounds

**Required Fix:**
```cpp
// In Row6Component.cpp resized() method
void Row6Component::resized() {
    ResponsiveComponent::resized();
    
    auto bounds = getRowBounds();
    int fullWindowWidth = layoutManager.getWindowWidth(); // Get full app width
    
    // Position labels at edges
    int labelHeight = layoutManager.scaled(INIConfig::LayoutConstants::Row6::labelHeight);
    int labelY = bounds.getY() + 5; // Small margin from top
    
    startLabel.setBounds(10, labelY, 100, labelHeight); // Left edge with margin
    endLabel.setBounds(fullWindowWidth - 110, labelY, 100, labelHeight); // Right edge with margin
    
    // Slider spans full width with margins
    int sliderMargin = 20;
    int sliderY = labelY + labelHeight + 5; // Below labels
    int sliderHeight = bounds.getHeight() - labelHeight - 15; // Remaining height
    
    loopSlider.setBounds(sliderMargin, sliderY, 
                        fullWindowWidth - (2 * sliderMargin), sliderHeight);
}
```

### Issue 6.4: Slider Image Should Rotate 90 Degrees (Visual Only)
**Current State:**
- Slider travels horizontally but image might need rotation

**Required Fix:**
```cpp
// Create custom LookAndFeel for rotated slider thumb
class RotatedSliderLookAndFeel : public juce::LookAndFeel_V4 {
public:
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float minSliderPos, float maxSliderPos,
                         const juce::Slider::SliderStyle style, juce::Slider& slider) override {
        
        // Draw the track normally (horizontal)
        juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, 
                                              minSliderPos, maxSliderPos, style, slider);
        
        // Draw custom rotated thumb
        float thumbX = x + sliderPos * (width - 20); // 20 = thumb width
        float thumbY = y + (height - 30) / 2; // 30 = thumb height, center vertically
        
        // Save graphics state
        juce::Graphics::ScopedSaveState saveState(g);
        
        // Rotate thumb image 90 degrees
        g.addTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::halfPi, 
                                                      thumbX + 10, thumbY + 15));
        
        // Draw rotated thumb
        juce::Rectangle<float> thumbBounds(thumbX, thumbY, 20, 30);
        g.setColour(juce::Colours::lightgrey);
        g.fillRoundedRectangle(thumbBounds, 3.0f);
        g.setColour(juce::Colours::darkgrey);
        g.drawRoundedRectangle(thumbBounds, 3.0f, 1.0f);
    }
};

// In Row6Component constructor
Row6Component::Row6Component(...) {
    // ... existing initialization ...
    
    // Apply custom look and feel to slider
    rotatedSliderLookAndFeel = std::make_unique<RotatedSliderLookAndFeel>();
    loopSlider.setLookAndFeel(rotatedSliderLookAndFeel.get());
}

// Add to Row6Component.h private members:
std::unique_ptr<RotatedSliderLookAndFeel> rotatedSliderLookAndFeel;
```

---

## Implementation Priority and Dependencies

### Phase 1: Core Functionality Fixes (High Priority)
1. **Row 1**: Gear button settings window, chevron navigation, OTTO logo splash
2. **Row 3**: Edit button drumkit editor, mixer button window, mute functionality
3. **Row 4**: Pattern group editor, navigation, favorites system

### Phase 2: Visual and Layout Improvements (Medium Priority)
1. **Row 2**: Font sizing, color scheme integration, centering
2. **Row 5**: Button sizing, vertical arrangements, MIDI filename loading
3. **Row 6**: Duplicate removal, full-width slider

### Phase 3: Advanced Features (Lower Priority)
1. **Row 5**: Pattern group integration with MIDI files
2. **Row 4**: Advanced pattern group animations
3. **Row 6**: Custom slider visual effects

### Required New Components
1. **SettingsWindow** - For gear button functionality
2. **DrumKitEditorWindow** - For drumkit editing
3. **MixerWindow** - For mixer functionality
4. **PatternGroupEditorWindow** - For pattern group editing
5. **SplashWindow** - For OTTO logo click

### ColorScheme System Updates
1. Add `buttonTextColor` property to ThemeSettings
2. Ensure all color values are saved/loaded via INI system
3. Update ColorScheme.h and ColorScheme.cpp accordingly

### Font System Updates
1. Add Playfair Display font support to FontManager
2. Implement 2x font sizing for Row 2 and Row 4 labels
3. Ensure font resources are properly embedded

### MIDI File Integration
1. Create MIDI file loading system for Assets/MidiFiles/Grooves
2. Implement pattern group filtering for MIDI files
3. Add drag-and-drop support for custom MIDI files

---

## Testing and Validation

### Functional Testing
1. Verify all button callbacks trigger appropriate windows/actions
2. Test chevron navigation in both directions with wraparound
3. Validate mute functionality actually affects MIDI signal
4. Confirm preset and drumkit selection persistence

### Visual Testing
1. Verify font sizes are exactly 2x larger where specified
2. Confirm vertical arrangements for toggles, fills, and sliders
3. Test centering of all icons and components
4. Validate full-width slider spans entire app window

### Integration Testing
1. Test ColorScheme integration with INI save/load system
2. Verify MIDI filename loading from Assets directory
3. Confirm pattern group system integration
4. Test responsive layout at different window sizes

### Accessibility Testing
1. Verify minimum 44px touch targets maintained
2. Test keyboard navigation for all interactive elements
3. Confirm color contrast meets accessibility standards
4. Validate screen reader compatibility

---

## Conclusion

This comprehensive guide provides detailed implementation steps for fixing all identified GUI issues across OTTO's 6-row interface. Each fix includes specific code examples, file locations, and integration requirements. The phased approach ensures critical functionality is addressed first, followed by visual improvements and advanced features.

The implementation should maintain OTTO's existing INI-driven architecture while adding the missing functionality and improving the user experience as specified in the original requirements.
