#pragma once

#include <JuceHeader.h>
#include "MidiEngine.h"
#include "Mixer.h"
#include "ComponentState.h"
#include "INIDataManager.h"

class PresetManager {
public:
    PresetManager(MidiEngine& midiEngine, Mixer& mixer);
    ~PresetManager() = default;

    void prepare();

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    void saveCurrentPreset();
    void loadPreset(int presetIndex);
    void loadPreset(const juce::String& presetName);

    juce::StringArray getAvailablePresets() const;
    void createPreset(const juce::String& presetName);
    void deletePreset(const juce::String& presetName);
    void renamePreset(const juce::String& oldName, const juce::String& newName);

    void saveState(juce::MemoryBlock& destData);
    void loadState(const void* data, int sizeInBytes);

    juce::String getCurrentPresetName() const { return currentPresetName; }
    void setCurrentPresetName(const juce::String& name);
    int getCurrentPresetIndex() const { return currentPresetIndex; }
    void setCurrentPresetIndex(int index);

    void setINIDataManager(INIDataManager* manager) { dataManager = manager; }

    bool isValidPresetName(const juce::String& name) const;
    bool presetExists(const juce::String& name) const;

private:
    MidiEngine& midiEngine;
    Mixer& mixer;
    INIDataManager* dataManager = nullptr;

    int currentPresetIndex = 0;
    juce::String currentPresetName = "Default";

    juce::StringArray presetNames;
    juce::Array<int> presetIndices;

    void updatePresetList();
    void applyPresetToEngine(const ComponentState& state);
    void captureEngineState(ComponentState& state);
    void ensureDefaultPreset();
    bool validatePresetState(const ComponentState& state) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};
