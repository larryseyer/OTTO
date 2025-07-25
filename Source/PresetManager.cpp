#include "PresetManager.h"
#include "INIConfig.h"

PresetManager::PresetManager(MidiEngine& me, Mixer& m)
    : midiEngine(me), mixer(m) {
    currentPresetName = "Default";
    currentPresetIndex = 0;
}

void PresetManager::prepare() {
    if (dataManager) {
        updatePresetList();

        if (!presetExists("Default")) {
            ensureDefaultPreset();
        }
    }
}

void PresetManager::saveStates(ComponentState& state) {
    state.currentPreset = currentPresetIndex;
    state.sliderValues["currentPresetIndex"] = static_cast<float>(currentPresetIndex);
    state.dropdownSelections["currentPresetName"] = currentPresetIndex;

    captureEngineState(state);

    mixer.saveState(state);
}

void PresetManager::loadStates(const ComponentState& state) {
    currentPresetIndex = INIConfig::clampPresetIndex(state.currentPreset);
    currentPresetName = "Default";

    if (state.sliderValues.count("currentPresetIndex") > 0) {
        currentPresetIndex = INIConfig::clampPresetIndex(
            static_cast<int>(state.sliderValues.at("currentPresetIndex"))
        );
    }

    auto presets = getAvailablePresets();
    if (currentPresetIndex < presets.size()) {
        currentPresetName = presets[currentPresetIndex];
    }

    applyPresetToEngine(state);

    mixer.loadState(state);
}

void PresetManager::saveCurrentPreset() {
    if (!dataManager) {
        return;
    }

    ComponentState state;
    dataManager->loadAllData(state);
    saveStates(state);

    state.currentPreset = currentPresetIndex;
    state.sliderValues["currentPresetIndex"] = static_cast<float>(currentPresetIndex);

    if (!dataManager->saveAllData(state)) {

    } else {

    }
}

void PresetManager::loadPreset(int presetIndex) {
    if (!dataManager) {
        return;
    }

    currentPresetIndex = INIConfig::clampPresetIndex(presetIndex);

    auto presets = getAvailablePresets();
    if (currentPresetIndex < presets.size()) {
        loadPreset(presets[currentPresetIndex]);
    }
}

void PresetManager::loadPreset(const juce::String& presetName) {
    if (!dataManager || !presetExists(presetName)) {

        return;
    }

    currentPresetName = presetName;

    auto presets = getAvailablePresets();
    for (int i = 0; i < presets.size(); ++i) {
        if (presets[i] == presetName) {
            currentPresetIndex = i;
            break;
        }
    }

    ComponentState state;
    if (dataManager->loadAllData(state)) {
        state.currentPreset = currentPresetIndex;
        state.sliderValues["currentPresetIndex"] = static_cast<float>(currentPresetIndex);

        loadStates(state);

    } else {

    }
}

juce::StringArray PresetManager::getAvailablePresets() const {
    juce::StringArray presets;
    presets.add("Default");
    presets.add("Rock");
    presets.add("Jazz");
    presets.add("Electronic");
    presets.add("Hip Hop");
    presets.add("Latin");
    presets.add("Funk");
    presets.add("Custom");

    return presets;
}

void PresetManager::createPreset(const juce::String& presetName) {
    if (!isValidPresetName(presetName) || presetExists(presetName)) {

        return;
    }

    currentPresetName = presetName;
    auto presets = getAvailablePresets();
    currentPresetIndex = presets.size();

    saveCurrentPreset();
    updatePresetList();
}

void PresetManager::deletePreset(const juce::String& presetName) {
    if (presetName == "Default") {

        return;
    }

    updatePresetList();
}

void PresetManager::renamePreset(const juce::String& oldName, const juce::String& newName) {
    if (!presetExists(oldName) || !isValidPresetName(newName) || presetExists(newName)) {

        return;
    }

    if (currentPresetName == oldName) {
        currentPresetName = newName;
    }

    updatePresetList();
}

void PresetManager::saveState(juce::MemoryBlock& destData) {
    ComponentState state;
    if (dataManager) {
        dataManager->loadAllData(state);
    }
    saveStates(state);

    juce::ValueTree vt("PresetManagerState");
    vt.setProperty("currentPresetIndex", currentPresetIndex, nullptr);
    vt.setProperty("currentPresetName", currentPresetName, nullptr);
    vt.setProperty("tempo", state.tempo, nullptr);
    vt.setProperty("isPlaying", state.playState, nullptr);

    vt.setProperty("masterVolume", mixer.getMasterVolume(), nullptr);
    vt.setProperty("limiterEnabled", mixer.isLimiterEnabled(), nullptr);
    vt.setProperty("limiterThreshold", mixer.getLimiterThreshold(), nullptr);

    const auto& reverbState = mixer.getReverbState();
    vt.setProperty("reverbEnabled", reverbState.enabled.load(), nullptr);
    vt.setProperty("reverbMix", reverbState.mix.load(), nullptr);
    vt.setProperty("reverbRoomSize", reverbState.roomSize.load(), nullptr);

    const auto& delayState = mixer.getDelayState();
    vt.setProperty("delayEnabled", delayState.enabled.load(), nullptr);
    vt.setProperty("delayMix", delayState.mix.load(), nullptr);
    vt.setProperty("delayTime", delayState.delayTime.load(), nullptr);

    for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
        juce::ValueTree channelTree("Channel" + juce::String(i));
        channelTree.setProperty("volume", mixer.getChannelVolume(i), nullptr);
        channelTree.setProperty("pan", mixer.getChannelPan(i), nullptr);
        channelTree.setProperty("muted", mixer.isChannelMuted(i), nullptr);
        channelTree.setProperty("soloed", mixer.isChannelSoloed(i), nullptr);
        vt.addChild(channelTree, -1, nullptr);
    }

    for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
        juce::ValueTree playerTree("Player" + juce::String(i));
        const auto& player = state.playerSettings[i];
        playerTree.setProperty("enabled", player.enabled, nullptr);
        playerTree.setProperty("drumkit", player.selectedDrumkit, nullptr);
        playerTree.setProperty("swingValue", player.swingValue, nullptr);
        playerTree.setProperty("energyValue", player.energyValue, nullptr);
        vt.addChild(playerTree, -1, nullptr);
    }

    auto xml = vt.toXmlString();
    destData.replaceAll(xml.getCharPointer(), xml.getNumBytesAsUTF8());
}

void PresetManager::loadState(const void* data, int sizeInBytes) {
    juce::String xml(static_cast<const char*>(data), sizeInBytes);
    auto xmlDoc = juce::XmlDocument::parse(xml);

    if (xmlDoc) {
        juce::ValueTree vt = juce::ValueTree::fromXml(*xmlDoc);
        if (vt.hasType("PresetManagerState")) {
            ComponentState state;
            if (dataManager) {
                dataManager->loadAllData(state);
            }

            currentPresetIndex = vt.getProperty("currentPresetIndex", 0);
            currentPresetName = vt.getProperty("currentPresetName", "Default").toString();

            state.tempo = INIConfig::clampTempo(
                vt.getProperty("tempo", INIConfig::Defaults::DEFAULT_TEMPO).operator int()
            );
            state.playState = vt.getProperty("isPlaying", false);

            mixer.setMasterVolume(vt.getProperty("masterVolume", INIConfig::Defaults::DEFAULT_MASTER_VOLUME));
            mixer.setLimiterEnabled(vt.getProperty("limiterEnabled", true));
            mixer.setLimiterThreshold(vt.getProperty("limiterThreshold", -3.0f));

            mixer.setReverbEnabled(vt.getProperty("reverbEnabled", true));
            mixer.setReverbMix(vt.getProperty("reverbMix", INIConfig::Defaults::DEFAULT_REVERB_MIX));
            mixer.setReverbRoomSize(vt.getProperty("reverbRoomSize", 0.5f));

            mixer.setDelayEnabled(vt.getProperty("delayEnabled", true));
            mixer.setDelayMix(vt.getProperty("delayMix", INIConfig::Defaults::DEFAULT_DELAY_MIX));
            mixer.setDelayTime(vt.getProperty("delayTime", 250.0f));

            for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
                auto channelTree = vt.getChildWithName("Channel" + juce::String(i));
                if (channelTree.isValid()) {
                    mixer.setChannelVolume(i, channelTree.getProperty("volume", INIConfig::Defaults::DEFAULT_MASTER_VOLUME));
                    mixer.setChannelPan(i, channelTree.getProperty("pan", 0.0f));
                    mixer.setChannelMute(i, channelTree.getProperty("muted", false));
                    mixer.setChannelSolo(i, channelTree.getProperty("soloed", false));
                }
            }

            for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
                auto playerTree = vt.getChildWithName("Player" + juce::String(i));
                if (playerTree.isValid()) {
                    auto& player = state.playerSettings[i];
                    player.enabled = playerTree.getProperty("enabled", true);
                    player.selectedDrumkit = playerTree.getProperty("drumkit", INIConfig::Defaults::DEFAULT_DRUMKIT).toString();
                    player.swingValue = INIConfig::clampSwing(playerTree.getProperty("swingValue", INIConfig::Defaults::SWING));
                    player.energyValue = INIConfig::clampEnergy(playerTree.getProperty("energyValue", INIConfig::Defaults::ENERGY));
                }
            }

            loadStates(state);
        }
    }
}

void PresetManager::setCurrentPresetName(const juce::String& name) {
    if (isValidPresetName(name)) {
        currentPresetName = name;
    }
}

void PresetManager::setCurrentPresetIndex(int index) {
    currentPresetIndex = INIConfig::clampPresetIndex(index);
}

bool PresetManager::isValidPresetName(const juce::String& name) const {
    if (name.isEmpty() || name.length() > 50) {
        return false;
    }

    juce::String invalidChars = "/\\:*?\"<>|,\n\r\t";
    for (auto c : invalidChars) {
        if (name.contains(juce::String::charToString(c))) {
            return false;
        }
    }

    return true;
}

bool PresetManager::presetExists(const juce::String& name) const {
    return getAvailablePresets().contains(name);
}

void PresetManager::updatePresetList() {
    presetNames = getAvailablePresets();
    presetIndices.clear();
    for (int i = 0; i < presetNames.size(); ++i) {
        presetIndices.add(i);
    }
}

void PresetManager::applyPresetToEngine(const ComponentState& state) {
    MidiEngine::EngineState engineState;
    engineState.tempo = state.tempo;
    engineState.isPlaying = state.playState;

    engineState.swingValue = state.sliderValues.count("swingValue") > 0 ?
        state.sliderValues.at("swingValue") : INIConfig::Defaults::SWING;
    engineState.energyValue = state.sliderValues.count("energyValue") > 0 ?
        state.sliderValues.at("energyValue") : INIConfig::Defaults::ENERGY;

    engineState.currentPosition = 0;
    engineState.isFillActive = false;

    for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
        const auto& player = state.playerSettings[i];
        if (player.enabled) {
            for (int j = 0; j < INIConfig::LayoutConstants::drumButtonsPerRow * INIConfig::LayoutConstants::drumButtonRows; ++j) {
                if (player.assignedMidiFiles[j].isNotEmpty()) {
                    MidiEngine::EngineState::PatternInfo pattern;
                    pattern.group = i;
                    pattern.index = j;
                    pattern.name = "Pattern " + juce::String(j + 1);
                    pattern.midiFileName = player.assignedMidiFiles[j];
                    engineState.patterns.add(pattern);
                }
            }
        }
    }

    midiEngine.applyEngineState(engineState);
}

void PresetManager::captureEngineState(ComponentState& state) {
    auto engineState = midiEngine.getCurrentEngineState();

    state.tempo = INIConfig::clampTempo(engineState.tempo);
    state.playState = engineState.isPlaying;

    state.sliderValues["swingValue"] = INIConfig::clampSwing(engineState.swingValue);
    state.sliderValues["energyValue"] = INIConfig::clampEnergy(engineState.energyValue);

    for (const auto& pattern : engineState.patterns) {
        int playerIndex = pattern.group;
        if (INIConfig::isValidPlayerIndex(playerIndex)) {
            int buttonIndex = pattern.index;
            if (INIConfig::isValidButtonIndex(buttonIndex) && buttonIndex < INIConfig::LayoutConstants::drumButtonsPerRow * INIConfig::LayoutConstants::drumButtonRows) {
                state.playerSettings[playerIndex].assignedMidiFiles[buttonIndex] = pattern.midiFileName;
            }
        }
    }
}

void PresetManager::ensureDefaultPreset() {
    if (!dataManager) return;

    ComponentState defaultState;
    dataManager->initializeDefaults(defaultState);

    currentPresetName = "Default";
    currentPresetIndex = 0;

    saveStates(defaultState);
    dataManager->saveAllData(defaultState);

}

bool PresetManager::validatePresetState(const ComponentState& state) const {
    if (!INIConfig::isValidTempo(state.tempo)) {
        return false;
    }

    if (state.sliderValues.count("masterVolume") > 0) {
        float masterVolume = state.sliderValues.at("masterVolume");
        if (!INIConfig::isValidVolume(masterVolume)) {
            return false;
        }
    }

    if (state.sliderValues.count("swingValue") > 0) {
        float swingValue = state.sliderValues.at("swingValue");
        if (!INIConfig::isValidSwing(swingValue)) {
            return false;
        }
    }

    if (state.sliderValues.count("energyValue") > 0) {
        float energyValue = state.sliderValues.at("energyValue");
        if (!INIConfig::isValidEnergy(energyValue)) {
            return false;
        }
    }

    if (!INIConfig::isValidPresetIndex(state.currentPreset)) {
        return false;
    }

    return true;
}
