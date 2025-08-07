#include "PresetManager.h"
#include "INIConfig.h"
#include "ErrorHandling.h"

PresetManager::PresetManager(MidiEngine& me, Mixer& m)
    : midiEngine(me), mixer(m) {
    currentPresetName = "Default";
    currentPresetIndex = 0;
}

void PresetManager::prepare() {
    // Null-pointer safety: Validate dataManager before use
    if (dataManager == nullptr) {
        DBG("PresetManager: Cannot prepare - dataManager is null");
        return;
    }

    try {
        ErrorHandler::safeExecute([&]() {
            updatePresetList();
        }, "PresetManager preset list update");

        if (!presetExists("Default")) {
            ErrorHandler::safeExecute([&]() {
                ensureDefaultPreset();
            }, "PresetManager default preset creation");
        }
    } catch (const std::exception& e) {
        DBG("PresetManager: Exception in prepare - " + juce::String(e.what()));
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
    try {
        // Null-pointer safety: Validate preset index before use
        currentPresetIndex = INIConfig::clampPresetIndex(state.currentPreset);
        currentPresetName = "Default";

        // Null-pointer safety: Safe access to slider values map
        if (state.sliderValues.count("currentPresetIndex") > 0) {
            try {
                float indexValue = state.sliderValues.at("currentPresetIndex");
                if (std::isfinite(indexValue)) {
                    currentPresetIndex = INIConfig::clampPresetIndex(static_cast<int>(indexValue));
                } else {
                    DBG("PresetManager: Invalid preset index value, using default");
                }
            } catch (const std::exception& e) {
                DBG("PresetManager: Exception accessing preset index - " + juce::String(e.what()));
            }
        }

        // Null-pointer safety: Validate preset list before accessing
        auto presets = getAvailablePresets();
        if (currentPresetIndex >= 0 && currentPresetIndex < presets.size()) {
            currentPresetName = presets[currentPresetIndex];
            if (currentPresetName.isEmpty()) {
                DBG("PresetManager: Empty preset name at index " + juce::String(currentPresetIndex));
                currentPresetName = "Default";
            }
        } else {
            DBG("PresetManager: Invalid preset index " + juce::String(currentPresetIndex) + 
                ", preset count: " + juce::String(presets.size()));
            currentPresetIndex = 0;
            currentPresetName = "Default";
        }

        // Null-pointer safety: Apply preset with error handling
        ErrorHandler::safeExecute([&]() {
            applyPresetToEngine(state);
        }, "PresetManager apply preset");

        // Null-pointer safety: Load mixer state with error handling
        ErrorHandler::safeExecute([&]() {
            mixer.loadState(state);
        }, "PresetManager mixer state loading");

    } catch (const std::exception& e) {
        DBG("PresetManager: Exception in loadStates - " + juce::String(e.what()));
        // Set safe defaults on error
        currentPresetIndex = 0;
        currentPresetName = "Default";
    }
}

void PresetManager::saveCurrentPreset() {
    if (!dataManager) {
        return;
    }

    ComponentState state;
    dataManager->loadAllData(state);
    captureEngineState(state);

    state.currentPreset = currentPresetIndex;
    state.sliderValues["currentPresetIndex"] = static_cast<float>(currentPresetIndex);

    // Save the preset to the presets directory
    dataManager->savePreset(currentPresetName, state);
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

    ComponentState state;
    if (dataManager->loadPreset(presetName, state)) {
        currentPresetName = presetName;

        // Update current index
        auto presets = getAvailablePresets();
        for (int i = 0; i < presets.size(); ++i) {
            if (presets[i] == presetName) {
                currentPresetIndex = i;
                break;
            }
        }

        state.currentPreset = currentPresetIndex;
        state.sliderValues["currentPresetIndex"] = static_cast<float>(currentPresetIndex);

        loadStates(state);
    }
}

juce::StringArray PresetManager::getAvailablePresets() const {
    if (dataManager) {
        return dataManager->getAvailablePresetNames();
    }
    
    // Fallback to hardcoded list if dataManager is not available
    juce::StringArray presets;
    presets.add("Default");
    return presets;
}

void PresetManager::createPreset(const juce::String& presetName) {
    if (!dataManager || !isValidPresetName(presetName) || presetExists(presetName)) {
        return;
    }

    // Capture current state
    ComponentState state;
    dataManager->loadAllData(state);
    captureEngineState(state);

    // Save the preset
    if (dataManager->savePreset(presetName, state)) {
        currentPresetName = presetName;
        updatePresetList();
        
        // Update current index
        auto presets = getAvailablePresets();
        for (int i = 0; i < presets.size(); ++i) {
            if (presets[i] == presetName) {
                currentPresetIndex = i;
                break;
            }
        }
    }
}

void PresetManager::deletePreset(const juce::String& presetName) {
    if (!dataManager || presetName == "Default") {
        return;
    }

    if (dataManager->deletePreset(presetName)) {
        // If we deleted the current preset, switch to Default
        if (currentPresetName == presetName) {
            currentPresetName = "Default";
            currentPresetIndex = 0;
            loadPreset("Default");
        }
        updatePresetList();
    }
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
    if (dataManager) {
        return dataManager->presetExists(name);
    }
    return name == "Default";
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

    // Create default preset if it doesn't exist
    if (!dataManager->presetExists("Default")) {
        dataManager->createDefaultPreset();
    }

    currentPresetName = "Default";
    currentPresetIndex = 0;
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
