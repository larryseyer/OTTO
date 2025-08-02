/**
 * @file MainContentComponent.cpp
 * @brief Implementation of OTTO's main interface layout using responsive row-based design
 * 
 * This file implements the MainContentComponent class, which serves as the primary
 * container for OTTO's user interface. It implements a sophisticated 6-row layout
 * system based on percentage calculations from INIConfig, ensuring consistent
 * scaling across all screen sizes and DPI settings.
 * 
 * ROW-BASED LAYOUT ARCHITECTURE:
 * ===============================
 * Row 1 (10%): TopBarComponent - Settings, presets, transport controls
 * Row 2 (8%):  PlayerTabsComponent - Player selection tabs  
 * Row 3 (16%): DrumKit controls - Edit, navigation, kit selection
 * Row 4 (14%): Pattern management - Groups, labels, status
 * Row 5 (44%): Main content area - Drum grid, sliders, controls
 * Row 6 (8%):  Loop/transport section - Scene launcher, performance controls
 * 
 * RESPONSIVE DESIGN FEATURES:
 * ===========================
 * - INI-Driven Layout: All positioning via INIConfig::LayoutConstants
 * - Percentage-Based Scaling: Maintains proportions at any resolution
 * - Minimum Size Enforcement: Ensures 44px touch targets for accessibility
 * - Real-time Resizing: Components adjust immediately to window changes
 * - Cross-Platform Consistency: Identical layout on all supported platforms
 * 
 * COMPONENT INTEGRATION:
 * ======================
 * - ResponsiveLayoutManager.cpp: Provides scaling calculations and updates
 * - FontManager.cpp: Ensures consistent typography across all child components
 * - ColorScheme.cpp: Provides theme-consistent colors for all visual elements
 * - INIConfig.h: Sources all layout constants and default values
 * - MidiEngine.cpp: Integrates MIDI processing with UI controls
 * - Mixer.cpp: Connects audio processing with visual feedback
 * 
 * CHILD COMPONENT STRUCTURE:
 * ==========================
 * - MainContentComponentLeftSection: Drum pattern grid and sequence controls
 * - MainContentComponentRightSection: Parameter sliders and mix controls
 * - LoopSectionComponent: Transport and scene launching controls
 * - Various PhosphorIconButtons: Edit, navigation, and action controls
 * - SeparatorComponents: Visual row divisions using theme colors
 * 
 * @author OTTO Development Team
 * @version 2.0 
 * @date 2024
 * @see ResponsiveLayoutManager.cpp for scaling implementation
 * @see INIConfig.h for layout constants and row definitions
 * @see ColorScheme.cpp for theming integration
 */

#include "MainContentComponent.h"
#include "MainContentComponentLeftSection.h"
#include "MainContentComponentRightSection.h"
#include "SceneLauncherComponent.h"
#include "LoopSectionComponent.h"
#include "Mixer.h"
#include "INIConfig.h"
#include "INIDataManager.h"
#include "ErrorHandling.h"
#include "PerformanceOptimizations.h"
#include <iostream>

/**
 * @brief MainContentComponent constructor - initialize row-based layout with all subsystems
 * 
 * Sets up the complete main interface using JUCE 8 modern initialization patterns.
 * Creates all child components, establishes parent-child relationships, and configures
 * the responsive layout system for immediate scaling and theming support.
 * 
 * INITIALIZATION SEQUENCE:
 * 1. Store references to all required subsystems (MIDI, Mixer, etc.)
 * 2. Initialize Phosphor icon buttons with proper font weights
 * 3. Create child section components with shared system references
 * 4. Add all components to visibility and layout management
 * 5. Configure separators for visual row division
 * 6. Set up responsive layout integration
 * 
 * PHOSPHOR ICON INTEGRATION:
 * - All buttons use FontManager::PhosphorWeight::Regular for consistency
 * - Icon names correspond to official Phosphor icon library
 * - Buttons automatically scale with ResponsiveLayoutManager
 * 
 * CHILD COMPONENT CREATION:
 * - Unique pointers ensure proper memory management
 * - All child components receive shared system references
 * - Components are immediately added to layout management
 * 
 * @param midiEngine Reference to MIDI processing system
 * @param mixer Reference to audio mixing system  
 * @param valueTreeState JUCE parameter automation system
 * @param layoutManager Responsive layout calculation system
 * @param fontManager Typography and icon font management
 * @param colorScheme Theme and color management system
 * 
 * Called by: PluginEditor.cpp during interface construction
 * References: All child component constructors, INIConfig.h for constants
 */
MainContentComponent::MainContentComponent(MidiEngine& midiEngine,
                                          Mixer& mixer,
                                          juce::AudioProcessorValueTreeState& valueTreeState,
                                          ResponsiveLayoutManager& layoutManager,
                                          FontManager& fontManager,
                                          ColorScheme& colorScheme)
    : midiEngine(midiEngine), mixer(mixer), valueTreeState(valueTreeState),
      layoutManager(layoutManager), fontManager(fontManager), colorScheme(colorScheme),
      
      // ROW 3 COMPONENTS: DrumKit control buttons using Phosphor icon system
      // These buttons provide core drum kit navigation and editing functionality
      // All use Regular weight for consistent visual hierarchy
      drumKitEditButton("pencil", FontManager::PhosphorWeight::Regular),        // Toggle edit mode for pattern editing
      drumKitLeftChevron("caret-left", FontManager::PhosphorWeight::Regular),   // Navigate to previous drum kit
      drumKitRightChevron("caret-right", FontManager::PhosphorWeight::Regular), // Navigate to next drum kit
      drumKitMuteButton("unmute", FontManager::PhosphorWeight::Regular), // Mute/unmute current player
      drumKitMixerButton("mixer", FontManager::PhosphorWeight::Regular),        // Open mixer window
      
      // ROW 4 COMPONENTS: Pattern management buttons for add/remove operations
      // Provide quick access to pattern creation and deletion functions
      patternAddButton("plus", FontManager::PhosphorWeight::Regular),          // Create new pattern
      patternDeleteButton("trash", FontManager::PhosphorWeight::Regular),      // Delete selected pattern
      
      // ROW SEPARATORS: Visual dividers between each interface row
      // Use ColorScheme for theme-consistent styling, positioned between rows 1-6
      row1Separator(colorScheme), row2Separator(colorScheme), row3Separator(colorScheme), 
      row4Separator(colorScheme), row5Separator(colorScheme) {

    // CHILD COMPONENT CREATION: Initialize major layout sections
    // Each section receives references to required subsystems for full functionality
    
    // LEFT SECTION: Houses drum pattern grid and sequence controls
    // Manages 4x4 drum button matrix and pattern sequencing interface
    leftSection = std::make_unique<MainContentComponentLeftSection>(
        midiEngine, layoutManager, fontManager, colorScheme);
    
    // RIGHT SECTION: Contains parameter sliders and mixing controls  
    // Provides swing, energy, volume controls and effects processing
    rightSection = std::make_unique<MainContentComponentRightSection>(
        midiEngine, mixer, valueTreeState, layoutManager, fontManager, colorScheme);
    
    // LOOP SECTION: Transport controls and scene launching functionality
    // Handles loop position, scene management, and performance controls
    loopSection = std::make_unique<LoopSectionComponent>(
        layoutManager, fontManager, colorScheme);

    // COMPONENT VISIBILITY SETUP: Add all components to JUCE's component hierarchy
    // This establishes parent-child relationships and enables automatic rendering
    
    // Row 3 labels and player number display
    addAndMakeVisible(rhythmLabel);     // "Rhythm" label for pattern identification

    
    // Major layout sections - these contain most of the interface
    addAndMakeVisible(*leftSection);    // Drum grid and pattern controls
    addAndMakeVisible(*rightSection);   // Parameter sliders and mix controls  
    addAndMakeVisible(*loopSection);    // Transport and scene launcher
    
    // ROW SEPARATORS: Visual dividers for clear section definition
    // Positioned between rows using INIConfig::LayoutConstants positioning
    addAndMakeVisible(row1Separator);   // Between TopBar and PlayerTabs
    addAndMakeVisible(row2Separator);   // Between PlayerTabs and DrumKit controls
    addAndMakeVisible(row3Separator);   // Between DrumKit and Pattern controls
    addAndMakeVisible(row4Separator);   // Between Pattern and Main content
    addAndMakeVisible(row5Separator);   // Between Main content and Loop section
    
    // TEMPORARY: Add row identification labels for debugging
    addAndMakeVisible(rowLabel1);
    addAndMakeVisible(rowLabel2);
    // addAndMakeVisible(rowLabel3); // Removed debug label
    addAndMakeVisible(rowLabel4);
    addAndMakeVisible(rowLabel5);
    addAndMakeVisible(rowLabel6);
    
    // Setup Row 3 components
    setupRow3Components();
    
    // Setup Row 4 components
    setupRow4Components();

    rhythmLabel.setComponentID("player_label");
    rhythmLabel.setText("PLAYER", juce::dontSendNotification);
    rhythmLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    rhythmLabel.setJustificationType(juce::Justification::centredRight);
    rhythmLabel.setVisible(false); // Hide the PLAYER label - not needed in Row 3

    // PHASE 3: Player number now positioned and styled in Row 3




    
    // TEMPORARY: Setup row identification labels with red text using FontManager
    auto setupRowLabel = [&](juce::Label& label, const juce::String& text) {
        label.setText(text, juce::dontSendNotification);
        label.setColour(juce::Label::textColourId, juce::Colours::red);
        label.setColour(juce::Label::backgroundColourId, juce::Colours::yellow); // Yellow background for visibility
        label.setJustificationType(juce::Justification::centred);
        label.setFont(fontManager.getFont(FontManager::FontRole::Header, 32.0f)); // Even bigger font
    };
    
    setupRowLabel(rowLabel1, "ROW 1");
    setupRowLabel(rowLabel2, "ROW 2");
    // setupRowLabel(rowLabel3, "ROW 3"); // Removed debug label
    setupRowLabel(rowLabel4, "ROW 4");
    setupRowLabel(rowLabel5, "ROW 5");
    setupRowLabel(rowLabel6, "ROW 6");

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
    
    // Update mute button state and icon to reflect current player's mute status
    bool isCurrentPlayerMuted = mixer.isChannelMuted(currentPlayerIndex);
    drumKitMuteButton.setToggleState(isCurrentPlayerMuted, juce::dontSendNotification);
    drumKitMuteButton.setIconName(isCurrentPlayerMuted ? "mute" : "unmute");

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
    // ========================================================================
    // PROMPT 6: FINAL INTEGRATION - COMPLETE INI-DRIVEN 6-ROW LAYOUT SYSTEM
    // ========================================================================
    // All positioning uses consistent namespace constants with comprehensive validation
    // Complete responsive layout system with performance optimization
    
    using namespace INIConfig::LayoutConstants;
    
    auto bounds = getLocalBounds();
    
    // ========================================================================
    // ROW 1: TopBar Header - Enhanced Integration
    // ========================================================================
    // Position rhythm label in Row 1 using proper Row1 constants with consistent namespace
    rhythmLabel.setBounds(layoutManager.scaled(rhythmLabelX),
                         layoutManager.scaled(Row1::yPosition + defaultPadding),
                         layoutManager.scaled(rhythmLabelWidth),
                         layoutManager.scaled(rhythmLabelHeight));

    // ========================================================================
    // ROW SEPARATORS: Enhanced 5-Separator System with Consistent Namespace
    // ========================================================================
    // Position EXACTLY 5 row separators (between rows 1-2, 2-3, 3-4, 4-5, 5-6)
    // Row 6 is the FINAL row and has no separator after it
    
    int scaledSeparatorThickness = layoutManager.scaled(separatorThickness);
    
    // MainContentComponent scope: Handle separators for rows 3-6 only
    // Row 1-2 separators managed by parent PluginEditor for proper layering
    row1Separator.setVisible(false);  // TopBar/PlayerTabs separator (PluginEditor scope)
    row2Separator.setVisible(false);  // PlayerTabs/MainContent separator (PluginEditor scope)
    
    // Calculate relative positions within MainContentComponent coordinate system
    int mainContentOffset = layoutManager.scaled(ROW_3_Y);
    
    // Row bottom positions using consistent namespace constants
    int row3Bottom = (layoutManager.scaled(ROW_3_Y + ROW_3_HEIGHT)) - mainContentOffset;
    int row4Bottom = (layoutManager.scaled(ROW_4_Y + ROW_4_HEIGHT)) - mainContentOffset;
    int row5Bottom = (layoutManager.scaled(ROW_5_Y + ROW_5_HEIGHT)) - mainContentOffset;
    
    // Position MainContentComponent's 3 separators with enhanced documentation
    row3Separator.setBounds(0, row3Bottom, bounds.getWidth(), scaledSeparatorThickness);  // Row3/Row4 boundary
    row4Separator.setBounds(0, row4Bottom, bounds.getWidth(), scaledSeparatorThickness);  // Row4/Row5 boundary  
    row5Separator.setBounds(0, row5Bottom, bounds.getWidth(), scaledSeparatorThickness);  // Row5/Row6 boundary
    
    // ========================================================================
    // DEBUG LABELS: Row Identification System (Temporary - Remove in Production)
    // ========================================================================
    // Visual validation that all 6 rows align with INI-defined positions
    // Critical for debugging coordinate system and responsive scaling
    
    constexpr int rowLabelWidth = 120;   // Debug label width
    constexpr int rowLabelHeight = 50;   // Debug label height  
    int rowLabelX = bounds.getWidth() - rowLabelWidth - 20;  // Right-side positioning
    
    // MainContentComponent scope: Show only Rows 3-6 labels
    // Rows 1-2 are managed by parent PluginEditor component
    rowLabel1.setVisible(false);  // TopBar row (PluginEditor scope)
    rowLabel2.setVisible(false);  // PlayerTabs row (PluginEditor scope)
    
    // Position Row 3-6 debug labels using consistent namespace constants
    rowLabel3.setBounds(rowLabelX, layoutManager.scaled(ROW_3_Y) - mainContentOffset, 
                       rowLabelWidth, rowLabelHeight);
    rowLabel4.setBounds(rowLabelX, layoutManager.scaled(ROW_4_Y) - mainContentOffset, 
                       rowLabelWidth, rowLabelHeight);
    rowLabel5.setBounds(rowLabelX, layoutManager.scaled(ROW_5_Y) - mainContentOffset, 
                       rowLabelWidth, rowLabelHeight);
    rowLabel6.setBounds(rowLabelX, layoutManager.scaled(ROW_6_Y) - mainContentOffset, 
                       rowLabelWidth, rowLabelHeight);
    
    // ========================================================================
    // COMPLETE 6-ROW SYSTEM: Final Integration Update Calls
    // ========================================================================
    // All rows now use consistent namespace and enhanced validation systems
    
    // Row 2: PlayerTabs (managed by parent PluginEditor - out of MainContentComponent scope)
    // Note: PlayerTabs positioning handled by parent for proper component layering
    
    // Row 3: Player & DrumKit Controls - ENHANCED INI Integration ✅

    updateRow3Layout();
    
    // Row 4: Pattern Group Controls - ENHANCED INI Integration ✅  
    updateRow4Layout();
    
    // Row 5: 4x4 Pattern Matrix + Controls - ENHANCED INI Integration ✅
    updateRow5Layout();

    // Row 6: Loop Timeline Controls - ENHANCED INI Integration ✅
    updateRow6Layout();
    
    // ========================================================================
    // FINAL INTEGRATION VALIDATION: Complete System Health Check
    // ========================================================================
    #ifdef JUCE_DEBUG
        performIntegrationValidation(bounds);
    #endif
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
    // Add sample drumkits for testing
    drumKitDropdown.addItem("808 Classic", 1);
    drumKitDropdown.addItem("Trap Kit", 2);
    drumKitDropdown.addItem("House Kit", 3);
    drumKitDropdown.addItem("Techno Kit", 4);
    drumKitDropdown.addItem("Hip Hop Kit", 5);
    drumKitDropdown.setSelectedId(1, juce::dontSendNotification); // Default selection
    
    // Apply Playfair Display font to dropdown using JUCE 8 patterns and CustomLookAndFeel
    drumKitDropdown.setLookAndFeel(&getLookAndFeel()); // Ensures CustomLookAndFeel::getComboBoxFont is used
    drumKitDropdown.setColour(juce::ComboBox::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    drumKitDropdown.setColour(juce::ComboBox::backgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    
    addAndMakeVisible(drumKitDropdown);
    
    // Set onPopupRequest callback to enable HierarchicalComboBox popup functionality
    drumKitDropdown.onPopupRequest = [this]() {
        drumKitDropdown.juce::ComboBox::showPopup();
    };
    
    // DrumKit selected label - shows current selection with Playfair Display (following preset pattern)
    drumKitSelectedLabel.setComponentID("drumkit_selected_label");
    drumKitSelectedLabel.setText("808 Classic", juce::dontSendNotification); // Default to match dropdown
    drumKitSelectedLabel.setJustificationType(juce::Justification::centred);
    // Use same font as preset menu via FontManager
    drumKitSelectedLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, INIConfig::LayoutConstants::fontSizePresetMenuReduced));
    drumKitSelectedLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    drumKitSelectedLabel.setColour(juce::Label::backgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    
    // Make drumkit label clickable (following preset pattern)
    drumKitSelectedLabel.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    drumKitSelectedLabel.addMouseListener(this, false);
    
    addAndMakeVisible(drumKitSelectedLabel);
    
    // Initialize drumkit display toggle state (following preset pattern)
    updateDrumKitDisplayToggle();
    
    // Right chevron for next drumkit  
    drumKitRightChevron.setComponentID("drumkit_right_chevron");
    drumKitRightChevron.setColorScheme(&colorScheme);
    drumKitRightChevron.addListener(this);
    addAndMakeVisible(drumKitRightChevron);
    
    // Mute button
    drumKitMuteButton.setComponentID("drumkit_mute_button");
    drumKitMuteButton.setColorScheme(&colorScheme);
    drumKitMuteButton.addListener(this);
    // Initialize mute button state and icon based on current player's mute status
    bool isCurrentPlayerMuted = mixer.isChannelMuted(currentPlayerIndex);
    drumKitMuteButton.setToggleState(isCurrentPlayerMuted, juce::dontSendNotification);
    drumKitMuteButton.setIconName(isCurrentPlayerMuted ? "mute" : "unmute");
    addAndMakeVisible(drumKitMuteButton);
    
    // Mixer button for drumkit mixer popup
    drumKitMixerButton.setComponentID("drumkit_mixer_button");
    drumKitMixerButton.setColorScheme(&colorScheme);
    drumKitMixerButton.addListener(this);
    addAndMakeVisible(drumKitMixerButton);
}

void MainContentComponent::setupRow4Components() {
    // ========================================================================
    // PHASE 4: ROW 4 PATTERN GROUP CONTROLS SETUP - ENHANCED INI INTEGRATION
    // ========================================================================
    // Layout: [Group Label] [Pattern Group Dropdown] [Status Info] [Actions]
    // All components use INIConfig::LayoutConstants::Row4:: constants for positioning
    // Complete ColorScheme integration with proper FontManager usage
    
    // Group label - uses FontManager for consistent typography
    patternGroupLabel.setComponentID("pattern_group_label");
    patternGroupLabel.setText("Group", juce::dontSendNotification);
    patternGroupLabel.setColour(juce::Label::textColourId, 
                               colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    patternGroupLabel.setColour(juce::Label::backgroundColourId, 
                               colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    patternGroupLabel.setJustificationType(juce::Justification::centredRight);
    patternGroupLabel.setFont(fontManager.getFont(FontManager::FontRole::Body, 14.0f));
    addAndMakeVisible(patternGroupLabel);
    
    // Pattern group dropdown - complete ColorScheme integration
    patternGroupDropdown.setComponentID("pattern_group_dropdown");
    patternGroupDropdown.addListener(this);
    patternGroupDropdown.setTextWhenNothingSelected("Select Pattern Group");
    
    // Enhanced ColorScheme integration for dropdown
    patternGroupDropdown.setColour(juce::ComboBox::backgroundColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    patternGroupDropdown.setColour(juce::ComboBox::textColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    patternGroupDropdown.setColour(juce::ComboBox::arrowColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    patternGroupDropdown.setColour(juce::ComboBox::buttonColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
    patternGroupDropdown.setColour(juce::ComboBox::outlineColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::Separator));
    
    // Add pattern groups with proper hierarchy
    patternGroupDropdown.addItem("Main Patterns", 1);
    patternGroupDropdown.addItem("Fill Patterns", 2);
    patternGroupDropdown.addItem("Intro Patterns", 3);
    patternGroupDropdown.addItem("Outro Patterns", 4);
    patternGroupDropdown.addItem("Groove Patterns", 5);
    patternGroupDropdown.addItem("Custom Patterns", 6);
    patternGroupDropdown.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(patternGroupDropdown);
    
    // Status display label - enhanced typography and ColorScheme
    patternStatusLabel.setComponentID("pattern_status_label");
    patternStatusLabel.setText("4/16 patterns", juce::dontSendNotification);
    patternStatusLabel.setColour(juce::Label::textColourId, 
                                colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    patternStatusLabel.setColour(juce::Label::backgroundColourId, 
                                colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    patternStatusLabel.setJustificationType(juce::Justification::centredLeft);
    patternStatusLabel.setFont(fontManager.getFont(FontManager::FontRole::Body, 12.0f));
    addAndMakeVisible(patternStatusLabel);
    
    // Add pattern button - enhanced with proper tooltip and ColorScheme
    patternAddButton.setComponentID("pattern_add_button");
    patternAddButton.setColorScheme(&colorScheme);
    patternAddButton.addListener(this);
    patternAddButton.setTooltip("Add new pattern to selected group");
    addAndMakeVisible(patternAddButton);
    
    // Delete pattern button - enhanced with proper tooltip and ColorScheme
    patternDeleteButton.setComponentID("pattern_delete_button");
    patternDeleteButton.setColorScheme(&colorScheme);
    patternDeleteButton.addListener(this);
    patternDeleteButton.setTooltip("Delete selected pattern from group");
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
        handleDrumKitChevrons(false);
    }
    else if (button == &drumKitRightChevron) {
        // Handle next drumkit
        handleDrumKitChevrons(true);
    }
    else if (button == &drumKitMuteButton) {
        // Handle drumkit mute
        // Handle player mute toggle - mute/unmute the currently selected player
        bool currentMuteState = mixer.isChannelMuted(currentPlayerIndex);
        bool newMuteState = !currentMuteState;
        
        // Update mixer state
        mixer.setChannelMute(currentPlayerIndex, newMuteState);
        
        // Update button visual state and icon
        drumKitMuteButton.setToggleState(newMuteState, juce::dontSendNotification);
        drumKitMuteButton.setIconName(newMuteState ? "mute" : "unmute");
        
        DBG("Player " << (currentPlayerIndex + 1) << " mute toggled: " << (newMuteState ? "MUTED" : "UNMUTED"));
    }
    else if (button == &drumKitMixerButton) {
        // Handle drumkit mixer popup
        if (onMixerPopupRequested) {
            onMixerPopupRequested();
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
        // Handle normal dropdown selection
        int selectedId = drumKitDropdown.getSelectedId();
        juce::String selectedText = drumKitDropdown.getText();
        // Update the selected label with Playfair Display font
        drumKitSelectedLabel.setText(selectedText, juce::dontSendNotification);
        drumKitSelectedLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, 16.0f)); // Ensure Playfair Display
        
        DBG("DrumKit selected: " << selectedText << " (ID: " << selectedId << ")");
        
        // Hide menu after selection (following preset pattern)
        showDrumKitLabel();
        
        // Notify parent component of drumkit change
        if (onDrumKitChanged) {
            onDrumKitChanged(selectedText);
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
    
    using namespace INIConfig::LayoutConstants;
    
    // CONSOLIDATION: Position large player number in Row 3 with drumkit controls
    // This consolidates what was previously separate player number and drumkit rows
    
    // Large Player Number - positioned first in Row 3
// MOVED TO ROW 2:     playerNumber.setBounds(layoutManager.scaled(Row3::playerNumberX),
// MOVED TO ROW 2:                           layoutManager.scaled(Row3::playerNumberY),
// MOVED TO ROW 2:                           layoutManager.scaled(Row3::playerNumberWidth),
// MOVED TO ROW 2:                           layoutManager.scaled(Row3::playerNumberHeight));
// MOVED TO ROW 2:     
// MOVED TO ROW 2:     // Set large font size for player number (using Brand role for prominent display)
// MOVED TO ROW 2:     juce::Font playerFont = fontManager.getFont(FontManager::FontRole::Brand, 
// MOVED TO ROW 2:                                                layoutManager.scaled(Row3::largePlayerFontSize));
// MOVED TO ROW 2:     playerNumber.setFont(playerFont);
    
    // Edit button - positioned using INI constants
    drumKitEditButton.setBounds(layoutManager.scaled(defaultMargin),
                               layoutManager.scaled(defaultPadding + ((Row3::contentHeight - Row3::buttonSize) / 2)),
                               layoutManager.scaled(Row3::editButtonSize),
                               layoutManager.scaled(Row3::editButtonSize));
    
    // Left chevron - positioned using INI constants
    // CENTER THE CHEVRON-DROPDOWN-CHEVRON TRIO
    const int buttonY = layoutManager.scaled(defaultPadding + ((Row3::contentHeight - Row3::buttonSize) / 2));
    const int chevronSize = layoutManager.scaled(Row3::leftChevronSize);
    const int dropdownWidth = layoutManager.scaled(300); // Fixed width for dropdown
    const int dropdownHeight = layoutManager.scaled(Row3::dropdownHeight);
    const int spacing = layoutManager.scaled(8); // Spacing between elements
    
    // Calculate total width of the trio
    const int trioWidth = chevronSize + spacing + dropdownWidth + spacing + chevronSize;
    const int startX = (getWidth() - trioWidth) / 2; // Center horizontally
    
    // Left chevron - centered trio positioning
    drumKitLeftChevron.setBounds(startX,
                                buttonY,
                                chevronSize,
                                chevronSize);
    
    // DrumKit dropdown - centered trio positioning
    drumKitDropdown.setBounds(startX + chevronSize + spacing,
                             layoutManager.scaled(defaultPadding + ((Row3::contentHeight - Row3::dropdownHeight) / 2)),
                             dropdownWidth,
                             dropdownHeight);
    
    // Right chevron - centered trio positioning
    drumKitRightChevron.setBounds(startX + chevronSize + spacing + dropdownWidth + spacing,
                                 buttonY,
                                 chevronSize,
                                 chevronSize);
    
    // Mute button - positioned using INI constants with coordinate system adjustment
    // Note: Row3 Y positions are absolute from interface top, subtract ROW_3_Y for MainContentComponent coordinates
    drumKitMuteButton.setBounds(layoutManager.scaled(Row3::muteButtonX),
                               layoutManager.scaled(Row3::muteButtonY) - layoutManager.scaled(ROW_3_Y),
                               layoutManager.scaled(Row3::muteButtonSize),
                               layoutManager.scaled(Row3::muteButtonSize));
    // Mixer button - positioned using INI constants (rightmost) with coordinate system adjustment
    drumKitMixerButton.setBounds(layoutManager.scaled(Row3::mixerButtonX),
                                layoutManager.scaled(Row3::mixerButtonY) - layoutManager.scaled(ROW_3_Y),
                                layoutManager.scaled(Row3::mixerButtonSize),
                                layoutManager.scaled(Row3::mixerButtonSize));
    
    // DrumKit selected label - positioned using IDENTICAL bounds as dropdown (following preset pattern)
    drumKitSelectedLabel.setBounds(startX + chevronSize + spacing,
                                  layoutManager.scaled(defaultPadding + ((Row3::contentHeight - Row3::dropdownHeight) / 2)),
                                  dropdownWidth,
                                  dropdownHeight);
    
    // ========================================================================
    // VALIDATION: Ensure all components fit within Row 3 bounds  
    // The INI constants include static assertions for this, but we can add runtime validation
    #ifdef JUCE_DEBUG
        int totalUsedWidth = layoutManager.scaled(Row3::totalUsedWidth);
        int availableWidth = getWidth();
        jassert(totalUsedWidth <= availableWidth); // Debug assertion for layout validation
    #endif
}

void MainContentComponent::updateRow4Layout() {
    // ========================================================================
    // PHASE 4: ROW 4 PATTERN GROUP CONTROLS - ENHANCED INI-DRIVEN LAYOUT
    // ========================================================================
    // Layout: [Group Label] [Pattern Group Dropdown] [Status Info] [Actions]
    // All positioning uses Row4:: namespace constants with consistent formatting
    
    using namespace INIConfig::LayoutConstants;
    
    // Group Label positioning - left aligned with standard margin and FontManager
    patternGroupLabel.setBounds(layoutManager.scaled(Row4::groupLabelX),
                               layoutManager.scaled(Row4::yPosition + Row4::groupLabelY),
                               layoutManager.scaled(Row4::groupLabelWidth),
                               layoutManager.scaled(Row4::labelHeight));
    
    // Apply consistent FontManager sizing for label
    patternGroupLabel.setFont(fontManager.getFont(FontManager::FontRole::Body, 
                                                 layoutManager.scaled(14.0f)));
    
    // Pattern Group Dropdown positioning - percentage-based responsive width
    patternGroupDropdown.setBounds(layoutManager.scaled(Row4::dropdownX),
                                  layoutManager.scaled(Row4::yPosition + Row4::dropdownY),
                                  layoutManager.scaled(Row4::dropdownWidth),
                                  layoutManager.scaled(Row4::dropdownHeight));
    
    // Status display positioning - shows pattern count information with FontManager
    patternStatusLabel.setBounds(layoutManager.scaled(Row4::statusX),
                                layoutManager.scaled(Row4::yPosition + Row4::statusY),
                                layoutManager.scaled(Row4::statusWidth),
                                layoutManager.scaled(Row4::labelHeight));
    
    // Apply consistent FontManager sizing for status
    patternStatusLabel.setFont(fontManager.getFont(FontManager::FontRole::Body, 
                                                  layoutManager.scaled(12.0f)));
    
    // Add button positioning - first action button with percentage-based sizing
    patternAddButton.setBounds(layoutManager.scaled(Row4::firstActionButtonX),
                              layoutManager.scaled(Row4::yPosition + Row4::actionButtonY),
                              layoutManager.scaled(Row4::actionButtonWidth),
                              layoutManager.scaled(Row4::buttonHeight));
    
    // Delete button positioning - second action button with percentage-based sizing
    patternDeleteButton.setBounds(layoutManager.scaled(Row4::secondActionButtonX),
                                 layoutManager.scaled(Row4::yPosition + Row4::actionButtonY),
                                 layoutManager.scaled(Row4::actionButtonWidth),
                                 layoutManager.scaled(Row4::buttonHeight));
    
    // ========================================================================
    // VALIDATION: Ensure all components fit within Row 4 bounds
    // INI static assertions ensure layout validation at compile time
    #ifdef JUCE_DEBUG
        int totalUsedWidth = layoutManager.scaled(Row4::totalUsedWidth);
        int availableWidth = getWidth();
        jassert(totalUsedWidth <= availableWidth); // Runtime validation for dynamic sizing
        
        // Additional touch target validation
        jassert(layoutManager.scaled(Row4::buttonHeight) >= 44); // Min touch target
        jassert(layoutManager.scaled(Row4::dropdownHeight) >= 44); // Min touch target
    #endif
}

void MainContentComponent::updateRow5Layout() {
    // ========================================================================
    // PHASE 5: ROW 5 PATTERN MATRIX + CONTROLS - ENHANCED INI-DRIVEN LAYOUT
    // ========================================================================
    // Layout: [Pattern Matrix Section (63.5%)] [Margin] [Controls Section (35%)]
    // All positioning uses Row5:: namespace constants with consistent formatting
    // This is the MAIN CONTENT ROW - 45% of total interface height
    
    using namespace INIConfig::LayoutConstants;
    
    if (!leftSection || !rightSection) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
            "Left or right section is null, cannot update Row 5 layout", "MainContentComponent");
        return;
    }
    
    // Row5 main positioning - 45% of interface height (largest content area)
    int row5Y = layoutManager.scaled(defaultPadding + ROW_3_HEIGHT + ROW_4_HEIGHT);
    int row5Height = layoutManager.scaled(Row5::height);
    
    // Left Section: 4x4 Pattern Matrix (63.5% width) - optimized for matrix display
    int leftSectionX = layoutManager.scaled(Row5::leftSectionX);
    int leftSectionWidth = layoutManager.scaled(Row5::leftSectionWidth);
    leftSection->setBounds(leftSectionX, row5Y, leftSectionWidth, row5Height);
    
    // Right Section: Controls (35% width) - sliders, toggles, fills
    int rightSectionX = layoutManager.scaled(Row5::rightSectionX);
    int rightSectionWidth = layoutManager.scaled(Row5::rightSectionWidth);
    rightSection->setBounds(rightSectionX, row5Y, rightSectionWidth, row5Height);
    
    // ========================================================================
    // ENHANCED VALIDATION: Comprehensive layout validation with detailed checks
    #ifdef JUCE_DEBUG
        int totalUsedWidth = layoutManager.scaled(Row5::totalUsedWidth);
        int availableWidth = getLocalBounds().getWidth();
        int sectionMargin = layoutManager.scaled(Row5::sectionMargin);
        
        // Primary layout validation
        jassert(totalUsedWidth <= availableWidth); // Overall width fits
        jassert(leftSectionX + leftSectionWidth + sectionMargin <= rightSectionX); // No overlap
        
        // Content area validation
        jassert(leftSectionWidth >= layoutManager.scaled(200)); // Minimum matrix width
        jassert(rightSectionWidth >= layoutManager.scaled(100)); // Minimum controls width
        jassert(row5Height == layoutManager.scaled(Row5::height)); // Height consistency
        
        // Percentage validation (debug info)
        float actualLeftPercent = (static_cast<float>(leftSectionWidth) / availableWidth) * 100.0f;
        float actualRightPercent = (static_cast<float>(rightSectionWidth) / availableWidth) * 100.0f;
        jassert(actualLeftPercent >= 60.0f && actualLeftPercent <= 70.0f); // ~63.5%
        jassert(actualRightPercent >= 30.0f && actualRightPercent <= 40.0f); // ~35%
    #endif
}

void MainContentComponent::updateRow6Layout() {
    // ========================================================================
    // PHASE 5: ROW 6 LOOP SECTION - ENHANCED INI-DRIVEN LAYOUT  
    // ========================================================================
    // Layout: [LOOP START] [================== Loop Slider ==================] [LOOP END]
    // All positioning uses Row6:: namespace constants with consistent formatting
    // This is the BOTTOM ROW - 12% of total interface height for loop timeline
    
    using namespace INIConfig::LayoutConstants;
    
    if (!loopSection) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
            "Loop section is null, cannot update Row 6 layout", "MainContentComponent");
        return;
    }
    
    auto bounds = getLocalBounds();
    
    // Row6 positioning - 12% of interface height (compact loop controls)
    int row6Y = layoutManager.scaled(defaultPadding + ROW_3_HEIGHT + ROW_4_HEIGHT + ROW_5_HEIGHT);
    int row6Height = layoutManager.scaled(Row6::height);
    
    // Position LoopSectionComponent across full interface width
    loopSection->setBounds(0, row6Y, bounds.getWidth(), row6Height);
    
    // Row 6 is the FINAL ROW - no separator needed after it
    
    // ========================================================================
    // ENHANCED VALIDATION: Loop section layout validation with accessibility checks
    #ifdef JUCE_DEBUG
        int totalRowHeight = row6Y + row6Height;
        int availableHeight = bounds.getHeight();
        
        // Primary layout validation
        jassert(totalRowHeight <= availableHeight); // Row fits within interface
        jassert(row6Y >= layoutManager.scaled(Row5::yPosition + Row5::height)); // Positioned after Row5
        
        // Loop section validation (ensuring proper responsive scaling)
        jassert(row6Height >= layoutManager.scaled(44)); // Minimum height for loop controls
        jassert(bounds.getWidth() >= layoutManager.scaled(300)); // Minimum width for loop timeline
        
        // Interface completeness validation
        int expectedTotalHeight = layoutManager.scaled(Row6::yPosition + Row6::height);
        jassert(abs(expectedTotalHeight - availableHeight) <= 10); // Allow small rounding differences
        
        // Row percentage validation (debug info)
        float actualRowPercent = (static_cast<float>(row6Height) / availableHeight) * 100.0f;
        jassert(actualRowPercent >= 10.0f && actualRowPercent <= 15.0f); // ~12%
    #endif
}

void MainContentComponent::lookAndFeelChanged() {
    // ========================================================================
    // COMPREHENSIVE COLORSCHEME UPDATE FOR ALL ROW COMPONENTS
    // ========================================================================
    
    // ========================================================================
    // ROW 3: DrumKit Menu Controls ColorScheme Integration
    // ========================================================================
    drumKitEditButton.setColorScheme(&colorScheme);
    drumKitLeftChevron.setColorScheme(&colorScheme);
    drumKitRightChevron.setColorScheme(&colorScheme);
    drumKitMuteButton.setColorScheme(&colorScheme);
    drumKitMixerButton.setColorScheme(&colorScheme);
    
    // Row3 DrumKit dropdown complete ColorScheme integration
    drumKitDropdown.setColour(juce::ComboBox::backgroundColourId, 
                             colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    drumKitDropdown.setColour(juce::ComboBox::textColourId, 
                             colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    drumKitDropdown.setColour(juce::ComboBox::arrowColourId, 
                             colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    drumKitDropdown.setColour(juce::ComboBox::buttonColourId, 
                             colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
    drumKitDropdown.setColour(juce::ComboBox::outlineColourId, 
                             colorScheme.getColor(ColorScheme::ColorRole::Separator));
    

    
    // ========================================================================
    // ROW 4: Pattern Group Controls - ENHANCED ColorScheme Integration
    // ========================================================================
    
    // Pattern Group Label - uses SecondaryText for subtle hierarchy
    patternGroupLabel.setColour(juce::Label::textColourId, 
                               colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    patternGroupLabel.setColour(juce::Label::backgroundColourId, 
                               colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    
    // Pattern Group Dropdown - complete ColorScheme integration
    patternGroupDropdown.setColour(juce::ComboBox::backgroundColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    patternGroupDropdown.setColour(juce::ComboBox::textColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    patternGroupDropdown.setColour(juce::ComboBox::arrowColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    patternGroupDropdown.setColour(juce::ComboBox::buttonColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
    patternGroupDropdown.setColour(juce::ComboBox::outlineColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::Separator));
    
    // Pattern Status Label - uses SecondaryText for information display
    patternStatusLabel.setColour(juce::Label::textColourId, 
                                colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    patternStatusLabel.setColour(juce::Label::backgroundColourId, 
                                colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    
    // Pattern Action Buttons - PhosphorIconButtons with ColorScheme
    patternAddButton.setColorScheme(&colorScheme);
    patternDeleteButton.setColorScheme(&colorScheme);
    
    // ========================================================================
    // ROW 5/6: Child Component ColorScheme Propagation - ENHANCED INTEGRATION
    // ========================================================================
    
    // Row5 Components: 4x4 Matrix & Controls - complete ColorScheme propagation
    if (leftSection) {
        leftSection->lookAndFeelChanged(); // 4x4 pattern matrix ColorScheme update
    }
    if (rightSection) {
        rightSection->lookAndFeelChanged(); // Controls (sliders, toggles) ColorScheme update
    }
    
    // Row6 Component: Loop Section - ColorScheme propagation (if supported)
    if (loopSection) {
        // Check if LoopSectionComponent supports lookAndFeelChanged
        // This will propagate ColorScheme to loop timeline controls
        try {
            loopSection->lookAndFeelChanged();
        } catch (...) {
            // Silently handle if lookAndFeelChanged not implemented
            // LoopSectionComponent may handle ColorScheme internally
        }
    }
    
    // Force complete UI repaint to apply all ColorScheme changes
    repaint();
}

// ========================================================================
// PROMPT 6: FINAL INTEGRATION VALIDATION - COMPREHENSIVE SYSTEM TESTING
// ========================================================================
#ifdef JUCE_DEBUG
void MainContentComponent::performIntegrationValidation(const juce::Rectangle<int>& bounds) {
    using namespace INIConfig::LayoutConstants;
    
    // ========================================================================
    // COMPLETE 6-ROW SYSTEM VALIDATION
    // ========================================================================
    
    // Interface completeness validation
    int expectedTotalHeight = layoutManager.scaled(ROW_6_Y + ROW_6_HEIGHT);
    int mainContentHeight = bounds.getHeight();
    int mainContentOffset = layoutManager.scaled(ROW_3_Y);
    
    // Primary system validation
    jassert(mainContentHeight > 0); // Valid component size
    jassert(bounds.getWidth() >= layoutManager.scaled(600)); // Minimum interface width
    
    // Row height validations - ensure all rows have reasonable sizes
    jassert(layoutManager.scaled(Row3::height) >= layoutManager.scaled(44)); // Row3 accessibility
    jassert(layoutManager.scaled(Row4::height) >= layoutManager.scaled(44)); // Row4 accessibility  
    jassert(layoutManager.scaled(Row5::height) >= layoutManager.scaled(100)); // Row5 main content
    jassert(layoutManager.scaled(Row6::height) >= layoutManager.scaled(44)); // Row6 accessibility
    
    // Row positioning validation - ensure proper sequence
    jassert(layoutManager.scaled(ROW_3_Y) < layoutManager.scaled(ROW_4_Y)); // Row3 before Row4
    jassert(layoutManager.scaled(ROW_4_Y) < layoutManager.scaled(ROW_5_Y)); // Row4 before Row5
    jassert(layoutManager.scaled(ROW_5_Y) < layoutManager.scaled(ROW_6_Y)); // Row5 before Row6
    
    // Component existence validation
    jassert(leftSection != nullptr); // Row5 left section exists
    jassert(rightSection != nullptr); // Row5 right section exists
    jassert(loopSection != nullptr); // Row6 loop section exists
    
    // Separator validation
    jassert(row3Separator.isVisible()); // Row3/4 separator visible
    jassert(row4Separator.isVisible()); // Row4/5 separator visible
    jassert(row5Separator.isVisible()); // Row5/6 separator visible
    jassert(!row1Separator.isVisible()); // Row1/2 separator hidden (PluginEditor scope)
    jassert(!row2Separator.isVisible()); // Row2/3 separator hidden (PluginEditor scope)
    
    // Row5 section validation (main content area)
    auto leftBounds = leftSection->getBounds();
    auto rightBounds = rightSection->getBounds();
    jassert(!leftBounds.intersects(rightBounds)); // No section overlap
    jassert(leftBounds.getWidth() > rightBounds.getWidth()); // Left section larger (63.5% vs 35%)
    
    // Interface scaling validation - check scaled values are reasonable
    int testScaledValue = layoutManager.scaled(100);
    jassert(testScaledValue > 50 && testScaledValue < 300); // Reasonable scale range (0.5x to 3.0x)
    
    // ColorScheme validation
    jassert(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground).isOpaque()); // Valid background
    jassert(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText) != juce::Colours::transparentBlack); // Valid text color
    
    // FontManager validation
    auto testFont = fontManager.getFont(FontManager::FontRole::Body, 14.0f);
    jassert(testFont.getHeight() > 0); // Valid font rendering
    
    // Performance validation - ensure reasonable layout times
    static auto lastResizeTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    auto resizeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastResizeTime);
    lastResizeTime = currentTime;
    
    // Layout should complete in reasonable time (< 16ms for 60fps)
    if (resizeDuration.count() > 0) { // Avoid first run validation
        jassert(resizeDuration.count() < 50); // 50ms maximum layout time
    }
    
    // Integration success confirmation
    DBG("✅ OTTO INTEGRATION VALIDATION PASSED: Complete 6-row INI-driven layout system operational");
}
#endif




void MainContentComponent::handleDrumKitChevrons(bool isRight) {
    // Get current drumkit selection from dropdown
    int currentIndex = drumKitDropdown.getSelectedId() - 1; // ComboBox IDs start at 1
    int itemCount = drumKitDropdown.getNumItems();
    
    if (itemCount > 0) {
        if (isRight) {
            // Next drumkit (increment with wrap-around)
            currentIndex = (currentIndex + 1) % itemCount;
        } else {
            // Previous drumkit (decrement with wrap-around)
            currentIndex = (currentIndex - 1 + itemCount) % itemCount;
        }
        
        // Update dropdown selection (ComboBox IDs start at 1)
        drumKitDropdown.setSelectedId(currentIndex + 1, juce::dontSendNotification);
        
        // Update the selected label with Playfair Display font
        juce::String selectedText = drumKitDropdown.getText();
        drumKitSelectedLabel.setText(selectedText, juce::dontSendNotification);
        drumKitSelectedLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, 16.0f)); // Ensure Playfair Display
        
        // Ensure label is shown after chevron navigation (following preset pattern)
        showDrumKitLabel();
        
        // Notify parent component of drumkit change
        if (onDrumKitChanged) {
            onDrumKitChanged(selectedText);
        }
        
        // Trigger the dropdown's change callback if it exists
        if (drumKitDropdown.onChange) {
            drumKitDropdown.onChange();
        }
    }
}

void MainContentComponent::updateDrumKitList(const juce::StringArray& drumkitNames) {
    // Clear existing items and repopulate with new drumkit list
    drumKitDropdown.clear(juce::dontSendNotification);
    
    // Add drumkits to dropdown with proper IDs (starting from 1)
    for (int i = 0; i < drumkitNames.size(); ++i) {
        drumKitDropdown.addItem(drumkitNames[i], i + 1);
    }
    
    // Set default selection if items exist
    if (drumkitNames.size() > 0) {
        drumKitDropdown.setSelectedId(1, juce::dontSendNotification);
        // Update label with Playfair Display font
        drumKitSelectedLabel.setText(drumkitNames[0], juce::dontSendNotification);
        drumKitSelectedLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, 16.0f));
    } else {
        drumKitSelectedLabel.setText("No DrumKits", juce::dontSendNotification);
        drumKitSelectedLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, 16.0f));
    }
}

void MainContentComponent::setSelectedDrumKit(const juce::String& drumkitName) {
    // Find the drumkit in the dropdown and select it
    for (int i = 0; i < drumKitDropdown.getNumItems(); ++i) {
        if (drumKitDropdown.getItemText(i) == drumkitName) {
            drumKitDropdown.setSelectedId(i + 1, juce::dontSendNotification);
            // Update label with Playfair Display font
            drumKitSelectedLabel.setText(drumkitName, juce::dontSendNotification);
            drumKitSelectedLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, 16.0f));
            
            // Notify parent component
            if (onDrumKitChanged) {
                onDrumKitChanged(drumkitName);
            }
            break;
        }
    }
}

void MainContentComponent::updateDrumKitDisplayToggle() {
    if (showingDrumKitLabel) {
        // Show large drumkit label, hide dropdown menu (following preset pattern)
        drumKitSelectedLabel.setVisible(true);
        drumKitDropdown.setVisible(false);
    } else {
        // Show dropdown menu, hide large drumkit label (following preset pattern)
        drumKitSelectedLabel.setVisible(false);
        drumKitDropdown.setVisible(true);
    }
}

void MainContentComponent::showDrumKitLabel() {
    showingDrumKitLabel = true;
    updateDrumKitDisplayToggle();
}

void MainContentComponent::showDrumKitMenu() {
    showingDrumKitLabel = false;
    updateDrumKitDisplayToggle();
    
    // Focus on the dropdown to show it (following preset pattern)
    drumKitDropdown.showPopup();
}

void MainContentComponent::mouseDown(const juce::MouseEvent& event) {
    // Handle drumkit label clicks (following TopBarComponent pattern)
    if (event.eventComponent == &drumKitSelectedLabel && showingDrumKitLabel) {
        // User clicked the drumkit label - show menu
        showDrumKitMenu();
        return;
    }
    
    // Call parent implementation for other mouse events
    juce::Component::mouseDown(event);
}
