#pragma once
#include <JuceHeader.h>
#include "../Source/StateManager.h"
#include "../Source/INIDataManager.h"
#include "../Source/ComponentState.h"
#include "../Source/INIConfig.h"

class StatePersistenceTests : public juce::UnitTest {
public:
    StatePersistenceTests() : juce::UnitTest("State Persistence Tests") {}

    void runTest() override {
        beginTest("INI File Creation");
        testINIFileCreation();

        beginTest("State Save/Load");
        testStateSaveLoad();

        beginTest("Player State Persistence");
        testPlayerStatePersistence();

        beginTest("Global Settings Persistence");
        testGlobalSettingsPersistence();

        beginTest("Theme Settings Persistence");
        testThemeSettingsPersistence();

        beginTest("MIDI Mappings Persistence");
        testMidiMappingsPersistence();

        beginTest("Pattern Groups Persistence");
        testPatternGroupsPersistence();

        beginTest("Preset Management");
        testPresetManagement();

        beginTest("File Validation");
        testFileValidation();

        beginTest("Backup and Restore");
        testBackupRestore();
    }

private:
    void testINIFileCreation() {
        auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                        .getChildFile("OTTOTest_" + juce::String(juce::Random::getSystemRandom().nextInt()));
        tempDir.createDirectory();

        auto originalDataDir = INIConfig::getOTTODataDirectory();

        {
            INIDataManager iniManager;

            expect(iniManager.createDirectoryStructure(), "Should create directory structure");
            expect(iniManager.directoryStructureExists(), "Directory structure should exist");

            expect(iniManager.createAllRequiredFiles(), "Should create all required files");
            expect(iniManager.allFilesExist(), "All files should exist");

            expect(INIConfig::getGlobalSettingsFile().existsAsFile(), "Global settings file should exist");
            expect(INIConfig::getThemesFile().existsAsFile(), "Themes file should exist");
            expect(INIConfig::getAudioSettingsFile().existsAsFile(), "Audio settings file should exist");
            expect(INIConfig::getPlayersFile().existsAsFile(), "Players file should exist");
            expect(INIConfig::getPatternGroupsFile().existsAsFile(), "Pattern groups file should exist");
        }

        tempDir.deleteRecursively();
    }

    void testStateSaveLoad() {
        StateManager stateManager;
        ComponentState originalState;

        originalState.currentPlayer = 3;
        originalState.tempo = 135;
        originalState.playState = true;
        originalState.editMode = true;
        originalState.interfaceScale = 1.5f;
        originalState.interfaceWidth = 1400;
        originalState.interfaceHeight = 900;

        originalState.sliderValues["testSlider"] = 0.75f;
        originalState.toggleStates[42] = true;
        originalState.dropdownSelections["testDropdown"] = 5;

        stateManager.saveStates(originalState);
        stateManager.saveAllToINI();

        StateManager loadedStateManager;
        ComponentState loadedState;

        loadedStateManager.loadAllFromINI();
        loadedStateManager.loadStates(loadedState);

        expectEquals(loadedState.currentPlayer, originalState.currentPlayer, "Current player should match");
        expectEquals(loadedState.tempo, originalState.tempo, "Tempo should match");
        expect(loadedState.playState == originalState.playState, "Play state should match");
        expect(loadedState.editMode == originalState.editMode, "Edit mode should match");
        expectWithinAbsoluteError(loadedState.interfaceScale, originalState.interfaceScale, 0.01f,
            "Interface scale should match");
        expectEquals(loadedState.interfaceWidth, originalState.interfaceWidth, "Interface width should match");
        expectEquals(loadedState.interfaceHeight, originalState.interfaceHeight, "Interface height should match");

        expect(loadedState.sliderValues.count("testSlider") > 0, "Slider value should exist");
        if (loadedState.sliderValues.count("testSlider") > 0) {
            expectWithinAbsoluteError(loadedState.sliderValues.at("testSlider"), 0.75f, 0.01f,
                "Slider value should match");
        }

        expect(loadedState.toggleStates.count(42) > 0, "Toggle state should exist");
        if (loadedState.toggleStates.count(42) > 0) {
            expect(loadedState.toggleStates.at(42) == true, "Toggle state should match");
        }
    }

    void testPlayerStatePersistence() {
        StateManager stateManager;
        ComponentState state;

        for (int i = 0; i < 8; ++i) {
            auto& player = state.playerSettings[i];

            player.enabled = (i % 2 == 0);
            player.selectedDrumkit = "TestKit" + juce::String(i);
            player.volume = 0.1f + (i * 0.1f);
            player.pan = -0.5f + (i * 0.25f);
            player.muted = (i == 2);
            player.soloed = (i == 4);
            player.midiChannel = i + 1;
            player.outputChannel = i + 1;
            player.selectedMidiGroup = "Group" + juce::String(i);
            player.selectedButton = i;
            player.swingValue = 40.0f + (i * 5.0f);
            player.energyValue = 30.0f + (i * 10.0f);

            for (int j = 0; j < 16; ++j) {
                player.assignedMidiFiles[j] = "Player" + juce::String(i) + "_File" + juce::String(j);
            }

            for (int j = 0; j < 5; ++j) {
                player.toggleStates[j] = ((i + j) % 2 == 0);
                player.fillStates[j] = ((i + j) % 3 == 0);
            }
        }

        stateManager.saveStates(state);
        stateManager.saveAllToINI();

        StateManager loadedManager;
        ComponentState loadedState;
        loadedManager.loadAllFromINI();
        loadedManager.loadStates(loadedState);

        for (int i = 0; i < 8; ++i) {
            const auto& original = state.playerSettings[i];
            const auto& loaded = loadedState.playerSettings[i];

            expect(loaded.enabled == original.enabled,
                "Player " + juce::String(i) + " enabled state should match");
            expectEquals(loaded.selectedDrumkit, original.selectedDrumkit,
                "Player " + juce::String(i) + " drumkit should match");
            expectWithinAbsoluteError(loaded.volume, original.volume, 0.01f,
                "Player " + juce::String(i) + " volume should match");
            expectWithinAbsoluteError(loaded.pan, original.pan, 0.01f,
                "Player " + juce::String(i) + " pan should match");
            expect(loaded.muted == original.muted,
                "Player " + juce::String(i) + " mute state should match");
            expect(loaded.soloed == original.soloed,
                "Player " + juce::String(i) + " solo state should match");
            expectEquals(loaded.midiChannel, original.midiChannel,
                "Player " + juce::String(i) + " MIDI channel should match");

            for (int j = 0; j < 16; ++j) {
                expectEquals(loaded.assignedMidiFiles[j], original.assignedMidiFiles[j],
                    "Player " + juce::String(i) + " file " + juce::String(j) + " should match");
            }

            for (int j = 0; j < 5; ++j) {
                expect(loaded.toggleStates[j] == original.toggleStates[j],
                    "Player " + juce::String(i) + " toggle " + juce::String(j) + " should match");
                expect(loaded.fillStates[j] == original.fillStates[j],
                    "Player " + juce::String(i) + " fill " + juce::String(j) + " should match");
            }
        }
    }

    void testGlobalSettingsPersistence() {
        INIDataManager iniManager;
        GlobalSettings original;

        original.settingsID = 42;
        original.settingsName = "Test Settings";
        original.tempo = 145;
        original.linkSyncMode = "Ableton Link";
        original.linkSyncValue = 4;
        original.currentCloudConnection = "Dropbox";
        original.interfaceScale = 1.25f;
        original.interfaceStartingX = 200;
        original.interfaceStartingY = 150;
        original.interfaceWidth = 1600;
        original.interfaceHeight = 1000;
        original.metronomeEnabled = true;
        original.metronomeVolume = 0.7f;
        original.metronomeSound = "Click";
        original.quantizeValue = 16;
        original.countInBars = 2;
        original.midiClockOut = true;
        original.midiClockIn = false;
        original.autoSaveInterval = 300;

        expect(iniManager.saveGlobalSettings(original), "Should save global settings");

        GlobalSettings loaded;
        expect(iniManager.loadGlobalSettings(loaded), "Should load global settings");

        expectEquals(loaded.settingsID, original.settingsID, "Settings ID should match");
        expectEquals(loaded.settingsName, original.settingsName, "Settings name should match");
        expectEquals(loaded.tempo, original.tempo, "Tempo should match");
        expectEquals(loaded.linkSyncMode, original.linkSyncMode, "Link sync mode should match");
        expectEquals(loaded.linkSyncValue, original.linkSyncValue, "Link sync value should match");
        expectEquals(loaded.currentCloudConnection, original.currentCloudConnection,
            "Cloud connection should match");
        expectWithinAbsoluteError(loaded.interfaceScale, original.interfaceScale, 0.01f,
            "Interface scale should match");
        expectEquals(loaded.interfaceStartingX, original.interfaceStartingX, "Interface X should match");
        expectEquals(loaded.interfaceStartingY, original.interfaceStartingY, "Interface Y should match");
        expectEquals(loaded.interfaceWidth, original.interfaceWidth, "Interface width should match");
        expectEquals(loaded.interfaceHeight, original.interfaceHeight, "Interface height should match");
        expect(loaded.metronomeEnabled == original.metronomeEnabled, "Metronome enabled should match");
        expectWithinAbsoluteError(loaded.metronomeVolume, original.metronomeVolume, 0.01f,
            "Metronome volume should match");
        expectEquals(loaded.metronomeSound, original.metronomeSound, "Metronome sound should match");
        expectEquals(loaded.quantizeValue, original.quantizeValue, "Quantize value should match");
        expectEquals(loaded.countInBars, original.countInBars, "Count-in bars should match");
        expect(loaded.midiClockOut == original.midiClockOut, "MIDI clock out should match");
        expect(loaded.midiClockIn == original.midiClockIn, "MIDI clock in should match");
        expectEquals(loaded.autoSaveInterval, original.autoSaveInterval, "Auto-save interval should match");
    }

    void testThemeSettingsPersistence() {
        INIDataManager iniManager;
        juce::Array<ThemeSettings> themes;

        ThemeSettings darkTheme;
        darkTheme.setDefaults();
        darkTheme.themeName = "Custom Dark";
        darkTheme.backgroundColor = "#1A1A1A";
        darkTheme.accentColor = "#FF5500";
        themes.add(darkTheme);

        ThemeSettings lightTheme;
        lightTheme.setLightThemeDefaults();
        lightTheme.themeName = "Custom Light";
        lightTheme.buttonHoverColor = "#E0E0FF";
        themes.add(lightTheme);

        ThemeSettings neonTheme;
        neonTheme.themeID = 10;
        neonTheme.themeName = "Neon";
        neonTheme.backgroundColor = "#000000";
        neonTheme.foregroundColor = "#00FF00";
        neonTheme.accentColor = "#FF00FF";
        neonTheme.textColor = "#00FFFF";
        themes.add(neonTheme);

        expect(iniManager.saveAllThemes(themes), "Should save all themes");

        juce::Array<ThemeSettings> loadedThemes;
        expect(iniManager.loadAllThemes(loadedThemes), "Should load all themes");

        expectEquals(loadedThemes.size(), themes.size(), "Number of themes should match");

        for (int i = 0; i < themes.size(); ++i) {
            const auto& original = themes[i];

            ThemeSettings* loaded = nullptr;
            for (auto& theme : loadedThemes) {
                if (theme.themeID == original.themeID) {
                    loaded = &theme;
                    break;
                }
            }

            expect(loaded != nullptr, "Theme with ID " + juce::String(original.themeID) + " should exist");

            if (loaded) {
                expectEquals(loaded->themeName, original.themeName, "Theme name should match");
                expectEquals(loaded->backgroundColor, original.backgroundColor,
                    "Background color should match");
                expectEquals(loaded->foregroundColor, original.foregroundColor,
                    "Foreground color should match");
                expectEquals(loaded->accentColor, original.accentColor, "Accent color should match");
                expectEquals(loaded->textColor, original.textColor, "Text color should match");
                expectEquals(loaded->buttonHoverColor, original.buttonHoverColor,
                    "Button hover color should match");
            }
        }
    }

    void testMidiMappingsPersistence() {
        INIDataManager iniManager;
        juce::Array<MidiEngine::MidiMapping> mappings;

        MidiEngine::MidiMapping mapping1;
        mapping1.ccNumber = 7;
        mapping1.channel = 1;
        mapping1.parameterID = "masterVolume";
        mapping1.minValue = 0.0f;
        mapping1.maxValue = 1.0f;
        mapping1.enabled = true;
        mapping1.sendFeedback = true;
        mapping1.feedbackType = 1;
        mappings.add(mapping1);

        MidiEngine::MidiMapping mapping2;
        mapping2.ccNumber = 10;
        mapping2.channel = 0;
        mapping2.parameterID = "tempo";
        mapping2.minValue = INIConfig::Defaults::MIN_TEMPO;
        mapping2.maxValue = INIConfig::Defaults::MAX_TEMPO;
        mapping2.enabled = false;
        mapping2.sendFeedback = false;
        mapping2.feedbackType = 0;
        mappings.add(mapping2);

        expect(iniManager.saveMidiMappings(mappings), "Should save MIDI mappings");

        juce::Array<MidiEngine::MidiMapping> loadedMappings;
        expect(iniManager.loadMidiMappings(loadedMappings), "Should load MIDI mappings");

        expectEquals(loadedMappings.size(), mappings.size(), "Number of mappings should match");

        for (int i = 0; i < mappings.size(); ++i) {
            const auto& original = mappings[i];
            const auto& loaded = loadedMappings[i];

            expectEquals(loaded.ccNumber, original.ccNumber, "CC number should match");
            expectEquals(loaded.channel, original.channel, "Channel should match");
            expectEquals(loaded.parameterID, original.parameterID, "Parameter ID should match");
            expectWithinAbsoluteError(loaded.minValue, original.minValue, 0.01f, "Min value should match");
            expectWithinAbsoluteError(loaded.maxValue, original.maxValue, 0.01f, "Max value should match");
            expect(loaded.enabled == original.enabled, "Enabled state should match");
            expect(loaded.sendFeedback == original.sendFeedback, "Send feedback should match");
            expectEquals(loaded.feedbackType, original.feedbackType, "Feedback type should match");
        }
    }

    void testPatternGroupsPersistence() {
        INIDataManager iniManager;
        juce::Array<BeatsButtonGroup> groups;

        for (int i = 0; i < 5; ++i) {
            BeatsButtonGroup group("TestGroup" + juce::String(i));
            group.isCustomGroup = (i >= 3);
            group.selectedButton = i * 2;
            group.isFavorite = (i % 2 == 0);

            for (int j = 0; j < 8; ++j) {
                group.midiFiles.add("Group" + juce::String(i) + "_Pattern" + juce::String(j) + ".mid");
            }

            group.averageSwing = 50.0f + (i * 5.0f);
            group.averageVelocity = 64.0f + (i * 8.0f);
            group.timeSignatureNumerator = 4;
            group.timeSignatureDenominator = 4;
            group.grooveTightness = 0.8f + (i * 0.05f);

            groups.add(group);
        }

        expect(iniManager.savePatternGroups(groups), "Should save pattern groups");

        juce::Array<BeatsButtonGroup> loadedGroups;
        expect(iniManager.loadPatternGroups(loadedGroups), "Should load pattern groups");

        expectEquals(loadedGroups.size(), groups.size(), "Number of groups should match");

        for (int i = 0; i < groups.size(); ++i) {
            const auto& original = groups[i];

            BeatsButtonGroup* loaded = nullptr;
            for (auto& group : loadedGroups) {
                if (group.groupName == original.groupName) {
                    loaded = &group;
                    break;
                }
            }

            expect(loaded != nullptr, "Group " + original.groupName + " should exist");

            if (loaded) {
                expect(loaded->isCustomGroup == original.isCustomGroup, "Custom group flag should match");
                expectEquals(loaded->selectedButton, original.selectedButton, "Selected button should match");
                expect(loaded->isFavorite == original.isFavorite, "Favorite flag should match");

                expectEquals(loaded->midiFiles.size(), original.midiFiles.size(),
                    "Number of MIDI files should match");

                for (int j = 0; j < original.midiFiles.size(); ++j) {
                    expectEquals(loaded->midiFiles[j], original.midiFiles[j],
                        "MIDI file " + juce::String(j) + " should match");
                }
            }
        }
    }

    void testPresetManagement() {
        StateManager stateManager;

        for (int i = 0; i < 5; ++i) {
            ComponentState presetState;
            presetState.currentPreset = i;
            presetState.tempo = 100 + (i * 10);
            presetState.globalSettings.tempo = presetState.tempo;

            for (int j = 0; j < 8; ++j) {
                presetState.playerSettings[j].volume = 0.5f + (i * 0.1f);
                presetState.playerSettings[j].enabled = (j <= i);
            }

            stateManager.loadStates(presetState);
            stateManager.setCurrentPreset(i);
            stateManager.saveAllToINI();
        }

        for (int i = 0; i < 5; ++i) {
            StateManager loadManager;
            loadManager.setCurrentPreset(i);
            loadManager.loadAllFromINI();

            const auto& state = loadManager.getState();

            expectEquals(state.currentPreset, i, "Preset index should match");
            expectEquals(state.tempo, 100 + (i * 10), "Preset tempo should match");

            for (int j = 0; j < 8; ++j) {
                expectWithinAbsoluteError(state.playerSettings[j].volume, 0.5f + (i * 0.1f), 0.01f,
                    "Player volume in preset should match");
                expect(state.playerSettings[j].enabled == (j <= i),
                    "Player enabled state in preset should match");
            }
        }
    }

    void testFileValidation() {
        INIDataManager iniManager;

        auto report = iniManager.getValidationReport();
        expect(report.size() > 0, "Validation report should contain entries");

        for (const auto& line : report) {
            logMessage(line);
        }

        auto tempFile = juce::File::getSpecialLocation(juce::File::tempDirectory)
                        .getChildFile("corrupt_test.ini");

        tempFile.replaceWithText("This is not valid INI format!!!\n#$%^&*()");

        expect(!INIUtils::validateINIFile(tempFile), "Should detect corrupted file");

        juce::StringArray validContent;
        validContent.add("[section]");
        validContent.add("key=value");
        validContent.add("number=123");

        tempFile.replaceWithText(validContent.joinIntoString("\n"));

        expect(INIUtils::validateINIFile(tempFile), "Should validate correct file");

        tempFile.deleteFile();
    }

    void testBackupRestore() {
        StateManager stateManager;
        INIDataManager& iniManager = stateManager.getINIManager();

        ComponentState originalState;
        originalState.tempo = 155;
        originalState.currentPlayer = 5;
        originalState.editMode = true;

        stateManager.loadStates(originalState);
        stateManager.saveAllToINI();

        expect(iniManager.createBackups(), "Should create backups");

        ComponentState modifiedState;
        modifiedState.tempo = 90;
        modifiedState.currentPlayer = 2;
        modifiedState.editMode = false;

        stateManager.loadStates(modifiedState);
        stateManager.saveAllToINI();

        auto dataDir = INIConfig::getOTTODataDirectory();
        juce::Array<juce::File> backupFiles;
        dataDir.findChildFiles(backupFiles, juce::File::findFiles, true, "*backup*.ini");

        expect(backupFiles.size() > 0, "Should have created backup files");

        for (const auto& backupFile : backupFiles) {
            if (backupFile.getFileName().contains("Global")) {
                std::map<juce::String, std::map<juce::String, juce::String>> backupData;
                if (INIUtils::readINIFile(backupFile, backupData)) {
                    if (backupData.count("settings") && backupData["settings"].count("tempo")) {
                        int backupTempo = backupData["settings"]["tempo"].getIntValue();
                        expectEquals(backupTempo, 155, "Backup should contain original tempo");
                    }
                }
            }
        }

        for (auto& backup : backupFiles) {
            backup.deleteFile();
        }
    }
};

static StatePersistenceTests statePersistenceTests;
