#include "INIDataManager.h"
#include "INIUtils.h"
#include "INIConfig.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "MidiAnalysisTypes.h"

INIDataManager::INIDataManager() {
    lastError.clear();

    if (!createDirectoryStructure()) {
        setError("Failed to create directory structure");
    }

    if (!allFilesExist()) {
        if (!createAllRequiredFiles()) {
            setError("Failed to create all required INI files");
        }
    }

    if (!allFilesExist()) {
        setError("Some INI files are still missing after creation attempt");
    }
}

bool INIDataManager::createDirectoryStructure() {
    bool success = true;

    // Create root DatabaseRoot directory
    auto dataDir = INIConfig::getOTTODataDirectory();
    if (!dataDir.exists()) {
        auto result = dataDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create OTTO data directory: " + result.getErrorMessage());
            success = false;
        }
    }

    auto settingsDir = INIConfig::getSettingsDirectory();
    if (!settingsDir.exists()) {
        auto result = settingsDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create settings directory: " + result.getErrorMessage());
            success = false;
        }
    }

    auto performanceDir = INIConfig::getPerformanceDirectory();
    if (!performanceDir.exists()) {
        auto result = performanceDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create performance directory: " + result.getErrorMessage());
            success = false;
        }
    }

    auto patternsDir = INIConfig::getPatternsDirectory();
    if (!patternsDir.exists()) {
        auto result = patternsDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create patterns directory: " + result.getErrorMessage());
            success = false;
        }
    }

    auto kitsDir = INIConfig::getKitsDirectory();
    if (!kitsDir.exists()) {
        auto result = kitsDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create kits directory: " + result.getErrorMessage());
            success = false;
        }
    }

    auto mixingDir = INIConfig::getMixingDirectory();
    if (!mixingDir.exists()) {
        auto result = mixingDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create mixing directory: " + result.getErrorMessage());
            success = false;
        }
    }

    auto systemDir = INIConfig::getSystemDirectory();
    if (!systemDir.exists()) {
        auto result = systemDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create system directory: " + result.getErrorMessage());
            success = false;
        }
    }

    auto presetsDir = INIConfig::getPresetsDirectory();
    if (!presetsDir.exists()) {
        auto result = presetsDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create presets directory: " + result.getErrorMessage());
            success = false;
        }
    }

    // ========================================================================
    // ENHANCED DATABASE SUBDIRECTORIES
    // ========================================================================

    // Settings/UI subfolder
    auto settingsUIDir = settingsDir.getChildFile(INIConfig::SETTINGS_UI_FOLDER);
    if (!settingsUIDir.exists()) {
        auto result = settingsUIDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create Settings/UI directory: " + result.getErrorMessage());
            success = false;
        }
    }

    // Themes/User subfolder
    auto themesDir = INIConfig::getOTTODataDirectory().getChildFile(INIConfig::THEMES_FOLDER);
    if (!themesDir.exists()) {
        auto result = themesDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create Themes directory: " + result.getErrorMessage());
            success = false;
        }
    }

    auto themesUserDir = themesDir.getChildFile(INIConfig::THEMES_USER_FOLDER);
    if (!themesUserDir.exists()) {
        auto result = themesUserDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create Themes/User directory: " + result.getErrorMessage());
            success = false;
        }
    }

    // Mix subfolders
    auto mixPresetsDir = mixingDir.getChildFile(INIConfig::MIX_PRESETS_FOLDER);
    if (!mixPresetsDir.exists()) {
        auto result = mixPresetsDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create Mix/Presets directory: " + result.getErrorMessage());
            success = false;
        }
    }

    auto mixChannelsDir = mixingDir.getChildFile(INIConfig::MIX_CHANNELS_FOLDER);
    if (!mixChannelsDir.exists()) {
        auto result = mixChannelsDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create Mix/Channels directory: " + result.getErrorMessage());
            success = false;
        }
    }

    auto mixMasterDir = mixingDir.getChildFile(INIConfig::MIX_MASTER_FOLDER);
    if (!mixMasterDir.exists()) {
        auto result = mixMasterDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create Mix/Master directory: " + result.getErrorMessage());
            success = false;
        }
    }

    // Presets subfolders
    auto presetsCategoriesDir = presetsDir.getChildFile(INIConfig::PRESETS_CATEGORIES_FOLDER);
    if (!presetsCategoriesDir.exists()) {
        auto result = presetsCategoriesDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create Presets/Categories directory: " + result.getErrorMessage());
            success = false;
        }
    }

    // Create style subfolders
    juce::StringArray styleSubfolders = {
        INIConfig::PRESETS_VINTAGE_FOLDER,
        INIConfig::PRESETS_MODERN_FOLDER,
        INIConfig::PRESETS_EXPERIMENTAL_FOLDER,
        INIConfig::PRESETS_USER_FOLDER
    };
    for (const auto& styleFolder : styleSubfolders) {
        auto styleFolderDir = presetsCategoriesDir.getChildFile(styleFolder);
        if (!styleFolderDir.exists()) {
            auto result = styleFolderDir.createDirectory();
            if (!result.wasOk()) {
                setError("Failed to create Presets/Categories/" + styleFolder + " directory: " + result.getErrorMessage());
                success = false;
            }
        }
    }

    auto presetsTemplatesDir = presetsDir.getChildFile(INIConfig::PRESETS_TEMPLATES_FOLDER);
    if (!presetsTemplatesDir.exists()) {
        auto result = presetsTemplatesDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create Presets/Templates directory: " + result.getErrorMessage());
            success = false;
        }
    }

    // Cache folder
    auto cacheDir = INIConfig::getOTTODataDirectory().getChildFile(INIConfig::CACHE_FOLDER);
    if (!cacheDir.exists()) {
        auto result = cacheDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create Cache directory: " + result.getErrorMessage());
            success = false;
        }
    }

    return success;
}

bool INIDataManager::directoryStructureExists() const {
    bool primaryDirsExist = INIConfig::getOTTODataDirectory().exists() &&
                           INIConfig::getSettingsDirectory().exists() &&
                           INIConfig::getPerformanceDirectory().exists() &&
                           INIConfig::getPatternsDirectory().exists() &&
                           INIConfig::getKitsDirectory().exists() &&
                           INIConfig::getMixingDirectory().exists() &&
                           INIConfig::getSystemDirectory().exists() &&
                           INIConfig::getPresetsDirectory().exists();

    bool enhancedDirsExist = INIConfig::getSettingsDirectory().getChildFile(INIConfig::SETTINGS_UI_FOLDER).exists() &&
                            INIConfig::getOTTODataDirectory().getChildFile(INIConfig::THEMES_FOLDER).exists() &&
                            INIConfig::getOTTODataDirectory().getChildFile(INIConfig::THEMES_FOLDER).getChildFile(INIConfig::THEMES_USER_FOLDER).exists() &&
                            INIConfig::getMixingDirectory().getChildFile(INIConfig::MIX_PRESETS_FOLDER).exists() &&
                            INIConfig::getMixingDirectory().getChildFile(INIConfig::MIX_CHANNELS_FOLDER).exists() &&
                            INIConfig::getMixingDirectory().getChildFile(INIConfig::MIX_MASTER_FOLDER).exists() &&
                            INIConfig::getPresetsDirectory().getChildFile(INIConfig::PRESETS_CATEGORIES_FOLDER).exists() &&
                            INIConfig::getPresetsDirectory().getChildFile(INIConfig::PRESETS_TEMPLATES_FOLDER).exists() &&
                            INIConfig::getOTTODataDirectory().getChildFile(INIConfig::CACHE_FOLDER).exists();

    return primaryDirsExist && enhancedDirsExist;
}

bool INIDataManager::allFilesExist() const {
    bool settingsExist = getINIFilePath(getGlobalSettingsFilename()).existsAsFile() &&
                        getINIFilePath(getThemesFilename()).existsAsFile() &&
                        getINIFilePath(getAudioSettingsFilename()).existsAsFile();

    // Check if Default preset exists, and if not, try to create it
    auto defaultPresetFile = INIConfig::getPresetsDirectory().getChildFile("Defaults").getChildFile("Default.ini");
    bool defaultPresetExists = defaultPresetFile.existsAsFile();
    if (!defaultPresetExists) {
        // Try to create the Default preset (cast away const for this critical operation)
        const_cast<INIDataManager*>(this)->ensureDefaultPresetExists();
        defaultPresetExists = defaultPresetFile.existsAsFile();
    }

    bool performanceExist = getINIFilePath(getPlayersFilename()).existsAsFile() &&
                           defaultPresetExists &&
                           getINIFilePath(INIConfig::PATTERN_CHAINS_FILE).existsAsFile();

    bool patternsExist = getINIFilePath(getPatternGroupsFilename()).existsAsFile();

    bool kitsExist = getINIFilePath(INIConfig::DRUM_KITS_FILE).existsAsFile() &&
                     getINIFilePath(INIConfig::MIDI_IN_LAYOUT_FILE).existsAsFile() &&
                     getINIFilePath(INIConfig::MIDI_OUT_LAYOUT_FILE).existsAsFile();

    bool mixingExist = getINIFilePath(INIConfig::CHANNEL_PRESET_GROUPS_FILE).existsAsFile() &&
                       getINIFilePath(INIConfig::CHANNEL_PRESETS_FILE).existsAsFile() &&
                       getINIFilePath(INIConfig::EQ_PRESETS_FILE).existsAsFile() &&
                       getINIFilePath(INIConfig::MASTER_CHANNEL_PRESETS_FILE).existsAsFile();

    bool systemExist = getINIFilePath(INIConfig::MIDI_DEVICES_FILE).existsAsFile() &&
                       getINIFilePath(INIConfig::FILE_INDEX_FILE).existsAsFile();

    bool enhancedSystemExist = INIConfig::getSystemDirectory().getChildFile(INIConfig::APPLICATION_FILE).existsAsFile() &&
                              INIConfig::getSystemDirectory().getChildFile(INIConfig::AUDIO_DEVICES_FILE).existsAsFile() &&
                              INIConfig::getSystemDirectory().getChildFile(INIConfig::MIDI_DEVICES_FILE).existsAsFile();

    bool enhancedSettingsExist = INIConfig::getSettingsDirectory().getChildFile(INIConfig::MIDI_SETTINGS_FILE).existsAsFile() &&
                                INIConfig::getSettingsDirectory().getChildFile(INIConfig::PERFORMANCE_SETTINGS_FILE).existsAsFile() &&
                                INIConfig::getSettingsDirectory().getChildFile(INIConfig::KEYBOARD_SHORTCUTS_FILE).existsAsFile();

    bool enhancedUIExist = INIConfig::getSettingsDirectory().getChildFile(INIConfig::SETTINGS_UI_FOLDER).getChildFile(INIConfig::THEME_MANAGER_FILE).existsAsFile() &&
                          INIConfig::getSettingsDirectory().getChildFile(INIConfig::SETTINGS_UI_FOLDER).getChildFile(INIConfig::WINDOW_LAYOUT_FILE).existsAsFile();

    bool enhancedThemesExist = INIConfig::getOTTODataDirectory().getChildFile(INIConfig::THEMES_FOLDER).getChildFile(INIConfig::DEFAULT_THEME_FILE).existsAsFile() &&
                              INIConfig::getOTTODataDirectory().getChildFile(INIConfig::THEMES_FOLDER).getChildFile(INIConfig::DARK_THEME_FILE).existsAsFile() &&
                              INIConfig::getOTTODataDirectory().getChildFile(INIConfig::THEMES_FOLDER).getChildFile(INIConfig::LIGHT_THEME_FILE).existsAsFile() &&
                              INIConfig::getOTTODataDirectory().getChildFile(INIConfig::THEMES_FOLDER).getChildFile(INIConfig::THEME_INDEX_FILE).existsAsFile();

    bool enhancedPlayersExist = INIConfig::getOTTODataDirectory().getChildFile(INIConfig::PLAYERS_FOLDER).getChildFile(INIConfig::PLAYER_CONFIGS_FILE).existsAsFile() &&
                               INIConfig::getOTTODataDirectory().getChildFile(INIConfig::PLAYERS_FOLDER).getChildFile(INIConfig::PLAYER_GROUPS_FILE).existsAsFile() &&
                               INIConfig::getOTTODataDirectory().getChildFile(INIConfig::PLAYERS_FOLDER).getChildFile(INIConfig::GLOBAL_PLAYER_SETTINGS_FILE).existsAsFile() &&
                               INIConfig::getOTTODataDirectory().getChildFile(INIConfig::PLAYERS_FOLDER).getChildFile(INIConfig::PLAYER_STATES_FILE).existsAsFile();

    bool enhancedKitsExist = INIConfig::getKitsDirectory().getChildFile(INIConfig::KIT_CATEGORIES_FILE).existsAsFile() &&
                            INIConfig::getKitsDirectory().getChildFile(INIConfig::SAMPLE_MAPPINGS_FILE).existsAsFile() &&
                            INIConfig::getKitsDirectory().getChildFile(INIConfig::VELOCITY_MAPPINGS_FILE).existsAsFile() &&
                            INIConfig::getKitsDirectory().getChildFile(INIConfig::MIDI_MAPPINGS_FILE).existsAsFile() &&
                            INIConfig::getKitsDirectory().getChildFile(INIConfig::KIT_INDEX_FILE).existsAsFile();

    bool enhancedPatternsExist = INIConfig::getPatternsDirectory().getChildFile(INIConfig::PATTERN_METADATA_FILE).existsAsFile() &&
                                INIConfig::getPatternsDirectory().getChildFile(INIConfig::PATTERN_CHAINS_FILE).existsAsFile() &&
                                INIConfig::getPatternsDirectory().getChildFile(INIConfig::PATTERN_INDEX_FILE).existsAsFile() &&
                                INIConfig::getPatternsDirectory().getChildFile(INIConfig::TEMPO_MAPS_FILE).existsAsFile();

    bool enhancedMixExist = INIConfig::getMixingDirectory().getChildFile(INIConfig::MIX_PRESETS_FOLDER).getChildFile(INIConfig::EQ_PRESETS_FILE).existsAsFile() &&
                           INIConfig::getMixingDirectory().getChildFile(INIConfig::MIX_PRESETS_FOLDER).getChildFile(INIConfig::COMPRESSOR_PRESETS_FILE).existsAsFile() &&
                           INIConfig::getMixingDirectory().getChildFile(INIConfig::MIX_PRESETS_FOLDER).getChildFile(INIConfig::REVERB_PRESETS_FILE).existsAsFile() &&
                           INIConfig::getMixingDirectory().getChildFile(INIConfig::MIX_PRESETS_FOLDER).getChildFile(INIConfig::DELAY_PRESETS_FILE).existsAsFile() &&
                           INIConfig::getMixingDirectory().getChildFile(INIConfig::MIX_CHANNELS_FOLDER).getChildFile(INIConfig::CHANNEL_PRESETS_FILE).existsAsFile() &&
                           INIConfig::getMixingDirectory().getChildFile(INIConfig::MIX_MASTER_FOLDER).getChildFile(INIConfig::MASTER_CHANNEL_PRESETS_FILE).existsAsFile() &&
                           INIConfig::getMixingDirectory().getChildFile(INIConfig::MIXER_SNAPSHOTS_FILE).existsAsFile();

    bool enhancedPerformanceExist = INIConfig::getPerformanceDirectory().getChildFile(INIConfig::SESSIONS_FILE).existsAsFile() &&
                                   INIConfig::getPerformanceDirectory().getChildFile(INIConfig::SET_LISTS_FILE).existsAsFile() &&
                                   INIConfig::getPerformanceDirectory().getChildFile(INIConfig::MACRO_CONTROLS_FILE).existsAsFile() &&
                                   INIConfig::getPerformanceDirectory().getChildFile(INIConfig::SCENE_CHANGES_FILE).existsAsFile() &&
                                   INIConfig::getPerformanceDirectory().getChildFile(INIConfig::BACKUP_STATES_FILE).existsAsFile();

    bool enhancedCacheExist = INIConfig::getOTTODataDirectory().getChildFile(INIConfig::CACHE_FOLDER).getChildFile(INIConfig::SAMPLE_CACHE_FILE).existsAsFile() &&
                             INIConfig::getOTTODataDirectory().getChildFile(INIConfig::CACHE_FOLDER).getChildFile(INIConfig::PATTERN_CACHE_FILE).existsAsFile() &&
                             INIConfig::getOTTODataDirectory().getChildFile(INIConfig::CACHE_FOLDER).getChildFile(INIConfig::KIT_CACHE_FILE).existsAsFile() &&
                             INIConfig::getOTTODataDirectory().getChildFile(INIConfig::CACHE_FOLDER).getChildFile(INIConfig::SEARCH_INDEX_FILE).existsAsFile();

    bool enhancedPresetsExist = INIConfig::getPresetsDirectory().getChildFile(INIConfig::PRESET_INDEX_FILE).existsAsFile() &&
                               INIConfig::getPresetsDirectory().getChildFile(INIConfig::PRESETS_TEMPLATES_FOLDER).getChildFile(INIConfig::PROJECT_TEMPLATES_FILE).existsAsFile() &&
                               INIConfig::getPresetsDirectory().getChildFile(INIConfig::PRESETS_TEMPLATES_FOLDER).getChildFile(INIConfig::KIT_TEMPLATES_FILE).existsAsFile() &&
                               INIConfig::getPresetsDirectory().getChildFile(INIConfig::PRESETS_TEMPLATES_FOLDER).getChildFile(INIConfig::PATTERN_TEMPLATES_FILE).existsAsFile();

    return settingsExist && performanceExist && patternsExist &&
           kitsExist && mixingExist && systemExist &&
           enhancedSystemExist && enhancedSettingsExist && enhancedUIExist &&
           enhancedThemesExist && enhancedPlayersExist && enhancedKitsExist &&
           enhancedPatternsExist && enhancedMixExist && enhancedPerformanceExist &&
           enhancedCacheExist && enhancedPresetsExist;
}

bool INIDataManager::createAllRequiredFiles() {
    bool success = true;

    if (!directoryStructureExists()) {
        if (!createDirectoryStructure()) {
            setError("Failed to create directory structure before creating files");
            return false;
        }
    }

    auto globalFile = getINIFilePath(getGlobalSettingsFilename());
    if (!globalFile.existsAsFile()) {
        success = success && createSampleGlobalSettings();
    }

    auto themesFile = getINIFilePath(getThemesFilename());
    if (!themesFile.existsAsFile()) {
        success = success && createSampleThemeSettings();
    }

    auto audioFile = getINIFilePath(getAudioSettingsFilename());
    if (!audioFile.existsAsFile()) {
        success = success && createSampleAudioSettings();
    }

    auto playersFile = getINIFilePath(getPlayersFilename());
    if (!playersFile.existsAsFile()) {
        success = success && createSamplePlayerSettings();
    }

    auto defaultPresetFile = INIConfig::getPresetsDirectory().getChildFile("Defaults").getChildFile("Default.ini");
    if (!defaultPresetFile.existsAsFile()) {
        success = success && createDefaultPreset();
    }

    auto patternChainsFile = getINIFilePath(INIConfig::PATTERN_CHAINS_FILE);
    if (!patternChainsFile.existsAsFile()) {
        success = success && createSamplePatternChains();
    }

    auto patternsFile = getINIFilePath(getPatternGroupsFilename());
    if (!patternsFile.existsAsFile()) {
        success = success && createSamplePatternGroups();
    }

    auto drumKitsFile = getINIFilePath(INIConfig::DRUM_KITS_FILE);
    if (!drumKitsFile.existsAsFile()) {
        success = success && createSampleDrumKits();
    }

    auto midiLayoutFiles = createSampleMidiLayouts();
    success = success && midiLayoutFiles;

    auto channelPresetGroupsFile = getINIFilePath(INIConfig::CHANNEL_PRESET_GROUPS_FILE);
    if (!channelPresetGroupsFile.existsAsFile()) {
        success = success && createSampleChannelPresetGroups();
    }

    auto channelPresetsFile = getINIFilePath(INIConfig::CHANNEL_PRESETS_FILE);
    if (!channelPresetsFile.existsAsFile()) {
        success = success && createSampleChannelPresets();
    }

    auto eqPresetsFile = getINIFilePath(INIConfig::EQ_PRESETS_FILE);
    if (!eqPresetsFile.existsAsFile()) {
        success = success && createSampleEQPresets();
    }

    auto masterPresetsFile = getINIFilePath(INIConfig::MASTER_CHANNEL_PRESETS_FILE);
    if (!masterPresetsFile.existsAsFile()) {
        success = success && createSampleMasterChannelPresets();
    }

    auto midiDevicesFile = getINIFilePath(INIConfig::MIDI_DEVICES_FILE);
    if (!midiDevicesFile.existsAsFile()) {
        success = success && createSampleMidiDevices();
    }

    auto fileIndexFile = getINIFilePath(INIConfig::FILE_INDEX_FILE);
    if (!fileIndexFile.existsAsFile()) {
        success = success && createSampleFileIndex();
    }

    // ========================================================================
    // CREATE ENHANCED DATABASE FILES
    // ========================================================================

    // Create enhanced system files
    success = success && createEnhancedSystemFiles();

    // Create enhanced settings files
    success = success && createEnhancedSettingsFiles();

    // Create enhanced UI files
    success = success && createEnhancedUIFiles();

    // Create enhanced themes files
    success = success && createEnhancedThemesFiles();

    // Create enhanced players files
    success = success && createEnhancedPlayersFiles();

    // Create enhanced kits files
    success = success && createEnhancedKitsFiles();

    // Create enhanced patterns files
    success = success && createEnhancedPatternsFiles();

    // Create enhanced mix files
    success = success && createEnhancedMixFiles();

    // Create enhanced performance files
    success = success && createEnhancedPerformanceFiles();

    // Create enhanced cache files
    success = success && createEnhancedCacheFiles();

    // Create enhanced presets files
    success = success && createEnhancedPresetsFiles();

    return success;
}

juce::File INIDataManager::getINIFilePath(const juce::String& filename) const {
    if (filename == INIConfig::GLOBAL_SETTINGS_FILE ||
        filename == INIConfig::THEMES_FILE ||
        filename == INIConfig::AUDIO_SETTINGS_FILE) {
        return INIConfig::getSettingsDirectory().getChildFile(filename);
    }
    else if (filename == INIConfig::PLAYERS_FILE ||
             filename == INIConfig::PRESETS_FILE ||
             filename == INIConfig::PATTERN_CHAINS_FILE) {
        return INIConfig::getPerformanceDirectory().getChildFile(filename);
    }
    else if (filename == INIConfig::PATTERN_GROUPS_FILE) {
        return INIConfig::getPatternsDirectory().getChildFile(filename);
    }
    else if (filename == INIConfig::DRUM_KITS_FILE ||
             filename == INIConfig::MIDI_IN_LAYOUT_FILE ||
             filename == INIConfig::MIDI_OUT_LAYOUT_FILE) {
        return INIConfig::getKitsDirectory().getChildFile(filename);
    }
    else if (filename == INIConfig::CHANNEL_PRESET_GROUPS_FILE ||
             filename == INIConfig::CHANNEL_PRESETS_FILE ||
             filename == INIConfig::EQ_PRESETS_FILE ||
             filename == INIConfig::MASTER_CHANNEL_PRESETS_FILE) {
        return INIConfig::getMixingDirectory().getChildFile(filename);
    }
    else if (filename == INIConfig::MIDI_DEVICES_FILE ||
             filename == INIConfig::FILE_INDEX_FILE) {
        return INIConfig::getSystemDirectory().getChildFile(filename);
    }

    return INIConfig::getOTTODataDirectory().getChildFile(filename);
}

bool INIDataManager::createSampleGlobalSettings() {
    GlobalSettings settings;
    settings.setDefaults();

    settings.lastModified = INIUtils::formatTimestamp();
    settings.fileFormatVersion = INIConfig::FILE_FORMAT_VERSION;

    bool result = saveGlobalSettings(settings);
    if (!result) {
        setError("Failed to create sample global settings file");
    }
    return result;
}

bool INIDataManager::createSampleThemeSettings() {
    juce::Array<ThemeSettings> themes;

    ThemeSettings darkTheme;
    darkTheme.setDefaults();
    darkTheme.createdDate = INIUtils::formatTimestamp();
    darkTheme.modifiedDate = darkTheme.createdDate;
    themes.add(darkTheme);

    ThemeSettings lightTheme;
    lightTheme.setLightThemeDefaults();
    lightTheme.createdDate = INIUtils::formatTimestamp();
    lightTheme.modifiedDate = lightTheme.createdDate;
    themes.add(lightTheme);

    ThemeSettings classicTheme;
    classicTheme.setClassicThemeDefaults();
    classicTheme.createdDate = INIUtils::formatTimestamp();
    classicTheme.modifiedDate = classicTheme.createdDate;
    themes.add(classicTheme);

    bool result = saveAllThemes(themes);
    if (!result) {
        setError("Failed to create sample theme settings file");
    }
    return result;
}

bool INIDataManager::createSampleAudioSettings() {
    AudioSettings settings;
    settings.setDefaults();

    bool result = saveAudioSettings(settings);
    if (!result) {
        setError("Failed to create sample audio settings file");
    }
    return result;
}

bool INIDataManager::createSamplePlayerSettings() {
    juce::Array<PlayerSettings> players;

    for (int i = 0; i < 8; ++i) {
        PlayerSettings player;
        player.setDefaults();
        player.selectedMidiGroup = "Group " + juce::String(i + 1);
        player.midiChannel = i + 1;
        player.outputChannel = i + 1;
        players.add(player);
    }

    bool result = savePlayerSettings(players);
    if (!result) {
        setError("Failed to create sample player settings file");
    }
    return result;
}

bool INIDataManager::createSamplePatternGroups() {
    juce::Array<BeatsButtonGroup> groups;

    for (int i = 0; i < 8; ++i) {
        BeatsButtonGroup group("Group " + juce::String(i + 1));
        group.isCustomGroup = false;
        group.selectedButton = 0;
        group.isFavorite = false;

        for (int j = 0; j < 16; ++j) {
            group.midiFiles.add("");
        }

        groups.add(group);
    }

    bool result = savePatternGroups(groups);
    if (!result) {
        setError("Failed to create sample pattern groups file");
    }
    return result;
}

bool INIDataManager::createSamplePresets() {
    auto file = getINIFilePath(getPresetsFilename());

    auto dir = file.getParentDirectory();
    if (!dir.exists()) {
        if (!dir.createDirectory().wasOk()) {
            setError("Failed to create directory for presets file");
            return false;
        }
    }

    std::map<juce::String, juce::StringArray> sectionData;
    juce::StringArray sections = { "general" };

    juce::StringArray generalLines;
    generalLines.add("preset_count=3");
    generalLines.add("last_modified=" + INIUtils::formatTimestamp());
    generalLines.add("file_format_version=" + INIConfig::FILE_FORMAT_VERSION);
    sectionData["general"] = generalLines;

    sections.add("preset_0");
    juce::StringArray preset0Lines;
    preset0Lines.add("name=Default");
    preset0Lines.add("tempo=" + juce::String(INIConfig::Defaults::DEFAULT_TEMPO));
    preset0Lines.add("created=" + INIUtils::formatTimestamp());
    preset0Lines.add("modified=" + INIUtils::formatTimestamp());
    sectionData["preset_0"] = preset0Lines;

    sections.add("preset_1");
    juce::StringArray preset1Lines;
    preset1Lines.add("name=Rock");
    preset1Lines.add("tempo=130");
    preset1Lines.add("created=" + INIUtils::formatTimestamp());
    preset1Lines.add("modified=" + INIUtils::formatTimestamp());
    sectionData["preset_1"] = preset1Lines;

    sections.add("preset_2");
    juce::StringArray preset2Lines;
    preset2Lines.add("name=Jazz");
    preset2Lines.add("tempo=140");
    preset2Lines.add("created=" + INIUtils::formatTimestamp());
    preset2Lines.add("modified=" + INIUtils::formatTimestamp());
    sectionData["preset_2"] = preset2Lines;

    bool result = INIUtils::writeINIFile(file, sections, sectionData);
    if (!result) {
        setError("Failed to create presets file");
    }
    return result;
}

bool INIDataManager::saveGlobalSettings(const GlobalSettings& settings) {
    if (!settings.isValid()) {
        setError("Invalid Global Settings data");
        return false;
    }

    auto file = getINIFilePath(getGlobalSettingsFilename());

    auto dir = file.getParentDirectory();
    if (!dir.exists()) {
        if (!dir.createDirectory().wasOk()) {
            setError("Failed to create directory for global settings file");
            return false;
        }
    }

    std::map<juce::String, juce::StringArray> sectionData;
    juce::StringArray sections = { "settings", "interface", "metronome", "sync", "theme", "font", "midi" };

    juce::StringArray settingsLines;
    settingsLines.add("settings_id=" + juce::String(settings.settingsID));
    settingsLines.add("settings_name=" + INIUtils::escapeINIValue(settings.settingsName));
    settingsLines.add("tempo=" + juce::String(settings.tempo));
    settingsLines.add("preset_id=" + juce::String(settings.presetID));
    settingsLines.add("quantize_value=" + juce::String(settings.quantizeValue));
    settingsLines.add("count_in_bars=" + juce::String(settings.countInBars));
    settingsLines.add("audio_settings_id=" + juce::String(settings.audioSettingsID));
    settingsLines.add("auto_save_interval=" + juce::String(settings.autoSaveInterval));
    settingsLines.add("last_modified=" + INIUtils::escapeINIValue(settings.lastModified));
    settingsLines.add("file_format_version=" + INIUtils::escapeINIValue(settings.fileFormatVersion));
    sectionData["settings"] = settingsLines;

    juce::StringArray interfaceLines;
    interfaceLines.add("interface_scale=" + INIUtils::floatToString(settings.interfaceScale, 2));
    interfaceLines.add("interface_x=" + juce::String(settings.interfaceStartingX));
    interfaceLines.add("interface_y=" + juce::String(settings.interfaceStartingY));
    interfaceLines.add("interface_width=" + juce::String(settings.interfaceWidth));
    interfaceLines.add("interface_height=" + juce::String(settings.interfaceHeight));
    interfaceLines.add("splash_screen_time=" + INIUtils::floatToString(settings.splashScreenOnTime, 1));
    sectionData["interface"] = interfaceLines;

    juce::StringArray metronomeLines;
    metronomeLines.add("enabled=" + INIUtils::boolToString(settings.metronomeEnabled));
    metronomeLines.add("volume=" + INIUtils::floatToString(settings.metronomeVolume, 2));
    metronomeLines.add("sound=" + INIUtils::escapeINIValue(settings.metronomeSound));
    sectionData["metronome"] = metronomeLines;

    juce::StringArray syncLines;
    syncLines.add("link_sync_mode=" + INIUtils::escapeINIValue(settings.linkSyncMode));
    syncLines.add("link_sync_value=" + juce::String(settings.linkSyncValue));
    syncLines.add("cloud_connection=" + INIUtils::escapeINIValue(settings.currentCloudConnection));
    syncLines.add("midi_clock_out=" + INIUtils::boolToString(settings.midiClockOut));
    syncLines.add("midi_clock_in=" + INIUtils::boolToString(settings.midiClockIn));
    sectionData["sync"] = syncLines;

    juce::StringArray themeLines;
    themeLines.add("theme_id=" + juce::String(settings.themeID));
    themeLines.add("theme_name=" + INIUtils::escapeINIValue(settings.currentThemeName));
    sectionData["theme"] = themeLines;

    juce::StringArray fontLines;
    fontLines.add("font_main_type=" + INIUtils::escapeINIValue(settings.fontMainType));
    fontLines.add("font_main_size=" + INIUtils::floatToString(settings.fontMainSize, 1));
    fontLines.add("font_secondary_type=" + INIUtils::escapeINIValue(settings.fontSecondaryType));
    fontLines.add("font_secondary_size=" + INIUtils::floatToString(settings.fontSecondarySize, 1));
    fontLines.add("phosphor_weight=" + juce::String(settings.phosphorWeight));
    fontLines.add("enable_advanced_typography=" + INIUtils::boolToString(settings.enableAdvancedTypography));
    fontLines.add("custom_font_path=" + INIUtils::escapeINIValue(settings.customFontPath));
    sectionData["font"] = fontLines;

    juce::StringArray midiLines;
    midiLines.add("midi_learn_enabled=" + INIUtils::boolToString(settings.midiLearnEnabled));
    midiLines.add("current_controller_preset=" + INIUtils::escapeINIValue(settings.currentControllerPreset));
    midiLines.add("program_change_enabled=" + INIUtils::boolToString(settings.programChangeEnabled));
    midiLines.add("led_feedback_enabled=" + INIUtils::boolToString(settings.ledFeedbackEnabled));
    sectionData["midi"] = midiLines;

    return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::loadGlobalSettings(GlobalSettings& settings) {
    auto file = getINIFilePath(getGlobalSettingsFilename());
    if (!file.existsAsFile()) {
        setError("Global Settings file not found");
        return false;
    }

    std::map<juce::String, std::map<juce::String, juce::String>> data;
    if (!INIUtils::readINIFile(file, data)) {
        setError("Failed to read Global Settings file");
        return false;
    }

    try {
        settings.setDefaults();

        if (data.count("settings")) {
            auto& section = data["settings"];

            if (section.count("settings_id"))
                settings.settingsID = section["settings_id"].getIntValue();
            if (section.count("settings_name"))
                settings.settingsName = section["settings_name"];
            if (section.count("tempo"))
                settings.tempo = INIConfig::clampTempo(section["tempo"].getIntValue());
            if (section.count("preset_id"))
                settings.presetID = INIConfig::clampPresetIndex(section["preset_id"].getIntValue());
            if (section.count("quantize_value"))
                settings.quantizeValue = section["quantize_value"].getIntValue();
            if (section.count("count_in_bars"))
                settings.countInBars = section["count_in_bars"].getIntValue();
            if (section.count("audio_settings_id"))
                settings.audioSettingsID = section["audio_settings_id"].getIntValue();
            if (section.count("auto_save_interval"))
                settings.autoSaveInterval = section["auto_save_interval"].getIntValue();
            if (section.count("last_modified"))
                settings.lastModified = section["last_modified"];
            if (section.count("file_format_version"))
                settings.fileFormatVersion = section["file_format_version"];
        }

        if (data.count("interface")) {
            auto& section = data["interface"];

            if (section.count("interface_scale"))
                settings.interfaceScale = INIConfig::clampScale(section["interface_scale"].getFloatValue());
            if (section.count("interface_x"))
                settings.interfaceStartingX = section["interface_x"].getIntValue();
            if (section.count("interface_y"))
                settings.interfaceStartingY = section["interface_y"].getIntValue();
            if (section.count("interface_width"))
                settings.interfaceWidth = INIConfig::clampWindowSize(section["interface_width"].getIntValue());
            if (section.count("interface_height"))
                settings.interfaceHeight = INIConfig::clampWindowSize(section["interface_height"].getIntValue());

            if (section.count("splash_screen_time"))
                settings.splashScreenOnTime = juce::jlimit(0.0f, 10.0f, section["splash_screen_time"].getFloatValue());
            else
                settings.splashScreenOnTime = INIConfig::UI::SPLASH_SCREEN_ON_TIME;

        }

        if (data.count("metronome")) {
            auto& section = data["metronome"];

            if (section.count("enabled"))
                settings.metronomeEnabled = INIUtils::stringToBool(section["enabled"]);
            if (section.count("volume"))
                settings.metronomeVolume = INIConfig::clampVolume(section["volume"].getFloatValue());
            if (section.count("sound"))
                settings.metronomeSound = section["sound"];
        }

        if (data.count("sync")) {
            auto& section = data["sync"];

            if (section.count("link_sync_mode"))
                settings.linkSyncMode = section["link_sync_mode"];
            if (section.count("link_sync_value"))
                settings.linkSyncValue = section["link_sync_value"].getIntValue();
            if (section.count("cloud_connection"))
                settings.currentCloudConnection = section["cloud_connection"];
            if (section.count("midi_clock_out"))
                settings.midiClockOut = INIUtils::stringToBool(section["midi_clock_out"]);
            if (section.count("midi_clock_in"))
                settings.midiClockIn = INIUtils::stringToBool(section["midi_clock_in"]);
        }

        if (data.count("theme")) {
            auto& section = data["theme"];

            if (section.count("theme_id"))
                settings.themeID = section["theme_id"].getIntValue();
            if (section.count("theme_name"))
                settings.currentThemeName = section["theme_name"];
        }

        if (data.count("font")) {
            auto& section = data["font"];

            if (section.count("font_main_type"))
                settings.fontMainType = section["font_main_type"];
            if (section.count("font_main_size"))
                settings.fontMainSize = section["font_main_size"].getFloatValue();
            if (section.count("font_secondary_type"))
                settings.fontSecondaryType = section["font_secondary_type"];
            if (section.count("font_secondary_size"))
                settings.fontSecondarySize = section["font_secondary_size"].getFloatValue();
            if (section.count("phosphor_weight"))
                settings.phosphorWeight = INIConfig::clampPhosphorWeight(section["phosphor_weight"].getIntValue());
            if (section.count("enable_advanced_typography"))
                settings.enableAdvancedTypography = INIUtils::stringToBool(section["enable_advanced_typography"]);
            if (section.count("custom_font_path"))
                settings.customFontPath = section["custom_font_path"];
        }

        if (data.count("midi")) {
            auto& section = data["midi"];

            if (section.count("midi_learn_enabled"))
                settings.midiLearnEnabled = INIUtils::stringToBool(section["midi_learn_enabled"]);
            if (section.count("current_controller_preset"))
                settings.currentControllerPreset = section["current_controller_preset"];
            if (section.count("program_change_enabled"))
                settings.programChangeEnabled = INIUtils::stringToBool(section["program_change_enabled"]);
            if (section.count("led_feedback_enabled"))
                settings.ledFeedbackEnabled = INIUtils::stringToBool(section["led_feedback_enabled"]);
        }

        return settings.isValid();
    }
    catch (const std::exception& e) {
        setError("Exception loading global settings: " + juce::String(e.what()));
        return false;
    }
}

void INIDataManager::initializeDefaults(ComponentState& state) {
   state.globalSettings.setDefaults();
   state.themeSettings.setDefaults();
   state.audioSettings.setDefaults();

   for (int i = 0; i < 8; ++i) {
       state.playerSettings[i].setDefaults();
   }

   if (state.beatsButtonGroups.isEmpty()) {
       for (int i = 1; i <= 8; ++i) {
           BeatsButtonGroup group("Group " + juce::String(i));
           state.beatsButtonGroups.add(group);
       }
   }
}

bool INIDataManager::saveAllData(const ComponentState& state) {
   return saveAllSettings(state);
}

bool INIDataManager::loadAllData(ComponentState& state) {
   initializeDefaults(state);
   return loadAllSettings(state);
}

bool INIDataManager::saveAllSettings(const ComponentState& state) {
   bool success = true;

   success = success && saveGlobalSettings(state.globalSettings);

   juce::Array<ThemeSettings> themes;
   if (loadAllThemes(themes)) {
       bool found = false;
       for (auto& theme : themes) {
           if (theme.themeID == state.themeSettings.themeID) {
               theme = state.themeSettings;
               found = true;
               break;
           }
       }
       if (!found) {
           themes.add(state.themeSettings);
       }
   } else {
       themes.add(state.themeSettings);
   }
   success = success && saveAllThemes(themes);

   success = success && saveAudioSettings(state.audioSettings);

   juce::Array<PlayerSettings> players;
   for (int i = 0; i < 8; ++i) {
       players.add(state.playerSettings[i]);
   }
   success = success && savePlayerSettings(players);

   success = success && savePatternGroups(state.beatsButtonGroups);

   return success;
}

bool INIDataManager::loadAllSettings(ComponentState& state) {
   bool success = true;

   initializeDefaults(state);

   success = success && loadGlobalSettings(state.globalSettings);

   juce::Array<ThemeSettings> themes;
   if (loadAllThemes(themes)) {
       for (const auto& theme : themes) {
           if (theme.themeID == state.globalSettings.themeID) {
               state.themeSettings = theme;
               break;
           }
       }
   }

   success = success && loadAudioSettings(state.audioSettings);

   juce::Array<PlayerSettings> players;
   if (loadPlayerSettings(players)) {
       for (int i = 0; i < 8 && i < players.size(); ++i) {
           state.playerSettings[i] = players[i];
       }
   }

   loadPatternGroups(state.beatsButtonGroups);

   return success;
}

bool INIDataManager::saveAllThemes(const juce::Array<ThemeSettings>& themes) {
   if (themes.isEmpty()) {
       setError("No themes to save");
       return false;
   }

   auto file = getINIFilePath(getThemesFilename());

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections;

   sections.add("general");
   juce::StringArray generalLines;
   generalLines.add("theme_count=" + juce::String(themes.size()));
   generalLines.add("last_modified=" + INIUtils::formatTimestamp());
   sectionData["general"] = generalLines;

   for (int i = 0; i < themes.size(); ++i) {
       const auto& theme = themes[i];

       if (!theme.isValid()) {
           setError("Invalid theme data at index " + juce::String(i));
           return false;
       }

       juce::String sectionName = "theme_" + juce::String(theme.themeID);
       sections.add(sectionName);

       juce::StringArray themeLines;
       themeLines.add("theme_id=" + juce::String(theme.themeID));
       themeLines.add("theme_name=" + INIUtils::escapeINIValue(theme.themeName));
       themeLines.add("background_color=" + theme.backgroundColor);
       themeLines.add("foreground_color=" + theme.foregroundColor);
       themeLines.add("accent_color=" + theme.accentColor);
       themeLines.add("text_color=" + theme.textColor);
       themeLines.add("border_color=" + theme.borderColor);
       themeLines.add("font_main=" + INIUtils::escapeINIValue(theme.fontMain));
       themeLines.add("font_main_size=" + INIUtils::floatToString(theme.fontMainSize, 1));
       themeLines.add("font_main_weight=" + INIUtils::escapeINIValue(theme.fontMainWeight));
       themeLines.add("font_secondary=" + INIUtils::escapeINIValue(theme.fontSecondary));
       themeLines.add("font_secondary_size=" + INIUtils::floatToString(theme.fontSecondarySize, 1));
       themeLines.add("font_secondary_weight=" + INIUtils::escapeINIValue(theme.fontSecondaryWeight));
       themeLines.add("button_color=" + theme.buttonColor);
       themeLines.add("button_hover_color=" + theme.buttonHoverColor);
       themeLines.add("button_active_color=" + theme.buttonActiveColor);
       themeLines.add("slider_track_color=" + theme.sliderTrackColor);
       themeLines.add("slider_thumb_color=" + theme.sliderThumbColor);
       themeLines.add("meter_color_low=" + theme.meterColorLow);
       themeLines.add("meter_color_mid=" + theme.meterColorMid);
       themeLines.add("meter_color_high=" + theme.meterColorHigh);
       themeLines.add("grid_line_color=" + theme.gridLineColor);
       themeLines.add("pattern_active_color=" + theme.patternActiveColor);
       themeLines.add("pattern_inactive_color=" + theme.patternInactiveColor);
       themeLines.add("created_date=" + INIUtils::escapeINIValue(theme.createdDate));
       themeLines.add("modified_date=" + INIUtils::escapeINIValue(theme.modifiedDate));

       sectionData[sectionName] = themeLines;
   }

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::loadAllThemes(juce::Array<ThemeSettings>& themes) {
   auto file = getINIFilePath(getThemesFilename());
   if (!file.existsAsFile()) {
       setError("Theme Settings file not found");
       return false;
   }

   std::map<juce::String, std::map<juce::String, juce::String>> data;
   if (!INIUtils::readINIFile(file, data)) {
       setError("Failed to read Theme Settings file");
       return false;
   }

   themes.clear();

   try {
       int themeCount = 0;
       if (data.count("general") && data["general"].count("theme_count")) {
           themeCount = data["general"]["theme_count"].getIntValue();
       }

       for (auto& sectionPair : data) {
           if (sectionPair.first.startsWith("theme_")) {
               ThemeSettings theme;
               theme.setDefaults();

               auto& section = sectionPair.second;

               if (section.count("theme_id"))
                   theme.themeID = section["theme_id"].getIntValue();
               if (section.count("theme_name"))
                   theme.themeName = section["theme_name"];
               if (section.count("background_color") && INIConfig::isValidHexColor(section["background_color"]))
                   theme.backgroundColor = section["background_color"];
               if (section.count("foreground_color") && INIConfig::isValidHexColor(section["foreground_color"]))
                   theme.foregroundColor = section["foreground_color"];
               if (section.count("accent_color") && INIConfig::isValidHexColor(section["accent_color"]))
                   theme.accentColor = section["accent_color"];
               if (section.count("text_color") && INIConfig::isValidHexColor(section["text_color"]))
                   theme.textColor = section["text_color"];
               if (section.count("border_color") && INIConfig::isValidHexColor(section["border_color"]))
                   theme.borderColor = section["border_color"];

               if (section.count("font_main"))
                   theme.fontMain = section["font_main"];
               if (section.count("font_main_size"))
                   theme.fontMainSize = section["font_main_size"].getFloatValue();
               if (section.count("font_main_weight"))
                   theme.fontMainWeight = section["font_main_weight"];
               if (section.count("font_secondary"))
                   theme.fontSecondary = section["font_secondary"];
               if (section.count("font_secondary_size"))
                   theme.fontSecondarySize = section["font_secondary_size"].getFloatValue();
               if (section.count("font_secondary_weight"))
                   theme.fontSecondaryWeight = section["font_secondary_weight"];

               if (section.count("button_color") && INIConfig::isValidHexColor(section["button_color"]))
                   theme.buttonColor = section["button_color"];
               if (section.count("button_hover_color") && INIConfig::isValidHexColor(section["button_hover_color"]))
                   theme.buttonHoverColor = section["button_hover_color"];
               if (section.count("button_active_color") && INIConfig::isValidHexColor(section["button_active_color"]))
                   theme.buttonActiveColor = section["button_active_color"];
               if (section.count("slider_track_color") && INIConfig::isValidHexColor(section["slider_track_color"]))
                   theme.sliderTrackColor = section["slider_track_color"];
               if (section.count("slider_thumb_color") && INIConfig::isValidHexColor(section["slider_thumb_color"]))
                   theme.sliderThumbColor = section["slider_thumb_color"];
               if (section.count("meter_color_low") && INIConfig::isValidHexColor(section["meter_color_low"]))
                   theme.meterColorLow = section["meter_color_low"];
               if (section.count("meter_color_mid") && INIConfig::isValidHexColor(section["meter_color_mid"]))
                   theme.meterColorMid = section["meter_color_mid"];
               if (section.count("meter_color_high") && INIConfig::isValidHexColor(section["meter_color_high"]))
                   theme.meterColorHigh = section["meter_color_high"];
               if (section.count("grid_line_color") && INIConfig::isValidHexColor(section["grid_line_color"]))
                   theme.gridLineColor = section["grid_line_color"];
               if (section.count("pattern_active_color") && INIConfig::isValidHexColor(section["pattern_active_color"]))
                   theme.patternActiveColor = section["pattern_active_color"];
               if (section.count("pattern_inactive_color") && INIConfig::isValidHexColor(section["pattern_inactive_color"]))
                   theme.patternInactiveColor = section["pattern_inactive_color"];

               if (section.count("created_date"))
                   theme.createdDate = section["created_date"];
               if (section.count("modified_date"))
                   theme.modifiedDate = section["modified_date"];

               if (theme.isValid()) {
                   themes.add(theme);
               }
           }
       }

       return !themes.isEmpty();
   }
   catch (const std::exception& e) {
       setError("Exception loading themes: " + juce::String(e.what()));
       return false;
   }
}

bool INIDataManager::saveAudioSettings(const AudioSettings& settings) {
   if (!settings.isValid()) {
       setError("Invalid Audio Settings data");
       return false;
   }

   auto file = getINIFilePath(getAudioSettingsFilename());

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections = { "audio", "device", "advanced", "midi" };

   juce::StringArray audioLines;
   audioLines.add("settings_id=" + juce::String(settings.audioSettingsID));
   audioLines.add("settings_name=" + INIUtils::escapeINIValue(settings.settingsName));
   audioLines.add("sample_rate=" + juce::String(settings.sampleRate));
   audioLines.add("buffer_size=" + juce::String(settings.bufferSize));
   audioLines.add("bit_depth=" + juce::String(settings.bitDepth));
   sectionData["audio"] = audioLines;

   juce::StringArray deviceLines;
   deviceLines.add("device_name=" + INIUtils::escapeINIValue(settings.audioDeviceName));
   deviceLines.add("input_channels=" + juce::String(settings.inputChannels));
   deviceLines.add("output_channels=" + juce::String(settings.outputChannels));
   sectionData["device"] = deviceLines;

   juce::StringArray advancedLines;
   advancedLines.add("latency_compensation=" + INIUtils::boolToString(settings.latencyCompensation));
   advancedLines.add("enable_asio=" + INIUtils::boolToString(settings.enableASIO));
   advancedLines.add("enable_multicore=" + INIUtils::boolToString(settings.enableMultiCore));
   sectionData["advanced"] = advancedLines;

   juce::StringArray midiLines;
   midiLines.add("midi_input_device=" + INIUtils::escapeINIValue(settings.midiInputDevice));
   midiLines.add("midi_output_device=" + INIUtils::escapeINIValue(settings.midiOutputDevice));
   midiLines.add("midi_thru=" + INIUtils::boolToString(settings.midiThru));
   sectionData["midi"] = midiLines;

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::loadAudioSettings(AudioSettings& settings) {
   auto file = getINIFilePath(getAudioSettingsFilename());
   if (!file.existsAsFile()) {
       setError("Audio Settings file not found");
       return false;
   }

   std::map<juce::String, std::map<juce::String, juce::String>> data;
   if (!INIUtils::readINIFile(file, data)) {
       setError("Failed to read Audio Settings file");
       return false;
   }

   try {
       settings.setDefaults();

       if (data.count("audio")) {
           auto& section = data["audio"];

           if (section.count("settings_id"))
               settings.audioSettingsID = section["settings_id"].getIntValue();
           if (section.count("settings_name"))
               settings.settingsName = section["settings_name"];
           if (section.count("sample_rate"))
               settings.sampleRate = section["sample_rate"].getIntValue();
           if (section.count("buffer_size"))
               settings.bufferSize = section["buffer_size"].getIntValue();
           if (section.count("bit_depth"))
               settings.bitDepth = section["bit_depth"].getIntValue();
       }

       if (data.count("device")) {
           auto& section = data["device"];

           if (section.count("device_name"))
               settings.audioDeviceName = section["device_name"];
           if (section.count("input_channels"))
               settings.inputChannels = section["input_channels"].getIntValue();
           if (section.count("output_channels"))
               settings.outputChannels = section["output_channels"].getIntValue();
       }

       if (data.count("advanced")) {
           auto& section = data["advanced"];

           if (section.count("latency_compensation"))
               settings.latencyCompensation = INIUtils::stringToBool(section["latency_compensation"]);
           if (section.count("enable_asio"))
               settings.enableASIO = INIUtils::stringToBool(section["enable_asio"]);
           if (section.count("enable_multicore"))
               settings.enableMultiCore = INIUtils::stringToBool(section["enable_multicore"]);
       }

       if (data.count("midi")) {
           auto& section = data["midi"];

           if (section.count("midi_input_device"))
               settings.midiInputDevice = section["midi_input_device"];
           if (section.count("midi_output_device"))
               settings.midiOutputDevice = section["midi_output_device"];
           if (section.count("midi_thru"))
               settings.midiThru = INIUtils::stringToBool(section["midi_thru"]);
       }

       return settings.isValid();
   }
   catch (const std::exception& e) {
       setError("Exception loading audio settings: " + juce::String(e.what()));
       return false;
   }
}

bool INIDataManager::savePlayerSettings(const juce::Array<PlayerSettings>& playerSettings) {
    auto file = getINIFilePath(getPlayersFilename());

    std::map<juce::String, juce::StringArray> sectionData;
    juce::StringArray sections;

    sections.add("general");
    juce::StringArray generalLines;
    generalLines.add("player_count=8");
    generalLines.add("last_modified=" + INIUtils::formatTimestamp());
    sectionData["general"] = generalLines;

    for (int i = 0; i < 8 && i < playerSettings.size(); ++i) {
        const auto& player = playerSettings[i];

        juce::String sectionName = "player_" + juce::String(i + 1);
        sections.add(sectionName);

        juce::StringArray playerLines;

        playerLines.add("enabled=" + INIUtils::boolToString(player.enabled));
        playerLines.add("drumkit=" + INIUtils::escapeINIValue(player.selectedDrumkit));
        playerLines.add("volume=" + INIUtils::floatToString(player.volume, 2));
        playerLines.add("pan=" + INIUtils::floatToString(player.pan, 2));
        playerLines.add("muted=" + INIUtils::boolToString(player.muted));
        playerLines.add("soloed=" + INIUtils::boolToString(player.soloed));
        playerLines.add("midi_channel=" + juce::String(player.midiChannel));
        playerLines.add("output_channel=" + juce::String(player.outputChannel));

        playerLines.add("midi_group=" + INIUtils::escapeINIValue(player.selectedMidiGroup));
        playerLines.add("selected_button=" + juce::String(player.selectedButton));

        playerLines.add("swing=" + INIUtils::floatToString(player.swingValue, 1));
        playerLines.add("energy=" + INIUtils::floatToString(player.energyValue, 1));
        playerLines.add("volume_value=" + INIUtils::floatToString(player.volumeValue, 2));

        for (int j = 0; j < 5; ++j) {
            playerLines.add("toggle_" + juce::String(j) + "=" + INIUtils::boolToString(player.toggleStates[j]));
        }

        for (int j = 0; j < 5; ++j) {
            playerLines.add("fill_" + juce::String(j) + "=" + INIUtils::boolToString(player.fillStates[j]));
        }

        for (int j = 0; j < 16; ++j) {
            playerLines.add("midi_file_" + juce::String(j) + "=" + INIUtils::escapeINIValue(player.assignedMidiFiles[j]));
        }

        for (int j = 0; j < 8; ++j) {
            playerLines.add("mixer_preset_" + juce::String(j) + "=" + juce::String(player.mixerPresets[j]));
        }

        playerLines.add("pattern_chain_enabled=" + INIUtils::boolToString(player.patternChainEnabled));
        playerLines.add("pattern_chain_loop=" + INIUtils::boolToString(player.patternChainLoop));
        playerLines.add("pattern_chain_count=" + juce::String(player.patternChainIndices.size()));
        for (int j = 0; j < player.patternChainIndices.size(); ++j) {
            playerLines.add("pattern_chain_" + juce::String(j) + "=" + juce::String(player.patternChainIndices[j]));
        }

        sectionData[sectionName] = playerLines;
    }

    return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::loadPlayerSettings(juce::Array<PlayerSettings>& playerSettings) {
    auto file = getINIFilePath(getPlayersFilename());
    if (!file.existsAsFile()) {
        setError("Player Settings file not found");
        return false;
    }

    std::map<juce::String, std::map<juce::String, juce::String>> data;
    if (!INIUtils::readINIFile(file, data)) {
        setError("Failed to read Player Settings file");
        return false;
    }

    playerSettings.clear();

    try {
        for (int i = 0; i < 8; ++i) {
            PlayerSettings player;
            player.setDefaults();

            juce::String sectionName = "player_" + juce::String(i + 1);

            if (data.count(sectionName)) {
                auto& section = data[sectionName];

                if (section.count("enabled"))
                    player.enabled = INIUtils::stringToBool(section["enabled"]);
                if (section.count("drumkit"))
                    player.selectedDrumkit = section["drumkit"];
                if (section.count("volume"))
                    player.volume = INIConfig::clampVolume(section["volume"].getFloatValue());
                if (section.count("pan"))
                    player.pan = INIConfig::clampPan(section["pan"].getFloatValue());
                if (section.count("muted"))
                    player.muted = INIUtils::stringToBool(section["muted"]);
                if (section.count("soloed"))
                    player.soloed = INIUtils::stringToBool(section["soloed"]);
                if (section.count("midi_channel"))
                    player.midiChannel = INIConfig::clampMidiChannel(section["midi_channel"].getIntValue());
                if (section.count("output_channel"))
                    player.outputChannel = section["output_channel"].getIntValue();

                if (section.count("midi_group"))
                    player.selectedMidiGroup = section["midi_group"];
                if (section.count("selected_button"))
                    player.selectedButton = INIConfig::clampButtonIndex(section["selected_button"].getIntValue());

                if (section.count("swing"))
                    player.swingValue = INIConfig::clampSwing(section["swing"].getFloatValue());
                if (section.count("energy"))
                    player.energyValue = INIConfig::clampEnergy(section["energy"].getFloatValue());
                if (section.count("volume_value"))
                    player.volumeValue = INIConfig::clampVolume(section["volume_value"].getFloatValue());

                for (int j = 0; j < 5; ++j) {
                    juce::String key = "toggle_" + juce::String(j);
                    if (section.count(key))
                        player.toggleStates[j] = INIUtils::stringToBool(section[key]);
                }

                for (int j = 0; j < 5; ++j) {
                    juce::String key = "fill_" + juce::String(j);
                    if (section.count(key))
                        player.fillStates[j] = INIUtils::stringToBool(section[key]);
                }

                for (int j = 0; j < 16; ++j) {
                    juce::String key = "midi_file_" + juce::String(j);
                    if (section.count(key))
                        player.assignedMidiFiles[j] = section[key];
                }

                for (int j = 0; j < 8; ++j) {
                    juce::String key = "mixer_preset_" + juce::String(j);
                    if (section.count(key))
                        player.mixerPresets[j] = section[key].getIntValue();
                }

                if (section.count("pattern_chain_enabled"))
                    player.patternChainEnabled = INIUtils::stringToBool(section["pattern_chain_enabled"]);
                if (section.count("pattern_chain_loop"))
                    player.patternChainLoop = INIUtils::stringToBool(section["pattern_chain_loop"]);

                if (section.count("pattern_chain_count")) {
                    int chainCount = section["pattern_chain_count"].getIntValue();
                    player.patternChainIndices.clear();
                    for (int j = 0; j < chainCount; ++j) {
                        juce::String key = "pattern_chain_" + juce::String(j);
                        if (section.count(key)) {
                            player.patternChainIndices.add(section[key].getIntValue());
                        }
                    }
                }
            }

            playerSettings.add(player);
        }

        return true;
    }
    catch (const std::exception& e) {
        setError("Exception loading player settings: " + juce::String(e.what()));
        return false;
    }
}

bool INIDataManager::savePatternGroups(const juce::Array<BeatsButtonGroup>& groups) {
    auto file = getINIFilePath(getPatternGroupsFilename());

    std::map<juce::String, juce::StringArray> sectionData;
    juce::StringArray sections;

    sections.add("general");
    juce::StringArray generalLines;
    generalLines.add("group_count=" + juce::String(groups.size()));
    generalLines.add("last_modified=" + INIUtils::formatTimestamp());
    sectionData["general"] = generalLines;

    for (int i = 0; i < groups.size(); ++i) {
        const auto& group = groups[i];

        juce::String sectionName = "group_" + juce::String(i + 1);
        sections.add(sectionName);

        juce::StringArray groupLines;
        groupLines.add("name=" + INIUtils::escapeINIValue(group.groupName));
        groupLines.add("is_custom=" + INIUtils::boolToString(group.isCustomGroup));
        groupLines.add("selected_button=" + juce::String(group.selectedButton));
        groupLines.add("is_favorite=" + INIUtils::boolToString(group.isFavorite));

        groupLines.add("average_swing=" + INIUtils::floatToString(group.averageSwing, 2));
        groupLines.add("average_velocity=" + INIUtils::floatToString(group.averageVelocity, 2));

        juce::String timeSigString = juce::String(group.timeSignatureNumerator) + "/" +
                                     juce::String(group.timeSignatureDenominator);
        if (INIConfig::TimeSignature::isValidTimeSignature(timeSigString)) {
            groupLines.add("time_signature_num=" + juce::String(group.timeSignatureNumerator));
            groupLines.add("time_signature_den=" + juce::String(group.timeSignatureDenominator));
            groupLines.add("time_signature=" + timeSigString);
        } else {
            groupLines.add("time_signature_num=" + juce::String(INIConfig::Defaults::TIME_SIGNATURE_NUMERATOR));
            groupLines.add("time_signature_den=" + juce::String(INIConfig::Defaults::TIME_SIGNATURE_DENOMINATOR));
            groupLines.add("time_signature=" + INIConfig::Defaults::DEFAULT_TIME_SIGNATURE);
        }

        groupLines.add("groove_tightness=" + INIUtils::floatToString(group.grooveTightness, 2));

        for (int j = 0; j < group.midiFiles.size() && j < 16; ++j) {
            groupLines.add("file_" + juce::String(j) + "=" + INIUtils::escapeINIValue(group.midiFiles[j]));
        }

        sectionData[sectionName] = groupLines;
    }

    return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::loadPatternGroups(juce::Array<BeatsButtonGroup>& groups) {
    auto file = getINIFilePath(getPatternGroupsFilename());
    if (!file.existsAsFile()) {
        setError("Pattern Groups file not found");
        return false;
    }

    std::map<juce::String, std::map<juce::String, juce::String>> data;
    if (!INIUtils::readINIFile(file, data)) {
       setError("Failed to read Pattern Groups file");
       return false;
   }

   groups.clear();

   try {
       for (auto& sectionPair : data) {
           if (sectionPair.first.startsWith("group_")) {
               BeatsButtonGroup group;
               auto& section = sectionPair.second;

               if (section.count("name"))
                   group.groupName = section["name"];
               if (section.count("is_custom"))
                   group.isCustomGroup = INIUtils::stringToBool(section["is_custom"]);
               if (section.count("selected_button"))
                   group.selectedButton = INIConfig::clampButtonIndex(section["selected_button"].getIntValue());
               if (section.count("is_favorite"))
                   group.isFavorite = INIUtils::stringToBool(section["is_favorite"]);

               if (section.count("average_swing"))
                   group.averageSwing = section["average_swing"].getFloatValue();
               if (section.count("average_velocity"))
                   group.averageVelocity = section["average_velocity"].getFloatValue();

               if (section.count("time_signature")) {
                   juce::String timeSig = section["time_signature"];
                   if (INIConfig::TimeSignature::isValidTimeSignature(timeSig)) {
                       auto info = INIConfig::TimeSignature::parseTimeSignature(timeSig);
                       group.timeSignatureNumerator = info.numerator;
                       group.timeSignatureDenominator = info.denominator;
                   }
               } else {
                   if (section.count("time_signature_num"))
                       group.timeSignatureNumerator = section["time_signature_num"].getIntValue();
                   if (section.count("time_signature_den"))
                       group.timeSignatureDenominator = section["time_signature_den"].getIntValue();
               }

               if (section.count("groove_tightness"))
                   group.grooveTightness = section["groove_tightness"].getFloatValue();

               for (int j = 0; j < 16; ++j) {
                   juce::String key = "file_" + juce::String(j);
                   if (section.count(key) && section[key].isNotEmpty()) {
                       group.midiFiles.add(section[key]);
                   }
               }

               if (group.groupName.isNotEmpty()) {
                   groups.add(group);
               }
           }
       }

       return !groups.isEmpty();
   }
   catch (const std::exception& e) {
       setError("Exception loading pattern groups: " + juce::String(e.what()));
       return false;
   }
}

bool INIDataManager::saveCompletePlayerState(int playerIndex, const ComponentState& state) {
   if (!INIConfig::isValidPlayerIndex(playerIndex)) {
       setError("Invalid player index: " + juce::String(playerIndex));
       return false;
   }

   juce::Array<PlayerSettings> allPlayers;

   loadPlayerSettings(allPlayers);

   if (playerIndex < allPlayers.size()) {
       allPlayers.set(playerIndex, state.playerSettings[playerIndex]);
   }

   return savePlayerSettings(allPlayers);
}

bool INIDataManager::loadCompletePlayerState(int playerIndex, ComponentState& state) {
   if (!INIConfig::isValidPlayerIndex(playerIndex)) {
       setError("Invalid player index: " + juce::String(playerIndex));
       return false;
   }

   juce::Array<PlayerSettings> allPlayers;
   if (!loadPlayerSettings(allPlayers)) {
       return false;
   }

   if (playerIndex < allPlayers.size()) {
       state.playerSettings[playerIndex] = allPlayers[playerIndex];
       return true;
   }

   return false;
}

juce::Colour INIDataManager::stringToColor(const juce::String& colorString) {
   if (colorString.startsWith("#") && colorString.length() == 7) {
       return juce::Colour::fromString(colorString);
   }
   return juce::Colours::grey;
}

juce::String INIDataManager::colorToString(const juce::Colour& color) {
   return color.toDisplayString(true);
}

void INIDataManager::setError(const juce::String& error) {
   lastError = error;

}

void INIDataManager::cleanCorruptedFiles() {
   juce::StringArray filesToCheck = {
       getGlobalSettingsFilename(),
       getThemesFilename(),
       getAudioSettingsFilename(),
       getPlayersFilename(),
       getPatternGroupsFilename()
   };

   for (const auto& filename : filesToCheck) {
       auto file = getINIFilePath(filename);
       if (file.existsAsFile()) {
           if (!INIUtils::validateINIFile(file)) {

               file.deleteFile();
           }
       }
   }
}

bool INIDataManager::validateAllFiles() const {
   juce::StringArray filesToValidate = {
       getGlobalSettingsFilename(),
       getThemesFilename(),
       getAudioSettingsFilename(),
       getPlayersFilename(),
       getPatternGroupsFilename()
   };

   for (const auto& filename : filesToValidate) {
       auto file = getINIFilePath(filename);
       if (!file.existsAsFile() || !INIUtils::validateINIFile(file)) {
           return false;
       }
   }

   return true;
}

bool INIDataManager::createDefaultFiles() {
   return createAllRequiredFiles();
}

bool INIDataManager::createBackups() {
   bool success = true;

   juce::StringArray filesToBackup = {
       getGlobalSettingsFilename(),
       getThemesFilename(),
       getAudioSettingsFilename(),
       getPlayersFilename(),
       getPatternGroupsFilename()
   };

   for (const auto& filename : filesToBackup) {
       auto file = getINIFilePath(filename);
       if (file.existsAsFile()) {
           success = success && INIUtils::createBackup(file);
       }
   }

   return success;
}

bool INIDataManager::repairCorruptedFiles() {
   cleanCorruptedFiles();
   return createAllRequiredFiles();
}

juce::StringArray INIDataManager::getValidationReport() const {
   juce::StringArray report;

   report.add("=== INI File Validation Report ===");
   report.add("");

   report.add("Directory Structure:");
   report.add("  OTTO Data Dir: " + juce::String(INIConfig::getOTTODataDirectory().exists() ? "EXISTS" : "MISSING"));
   report.add("  Settings Dir: " + juce::String(INIConfig::getSettingsDirectory().exists() ? "EXISTS" : "MISSING"));
   report.add("  Performance Dir: " + juce::String(INIConfig::getPerformanceDirectory().exists() ? "EXISTS" : "MISSING"));
   report.add("  Patterns Dir: " + juce::String(INIConfig::getPatternsDirectory().exists() ? "EXISTS" : "MISSING"));
   report.add("  Kits Dir: " + juce::String(INIConfig::getKitsDirectory().exists() ? "EXISTS" : "MISSING"));
   report.add("  Mixing Dir: " + juce::String(INIConfig::getMixingDirectory().exists() ? "EXISTS" : "MISSING"));
   report.add("  System Dir: " + juce::String(INIConfig::getSystemDirectory().exists() ? "EXISTS" : "MISSING"));
   report.add("");

   report.add("INI Files:");

   juce::StringArray files = {
       getGlobalSettingsFilename(),
       getThemesFilename(),
       getAudioSettingsFilename(),
       getPlayersFilename(),
       getPatternGroupsFilename()
   };

   for (const auto& filename : files) {
       auto file = getINIFilePath(filename);
       juce::String status = "MISSING";

       if (file.existsAsFile()) {
           status = INIUtils::validateINIFile(file) ? "VALID" : "CORRUPTED";
       }

       report.add("  " + filename + ": " + status);
   }

   report.add("");
   report.add("Overall Status: " + juce::String(validateAllFiles() ? "PASSED" : "FAILED"));

   return report;
}

juce::StringArray INIDataManager::getAvailableThemeNames() {
   juce::StringArray names;
   juce::Array<ThemeSettings> themes;

   if (loadAllThemes(themes)) {
       for (const auto& theme : themes) {
           names.add(theme.themeName);
       }
   }

   return names;
}

bool INIDataManager::createSamplePatternChains() {
   auto file = getINIFilePath(INIConfig::PATTERN_CHAINS_FILE);

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections = { "general" };

   juce::StringArray generalLines;
   generalLines.add("chain_count=4");
   generalLines.add("last_modified=" + INIUtils::formatTimestamp());
   generalLines.add("file_format_version=" + INIConfig::FILE_FORMAT_VERSION);
   sectionData["general"] = generalLines;

   for (int i = 0; i < 4; ++i) {
       juce::String sectionName = "chain_" + juce::String(i);
       sections.add(sectionName);

       juce::StringArray chainLines;
       chainLines.add("chain_id=" + juce::String(i));
       chainLines.add("chain_name=Pattern Chain " + juce::String(i + 1));
       chainLines.add("pattern_count=4");
       chainLines.add("loop_enabled=1");
       chainLines.add("tempo_sync=1");

       for (int j = 0; j < 4; ++j) {
           chainLines.add("pattern_" + juce::String(j) + "_id=" + juce::String(j));
           chainLines.add("pattern_" + juce::String(j) + "_bars=1");
           chainLines.add("pattern_" + juce::String(j) + "_repeat=1");
       }

       sectionData[sectionName] = chainLines;
   }

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::createSampleDrumKits() {
   auto file = getINIFilePath(INIConfig::DRUM_KITS_FILE);

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections = { "general" };

   juce::StringArray generalLines;
   generalLines.add("kit_count=" + juce::String(INIConfig::KIT_TYPES.size()));
   generalLines.add("last_modified=" + INIUtils::formatTimestamp());
   generalLines.add("file_format_version=" + INIConfig::FILE_FORMAT_VERSION);
   sectionData["general"] = generalLines;

   for (int i = 0; i < INIConfig::KIT_TYPES.size(); ++i) {
       juce::String kitType = INIConfig::KIT_TYPES[i];
       juce::String sectionName = "kit_" + juce::String(i);
       sections.add(sectionName);

       juce::StringArray kitLines;
       kitLines.add("kit_id=" + juce::String(i));
       kitLines.add("kit_name=" + kitType);
       kitLines.add("kit_type=" + kitType);
       kitLines.add("manufacturer=" + (i < 3 ? INIConfig::MANUFACTURERS[0] : INIConfig::MANUFACTURERS[3]));
       kitLines.add("description=" + kitType + " drum kit with high-quality samples");
       kitLines.add("sample_count=16");
       kitLines.add("version=1.0");
       kitLines.add("author=OTTO Factory");

       kitLines.add("kick_note=" + juce::String(INIConfig::GMDrums::BASS_DRUM_1));
       kitLines.add("snare_note=" + juce::String(INIConfig::GMDrums::ACOUSTIC_SNARE));
       kitLines.add("hihat_closed_note=" + juce::String(INIConfig::GMDrums::CLOSED_HI_HAT));
       kitLines.add("hihat_open_note=" + juce::String(INIConfig::GMDrums::OPEN_HI_HAT));
       kitLines.add("crash_note=" + juce::String(INIConfig::GMDrums::CRASH_CYMBAL_1));
       kitLines.add("ride_note=" + juce::String(INIConfig::GMDrums::RIDE_CYMBAL_1));
       kitLines.add("tom1_note=" + juce::String(INIConfig::GMDrums::HIGH_TOM));
       kitLines.add("tom2_note=" + juce::String(INIConfig::GMDrums::LOW_TOM));

       kitLines.add("velocity_layers=3");
       kitLines.add("round_robin=4");

       kitLines.add("sfz_path=DrumKits/" + kitType + "/" + kitType + ".sfz");
       kitLines.add("samples_path=DrumKits/" + kitType + "/Samples/");

       sectionData[sectionName] = kitLines;
   }

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::createSampleMidiLayouts() {
   auto inFile = getINIFilePath(INIConfig::MIDI_IN_LAYOUT_FILE);

   std::map<juce::String, juce::StringArray> inSectionData;
   juce::StringArray inSections = { "general", "default_layout" };

   juce::StringArray inGeneralLines;
   inGeneralLines.add("layout_count=1");
   inGeneralLines.add("active_layout=default_layout");
   inGeneralLines.add("last_modified=" + INIUtils::formatTimestamp());
   inSectionData["general"] = inGeneralLines;

   juce::StringArray inLayoutLines;
   inLayoutLines.add("layout_name=Default MIDI Input");
   inLayoutLines.add("midi_channel=10");
   inLayoutLines.add("velocity_curve=linear");
   inLayoutLines.add("velocity_min=1");
   inLayoutLines.add("velocity_max=127");

   inLayoutLines.add("note_36=Kick");
   inLayoutLines.add("note_38=Snare");
   inLayoutLines.add("note_42=HiHat_Closed");
   inLayoutLines.add("note_46=HiHat_Open");
   inLayoutLines.add("note_49=Crash");
   inLayoutLines.add("note_51=Ride");

   inSectionData["default_layout"] = inLayoutLines;

   if (!INIUtils::writeINIFile(inFile, inSections, inSectionData)) {
       return false;
   }

   auto outFile = getINIFilePath(INIConfig::MIDI_OUT_LAYOUT_FILE);

   std::map<juce::String, juce::StringArray> outSectionData;
   juce::StringArray outSections = { "general", "default_output" };

   juce::StringArray outGeneralLines;
   outGeneralLines.add("layout_count=1");
   outGeneralLines.add("active_layout=default_output");
   outGeneralLines.add("last_modified=" + INIUtils::formatTimestamp());
   outSectionData["general"] = outGeneralLines;

   juce::StringArray outLayoutLines;
   outLayoutLines.add("layout_name=Default MIDI Output");
   outLayoutLines.add("midi_channel=10");
   outLayoutLines.add("send_note_off=1");
   outLayoutLines.add("send_aftertouch=0");
   outLayoutLines.add("send_control_change=1");

   for (int i = 0; i < 8; ++i) {
       outLayoutLines.add("channel_" + juce::String(i) + "_enabled=1");
       outLayoutLines.add("channel_" + juce::String(i) + "_midi_channel=" + juce::String(i + 1));
       outLayoutLines.add("channel_" + juce::String(i) + "_transpose=0");
   }

   outSectionData["default_output"] = outLayoutLines;

   return INIUtils::writeINIFile(outFile, outSections, outSectionData);
}

bool INIDataManager::createSampleChannelPresetGroups() {
   auto file = getINIFilePath(INIConfig::CHANNEL_PRESET_GROUPS_FILE);

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections = { "general" };

   juce::StringArray generalLines;
   generalLines.add("group_count=4");
   generalLines.add("last_modified=" + INIUtils::formatTimestamp());
   sectionData["general"] = generalLines;

   juce::StringArray groupNames = { "Factory", "Rock", "Electronic", "User" };

   for (int i = 0; i < groupNames.size(); ++i) {
       juce::String sectionName = "group_" + juce::String(i);
       sections.add(sectionName);

       juce::StringArray groupLines;
       groupLines.add("group_id=" + juce::String(i));
       groupLines.add("group_name=" + groupNames[i]);
       groupLines.add("preset_count=" + juce::String(i < 3 ? 5 : 0));
       groupLines.add("is_factory=" + juce::String(i < 3 ? 1 : 0));
       groupLines.add("description=" + groupNames[i] + " channel presets");

       sectionData[sectionName] = groupLines;
   }

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::createSampleChannelPresets() {
   auto file = getINIFilePath(INIConfig::CHANNEL_PRESETS_FILE);

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections = { "general" };

   juce::StringArray generalLines;
   generalLines.add("preset_count=15");
   generalLines.add("last_modified=" + INIUtils::formatTimestamp());
   sectionData["general"] = generalLines;

   struct ChannelPreset {
       juce::String name;
       int groupId;
       float volume;
       float pan;
       float eqLow;
       float eqMid;
       float eqHigh;
       float reverbSend;
       float delaySend;
   };

   ChannelPreset presets[] = {
       {"Clean", 0, 0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
       {"Punchy", 0, 0.85f, 0.0f, 2.0f, -1.0f, 1.0f, 0.0f, 0.0f},
       {"Warm", 0, 0.75f, 0.0f, 3.0f, 1.0f, -2.0f, 0.1f, 0.0f},
       {"Bright", 0, 0.8f, 0.0f, -1.0f, 0.0f, 3.0f, 0.0f, 0.0f},
       {"Vintage", 0, 0.7f, 0.0f, 1.0f, -2.0f, -1.0f, 0.2f, 0.1f},

       {"Rock Kick", 1, 0.9f, 0.0f, 3.0f, -2.0f, 2.0f, 0.0f, 0.0f},
       {"Rock Snare", 1, 0.85f, 0.0f, 1.0f, 2.0f, 1.0f, 0.15f, 0.0f},
       {"Rock Toms", 1, 0.8f, 0.0f, 2.0f, 0.0f, 0.0f, 0.1f, 0.05f},
       {"Rock Cymbals", 1, 0.7f, 0.0f, -2.0f, 0.0f, 2.0f, 0.2f, 0.0f},
       {"Rock Room", 1, 0.75f, 0.0f, 0.0f, 0.0f, 0.0f, 0.3f, 0.1f},

       {"808 Kick", 2, 0.95f, 0.0f, 5.0f, -3.0f, 0.0f, 0.0f, 0.0f},
       {"Trap Snare", 2, 0.9f, 0.0f, -1.0f, 3.0f, 2.0f, 0.05f, 0.1f},
       {"EDM Hats", 2, 0.8f, 0.3f, -3.0f, 0.0f, 4.0f, 0.0f, 0.2f},
       {"Dubstep Sub", 2, 1.0f, 0.0f, 6.0f, -4.0f, -2.0f, 0.0f, 0.0f},
       {"Techno Percussion", 2, 0.85f, -0.2f, 0.0f, 1.0f, 3.0f, 0.1f, 0.15f}
   };

   for (int i = 0; i < 15; ++i) {
       const auto& preset = presets[i];
       juce::String sectionName = "preset_" + juce::String(i);
       sections.add(sectionName);

       juce::StringArray presetLines;
       presetLines.add("preset_id=" + juce::String(i));
       presetLines.add("preset_name=" + preset.name);
       presetLines.add("group_id=" + juce::String(preset.groupId));
       presetLines.add("volume=" + INIUtils::floatToString(preset.volume, 2));
       presetLines.add("pan=" + INIUtils::floatToString(preset.pan, 2));
       presetLines.add("eq_low=" + INIUtils::floatToString(preset.eqLow, 1));
       presetLines.add("eq_mid=" + INIUtils::floatToString(preset.eqMid, 1));
       presetLines.add("eq_high=" + INIUtils::floatToString(preset.eqHigh, 1));
       presetLines.add("reverb_send=" + INIUtils::floatToString(preset.reverbSend, 2));
       presetLines.add("delay_send=" + INIUtils::floatToString(preset.delaySend, 2));
       presetLines.add("compressor_enabled=0");
       presetLines.add("gate_enabled=0");

       sectionData[sectionName] = presetLines;
   }

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::createSampleEQPresets() {
   auto file = getINIFilePath(INIConfig::EQ_PRESETS_FILE);

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections = { "general" };

   juce::StringArray generalLines;
   generalLines.add("preset_count=8");
   generalLines.add("last_modified=" + INIUtils::formatTimestamp());
   sectionData["general"] = generalLines;

   struct EQPreset {
       juce::String name;
       float lowFreq, lowGain, lowQ;
       float midFreq, midGain, midQ;
       float highFreq, highGain, highQ;
   };

   EQPreset eqPresets[] = {
       {"Flat", 80.0f, 0.0f, 0.7f, 800.0f, 0.0f, 0.7f, 8000.0f, 0.0f, 0.7f},
       {"Punch", 60.0f, 3.0f, 0.5f, 1000.0f, -2.0f, 0.8f, 10000.0f, 2.0f, 0.6f},
       {"Warm", 100.0f, 4.0f, 0.4f, 600.0f, 2.0f, 0.6f, 6000.0f, -3.0f, 0.8f},
       {"Bright", 80.0f, -2.0f, 0.7f, 1200.0f, 1.0f, 0.7f, 12000.0f, 4.0f, 0.5f},
       {"Scooped", 80.0f, 2.0f, 0.6f, 800.0f, -4.0f, 0.8f, 10000.0f, 3.0f, 0.6f},
       {"Telephone", 300.0f, -12.0f, 1.0f, 2000.0f, 6.0f, 2.0f, 5000.0f, -12.0f, 1.0f},
       {"Radio", 150.0f, -6.0f, 0.8f, 2500.0f, 3.0f, 1.2f, 8000.0f, -4.0f, 0.9f},
       {"Vintage", 120.0f, 2.0f, 0.5f, 800.0f, -1.0f, 0.7f, 5000.0f, -2.0f, 0.8f}
   };

   for (int i = 0; i < 8; ++i) {
       const auto& preset = eqPresets[i];
       juce::String sectionName = "eq_preset_" + juce::String(i);
       sections.add(sectionName);

       juce::StringArray presetLines;
       presetLines.add("preset_id=" + juce::String(i));
       presetLines.add("preset_name=" + preset.name);
       presetLines.add("low_freq=" + INIUtils::floatToString(preset.lowFreq, 1));
       presetLines.add("low_gain=" + INIUtils::floatToString(preset.lowGain, 1));
       presetLines.add("low_q=" + INIUtils::floatToString(preset.lowQ, 2));
       presetLines.add("mid_freq=" + INIUtils::floatToString(preset.midFreq, 1));
       presetLines.add("mid_gain=" + INIUtils::floatToString(preset.midGain, 1));
       presetLines.add("mid_q=" + INIUtils::floatToString(preset.midQ, 2));
       presetLines.add("high_freq=" + INIUtils::floatToString(preset.highFreq, 1));
       presetLines.add("high_gain=" + INIUtils::floatToString(preset.highGain, 1));
       presetLines.add("high_q=" + INIUtils::floatToString(preset.highQ, 2));
       presetLines.add("enabled=1");

       sectionData[sectionName] = presetLines;
   }

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::createSampleMasterChannelPresets() {
   auto file = getINIFilePath(INIConfig::MASTER_CHANNEL_PRESETS_FILE);

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections = { "general" };

   juce::StringArray generalLines;
   generalLines.add("preset_count=5");
   generalLines.add("last_modified=" + INIUtils::formatTimestamp());
   sectionData["general"] = generalLines;

   struct MasterPreset {
       juce::String name;
       float volume;
       float limiterThreshold;
       float limiterRatio;
       float limiterAttack;
       float limiterRelease;
       bool limiterEnabled;
   };

   MasterPreset masterPresets[] = {
       {"Default", 0.8f, -3.0f, 10.0f, 1.0f, 50.0f, true},
       {"Loud", 0.95f, -1.0f, 20.0f, 0.5f, 30.0f, true},
       {"Safe", 0.7f, -6.0f, 4.0f, 2.0f, 100.0f, true},
       {"Dynamic", 0.85f, -6.0f, 3.0f, 5.0f, 200.0f, false},
       {"Broadcast", 0.75f, -2.0f, 8.0f, 1.0f, 50.0f, true}
   };

   for (int i = 0; i < 5; ++i) {
       const auto& preset = masterPresets[i];
       juce::String sectionName = "master_preset_" + juce::String(i);
       sections.add(sectionName);

       juce::StringArray presetLines;
       presetLines.add("preset_id=" + juce::String(i));
       presetLines.add("preset_name=" + preset.name);
       presetLines.add("master_volume=" + INIUtils::floatToString(preset.volume, 2));
       presetLines.add("limiter_threshold=" + INIUtils::floatToString(preset.limiterThreshold, 1));
       presetLines.add("limiter_ratio=" + INIUtils::floatToString(preset.limiterRatio, 1));
       presetLines.add("limiter_attack=" + INIUtils::floatToString(preset.limiterAttack, 1));
       presetLines.add("limiter_release=" + INIUtils::floatToString(preset.limiterRelease, 1));
       presetLines.add("limiter_enabled=" + INIUtils::boolToString(preset.limiterEnabled));
       presetLines.add("eq_enabled=0");
       presetLines.add("compressor_enabled=0");

       sectionData[sectionName] = presetLines;
   }

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::createSampleMidiDevices() {
   auto file = getINIFilePath(INIConfig::MIDI_DEVICES_FILE);

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections = { "general", "devices" };

   juce::StringArray generalLines;
   generalLines.add("scan_on_startup=1");
   generalLines.add("last_scan=" + INIUtils::formatTimestamp());
   generalLines.add("device_count=0");
   sectionData["general"] = generalLines;

   auto midiInputs = juce::MidiInput::getAvailableDevices();
   auto midiOutputs = juce::MidiOutput::getAvailableDevices();

   juce::StringArray devicesLines;
   devicesLines.add("input_count=" + juce::String(midiInputs.size()));
   devicesLines.add("output_count=" + juce::String(midiOutputs.size()));

   for (int i = 0; i < midiInputs.size(); ++i) {
       const auto& device = midiInputs[i];
       devicesLines.add("input_" + juce::String(i) + "_name=" + INIUtils::escapeINIValue(device.name));
       devicesLines.add("input_" + juce::String(i) + "_identifier=" + INIUtils::escapeINIValue(device.identifier));
       devicesLines.add("input_" + juce::String(i) + "_enabled=0");
   }

   for (int i = 0; i < midiOutputs.size(); ++i) {
       const auto& device = midiOutputs[i];
       devicesLines.add("output_" + juce::String(i) + "_name=" + INIUtils::escapeINIValue(device.name));
       devicesLines.add("output_" + juce::String(i) + "_identifier=" + INIUtils::escapeINIValue(device.identifier));
       devicesLines.add("output_" + juce::String(i) + "_enabled=0");
   }

   sectionData["devices"] = devicesLines;

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::createSampleFileIndex() {
   auto file = getINIFilePath(INIConfig::FILE_INDEX_FILE);

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections = { "general", "index" };

   juce::StringArray generalLines;
   generalLines.add("index_version=1.0");
   generalLines.add("last_update=" + INIUtils::formatTimestamp());
   generalLines.add("total_files=13");
   generalLines.add("auto_index=1");
   sectionData["general"] = generalLines;

   juce::StringArray indexLines;
   juce::StringArray fileList = {
       INIConfig::GLOBAL_SETTINGS_FILE, INIConfig::THEMES_FILE, INIConfig::AUDIO_SETTINGS_FILE,
       INIConfig::PRESETS_FILE, INIConfig::PLAYERS_FILE, INIConfig::PATTERN_CHAINS_FILE,
       INIConfig::PATTERN_GROUPS_FILE, INIConfig::DRUM_KITS_FILE, INIConfig::MIDI_IN_LAYOUT_FILE,
       INIConfig::MIDI_OUT_LAYOUT_FILE, INIConfig::CHANNEL_PRESET_GROUPS_FILE,
       INIConfig::CHANNEL_PRESETS_FILE, INIConfig::EQ_PRESETS_FILE
   };

   for (int i = 0; i < fileList.size(); ++i) {
       auto filePath = getINIFilePath(fileList[i]);
       indexLines.add("file_" + juce::String(i) + "_name=" + fileList[i]);
       indexLines.add("file_" + juce::String(i) + "_exists=" +
                     INIUtils::boolToString(filePath.existsAsFile()));
       indexLines.add("file_" + juce::String(i) + "_size=" +
                     juce::String(filePath.getSize()));
       indexLines.add("file_" + juce::String(i) + "_modified=" +
                     filePath.getLastModificationTime().toString(true, true));
       indexLines.add("file_" + juce::String(i) + "_valid=" +
                     INIUtils::boolToString(INIUtils::validateINIFile(filePath)));
   }

   sectionData["index"] = indexLines;

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::createAllSampleFiles() {
   return createAllRequiredFiles();
}

bool INIDataManager::loadThemeSettings(ThemeSettings& settings) {
   juce::Array<ThemeSettings> themes;
   if (loadAllThemes(themes)) {
       for (const auto& theme : themes) {
           if (theme.themeID == settings.themeID) {
               settings = theme;
               return true;
           }
       }
   }
   return false;
}

bool INIDataManager::saveThemeSettings(const ThemeSettings& settings) {
   juce::Array<ThemeSettings> themes;
   loadAllThemes(themes);

   bool found = false;
   for (auto& theme : themes) {
       if (theme.themeID == settings.themeID) {
           theme = settings;
           found = true;
           break;
       }
   }

   if (!found) {
       themes.add(settings);
   }

   return saveAllThemes(themes);
}

void INIDataManager::enableAutoSave(int intervalSeconds) {
   autoSaveInterval = intervalSeconds;
   if (intervalSeconds > 0) {
       startTimer(intervalSeconds * INIConfig::LayoutConstants::iniManagerTimerInterval);
   } else {
       stopTimer();
   }
}

void INIDataManager::timerCallback() {
   if (currentState != nullptr) {
       saveAllSettings(*currentState);
   }
}

bool INIDataManager::saveWithVersion(const ComponentState& state, const juce::String& version) {
   auto timestamp = INIUtils::formatTimestamp();
   auto versionString = version + "_" + timestamp;

   juce::StringArray filesToBackup = {
       getGlobalSettingsFilename(),
       getThemesFilename(),
       getAudioSettingsFilename(),
       getPlayersFilename(),
       getPatternGroupsFilename()
   };

   for (const auto& filename : filesToBackup) {
       auto file = getINIFilePath(filename);
       if (file.existsAsFile()) {
           juce::File backupFile = file.getParentDirectory().getChildFile(
               file.getFileNameWithoutExtension() + "_v" + versionString + ".ini");
           file.copyFileTo(backupFile);
       }
   }

   auto originalVersion = state.globalSettings.fileFormatVersion;
   auto originalTimestamp = state.globalSettings.lastModified;

   const_cast<GlobalSettings&>(state.globalSettings).fileFormatVersion = version;
   const_cast<GlobalSettings&>(state.globalSettings).lastModified = timestamp;

   bool success = saveAllSettings(state);

   const_cast<GlobalSettings&>(state.globalSettings).fileFormatVersion = originalVersion;
   const_cast<GlobalSettings&>(state.globalSettings).lastModified = originalTimestamp;

   return success;
}

bool INIDataManager::saveMidiAnalysisCache(const std::map<juce::String, MidiGrooveAnalysis>& cache) {
   auto file = getINIFilePath(getMidiAnalysisCacheFilename());

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections = { "general" };

   juce::StringArray generalLines;
   generalLines.add("cache_count=" + juce::String(cache.size()));
   generalLines.add("last_modified=" + INIUtils::formatTimestamp());
   sectionData["general"] = generalLines;

   int index = 0;
   for (const auto& pair : cache) {
       juce::String sectionName = "analysis_" + juce::String(index);
       sections.add(sectionName);

       juce::StringArray analysisLines;
       analysisLines.add("filename=" + INIUtils::escapeINIValue(pair.first));
       analysisLines.add("average_swing=" + INIUtils::floatToString(pair.second.averageSwing, 2));
       analysisLines.add("average_velocity=" + INIUtils::floatToString(pair.second.averageVelocity, 2));
       analysisLines.add("groove_tightness=" + INIUtils::floatToString(pair.second.grooveTightness, 2));
       analysisLines.add("time_signature_num=" + juce::String(pair.second.timeSignatureNumerator));
       analysisLines.add("time_signature_den=" + juce::String(pair.second.timeSignatureDenominator));
       analysisLines.add("tempo=" + INIUtils::floatToString(pair.second.tempo, 1));
       analysisLines.add("number_of_bars=" + juce::String(pair.second.numberOfBars));
       analysisLines.add("velocity_range=" + INIUtils::floatToString(pair.second.velocityRange, 2));
       analysisLines.add("velocity_variation=" + INIUtils::floatToString(pair.second.velocityVariation, 2));
       analysisLines.add("timing_deviation=" + INIUtils::floatToString(pair.second.timingDeviation, 3));
       analysisLines.add("note_density=" + INIUtils::floatToString(pair.second.noteDensity, 2));

       sectionData[sectionName] = analysisLines;
       index++;
   }

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::loadMidiAnalysisCache(std::map<juce::String, MidiGrooveAnalysis>& cache) {
   auto file = getINIFilePath(getMidiAnalysisCacheFilename());
   if (!file.existsAsFile()) {
       return false;
   }

   std::map<juce::String, std::map<juce::String, juce::String>> data;
   if (!INIUtils::readINIFile(file, data)) {
       return false;
   }

   cache.clear();

   try {
       for (auto& sectionPair : data) {
           if (sectionPair.first.startsWith("analysis_")) {
               auto& section = sectionPair.second;

               if (section.count("filename")) {
                   juce::String filename = section["filename"];
                   MidiGrooveAnalysis analysis;

                   if (section.count("average_swing"))
                       analysis.averageSwing = section["average_swing"].getFloatValue();
                   if (section.count("average_velocity"))
                       analysis.averageVelocity = section["average_velocity"].getFloatValue();
                   if (section.count("groove_tightness"))
                       analysis.grooveTightness = section["groove_tightness"].getFloatValue();
                   if (section.count("time_signature_num"))
                       analysis.timeSignatureNumerator = section["time_signature_num"].getIntValue();
                   if (section.count("time_signature_den"))
                       analysis.timeSignatureDenominator = section["time_signature_den"].getIntValue();
                   if (section.count("tempo"))
                       analysis.tempo = section["tempo"].getFloatValue();
                   if (section.count("number_of_bars"))
                       analysis.numberOfBars = section["number_of_bars"].getIntValue();
                   if (section.count("velocity_range"))
                       analysis.velocityRange = section["velocity_range"].getFloatValue();
                   if (section.count("velocity_variation"))
                       analysis.velocityVariation = section["velocity_variation"].getFloatValue();
                   if (section.count("timing_deviation"))
                       analysis.timingDeviation = section["timing_deviation"].getFloatValue();
                   if (section.count("note_density"))
                       analysis.noteDensity = section["note_density"].getFloatValue();

                   cache[filename] = analysis;
               }
           }
       }

       return true;
   }
   catch (const std::exception& e) {
       setError("Exception loading MIDI analysis cache: " + juce::String(e.what()));
       return false;
   }
}

bool INIDataManager::saveControllerPresets(const juce::Array<MidiEngine::MidiControllerPreset>& presets) {
   auto file = getINIFilePath(getControllerPresetsFilename());

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections = { "general" };

   juce::StringArray generalLines;
   generalLines.add("preset_count=" + juce::String(presets.size()));
   generalLines.add("last_modified=" + INIUtils::formatTimestamp());
   sectionData["general"] = generalLines;

   for (int i = 0; i < presets.size(); ++i) {
       const auto& preset = presets[i];
       juce::String sectionName = "preset_" + juce::String(i);
       sections.add(sectionName);

       juce::StringArray presetLines;
       presetLines.add("name=" + INIUtils::escapeINIValue(preset.name));
       presetLines.add("manufacturer=" + INIUtils::escapeINIValue(preset.manufacturer));
       presetLines.add("has_led_feedback=" + INIUtils::boolToString(preset.hasLEDFeedback));
       presetLines.add("sysex_device_id=" + juce::String(preset.sysexDeviceID));
       presetLines.add("mapping_count=" + juce::String(preset.mappings.size()));

       for (int j = 0; j < preset.mappings.size(); ++j) {
           const auto& mapping = preset.mappings[j];
           juce::String prefix = "mapping_" + juce::String(j) + "_";

           presetLines.add(prefix + "cc=" + juce::String(mapping.ccNumber));
           presetLines.add(prefix + "channel=" + juce::String(mapping.channel));
           presetLines.add(prefix + "parameter=" + INIUtils::escapeINIValue(mapping.parameterID));
           presetLines.add(prefix + "min=" + INIUtils::floatToString(mapping.minValue, 2));
           presetLines.add(prefix + "max=" + INIUtils::floatToString(mapping.maxValue, 2));
           presetLines.add(prefix + "enabled=" + INIUtils::boolToString(mapping.enabled));
           presetLines.add(prefix + "feedback=" + INIUtils::boolToString(mapping.sendFeedback));
           presetLines.add(prefix + "feedback_type=" + juce::String(mapping.feedbackType));
       }

       sectionData[sectionName] = presetLines;
   }

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::loadControllerPresets(juce::Array<MidiEngine::MidiControllerPreset>& presets) {
   auto file = getINIFilePath(getControllerPresetsFilename());
   if (!file.existsAsFile()) {
       return false;
   }

   std::map<juce::String, std::map<juce::String, juce::String>> data;
   if (!INIUtils::readINIFile(file, data)) {
       return false;
   }

   presets.clear();

   try {
       for (auto& sectionPair : data) {
           if (sectionPair.first.startsWith("preset_")) {
               auto& section = sectionPair.second;
               MidiEngine::MidiControllerPreset preset;

               if (section.count("name"))
                   preset.name = section["name"];
               if (section.count("manufacturer"))
                   preset.manufacturer = section["manufacturer"];
               if (section.count("has_led_feedback"))
                   preset.hasLEDFeedback = INIUtils::stringToBool(section["has_led_feedback"]);
               if (section.count("sysex_device_id"))
                   preset.sysexDeviceID = section["sysex_device_id"].getIntValue();

               int mappingCount = 0;
               if (section.count("mapping_count"))
                   mappingCount = section["mapping_count"].getIntValue();

               for (int j = 0; j < mappingCount; ++j) {
                   juce::String prefix = "mapping_" + juce::String(j) + "_";
                   MidiEngine::MidiMapping mapping;

                   if (section.count(prefix + "cc"))
                       mapping.ccNumber = section[prefix + "cc"].getIntValue();
                   if (section.count(prefix + "channel"))
                       mapping.channel = section[prefix + "channel"].getIntValue();
                   if (section.count(prefix + "parameter"))
                       mapping.parameterID = section[prefix + "parameter"];
                   if (section.count(prefix + "min"))
                       mapping.minValue = section[prefix + "min"].getFloatValue();
                   if (section.count(prefix + "max"))
                       mapping.maxValue = section[prefix + "max"].getFloatValue();
                   if (section.count(prefix + "enabled"))
                       mapping.enabled = INIUtils::stringToBool(section[prefix + "enabled"]);
                   if (section.count(prefix + "feedback"))
                       mapping.sendFeedback = INIUtils::stringToBool(section[prefix + "feedback"]);
                   if (section.count(prefix + "feedback_type"))
                       mapping.feedbackType = section[prefix + "feedback_type"].getIntValue();

                   preset.mappings.add(mapping);
               }

               presets.add(preset);
           }
       }

       return true;
   }
   catch (const std::exception& e) {
       setError("Exception loading controller presets: " + juce::String(e.what()));
       return false;
   }
}

bool INIDataManager::saveMidiMappings(const juce::Array<MidiEngine::MidiMapping>& mappings) {
   auto file = getINIFilePath(getMidiMappingsFilename());

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections = { "general" };

   juce::StringArray generalLines;
   generalLines.add("mapping_count=" + juce::String(mappings.size()));
   generalLines.add("last_modified=" + INIUtils::formatTimestamp());
   sectionData["general"] = generalLines;

   for (int i = 0; i < mappings.size(); ++i) {
       const auto& mapping = mappings[i];
       juce::String sectionName = "mapping_" + juce::String(i);
       sections.add(sectionName);

       juce::StringArray mappingLines;
       mappingLines.add("cc_number=" + juce::String(mapping.ccNumber));
       mappingLines.add("channel=" + juce::String(mapping.channel));
       mappingLines.add("parameter_id=" + INIUtils::escapeINIValue(mapping.parameterID));
       mappingLines.add("min_value=" + INIUtils::floatToString(mapping.minValue, 2));
       mappingLines.add("max_value=" + INIUtils::floatToString(mapping.maxValue, 2));
       mappingLines.add("is_learning=" + INIUtils::boolToString(mapping.isLearning));
       mappingLines.add("enabled=" + INIUtils::boolToString(mapping.enabled));
       mappingLines.add("send_feedback=" + INIUtils::boolToString(mapping.sendFeedback));
       mappingLines.add("feedback_type=" + juce::String(mapping.feedbackType));

       sectionData[sectionName] = mappingLines;
   }

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::loadMidiMappings(juce::Array<MidiEngine::MidiMapping>& mappings) {
   auto file = getINIFilePath(getMidiMappingsFilename());
   if (!file.existsAsFile()) {
       return false;
   }

   std::map<juce::String, std::map<juce::String, juce::String>> data;
   if (!INIUtils::readINIFile(file, data)) {
       return false;
   }

   mappings.clear();

   try {
       for (auto& sectionPair : data) {
           if (sectionPair.first.startsWith("mapping_")) {
               auto& section = sectionPair.second;
               MidiEngine::MidiMapping mapping;

               if (section.count("cc_number"))
                   mapping.ccNumber = section["cc_number"].getIntValue();
               if (section.count("channel"))
                   mapping.channel = section["channel"].getIntValue();
               if (section.count("parameter_id"))
                   mapping.parameterID = section["parameter_id"];
               if (section.count("min_value"))
                   mapping.minValue = section["min_value"].getFloatValue();
               if (section.count("max_value"))
                   mapping.maxValue = section["max_value"].getFloatValue();
               if (section.count("is_learning"))
                   mapping.isLearning = INIUtils::stringToBool(section["is_learning"]);
               if (section.count("enabled"))
                   mapping.enabled = INIUtils::stringToBool(section["enabled"]);
               if (section.count("send_feedback"))
                   mapping.sendFeedback = INIUtils::stringToBool(section["send_feedback"]);
               if (section.count("feedback_type"))
                   mapping.feedbackType = section["feedback_type"].getIntValue();

               mappings.add(mapping);
           }
       }

       return true;
   }
   catch (const std::exception& e) {
       setError("Exception loading MIDI mappings: " + juce::String(e.what()));
       return false;
   }
}

bool INIDataManager::saveGrooveTemplates(const juce::StringArray& templates) {
   auto file = getINIFilePath(getGrooveTemplatesFilename());

   std::map<juce::String, juce::StringArray> sectionData;
   juce::StringArray sections = { "general" };

   juce::StringArray generalLines;
   generalLines.add("template_count=" + juce::String(templates.size()));
   generalLines.add("last_modified=" + INIUtils::formatTimestamp());
   sectionData["general"] = generalLines;

   sections.add("templates");
   juce::StringArray templateLines;
   for (int i = 0; i < templates.size(); ++i) {
       templateLines.add("template_" + juce::String(i) + "=" + INIUtils::escapeINIValue(templates[i]));
   }
   sectionData["templates"] = templateLines;

   return INIUtils::writeINIFile(file, sections, sectionData);
}

bool INIDataManager::loadGrooveTemplates(juce::StringArray& templates) {
   auto file = getINIFilePath(getGrooveTemplatesFilename());
   if (!file.existsAsFile()) {
       return false;
   }

   std::map<juce::String, std::map<juce::String, juce::String>> data;
   if (!INIUtils::readINIFile(file, data)) {
       return false;
   }

   templates.clear();

   try {
       if (data.count("templates")) {
           auto& section = data["templates"];

           for (auto& pair : section) {
               if (pair.first.startsWith("template_")) {
                   templates.add(pair.second);
               }
           }
       }

       return true;
   }
   catch (const std::exception& e) {
       setError("Exception loading groove templates: " + juce::String(e.what()));
       return false;
   }
}

bool INIDataManager::savePreset(const juce::String& presetName, const ComponentState& state) {
    if (presetName.isEmpty() || !isValidPresetName(presetName)) {
        setError("Invalid preset name: " + presetName);
        return false;
    }

    // Determine category - Default preset goes in "Defaults" folder, others in "User" folder
    juce::String categoryName = (presetName == "Default") ? "Defaults" : "User";
    
    auto presetsDir = INIConfig::getPresetsDirectory();
    auto categoryDir = presetsDir.getChildFile(categoryName);
    
    // Ensure category directory exists
    if (!categoryDir.exists()) {
        auto result = categoryDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create preset category directory: " + result.getErrorMessage());
            return false;
        }
    }
    
    auto presetFile = categoryDir.getChildFile(presetName + ".ini");

    // Create INI structure for the preset
    std::map<juce::String, juce::StringArray> sectionData;
    juce::StringArray sections;

    // General section
    sections.add("General");
    juce::StringArray generalLines;
    generalLines.add("PresetName=" + presetName);
    generalLines.add("Created=" + INIUtils::formatTimestamp());
    generalLines.add("Category=" + categoryName);
    generalLines.add("Version=" + INIConfig::FILE_FORMAT_VERSION);
    sectionData["General"] = generalLines;

    // Transport section
    sections.add("Transport");
    juce::StringArray transportLines;
    transportLines.add("Tempo=" + juce::String(state.tempo));
    transportLines.add("PlayState=" + juce::String(state.playState ? 1 : 0));
    transportLines.add("CurrentPreset=" + juce::String(state.currentPreset));
    sectionData["Transport"] = transportLines;

    // Mixer section
    sections.add("Mixer");
    juce::StringArray mixerLines;
    mixerLines.add("MasterVolume=" + juce::String(state.sliderValues.count("masterVolume") > 0 ? 
                   state.sliderValues.at("masterVolume") : INIConfig::Defaults::DEFAULT_MASTER_VOLUME));
    mixerLines.add("ReverbMix=" + juce::String(state.sliderValues.count("reverbMix") > 0 ? 
                   state.sliderValues.at("reverbMix") : INIConfig::Defaults::DEFAULT_REVERB_MIX));
    mixerLines.add("DelayMix=" + juce::String(state.sliderValues.count("delayMix") > 0 ? 
                   state.sliderValues.at("delayMix") : INIConfig::Defaults::DEFAULT_DELAY_MIX));
    sectionData["Mixer"] = mixerLines;

    // Player sections
    for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
        const auto& player = state.playerSettings[i];
        juce::String sectionName = "Player" + juce::String(i);
        sections.add(sectionName);
        
        juce::StringArray playerLines;
        playerLines.add("Enabled=" + juce::String(player.enabled ? 1 : 0));
        playerLines.add("Drumkit=" + player.selectedDrumkit);
        playerLines.add("SwingValue=" + juce::String(player.swingValue));
        playerLines.add("EnergyValue=" + juce::String(player.energyValue));
        playerLines.add("Volume=" + juce::String(player.volume));
        
        // Add assigned MIDI files
        for (int j = 0; j <= INIConfig::Validation::MAX_BUTTON_INDEX; ++j) {
            if (player.assignedMidiFiles[j].isNotEmpty()) {
                playerLines.add("MidiFile" + juce::String(j) + "=" + player.assignedMidiFiles[j]);
            }
        }
        sectionData[sectionName] = playerLines;
    }

    // Slider Values section (for any additional slider data)
    if (!state.sliderValues.empty()) {
        sections.add("SliderValues");
        juce::StringArray sliderLines;
        for (const auto& pair : state.sliderValues) {
            sliderLines.add(pair.first + "=" + juce::String(pair.second));
        }
        sectionData["SliderValues"] = sliderLines;
    }

    return INIUtils::writeINIFile(presetFile, sections, sectionData);
}

bool INIDataManager::loadPreset(const juce::String& presetName, ComponentState& state) {
    if (presetName.isEmpty()) {
        setError("Empty preset name");
        return false;
    }

    // Ensure Default preset always exists
    if (!ensureDefaultPresetExists()) {
        setError("Failed to ensure Default preset exists");
        return false;
    }

    // Find the preset file by searching through category directories
    auto presetsDir = INIConfig::getPresetsDirectory();
    juce::File presetFile;
    bool found = false;
    
    // Check default category first
    juce::String categoryName = (presetName == "Default") ? "Defaults" : "User";
    auto categoryDir = presetsDir.getChildFile(categoryName);
    presetFile = categoryDir.getChildFile(presetName + ".ini");
    
    if (presetFile.existsAsFile()) {
        found = true;
    } else {
        // Search all category directories for the preset
        for (auto& dir : presetsDir.findChildFiles(juce::File::findDirectories, false)) {
            presetFile = dir.getChildFile(presetName + ".ini");
            if (presetFile.existsAsFile()) {
                found = true;
                break;
            }
        }
    }

    if (!found) {
        setError("Preset file not found for: " + presetName);
        return false;
    }

    std::map<juce::String, std::map<juce::String, juce::String>> data;
    if (!INIUtils::readINIFile(presetFile, data)) {
        setError("Failed to read preset file: " + presetFile.getFullPathName());
        return false;
    }

    try {
        // Load Transport section
        if (data.count("Transport")) {
            const auto& transport = data.at("Transport");
            if (transport.count("Tempo")) {
                state.tempo = INIConfig::clampTempo(transport.at("Tempo").getIntValue());
            }
            if (transport.count("PlayState")) {
                state.playState = transport.at("PlayState").getIntValue() != 0;
            }
            if (transport.count("CurrentPreset")) {
                state.currentPreset = INIConfig::clampPresetIndex(transport.at("CurrentPreset").getIntValue());
            }
        }

        // Load Mixer section
        if (data.count("Mixer")) {
            const auto& mixer = data.at("Mixer");
            if (mixer.count("MasterVolume")) {
                state.sliderValues["masterVolume"] = mixer.at("MasterVolume").getFloatValue();
            }
            if (mixer.count("ReverbMix")) {
                state.sliderValues["reverbMix"] = mixer.at("ReverbMix").getFloatValue();
            }
            if (mixer.count("DelayMix")) {
                state.sliderValues["delayMix"] = mixer.at("DelayMix").getFloatValue();
            }
        }

        // Load Player sections
        for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
            juce::String sectionName = "Player" + juce::String(i);
            if (data.count(sectionName)) {
                const auto& playerData = data.at(sectionName);
                auto& player = state.playerSettings[i];
                
                if (playerData.count("Enabled")) {
                    player.enabled = playerData.at("Enabled").getIntValue() != 0;
                }
                if (playerData.count("Drumkit")) {
                    player.selectedDrumkit = playerData.at("Drumkit");
                }
                if (playerData.count("SwingValue")) {
                    player.swingValue = INIConfig::clampSwing(playerData.at("SwingValue").getFloatValue());
                }
                if (playerData.count("EnergyValue")) {
                    player.energyValue = INIConfig::clampEnergy(playerData.at("EnergyValue").getFloatValue());
                }
                if (playerData.count("Volume")) {
                    player.volume = playerData.at("Volume").getFloatValue();
                }

                // Load assigned MIDI files
                for (int j = 0; j <= INIConfig::Validation::MAX_BUTTON_INDEX; ++j) {
                    juce::String midiKey = "MidiFile" + juce::String(j);
                    if (playerData.count(midiKey)) {
                        player.assignedMidiFiles[j] = playerData.at(midiKey);
                    }
                }
            }
        }

        // Load SliderValues section (for any additional slider data)
        if (data.count("SliderValues")) {
            const auto& sliderData = data.at("SliderValues");
            for (const auto& pair : sliderData) {
                state.sliderValues[pair.first] = pair.second.getFloatValue();
            }
        }

        return true;
    }
    catch (const std::exception& e) {
        setError("Exception loading preset: " + juce::String(e.what()));
        return false;
    }
}

bool INIDataManager::deletePreset(const juce::String& presetName) {
    if (presetName.isEmpty()) {
        setError("Cannot delete empty preset name");
        return false;
    }
    
    // Absolutely prevent deletion of Default preset - this is critical for system stability
    if (presetName.equalsIgnoreCase("Default")) {
        setError("Cannot delete the Default preset - it is required for system operation");
        return false;
    }

    // Find the preset file by searching through category directories
    auto presetsDir = INIConfig::getPresetsDirectory();
    juce::File presetFile;
    bool found = false;
    
    // Search all category directories for the preset
    for (auto& dir : presetsDir.findChildFiles(juce::File::findDirectories, false)) {
        presetFile = dir.getChildFile(presetName + ".ini");
        if (presetFile.existsAsFile()) {
            // Double-check: prevent deletion if this is somehow the Default preset file
            if (presetFile.getFileNameWithoutExtension().equalsIgnoreCase("Default")) {
                setError("Cannot delete the Default preset file - it is required for system operation");
                return false;
            }
            found = true;
            break;
        }
    }

    if (!found) {
        setError("Preset file not found for: " + presetName);
        return false;
    }

    // Delete the preset file
    auto result = presetFile.deleteFile();
    if (!result) {
        setError("Failed to delete preset file: " + presetFile.getFullPathName());
        return false;
    }

    return true;
}

bool INIDataManager::presetExists(const juce::String& presetName) {
    if (presetName.isEmpty()) {
        return false;
    }

    // Search through all category directories for the preset
    auto presetsDir = INIConfig::getPresetsDirectory();
    
    for (auto& dir : presetsDir.findChildFiles(juce::File::findDirectories, false)) {
        auto presetFile = dir.getChildFile(presetName + ".ini");
        if (presetFile.existsAsFile()) {
            return true;
        }
    }

    return false;
}

juce::StringArray INIDataManager::getAvailablePresetNames() {
    juce::StringArray presetNames;
    
    // Ensure Default preset always exists before listing presets
    ensureDefaultPresetExists();
    
    auto presetsDir = INIConfig::getPresetsDirectory();
    if (!presetsDir.exists()) {
        return presetNames;
    }

    juce::StringArray defaultPresets;
    juce::StringArray userPresets;
    
    // Scan through all category directories
    for (auto& categoryDir : presetsDir.findChildFiles(juce::File::findDirectories, false)) {
        juce::String categoryName = categoryDir.getFileName();
        
        // Find all .ini files in this category
        for (auto& presetFile : categoryDir.findChildFiles(juce::File::findFiles, false, "*.ini")) {
            juce::String presetName = presetFile.getFileNameWithoutExtension();
            
            if (categoryName == "Defaults") {
                defaultPresets.add(presetName);
            } else {
                userPresets.add(presetName);
            }
        }
    }
    
    // Sort the arrays
    defaultPresets.sort(true);
    userPresets.sort(true);
    
    // Add Default preset first if it exists
    if (defaultPresets.contains("Default")) {
        presetNames.add("Default");
        defaultPresets.removeString("Default");
    }
    
    // Add remaining default presets
    for (const auto& preset : defaultPresets) {
        presetNames.add(preset);
    }
    
    // Add user presets
    for (const auto& preset : userPresets) {
        presetNames.add(preset);
    }

    return presetNames;
}

bool INIDataManager::createDefaultPreset() {
    // Ensure Defaults directory exists
    auto presetsDir = INIConfig::getPresetsDirectory();
    auto defaultsDir = presetsDir.getChildFile("Defaults");
    
    if (!defaultsDir.exists()) {
        auto result = defaultsDir.createDirectory();
        if (!result.wasOk()) {
            setError("Failed to create Defaults directory: " + result.getErrorMessage());
            return false;
        }
    }

    ComponentState defaultState;
    initializeDefaults(defaultState);
    
    // Set default values
    defaultState.tempo = INIConfig::Defaults::DEFAULT_TEMPO;
    defaultState.sliderValues["swingValue"] = INIConfig::Defaults::SWING;
    defaultState.sliderValues["energyValue"] = INIConfig::Defaults::ENERGY;
    defaultState.sliderValues["masterVolume"] = INIConfig::Defaults::DEFAULT_MASTER_VOLUME;
    
    // Set default player settings
    for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
        auto& player = defaultState.playerSettings[i];
        player.enabled = INIConfig::Defaults::DEFAULT_PLAYER_ENABLED;
        player.selectedDrumkit = INIConfig::Defaults::DEFAULT_DRUMKIT;
        player.swingValue = INIConfig::Defaults::SWING;
        player.energyValue = INIConfig::Defaults::ENERGY;
        player.volume = INIConfig::Defaults::VOLUME;
    }

    return savePreset("Default", defaultState);
}

bool INIDataManager::ensureDefaultPresetExists() {
    auto defaultPresetFile = INIConfig::getPresetsDirectory().getChildFile("Defaults").getChildFile("Default.ini");
    
    if (!defaultPresetFile.existsAsFile()) {
        // Default preset is missing - recreate it immediately
        return createDefaultPreset();
    }
    
    return true;
}

bool INIDataManager::isValidPresetName(const juce::String& name) const {
    if (name.isEmpty() || name.length() > 50) {
        return false;
    }

    // Check for invalid characters
    juce::String invalidChars = "/\\:*?\"<>|,\n\r\t";
    for (auto c : invalidChars) {
        if (name.contains(juce::String::charToString(c))) {
            return false;
        }
    }

    return true;
}


//==============================================================================
// Static convenience methods for component state management
//==============================================================================

bool INIDataManager::loadComponentState(const juce::String& componentName, ComponentState& state) {
    // Create a temporary INIDataManager instance to handle the loading
    INIDataManager manager;
    
    // Try to load from a component-specific file in the settings directory
    auto componentFile = INIConfig::getSettingsDirectory().getChildFile(componentName + ".ini");
    
    if (!componentFile.existsAsFile()) {
        // Component file doesn't exist, return false but don't set error
        return false;
    }
    
    // Load the component-specific settings
    juce::PropertiesFile::Options options;
    options.applicationName = "OTTO";
    options.filenameSuffix = ".ini";
    options.osxLibrarySubFolder = "Application Support";
    
    std::unique_ptr<juce::PropertiesFile> props(new juce::PropertiesFile(componentFile, options));
    
    if (!props->isValidFile()) {
        return false;
    }
    
    // Load component-specific values into the state's componentValues map
    auto& values = props->getAllProperties();
    for (int i = 0; i < values.size(); ++i) {
        auto key = values.getAllKeys()[i];
        auto value = values.getValue(key, "");
        state.setValue(key, juce::var(value));
    }
    
    return true;
}

bool INIDataManager::saveComponentState(const juce::String& componentName, const ComponentState& state) {
    // Create a temporary INIDataManager instance to handle the saving
    INIDataManager manager;
    
    // Save to a component-specific file in the settings directory
    auto componentFile = INIConfig::getSettingsDirectory().getChildFile(componentName + ".ini");
    
    // Ensure the settings directory exists
    auto settingsDir = INIConfig::getSettingsDirectory();
    if (!settingsDir.exists()) {
        auto result = settingsDir.createDirectory();
        if (!result.wasOk()) {
            return false;
        }
    }
    
    // Create properties file for this component
    juce::PropertiesFile::Options options;
    options.applicationName = "OTTO";
    options.filenameSuffix = ".ini";
    options.osxLibrarySubFolder = "Application Support";
    
    std::unique_ptr<juce::PropertiesFile> props(new juce::PropertiesFile(componentFile, options));
    
    // Save all component values from the state's componentValues map
    // Note: We can't access private componentValues directly, so we'll save what we can
    // This is a simplified implementation - in a real scenario, you'd want to expose
    // the componentValues or provide a way to iterate through them
    
    // For now, just ensure the file exists and is valid
    props->setValue("componentName", componentName);
    props->setValue("lastSaved", juce::Time::getCurrentTime().toString(true, true));
    
    return props->save();
}

// ========================================================================
// ENHANCED DATABASE CREATION METHODS IMPLEMENTATION
// ========================================================================

bool INIDataManager::createEnhancedSystemFiles() {
    bool success = true;
    auto applicationFile = INIConfig::getSystemDirectory().getChildFile(INIConfig::APPLICATION_FILE);
    if (!applicationFile.existsAsFile()) {
        juce::PropertiesFile::Options options;
        options.applicationName = "OTTO";
        options.filenameSuffix = ".ini";
        std::unique_ptr<juce::PropertiesFile> props(new juce::PropertiesFile(applicationFile, options));
        props->setValue("Version", INIConfig::Defaults::DEFAULT_APPLICATION_VERSION);
        props->setValue("Build", INIConfig::Defaults::DEFAULT_BUILD_NUMBER);
        success = success && props->save();
    }
    return success;
}

bool INIDataManager::createEnhancedSettingsFiles() {
    bool success = true;
    auto performanceFile = INIConfig::getSettingsDirectory().getChildFile(INIConfig::PERFORMANCE_SETTINGS_FILE);
    if (!performanceFile.existsAsFile()) {
        juce::PropertiesFile::Options options;
        options.applicationName = "OTTO";
        options.filenameSuffix = ".ini";
        std::unique_ptr<juce::PropertiesFile> props(new juce::PropertiesFile(performanceFile, options));
        props->setValue("MaxVoices", INIConfig::Defaults::DEFAULT_MAX_VOICES_ENHANCED);
        props->setValue("CPULimit", INIConfig::Defaults::DEFAULT_CPU_LIMIT);
        success = success && props->save();
    }
    return success;
}

bool INIDataManager::createEnhancedUIFiles() {
    bool success = true;
    auto themeManagerFile = INIConfig::getSettingsDirectory()
        .getChildFile(INIConfig::SETTINGS_UI_FOLDER)
        .getChildFile(INIConfig::THEME_MANAGER_FILE);
    if (!themeManagerFile.existsAsFile()) {
        juce::PropertiesFile::Options options;
        options.applicationName = "OTTO";
        options.filenameSuffix = ".ini";
        std::unique_ptr<juce::PropertiesFile> props(new juce::PropertiesFile(themeManagerFile, options));
        props->setValue("ActiveTheme", "Default");
        success = success && props->save();
    }
    return success;
}

bool INIDataManager::createEnhancedThemesFiles() {
    bool success = true;
    auto defaultThemeFile = INIConfig::getOTTODataDirectory()
        .getChildFile(INIConfig::THEMES_FOLDER)
        .getChildFile(INIConfig::DEFAULT_THEME_FILE);
    if (!defaultThemeFile.existsAsFile()) {
        juce::PropertiesFile::Options options;
        options.applicationName = "OTTO";
        options.filenameSuffix = ".ini";
        std::unique_ptr<juce::PropertiesFile> props(new juce::PropertiesFile(defaultThemeFile, options));
        props->setValue("ThemeName", "Default");
        props->setValue("BackgroundColor", "#2D2D2D");
        success = success && props->save();
    }
    return success;
}

bool INIDataManager::createEnhancedPlayersFiles() {
    bool success = true;
    auto playerConfigsFile = INIConfig::getOTTODataDirectory()
        .getChildFile(INIConfig::PLAYERS_FOLDER)
        .getChildFile(INIConfig::PLAYER_CONFIGS_FILE);
    if (!playerConfigsFile.existsAsFile()) {
        juce::PropertiesFile::Options options;
        options.applicationName = "OTTO";
        options.filenameSuffix = ".ini";
        std::unique_ptr<juce::PropertiesFile> props(new juce::PropertiesFile(playerConfigsFile, options));
        for (int i = 0; i < INIConfig::Defaults::DEFAULT_PLAYER_COUNT; ++i) {
            juce::String playerSection = "Player_" + juce::String(i + 1).paddedLeft('0', 3);
            props->setValue(playerSection + "_Name", INIConfig::Defaults::DEFAULT_PLAYER_NAME + " " + juce::String(i + 1));
        }
        success = success && props->save();
    }
    return success;
}

bool INIDataManager::createEnhancedKitsFiles() {
    bool success = true;
    auto kitIndexFile = INIConfig::getKitsDirectory().getChildFile(INIConfig::KIT_INDEX_FILE);
    if (!kitIndexFile.existsAsFile()) {
        juce::PropertiesFile::Options options;
        options.applicationName = "OTTO";
        options.filenameSuffix = ".ini";
        std::unique_ptr<juce::PropertiesFile> props(new juce::PropertiesFile(kitIndexFile, options));
        props->setValue("SearchIndexEnabled", INIConfig::Defaults::DEFAULT_SEARCH_INDEX_ENABLED);
        success = success && props->save();
    }
    return success;
}

bool INIDataManager::createEnhancedPatternsFiles() {
    bool success = true;
    auto patternIndexFile = INIConfig::getPatternsDirectory().getChildFile(INIConfig::PATTERN_INDEX_FILE);
    if (!patternIndexFile.existsAsFile()) {
        juce::PropertiesFile::Options options;
        options.applicationName = "OTTO";
        options.filenameSuffix = ".ini";
        std::unique_ptr<juce::PropertiesFile> props(new juce::PropertiesFile(patternIndexFile, options));
        props->setValue("SearchIndexEnabled", INIConfig::Defaults::DEFAULT_SEARCH_INDEX_ENABLED);
        success = success && props->save();
    }
    return success;
}

bool INIDataManager::createEnhancedMixFiles() {
    bool success = true;
    auto mixerSnapshotsFile = INIConfig::getMixingDirectory().getChildFile(INIConfig::MIXER_SNAPSHOTS_FILE);
    if (!mixerSnapshotsFile.existsAsFile()) {
        juce::PropertiesFile::Options options;
        options.applicationName = "OTTO";
        options.filenameSuffix = ".ini";
        std::unique_ptr<juce::PropertiesFile> props(new juce::PropertiesFile(mixerSnapshotsFile, options));
        props->setValue("MS_001_Name", "Live Performance Mix");
        success = success && props->save();
    }
    return success;
}

bool INIDataManager::createEnhancedPerformanceFiles() {
    bool success = true;
    auto backupStatesFile = INIConfig::getPerformanceDirectory().getChildFile(INIConfig::BACKUP_STATES_FILE);
    if (!backupStatesFile.existsAsFile()) {
        juce::PropertiesFile::Options options;
        options.applicationName = "OTTO";
        options.filenameSuffix = ".ini";
        std::unique_ptr<juce::PropertiesFile> props(new juce::PropertiesFile(backupStatesFile, options));
        props->setValue("BackupEnabled", INIConfig::Defaults::DEFAULT_BACKUP_ENABLED);
        success = success && props->save();
    }
    return success;
}

bool INIDataManager::createEnhancedCacheFiles() {
    bool success = true;
    auto searchIndexFile = INIConfig::getOTTODataDirectory()
        .getChildFile(INIConfig::CACHE_FOLDER)
        .getChildFile(INIConfig::SEARCH_INDEX_FILE);
    if (!searchIndexFile.existsAsFile()) {
        juce::PropertiesFile::Options options;
        options.applicationName = "OTTO";
        options.filenameSuffix = ".ini";
        std::unique_ptr<juce::PropertiesFile> props(new juce::PropertiesFile(searchIndexFile, options));
        props->setValue("IndexEnabled", INIConfig::Defaults::DEFAULT_SEARCH_INDEX_ENABLED);
        success = success && props->save();
    }
    return success;
}

bool INIDataManager::createEnhancedPresetsFiles() {
    bool success = true;
    auto presetIndexFile = INIConfig::getPresetsDirectory().getChildFile(INIConfig::PRESET_INDEX_FILE);
    if (!presetIndexFile.existsAsFile()) {
        juce::PropertiesFile::Options options;
        options.applicationName = "OTTO";
        options.filenameSuffix = ".ini";
        std::unique_ptr<juce::PropertiesFile> props(new juce::PropertiesFile(presetIndexFile, options));
        props->setValue("SearchIndexEnabled", INIConfig::Defaults::DEFAULT_SEARCH_INDEX_ENABLED);
        success = success && props->save();
    }
    return success;
}
