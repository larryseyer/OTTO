#pragma once

#include <JuceHeader.h>
#include "ComponentState.h"
#include "INIDataManager.h"

class OTTOAudioProcessorEditor;
class MidiFileManager;

class StateManager {
public:
    StateManager();
    ~StateManager() = default;

    ComponentState& getState() { return currentState; }
    const ComponentState& getState() const { return currentState; }

    int getCurrentPlayer() const { return currentState.currentPlayer; }
    void setCurrentPlayer(int player);

    void saveCurrentPlayerSettings();
    void loadCurrentPlayerSettings();

    void setPlayState(bool playing) { currentState.playState = playing; }
    void setTempo(float tempo) { currentState.tempo = INIConfig::clampTempo(tempo); }
    void setCurrentPreset(int preset) { currentState.currentPreset = INIConfig::clampPresetIndex(preset); }
    void setCurrentKit(int kit) { currentState.currentKit = kit; }
    void setWindowSize(int width, int height) {
        currentState.interfaceWidth = INIConfig::clampWindowSize(width);
        currentState.interfaceHeight = INIConfig::clampWindowSize(height);
    }
    void setEditMode(bool editMode) { currentState.editMode = editMode; }
    void setPlayerEnabled(bool enabled) { currentState.playerEnabled = enabled; }

    void setSliderValue(const juce::String& sliderId, float value) {
        currentState.sliderValues[sliderId] = value;
    }
    float getSliderValue(const juce::String& sliderId) const {
        auto it = currentState.sliderValues.find(sliderId);
        return (it != currentState.sliderValues.end()) ? it->second : 0.0f;
    }

    void setMidiFileAssignment(int buttonIndex, const juce::String& midiFile);
    juce::String getMidiFileAssignment(int buttonIndex) const;

    void setToggleState(int index, bool state);
    void setFillState(int index, bool state);
    bool getToggleState(int index) const;
    bool getFillState(int index) const;

    void setPlayerMidiGroup(int playerIndex, const juce::String& groupName);
    juce::String getPlayerMidiGroup(int playerIndex) const;
    void setPlayerSelectedButton(int playerIndex, int buttonIndex);
    int getPlayerSelectedButton(int playerIndex) const;
    void setPlayerFileAssignment(int playerIndex, int buttonIndex, const juce::String& midiFile);
    juce::String getPlayerFileAssignment(int playerIndex, int buttonIndex) const;

    void savePlayerCompleteState(int playerIndex, const juce::String& selectedGroup,
                               int selectedButton, const juce::Array<juce::String>& fileAssignments);
    void loadPlayerCompleteState(int playerIndex, juce::String& selectedGroup,
                               int& selectedButton, juce::Array<juce::String>& fileAssignments);

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    void saveComponentStates(const OTTOAudioProcessorEditor& editor);
    void loadComponentStates(OTTOAudioProcessorEditor& editor);

    void saveAllToINI();
    void loadAllFromINI();
    bool validateState() const;
    void createDefaultState();

    INIDataManager& getINIManager() { return INIManager; }
    const INIDataManager& getINIManager() const { return INIManager; }
    void setMidiFileManager(MidiFileManager* manager);

    void createINIBackups();
    void repairINIFiles();
    juce::StringArray getINIValidationReport() const;
    bool isFirstRun() const;

    bool hasINIError() const { return INIManager.hasError(); }
    juce::String getINIError() const { return INIManager.getLastError(); }
    void clearINIError() { INIManager.clearError(); }

private:
    ComponentState currentState;
    INIDataManager INIManager;

    void validatePlayerIndex(int playerIndex) const;
    void initializeDefaultBeatsButtonGroups();
    void syncPlayerSettingsWithState();
    void ensureValidState();
    void copyComponentState(ComponentState& dest, const ComponentState& src);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StateManager)
};
