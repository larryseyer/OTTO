#include "MainContentComponent.h"
#include "MainContentComponentLeftSection.h"
#include "MainContentComponentRightSection.h"
#include "SceneLauncherComponent.h"
#include "INIConfig.h"
#include "INIDataManager.h"
#include "ErrorHandling.h"

MainContentComponent::MainContentComponent(MidiEngine& midiEngine,
                                          Mixer& mixer,
                                          juce::AudioProcessorValueTreeState& valueTreeState,
                                          ResponsiveLayoutManager& layoutManager,
                                          FontManager& fontManager,
                                          ColorScheme& colorScheme)
    : midiEngine(midiEngine), mixer(mixer), valueTreeState(valueTreeState),
      layoutManager(layoutManager), fontManager(fontManager), colorScheme(colorScheme),
      topSeparator(colorScheme), bottomSeparator(colorScheme) {

    leftSection = std::make_unique<MainContentComponentLeftSection>(
        midiEngine, layoutManager, fontManager, colorScheme);
    rightSection = std::make_unique<MainContentComponentRightSection>(
        midiEngine, mixer, valueTreeState, layoutManager, fontManager, colorScheme);

    addAndMakeVisible(rhythmLabel);
    addAndMakeVisible(playerNumber);
    addAndMakeVisible(*leftSection);
    addAndMakeVisible(*rightSection);
    addAndMakeVisible(topSeparator);
    addAndMakeVisible(bottomSeparator);

    rhythmLabel.setComponentID("player_label");
    rhythmLabel.setText("PLAYER", juce::dontSendNotification);
    rhythmLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    rhythmLabel.setJustificationType(juce::Justification::centredRight);

    playerNumber.setComponentID("player_number");
    playerNumber.setText("PLAYER 1", juce::dontSendNotification);
    playerNumber.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    playerNumber.setJustificationType(juce::Justification::centredLeft);

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

    int headerHeight = layoutManager.scaled(INIConfig::LayoutConstants::mainHeaderHeight);
    int rhythmLabelWidth = layoutManager.scaled(INIConfig::LayoutConstants::rhythmLabelWidth);
    int playerNumberWidth = layoutManager.scaled(INIConfig::LayoutConstants::playerNumberWidth);

    rhythmLabel.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::rhythmLabelX),
                         layoutManager.scaled(INIConfig::LayoutConstants::rhythmLabelY),
                         rhythmLabelWidth,
                         layoutManager.scaled(INIConfig::LayoutConstants::rhythmLabelHeight));
    playerNumber.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::playerNumberX),
                          layoutManager.scaled(INIConfig::LayoutConstants::playerNumberY),
                          playerNumberWidth,
                          layoutManager.scaled(INIConfig::LayoutConstants::rhythmLabelHeight));

    topSeparator.setBounds(0, headerHeight, bounds.getWidth(), layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness));
    int currentY = headerHeight + layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness);

    int contentHeight = bounds.getHeight() - currentY - layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness);
    leftSection->setBounds(0, currentY, bounds.getWidth() / 2, contentHeight);
    rightSection->setBounds(bounds.getWidth() / 2, currentY, bounds.getWidth() / 2, contentHeight);

    bottomSeparator.setBounds(0, bounds.getBottom() - layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness),
                             bounds.getWidth(), layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness));
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
    playerNumber.setText(juce::String("PLAYER ") + juce::String(currentPlayerIndex + 1), juce::dontSendNotification);
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
