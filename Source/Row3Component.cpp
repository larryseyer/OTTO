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

#include "Row3Component.h"
#include "MidiEngine.h"
#include "Mixer.h"
#include "INIConfig.h"
#include "ErrorHandling.h"

Row3Component::Row3Component(MidiEngine& midiEngine,
                           Mixer& mixer,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : RowComponentBase(3, layoutManager, fontManager, colorScheme),
      midiEngine(midiEngine), mixer(mixer),
      
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
    updateDrumKitLayout();
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
    updatePlayerNumberDisplay();
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
    RowComponentBase::lookAndFeelChanged();
    
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
    updatePlayerNumberDisplay();
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
    playerNumberLabel.setComponentID("player_number_label");
    playerNumberLabel.setJustificationType(juce::Justification::centred);
    playerNumberLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, 
                                                 INIConfig::LayoutConstants::Row3::largePlayerFontSize));
    playerNumberLabel.setColour(juce::Label::textColourId, 
                               colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    addAndMakeVisible(playerNumberLabel);
    
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
    updatePlayerNumberDisplay();
    updateMuteButtonState();
    updateDrumKitDisplayToggle();
}

void Row3Component::updateDrumKitLayout() {
    using namespace INIConfig::LayoutConstants::Row3;
    
    // Player number label
    playerNumberLabel.setBounds(playerNumberX, playerNumberY, 
                               playerNumberWidth, playerNumberHeight);
    
    // Edit button
    drumKitEditButton.setBounds(editButtonX, editButtonY, 
                               editButtonSize, editButtonSize);
    
    // Left chevron
    drumKitLeftChevron.setBounds(leftChevronX, leftChevronY, 
                                leftChevronSize, leftChevronSize);
    
    // DrumKit dropdown and label (toggle visibility based on state)
    drumKitDropdown.setBounds(dropdownX, dropdownY, 
                             dropdownWidth, dropdownHeight);
    drumKitSelectedLabel.setBounds(selectedLabelX, selectedLabelY, 
                                  selectedLabelWidth, selectedLabelHeight);
    
    // Right chevron
    drumKitRightChevron.setBounds(rightChevronX, rightChevronY, 
                                 rightChevronSize, rightChevronSize);
    
    // Mixer button
    drumKitMixerButton.setBounds(mixerButtonX, mixerButtonY, 
                                mixerButtonSize, mixerButtonSize);
    
    // Mute button
    drumKitMuteButton.setBounds(muteButtonX, muteButtonY, 
                               muteButtonSize, muteButtonSize);
}

void Row3Component::updatePlayerNumberDisplay() {
    playerNumberLabel.setText(juce::String(currentPlayerIndex + 1), juce::dontSendNotification);
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
    if (onDrumKitNavigated) {
        onDrumKitNavigated(isRight);
    }
}

void Row3Component::handleEditButtonClick() {
    editMode = !editMode;
    drumKitEditButton.setToggleState(editMode, juce::dontSendNotification);
    
    if (onEditModeChanged) {
        onEditModeChanged(editMode);
    }
}

void Row3Component::handleMuteButtonClick() {
    bool newMuteState = !drumKitMuteButton.getToggleState();
    setMuteState(newMuteState);
    
    // Update mixer state
    mixer.setChannelMute(currentPlayerIndex, newMuteState);
    
    if (onMuteToggled) {
        onMuteToggled(newMuteState);
    }
}

void Row3Component::handleMixerButtonClick() {
    if (onMixerRequested) {
        onMixerRequested();
    }
}

void Row3Component::handleDrumKitDropdownChange() {
    int selectedId = drumKitDropdown.getSelectedId();
    if (selectedId > 0) {
        juce::String selectedName = drumKitDropdown.getItemText(selectedId - 1);
        setDrumKitName(selectedName);
        
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