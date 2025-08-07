# OTTO INI Rules & Data Management System

This document outlines OTTO's comprehensive INI-based data management system for configuration, presets, and all application settings.

## 🗂️ INI System Overview

OTTO uses a **Reaper-style INI plain text database** for all data storage and configuration:

- **Human-Readable Format**: All settings stored in plain text INI files
- **Version Control Friendly**: Easy to track changes and merge conflicts
- **Cross-Platform Compatibility**: Identical format across all 5 platforms
- **Centralized Configuration**: All constants defined in `INIConfig.h`
- **No Hardcoded Values**: Every layout and setting value controlled via INI system

## 📁 Data Organization Structure

### Primary Data Directory
```
OTTO_Data/
├── Settings/          # Global application settings
├── Presets/           # User-created plugin presets
├── Performance/       # Performance and optimization settings
├── Patterns/          # MIDI pattern configurations
├── Kits/              # Drum kit configurations
├── Mix/               # Mixer and effects presets
├── System/            # System-level configuration
└── Backup/            # Automated backup files
```

### INI File Categories

#### System Configuration Files
- `Global.ini` - Global application settings
- `Themes.ini` - UI theme and color scheme settings
- `AudioSettings.ini` - Audio device and processing settings
- `MidiDevices.ini` - MIDI device configuration

#### User Content Files
- `Presets.ini` - Plugin preset definitions
- `Players.ini` - Individual player configurations (8 players)
- `PatternChains.ini` - Pattern chain sequences
- `PatternGroups.ini` - Pattern group organization
- `DrumKits.ini` - Drum kit assignments and settings

#### Advanced Configuration Files
- `MidiInLayout.ini` - MIDI input routing configuration
- `MidiOutLayout.ini` - MIDI output routing configuration
- `ChannelPresetGroups.ini` - Audio channel groupings
- `ChannelPresets.ini` - Audio channel settings
- `EQPresets.ini` - Equalizer presets
- `MasterChannelPresets.ini` - Master channel settings

#### Cache and Analysis Files
- `FileIndex.ini` - File system index cache
- `MidiAnalysisCache.ini` - MIDI pattern analysis cache
- `ControllerPresets.ini` - MIDI controller presets
- `MidiMappings.ini` - MIDI CC mapping assignments
- `GrooveTemplates.ini` - Groove template definitions

## 🔧 INIConfig.h - Central Configuration

### Layout Constants (CRITICAL)
**All UI positioning MUST use these constants - NO HARDCODED VALUES:**

```cpp
namespace INIConfig::LayoutConstants {
    // Interface Base Dimensions
    constexpr int defaultInterfaceWidth = 1200;
    constexpr int defaultInterfaceHeight = 800;
    constexpr float defaultScaleFactor = 1.0f;

    // Row Height Percentages (must total 100%)
    constexpr float row1HeightPercent = 8.0f;   // Top Bar
    constexpr float row2HeightPercent = 10.0f;  // Player Tabs
    constexpr float row3HeightPercent = 15.0f;  // Drum Kit Section
    constexpr float row4HeightPercent = 20.0f;  // Scene Launcher
    constexpr float row5HeightPercent = 32.0f;  // Pattern Matrix
    constexpr float row6HeightPercent = 15.0f;  // Loop Section

    // Component Sizing
    constexpr int defaultMargin = 10;
    constexpr int defaultSpacing = 5;
    constexpr int defaultButtonHeight = 30;
    constexpr int defaultSliderHeight = 30;
    constexpr int defaultCornerRadius = 4;

    // Font Sizes
    constexpr float fontSizeSmall = 12.0f;
    constexpr float fontSizeNormal = 14.0f;
    constexpr float fontSizeTitle = 16.0f;
    constexpr float fontSizeLarge = 18.0f;

    // Player Button Layout (Special Case)
    constexpr float playerButtonMarginPercent = 2.5f;    // 1/4 width
    constexpr float playerButtonStartMarginPercent = 5.0f; // 1/2 width for first button
    constexpr float playerButtonEndMarginPercent = 5.0f;   // 1/2 width for last button

    // Row-Specific Layout Constants
    namespace Row1 { /* Top Bar constants */ }
    namespace Row2 { /* Player Tabs constants */ }
    namespace Row3 { /* Drum Kit Section constants */ }
    namespace Row4 { /* Scene Launcher constants */ }
    namespace Row5 { /* Pattern Matrix constants */
        constexpr float leftSectionWidthPercent = 63.5f;
        constexpr float rightSectionWidthPercent = 35.0f;
        constexpr float sectionMarginPercent = 0.3f;
    }
    namespace Row6 { /* Loop Section constants */
        constexpr float labelWidthPercent = 8.5f;
        constexpr int sliderHeight = 30;
    }
}
```

### Default Values System
```cpp
namespace INIConfig::Defaults {
    // Audio Settings
    static const int DEFAULT_SAMPLE_RATE = 48000;
    static const int DEFAULT_BUFFER_SIZE = 256;
    static const int DEFAULT_INPUT_CHANNELS = 2;
    static const int DEFAULT_OUTPUT_CHANNELS = 2;
    static const int DEFAULT_BIT_DEPTH = 24;

    // Player Settings
    static const bool DEFAULT_PLAYER_ENABLED = true;
    static const float DEFAULT_SWING = 10.0f;
    static const float DEFAULT_ENERGY = 75.0f;
    static const float DEFAULT_VOLUME = 0.8f;
    static const int DEFAULT_CURRENT_PLAYER = 0;

    // Global Settings
    static const int DEFAULT_TEMPO = 120;
    static const juce::String DEFAULT_TIME_SIGNATURE = "4/4";
    static const bool DEFAULT_METRONOME_ENABLED = false;
    static const float DEFAULT_METRONOME_VOLUME = 0.5f;

    // UI Settings
    static const int DEFAULT_THEME_ID = 1;
    static const bool DEFAULT_EDIT_MODE = false;
    static const int DEFAULT_SELECTED_BUTTON = 0;

    // Effects Settings
    static const float DEFAULT_REVERB_MIX = 0.2f;
    static const float DEFAULT_DELAY_MIX = 0.1f;
    static const float DEFAULT_MASTER_VOLUME = 0.8f;

    // Performance Settings
    static const int DEFAULT_MAX_VOICES = 64;
    static const int DEFAULT_MAX_VOICES_PER_NOTE = 4;
    static const bool DEFAULT_ENABLE_MULTICORE = true;
    static const bool DEFAULT_LATENCY_COMPENSATION = false;
}
```

## 💾 INI File Format Standards

### File Header Standard
```ini
[OTTO_FILE_INFO]
Version=2.0
Created=2024-01-15T10:30:00Z
LastModified=2024-01-15T10:30:00Z
Application=OTTO
Platform=macOS
```

### Section Organization
```ini
# Global.ini example
[AUDIO_SETTINGS]
SampleRate=48000
BufferSize=256
InputChannels=2
OutputChannels=2
AudioDevice=Default
EnableASIO=false
EnableMulticore=true

[UI_SETTINGS]
WindowWidth=1200
WindowHeight=800
ScaleFactor=1.0
ThemeID=1
EditMode=false

[MIDI_SETTINGS]
MidiClockOut=false
MidiClockIn=false
QuantizeValue=16
SwingAmount=10.0
HumanizeAmount=0.0
```

### Player Configuration Format
```ini
# Players.ini example
[PLAYER_1]
Enabled=true
DrumKit=Acoustic
Energy=75.0
Swing=10.0
Volume=0.8
Transpose=0
VelocityShift=0
TimingShift=0
RecordMode=Replace
SyncType=Host
QuantizeAmount=100
PrimaryQuantize=SixteenthNote
SecondaryQuantize=None

[PLAYER_1_TRANSPORT]
Transport=Stop
RecordMIDIChannel=1
Monitor=false
Point1=0.0
Point2=0.0
Point3=0.0
Point4=0.0

[PLAYER_1_EFFECTS]
ReverbMix=0.2
DelayMix=0.1
CompressorThreshold=-12.0
CompressorRatio=4.0
LimiterThreshold=-3.0
```

## 🔄 Data Access Patterns

### Reading INI Data
```cpp
// INIDataManager handles all INI operations
class INIDataManager {
public:
    // Load global settings
    static GlobalSettings loadGlobalSettings() {
        auto file = getINIFile(INIConfig::GLOBAL_SETTINGS_FILE);
        GlobalSettings settings;

        settings.sampleRate = file.getValue("AUDIO_SETTINGS", "SampleRate",
                                          INIConfig::Defaults::DEFAULT_SAMPLE_RATE);
        settings.bufferSize = file.getValue("AUDIO_SETTINGS", "BufferSize",
                                          INIConfig::Defaults::DEFAULT_BUFFER_SIZE);
        // ... continue for all settings

        return settings;
    }

    // Load player settings
    static PlayerSettings loadPlayerSettings(int playerIndex) {
        auto file = getINIFile(INIConfig::PLAYERS_FILE);
        auto section = "PLAYER_" + juce::String(playerIndex + 1);

        PlayerSettings settings;
        settings.enabled = file.getBoolValue(section, "Enabled",
                                           INIConfig::Defaults::DEFAULT_PLAYER_ENABLED);
        settings.energy = file.getFloatValue(section, "Energy",
                                           INIConfig::Defaults::DEFAULT_ENERGY);
        // ... continue for all player settings

        return settings;
    }
};
```

### Writing INI Data
```cpp
// Save global settings
static void saveGlobalSettings(const GlobalSettings& settings) {
    auto file = getINIFile(INIConfig::GLOBAL_SETTINGS_FILE);

    file.setValue("AUDIO_SETTINGS", "SampleRate", settings.sampleRate);
    file.setValue("AUDIO_SETTINGS", "BufferSize", settings.bufferSize);
    file.setValue("UI_SETTINGS", "WindowWidth", settings.windowWidth);
    file.setValue("UI_SETTINGS", "WindowHeight", settings.windowHeight);

    // Add file header
    updateFileHeader(file);

    // Save to disk
    file.save();
}

// Save player settings with validation
static void savePlayerSettings(int playerIndex, const PlayerSettings& settings) {
    // Validate settings before saving
    if (!validatePlayerSettings(settings)) {
        throw std::invalid_argument("Invalid player settings");
    }

    auto file = getINIFile(INIConfig::PLAYERS_FILE);
    auto section = "PLAYER_" + juce::String(playerIndex + 1);

    file.setValue(section, "Enabled", settings.enabled);
    file.setValue(section, "Energy", settings.energy);
    file.setValue(section, "DrumKit", settings.drumKit);

    updateFileHeader(file);
    file.save();
}
```

## 🎯 Layout Integration Pattern

### Component Initialization from INI
```cpp
class StandardComponent : public juce::Component {
public:
    StandardComponent() {
        // Initialize from INI constants (NEVER hardcode)
        initializeFromINI();
        setupLayout();
    }

private:
    void initializeFromINI() {
        // Get dimensions from INI
        defaultWidth = INIConfig::LayoutConstants::defaultInterfaceWidth;
        defaultHeight = INIConfig::LayoutConstants::defaultInterfaceHeight;
        margin = INIConfig::LayoutConstants::defaultMargin;
        spacing = INIConfig::LayoutConstants::defaultSpacing;

        // Load component-specific settings
        loadComponentSettings();
    }

    void setupLayout() {
        // Calculate proportional layout based on INI constants
        auto area = getLocalBounds();
        auto scaleFactor = getWidth() / static_cast<float>(defaultWidth);

        // Apply INI-driven sizing
        buttonHeight = INIConfig::LayoutConstants::defaultButtonHeight * scaleFactor;
        sliderHeight = INIConfig::LayoutConstants::defaultSliderHeight * scaleFactor;
    }

    void resized() override {
        // Recalculate layout using INI constants
        setupLayout();
        updateChildComponents();
    }
};
```

### Player Button Layout (Special Case)
```cpp
std::tuple<int, int, int, int> calculatePlayerButtonLayout() {
    auto availableWidth = getWidth();

    // Use INI constants for margin calculations
    auto buttonWidth = availableWidth / 10.75f; // Derived from INI percentages
    auto quarterMargin = static_cast<int>(buttonWidth *
        INIConfig::LayoutConstants::playerButtonMarginPercent / 100.0f);
    auto halfMargin = static_cast<int>(buttonWidth *
        INIConfig::LayoutConstants::playerButtonStartMarginPercent / 100.0f);

    // Layout based on INI configuration
    struct ButtonLayout {
        int x, y, width, height;
    };

    std::vector<ButtonLayout> layouts(8);

    // Player 1: half margin left
    layouts[0] = {halfMargin, 0, buttonWidth, getHeight()};

    // Players 2-7: quarter margin left
    for (int i = 1; i < 7; ++i) {
        layouts[i] = {layouts[i-1].x + buttonWidth + quarterMargin, 0,
                     buttonWidth, getHeight()};
    }

    // Player 8: quarter margin left + half margin right
    layouts[7] = {layouts[6].x + buttonWidth + quarterMargin, 0,
                 buttonWidth, getHeight()};

    return layouts;
}
```

## 🔍 Data Validation & Safety

### Input Validation
```cpp
class INIValidator {
public:
    static bool validateAudioSettings(const AudioSettings& settings) {
        // Sample rate validation
        const std::vector<int> validSampleRates = {44100, 48000, 88200, 96000, 192000};
        if (std::find(validSampleRates.begin(), validSampleRates.end(),
                     settings.sampleRate) == validSampleRates.end()) {
            return false;
        }

        // Buffer size validation (power of 2, reasonable range)
        if (settings.bufferSize < 32 || settings.bufferSize > 4096 ||
            (settings.bufferSize & (settings.bufferSize - 1)) != 0) {
            return false;
        }

        return true;
    }

    static bool validatePlayerSettings(const PlayerSettings& settings) {
        // Energy range validation
        if (settings.energy < 0.0f || settings.energy > 100.0f) {
            return false;
        }

        // Swing range validation
        if (settings.swing < 0.0f || settings.swing > 100.0f) {
            return false;
        }

        // Volume range validation
        if (settings.volume < 0.0f || settings.volume > 1.0f) {
            return false;
        }

        return true;
    }
};
```

### Error Handling
```cpp
class INIErrorHandler {
public:
    static void handleCorruptedFile(const juce::String& filename) {
        DBG("INI file corrupted: " + filename);

        // Create backup of corrupted file
        auto backupName = filename + ".corrupted." +
                         juce::Time::getCurrentTime().toString(false, true);
        // ... backup logic

        // Restore from backup or create default
        restoreOrCreateDefault(filename);
    }

    static void handleMissingFile(const juce::String& filename) {
        DBG("INI file missing: " + filename);
        createDefaultFile(filename);
    }

private:
    static void createDefaultFile(const juce::String& filename) {
        // Create file with default values from INIConfig::Defaults
        auto file = getINIFile(filename);
        populateWithDefaults(file);
        updateFileHeader(file);
        file.save();
    }
};
```

## 🔄 Preset System Integration

### Preset Structure
```ini
# Example preset file
[PRESET_INFO]
Name=Rock Preset
Author=OTTO User
Created=2024-01-15T10:30:00Z
Description=Heavy rock drum setup
Tags=rock,heavy,energetic

[GLOBAL_SETTINGS]
Tempo=140
TimeSignature=4/4
MasterVolume=0.85

[PLAYER_STATES]
ActivePlayers=1,2,3,4
Player1Pattern=Rock_Kick
Player2Pattern=Rock_Snare
Player3Pattern=Rock_Hihat
Player4Pattern=Rock_Crash

[MIXER_STATE]
Player1Volume=0.9
Player2Volume=0.8
Player3Volume=0.7
Player4Volume=0.6
ReverbSend=0.3
DelaySend=0.1
```

### Preset Loading
```cpp
class PresetManager {
public:
    static bool loadPreset(const juce::String& presetName) {
        try {
            auto presetFile = getPresetFile(presetName);

            // Load global settings
            auto globalSettings = loadGlobalSettingsFromPreset(presetFile);
            applyGlobalSettings(globalSettings);

            // Load player states
            auto playerStates = loadPlayerStatesFromPreset(presetFile);
            applyPlayerStates(playerStates);

            // Load mixer state
            auto mixerState = loadMixerStateFromPreset(presetFile);
            applyMixerState(mixerState);

            return true;
        }
        catch (const std::exception& e) {
            DBG("Failed to load preset: " + juce::String(e.what()));
            return false;
        }
    }

    static bool savePreset(const juce::String& presetName,
                          const PresetInfo& info) {
        try {
            auto presetFile = createPresetFile(presetName);

            // Save preset info
            savePresetInfo(presetFile, info);

            // Save current application state
            saveCurrentGlobalSettings(presetFile);
            saveCurrentPlayerStates(presetFile);
            saveCurrentMixerState(presetFile);

            updateFileHeader(presetFile);
            presetFile.save();

            return true;
        }
        catch (const std::exception& e) {
            DBG("Failed to save preset: " + juce::String(e.what()));
            return false;
        }
    }
};
```

## 🗃️ File Management

### Directory Structure Management
```cpp
class INIFileManager {
public:
    static void initializeDataDirectories() {
        auto dataDir = getOTTODataDirectory();

        // Create all required directories
        createDirectoryIfNotExists(dataDir.getChildFile(INIConfig::SETTINGS_FOLDER));
        createDirectoryIfNotExists(dataDir.getChildFile(INIConfig::PRESETS_FOLDER));
        createDirectoryIfNotExists(dataDir.getChildFile(INIConfig::PATTERNS_FOLDER));
        createDirectoryIfNotExists(dataDir.getChildFile(INIConfig::KITS_FOLDER));
        createDirectoryIfNotExists(dataDir.getChildFile(INIConfig::MIXING_FOLDER));
        createDirectoryIfNotExists(dataDir.getChildFile(INIConfig::SYSTEM_FOLDER));
        createDirectoryIfNotExists(dataDir.getChildFile("Backup"));
    }

    static juce::File getOTTODataDirectory() {
        #if JUCE_MAC
            return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                   .getChildFile("OTTO");
        #elif JUCE_WINDOWS
            return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                   .getChildFile("OTTO");
        #elif JUCE_LINUX
            return juce::File::getSpecialLocation(juce::File::userHomeDirectory)
                   .getChildFile(".otto");
        #elif JUCE_IOS
            return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                   .getChildFile("OTTO");
        #elif JUCE_ANDROID
            return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                   .getChildFile("OTTO");
        #endif
    }
};
```

### Backup System
```cpp
class INIBackupManager {
public:
    static void createBackup() {
        auto timestamp = juce::Time::getCurrentTime().toString(false, true, false, true);
        auto backupDir = getOTTODataDirectory().getChildFile("Backup").getChildFile(timestamp);

        if (!backupDir.createDirectory()) {
            DBG("Failed to create backup directory");
            return;
        }

        // Copy all INI files to backup
        copyAllINIFiles(getOTTODataDirectory(), backupDir);
    }

    static bool restoreFromBackup(const juce::String& backupName) {
        auto backupDir = getOTTODataDirectory().getChildFile("Backup").getChildFile(backupName);

        if (!backupDir.exists()) {
            return false;
        }

        // Restore all INI files from backup
        copyAllINIFiles(backupDir, getOTTODataDirectory());
        return true;
    }

    static void autoBackup() {
        // Create daily backup automatically
        static auto lastBackup = juce::Time();
        auto now = juce::Time::getCurrentTime();

        if (now.toMilliseconds() - lastBackup.toMilliseconds() > 24 * 60 * 60 * 1000) {
            createBackup();
            lastBackup = now;
        }
    }
};
```

## ✅ INI Development Checklist

Before implementing any new feature, verify:

- [ ] **All Constants in INIConfig.h**: No hardcoded values in implementation
- [ ] **Default Values Defined**: All settings have defaults in INIConfig::Defaults
- [ ] **Validation Implemented**: Input validation for all user-modifiable values
- [ ] **Error Handling**: Graceful handling of corrupted/missing files
- [ ] **Backup Integration**: Settings changes trigger backup creation
- [ ] **Cross-Platform Paths**: File paths work on all 5 platforms
- [ ] **Thread Safety**: INI operations are thread-safe where needed
- [ ] **Performance Optimized**: Minimal file I/O in real-time code paths
- [ ] **Documentation Updated**: Changes reflected in INI documentation
- [ ] **Testing Completed**: INI operations tested on all platforms

---

This INI-based system ensures **consistent, maintainable, and user-friendly** data management while eliminating hardcoded values and providing professional-grade configuration capabilities across all platforms.
