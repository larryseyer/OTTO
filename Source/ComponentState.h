#pragma once
#include <JuceHeader.h>
#include "INIConfig.h"
#include "INIUtils.h"

struct GlobalSettings {
    int settingsID = INIConfig::Defaults::DEFAULT_AUDIO_SETTINGS_ID;
    juce::String settingsName = "Default";
    int themeID = INIConfig::Defaults::DEFAULT_THEME_ID;
    int presetID = INIConfig::Defaults::DEFAULT_CURRENT_PRESET;
    int tempo = INIConfig::Defaults::DEFAULT_TEMPO;
    juce::String linkSyncMode = INIConfig::Defaults::DEFAULT_LINK_SYNC_MODE;
    int linkSyncValue = INIConfig::Defaults::DEFAULT_LINK_SYNC_VALUE;
    juce::String currentCloudConnection = INIConfig::Defaults::DEFAULT_CLOUD_CONNECTION;
    float interfaceScale = INIConfig::Defaults::DEFAULT_INTERFACE_SCALE;
    int interfaceStartingX = INIConfig::Defaults::DEFAULT_INTERFACE_X;
    int interfaceStartingY = INIConfig::Defaults::DEFAULT_INTERFACE_Y;
    int interfaceWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
    int interfaceHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
    bool metronomeEnabled = INIConfig::Defaults::DEFAULT_METRONOME_ENABLED;
    float metronomeVolume = INIConfig::Defaults::DEFAULT_METRONOME_VOLUME;
    juce::String metronomeSound = INIConfig::Defaults::DEFAULT_METRONOME_SOUND;
    int quantizeValue = INIConfig::Defaults::DEFAULT_QUANTIZE_VALUE;
    int countInBars = INIConfig::Defaults::DEFAULT_COUNT_IN_BARS;
    int audioSettingsID = INIConfig::Defaults::DEFAULT_AUDIO_SETTINGS_ID;
    juce::String lastModified;
    juce::String fileFormatVersion = INIConfig::FILE_FORMAT_VERSION;
    bool midiClockOut = INIConfig::Defaults::DEFAULT_MIDI_CLOCK_OUT;
    bool midiClockIn = INIConfig::Defaults::DEFAULT_MIDI_CLOCK_IN;
    int autoSaveInterval = INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL;
    juce::String currentThemeName = "Dark";
    float splashScreenOnTime = INIConfig::UI::SPLASH_SCREEN_ON_TIME;

    bool midiLearnEnabled = false;
    juce::String currentControllerPreset = "";
    bool programChangeEnabled = true;
    bool ledFeedbackEnabled = true;

    juce::String fontMainType = "Roboto";
    float fontMainSize = INIConfig::Fonts::BODY_DEFAULT_SIZE;
    juce::String fontSecondaryType = "Playfair Display";
    float fontSecondarySize = INIConfig::Fonts::HEADER_DEFAULT_SIZE;
    int phosphorWeight = INIConfig::Defaults::DEFAULT_PHOSPHOR_WEIGHT;
    bool enableAdvancedTypography = false;
    juce::String customFontPath = "";

    void setDefaults() {
        settingsID = INIConfig::Defaults::DEFAULT_AUDIO_SETTINGS_ID;
        settingsName = "Default";
        themeID = INIConfig::Defaults::DEFAULT_THEME_ID;
        presetID = INIConfig::Defaults::DEFAULT_CURRENT_PRESET;
        tempo = INIConfig::Defaults::DEFAULT_TEMPO;
        linkSyncMode = INIConfig::Defaults::DEFAULT_LINK_SYNC_MODE;
        linkSyncValue = INIConfig::Defaults::DEFAULT_LINK_SYNC_VALUE;
        currentCloudConnection = INIConfig::Defaults::DEFAULT_CLOUD_CONNECTION;
        interfaceScale = INIConfig::Defaults::DEFAULT_INTERFACE_SCALE;
        interfaceStartingX = INIConfig::Defaults::DEFAULT_INTERFACE_X;
        interfaceStartingY = INIConfig::Defaults::DEFAULT_INTERFACE_Y;
        interfaceWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
        interfaceHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
        metronomeEnabled = INIConfig::Defaults::DEFAULT_METRONOME_ENABLED;
        metronomeVolume = INIConfig::Defaults::DEFAULT_METRONOME_VOLUME;
        metronomeSound = INIConfig::Defaults::DEFAULT_METRONOME_SOUND;
        quantizeValue = INIConfig::Defaults::DEFAULT_QUANTIZE_VALUE;
        countInBars = INIConfig::Defaults::DEFAULT_COUNT_IN_BARS;
        audioSettingsID = INIConfig::Defaults::DEFAULT_AUDIO_SETTINGS_ID;
        lastModified = juce::Time::getCurrentTime().toString(true, true);
        fileFormatVersion = INIConfig::FILE_FORMAT_VERSION;
        midiClockOut = INIConfig::Defaults::DEFAULT_MIDI_CLOCK_OUT;
        midiClockIn = INIConfig::Defaults::DEFAULT_MIDI_CLOCK_IN;
        autoSaveInterval = INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL;
        splashScreenOnTime = INIConfig::UI::SPLASH_SCREEN_ON_TIME;
        currentThemeName = "Dark";
        midiLearnEnabled = false;
        currentControllerPreset = "";
        programChangeEnabled = true;
        ledFeedbackEnabled = true;

        fontMainType = "Roboto";
        fontMainSize = INIConfig::Fonts::BODY_DEFAULT_SIZE;
        fontSecondaryType = "Playfair Display";
        fontSecondarySize = INIConfig::Fonts::HEADER_DEFAULT_SIZE;
        phosphorWeight = INIConfig::Defaults::DEFAULT_PHOSPHOR_WEIGHT;
        enableAdvancedTypography = false;
        customFontPath = "";
    }

    bool isValid() const {
        return INIConfig::isValidSettingsID(settingsID) &&
               INIConfig::isValidThemeID(themeID) &&
               INIConfig::isValidTempo(tempo) &&
               INIConfig::isValidInterfaceScale(interfaceScale) &&
               INIConfig::isValidVolume(metronomeVolume) &&
               INIConfig::isValidQuantizeValue(quantizeValue) &&
               countInBars >= INIConfig::Validation::MIN_COUNT_IN_BARS &&
               countInBars <= INIConfig::Validation::MAX_COUNT_IN_BARS &&
               autoSaveInterval >= INIConfig::Validation::MIN_AUTO_SAVE_INTERVAL &&
               autoSaveInterval <= INIConfig::Validation::MAX_AUTO_SAVE_INTERVAL &&
               phosphorWeight >= INIConfig::Validation::MIN_PHOSPHOR_WEIGHT &&
               phosphorWeight <= INIConfig::Validation::MAX_PHOSPHOR_WEIGHT;
    }
};

struct ThemeSettings {
    int themeID = INIConfig::Defaults::DEFAULT_THEME_ID;
    juce::String themeName = "Dark";
    juce::String backgroundColor = "#2A2A2A";
    juce::String foregroundColor = "#FFFFFF";
    juce::String accentColor = "#7A7A7A";
    juce::String textColor = "#FFFFFF";
    juce::String borderColor = "#3A3A3A";
    juce::String fontMain = "Playfair Display";
    float fontMainSize = INIConfig::Fonts::HEADER_DEFAULT_SIZE;
    juce::String fontMainWeight = "Regular";
    juce::String fontSecondary = "Roboto";
    float fontSecondarySize = INIConfig::Fonts::BODY_DEFAULT_SIZE;
    juce::String fontSecondaryWeight = "Regular";
    juce::String buttonColor = "#4A4A4A";
    juce::String buttonHoverColor = "#6A6A6A";
    juce::String buttonActiveColor = "#5A5A5A";
    juce::String sliderTrackColor = "#3A3A3A";
    juce::String sliderThumbColor = "#7A7A7A";
    juce::String meterColorLow = "#4CAF50";
    juce::String meterColorMid = "#FFC107";
    juce::String meterColorHigh = "#F44336";
    juce::String gridLineColor = "#3A3A3A";
    juce::String patternActiveColor = "#7A7A7A";
    juce::String patternInactiveColor = "#4A4A4A";
    juce::String createdDate;
    juce::String modifiedDate;

    void setDefaults() {
        themeID = INIConfig::Defaults::DEFAULT_THEME_ID;
        themeName = "Dark";
        backgroundColor = "#2A2A2A";
        foregroundColor = "#FFFFFF";
        accentColor = "#7A7A7A";
        textColor = "#FFFFFF";
        borderColor = "#3A3A3A";
        fontMain = "Playfair Display";
        fontMainSize = INIConfig::Fonts::HEADER_DEFAULT_SIZE;
        fontMainWeight = "Regular";
        fontSecondary = "Roboto";
        fontSecondarySize = INIConfig::Fonts::BODY_DEFAULT_SIZE;
        fontSecondaryWeight = "Regular";
        buttonColor = "#4A4A4A";
        buttonHoverColor = "#6A6A6A";
        buttonActiveColor = "#5A5A5A";
        sliderTrackColor = "#3A3A3A";
        sliderThumbColor = "#7A7A7A";
        meterColorLow = "#4CAF50";
        meterColorMid = "#FFC107";
        meterColorHigh = "#F44336";
        gridLineColor = "#3A3A3A";
        patternActiveColor = "#7A7A7A";
        patternInactiveColor = "#4A4A4A";
        auto now = juce::Time::getCurrentTime().toString(true, true);
        createdDate = now;
        modifiedDate = now;
    }

    void setLightThemeDefaults() {
        themeID = INIConfig::UI::LIGHT_THEME_ID;
        themeName = "Light";
        backgroundColor = "#F5F5F5";
        foregroundColor = "#212121";
        accentColor = "#2196F3";
        textColor = "#212121";
        borderColor = "#E0E0E0";
        fontMain = "Playfair Display";
        fontMainSize = INIConfig::Fonts::HEADER_DEFAULT_SIZE;
        fontMainWeight = "Regular";
        fontSecondary = "Roboto";
        fontSecondarySize = INIConfig::Fonts::BODY_DEFAULT_SIZE;
        fontSecondaryWeight = "Regular";
        buttonColor = "#E0E0E0";
        buttonHoverColor = "#D0D0D0";
        buttonActiveColor = "#C0C0C0";
        sliderTrackColor = "#E0E0E0";
        sliderThumbColor = "#2196F3";
        meterColorLow = "#4CAF50";
        meterColorMid = "#FFC107";
        meterColorHigh = "#F44336";
        gridLineColor = "#E0E0E0";
        patternActiveColor = "#B0B0B0";
        patternInactiveColor = "#E0E0E0";
        auto now = juce::Time::getCurrentTime().toString(true, true);
        createdDate = now;
        modifiedDate = now;
    }

    void setClassicThemeDefaults() {
        themeID = INIConfig::UI::CLASSIC_THEME_ID;
        themeName = "Classic";
        backgroundColor = "#3C3C3C";
        foregroundColor = "#E8E8E8";
        accentColor = "#FF8C00";
        textColor = "#E8E8E8";
        borderColor = "#505050";
        fontMain = "Playfair Display";
        fontMainSize = INIConfig::Fonts::HEADER_DEFAULT_SIZE;
        fontMainWeight = "Regular";
        fontSecondary = "Roboto";
        fontSecondarySize = INIConfig::Fonts::BODY_DEFAULT_SIZE;
        fontSecondaryWeight = "Regular";
        buttonColor = "#5A5A5A";
        buttonHoverColor = "#7A7A7A";
        buttonActiveColor = "#6A6A6A";
        sliderTrackColor = "#505050";
        sliderThumbColor = "#FF8C00";
        meterColorLow = "#00FF00";
        meterColorMid = "#FFFF00";
        meterColorHigh = "#FF0000";
        gridLineColor = "#505050";
        patternActiveColor = "#FF8C00";
        patternInactiveColor = "#5A5A5A";
        auto now = juce::Time::getCurrentTime().toString(true, true);
        createdDate = now;
        modifiedDate = now;
    }

    bool isValid() const {
        return INIConfig::isValidThemeID(themeID) &&
               themeName.length() <= INIConfig::Validation::MAX_THEME_NAME_LENGTH &&
               themeName.isNotEmpty() &&
               INIConfig::isValidHexColor(backgroundColor) &&
               INIConfig::isValidHexColor(foregroundColor) &&
               INIConfig::isValidHexColor(accentColor) &&
               INIConfig::isValidHexColor(textColor) &&
               INIConfig::isValidHexColor(borderColor) &&
               INIConfig::isValidHexColor(buttonColor) &&
               INIConfig::isValidHexColor(buttonHoverColor) &&
               INIConfig::isValidHexColor(buttonActiveColor) &&
               INIConfig::isValidHexColor(sliderTrackColor) &&
               INIConfig::isValidHexColor(sliderThumbColor) &&
               INIConfig::isValidHexColor(meterColorLow) &&
               INIConfig::isValidHexColor(meterColorMid) &&
               INIConfig::isValidHexColor(meterColorHigh) &&
               INIConfig::isValidHexColor(gridLineColor) &&
               INIConfig::isValidHexColor(patternActiveColor) &&
               INIConfig::isValidHexColor(patternInactiveColor) &&
               fontMainSize > INIConfig::Validation::MIN_VOLUME && fontMainSize <= INIConfig::Fonts::MAX_FONT_SIZE &&
               fontSecondarySize > INIConfig::Validation::MIN_VOLUME && fontSecondarySize <= INIConfig::Fonts::MAX_FONT_SIZE;
    }
};

struct AudioSettings {
    int audioSettingsID = INIConfig::Defaults::DEFAULT_AUDIO_SETTINGS_ID;
    juce::String settingsName = "Standard";
    int sampleRate = INIConfig::Defaults::DEFAULT_SAMPLE_RATE;
    int bufferSize = INIConfig::Defaults::DEFAULT_BUFFER_SIZE;
    juce::String audioDeviceName = INIConfig::Defaults::DEFAULT_AUDIO_DEVICE;
    int inputChannels = INIConfig::Defaults::DEFAULT_INPUT_CHANNELS;
    int outputChannels = INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS;
    bool latencyCompensation = INIConfig::Defaults::DEFAULT_LATENCY_COMPENSATION;
    int bitDepth = INIConfig::Defaults::DEFAULT_BIT_DEPTH;
    bool enableASIO = INIConfig::Defaults::DEFAULT_ENABLE_ASIO;
    bool enableMultiCore = INIConfig::Defaults::DEFAULT_ENABLE_MULTICORE;

    juce::String midiInputDevice = "";
    juce::String midiOutputDevice = "";
    bool midiThru = false;

    void setDefaults() {
        audioSettingsID = INIConfig::Defaults::DEFAULT_AUDIO_SETTINGS_ID;
        settingsName = "Standard";
        sampleRate = INIConfig::Defaults::DEFAULT_SAMPLE_RATE;
        bufferSize = INIConfig::Defaults::DEFAULT_BUFFER_SIZE;
        audioDeviceName = INIConfig::Defaults::DEFAULT_AUDIO_DEVICE;
        inputChannels = INIConfig::Defaults::DEFAULT_INPUT_CHANNELS;
        outputChannels = INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS;
        latencyCompensation = INIConfig::Defaults::DEFAULT_LATENCY_COMPENSATION;
        bitDepth = INIConfig::Defaults::DEFAULT_BIT_DEPTH;
        enableASIO = INIConfig::Defaults::DEFAULT_ENABLE_ASIO;
        enableMultiCore = INIConfig::Defaults::DEFAULT_ENABLE_MULTICORE;
        midiInputDevice = "";
        midiOutputDevice = "";
        midiThru = false;
    }

    bool isValid() const {
        return INIConfig::isValidSettingsID(audioSettingsID) &&
               settingsName.isNotEmpty() &&
               INIConfig::isValidSampleRate(sampleRate) &&
               INIConfig::isValidBufferSize(bufferSize) &&
               inputChannels >= INIConfig::Validation::MIN_AUDIO_CHANNELS &&
               inputChannels <= INIConfig::Validation::MAX_AUDIO_CHANNELS &&
               outputChannels >= INIConfig::Validation::MIN_AUDIO_CHANNELS &&
               outputChannels <= INIConfig::Validation::MAX_AUDIO_CHANNELS &&
               (bitDepth == INIConfig::Audio::BIT_DEPTH_16 ||
                bitDepth == INIConfig::Audio::BIT_DEPTH_24 ||
                bitDepth == INIConfig::Audio::BIT_DEPTH_32);
    }
};

struct PlayerSettings {
    bool enabled = true;
    juce::String selectedDrumkit = INIConfig::Defaults::DEFAULT_DRUMKIT;
    float volume = INIConfig::Defaults::VOLUME;
    float pan = INIConfig::Audio::DEFAULT_PAN;
    bool muted = false;
    bool soloed = false;
    int midiChannel = INIConfig::Validation::MIN_MIDI_CHANNEL;
    int outputChannel = INIConfig::Validation::MIN_MIDI_CHANNEL;
    juce::String selectedMidiGroup = INIConfig::Defaults::DEFAULT_GROUP_PREFIX + "1";
    int selectedButton = INIConfig::Defaults::DEFAULT_SELECTED_BUTTON;
    juce::String assignedMidiFiles[INIConfig::Validation::MAX_BUTTON_INDEX + 1];
    float swingValue = INIConfig::Defaults::SWING;
    float energyValue = INIConfig::Defaults::ENERGY;
    float volumeValue = INIConfig::Defaults::VOLUME;
    bool toggleStates[INIConfig::UI::MAX_TOGGLE_STATES] = {false};
    bool fillStates[INIConfig::UI::MAX_FILL_STATES] = {false};
    int mixerPresets[INIConfig::Defaults::MAX_PLAYERS] = {0};

    bool patternChainEnabled = false;
    juce::Array<int> patternChainIndices;
    bool patternChainLoop = true;

    void setDefaults() {
        enabled = INIConfig::Defaults::DEFAULT_PLAYER_ENABLED;
        selectedDrumkit = INIConfig::Defaults::DEFAULT_DRUMKIT;
        volume = INIConfig::Defaults::VOLUME;
        pan = INIConfig::Audio::DEFAULT_PAN;
        muted = false;
        soloed = false;
        midiChannel = INIConfig::Validation::MIN_MIDI_CHANNEL;
        outputChannel = INIConfig::Validation::MIN_MIDI_CHANNEL;
        selectedMidiGroup = INIConfig::Defaults::DEFAULT_GROUP_PREFIX + "1";
        selectedButton = INIConfig::Defaults::DEFAULT_SELECTED_BUTTON;
        swingValue = INIConfig::Defaults::SWING;
        energyValue = INIConfig::Defaults::ENERGY;
        volumeValue = INIConfig::Defaults::VOLUME;
        for (int i = 0; i <= INIConfig::Validation::MAX_BUTTON_INDEX; ++i) {
            assignedMidiFiles[i] = "";
        }
        for (int i = 0; i < INIConfig::UI::MAX_TOGGLE_STATES; ++i) {
            toggleStates[i] = false;
        }
        for (int i = 0; i < INIConfig::UI::MAX_FILL_STATES; ++i) {
            fillStates[i] = false;
        }
        for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
            mixerPresets[i] = INIConfig::Defaults::DEFAULT_CURRENT_PRESET;
        }
        patternChainEnabled = false;
        patternChainIndices.clear();
        patternChainLoop = true;
    }

    bool isValid() const {
        return INIConfig::isValidVolume(volume) &&
               INIConfig::isValidPan(pan) &&
               INIConfig::isValidMidiChannel(midiChannel) &&
               INIConfig::isValidMidiChannel(outputChannel) &&
               INIConfig::isValidButtonIndex(selectedButton) &&
               INIConfig::isValidSwing(swingValue) &&
               INIConfig::isValidEnergy(energyValue) &&
               INIConfig::isValidVolume(volumeValue);
    }
};

struct BeatsButtonGroup {
    juce::String groupName;
    juce::Array<juce::String> midiFiles;
    bool isCustomGroup = false;
    int selectedButton = INIConfig::Defaults::DEFAULT_SELECTED_BUTTON;
    bool isFavorite = false;

    float averageSwing = INIConfig::Defaults::SWING;
    float averageVelocity = INIConfig::Defaults::FIXED_VELOCITY / INIConfig::MIDI::VELOCITY_DIVISOR;
    int timeSignatureNumerator = INIConfig::Defaults::TIME_SIGNATURE_NUMERATOR;
    int timeSignatureDenominator = INIConfig::Defaults::TIME_SIGNATURE_DENOMINATOR;
    float grooveTightness = INIConfig::Audio::DEFAULT_GROOVE_TIGHTNESS;

    BeatsButtonGroup() = default;
    BeatsButtonGroup(const juce::String& name) : groupName(name) {}
};

struct CustomGroupManagerState {
    juce::String pendingGroupName;
    juce::String currentEditingGroup;
    int currentEditingPlayerIndex = INIConfig::MIDI::INACTIVE_PATTERN;
    bool hasUnsavedChanges = false;
};

struct MidiMappingData {
    int ccNumber = INIConfig::MIDI::DEFAULT_CC_NUMBER;
    int channel = INIConfig::Validation::MIN_MIDI_CHANNEL - 1;
    juce::String parameterID;
    float minValue = INIConfig::Validation::MIN_VOLUME;
    float maxValue = INIConfig::Validation::MAX_VOLUME;
    bool enabled = true;
    bool sendFeedback = false;
    int feedbackType = INIConfig::MIDI::DEFAULT_FEEDBACK_TYPE;
};

struct MidiControllerPresetData {
    juce::String name;
    juce::String manufacturer;
    juce::Array<MidiMappingData> mappings;
    bool hasLEDFeedback = false;
};

struct MidiMappingState {
    int ccNumber = INIConfig::MIDI::DEFAULT_CC_NUMBER;
    int channel = INIConfig::Validation::MIN_MIDI_CHANNEL - 1;
    juce::String parameterID;
    float minValue = INIConfig::Validation::MIN_VOLUME;
    float maxValue = INIConfig::Validation::MAX_VOLUME;
    bool enabled = true;
};

class ComponentState {
public:
    ComponentState() {
        globalSettings.setDefaults();
        themeSettings.setDefaults();
        audioSettings.setDefaults();
        for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
            playerSettings[i].setDefaults();
        }
    }

    GlobalSettings globalSettings;
    ThemeSettings themeSettings;
    AudioSettings audioSettings;
    PlayerSettings playerSettings[INIConfig::Defaults::MAX_PLAYERS];

    std::unordered_map<int, bool> beatStates;
    std::unordered_map<juce::String, int> dropdownSelections;
    std::unordered_map<juce::String, float> sliderValues;
    std::unordered_map<int, bool> toggleStates;
    std::unordered_map<int, bool> fillStates;
    bool favoriteStates[INIConfig::Validation::MAX_BUTTON_INDEX + 1] = {false};

    int currentPlayer = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;
    juce::Array<BeatsButtonGroup> beatsButtonGroups;
    CustomGroupManagerState customGroupManagerState;

    int currentKit = INIConfig::Defaults::DEFAULT_KIT_INDEX;
    bool playState = false;
    bool editMode = false;
    bool playerEnabled = true;

    float interfaceScale = INIConfig::Defaults::DEFAULT_INTERFACE_SCALE;
    int interfaceWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
    int interfaceHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
    int interfaceStartingX = INIConfig::Defaults::DEFAULT_INTERFACE_X;
    int interfaceStartingY = INIConfig::Defaults::DEFAULT_INTERFACE_Y;

    int tempo = INIConfig::Defaults::DEFAULT_TEMPO;
    int currentPreset = INIConfig::Defaults::DEFAULT_CURRENT_PRESET;
    int phosphorIconWeight = INIConfig::Defaults::DEFAULT_PHOSPHOR_WEIGHT;
    
    // PHASE 7: Loop controls state
    float loopPosition = INIConfig::Defaults::DEFAULT_LOOP_POSITION;

    // DrumKit popup window state
    int drumKitWindowX = 100;
    int drumKitWindowY = 100;
    int drumKitWindowWidth = 500;
    int drumKitWindowHeight = 400;

    juce::Array<MidiMappingState> midiMappings;
    MidiControllerPresetData currentControllerPreset;
    juce::String currentLearnParameter;
    bool midiLearnActive = false;

    // Generic key-value storage for component-specific settings
    std::unordered_map<juce::String, juce::var> componentValues;

    // Generic setValue/getValue methods for component state management
    void setValue(const juce::String& key, const juce::var& value) {
        componentValues[key] = value;
    }

    juce::var getValue(const juce::String& key, const juce::var& defaultValue = juce::var()) const {
        auto it = componentValues.find(key);
        return (it != componentValues.end()) ? it->second : defaultValue;
    }

    // Convenience methods for specific types
    int getIntValue(const juce::String& key, int defaultValue = 0) const {
        auto it = componentValues.find(key);
        return (it != componentValues.end()) ? static_cast<int>(it->second) : defaultValue;
    }

    double getDoubleValue(const juce::String& key, double defaultValue = 0.0) const {
        auto it = componentValues.find(key);
        return (it != componentValues.end()) ? static_cast<double>(it->second) : defaultValue;
    }

    float getFloatValue(const juce::String& key, float defaultValue = 0.0f) const {
        auto it = componentValues.find(key);
        return (it != componentValues.end()) ? static_cast<float>(it->second) : defaultValue;
    }

    bool getBoolValue(const juce::String& key, bool defaultValue = false) const {
        auto it = componentValues.find(key);
        return (it != componentValues.end()) ? static_cast<bool>(it->second) : defaultValue;
    }

    juce::String getStringValue(const juce::String& key, const juce::String& defaultValue = juce::String()) const {
        auto it = componentValues.find(key);
        return (it != componentValues.end()) ? it->second.toString() : defaultValue;
    }

    bool isValid() const {
        if (!globalSettings.isValid() || !themeSettings.isValid() || !audioSettings.isValid()) {
            return false;
        }
        for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
            if (!playerSettings[i].isValid()) {
                return false;
            }
        }
        return true;
    }

    void setError(const juce::String& error) {
        lastError = error;
    }

    juce::String getLastError() const { return lastError; }
    bool hasError() const { return !lastError.isEmpty(); }
    void clearError() { lastError.clear(); }

private:
    juce::String lastError;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComponentState)
};
