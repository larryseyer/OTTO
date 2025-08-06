/**
 * @file Row3Component.cpp
 * @brief Implementation of Row 3 DrumKit Controls for OTTO's row-based architecture
 * 
 * This file implements the Row3Component class, which manages all DrumKit-related
 * controls in the third row of OTTO's interface. It provides drumkit selection,
 * navigation, editing, and mixing functionality with proper JUCE 8 integration.
 * 
 * ROW 3 LAYOUT:
 * =============
 * [LARGE PLAYER #] [Edit] [<] [DrumKit ▼] [>] [Mixer] [Mute]
 * 
 * COMPONENTS:
 * ===========
 * - Player Number: Large display showing current player (1-8)
 * - Edit Button: Toggle edit mode for pattern editing
 * - Left/Right Chevrons: Navigate between drumkits
 * - DrumKit Dropdown/Label: Select drumkit or show current selection
 * - Mixer Button: Open mixer interface
 * - Mute Button: Toggle mute state for current player
 * 
 * @author OTTO Development Team
 * @version 2.0
 * @date 2025
 */

#include "JUCE8_CODING_STANDARDS.h"
#include "Row3Component.h"
#include "MidiEngine.h"
#include "Mixer.h"
#include "INIConfig.h"
#include "ErrorHandling.h"
#include "UI/Visualizations/WaveformDisplay.h"
#include "PopupWindows.h"

Row3Component::Row3Component(MidiEngine& midiEngine,
                           Mixer& mixer,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : ResponsiveComponent(),
      midiEngine(midiEngine), mixer(mixer),
      layoutManager(layoutManager), fontManager(fontManager), colorScheme(colorScheme),
      
      // Initialize DrumKit control buttons using Phosphor icon system
      drumKitEditButton("pencil", FontManager::PhosphorWeight::Regular),
      drumKitLeftChevron("caret-left", FontManager::PhosphorWeight::Regular),
      drumKitRightChevron("caret-right", FontManager::PhosphorWeight::Regular),
      drumKitMuteButton("unmute", FontManager::PhosphorWeight::Regular),
      drumKitMixerButton("mixer", FontManager::PhosphorWeight::Regular) {
    
    setupDrumKitComponents();
}

void Row3Component::paint(juce::Graphics& g) {
    // Fill background with theme color
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    
    // Optional: Add debug border in development
    #ifdef JUCE_DEBUG
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::DebugBorder));
    g.drawRect(getLocalBounds(), 1);
    #endif
}

void Row3Component::resized() {
    ResponsiveComponent::resized(); // Call parent first
    updateDrumKitLayout();
    
    // PHASE 9D: Update waveform bounds when component is resized
    if (waveformDisplay && waveformVisible) {
        updateWaveformBounds();
    }
}

void Row3Component::saveStates(ComponentState& state) {
    state.editMode = editMode;
    state.currentPlayer = currentPlayerIndex;
    // Store drumkit name in dropdown selections map
    state.dropdownSelections["row3_current_drumkit"] = 0; // Index will be set by actual dropdown
    // Store showing label state in toggle states
    state.toggleStates[3001] = showingDrumKitLabel; // Use unique ID for Row3 label toggle
}

void Row3Component::loadStates(const ComponentState& state) {
    editMode = state.editMode;
    currentPlayerIndex = state.currentPlayer;
    showingDrumKitLabel = state.toggleStates.count(3001) ? state.toggleStates.at(3001) : true;
    // currentDrumKitName will be set by the dropdown selection logic
    if (currentDrumKitName.isEmpty()) {
        currentDrumKitName = "808 Classic";
    }
    
    updateFromState(state);
}

void Row3Component::updateFromState(const ComponentState& state) {
    drumKitEditButton.setToggleState(editMode, juce::dontSendNotification);
    drumKitSelectedLabel.setText(currentDrumKitName, juce::dontSendNotification);
    // updatePlayerNumberDisplay();  // Issue 3.1: Removed player number display
    updateMuteButtonState();
    updateDrumKitDisplayToggle();
}

// getRowName() is defined inline in the header file

juce::Rectangle<int> Row3Component::getRowBounds() const {
    return juce::Rectangle<int>(0, INIConfig::LayoutConstants::Row3::yPosition,
                               INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH,
                               INIConfig::LayoutConstants::Row3::height);
}

void Row3Component::lookAndFeelChanged() {
    ResponsiveComponent::lookAndFeelChanged();
    
    // Update all component colors
    drumKitEditButton.setColorScheme(&colorScheme);
    drumKitLeftChevron.setColorScheme(&colorScheme);
    drumKitRightChevron.setColorScheme(&colorScheme);
    drumKitMuteButton.setColorScheme(&colorScheme);
    drumKitMixerButton.setColorScheme(&colorScheme);
    
    // Update label colors
    drumKitSelectedLabel.setColour(juce::Label::textColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    drumKitSelectedLabel.setColour(juce::Label::backgroundColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    
    playerNumberLabel.setColour(juce::Label::textColourId, 
                               colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    
    // Update dropdown colors
    drumKitDropdown.setColour(juce::ComboBox::textColourId, 
                             colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    drumKitDropdown.setColour(juce::ComboBox::backgroundColourId, 
                             colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
}

void Row3Component::mouseDown(const juce::MouseEvent& event) {
    // Handle drumkit label click to toggle display
    if (event.eventComponent == &drumKitSelectedLabel) {
        toggleDrumKitDisplay();
    }
}

void Row3Component::updatePlayerDisplay(int playerIndex) {
    currentPlayerIndex = INIConfig::clampPlayerIndex(playerIndex);
    // updatePlayerNumberDisplay();  // Issue 3.1: Removed player number display
    updateMuteButtonState();
}

void Row3Component::setDrumKitName(const juce::String& name) {
    currentDrumKitName = name;
    drumKitSelectedLabel.setText(name, juce::dontSendNotification);
    
    // Update dropdown selection if it matches
    for (int i = 1; i <= drumKitDropdown.getNumItems(); ++i) {
        if (drumKitDropdown.getItemText(i - 1) == name) {
            drumKitDropdown.setSelectedId(i, juce::dontSendNotification);
            break;
        }
    }
}

juce::String Row3Component::getCurrentDrumKitName() const {
    return currentDrumKitName;
}

void Row3Component::setEditMode(bool editMode) {
    this->editMode = editMode;
    drumKitEditButton.setToggleState(editMode, juce::dontSendNotification);
}

bool Row3Component::isEditMode() const {
    return editMode;
}

void Row3Component::setMuteState(bool muted) {
    drumKitMuteButton.setToggleState(muted, juce::dontSendNotification);
    drumKitMuteButton.setIconName(muted ? "mute" : "unmute");
}

bool Row3Component::isMuted() const {
    return drumKitMuteButton.getToggleState();
}

void Row3Component::setupDrumKitComponents() {
    // PLAYER NUMBER LABEL: Large display for current player
    // Issue 3.1: Remove Number "1" Display - Comment out player number label setup
    // playerNumberLabel.setComponentID("player_number_label");
    // playerNumberLabel.setJustificationType(juce::Justification::centred);
    // playerNumberLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, 
    //                                              INIConfig::LayoutConstants::Row3::largePlayerFontSize));
    // playerNumberLabel.setColour(juce::Label::textColourId, 
    //                            colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    // addAndMakeVisible(playerNumberLabel);  // Removed to hide player number display
    
    // EDIT BUTTON: Toggle edit mode for pattern editing
    drumKitEditButton.setComponentID("drumkit_edit_button");
    drumKitEditButton.setColorScheme(&colorScheme);
    drumKitEditButton.setTooltip("Toggle drumkit edit mode");
    drumKitEditButton.onClick = [this]() { handleEditButtonClick(); };
    addAndMakeVisible(drumKitEditButton);
    
    // LEFT CHEVRON: Navigate to previous drumkit
    drumKitLeftChevron.setComponentID("drumkit_left_chevron");
    drumKitLeftChevron.setColorScheme(&colorScheme);
    drumKitLeftChevron.setTooltip("Previous drumkit");
    drumKitLeftChevron.onClick = [this]() { handleDrumKitChevrons(false); };
    addAndMakeVisible(drumKitLeftChevron);
    
    // RIGHT CHEVRON: Navigate to next drumkit
    drumKitRightChevron.setComponentID("drumkit_right_chevron");
    drumKitRightChevron.setColorScheme(&colorScheme);
    drumKitRightChevron.setTooltip("Next drumkit");
    drumKitRightChevron.onClick = [this]() { handleDrumKitChevrons(true); };
    addAndMakeVisible(drumKitRightChevron);
    
    // DRUMKIT DROPDOWN: Selection menu
    drumKitDropdown.setComponentID("drumkit_dropdown");
    drumKitDropdown.setTextWhenNothingSelected("Select DrumKit");
    drumKitDropdown.onChange = [this]() { handleDrumKitDropdownChange(); };
    drumKitDropdown.onPopupRequest = [this]() {
        drumKitDropdown.juce::ComboBox::showPopup();
    };
    populateDrumKitDropdown();
    addAndMakeVisible(drumKitDropdown);
    
    // DRUMKIT SELECTED LABEL: Shows current selection
    drumKitSelectedLabel.setComponentID("drumkit_selected_label");
    drumKitSelectedLabel.setText(currentDrumKitName, juce::dontSendNotification);
    drumKitSelectedLabel.setJustificationType(juce::Justification::centred);
    drumKitSelectedLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, 
                                                    INIConfig::LayoutConstants::fontSizePresetMenuReduced));
    drumKitSelectedLabel.setColour(juce::Label::textColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    drumKitSelectedLabel.setColour(juce::Label::backgroundColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    drumKitSelectedLabel.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    drumKitSelectedLabel.addMouseListener(this, false);
    addAndMakeVisible(drumKitSelectedLabel);
    
    // MIXER BUTTON: Open mixer interface
    drumKitMixerButton.setComponentID("drumkit_mixer_button");
    drumKitMixerButton.setColorScheme(&colorScheme);
    drumKitMixerButton.setTooltip("Open mixer");
    drumKitMixerButton.onClick = [this]() { handleMixerButtonClick(); };
    addAndMakeVisible(drumKitMixerButton);
    
    // MUTE BUTTON: Toggle mute state
    drumKitMuteButton.setComponentID("drumkit_mute_button");
    drumKitMuteButton.setColorScheme(&colorScheme);
    drumKitMuteButton.setTooltip("Toggle mute");
    drumKitMuteButton.onClick = [this]() { handleMuteButtonClick(); };
    addAndMakeVisible(drumKitMuteButton);
    
    // Initialize display state
    // updatePlayerNumberDisplay();  // Issue 3.1: Removed player number display
    updateMuteButtonState();
    updateDrumKitDisplayToggle();
}

void Row3Component::updateDrumKitLayout() {
    auto bounds = getLocalBounds();
    
    // Use responsive calculations instead of hardcoded values
    int buttonSize = getResponsiveButtonSize();
    int spacing = getResponsiveSpacing();
    int margin = getResponsiveMargin(10);
    
    int buttonY = (bounds.getHeight() - buttonSize) / 2;
    
    // Player number label - large display on the left
    // Issue 3.1: Remove Number "1" Display - Comment out player number layout
    // int playerNumWidth = static_cast<int>(buttonSize * 1.5f);
    // int playerNumHeight = buttonSize;
    // int playerNumX = margin;
    // 
    // playerNumberLabel.setBounds(playerNumX, buttonY, playerNumWidth, playerNumHeight);
    // 
    // // Update player number font size responsively
    // float playerFontSize = getResponsiveFontSize(24.0f);
    // playerNumberLabel.setFont(JUCE8_FONT(playerFontSize));
    
    // Edit button - now starts from left margin since player number is removed
    int editButtonX = margin;
    drumKitEditButton.setBounds(editButtonX, buttonY, buttonSize, buttonSize);
    
    // Left chevron
    int leftChevronX = editButtonX + buttonSize + spacing;
    drumKitLeftChevron.setBounds(leftChevronX, buttonY, buttonSize, buttonSize);
    
    // DrumKit dropdown and label (center area)
    int dropdownX = leftChevronX + buttonSize + spacing;
    int dropdownWidth = static_cast<int>(bounds.getWidth() * 0.25f); // 25% of width
    
    drumKitDropdown.setBounds(dropdownX, buttonY, dropdownWidth, buttonSize);
    drumKitSelectedLabel.setBounds(dropdownX, buttonY, dropdownWidth, buttonSize);
    
    // Update dropdown font size responsively
    float dropdownFontSize = getResponsiveFontSize(14.0f);
    // Note: setFont() removed from HierarchicalComboBox in JUCE 8 - font styling handled by LookAndFeel
    drumKitSelectedLabel.setFont(JUCE8_FONT(dropdownFontSize));
    
    // Right chevron
    int rightChevronX = dropdownX + dropdownWidth + spacing;
    drumKitRightChevron.setBounds(rightChevronX, buttonY, buttonSize, buttonSize);
    
    // Mixer button
    int mixerButtonX = rightChevronX + buttonSize + spacing;
    drumKitMixerButton.setBounds(mixerButtonX, buttonY, buttonSize, buttonSize);
    
    // Mute button
    int muteButtonX = mixerButtonX + buttonSize + spacing;
    drumKitMuteButton.setBounds(muteButtonX, buttonY, buttonSize, buttonSize);
}

void Row3Component::updatePlayerNumberDisplay() {
    // Issue 3.1: Remove Number "1" Display - Comment out player number display update
    // playerNumberLabel.setText(juce::String(currentPlayerIndex + 1), juce::dontSendNotification);
}

void Row3Component::updateMuteButtonState() {
    bool isCurrentPlayerMuted = mixer.isChannelMuted(currentPlayerIndex);
    drumKitMuteButton.setToggleState(isCurrentPlayerMuted, juce::dontSendNotification);
    drumKitMuteButton.setIconName(isCurrentPlayerMuted ? "mute" : "unmute");
}

void Row3Component::updateDrumKitDisplayToggle() {
    drumKitDropdown.setVisible(!showingDrumKitLabel);
    drumKitSelectedLabel.setVisible(showingDrumKitLabel);
}

void Row3Component::toggleDrumKitDisplay() {
    showingDrumKitLabel = !showingDrumKitLabel;
    updateDrumKitDisplayToggle();
}

void Row3Component::handleDrumKitChevrons(bool isRight) {
    // Issue 3.3 & 3.5: Left/Right Chevron Does Not Decrement/Increment Drumkit Menu
    // Get list of available drumkits from the dropdown
    juce::StringArray drumKitList;
    for (int i = 0; i < drumKitDropdown.getNumItems(); ++i) {
        drumKitList.add(drumKitDropdown.getItemText(i));
    }
    
    if (drumKitList.size() == 0) return; // No drumkits available
    
    int currentIndex = drumKitList.indexOf(currentDrumKitName);
    if (currentIndex == -1) currentIndex = 0; // Default to first if not found
    
    if (isRight) {
        // Navigate to next drumkit (wrap around to beginning)
        currentIndex = (currentIndex + 1) % drumKitList.size();
    } else {
        // Navigate to previous drumkit (wrap around to end)
        currentIndex = (currentIndex - 1 + drumKitList.size()) % drumKitList.size();
    }
    
    // Update the current drumkit name and UI
    setDrumKitName(drumKitList[currentIndex]);
    
    // Notify parent component
    if (onDrumKitNavigated) {
        onDrumKitNavigated(isRight);
    }
    
    // Trigger drumkit change callback if available
    if (onDrumKitChanged) {
        onDrumKitChanged(currentDrumKitName);
    }
}

void Row3Component::handleEditButtonClick() {
    editMode = !editMode;
    drumKitEditButton.setToggleState(editMode, juce::dontSendNotification);
    
    // Issue 3.2: Edit Icon Does Not Bring Up Drumkit Edit Window
    // Use callback to notify parent component to show edit window
    if (onEditModeChanged) {
        onEditModeChanged(editMode);
    }
    
    // Note: The actual DrumKitEditorWindow creation should be handled by the parent component
    // that has access to SFZEngine and INIDataManager dependencies
}

void Row3Component::handleMuteButtonClick() {
    bool newMuteState = !drumKitMuteButton.getToggleState();
    setMuteState(newMuteState);
    
    // Issue 3.7: Mute Icon Toggles But Doesn't Mute MIDI Signal - ALREADY IMPLEMENTED
    // Update mixer state to actually mute the audio signal
    mixer.setChannelMute(currentPlayerIndex, newMuteState);
    
    if (onMuteToggled) {
        onMuteToggled(newMuteState);
    }
}

void Row3Component::handleMixerButtonClick() {
    // Issue 3.6: Mixer Icon Does Not Bring Up Mixer Window
    // Use callback to notify parent component to show mixer window
    if (onMixerRequested) {
        onMixerRequested();
    }
    
    // Note: The actual DrumKitMixerWindow creation should be handled by the parent component
    // that has access to the required dependencies
}

void Row3Component::handleDrumKitDropdownChange() {
    int selectedId = drumKitDropdown.getSelectedId();
    if (selectedId > 0) {
        juce::String selectedName = drumKitDropdown.getItemText(selectedId - 1);
        setDrumKitName(selectedName);
        
        // Issue 3.4: Drumkit Menu Label Does Not Return When Item Selected
        // Always return to label view after selection
        showingDrumKitLabel = true;
        updateDrumKitDisplayToggle();
        
        if (onDrumKitChanged) {
            onDrumKitChanged(selectedName);
        }
    }
}

void Row3Component::populateDrumKitDropdown() {
    drumKitDropdown.clear();
    
    // Add sample drumkits for testing
    drumKitDropdown.addItem("808 Classic", 1);
    drumKitDropdown.addItem("Trap Kit", 2);
    drumKitDropdown.addItem("House Kit", 3);
    drumKitDropdown.addItem("Techno Kit", 4);
    drumKitDropdown.addItem("Hip Hop Kit", 5);
    
    // Set default selection
    drumKitDropdown.setSelectedId(1, juce::dontSendNotification);
}

//==============================================================================
// PHASE 9D: WaveformDisplay Integration Implementation
//==============================================================================

void Row3Component::setWaveformDisplay(WaveformDisplay* display) {
    waveformDisplay = display;
    if (waveformDisplay) {
        setupWaveformIntegration();
    }
}

void Row3Component::updateWaveformForCurrentSample() {
    if (!waveformDisplay) return;
    
    // Get current drum kit sample path based on current selection
    juce::String samplePath = getCurrentDrumKitSamplePath();
    if (samplePath.isNotEmpty()) {
        loadSampleIntoWaveform(samplePath);
    }
}

void Row3Component::showWaveformVisualization(bool show) {
    waveformVisible = show;
    if (waveformDisplay) {
        waveformDisplay->setVisible(show);
        if (show) {
            updateWaveformBounds();
            updateWaveformForCurrentSample();
        }
    }
}

bool Row3Component::isWaveformVisible() const {
    return waveformVisible && waveformDisplay && waveformDisplay->isVisible();
}

void Row3Component::optimizeForTouch() {
    // Increase button sizes for touch interfaces
    #if JUCE_IOS || JUCE_ANDROID
    auto bounds = getLocalBounds();
    
    // Ensure minimum touch target size (44px)
    int minTouchSize = static_cast<int>(bounds.getHeight() * INIConfig::LayoutConstants::MIN_TOUCH_TARGET_HEIGHT_PERCENT / 100.0f);
    
    // Apply touch optimizations to all buttons
    drumKitEditButton.setSize(juce::jmax(drumKitEditButton.getWidth(), minTouchSize),
                             juce::jmax(drumKitEditButton.getHeight(), minTouchSize));
    drumKitLeftChevron.setSize(juce::jmax(drumKitLeftChevron.getWidth(), minTouchSize),
                              juce::jmax(drumKitLeftChevron.getHeight(), minTouchSize));
    drumKitRightChevron.setSize(juce::jmax(drumKitRightChevron.getWidth(), minTouchSize),
                               juce::jmax(drumKitRightChevron.getHeight(), minTouchSize));
    drumKitMuteButton.setSize(juce::jmax(drumKitMuteButton.getWidth(), minTouchSize),
                             juce::jmax(drumKitMuteButton.getHeight(), minTouchSize));
    drumKitMixerButton.setSize(juce::jmax(drumKitMixerButton.getWidth(), minTouchSize),
                              juce::jmax(drumKitMixerButton.getHeight(), minTouchSize));
    #endif
}

//==============================================================================
// PHASE 9D: Visualization Helper Methods
//==============================================================================

void Row3Component::setupWaveformIntegration() {
    if (!waveformDisplay) return;
    
    // Configure waveform display for drum sample visualization
    WaveformDisplay::WaveformSettings waveformSettings;
    waveformSettings.backgroundColor = colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground);
    waveformSettings.waveformColor = colorScheme.getColor(ColorScheme::ColorRole::Accent);
    waveformSettings.gridColor = colorScheme.getColor(ColorScheme::ColorRole::GridLine);
    waveformSettings.showGrid = true;
    waveformSettings.showRuler = true;
    waveformSettings.showSelection = true;
    waveformSettings.showPlayhead = true;
    
    waveformDisplay->setWaveformSettings(waveformSettings);
    
    // Load current sample if available
    updateWaveformForCurrentSample();
}

void Row3Component::updateWaveformBounds() {
    if (!waveformDisplay) return;
    
    auto waveformArea = getWaveformArea();
    waveformDisplay->setBounds(waveformArea);
}

void Row3Component::loadSampleIntoWaveform(const juce::String& samplePath) {
    if (!waveformDisplay || samplePath.isEmpty()) return;
    
    // Load audio file into waveform display
    juce::File sampleFile(samplePath);
    if (sampleFile.existsAsFile()) {
        waveformDisplay->setAudioFile(sampleFile);
    }
}

juce::Rectangle<int> Row3Component::getWaveformArea() const {
    auto bounds = getLocalBounds();
    
    // Position waveform in right 40% of Row 3, using INI-driven calculations
    int waveformX = static_cast<int>(bounds.getWidth() * 0.6f);
    int waveformY = static_cast<int>(bounds.getHeight() * 0.1f);  // 10% margin from top
    int waveformWidth = static_cast<int>(bounds.getWidth() * 0.4f);
    int waveformHeight = static_cast<int>(bounds.getHeight() * 0.8f);  // 80% of row height
    
    return juce::Rectangle<int>(waveformX, waveformY, waveformWidth, waveformHeight);
}

juce::String Row3Component::getCurrentDrumKitSamplePath() const {
    // This would typically interface with the drum kit manager to get the current sample path
    // For now, return a placeholder path based on current drum kit name
    juce::String basePath = "/drumkits/" + currentDrumKitName.toLowerCase().replaceCharacter(' ', '_');
    return basePath + "/kick.wav";  // Default to kick sample
}

//==============================================================================
// ResponsiveComponent Implementation
//==============================================================================

void Row3Component::updateResponsiveLayout() {
    auto category = getCurrentDeviceCategory();
    
    // Device-specific adjustments for drumkit controls
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            // Mobile: Larger touch targets, simplified layout
            break;
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            // Tablet: Medium touch targets, balanced layout
            break;
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            // Desktop: Standard layout with mouse precision
            break;
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            // Large Desktop: Expanded layout, more information density
            break;
        default:
            break;
    }
    
    // Layout update will happen naturally through component hierarchy
    // Do not call resized() here as it creates infinite recursion
}

int Row3Component::getResponsiveButtonSize() const {
    auto category = getCurrentDeviceCategory();
    auto rules = getCurrentLayoutRules();
    
    // Base size from row height
    int baseSize = static_cast<int>(getHeight() * 0.7f); // 70% of row height
    
    // Apply device-specific adjustments
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            return juce::jmax(static_cast<int>(rules.sizing.minTouchTarget), baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            return juce::jmax(static_cast<int>(rules.sizing.minTouchTarget * 0.9f), baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            return juce::jmax(32, baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            return juce::jmax(36, static_cast<int>(baseSize * 1.1f));
        default:
            return juce::jmax(32, baseSize);
    }
}

int Row3Component::getResponsiveSpacing() const {
    auto category = getCurrentDeviceCategory();
    auto rules = getCurrentLayoutRules();
    
    // Base spacing
    int baseSpacing = rules.spacing.defaultSpacing;
    
    // Apply device-specific adjustments
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            return juce::jmax(8, baseSpacing);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            return juce::jmax(6, baseSpacing);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            return juce::jmax(4, baseSpacing);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            return juce::jmax(5, baseSpacing);
        default:
            return baseSpacing;
    }
}

float Row3Component::getResponsiveFontSize(float baseSize) const {
    return ResponsiveComponent::getResponsiveFontSize(baseSize);
}