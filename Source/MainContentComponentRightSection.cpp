#include "MainContentComponentRightSection.h"
#include "CustomLookAndFeel.h"
#include "INIConfig.h"
#include "MidiEngine.h"

MainContentComponentRightSection::MainContentComponentRightSection(MidiEngine& midiEngine,
                                                                   Mixer& mixer,
                                                                   juce::AudioProcessorValueTreeState& valueTreeState,
                                                                   ResponsiveLayoutManager& layoutManager,
                                                                   FontManager& fontManager,
                                                                   ColorScheme& colorScheme)
    : midiEngine(midiEngine), mixer(mixer), valueTreeState(valueTreeState),
      layoutManager(layoutManager), fontManager(fontManager), colorScheme(colorScheme),
      swingSlider(), energySlider(), volumeSlider(),
      middleSeparator(colorScheme) {

    setupComponents();
    setupSliders();
    updateSliderCallbacks();
    updateButtonCallbacks();
}

void MainContentComponentRightSection::setupComponents() {
    // REMOVED: Labels are now managed by MainContentComponent Row 4
    // addAndMakeVisible(togglesLabel);
    // addAndMakeVisible(fillsLabel);
    // addAndMakeVisible(swingLabel);
    // addAndMakeVisible(energyLabel);
    // addAndMakeVisible(volumeLabel);
    // REMOVED: middleSeparator - no internal separators in unified 6-row system
    // addAndMakeVisible(middleSeparator);

    // REMOVED: Label configuration - now handled by MainContentComponent Row 4
    // MainContentComponent::setupRow4Label() method handles all label setup

    juce::String toggleNames[] = {"Auto", "Manual", "Stick", "Ride", "Lock"};
    for (int i = 0; i < 5; ++i) {
        addAndMakeVisible(toggleButtons[i]);
        toggleButtons[i].setButtonText(toggleNames[i]);
        toggleButtons[i].setColour(juce::TextButton::textColourOnId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        toggleButtons[i].setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        toggleButtons[i].setComponentID("toggles");
        toggleButtons[i].setClickingTogglesState(true);

        if (i < 2) {
            toggleButtons[i].setRadioGroupId(INIConfig::LayoutConstants::toggleButtonRadioGroup1);
        } else if (i >= 2 && i < 4) {
            toggleButtons[i].setRadioGroupId(INIConfig::LayoutConstants::toggleButtonRadioGroup2);
        }
    }

    juce::String fillNames[] = {"4", "8", "16", "32", "Solo"};
    for (int i = 0; i < 5; ++i) {
        addAndMakeVisible(fillButtons[i]);
        fillButtons[i].setButtonText(fillNames[i]);
        fillButtons[i].setColour(juce::TextButton::textColourOnId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        fillButtons[i].setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        fillButtons[i].setComponentID("fills");
        fillButtons[i].setClickingTogglesState(true);

        fillButtons[i].setRadioGroupId(INIConfig::LayoutConstants::fillButtonRadioGroup);
    }

    addAndMakeVisible(swingSlider);
    addAndMakeVisible(energySlider);
    addAndMakeVisible(volumeSlider);
}

void MainContentComponentRightSection::setupSliders() {
    swingSlider.setSliderStyle(juce::Slider::LinearVertical);
    swingSlider.setRange(INIConfig::Validation::MIN_SWING, INIConfig::Validation::MAX_SWING, 1.0);
    swingSlider.setValue(INIConfig::Defaults::SWING);
    swingSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    swingSlider.setColour(juce::Slider::backgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    swingSlider.setColour(juce::Slider::trackColourId, colorScheme.getColor(ColorScheme::ColorRole::SliderTrack));
    swingSlider.setColour(juce::Slider::thumbColourId, colorScheme.getColor(ColorScheme::ColorRole::SliderThumb));

    energySlider.setSliderStyle(juce::Slider::LinearVertical);
    energySlider.setRange(INIConfig::Validation::MIN_ENERGY, INIConfig::Validation::MAX_ENERGY, 1.0);
    energySlider.setValue(INIConfig::Defaults::ENERGY);
    energySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    energySlider.setColour(juce::Slider::backgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    energySlider.setColour(juce::Slider::trackColourId, colorScheme.getColor(ColorScheme::ColorRole::SliderTrack));
    energySlider.setColour(juce::Slider::thumbColourId, colorScheme.getColor(ColorScheme::ColorRole::SliderThumb));

    volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
    volumeSlider.setRange(INIConfig::Validation::MIN_VOLUME, INIConfig::Validation::MAX_VOLUME, 0.01);
    volumeSlider.setValue(INIConfig::Defaults::VOLUME);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.setColour(juce::Slider::backgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    volumeSlider.setColour(juce::Slider::trackColourId, colorScheme.getColor(ColorScheme::ColorRole::SliderTrack));
    volumeSlider.setColour(juce::Slider::thumbColourId, colorScheme.getColor(ColorScheme::ColorRole::SliderThumb));

    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, "masterVolume", volumeSlider);
}

void MainContentComponentRightSection::updateSliderCallbacks() {
    swingSlider.onValueChange = [this] {
        float value = INIConfig::clampSwing(static_cast<float>(swingSlider.getValue()));
        swingSlider.setValue(value, juce::dontSendNotification);

        if (onSliderValueChanged) {
            onSliderValueChanged("swing", value);
        }

        if (onPlayerSliderValueChanged) {
            onPlayerSliderValueChanged(currentPlayerIndex, "swing", value);
        }
    };

    energySlider.onValueChange = [this] {
        float value = INIConfig::clampEnergy(static_cast<float>(energySlider.getValue()));
        energySlider.setValue(value, juce::dontSendNotification);

        if (onSliderValueChanged) {
            onSliderValueChanged("energy", value);
        }

        if (onPlayerSliderValueChanged) {
            onPlayerSliderValueChanged(currentPlayerIndex, "energy", value);
        }
    };

    volumeSlider.onValueChange = [this] {
        float value = INIConfig::clampVolume(static_cast<float>(volumeSlider.getValue()));
        volumeSlider.setValue(value, juce::dontSendNotification);

        if (onSliderValueChanged) {
            onSliderValueChanged("volume", value);
        }

        if (onPlayerSliderValueChanged) {
            onPlayerSliderValueChanged(currentPlayerIndex, "volume", value);
        }
    };
}

void MainContentComponentRightSection::updateButtonCallbacks() {
    for (int i = 0; i < 5; ++i) {
        toggleButtons[i].onClick = [this, i] {
            toggleStates[i] = toggleButtons[i].getToggleState();

            if (onPlayerToggleChanged) {
                onPlayerToggleChanged(currentPlayerIndex, i, toggleStates[i]);
            }
        };

        fillButtons[i].onClick = [this, i] {
            midiEngine.triggerFill(currentPlayerIndex);
            fillStates[i] = fillButtons[i].getToggleState();

            if (onPlayerFillChanged) {
                onPlayerFillChanged(currentPlayerIndex, i, fillStates[i]);
            }
        };
    }
}

void MainContentComponentRightSection::validateSliderValues() {
    swingSlider.setValue(INIConfig::clampSwing(static_cast<float>(swingSlider.getValue())), juce::dontSendNotification);
    energySlider.setValue(INIConfig::clampEnergy(static_cast<float>(energySlider.getValue())), juce::dontSendNotification);
    volumeSlider.setValue(INIConfig::clampVolume(static_cast<float>(volumeSlider.getValue())), juce::dontSendNotification);
}

bool MainContentComponentRightSection::getToggleState(int index) const {
    if (index >= 0 && index < 5) {
        return toggleStates[index];
    }
    return false;
}

void MainContentComponentRightSection::setToggleState(int index, bool state) {
    if (index >= 0 && index < 5) {
        toggleStates[index] = state;
        toggleButtons[index].setToggleState(state, juce::dontSendNotification);
    }
}

bool MainContentComponentRightSection::getFillState(int index) const {
    if (index >= 0 && index < 5) {
        return fillStates[index];
    }
    return false;
}

void MainContentComponentRightSection::setFillState(int index, bool state) {
    if (index >= 0 && index < 5) {
        fillStates[index] = state;
        fillButtons[index].setToggleState(state, juce::dontSendNotification);
    }
}

void MainContentComponentRightSection::saveStates(ComponentState& state) {
    int playerIndex = INIConfig::clampPlayerIndex(currentPlayerIndex);
    auto& playerSettings = state.playerSettings[playerIndex];

    playerSettings.swingValue = INIConfig::clampSwing(getSwingValue());
    playerSettings.energyValue = INIConfig::clampEnergy(getEnergyValue());
    playerSettings.volumeValue = INIConfig::clampVolume(getVolumeValue());

    for (int i = 0; i < 5; ++i) {
        playerSettings.toggleStates[i] = toggleStates[i];
        playerSettings.fillStates[i] = fillStates[i];
    }
}

void MainContentComponentRightSection::loadStates(const ComponentState& state) {
    int playerIndex = INIConfig::clampPlayerIndex(currentPlayerIndex);
    const auto& playerSettings = state.playerSettings[playerIndex];

    setSwingValue(INIConfig::clampSwing(playerSettings.swingValue));
    setEnergyValue(INIConfig::clampEnergy(playerSettings.energyValue));
    setVolumeValue(INIConfig::clampVolume(playerSettings.volumeValue));

    for (int i = 0; i < 5; ++i) {
        setToggleState(i, playerSettings.toggleStates[i]);
        setFillState(i, playerSettings.fillStates[i]);
    }

    validateSliderValues();
}

void MainContentComponentRightSection::updatePlayerState(int playerIndex, const ComponentState& state) {
    playerIndex = INIConfig::clampPlayerIndex(playerIndex);
    currentPlayerIndex = playerIndex;

    const auto& playerSettings = state.playerSettings[playerIndex];

    setSwingValue(INIConfig::clampSwing(playerSettings.swingValue));
    setEnergyValue(INIConfig::clampEnergy(playerSettings.energyValue));
    setVolumeValue(INIConfig::clampVolume(playerSettings.volumeValue));

    for (int i = 0; i < 5; ++i) {
        setToggleState(i, playerSettings.toggleStates[i]);
        setFillState(i, playerSettings.fillStates[i]);
    }

    validateSliderValues();
}

void MainContentComponentRightSection::updateCurrentPlayerFromState(const ComponentState& state) {
    updatePlayerState(currentPlayerIndex, state);
}

void MainContentComponentRightSection::saveCurrentPlayerToState(ComponentState& state) {
    saveStates(state);
}

void MainContentComponentRightSection::paint(juce::Graphics& g) {
    // ========================================================================
    // CORRECTED: Remove internal separator - now part of unified 6-row system
    // All row separators are handled by MainContentComponent using INI constants
    // ========================================================================
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
    
    // No internal separators - the 6-row system handles all visual divisions
    // This component is positioned within Row 5 and should not draw additional separators
}

void MainContentComponentRightSection::resized() {
    // ========================================================================
    // CORRECTED: Use unified 6-row system - remove old topSectionHeightRatio logic
    // This component is positioned within Row 5 by MainContentComponent
    // Layout components using available space without internal separators
    // ========================================================================
    auto bounds = getLocalBounds();
    
    // Calculate top section height as a percentage of Row 5 content area
    // Use consistent percentage-based layout within the allocated Row 5 space
    int topSectionHeight = static_cast<int>(bounds.getHeight() * 0.2f); // 20% for top labels
    
    // REMOVED: Label positioning - now handled by MainContentComponent Row 4

    // REMOVED: middleSeparator - no internal separators in unified 6-row system
    // middleSeparator positioning removed - handled by MainContentComponent row separators

    // Position buttons and sliders in available space
    int buttonY = layoutManager.scaled(INIConfig::LayoutConstants::defaultPadding);
    int buttonWidth = layoutManager.scaled(INIConfig::LayoutConstants::rightSectionButtonWidth);
    int buttonHeight = layoutManager.scaled(INIConfig::LayoutConstants::rightSectionButtonHeight);
    int buttonSpacing = layoutManager.scaled(INIConfig::LayoutConstants::rightSectionButtonSpacing);

    int toggleCol = layoutManager.scaled(INIConfig::LayoutConstants::togglesLabelX);
    int fillCol = layoutManager.scaled(INIConfig::LayoutConstants::fillsLabelX);

    for (int i = 0; i < 5; ++i) {
        toggleButtons[i].setBounds(toggleCol, buttonY + (i * buttonSpacing), buttonWidth, buttonHeight);
        fillButtons[i].setBounds(fillCol, buttonY + (i * buttonSpacing), buttonWidth, buttonHeight);
    }

    int sliderWidth = layoutManager.scaled(INIConfig::LayoutConstants::sliderWidth);
    int sliderHeight = layoutManager.scaled(INIConfig::LayoutConstants::sliderHeight);
    int sliderY = buttonY;

    int sliderCol1 = layoutManager.scaled(INIConfig::LayoutConstants::swingLabelX);
    int sliderCol2 = layoutManager.scaled(INIConfig::LayoutConstants::energyLabelX);
    int sliderCol3 = layoutManager.scaled(INIConfig::LayoutConstants::volumeLabelX);

    swingSlider.setBounds(sliderCol1, sliderY, sliderWidth, sliderHeight);
    energySlider.setBounds(sliderCol2, sliderY, sliderWidth, sliderHeight);
    volumeSlider.setBounds(sliderCol3, sliderY, sliderWidth, sliderHeight);

    swingSlider.repaint();
    energySlider.repaint();
    volumeSlider.repaint();
}
