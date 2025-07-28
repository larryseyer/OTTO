#include "MainContentComponent.h"
#include "MainContentComponentLeftSection.h"
#include "MainContentComponentRightSection.h"
#include "SceneLauncherComponent.h"
#include "LoopSectionComponent.h"
#include "INIConfig.h"
#include "INIDataManager.h"
#include "ErrorHandling.h"
#include "PerformanceOptimizations.h"

MainContentComponent::MainContentComponent(MidiEngine& midiEngine,
                                          Mixer& mixer,
                                          juce::AudioProcessorValueTreeState& valueTreeState,
                                          ResponsiveLayoutManager& layoutManager,
                                          FontManager& fontManager,
                                          ColorScheme& colorScheme)
    : midiEngine(midiEngine), mixer(mixer), valueTreeState(valueTreeState),
      layoutManager(layoutManager), fontManager(fontManager), colorScheme(colorScheme),
      // JUCE 8 - Initialize Row 3 components in initializer list
      drumKitEditButton("pencil", FontManager::PhosphorWeight::Regular),
      drumKitLeftChevron("caret-left", FontManager::PhosphorWeight::Regular),
      drumKitRightChevron("caret-right", FontManager::PhosphorWeight::Regular),
      drumKitMuteButton("speaker-slash", FontManager::PhosphorWeight::Regular),
      drumKitMixerButton("mixer", FontManager::PhosphorWeight::Regular),
      // JUCE 8 - Initialize Row 4 components in initializer list
      patternAddButton("plus", FontManager::PhosphorWeight::Regular),
      patternDeleteButton("trash", FontManager::PhosphorWeight::Regular),
      topSeparator(colorScheme), bottomSeparator(colorScheme) {

    leftSection = std::make_unique<MainContentComponentLeftSection>(
        midiEngine, layoutManager, fontManager, colorScheme);
    rightSection = std::make_unique<MainContentComponentRightSection>(
        midiEngine, mixer, valueTreeState, layoutManager, fontManager, colorScheme);
    loopSection = std::make_unique<LoopSectionComponent>(
        layoutManager, fontManager, colorScheme);

    addAndMakeVisible(rhythmLabel);
    addAndMakeVisible(playerNumber);
    addAndMakeVisible(*leftSection);
    addAndMakeVisible(*rightSection);
    addAndMakeVisible(*loopSection);
    addAndMakeVisible(topSeparator);
    addAndMakeVisible(bottomSeparator);
    
    // Setup Row 3 components
    setupRow3Components();
    
    // Setup Row 4 components
    setupRow4Components();

    rhythmLabel.setComponentID("player_label");
    rhythmLabel.setText("PLAYER", juce::dontSendNotification);
    rhythmLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    rhythmLabel.setJustificationType(juce::Justification::centredRight);

    // PHASE 3: Player number now positioned and styled in Row 3
    playerNumber.setComponentID("player_number");
    playerNumber.setText("1", juce::dontSendNotification); // Large number only - formatting changed
    playerNumber.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    playerNumber.setJustificationType(juce::Justification::centred); // Centered for large display

    leftSection->onEditModeChanged = [this](bool editMode) {
        if (onEditModeChanged) onEditModeChanged(editMode);
    };

    leftSection->onMidiFileChanged = [this](int buttonIndex, const juce::String& midiFile) {
        if (onMidiFileChanged) onMidiFileChanged(buttonIndex, midiFile);
    };

    leftSection->onGrooveAndFillsRequested = [this]() {
        if (onGrooveAndFillsRequested) onGrooveAndFillsRequested();
    };

    rightSection->onSliderValueChanged = [this](const juce::String& sliderId, float value) {
        if (onSliderValueChanged) onSliderValueChanged(sliderId, value);
    };
}

MainContentComponent::~MainContentComponent() {
}

void MainContentComponent::updatePlayerDisplay(int playerIndex) {
    currentPlayerIndex = INIConfig::clampPlayerIndex(playerIndex);
    updatePlayerNumberDisplay();
}

float MainContentComponent::getSwingValue() const {
    if (!rightSection) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
            "Right section is null, returning default swing value", "MainContentComponent");
        return INIConfig::Defaults::SWING;
    }
    return rightSection->getSwingValue();
}

void MainContentComponent::setSwingValue(float value) {
    if (!rightSection) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
            "Right section is null, cannot set swing value", "MainContentComponent");
        return;
    }
    
    try {
        float clampedValue = INIConfig::clampSwing(value);
        rightSection->setSwingValue(clampedValue);
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to set swing value: " + juce::String(e.what()), "MainContentComponent");
    }
}

float MainContentComponent::getEnergyValue() const {
    if (!rightSection) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
            "Right section is null, returning default energy value", "MainContentComponent");
        return INIConfig::Defaults::ENERGY;
    }
    return rightSection->getEnergyValue();
}

void MainContentComponent::setEnergyValue(float value) {
    if (!rightSection) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
            "Right section is null, cannot set energy value", "MainContentComponent");
        return;
    }
    
    try {
        float clampedValue = INIConfig::clampEnergy(value);
        rightSection->setEnergyValue(clampedValue);
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to set energy value: " + juce::String(e.what()), "MainContentComponent");
    }
}

float MainContentComponent::getVolumeValue() const {
    if (!rightSection) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
            "Right section is null, returning default volume value", "MainContentComponent");
        return INIConfig::Defaults::VOLUME;
    }
    return rightSection->getVolumeValue();
}

void MainContentComponent::setVolumeValue(float value) {
    if (!rightSection) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
            "Right section is null, cannot set volume value", "MainContentComponent");
        return;
    }
    
    try {
        float clampedValue = INIConfig::clampVolume(value);
        rightSection->setVolumeValue(clampedValue);
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to set volume value: " + juce::String(e.what()), "MainContentComponent");
    }
}

bool MainContentComponent::getEditMode() const {
    if (!leftSection) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
            "Left section is null, returning default edit mode", "MainContentComponent");
        return INIConfig::Defaults::DEFAULT_EDIT_MODE;
    }
    return leftSection->getEditMode();
}

void MainContentComponent::setEditModeVisuals(bool isEditing) {
    if (!leftSection) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
            "Left section is null, cannot set edit mode visuals", "MainContentComponent");
        return;
    }
    
    try {
        leftSection->setEditModeVisuals(isEditing);
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to set edit mode visuals: " + juce::String(e.what()), "MainContentComponent");
    }
}

juce::String MainContentComponent::getMidiFileAssignment(int buttonIndex) const {
    if (!leftSection || !INIConfig::isValidButtonIndex(buttonIndex)) {
        return "";
    }
    return leftSection->getMidiFileAssignment(buttonIndex);
}

void MainContentComponent::setMidiFileAssignment(int buttonIndex, const juce::String& midiFile) {
    if (leftSection && INIConfig::isValidButtonIndex(buttonIndex)) {
        leftSection->setMidiFileAssignment(buttonIndex, midiFile);
    }
}

void MainContentComponent::saveStates(ComponentState& state) {
    try {
        validatePlayerIndex(currentPlayerIndex);

        auto& playerState = state.playerSettings[currentPlayerIndex];

        playerState.swingValue = getSwingValue();
        playerState.energyValue = getEnergyValue();
        playerState.volumeValue = getVolumeValue();

        if (leftSection) {
            leftSection->saveStates(state);
        }

        if (rightSection) {
            rightSection->saveStates(state);
        }

        state.currentPlayer = currentPlayerIndex;

        notifyStateChanged(state);
    }
    catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to save component states: " + juce::String(e.what()), "MainContentComponent");
    }
}

void MainContentComponent::loadStates(const ComponentState& state) {
    try {
        int savedPlayerIndex = INIConfig::clampPlayerIndex(state.currentPlayer);

        if (savedPlayerIndex != currentPlayerIndex) {
            currentPlayerIndex = savedPlayerIndex;
            updatePlayerNumberDisplay();
        }

        validatePlayerIndex(currentPlayerIndex);

        const auto& playerState = state.playerSettings[currentPlayerIndex];

        setSwingValue(playerState.swingValue);
        setEnergyValue(playerState.energyValue);
        setVolumeValue(playerState.volumeValue);

        if (leftSection) {
            leftSection->loadPlayerSpecificState(currentPlayerIndex, state);
            leftSection->checkAndLoadDefaultPatterns(const_cast<ComponentState&>(state));
        }

        if (rightSection) {
            rightSection->loadStates(state);
        }

    }
    catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to load component states: " + juce::String(e.what()), "MainContentComponent");
    }
}

int MainContentComponent::getFavoritesSelection() const {
    return leftSection ? leftSection->getFavoritesSelection() : 0;
}

void MainContentComponent::setFavoritesSelection(int selection) {
    if (leftSection) leftSection->setFavoritesSelection(selection);
}

void MainContentComponent::paint(juce::Graphics& g) {
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
}

void MainContentComponent::resized() {
    auto bounds = getLocalBounds();

    // ========================================================================
    // PHASE 3: INI-DRIVEN ROW LAYOUT IMPLEMENTATION
    // ========================================================================
    // Use INI Row system constants for all positioning - no hardcoded values
    
    // Row 1: TopBar (existing header logic)
    int headerHeight = layoutManager.scaled(INIConfig::LayoutConstants::Row1::height);
    int rhythmLabelWidth = layoutManager.scaled(INIConfig::LayoutConstants::rhythmLabelWidth);

    // Position rhythm label in Row 1 (keep existing logic for now)
    rhythmLabel.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::rhythmLabelX),
                         layoutManager.scaled(INIConfig::LayoutConstants::rhythmLabelY),
                         rhythmLabelWidth,
                         layoutManager.scaled(INIConfig::LayoutConstants::rhythmLabelHeight));

    topSeparator.setBounds(0, headerHeight, bounds.getWidth(), 
                          layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness));
    
    // Row 2: PlayerTabs - PHASE 4 IMPLEMENTATION
    // Note: PlayerTabs component positioning should be handled by parent (PluginEditor)
    // MainContentComponent just needs to respect Row 2 space
    
    // Row 3: Player & DrumKit Controls (consolidated - COMPLETE)
    updateRow3Layout();
    
    // Row 4: Pattern Group Controls - PHASE 4 IMPLEMENTATION  
    updateRow4Layout();
    // Row 5: Pattern Matrix + Controls (main content) - PHASE 5 INI INTEGRATION
    updateRow5Layout();

    // Row 6: Loop Section - PHASE 5 INI INTEGRATION
    updateRow6Layout();
}

void MainContentComponent::updateFromState(const ComponentState& state) {
    try {
        loadStates(state);
    }
    catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to update from state: " + juce::String(e.what()), "MainContentComponent");
    }
}

void MainContentComponent::switchToPlayer(int playerIndex, const ComponentState& state) {
    try {
        validatePlayerIndex(playerIndex);

        if (playerIndex != currentPlayerIndex) {
            currentPlayerIndex = playerIndex;
            updatePlayerNumberDisplay();
        }

        const auto& playerState = state.playerSettings[playerIndex];

        setSwingValue(playerState.swingValue);
        setEnergyValue(playerState.energyValue);
        setVolumeValue(playerState.volumeValue);

        if (leftSection) {
            leftSection->loadPlayerSpecificState(playerIndex, state);
        }

        if (rightSection) {
            rightSection->updatePlayerState(playerIndex, state);
        }

    }
    catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to switch to player " + juce::String(playerIndex) + ": " + juce::String(e.what()), 
            "MainContentComponent");
    }
}

void MainContentComponent::validatePlayerIndex(int playerIndex) const {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) {
        throw std::invalid_argument("Invalid player index: " + std::to_string(playerIndex));
    }
}

void MainContentComponent::updatePlayerNumberDisplay() {
    // PHASE 3: Large player number display - just the number for prominent display
    playerNumber.setText(juce::String(currentPlayerIndex + 1), juce::dontSendNotification);
}

void MainContentComponent::notifyStateChanged(ComponentState& state) {
    if (onStateChanged) {
        onStateChanged(state);
    }
}

void MainContentComponent::setLivePerformanceMode(bool enabled) {
    livePerformanceMode = enabled;
    updateLayoutForPerformanceMode();
}

void MainContentComponent::showSceneLauncher(bool show) {
    if (show && !sceneLauncher) {
        sceneLauncher = std::make_unique<SceneLauncherComponent>(layoutManager, fontManager, colorScheme);
        sceneLauncher->setMidiEngine(&midiEngine);
        sceneLauncher->initialise();
        addAndMakeVisible(sceneLauncher.get());
    } else if (!show && sceneLauncher) {
        removeChildComponent(sceneLauncher.get());
        sceneLauncher.reset();
    }
    resized();
}

void MainContentComponent::updateLayoutForPerformanceMode() {
    resized();
}

void MainContentComponent::setupRow3Components() {
    // Row 3: Player & DrumKit Controls according to GUI_REORGANIZATION_TRACKER.md
    // [LARGE PLAYER #] [Edit] [<] [DrumKit ▼] [>] [Mute] [Mixer]
    // JUCE 8 - Configure already constructed components
    
    // Edit button for drumkit editing
    drumKitEditButton.setComponentID("drumkit_edit_button");
    drumKitEditButton.setColorScheme(&colorScheme);
    drumKitEditButton.addListener(this);
    addAndMakeVisible(drumKitEditButton);
    
    // Left chevron for previous drumkit
    drumKitLeftChevron.setComponentID("drumkit_left_chevron");
    drumKitLeftChevron.setColorScheme(&colorScheme);
    drumKitLeftChevron.addListener(this);
    addAndMakeVisible(drumKitLeftChevron);
    
    // DrumKit dropdown - this will trigger the popup
    drumKitDropdown.setComponentID("drumkit_dropdown");
    drumKitDropdown.addListener(this);
    drumKitDropdown.setTextWhenNothingSelected("Select DrumKit");
    addAndMakeVisible(drumKitDropdown);
    
    // Right chevron for next drumkit  
    drumKitRightChevron.setComponentID("drumkit_right_chevron");
    drumKitRightChevron.setColorScheme(&colorScheme);
    drumKitRightChevron.addListener(this);
    addAndMakeVisible(drumKitRightChevron);
    
    // Mute button
    drumKitMuteButton.setComponentID("drumkit_mute_button");
    drumKitMuteButton.setColorScheme(&colorScheme);
    drumKitMuteButton.addListener(this);
    addAndMakeVisible(drumKitMuteButton);
    
    // Mixer button for drumkit mixer popup
    drumKitMixerButton.setComponentID("drumkit_mixer_button");
    drumKitMixerButton.setColorScheme(&colorScheme);
    drumKitMixerButton.addListener(this);
    addAndMakeVisible(drumKitMixerButton);
}

void MainContentComponent::setupRow4Components() {
    // ========================================================================
    // PHASE 4: ROW 4 PATTERN GROUP CONTROLS SETUP
    // ========================================================================
    // Layout: [Group Label] [Pattern Group Dropdown] [Status Info] [Actions]
    // All components use INIConfig::LayoutConstants::Row4::* constants for positioning
    
    // Group label
    patternGroupLabel.setComponentID("pattern_group_label");
    patternGroupLabel.setText("Group", juce::dontSendNotification);
    patternGroupLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    patternGroupLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(patternGroupLabel);
    
    // Pattern group dropdown
    patternGroupDropdown.setComponentID("pattern_group_dropdown");
    patternGroupDropdown.addListener(this);
    patternGroupDropdown.setTextWhenNothingSelected("Select Pattern Group");
    // Add some default pattern groups
    patternGroupDropdown.addItem("Main Patterns", 1);
    patternGroupDropdown.addItem("Fill Patterns", 2);
    patternGroupDropdown.addItem("Intro Patterns", 3);
    patternGroupDropdown.addItem("Outro Patterns", 4);
    patternGroupDropdown.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(patternGroupDropdown);
    
    // Status display label
    patternStatusLabel.setComponentID("pattern_status_label");
    patternStatusLabel.setText("4/16 patterns", juce::dontSendNotification);
    patternStatusLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    patternStatusLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(patternStatusLabel);
    
    // Add pattern button
    patternAddButton.setComponentID("pattern_add_button");
    patternAddButton.setColorScheme(&colorScheme);
    patternAddButton.addListener(this);
    patternAddButton.setTooltip("Add new pattern to group");
    addAndMakeVisible(patternAddButton);
    
    // Delete pattern button
    patternDeleteButton.setComponentID("pattern_delete_button");
    patternDeleteButton.setColorScheme(&colorScheme);
    patternDeleteButton.addListener(this);
    patternDeleteButton.setTooltip("Delete selected pattern");
    addAndMakeVisible(patternDeleteButton);
}

void MainContentComponent::buttonClicked(juce::Button* button) {
    if (button == &drumKitEditButton) {
        // Handle drumkit edit
        if (onDrumKitPopupRequested) {
            onDrumKitPopupRequested();
        }
    }
    else if (button == &drumKitLeftChevron) {
        // Handle previous drumkit
        // TODO: Implement drumkit navigation
    }
    else if (button == &drumKitRightChevron) {
        // Handle next drumkit
        // TODO: Implement drumkit navigation
    }
    else if (button == &drumKitMuteButton) {
        // Handle drumkit mute
        // TODO: Implement drumkit mute
    }
    else if (button == &drumKitMixerButton) {
        // Handle drumkit mixer popup
        if (onDrumKitPopupRequested) {
            onDrumKitPopupRequested();
        }
    }
    // Row 4: Pattern Group Controls button handling
    else if (button == &patternAddButton) {
        // Handle add new pattern to current group
        // TODO: Implement pattern creation
    }
    else if (button == &patternDeleteButton) {
        // Handle delete selected pattern
        // TODO: Implement pattern deletion
    }
}

void MainContentComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    if (comboBoxThatHasChanged == &drumKitDropdown) {
        // DrumKit dropdown changed - trigger popup for selection
        if (onDrumKitPopupRequested) {
            onDrumKitPopupRequested();
        }
    }
    else if (comboBoxThatHasChanged == &patternGroupDropdown) {
        // Pattern group dropdown changed
        int selectedId = patternGroupDropdown.getSelectedId();
        // TODO: Implement pattern group switching logic
        // Update pattern status label based on selected group
        patternStatusLabel.setText(juce::String(selectedId) + "/16 patterns", juce::dontSendNotification);
    }
}

void MainContentComponent::updateRow3Layout() {
    // ========================================================================
    // PHASE 3: ROW 3 CONSOLIDATION - INI-DRIVEN LAYOUT
    // ========================================================================
    // Layout: [LARGE PLAYER #] [Edit] [<] [DrumKit ▼] [>] [Mute] [Mixer]
    // All positioning uses INIConfig::LayoutConstants::Row3::* constants
    
    // CONSOLIDATION: Position large player number in Row 3 with drumkit controls
    // This consolidates what was previously separate player number and drumkit rows
    
    // Large Player Number - positioned first in Row 3
    playerNumber.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::Row3::playerNumberX),
                          layoutManager.scaled(INIConfig::LayoutConstants::Row3::playerNumberY),
                          layoutManager.scaled(INIConfig::LayoutConstants::Row3::playerNumberWidth),
                          layoutManager.scaled(INIConfig::LayoutConstants::Row3::playerNumberHeight));
    
    // Set large font size for player number (using Brand role for prominent display)
    juce::Font playerFont = fontManager.getFont(FontManager::FontRole::Brand, 
                                               layoutManager.scaled(INIConfig::LayoutConstants::Row3::largePlayerFontSize));
    playerNumber.setFont(playerFont);
    
    // Edit button - positioned using INI constants
    drumKitEditButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::Row3::editButtonX),
                               layoutManager.scaled(INIConfig::LayoutConstants::Row3::editButtonY),
                               layoutManager.scaled(INIConfig::LayoutConstants::Row3::editButtonSize),
                               layoutManager.scaled(INIConfig::LayoutConstants::Row3::editButtonSize));
    
    // Left chevron - positioned using INI constants
    drumKitLeftChevron.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::Row3::leftChevronX),
                                layoutManager.scaled(INIConfig::LayoutConstants::Row3::leftChevronY),
                                layoutManager.scaled(INIConfig::LayoutConstants::Row3::leftChevronSize),
                                layoutManager.scaled(INIConfig::LayoutConstants::Row3::leftChevronSize));
    
    // DrumKit dropdown - positioned using INI constants with responsive width
    drumKitDropdown.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::Row3::dropdownX),
                             layoutManager.scaled(INIConfig::LayoutConstants::Row3::dropdownY),
                             layoutManager.scaled(INIConfig::LayoutConstants::Row3::dropdownWidth),
                             layoutManager.scaled(INIConfig::LayoutConstants::Row3::dropdownHeight));
    
    // Right chevron - positioned using INI constants
    drumKitRightChevron.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::Row3::rightChevronX),
                                 layoutManager.scaled(INIConfig::LayoutConstants::Row3::rightChevronY),
                                 layoutManager.scaled(INIConfig::LayoutConstants::Row3::rightChevronSize),
                                 layoutManager.scaled(INIConfig::LayoutConstants::Row3::rightChevronSize));
    
    // Mute button - positioned using INI constants
    drumKitMuteButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::Row3::muteButtonX),
                               layoutManager.scaled(INIConfig::LayoutConstants::Row3::muteButtonY),
                               layoutManager.scaled(INIConfig::LayoutConstants::Row3::muteButtonSize),
                               layoutManager.scaled(INIConfig::LayoutConstants::Row3::muteButtonSize));
    
    // Mixer button - positioned using INI constants
    drumKitMixerButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::Row3::mixerButtonX),
                                layoutManager.scaled(INIConfig::LayoutConstants::Row3::mixerButtonY),
                                layoutManager.scaled(INIConfig::LayoutConstants::Row3::mixerButtonSize),
                                layoutManager.scaled(INIConfig::LayoutConstants::Row3::mixerButtonSize));
    
    // ========================================================================
    // VALIDATION: Ensure all components fit within Row 3 bounds  
    // The INI constants include static assertions for this, but we can add runtime validation
    #ifdef JUCE_DEBUG
        int totalUsedWidth = layoutManager.scaled(INIConfig::LayoutConstants::Row3::totalUsedWidth);
        int availableWidth = getWidth();
        jassert(totalUsedWidth <= availableWidth); // Debug assertion for layout validation
    #endif
}

void MainContentComponent::updateRow4Layout() {
    // ========================================================================
    // PHASE 4: ROW 4 PATTERN GROUP CONTROLS - INI-DRIVEN LAYOUT
    // ========================================================================
    // Layout: [Group Label] [Pattern Group Dropdown] [Status Info] [Actions]
    // All positioning uses INIConfig::LayoutConstants::Row4::* constants
    
    // Group Label positioning - left aligned with standard margin
    patternGroupLabel.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::Row4::groupLabelX),
                               layoutManager.scaled(INIConfig::LayoutConstants::Row4::groupLabelY),
                               layoutManager.scaled(INIConfig::LayoutConstants::Row4::groupLabelWidth),
                               layoutManager.scaled(INIConfig::LayoutConstants::Row4::labelHeight));
    
    // Pattern Group Dropdown positioning - percentage-based responsive width
    patternGroupDropdown.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::Row4::dropdownX),
                                  layoutManager.scaled(INIConfig::LayoutConstants::Row4::dropdownY),
                                  layoutManager.scaled(INIConfig::LayoutConstants::Row4::dropdownWidth),
                                  layoutManager.scaled(INIConfig::LayoutConstants::Row4::dropdownHeight));
    
    // Status display positioning - shows pattern count information
    patternStatusLabel.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::Row4::statusX),
                                layoutManager.scaled(INIConfig::LayoutConstants::Row4::statusY),
                                layoutManager.scaled(INIConfig::LayoutConstants::Row4::statusWidth),
                                layoutManager.scaled(INIConfig::LayoutConstants::Row4::labelHeight));
    
    // Add button positioning - first action button
    patternAddButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::Row4::firstActionButtonX),
                              layoutManager.scaled(INIConfig::LayoutConstants::Row4::actionButtonY),
                              layoutManager.scaled(INIConfig::LayoutConstants::Row4::actionButtonWidth),
                              layoutManager.scaled(INIConfig::LayoutConstants::Row4::buttonHeight));
    
    // Delete button positioning - second action button
    patternDeleteButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::Row4::secondActionButtonX),
                                 layoutManager.scaled(INIConfig::LayoutConstants::Row4::actionButtonY),
                                 layoutManager.scaled(INIConfig::LayoutConstants::Row4::actionButtonWidth),
                                 layoutManager.scaled(INIConfig::LayoutConstants::Row4::buttonHeight));
    
    // ========================================================================
    // VALIDATION: Ensure all components fit within Row 4 bounds
    #ifdef JUCE_DEBUG
        int totalUsedWidth = layoutManager.scaled(INIConfig::LayoutConstants::Row4::totalUsedWidth);
        int availableWidth = getWidth();
        jassert(totalUsedWidth <= availableWidth); // Debug assertion for layout validation
    #endif
}

void MainContentComponent::updateRow5Layout() {
    // ========================================================================
    // PHASE 5: ROW 5 PATTERN MATRIX + CONTROLS - INI-DRIVEN LAYOUT
    // ========================================================================
    // Layout: [Pattern Matrix Section (65%)] [Controls Section (35%)]
    // All positioning uses INIConfig::LayoutConstants::Row5::* constants
    
    if (!leftSection || !rightSection) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
            "Left or right section is null, cannot update Row 5 layout", "MainContentComponent");
        return;
    }
    
    auto bounds = getLocalBounds();
    
    // Use responsive scaling for all dimensions
    int row5Y = layoutManager.scaled(INIConfig::LayoutConstants::Row5::yPosition);
    int row5Height = layoutManager.scaled(INIConfig::LayoutConstants::Row5::height);
    
    // Left Section: Pattern Matrix (65% width) - positioned using INI constants
    int leftSectionX = layoutManager.scaled(INIConfig::LayoutConstants::Row5::leftSectionX);
    int leftSectionWidth = layoutManager.scaled(INIConfig::LayoutConstants::Row5::leftSectionWidth);
    leftSection->setBounds(leftSectionX, row5Y, leftSectionWidth, row5Height);
    
    // Right Section: Controls (35% width) - positioned using INI constants
    int rightSectionX = layoutManager.scaled(INIConfig::LayoutConstants::Row5::rightSectionX);
    int rightSectionWidth = layoutManager.scaled(INIConfig::LayoutConstants::Row5::rightSectionWidth);
    rightSection->setBounds(rightSectionX, row5Y, rightSectionWidth, row5Height);
    
    // ========================================================================
    // VALIDATION: Ensure sections fit within bounds and don't overlap
    #ifdef JUCE_DEBUG
        int totalUsedWidth = layoutManager.scaled(INIConfig::LayoutConstants::Row5::totalUsedWidth);
        int availableWidth = bounds.getWidth();
        jassert(totalUsedWidth <= availableWidth); // Debug assertion for layout validation
        jassert(leftSectionX + leftSectionWidth <= rightSectionX); // Ensure no overlap
    #endif
}

void MainContentComponent::updateRow6Layout() {
    // ========================================================================
    // PHASE 5: ROW 6 LOOP SECTION - INI-DRIVEN LAYOUT
    // ========================================================================
    // Layout: [LOOP START] [================== Loop Slider ==================] [LOOP END]
    // All positioning uses INIConfig::LayoutConstants::Row6::* constants
    
    if (!loopSection) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
            "Loop section is null, cannot update Row 6 layout", "MainContentComponent");
        return;
    }
    
    auto bounds = getLocalBounds();
    
    // Position LoopSectionComponent in Row 6 using INI constants
    int row6Y = layoutManager.scaled(INIConfig::LayoutConstants::Row6::yPosition);
    int row6Height = layoutManager.scaled(INIConfig::LayoutConstants::Row6::height);
    loopSection->setBounds(0, row6Y, bounds.getWidth(), row6Height);
    
    // Keep bottom separator for visual consistency (positioned at the very bottom)
    int separatorY = row6Y + row6Height - layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness);
    bottomSeparator.setBounds(0, separatorY, bounds.getWidth(), 
                             layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness));
    
    // ========================================================================
    // VALIDATION: Ensure Row 6 fits within interface bounds
    #ifdef JUCE_DEBUG
        int totalRowHeight = row6Y + row6Height;
        int availableHeight = bounds.getHeight();
        jassert(totalRowHeight <= availableHeight); // Debug assertion for layout validation
    #endif
}
