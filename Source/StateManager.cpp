#include "StateManager.h"
#include "PluginEditor.h"


StateManager::StateManager() {
    createDefaultState();
}

void StateManager::createDefaultState() {
    currentState.currentPlayer = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;
    currentState.currentKit = INIConfig::Defaults::DEFAULT_KIT_INDEX;
    currentState.currentPreset = INIConfig::Defaults::DEFAULT_CURRENT_PRESET;
    currentState.tempo = INIConfig::Defaults::DEFAULT_TEMPO;
    currentState.playState = INIConfig::Defaults::DEFAULT_PLAY_STATE;
    currentState.editMode = INIConfig::Defaults::DEFAULT_EDIT_MODE;
    currentState.playerEnabled = INIConfig::Defaults::DEFAULT_PLAYER_ENABLED;

    currentState.interfaceWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
    currentState.interfaceHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
    currentState.interfaceScale = INIConfig::Defaults::DEFAULT_INTERFACE_SCALE;
    currentState.interfaceStartingX = INIConfig::Defaults::DEFAULT_INTERFACE_X;
    currentState.interfaceStartingY = INIConfig::Defaults::DEFAULT_INTERFACE_Y;

    currentState.sliderValues["swing"] = INIConfig::Defaults::SWING;
    currentState.sliderValues["energy"] = INIConfig::Defaults::ENERGY;
    currentState.sliderValues["volume"] = INIConfig::Defaults::VOLUME;
    currentState.sliderValues["loopPosition"] = INIConfig::LayoutConstants::stateManagerDefaultLoopPosition;

    currentState.globalSettings.setDefaults();
    currentState.themeSettings.setDefaults();
    currentState.audioSettings.setDefaults();

    for (int i = 0; i < 8; ++i) {
        currentState.playerSettings[i].setDefaults();
        currentState.playerSettings[i].selectedMidiGroup = INIConfig::Defaults::DEFAULT_GROUP_PREFIX + juce::String(i + 1);
    }

    initializeDefaultBeatsButtonGroups();
}

void StateManager::initializeDefaultBeatsButtonGroups() {
    currentState.beatsButtonGroups.clear();

    for (int i = 0; i < 8; ++i) {
        BeatsButtonGroup group(INIConfig::Defaults::DEFAULT_GROUP_PREFIX + juce::String(i + 1));
        group.isFavorite = INIConfig::Defaults::DEFAULT_FAVORITE_STATE;
        group.selectedButton = INIConfig::Defaults::DEFAULT_SELECTED_BUTTON;
        group.isCustomGroup = false;

        for (int j = 0; j < 16; ++j) {
            group.midiFiles.add("");
        }

        currentState.beatsButtonGroups.add(group);
    }
}

void StateManager::validatePlayerIndex(int playerIndex) const {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) {

    }
}

void StateManager::setCurrentPlayer(int player) {
    if (INIConfig::isValidPlayerIndex(player) && player != currentState.currentPlayer) {
        saveCurrentPlayerSettings();
        currentState.currentPlayer = player;
        loadCurrentPlayerSettings();
    }
}

void StateManager::saveCurrentPlayerSettings() {
    int currentPlayer = currentState.currentPlayer;
    if (INIConfig::isValidPlayerIndex(currentPlayer)) {
        auto& settings = currentState.playerSettings[currentPlayer];

        settings.swingValue = INIConfig::clampSwing(getSliderValue("swing"));
        settings.energyValue = INIConfig::clampEnergy(getSliderValue("energy"));
        settings.volumeValue = INIConfig::clampVolume(getSliderValue("volume"));

        for (int i = 0; i < 5; ++i) {
            settings.toggleStates[i] = getToggleState(i);
            settings.fillStates[i] = getFillState(i);
        }
    }
}

void StateManager::loadCurrentPlayerSettings() {
    int currentPlayer = currentState.currentPlayer;
    if (INIConfig::isValidPlayerIndex(currentPlayer)) {
        const auto& settings = currentState.playerSettings[currentPlayer];

        setSliderValue("swing", settings.swingValue);
        setSliderValue("energy", settings.energyValue);
        setSliderValue("volume", settings.volumeValue);

        for (int i = 0; i < 5; ++i) {
            currentState.toggleStates[i] = settings.toggleStates[i];
            currentState.fillStates[i] = settings.fillStates[i];
        }
    }
}

void StateManager::setMidiFileAssignment(int buttonIndex, const juce::String& midiFile) {
    if (INIConfig::isValidButtonIndex(buttonIndex)) {
        int currentPlayer = getCurrentPlayer();
        if (INIConfig::isValidPlayerIndex(currentPlayer)) {
            currentState.playerSettings[currentPlayer].assignedMidiFiles[buttonIndex] = midiFile;
        }
    }
}

juce::String StateManager::getMidiFileAssignment(int buttonIndex) const {
    if (INIConfig::isValidButtonIndex(buttonIndex)) {
        int currentPlayer = getCurrentPlayer();
        if (INIConfig::isValidPlayerIndex(currentPlayer)) {
            return currentState.playerSettings[currentPlayer].assignedMidiFiles[buttonIndex];
        }
    }
    return "";
}

void StateManager::setToggleState(int index, bool state) {
    if (index >= 0 && index < 5) {
        currentState.toggleStates[index] = state;

        int currentPlayer = getCurrentPlayer();
        if (INIConfig::isValidPlayerIndex(currentPlayer)) {
            currentState.playerSettings[currentPlayer].toggleStates[index] = state;
        }
    }
}

void StateManager::setFillState(int index, bool state) {
    if (index >= 0 && index < 5) {
        currentState.fillStates[index] = state;

        int currentPlayer = getCurrentPlayer();
        if (INIConfig::isValidPlayerIndex(currentPlayer)) {
            currentState.playerSettings[currentPlayer].fillStates[index] = state;
        }
    }
}

bool StateManager::getToggleState(int index) const {
    if (index >= 0 && index < 5) {
        auto it = currentState.toggleStates.find(index);
        return (it != currentState.toggleStates.end()) ? it->second : false;
    }
    return false;
}

bool StateManager::getFillState(int index) const {
    if (index >= 0 && index < 5) {
        auto it = currentState.fillStates.find(index);
        return (it != currentState.fillStates.end()) ? it->second : false;
    }
    return false;
}

void StateManager::setPlayerMidiGroup(int playerIndex, const juce::String& groupName) {
    validatePlayerIndex(playerIndex);
    if (INIConfig::isValidPlayerIndex(playerIndex)) {
        currentState.playerSettings[playerIndex].selectedMidiGroup = groupName;
    }
}

juce::String StateManager::getPlayerMidiGroup(int playerIndex) const {
    validatePlayerIndex(playerIndex);
    if (INIConfig::isValidPlayerIndex(playerIndex)) {
        return currentState.playerSettings[playerIndex].selectedMidiGroup;
    }
    return INIConfig::Defaults::DEFAULT_GROUP_PREFIX + "1";
}

void StateManager::setPlayerSelectedButton(int playerIndex, int buttonIndex) {
    validatePlayerIndex(playerIndex);
    if (INIConfig::isValidPlayerIndex(playerIndex) && INIConfig::isValidButtonIndex(buttonIndex)) {
        currentState.playerSettings[playerIndex].selectedButton = buttonIndex;
    }
}

int StateManager::getPlayerSelectedButton(int playerIndex) const {
    validatePlayerIndex(playerIndex);
    if (INIConfig::isValidPlayerIndex(playerIndex)) {
        return currentState.playerSettings[playerIndex].selectedButton;
    }
    return INIConfig::Defaults::DEFAULT_SELECTED_BUTTON;
}

void StateManager::setPlayerFileAssignment(int playerIndex, int buttonIndex, const juce::String& midiFile) {
    validatePlayerIndex(playerIndex);
    if (INIConfig::isValidPlayerIndex(playerIndex) && INIConfig::isValidButtonIndex(buttonIndex)) {
        currentState.playerSettings[playerIndex].assignedMidiFiles[buttonIndex] = midiFile;
    }
}

juce::String StateManager::getPlayerFileAssignment(int playerIndex, int buttonIndex) const {
    validatePlayerIndex(playerIndex);
    if (INIConfig::isValidPlayerIndex(playerIndex) && INIConfig::isValidButtonIndex(buttonIndex)) {
        return currentState.playerSettings[playerIndex].assignedMidiFiles[buttonIndex];
    }
    return "";
}

void StateManager::savePlayerCompleteState(int playerIndex, const juce::String& selectedGroup,
                                         int selectedButton, const juce::Array<juce::String>& fileAssignments) {
    validatePlayerIndex(playerIndex);
    if (INIConfig::isValidPlayerIndex(playerIndex)) {
        auto& settings = currentState.playerSettings[playerIndex];

        settings.selectedMidiGroup = selectedGroup;
        settings.selectedButton = INIConfig::clampButtonIndex(selectedButton);

        for (int i = 0; i < 16; ++i) {
            if (i < fileAssignments.size()) {
                settings.assignedMidiFiles[i] = fileAssignments[i];
            } else {
                settings.assignedMidiFiles[i] = "";
            }
        }
    }
}

void StateManager::loadPlayerCompleteState(int playerIndex, juce::String& selectedGroup,
                                         int& selectedButton, juce::Array<juce::String>& fileAssignments) {
    validatePlayerIndex(playerIndex);
    if (INIConfig::isValidPlayerIndex(playerIndex)) {
        const auto& settings = currentState.playerSettings[playerIndex];

        selectedGroup = settings.selectedMidiGroup;
        selectedButton = settings.selectedButton;

        fileAssignments.clear();
        for (int i = 0; i < 16; ++i) {
            fileAssignments.add(settings.assignedMidiFiles[i]);
        }
    }
}

void StateManager::copyComponentState(ComponentState& dest, const ComponentState& src) {
    dest.currentPlayer = src.currentPlayer;
    dest.currentKit = src.currentKit;
    dest.currentPreset = src.currentPreset;
    dest.tempo = src.tempo;
    dest.playState = src.playState;
    dest.editMode = src.editMode;
    dest.playerEnabled = src.playerEnabled;
    dest.interfaceWidth = src.interfaceWidth;
    dest.interfaceHeight = src.interfaceHeight;
    dest.interfaceScale = src.interfaceScale;
    dest.interfaceStartingX = src.interfaceStartingX;
    dest.interfaceStartingY = src.interfaceStartingY;

    dest.sliderValues = src.sliderValues;
    dest.toggleStates = src.toggleStates;
    dest.fillStates = src.fillStates;

    dest.globalSettings = src.globalSettings;
    dest.themeSettings = src.themeSettings;
    dest.audioSettings = src.audioSettings;

    for (int i = 0; i < 8; ++i) {
        dest.playerSettings[i] = src.playerSettings[i];
    }

    dest.beatsButtonGroups = src.beatsButtonGroups;
}

void StateManager::saveStates(ComponentState& state) {
    saveCurrentPlayerSettings();
    copyComponentState(state, currentState);
    ensureValidState();
}

void StateManager::loadStates(const ComponentState& state) {
    copyComponentState(currentState, state);
    ensureValidState();
    loadCurrentPlayerSettings();
}

void StateManager::saveComponentStates(const OTTOAudioProcessorEditor& editor) {
    saveCurrentPlayerSettings();
    syncPlayerSettingsWithState();
}

void StateManager::loadComponentStates(OTTOAudioProcessorEditor& editor) {
    loadCurrentPlayerSettings();
}

void StateManager::saveAllToINI() {
    try {
        saveCurrentPlayerSettings();
        ensureValidState();

        if (!INIManager.saveAllData(currentState)) {
                    } else {
                    }
    }
    catch (const std::exception& e) {

    }
}

void StateManager::loadAllFromINI() {
    try {
        if (!INIManager.loadAllData(currentState)) {
                        createDefaultState();
        } else {
            ensureValidState();
            loadCurrentPlayerSettings();
                    }
    }
    catch (const std::exception& e) {

        createDefaultState();
    }
}

bool StateManager::validateState() const {
    if (!INIConfig::isValidPlayerIndex(currentState.currentPlayer)) {

        return false;
    }

    if (!INIConfig::isValidTempo(currentState.tempo)) {

        return false;
    }

    if (!INIConfig::isValidInterfaceScale(currentState.interfaceScale)) {

        return false;
    }

    for (int i = 0; i < 8; ++i) {
        const auto& player = currentState.playerSettings[i];

        if (!INIConfig::isValidSwing(player.swingValue) ||
            !INIConfig::isValidEnergy(player.energyValue) ||
            !INIConfig::isValidVolume(player.volumeValue) ||
            !INIConfig::isValidButtonIndex(player.selectedButton)) {

            return false;
        }
    }

    if (!currentState.globalSettings.isValid()) {
                return false;
    }

    if (!currentState.themeSettings.isValid()) {
                return false;
    }

    if (!currentState.audioSettings.isValid()) {
                return false;
    }

    return true;
}

void StateManager::setMidiFileManager(MidiFileManager* manager) {
    INIManager.setMidiFileManager(manager);
}

void StateManager::createINIBackups() {
    if (INIManager.createBackups()) {
            } else {
            }
}

void StateManager::repairINIFiles() {
    if (INIManager.repairCorruptedFiles()) {
            } else {
            }
}

juce::StringArray StateManager::getINIValidationReport() const {
    return INIManager.getValidationReport();
}

bool StateManager::isFirstRun() const {
    return INIManager.isFirstRun();
}

void StateManager::syncPlayerSettingsWithState() {
    saveCurrentPlayerSettings();
    ensureValidState();
}

void StateManager::ensureValidState() {
    currentState.currentPlayer = INIConfig::clampPlayerIndex(currentState.currentPlayer);
    currentState.currentPreset = INIConfig::clampPresetIndex(currentState.currentPreset);
    currentState.tempo = INIConfig::clampTempo(currentState.tempo);
    currentState.interfaceScale = INIConfig::clampScale(currentState.interfaceScale);
    currentState.interfaceWidth = INIConfig::clampWindowSize(currentState.interfaceWidth);
    currentState.interfaceHeight = INIConfig::clampWindowSize(currentState.interfaceHeight);

    if (!currentState.globalSettings.isValid()) {
        currentState.globalSettings.setDefaults();
    }

    if (!currentState.themeSettings.isValid()) {
        currentState.themeSettings.setDefaults();
    }

    if (!currentState.audioSettings.isValid()) {
        currentState.audioSettings.setDefaults();
    }

    for (int i = 0; i < 8; ++i) {
        auto& player = currentState.playerSettings[i];

        player.swingValue = INIConfig::clampSwing(player.swingValue);
        player.energyValue = INIConfig::clampEnergy(player.energyValue);
        player.volumeValue = INIConfig::clampVolume(player.volumeValue);
        player.volume = INIConfig::clampVolume(player.volume);
        player.pan = INIConfig::clampPan(player.pan);
        player.selectedButton = INIConfig::clampButtonIndex(player.selectedButton);
        player.midiChannel = INIConfig::clampMidiChannel(player.midiChannel);

        if (player.selectedMidiGroup.isEmpty()) {
            player.selectedMidiGroup = INIConfig::Defaults::DEFAULT_GROUP_PREFIX + juce::String(i + 1);
        }

        if (player.selectedDrumkit.isEmpty()) {
            player.selectedDrumkit = INIConfig::Defaults::DEFAULT_DRUMKIT;
        }
    }

    if (currentState.beatsButtonGroups.isEmpty()) {
        initializeDefaultBeatsButtonGroups();
    }
}
