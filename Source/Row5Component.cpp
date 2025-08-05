#include "JUCE8_CODING_STANDARDS.h"
#include "Row5Component.h"
#include "MidiEngine.h"
#include "Mixer.h"
#include "ErrorHandling.h"
#include "Animation/AnimationManager.h"
#include "DragDrop/DragDropManager.h"
#include "UI/Visualizations/SpectrumAnalyzer.h"
#include <iostream>

Row5Component::Row5Component(MidiEngine& midiEngine,
                           Mixer& mixer,
                           juce::AudioProcessorValueTreeState& valueTreeState,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : ResponsiveComponent()
    , midiEngine(midiEngine)
    , mixer(mixer)
    , valueTreeState(valueTreeState)
    , layoutManager(layoutManager)
    , fontManager(fontManager)
    , colorScheme(colorScheme)
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
    ResponsiveComponent::resized(); // Call parent first
    updateInteractiveLayout();
    
    // PHASE 9D: Update spectrum analyzer bounds when component is resized
    if (spectrumAnalyzer && spectrumVisible) {
        updateSpectrumBounds();
    }
}

void Row5Component::triggerDrumPad(int padIndex) {
    if (padIndex >= 0 && padIndex < INIConfig::Audio::NUM_DRUM_PADS) {
        drumButtons[padIndex].triggerClick();
    }
}

void Row5Component::setupDragDropTargets() {
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        auto dragTarget = std::make_unique<DrumPadDragTarget>(*this, i);
        drumButtons[i].addMouseListener(dragTarget.get(), false);
        drumPadDragTargets.add(std::move(dragTarget));
    }
}

void Row5Component::setupHoverEffects() {
}

void Row5Component::setupRealTimeIndicators() {
    class BeatVisualizationTimer : public juce::Timer {
    public:
        BeatVisualizationTimer(Row5Component& parent) : parentComponent(parent) {}
        void timerCallback() override {
            parentComponent.updateBeatVisualization();
        }
    private:
        Row5Component& parentComponent;
    };
    
    beatVisualizationTimer = std::make_unique<BeatVisualizationTimer>(*this);
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
    auto bounds = getLocalBounds();
    
    // Use responsive calculations instead of hardcoded values
    int spacing = getResponsiveSpacing();
    int margin = getResponsiveMargin(8);
    
    // Calculate section dimensions responsively
    int leftSectionWidth = static_cast<int>(bounds.getWidth() * 0.6f); // 60% for drum grid
    int rightSectionWidth = static_cast<int>(bounds.getWidth() * 0.35f); // 35% for controls
    
    // LEFT SECTION: 4x4 Drum Pattern Grid
    auto leftSection = bounds.removeFromLeft(leftSectionWidth);
    leftSection = leftSection.reduced(margin);
    
    // Position drum buttons in 4x4 grid with responsive sizing
    int drumButtonSize = getResponsiveButtonSize();
    // Ensure drum buttons fit in grid with proper spacing
    int maxButtonSize = std::min(leftSection.getWidth() / 4, leftSection.getHeight() / 4) - spacing;
    drumButtonSize = std::min(drumButtonSize, maxButtonSize);
    
    int gridStartX = leftSection.getX() + (leftSection.getWidth() - (drumButtonSize * 4 + spacing * 3)) / 2;
    int gridStartY = leftSection.getY() + (leftSection.getHeight() - (drumButtonSize * 4 + spacing * 3)) / 2;
    
    // Update drum button font size responsively
    float drumFontSize = getResponsiveFontSize(12.0f);
    
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            int buttonIndex = row * 4 + col;
            int x = gridStartX + col * (drumButtonSize + spacing);
            int y = gridStartY + row * (drumButtonSize + spacing);
            drumButtons[buttonIndex].setBounds(x, y, drumButtonSize, drumButtonSize);
            drumButtons[buttonIndex].setFont(JUCE8_FONT(drumFontSize));
        }
    }
    
    // Left separator with responsive thickness
    int separatorThickness = juce::jmax(1, static_cast<int>(bounds.getHeight() * 0.01f));
    leftSeparator.setBounds(leftSectionWidth, 0, separatorThickness, bounds.getHeight());
    
    // Skip margin
    bounds.removeFromLeft(margin);
    
    // RIGHT SECTION: Interactive Controls
    auto rightSection = bounds.removeFromLeft(rightSectionWidth);
    rightSection = rightSection.reduced(margin);
    
    // Layout right section controls vertically with responsive sizing
    int controlHeight = static_cast<int>(drumButtonSize * 0.7f); // Scale with drum buttons
    
    // Update control font size responsively
    float controlFontSize = getResponsiveFontSize(10.0f);
    
    // Toggle buttons row
    auto toggleArea = rightSection.removeFromTop(controlHeight);
    int toggleButtonWidth = (toggleArea.getWidth() - spacing * (INIConfig::UI::MAX_TOGGLE_STATES - 1)) / INIConfig::UI::MAX_TOGGLE_STATES;
    for (int i = 0; i < INIConfig::UI::MAX_TOGGLE_STATES; ++i) {
        int x = toggleArea.getX() + i * (toggleButtonWidth + spacing);
        toggleButtons[i].setBounds(x, toggleArea.getY(), toggleButtonWidth, controlHeight);
        toggleButtons[i].setFont(JUCE8_FONT(controlFontSize));
    }
    
    rightSection.removeFromTop(spacing);
    
    // Fill buttons row
    auto fillArea = rightSection.removeFromTop(controlHeight);
    int fillButtonWidth = (fillArea.getWidth() - spacing * (INIConfig::UI::MAX_FILL_STATES - 1)) / INIConfig::UI::MAX_FILL_STATES;
    for (int i = 0; i < INIConfig::UI::MAX_FILL_STATES; ++i) {
        int x = fillArea.getX() + i * (fillButtonWidth + spacing);
        fillButtons[i].setBounds(x, fillArea.getY(), fillButtonWidth, controlHeight);
        fillButtons[i].setFont(JUCE8_FONT(controlFontSize));
    }
    
    rightSection.removeFromTop(spacing * 2);
    
    // Sliders (remaining space divided equally) with responsive font
    int sliderHeight = (rightSection.getHeight() - spacing * 2) / 3;
    float sliderFontSize = getResponsiveFontSize(11.0f);
    
    swingSlider.setBounds(rightSection.removeFromTop(sliderHeight));
    swingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    rightSection.removeFromTop(spacing);
    
    energySlider.setBounds(rightSection.removeFromTop(sliderHeight));
    energySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    rightSection.removeFromTop(spacing);
    
    volumeSlider.setBounds(rightSection);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    // Right separator
    rightSeparator.setBounds(bounds.getRight() - separatorThickness, 0, 
                           separatorThickness, bounds.getHeight());
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
    // PHASE 9D: Integrate gesture recognition
    if (gestureRecognizer) {
        gestureRecognizer->handleMouseDown(event);
    }
    
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

//==============================================================================
// PHASE 9D: SpectrumAnalyzer Integration Implementation
//==============================================================================

void Row5Component::setSpectrumAnalyzer(SpectrumAnalyzer* analyzer) {
    spectrumAnalyzer = analyzer;
    if (spectrumAnalyzer) {
        setupSpectrumIntegration();
    }
}

void Row5Component::updateSpectrumAnalyzer(const juce::AudioBuffer<float>& buffer) {
    if (spectrumAnalyzer && spectrumVisible) {
        spectrumAnalyzer->processAudioBlock(buffer);
    }
}

void Row5Component::showSpectrumVisualization(bool show) {
    spectrumVisible = show;
    if (spectrumAnalyzer) {
        spectrumAnalyzer->setVisible(show);
        if (show) {
            updateSpectrumBounds();
        }
    }
}

bool Row5Component::isSpectrumVisible() const {
    return spectrumVisible && spectrumAnalyzer && spectrumAnalyzer->isVisible();
}

void Row5Component::setGestureRecognizer(GestureRecognizer* recognizer) {
    gestureRecognizer = recognizer;
}

void Row5Component::handleGestureInput(int gestureType, const juce::Point<float>& position, float velocity) {
    // Handle different gesture types for Row 5 interactions
    switch (gestureType) {
        case 0: // Swipe gesture
            if (velocity > 0.5f) {
                // Swipe right - next player
                if (currentPlayerIndex < 3) {
                    setCurrentPlayerIndex(currentPlayerIndex + 1);
                }
            } else if (velocity < -0.5f) {
                // Swipe left - previous player
                if (currentPlayerIndex > 0) {
                    setCurrentPlayerIndex(currentPlayerIndex - 1);
                }
            }
            break;
            
        case 1: // Long press gesture
            // Toggle edit mode on long press
            setEditModeVisuals(!isEditMode);
            break;
            
        case 2: // Pinch gesture
            // Scale drum pad sensitivity based on pinch
            if (velocity > 1.0f) {
                // Pinch out - increase sensitivity
                energySlider.setValue(juce::jmin(1.0, energySlider.getValue() + 0.1));
            } else if (velocity < 1.0f) {
                // Pinch in - decrease sensitivity
                energySlider.setValue(juce::jmax(0.0, energySlider.getValue() - 0.1));
            }
            break;
            
        default:
            break;
    }
}

void Row5Component::optimizeForTouch() {
    // Increase button and slider sizes for touch interfaces
    #if JUCE_IOS || JUCE_ANDROID
    auto bounds = getLocalBounds();
    
    // Ensure minimum touch target size (44px) using INI-driven calculations
    int minTouchSize = static_cast<int>(bounds.getHeight() * INIConfig::LayoutConstants::MIN_TOUCH_TARGET_HEIGHT_PERCENT / 100.0f);
    
    // Apply touch optimizations to drum buttons
    for (int i = 0; i < INIConfig::Audio::NUM_DRUM_PADS; ++i) {
        auto currentBounds = drumButtons[i].getBounds();
        drumButtons[i].setSize(juce::jmax(currentBounds.getWidth(), minTouchSize),
                              juce::jmax(currentBounds.getHeight(), minTouchSize));
    }
    
    // Apply touch optimizations to toggle buttons
    for (int i = 0; i < INIConfig::UI::MAX_TOGGLE_STATES; ++i) {
        auto currentBounds = toggleButtons[i].getBounds();
        toggleButtons[i].setSize(juce::jmax(currentBounds.getWidth(), minTouchSize),
                                juce::jmax(currentBounds.getHeight(), minTouchSize));
    }
    
    // Apply touch optimizations to fill buttons
    for (int i = 0; i < INIConfig::UI::MAX_FILL_STATES; ++i) {
        auto currentBounds = fillButtons[i].getBounds();
        fillButtons[i].setSize(juce::jmax(currentBounds.getWidth(), minTouchSize),
                              juce::jmax(currentBounds.getHeight(), minTouchSize));
    }
    
    // Increase slider thumb sizes for touch
    swingSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    energySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    #endif
}

//==============================================================================
// PHASE 9D: Spectrum Analyzer Helper Methods
//==============================================================================

void Row5Component::setupSpectrumIntegration() {
    if (!spectrumAnalyzer) return;
    
    // Configure spectrum analyzer for real-time audio analysis
    SpectrumAnalyzer::AnalyzerSettings spectrumSettings;
    spectrumSettings.backgroundColor = colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground);
    spectrumSettings.spectrumColor = colorScheme.getColor(ColorScheme::ColorRole::Accent);
    spectrumSettings.peakHoldColor = colorScheme.getColor(ColorScheme::ColorRole::Error);
    spectrumSettings.gridColor = colorScheme.getColor(ColorScheme::ColorRole::GridLine);
    spectrumSettings.fftSize = 2048;  // Good balance of resolution and performance
    spectrumSettings.overlapFactor = 4;
    spectrumSettings.windowType = SpectrumAnalyzer::WindowType::Hann;
    spectrumSettings.averagingMode = SpectrumAnalyzer::AveragingMode::Exponential;
    spectrumSettings.averagingFactor = 0.8f;
    spectrumSettings.minFrequency = 20.0f;
    spectrumSettings.maxFrequency = 20000.0f;
    spectrumSettings.minDecibels = -80.0f;
    spectrumSettings.maxDecibels = 0.0f;
    spectrumSettings.showGrid = true;
    spectrumSettings.showLabels = true;
    spectrumSettings.showPeakHold = true;
    spectrumSettings.showCursor = true;
    
    spectrumAnalyzer->setAnalyzerSettings(spectrumSettings);
    
    // Set sample rate from mixer
    double sampleRate = mixer.getSampleRate();
    if (sampleRate > 0) {
        spectrumAnalyzer->setSampleRate(sampleRate);
    }
}

void Row5Component::updateSpectrumBounds() {
    if (!spectrumAnalyzer) return;
    
    auto spectrumArea = getSpectrumArea();
    spectrumAnalyzer->setBounds(spectrumArea);
}

juce::Rectangle<int> Row5Component::getSpectrumArea() const {
    auto bounds = getLocalBounds();
    
    // Position spectrum analyzer in right 30% of Row 5, using INI-driven calculations
    // This leaves 70% for the drum grid and controls
    int spectrumX = static_cast<int>(bounds.getWidth() * INIConfig::LayoutConstants::ROW_5_SPECTRUM_X_PERCENT / 100.0f);
    int spectrumY = static_cast<int>(bounds.getHeight() * INIConfig::LayoutConstants::ROW_5_SPECTRUM_Y_PERCENT / 100.0f);
    int spectrumWidth = static_cast<int>(bounds.getWidth() * INIConfig::LayoutConstants::ROW_5_SPECTRUM_WIDTH_PERCENT / 100.0f);
    int spectrumHeight = static_cast<int>(bounds.getHeight() * INIConfig::LayoutConstants::ROW_5_SPECTRUM_HEIGHT_PERCENT / 100.0f);
    
    return juce::Rectangle<int>(spectrumX, spectrumY, spectrumWidth, spectrumHeight);
}

//==============================================================================
// ResponsiveComponent Implementation
//==============================================================================

void Row5Component::updateResponsiveLayout() {
    auto category = getCurrentDeviceCategory();
    
    // Device-specific adjustments for interactive controls
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
    
    resized();
}

int Row5Component::getResponsiveButtonSize() const {
    auto category = getCurrentDeviceCategory();
    auto rules = getCurrentLayoutRules();
    
    // Base size from available space - this is the main content area
    int baseSize = static_cast<int>(getHeight() * 0.15f); // 15% of row height for drum buttons
    
    // Apply device-specific adjustments
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            return juce::jmax(static_cast<int>(rules.sizing.minTouchTarget), baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            return juce::jmax(static_cast<int>(rules.sizing.minTouchTarget * 0.9f), baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            return juce::jmax(40, baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            return juce::jmax(48, static_cast<int>(baseSize * 1.2f));
        default:
            return juce::jmax(40, baseSize);
    }
}

int Row5Component::getResponsiveSpacing() const {
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
            return juce::jmax(6, baseSpacing);
        default:
            return baseSpacing;
    }
}

float Row5Component::getResponsiveFontSize(float baseSize) const {
    return ResponsiveComponent::getResponsiveFontSize(baseSize);
}
