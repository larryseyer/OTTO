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
 * Row 1 (10%): Row1Component - Settings, presets, transport controls
 * Row 2 (8%):  Row2Component - Player selection tabs  
 * Row 3 (16%): Row3Component - DrumKit controls, edit, navigation, kit selection
 * Row 4 (14%): Row4Component - Pattern management, groups, labels, status
 * Row 5 (44%): Row5Component - Interactive controls only (drum grid moved to Row5)
 * Row 6 (8%):  Row6Component - Loop/transport section, scene launcher, performance controls
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
 * - Row1Component: TopBar functionality
 * - Row2Component: Player tabs functionality  
 * - Row3Component: DrumKit controls functionality
 * - Row4Component: Pattern controls + labels functionality
 * - Row5Component: Interactive controls functionality
 * - Row6Component: Loop controls functionality
 * - LoopSectionComponent: Legacy transport controls (being phased out)
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

#include "SceneLauncherComponent.h"
#include "LoopSectionComponent.h"
#include "Row1Component.h"
#include "Row2Component.h"
#include "Row3Component.h"
#include "Row4Component.h"
#include "Row5Component.h"
#include "Row6Component.h"
#include "Mixer.h"
#include "INIConfig.h"
#include "INIDataManager.h"
#include "ErrorHandling.h"
#include "PerformanceOptimizations.h"
#include "Animation/AnimationManager.h"
#include "UX/KeyboardHandler.h"
#include "UX/ContextMenuManager.h"
#include "Performance/RenderOptimizer.h"
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
      
      // ROW 4 COMPONENTS: Pattern group control system per header specification
      patternGroupEditButton("pencil", FontManager::PhosphorWeight::Regular),     // Edit pattern groups
      patternGroupLeftChevron("caret-left", FontManager::PhosphorWeight::Regular), // Previous pattern group
      patternGroupRightChevron("caret-right", FontManager::PhosphorWeight::Regular), // Next pattern group
      patternGroupFavoriteButton("heart", FontManager::PhosphorWeight::Regular),   // Favorite pattern group
      
      togglesLabel(),
      fillsLabel(),
      swingLabel(),
      energyLabel(),
      volumeLabel(),

      
      // ROW SEPARATORS: Visual dividers between each interface row
      // Use ColorScheme for theme-consistent styling, positioned between rows 1-6
      row1Separator(colorScheme), row2Separator(colorScheme), row3Separator(colorScheme), 
      row4Separator(colorScheme), row5Separator(colorScheme) {

    // PHASE 8: ROW COMPONENT CREATION - Clean row-based architecture
    // All legacy left/right sections removed - functionality moved to Row components
    
    // LOOP SECTION: Transport controls and scene launching functionality
    // Handles loop position, scene management, and performance controls
    loopSection = std::make_unique<LoopSectionComponent>(
        layoutManager, fontManager, colorScheme);

    // COMPONENT VISIBILITY SETUP: Add all components to JUCE's component hierarchy
    // This establishes parent-child relationships and enables automatic rendering
    
    // Row 3 labels and player number display
    addAndMakeVisible(rhythmLabel);     // "Rhythm" label for pattern identification

    // Major layout sections - Row components contain all interface functionality
    addAndMakeVisible(*loopSection);    // Transport and scene launcher
    
    // Create Row1Component if not already created
    if (!row1Component) {
        row1Component = std::make_unique<Row1Component>(
            midiEngine, valueTreeState, layoutManager, fontManager, colorScheme);
    }
    addAndMakeVisible(*row1Component);  // TopBar functionality in row-based architecture
    
    // Create Row2Component if not already created
    if (!row2Component) {
        row2Component = std::make_unique<Row2Component>(
            midiEngine, valueTreeState, layoutManager, fontManager, colorScheme);
    }
    addAndMakeVisible(*row2Component);  // Player tabs functionality in row-based architecture
    
    // Create Row3Component if not already created
    if (!row3Component) {
        row3Component = std::make_unique<Row3Component>(
            midiEngine, mixer, layoutManager, fontManager, colorScheme);
    }
    addAndMakeVisible(*row3Component);  // DrumKit controls functionality in row-based architecture
    
    // Create Row4Component if not already created
    if (!row4Component) {
        row4Component = std::make_unique<Row4Component>(
            midiEngine, layoutManager, fontManager, colorScheme);
    }
    addAndMakeVisible(*row4Component);  // Pattern controls + labels functionality in row-based architecture
    
    // Create Row5Component if not already created
    if (!row5Component) {
        row5Component = std::make_unique<Row5Component>(
            midiEngine, mixer, valueTreeState, layoutManager, fontManager, colorScheme);
    }
    addAndMakeVisible(*row5Component);  // Interactive controls functionality in row-based architecture
    
    // Create Row6Component if not already created
    if (!row6Component) {
        row6Component = std::make_unique<Row6Component>(
            layoutManager, fontManager, colorScheme);
    }
    addAndMakeVisible(*row6Component);  // Loop controls functionality in row-based architecture

    animationManager = std::make_unique<AnimationManager>(platformManager);
    keyboardHandler = std::make_unique<KeyboardHandler>(fontManager, colorScheme);
    contextMenuManager = std::make_unique<ContextMenuManager>(colorScheme, fontManager);
    renderOptimizer = std::make_unique<RenderOptimizer>();

    setupKeyboardHandlerCallbacks();
    setupContextMenuCallbacks();
    setupAnimationManagers();
    
    setDragAndDropContainer(true);
    
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
    
    // PHASE 8: Row components handle their own setup in constructors
    // No additional setup needed - components are self-contained

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

    // PHASE 8: Row component callbacks - clean architecture using actual available callbacks
    
    // Row5Component has onMidiFileChanged callback - set it up
    row5Component->onMidiFileChanged = [this](int playerIndex, const juce::String& midiFile) {
        if (onMidiFileChanged) onMidiFileChanged(playerIndex, midiFile);
    };
    
    // Row5Component has onSliderValueChanged callback - set it up
    row5Component->onSliderValueChanged = [this](const juce::String& sliderName, float value) {
        if (onSliderValueChanged) onSliderValueChanged(sliderName, value);
    };

    // PHASE 8: Legacy rightSection callback removed - functionality moved to Row5Component
}

MainContentComponent::~MainContentComponent() {
}

void MainContentComponent::updatePlayerDisplay(int playerIndex) {
    currentPlayerIndex = INIConfig::clampPlayerIndex(playerIndex);
    
    // Update Row3Component with new player index
    if (row3Component) {
        row3Component->updatePlayerDisplay(currentPlayerIndex);
    }
    
    // Update Row4Component labels when player changes
    if (row4Component) {
        row4Component->updateLabelsFromControls();
    }
    
    // PHASE 6: Update Row5Component with new player index - Interactive controls only
    if (row5Component) {
        row5Component->setCurrentPlayerIndex(currentPlayerIndex);
    }
    
    // Legacy: Update mute button state and icon to reflect current player's mute status
    // TODO: Remove this when Row3Component fully replaces legacy drumkit controls
    bool isCurrentPlayerMuted = mixer.isChannelMuted(currentPlayerIndex);
    drumKitMuteButton.setToggleState(isCurrentPlayerMuted, juce::dontSendNotification);
    drumKitMuteButton.setIconName(isCurrentPlayerMuted ? "mute" : "unmute");
}

float MainContentComponent::getSwingValue() const {
    // PHASE 8: Use Row5Component for interactive controls - legacy rightSection removed
    if (row5Component) {
        return row5Component->getSwingValue();
    }
    
    ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
        "Row5Component is null, returning default swing value", "MainContentComponent");
    return INIConfig::Defaults::SWING;
}

void MainContentComponent::setSwingValue(float value) {
    try {
        float clampedValue = INIConfig::clampSwing(value);
        // PHASE 8: Use Row5Component for interactive controls - legacy rightSection removed
        if (row5Component) {
            row5Component->setSwingValue(clampedValue);
        } else {
            ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
                "Row5Component is null, cannot set swing value", "MainContentComponent");
        }
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to set swing value: " + juce::String(e.what()), "MainContentComponent");
    }
}

float MainContentComponent::getEnergyValue() const {
    // PHASE 8: Use Row5Component for interactive controls - legacy rightSection removed
    if (row5Component) {
        return row5Component->getEnergyValue();
    }
    
    ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
        "Row5Component is null, returning default energy value", "MainContentComponent");
    return INIConfig::Defaults::ENERGY;
}

void MainContentComponent::setEnergyValue(float value) {
    try {
        float clampedValue = INIConfig::clampEnergy(value);
        // PHASE 8: Use Row5Component for interactive controls - legacy rightSection removed
        if (row5Component) {
            row5Component->setEnergyValue(clampedValue);
        } else {
            ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
                "Row5Component is null, cannot set energy value", "MainContentComponent");
        }
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to set energy value: " + juce::String(e.what()), "MainContentComponent");
    }
}

float MainContentComponent::getVolumeValue() const {
    // PHASE 8: Use Row5Component for interactive controls - legacy rightSection removed
    if (row5Component) {
        return row5Component->getVolumeValue();
    }
    
    ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
        "Row5Component is null, returning default volume value", "MainContentComponent");
    return INIConfig::Defaults::VOLUME;
}

void MainContentComponent::setVolumeValue(float value) {
    try {
        float clampedValue = INIConfig::clampVolume(value);
        // PHASE 8: Use Row5Component for interactive controls - legacy rightSection removed
        if (row5Component) {
            row5Component->setVolumeValue(clampedValue);
        } else {
            ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
                "Row5Component is null, cannot set volume value", "MainContentComponent");
        }
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to set volume value: " + juce::String(e.what()), "MainContentComponent");
    }
}

bool MainContentComponent::getEditMode() const {
    // PHASE 8: Use Row5Component for edit mode - legacy leftSection removed
    if (row5Component) {
        return row5Component->getEditMode();
    }
    return false;
}

void MainContentComponent::setEditModeVisuals(bool isEditing) {
    // PHASE 8: Use Row5Component for edit mode visuals - legacy leftSection removed
    if (row5Component) {
        row5Component->setEditModeVisuals(isEditing);
    }
}

juce::String MainContentComponent::getMidiFileAssignment(int buttonIndex) const {
    // PHASE 8: Use Row5Component for MIDI file assignments - legacy leftSection removed
    if (row5Component) {
        return row5Component->getMidiFileAssignment(buttonIndex);
    }
    return "";
}

void MainContentComponent::setMidiFileAssignment(int buttonIndex, const juce::String& midiFile) {
    // PHASE 8: Use Row5Component for MIDI file assignments - legacy leftSection removed
    if (row5Component) {
        row5Component->setMidiFileAssignment(buttonIndex, midiFile);
    }
}

void MainContentComponent::saveStates(ComponentState& state) {
    // PHASE 8: Save states from Row components - legacy sections removed
    if (row1Component) {
        row1Component->saveStates(state);
    }
    if (row2Component) {
        row2Component->saveStates(state);
    }
    if (row3Component) {
        row3Component->saveStates(state);
    }
    if (row4Component) {
        row4Component->saveStates(state);
    }
    if (row5Component) {
        row5Component->saveStates(state);
    }
    if (row6Component) {
        row6Component->saveStates(state);
    }
}

void MainContentComponent::loadStates(const ComponentState& state) {
    // PHASE 8: Load states to Row components - legacy sections removed
    if (row1Component) {
        row1Component->loadStates(state);
    }
    if (row2Component) {
        row2Component->loadStates(state);
    }
    if (row3Component) {
        row3Component->loadStates(state);
    }
    if (row4Component) {
        row4Component->loadStates(state);
    }
    if (row5Component) {
        row5Component->loadStates(state);
    }
    if (row6Component) {
        row6Component->loadStates(state);
    }
}

int MainContentComponent::getFavoritesSelection() const {
    // PHASE 8: Use Row4Component for pattern group index - simplified interface
    if (row4Component) {
        return row4Component->getCurrentPatternGroupIndex();
    }
    return 0;
}

void MainContentComponent::setFavoritesSelection(int selection) {
    // PHASE 8: Use Row4Component for pattern group index - simplified interface
    if (row4Component) {
        row4Component->setCurrentPatternGroupIndex(selection);
    }
}

void MainContentComponent::setupRow4LabelIntegration() {
    // PHASE 8: Row4Component handles label integration internally via updateLabelsFromControls()
    if (row4Component) {
        row4Component->updateLabelsFromControls();
    }
}

void MainContentComponent::updateFromState(const ComponentState& state) {
    // PHASE 8: Update Row components from state - legacy sections removed
    if (row1Component) {
        row1Component->updateFromState(state);
    }
    if (row2Component) {
        row2Component->updateFromState(state);
    }
    if (row3Component) {
        row3Component->updateFromState(state);
    }
    if (row4Component) {
        row4Component->updateFromState(state);
    }
    if (row5Component) {
        row5Component->updateFromState(state);
    }
    if (row6Component) {
        row6Component->updateFromState(state);
    }
}

void MainContentComponent::switchToPlayer(int playerIndex, const ComponentState& state) {
    validatePlayerIndex(playerIndex);
    currentPlayerIndex = playerIndex;
    
    // PHASE 8: Update Row components with new player - legacy sections removed
    if (row3Component) {
        row3Component->updatePlayerDisplay(playerIndex);
    }
    if (row4Component) {
        row4Component->updateLabelsFromControls();
    }
    if (row5Component) {
        row5Component->setCurrentPlayerIndex(playerIndex);
    }
    
    updateFromState(state);
}

void MainContentComponent::setLivePerformanceMode(bool enabled) {
    livePerformanceMode = enabled;
    updateLayoutForPerformanceMode();
}

void MainContentComponent::showSceneLauncher(bool show) {
    if (sceneLauncher) {
        sceneLauncher->setVisible(show);
    }
}

void MainContentComponent::updateDrumKitList(const juce::StringArray& drumkitNames) {
    // PHASE 8: Row3Component handles drumkit list internally
    // No external update needed - component manages its own dropdown
}

void MainContentComponent::setSelectedDrumKit(const juce::String& drumkitName) {
    // PHASE 8: Use Row3Component's public interface for drumkit selection
    if (row3Component) {
        row3Component->setDrumKitName(drumkitName);
    }
}

void MainContentComponent::updateDrumKitDisplayToggle() {
    // PHASE 8: Row3Component handles display toggle internally
    // No external update needed - component manages its own state
}

void MainContentComponent::showDrumKitLabel() {
    // PHASE 8: Row3Component handles label/menu display internally
    // No external control needed - component manages its own display state
}

void MainContentComponent::showDrumKitMenu() {
    // PHASE 8: Row3Component handles label/menu display internally
    // No external control needed - component manages its own display state
}

void MainContentComponent::setDrumKitEditMode(bool editMode) {
    // PHASE 8: Use Row3Component for drumkit edit mode
    if (row3Component) {
        row3Component->setEditMode(editMode);
    }
}

bool MainContentComponent::isDrumKitEditMode() const {
    // PHASE 8: Use Row3Component for drumkit edit mode
    if (row3Component) {
        return row3Component->isEditMode();
    }
    return false;
}

void MainContentComponent::setDrumKitMuteState(bool muted) {
    // PHASE 8: Use Row3Component for drumkit mute state
    if (row3Component) {
        row3Component->setMuteState(muted);
    }
}

bool MainContentComponent::isDrumKitMuted() const {
    // PHASE 8: Use Row3Component for drumkit mute state
    if (row3Component) {
        return row3Component->isMuted();
    }
    return false;
}

float MainContentComponent::getLoopPosition() const {
    // PHASE 8: Use Row6Component for loop position
    if (row6Component) {
        return row6Component->getLoopPosition();
    }
    return 0.0f;
}

void MainContentComponent::setLoopPosition(float position) {
    // PHASE 8: Use Row6Component for loop position
    if (row6Component) {
        row6Component->setLoopPosition(position);
    }
}

void MainContentComponent::mouseDown(const juce::MouseEvent& event) {
    // PHASE 8: Handle mouse events - delegate to appropriate Row components
    juce::Component::mouseDown(event);
}

void MainContentComponent::paint(juce::Graphics& g) {
    // PHASE 8: Clean paint method - Row components handle their own painting
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
}

void MainContentComponent::resized() {
    // PHASE 8: Clean resized method - Row components handle their own positioning
    auto bounds = getLocalBounds();
    
    // Row components automatically position themselves using getRowBounds()
    if (row1Component) {
        row1Component->setBounds(row1Component->getRowBounds());
    }
    if (row2Component) {
        row2Component->setBounds(row2Component->getRowBounds());
    }
    if (row3Component) {
        row3Component->setBounds(row3Component->getRowBounds());
    }
    if (row4Component) {
        row4Component->setBounds(row4Component->getRowBounds());
    }
    if (row5Component) {
        row5Component->setBounds(row5Component->getRowBounds());
    }
    if (row6Component) {
        row6Component->setBounds(row6Component->getRowBounds());
    }
    
    // Legacy loop section positioning
    if (loopSection) {
        using namespace INIConfig::LayoutConstants;
        int row6Y = layoutManager.scaled(ROW_1_HEIGHT + ROW_2_HEIGHT + ROW_3_HEIGHT + ROW_4_HEIGHT + ROW_5_HEIGHT);
        int row6Height = layoutManager.scaled(ROW_6_HEIGHT);
        loopSection->setBounds(0, row6Y, bounds.getWidth(), row6Height);
    }
    
    // Position row separators
    updateRowSeparators();
    
    // Position debug labels
    updateDebugLabels();
}

void MainContentComponent::lookAndFeelChanged() {
    // PHASE 8: Propagate look and feel changes to Row components
    if (row1Component) {
        row1Component->lookAndFeelChanged();
    }
    if (row2Component) {
        row2Component->lookAndFeelChanged();
    }
    if (row3Component) {
        row3Component->lookAndFeelChanged();
    }
    if (row4Component) {
        row4Component->lookAndFeelChanged();
    }
    if (row5Component) {
        row5Component->lookAndFeelChanged();
    }
    if (row6Component) {
        row6Component->lookAndFeelChanged();
    }
    if (loopSection) {
        loopSection->lookAndFeelChanged();
    }
}

void MainContentComponent::buttonClicked(juce::Button* button) {
    // PHASE 8: Handle legacy button clicks - use public interfaces only
    if (button == &drumKitEditButton) {
        if (row3Component) {
            // Toggle edit mode using public interface
            row3Component->setEditMode(!row3Component->isEditMode());
        }
    }
    else if (button == &drumKitLeftChevron) {
        // Trigger drumkit navigation callback
        if (row3Component && row3Component->onDrumKitNavigated) {
            row3Component->onDrumKitNavigated(false); // false = previous
        }
    }
    else if (button == &drumKitRightChevron) {
        // Trigger drumkit navigation callback
        if (row3Component && row3Component->onDrumKitNavigated) {
            row3Component->onDrumKitNavigated(true); // true = next
        }
    }
    else if (button == &drumKitMuteButton) {
        if (row3Component) {
            // Toggle mute state using public interface
            row3Component->setMuteState(!row3Component->isMuted());
        }
    }
    else if (button == &drumKitMixerButton) {
        // Trigger mixer callback
        if (row3Component && row3Component->onMixerRequested) {
            row3Component->onMixerRequested();
        }
    }
    else if (button == &patternGroupEditButton) {
        if (row4Component) {
            // Toggle pattern group edit mode using public interface
            row4Component->setPatternGroupEditMode(!row4Component->isPatternGroupEditMode());
        }
    }
    else if (button == &patternGroupLeftChevron) {
        if (row4Component) {
            // Navigate to previous pattern group using public interface
            row4Component->navigatePatternGroup(false);
        }
    }
    else if (button == &patternGroupRightChevron) {
        if (row4Component) {
            // Navigate to next pattern group using public interface
            row4Component->navigatePatternGroup(true);
        }
    }
    else if (button == &patternGroupFavoriteButton) {
        if (row4Component) {
            // Toggle favorite state using public interface
            row4Component->togglePatternGroupFavorite();
        }
    }
}

void MainContentComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    // PHASE 8: Handle legacy combo box changes - use callbacks instead of private methods
    if (comboBoxThatHasChanged == &drumKitDropdown) {
        if (row3Component && row3Component->onDrumKitChanged) {
            // Get selected drumkit name and trigger callback
            auto selectedText = drumKitDropdown.getText();
            row3Component->onDrumKitChanged(selectedText);
        }
    }
    else if (comboBoxThatHasChanged == &patternGroupDropdown) {
        if (row4Component) {
            // Set the selected pattern group index based on dropdown selection
            int selectedIndex = patternGroupDropdown.getSelectedItemIndex();
            if (selectedIndex >= 0) {
                row4Component->setCurrentPatternGroupIndex(selectedIndex);
            }
        }
    }
}

// PHASE 8: Private helper methods - simplified for Row component architecture

void MainContentComponent::validatePlayerIndex(int playerIndex) const {
    if (playerIndex < 0 || playerIndex >= INIConfig::Defaults::MAX_PLAYERS) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Invalid player index: " + juce::String(playerIndex), "MainContentComponent");
    }
}

void MainContentComponent::notifyStateChanged(ComponentState& state) {
    if (onStateChanged) {
        onStateChanged(state);
    }
}

void MainContentComponent::updateLayoutForPerformanceMode() {
    // PHASE 8: Performance mode updates - Row components handle their own performance optimizations
    // No specific performance mode methods needed - components optimize themselves
    repaint();
}

// PHASE 8: Setup methods removed - Row components handle their own setup in constructors

void MainContentComponent::updateRowSeparators() {
    // Position row separators between rows using correct INIConfig constants
    using namespace INIConfig::LayoutConstants;
    auto bounds = getLocalBounds();
    
    int separatorHeight = 2;
    int row1Bottom = layoutManager.scaled(ROW_1_HEIGHT);
    int row2Bottom = row1Bottom + layoutManager.scaled(ROW_2_HEIGHT);
    int row3Bottom = row2Bottom + layoutManager.scaled(ROW_3_HEIGHT);
    int row4Bottom = row3Bottom + layoutManager.scaled(ROW_4_HEIGHT);
    int row5Bottom = row4Bottom + layoutManager.scaled(ROW_5_HEIGHT);
    
    row1Separator.setBounds(0, row1Bottom, bounds.getWidth(), separatorHeight);
    row2Separator.setBounds(0, row2Bottom, bounds.getWidth(), separatorHeight);
    row3Separator.setBounds(0, row3Bottom, bounds.getWidth(), separatorHeight);
    row4Separator.setBounds(0, row4Bottom, bounds.getWidth(), separatorHeight);
    row5Separator.setBounds(0, row5Bottom, bounds.getWidth(), separatorHeight);
}

void MainContentComponent::updateDebugLabels() {
    // Position red row markers on far left, just above their black horizontal lines
    using namespace INIConfig::LayoutConstants;
    auto bounds = getLocalBounds();
    
    int markerWidth = layoutManager.scaled(60);
    int markerHeight = layoutManager.scaled(20);
    int markerX = layoutManager.scaled(5); // Far left position
    
    int row1Y = layoutManager.scaled(Row1::yPosition);
    int row2Y = layoutManager.scaled(Row2::yPosition);
    int row4Y = layoutManager.scaled(Row4::yPosition);
    int row5Y = layoutManager.scaled(Row5::yPosition);
    int row6Y = layoutManager.scaled(Row6::yPosition);
    
    rowLabel1.setBounds(markerX, row1Y - markerHeight, markerWidth, markerHeight);
    rowLabel2.setBounds(markerX, row2Y - markerHeight, markerWidth, markerHeight);
    rowLabel4.setBounds(markerX, row4Y - markerHeight, markerWidth, markerHeight);
    rowLabel5.setBounds(markerX, row5Y - markerHeight, markerWidth, markerHeight);
    rowLabel6.setBounds(markerX, row6Y - markerHeight, markerWidth, markerHeight);
}

// PHASE 8: Pattern group management methods - simplified for Row4Component delegation

void MainContentComponent::initializePatternGroups() {
    // PHASE 8: Row4Component handles pattern group initialization internally
    // No external initialization needed - component is self-contained
}

void MainContentComponent::updatePatternGroupDropdown() {
    // PHASE 8: Row4Component handles dropdown updates internally
    // Trigger label update instead
    if (row4Component) {
        row4Component->updateLabelsFromControls();
    }
}

void MainContentComponent::handlePatternGroupNavigation(bool isNext) {
    if (row4Component) {
        row4Component->navigatePatternGroup(isNext);
    }
}

void MainContentComponent::togglePatternGroupEditMode() {
    if (row4Component) {
        bool currentMode = row4Component->isPatternGroupEditMode();
        row4Component->setPatternGroupEditMode(!currentMode);
    }
}

void MainContentComponent::togglePatternGroupFavorite() {
    if (row4Component) {
        row4Component->togglePatternGroupFavorite();
    }
}

void MainContentComponent::switchToPatternGroup(int groupIndex) {
    if (row4Component) {
        row4Component->setCurrentPatternGroupIndex(groupIndex);
    }
}

BeatsButtonGroup* MainContentComponent::getCurrentPatternGroup() {
    // PHASE 8: Row4Component doesn't expose internal pattern group objects
    // Return nullptr - pattern group management is internal to Row4Component
    return nullptr;
}

void MainContentComponent::updateRow4LabelsFromControls() {
    if (row4Component) {
        row4Component->updateLabelsFromControls();
    }
}

void MainContentComponent::updateRow4LabelStates() {
    // PHASE 8: Row4Component handles label state updates internally
    if (row4Component) {
        row4Component->updateLabelsFromControls();
    }
}

void MainContentComponent::refreshPatternGroupUI() {
    // PHASE 8: Row4Component handles UI refresh internally
    if (row4Component) {
        row4Component->updateLabelsFromControls();
    }
}

bool MainContentComponent::keyPressed(const juce::KeyPress& key) {
    // PHASE 8: Delegate keyboard shortcuts to appropriate Row components
    if (row1Component && row1Component->keyPressed(key)) return true;
    if (row2Component && row2Component->keyPressed(key)) return true;
    if (row3Component && row3Component->keyPressed(key)) return true;
    if (row4Component && row4Component->keyPressed(key)) return true;
    if (row5Component && row5Component->keyPressed(key)) return true;
    if (row6Component && row6Component->keyPressed(key)) return true;
    

void MainContentComponent::setupKeyboardHandlerCallbacks() {
    if (keyboardHandler) {
        keyboardHandler->onEditModeToggle = [this](bool editMode) {
            setEditModeVisuals(editMode);
        };

        keyboardHandler->onPlayerSwitch = [this](int playerIndex) {
            updatePlayerDisplay(playerIndex);
        };

        keyboardHandler->onDrumPadTrigger = [this](int padIndex) {
            if (row5Component) {
                row5Component->triggerDrumPad(padIndex);
            }
        };
    }
}

void MainContentComponent::setupContextMenuCallbacks() {
    if (contextMenuManager) {
        contextMenuManager->onPatternGroupAction = [this](const juce::String& action) {
            if (row4Component) {
                row4Component->handlePatternGroupAction(action);
            }
        };
    }
}

void MainContentComponent::setupAnimationManagers() {
    if (row4Component && animationManager) {
        row4Component->setAnimationManager(animationManager.get());
    }
    if (row5Component && animationManager) {
        row5Component->setAnimationManager(animationManager.get());
    }
}

void MainContentComponent::setEditModeVisuals(bool editMode) {
    if (row5Component) {
        row5Component->setEditModeVisuals(editMode);
    }
}

void MainContentComponent::updatePlayerDisplay(int playerIndex) {
    if (row5Component) {
        row5Component->setCurrentPlayerIndex(playerIndex);
    }
}

void MainContentComponent::setupKeyboardHandlerCallbacks() {
    keyboardHandler->onEditModeToggle = [this](bool editMode) {
        setEditModeVisuals(editMode);
    };

    keyboardHandler->onPlayerSwitch = [this](int playerIndex) {
        updatePlayerDisplay(playerIndex);
    };

    keyboardHandler->onDrumPadTrigger = [this](int padIndex) {
        if (row5Component) {
            row5Component->triggerDrumPad(padIndex);
        }
    };
}

void MainContentComponent::setupContextMenuCallbacks() {
    contextMenuManager->onPatternGroupAction = [this](const juce::String& action) {
        if (row4Component) {
            row4Component->handlePatternGroupAction(action);
        }
    };
}

void MainContentComponent::setupAnimationManagers() {
    if (row4Component) {
        row4Component->setAnimationManager(animationManager.get());
    }
    if (row5Component) {
        row5Component->setAnimationManager(animationManager.get());
    }
}

void MainContentComponent::setEditModeVisuals(bool editMode) {
    juce::ignoreUnused(editMode);
}

void MainContentComponent::updatePlayerDisplay(int playerIndex) {
    juce::ignoreUnused(playerIndex);
}


    return juce::Component::keyPressed(key);
}

#ifdef JUCE_DEBUG
void MainContentComponent::performIntegrationValidation(const juce::Rectangle<int>& bounds) {
    // PHASE 8: Validation handled by individual Row components
    if (row1Component) row1Component->performValidation();
    if (row2Component) row2Component->performValidation();
    if (row3Component) row3Component->performValidation();
    if (row4Component) row4Component->performValidation();
    if (row5Component) row5Component->performValidation();
    if (row6Component) row6Component->performValidation();
}
#endif
