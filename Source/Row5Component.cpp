#include "Row5Component.h"
#include "MidiEngine.h"
#include "Mixer.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "ErrorHandling.h"
#include "Animation/AnimationManager.h"
#include "DragDrop/DragDropManager.h"
#include <iostream>

Row5Component::Row5Component(MidiEngine& midiEngine,
                           Mixer& mixer,
                           juce::AudioProcessorValueTreeState& valueTreeState,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : RowComponentBase(5, layoutManager, fontManager, colorScheme)
    , midiEngine(midiEngine)
    , mixer(mixer)
    , valueTreeState(valueTreeState)
    , leftSeparator(colorScheme)
    , rightSeparator(colorScheme) {
    
    setupInteractiveComponents();
    setupDragDropTargets();
    setupHoverEffects();
    setupRealTimeIndicators();
}

void Row5Component::paint(juce::Graphics& g) {
    // Row 5 background - transparent to show parent background
    g.fillAll(juce::Colours::transparentBlack);
}

void Row5Component::resized() {
    updateInteractiveLayout();
}

void Row5Component::triggerDrumPad(int padIndex) {
    if (padIndex >= 0 && padIndex < INIConfig::Audio::NUM_DRUM_PADS) {
        drumButtons[padIndex].triggerClick();
    }
}

void Row5Component::setupDragDropTargets() {
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        auto dragTarget = std::make_unique<DrumPadDragTarget>(*this, i);
        drumPadDragTargets.add(std::move(dragTarget));
        drumButtons[i].addMouseListener(drumPadDragTargets[i], false);
    }
}

void Row5Component::setupHoverEffects() {
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        auto& button = drumButtons[i];
        
        button.onMouseEnter = [this, i]() {
            if (animationManager && animationManager->shouldUseAnimations()) {
                drumButtons[i].setColour(juce::TextButton::buttonColourId,
                    colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground).brighter(0.1f));
            }
        };
        
        button.onMouseExit = [this, i]() {
            drumButtons[i].setColour(juce::TextButton::buttonColourId,
                colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        };
    }
}

void Row5Component::setupRealTimeIndicators() {
    beatVisualizationTimer = std::make_unique<juce::Timer>();
    beatVisualizationTimer->onTimer = [this]() {
        updateBeatVisualization();
    };
    
    beatVisualizationTimer->startTimerHz(60);
}

void Row5Component::updateBeatVisualization() {
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        bool isActive = false;
        if (isActive) {
            drumButtons[i].setColour(juce::TextButton::buttonColourId,
                colorScheme.getColor(ColorScheme::ColorRole::Accent));
        }
    }
}

void Row5Component::saveStates(ComponentState& state) {
    try {
        auto& playerState = state.playerSettings[currentPlayerIndex];
        
        // Save slider values
        playerState.swingValue = getSwingValue();
        playerState.energyValue = getEnergyValue();
        playerState.volumeValue = getVolumeValue();
        
        // Save toggle and fill states
        for (int i = 0; i < INIConfig::UI::MAX_TOGGLE_STATES; ++i) {
            playerState.toggleStates[i] = toggleStates[i];
        }
        
        for (int i = 0; i < INIConfig::UI::MAX_FILL_STATES; ++i) {
            playerState.fillStates[i] = fillStates[i];
        }
        
        // Save MIDI file assignments
        for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
            playerState.assignedMidiFiles[i] = assignedMidiFiles[i];
        }
        
        playerState.selectedButton = selectedDrumButton;
        // Note: editMode is handled at component level, not per-player
    }
    catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to save Row5Component states: " + juce::String(e.what()), "Row5Component");
    }
}

void Row5Component::loadStates(const ComponentState& state) {
    try {
        const auto& playerState = state.playerSettings[currentPlayerIndex];
        
        // Load slider values
        setSwingValue(playerState.swingValue);
        setEnergyValue(playerState.energyValue);
        setVolumeValue(playerState.volumeValue);
        
        // Load toggle and fill states
        for (int i = 0; i < INIConfig::UI::MAX_TOGGLE_STATES; ++i) {
            setToggleState(i, playerState.toggleStates[i]);
        }
        
        for (int i = 0; i < INIConfig::UI::MAX_FILL_STATES; ++i) {
            setFillState(i, playerState.fillStates[i]);
        }
        
        // Load MIDI file assignments
        for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
            setMidiFileAssignment(i, playerState.assignedMidiFiles[i]);
        }
        
        setSelectedDrumButton(playerState.selectedButton);
        // Note: editMode is handled at component level, not per-player
    }
    catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to load Row5Component states: " + juce::String(e.what()), "Row5Component");
    }
}

void Row5Component::updateFromState(const ComponentState& state) {
    loadStates(state);
}

juce::Rectangle<int> Row5Component::getRowBounds() const {
    using namespace INIConfig::LayoutConstants;
    return getScaledBounds(0, Row5::yPosition, 
                          INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH, 
                          Row5::height);
}

bool Row5Component::getToggleState(int index) const {
    if (index >= 0 && index < INIConfig::UI::MAX_TOGGLE_STATES) {
        return toggleStates[index];
    }
    return false;
}

void Row5Component::setToggleState(int index, bool state) {
    if (index >= 0 && index < INIConfig::UI::MAX_TOGGLE_STATES) {
        toggleStates[index] = state;
        toggleButtons[index].setToggleState(state, juce::dontSendNotification);
    }
}

bool Row5Component::getFillState(int index) const {
    if (index >= 0 && index < INIConfig::UI::MAX_FILL_STATES) {
        return fillStates[index];
    }
    return false;
}

void Row5Component::setFillState(int index, bool state) {
    if (index >= 0 && index < INIConfig::UI::MAX_FILL_STATES) {
        fillStates[index] = state;
        fillButtons[index].setToggleState(state, juce::dontSendNotification);
    }
}

void Row5Component::setEditModeVisuals(bool isEditing) {
    isEditMode = isEditing;
    
    // Update drum button appearance based on edit mode
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        auto& button = drumButtons[i];
        if (isEditing) {
            button.setColour(juce::TextButton::buttonColourId, 
                           colorScheme.getColor(ColorScheme::ColorRole::Accent));
            button.setColour(juce::TextButton::textColourOffId,
                           colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
        } else {
            button.setColour(juce::TextButton::buttonColourId,
                           colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
            button.setColour(juce::TextButton::textColourOffId,
                           colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        }
    }
    
    repaint();
}

juce::String Row5Component::getMidiFileAssignment(int buttonIndex) const {
    if (buttonIndex >= 0 && buttonIndex < INIConfig::Audio::NUM_DRUM_PADS) {
        return assignedMidiFiles[buttonIndex];
    }
    return {};
}

void Row5Component::setMidiFileAssignment(int buttonIndex, const juce::String& midiFile) {
    if (buttonIndex >= 0 && buttonIndex < INIConfig::Audio::NUM_DRUM_PADS) {
        assignedMidiFiles[buttonIndex] = midiFile;
        
        // Update button text to show assignment
        auto& button = drumButtons[buttonIndex];
        if (midiFile.isNotEmpty()) {
            button.setButtonText(midiFile.upToFirstOccurrenceOf(".", false, false));
        } else {
            button.setButtonText(juce::String(buttonIndex + 1));
        }
        
        // Notify parent of change
        if (onMidiFileChanged) {
            onMidiFileChanged(buttonIndex, midiFile);
        }
    }
}

void Row5Component::setSelectedDrumButton(int buttonIndex) {
    if (buttonIndex >= 0 && buttonIndex < INIConfig::Audio::NUM_DRUM_PADS) {
        // Clear previous selection
        if (selectedDrumButton >= 0 && selectedDrumButton < INIConfig::Audio::NUM_DRUM_PADS) {
            drumButtons[selectedDrumButton].setColour(juce::TextButton::buttonOnColourId,
                colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        }
        
        selectedDrumButton = buttonIndex;
        
        // Highlight new selection
        drumButtons[selectedDrumButton].setColour(juce::TextButton::buttonOnColourId,
            colorScheme.getColor(ColorScheme::ColorRole::Accent));
    }
}

void Row5Component::loadPlayerSpecificState(int playerIndex, const ComponentState& state) {
    currentPlayerIndex = INIConfig::clampPlayerIndex(playerIndex);
    loadStates(state);
}

void Row5Component::saveCurrentPlayerToState(ComponentState& state) {
    saveStates(state);
}

void Row5Component::setupInteractiveComponents() {
    // Setup drum grid (4x4 pattern matrix)
    setupDrumGrid();
    
    // Setup control buttons (toggles and fills)
    setupControlButtons();
    
    // Setup sliders (swing, energy, volume)
    setupSliders();
    
    // Setup separators
    addAndMakeVisible(leftSeparator);
    addAndMakeVisible(rightSeparator);
    
    // Setup callbacks
    updateDrumButtonCallbacks();
    updateButtonCallbacks();
    updateSliderCallbacks();
    
    // Apply initial styling
    updateFontsAndColors();
}

void Row5Component::updateInteractiveLayout() {
    using namespace INIConfig::LayoutConstants;
    
    auto bounds = getLocalBounds();
    
    // Calculate section dimensions using Row5 constants
    int leftSectionWidth = layoutManager.scaled(Row5::leftSectionWidth);
    int rightSectionWidth = layoutManager.scaled(Row5::rightSectionWidth);
    int sectionMargin = layoutManager.scaled(Row5::sectionMargin);
    
    // LEFT SECTION: 4x4 Drum Pattern Grid (60% width)
    auto leftSection = bounds.removeFromLeft(leftSectionWidth);
    leftSection = leftSection.reduced(layoutManager.scaled(defaultPadding));
    
    // Position drum buttons in 4x4 grid
    int buttonSize = std::min(leftSection.getWidth() / 4, leftSection.getHeight() / 4) - 
                     layoutManager.scaled(defaultSpacing);
    int gridStartX = leftSection.getX() + (leftSection.getWidth() - (buttonSize * 4 + layoutManager.scaled(defaultSpacing) * 3)) / 2;
    int gridStartY = leftSection.getY() + (leftSection.getHeight() - (buttonSize * 4 + layoutManager.scaled(defaultSpacing) * 3)) / 2;
    
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            int buttonIndex = row * 4 + col;
            int x = gridStartX + col * (buttonSize + layoutManager.scaled(defaultSpacing));
            int y = gridStartY + row * (buttonSize + layoutManager.scaled(defaultSpacing));
            drumButtons[buttonIndex].setBounds(x, y, buttonSize, buttonSize);
        }
    }
    
    // Left separator
    leftSeparator.setBounds(leftSectionWidth, 0, layoutManager.scaled(separatorThickness), bounds.getHeight());
    
    // Skip margin
    bounds.removeFromLeft(sectionMargin);
    
    // RIGHT SECTION: Interactive Controls (39.5% width)
    auto rightSection = bounds.removeFromLeft(rightSectionWidth);
    rightSection = rightSection.reduced(layoutManager.scaled(defaultPadding));
    
    // Layout right section controls vertically
    int controlHeight = layoutManager.scaled(30);
    int controlSpacing = layoutManager.scaled(defaultSpacing);
    
    // Toggle buttons row
    auto toggleArea = rightSection.removeFromTop(controlHeight);
    int toggleButtonWidth = toggleArea.getWidth() / INIConfig::UI::MAX_TOGGLE_STATES - controlSpacing;
    for (int i = 0; i < INIConfig::UI::MAX_TOGGLE_STATES; ++i) {
        int x = toggleArea.getX() + i * (toggleButtonWidth + controlSpacing);
        toggleButtons[i].setBounds(x, toggleArea.getY(), toggleButtonWidth, controlHeight);
    }
    
    rightSection.removeFromTop(controlSpacing);
    
    // Fill buttons row
    auto fillArea = rightSection.removeFromTop(controlHeight);
    int fillButtonWidth = fillArea.getWidth() / INIConfig::UI::MAX_FILL_STATES - controlSpacing;
    for (int i = 0; i < INIConfig::UI::MAX_FILL_STATES; ++i) {
        int x = fillArea.getX() + i * (fillButtonWidth + controlSpacing);
        fillButtons[i].setBounds(x, fillArea.getY(), fillButtonWidth, controlHeight);
    }
    
    rightSection.removeFromTop(controlSpacing * 2);
    
    // Sliders (remaining space divided equally)
    int sliderHeight = (rightSection.getHeight() - controlSpacing * 2) / 3;
    
    swingSlider.setBounds(rightSection.removeFromTop(sliderHeight));
    rightSection.removeFromTop(controlSpacing);
    
    energySlider.setBounds(rightSection.removeFromTop(sliderHeight));
    rightSection.removeFromTop(controlSpacing);
    
    volumeSlider.setBounds(rightSection);
    
    // Right separator
    rightSeparator.setBounds(bounds.getRight() - layoutManager.scaled(separatorThickness), 0, 
                           layoutManager.scaled(separatorThickness), bounds.getHeight());
}

void Row5Component::setupDrumGrid() {
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        auto& button = drumButtons[i];
        button.setButtonText(juce::String(i + 1));
        button.setClickingTogglesState(false);
        button.setComponentID("drum_button_" + juce::String(i));
        addAndMakeVisible(button);
    }
}

void Row5Component::setupControlButtons() {
    // Setup toggle buttons
    for (int i = 0; i < INIConfig::UI::MAX_TOGGLE_STATES; ++i) {
        auto& button = toggleButtons[i];
        button.setButtonText("T" + juce::String(i + 1));
        button.setClickingTogglesState(true);
        button.setComponentID("toggle_button_" + juce::String(i));
        addAndMakeVisible(button);
    }
    
    // Setup fill buttons
    for (int i = 0; i < INIConfig::UI::MAX_FILL_STATES; ++i) {
        auto& button = fillButtons[i];
        button.setButtonText("F" + juce::String(i + 1));
        button.setClickingTogglesState(true);
        button.setComponentID("fill_button_" + juce::String(i));
        addAndMakeVisible(button);
    }
}

void Row5Component::setupSliders() {
    // Swing slider
    swingSlider.setRange(INIConfig::Validation::MIN_SWING, INIConfig::Validation::MAX_SWING, 0.01);
    swingSlider.setValue(INIConfig::Defaults::SWING);
    swingSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    swingSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    swingSlider.setComponentID("swing_slider");
    addAndMakeVisible(swingSlider);
    
    // Energy slider
    energySlider.setRange(INIConfig::Validation::MIN_ENERGY, INIConfig::Validation::MAX_ENERGY, 0.01);
    energySlider.setValue(INIConfig::Defaults::ENERGY);
    energySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    energySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    energySlider.setComponentID("energy_slider");
    addAndMakeVisible(energySlider);
    
    // Volume slider
    volumeSlider.setRange(INIConfig::Validation::MIN_VOLUME, INIConfig::Validation::MAX_VOLUME, 0.01);
    volumeSlider.setValue(INIConfig::Defaults::VOLUME);
    volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    volumeSlider.setComponentID("volume_slider");
    addAndMakeVisible(volumeSlider);
    
    // Setup volume attachment for automation
    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "volume", volumeSlider);
}

void Row5Component::updateSliderCallbacks() {
    swingSlider.onValueChange = [this]() {
        if (onSliderValueChanged) {
            onSliderValueChanged("swing", static_cast<float>(swingSlider.getValue()));
        }
        if (onPlayerSliderValueChanged) {
            onPlayerSliderValueChanged(currentPlayerIndex, "swing", static_cast<float>(swingSlider.getValue()));
        }
    };
    
    energySlider.onValueChange = [this]() {
        if (onSliderValueChanged) {
            onSliderValueChanged("energy", static_cast<float>(energySlider.getValue()));
        }
        if (onPlayerSliderValueChanged) {
            onPlayerSliderValueChanged(currentPlayerIndex, "energy", static_cast<float>(energySlider.getValue()));
        }
    };
    
    volumeSlider.onValueChange = [this]() {
        if (onSliderValueChanged) {
            onSliderValueChanged("volume", static_cast<float>(volumeSlider.getValue()));
        }
        if (onPlayerSliderValueChanged) {
            onPlayerSliderValueChanged(currentPlayerIndex, "volume", static_cast<float>(volumeSlider.getValue()));
        }
    };
}

void Row5Component::updateButtonCallbacks() {
    // Toggle button callbacks
    for (int i = 0; i < INIConfig::UI::MAX_TOGGLE_STATES; ++i) {
        toggleButtons[i].onClick = [this, i]() {
            bool newState = toggleButtons[i].getToggleState();
            setToggleState(i, newState);
            if (onPlayerToggleChanged) {
                onPlayerToggleChanged(currentPlayerIndex, i, newState);
            }
        };
    }
    
    // Fill button callbacks
    for (int i = 0; i < INIConfig::UI::MAX_FILL_STATES; ++i) {
        fillButtons[i].onClick = [this, i]() {
            bool newState = fillButtons[i].getToggleState();
            setFillState(i, newState);
            if (onPlayerFillChanged) {
                onPlayerFillChanged(currentPlayerIndex, i, newState);
            }
        };
    }
}

void Row5Component::updateDrumButtonCallbacks() {
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        drumButtons[i].onClick = [this, i]() {
            onDrumButtonPressed(i);
        };
    }
}

void Row5Component::validateSliderValues() {
    // Ensure slider values are within valid ranges
    swingSlider.setValue(juce::jlimit(static_cast<double>(INIConfig::Validation::MIN_SWING), 
                                     static_cast<double>(INIConfig::Validation::MAX_SWING), 
                                     swingSlider.getValue()));
    energySlider.setValue(juce::jlimit(static_cast<double>(INIConfig::Validation::MIN_ENERGY), 
                                      static_cast<double>(INIConfig::Validation::MAX_ENERGY), 
                                      energySlider.getValue()));
    volumeSlider.setValue(juce::jlimit(static_cast<double>(INIConfig::Validation::MIN_VOLUME), 
                                      static_cast<double>(INIConfig::Validation::MAX_VOLUME), 
                                      volumeSlider.getValue()));
}

void Row5Component::updateFontsAndColors() {
    // Apply consistent fonts and colors to all components
    auto buttonFont = fontManager.getFont(FontManager::FontRole::Button, 12.0f);
    auto sliderFont = fontManager.getFont(FontManager::FontRole::Body, 11.0f);
    

class Row5Component::DrumPadDragTarget : public juce::FileDragAndDropTarget {
public:
    DrumPadDragTarget(Row5Component& parent, int padIndex) 
        : parentComponent(parent), padIndex(padIndex) {}
    
    bool isInterestedInFileDrag(const juce::StringArray& files) override {
        return files.size() == 1 && 
               (files[0].endsWith(".mid") || files[0].endsWith(".midi"));
    }
    
    void filesDropped(const juce::StringArray& files, int x, int y) override {
        juce::ignoreUnused(x, y);
        if (files.size() > 0) {
            parentComponent.setMidiFileAssignment(padIndex, files[0]);
            parentComponent.drumButtons[padIndex].setColour(
                juce::TextButton::buttonColourId,
                parentComponent.colorScheme.getColor(ColorScheme::ColorRole::Success));
        }
    }
    
    void fileDragEnter(const juce::StringArray& files, int x, int y) override {
        juce::ignoreUnused(files, x, y);
        parentComponent.drumButtons[padIndex].setColour(
            juce::TextButton::buttonColourId,
            parentComponent.colorScheme.getColor(ColorScheme::ColorRole::Accent));
    }
    
    void fileDragExit(const juce::StringArray& files) override {
        juce::ignoreUnused(files);
        parentComponent.drumButtons[padIndex].setColour(
            juce::TextButton::buttonColourId,
            parentComponent.colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
    }
    
private:
    Row5Component& parentComponent;
    int padIndex;
};

void Row5Component::setupDragDropTargets() {
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        auto dragTarget = std::make_unique<DrumPadDragTarget>(*this, i);
        drumPadDragTargets.add(std::move(dragTarget));
        drumButtons[i].addMouseListener(drumPadDragTargets[i], false);
    }
}

void Row5Component::setupHoverEffects() {
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        auto& button = drumButtons[i];
        
        button.onMouseEnter = [this, i]() {
            if (animationManager && animationManager->shouldUseAnimations()) {
                drumButtons[i].setColour(juce::TextButton::buttonColourId,
                    colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground).brighter(0.1f));
            }
        };
        
        button.onMouseExit = [this, i]() {
            drumButtons[i].setColour(juce::TextButton::buttonColourId,
                colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        };
    }
}

void Row5Component::setupRealTimeIndicators() {
    beatVisualizationTimer = std::make_unique<juce::Timer>();
    beatVisualizationTimer->onTimer = [this]() {
        updateBeatVisualization();
    };
    
    beatVisualizationTimer->startTimerHz(60);
}

void Row5Component::updateBeatVisualization() {
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        bool isActive = false;
        if (isActive) {
            drumButtons[i].setColour(juce::TextButton::buttonColourId,
                colorScheme.getColor(ColorScheme::ColorRole::Accent));
        }
    }
}

void Row5Component::triggerDrumPad(int padIndex) {
    if (padIndex >= 0 && padIndex < INIConfig::Audio::NUM_DRUM_PADS) {
        setSelectedDrumButton(padIndex);
        if (animationManager) {
            animationManager->animateButtonPress(drumButtons[padIndex], 150);
        }
    }
}

    // Drum buttons
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        auto& button = drumButtons[i];
        button.setColour(juce::TextButton::buttonColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        button.setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        button.setColour(juce::TextButton::buttonOnColourId, colorScheme.getColor(ColorScheme::ColorRole::Accent));
    }
    
    // Toggle buttons
    for (int i = 0; i < INIConfig::UI::MAX_TOGGLE_STATES; ++i) {
        auto& button = toggleButtons[i];
        button.setColour(juce::TextButton::buttonColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        button.setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        button.setColour(juce::TextButton::buttonOnColourId, colorScheme.getColor(ColorScheme::ColorRole::Accent));
    }
    
    // Fill buttons
    for (int i = 0; i < INIConfig::UI::MAX_FILL_STATES; ++i) {
        auto& button = fillButtons[i];
        button.setColour(juce::TextButton::buttonColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        button.setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        button.setColour(juce::TextButton::buttonOnColourId, colorScheme.getColor(ColorScheme::ColorRole::Accent));
    }
    
    // Sliders
    auto sliderColor = colorScheme.getColor(ColorScheme::ColorRole::SliderTrack);
    auto thumbColor = colorScheme.getColor(ColorScheme::ColorRole::SliderThumb);
    auto textColor = colorScheme.getColor(ColorScheme::ColorRole::PrimaryText);
    
    for (auto* slider : {&swingSlider, &energySlider, &volumeSlider}) {
        slider->setColour(juce::Slider::backgroundColourId, sliderColor);
        slider->setColour(juce::Slider::thumbColourId, thumbColor);
        slider->setColour(juce::Slider::textBoxTextColourId, textColor);
        slider->setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    }
}

void Row5Component::onDrumButtonPressed(int buttonIndex) {
    setSelectedDrumButton(buttonIndex);
    
    // In edit mode, allow MIDI file assignment
    if (isEditMode) {
        // TODO: Show MIDI file selection popup
        // For now, just notify parent
        if (onMidiFileChanged) {
            onMidiFileChanged(buttonIndex, assignedMidiFiles[buttonIndex]);
        }
    } else {
        // In play mode, trigger drum sound
        midiEngine.triggerClip(0, buttonIndex);
    }
}

void Row5Component::onDrumButtonRightClicked(int buttonIndex) {
    // Right-click always shows context menu regardless of edit mode
    setSelectedDrumButton(buttonIndex);
    
    // TODO: Show advanced context menu
    // For now, just notify parent
    if (onMidiFileChanged) {
        onMidiFileChanged(buttonIndex, assignedMidiFiles[buttonIndex]);
    }
}

void Row5Component::mouseDown(const juce::MouseEvent& event) {
    // Handle right-clicks on drum buttons
    if (event.mods.isRightButtonDown()) {
        for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
            if (drumButtons[i].getBounds().contains(event.getPosition())) {
                onDrumButtonRightClicked(i);
                return;
            }
        }
    }
    
    Component::mouseDown(event);
}
