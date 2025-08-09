#pragma once
#include <JuceHeader.h>
#include "ComponentState.h"
#include "INIConfig.h"
#include "INIUtils.h"
#include "MidiEngine.h"
#include "MidiAnalysisTypes.h"

class ResponsiveLayoutManager;
class FontManager;

class INIDataManager : private juce::Timer {
public:
    INIDataManager();
    ~INIDataManager() = default;

    bool saveGlobalSettings(const GlobalSettings& settings);
    bool loadGlobalSettings(GlobalSettings& settings);

    bool saveThemeSettings(const ThemeSettings& settings);
    bool loadThemeSettings(ThemeSettings& settings);
    bool loadAllThemes(juce::Array<ThemeSettings>& themes);
    bool saveAllThemes(const juce::Array<ThemeSettings>& themes);
    juce::StringArray getAvailableThemeNames();

    bool saveAudioSettings(const AudioSettings& settings);
    bool loadAudioSettings(AudioSettings& settings);

    bool savePlayerSettings(const juce::Array<PlayerSettings>& playerSettings);
    bool loadPlayerSettings(juce::Array<PlayerSettings>& playerSettings);

    bool savePatternGroups(const juce::Array<BeatsButtonGroup>& groups);
    bool loadPatternGroups(juce::Array<BeatsButtonGroup>& groups);

    bool createSamplePresets();
    bool createSamplePatternChains();
    bool createSampleDrumKits();
    bool createSampleMidiLayouts();
    bool createSampleChannelPresetGroups();
    bool createSampleChannelPresets();
    bool createSampleEQPresets();
    bool createSampleMasterChannelPresets();
    bool createSampleMidiDevices();
    bool createSampleFileIndex();

    bool saveAllSettings(const ComponentState& state);
    bool loadAllSettings(ComponentState& state);

    bool createDirectoryStructure();
    bool directoryStructureExists() const;
    bool createAllRequiredFiles();
    bool allFilesExist() const;

    bool createSampleGlobalSettings();
    bool createSampleThemeSettings();
    bool createSampleAudioSettings();
    bool createSamplePlayerSettings();
    bool createSamplePatternGroups();
    bool createAllSampleFiles();

    bool saveMidiAnalysisCache(const std::map<juce::String, MidiGrooveAnalysis>& cache);
    bool loadMidiAnalysisCache(std::map<juce::String, MidiGrooveAnalysis>& cache);

    bool saveControllerPresets(const juce::Array<MidiEngine::MidiControllerPreset>& presets);
    bool loadControllerPresets(juce::Array<MidiEngine::MidiControllerPreset>& presets);

    bool saveMidiMappings(const juce::Array<MidiEngine::MidiMapping>& mappings);
    bool loadMidiMappings(juce::Array<MidiEngine::MidiMapping>& mappings);

    bool saveGrooveTemplates(const juce::StringArray& templates);
    bool loadGrooveTemplates(juce::StringArray& templates);

    // Preset management functions
    bool savePreset(const juce::String& presetName, const ComponentState& state);
    bool loadPreset(const juce::String& presetName, ComponentState& state);
    bool deletePreset(const juce::String& presetName);
    bool presetExists(const juce::String& presetName);
    juce::StringArray getAvailablePresetNames();
    bool createDefaultPreset();
    bool ensureDefaultPresetExists();
    bool isValidPresetName(const juce::String& name) const;

    juce::Colour stringToColor(const juce::String& colorString);
    juce::String colorToString(const juce::Colour& color);

    juce::String getLastError() const { return lastError; }
    bool hasError() const { return !lastError.isEmpty(); }
    void clearError() { lastError.clear(); }

    juce::StringArray getValidationReport() const;
    bool isFirstRun() const { return !directoryStructureExists(); }

    void initializeDefaults(ComponentState& state);
    bool saveCompletePlayerState(int playerIndex, const ComponentState& state);
    bool loadCompletePlayerState(int playerIndex, ComponentState& state);
    void setMidiFileManager(void* manager) { midiFileManager = manager; }

    void cleanCorruptedFiles();
    bool validateAllFiles() const;
    bool createDefaultFiles();
    bool createBackups();
    bool repairCorruptedFiles();

    bool saveAllData(const ComponentState& state);
    bool loadAllData(ComponentState& state);

    void enableAutoSave(int intervalSeconds);
    bool saveWithVersion(const ComponentState& state, const juce::String& version);

    // Static convenience methods for component state management
    static bool loadComponentState(const juce::String& componentName, ComponentState& state);
    static bool saveComponentState(const juce::String& componentName, const ComponentState& state);

    // Enhanced database creation methods
    bool createEnhancedSystemFiles();
    bool createEnhancedSettingsFiles();
    bool createEnhancedUIFiles();
    bool createEnhancedThemesFiles();
    bool createEnhancedPlayersFiles();
    bool createEnhancedKitsFiles();
    bool createEnhancedPatternsFiles();
    bool createEnhancedMixFiles();
    bool createEnhancedPerformanceFiles();
    bool createEnhancedCacheFiles();
    bool createEnhancedPresetsFiles();

private:
    juce::String lastError;
    void* midiFileManager = nullptr;
    ComponentState* currentState = nullptr;
    int autoSaveInterval = 0;

    void setError(const juce::String& error);
    void timerCallback() override;

    juce::File getINIFilePath(const juce::String& filename) const;

    static juce::String getGlobalSettingsFilename() { return INIConfig::GLOBAL_SETTINGS_FILE; }
    static juce::String getThemesFilename() { return INIConfig::THEMES_FILE; }
    static juce::String getAudioSettingsFilename() { return INIConfig::AUDIO_SETTINGS_FILE; }
    static juce::String getPlayersFilename() { return INIConfig::PLAYERS_FILE; }
    static juce::String getPatternGroupsFilename() { return INIConfig::PATTERN_GROUPS_FILE; }
    static juce::String getPresetsFilename() { return INIConfig::PRESETS_FILE; }
    static juce::String getMidiAnalysisCacheFilename() { return INIConfig::MIDI_ANALYSIS_CACHE_FILE; }
    static juce::String getControllerPresetsFilename() { return INIConfig::CONTROLLER_PRESETS_FILE; }
    static juce::String getMidiMappingsFilename() { return INIConfig::MIDI_MAPPINGS_FILE; }
    static juce::String getGrooveTemplatesFilename() { return INIConfig::GROOVE_TEMPLATES_FILE; }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(INIDataManager)
};
