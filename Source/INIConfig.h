/**
 * @file INIConfig.h
 * @brief Central INI-driven configuration system for OTTO JUCE 8 application
 *
 * This file contains all constants and configuration values used throughout the OTTO application.
 * It implements a percentage-based responsive layout system that scales components proportionally
 * across different screen sizes while maintaining minimum touch target accessibility requirements.
 *
 * DESIGN PHILOSOPHY:
 * ==================
 * - INI-Driven Layout Priority: All positioning uses constants from this file, no hardcoded values
 * - Responsive Design: Uses percentage-based calculations relative to DEFAULT_INTERFACE_WIDTH/HEIGHT
 * - Accessibility First: Enforces minimum 44px touch targets for mobile/tablet compatibility
 * - Future-Proof: Centralized configuration allows easy theme and layout modifications
 *
 * USAGE PATTERNS:
 * ===============
 * - Layout constants: INIConfig::LayoutConstants::Row3::buttonSize
 * - Default values: INIConfig::Defaults::DEFAULT_TEMPO
 * - File paths: INIConfig::THEMES_FILE
 * - Responsive sizing: All dimensions calculated as percentages of base resolution (1024x768)
 *
 * INTEGRATION WITH OTHER FILES:
 * =============================
 * - ResponsiveLayoutManager.cpp: Uses LayoutConstants for component positioning
 * - ColorScheme.cpp: References theme defaults and alpha values
 * - CustomLookAndFeel.cpp: Uses font sizes, corner radii, and spacing constants
 * - MainContentComponent.cpp: Applies row-based layout system from LayoutConstants
 * - Row1Component.cpp: Uses Row1 constants for header positioning
 * - PlayerTabsComponent.cpp: Uses Row2 constants for tab dimensions
 *
 * @author OTTO Development Team
 * @version 2.0
 * @date 2025
 */

#pragma once

#include <JuceHeader.h>

/**
 * @namespace INIConfig
 * @brief Root namespace containing all INI-driven configuration constants for OTTO
 *
 * This namespace encapsulates the entire configuration system, organizing constants
 * into logical sub-namespaces for easier maintenance and discovery.
 */
namespace INIConfig {

    // ============================================================================
    // ENHANCED DATABASE ROOT STRUCTURE CONSTANTS
    // ============================================================================
    // These constants define the comprehensive hierarchical folder structure for OTTO's
    // professional database system following the enhanced schema.
    // Used by: INIDataManager.cpp, StateManager.cpp, PresetManager.cpp

    /** @brief Root folder name for all OTTO application data */
    static const juce::String DATABASE_ROOT_FOLDER = "DatabaseRoot";

    // ========================================================================
    // PRIMARY DATABASE FOLDERS
    // ========================================================================

    /** @brief System-level configuration and device management */
    static const juce::String SYSTEM_FOLDER = "System";

    /** @brief Application settings and user preferences */
    static const juce::String SETTINGS_FOLDER = "Settings";

    /** @brief UI themes and visual configurations */
    static const juce::String THEMES_FOLDER = "Themes";

    /** @brief Player configurations and states */
    static const juce::String PLAYERS_FOLDER = "Players";

    /** @brief Drum kit definitions and sample mappings */
    static const juce::String KITS_FOLDER = "Kits";

    /** @brief Pattern definitions and arrangements */
    static const juce::String PATTERNS_FOLDER = "Patterns";

    /** @brief Mixer settings and effects configurations */
    static const juce::String MIX_FOLDER = "Mix";

    /** @brief User and factory presets organized by category */
    static const juce::String PRESETS_FOLDER = "Presets";

    /** @brief Performance session and live configurations */
    static const juce::String PERFORMANCE_FOLDER = "Performance";

    /** @brief Cache files for optimization */
    static const juce::String CACHE_FOLDER = "Cache";

    // ========================================================================
    // SETTINGS SUBFOLDERS
    // ========================================================================

    /** @brief UI-specific settings subfolder */
    static const juce::String SETTINGS_UI_FOLDER = "UI";

    // ========================================================================
    // MIX SUBFOLDERS
    // ========================================================================

    /** @brief Effect presets subfolder */
    static const juce::String MIX_PRESETS_FOLDER = "Presets";

    /** @brief Channel configurations subfolder */
    static const juce::String MIX_CHANNELS_FOLDER = "Channels";

    /** @brief Master channel settings subfolder */
    static const juce::String MIX_MASTER_FOLDER = "Master";

    static const juce::String THEMES_USER_FOLDER = "User";

    // ========================================================================
    // PRESETS SUBFOLDERS
    // ========================================================================

    /** @brief Preset categories subfolder */
    static const juce::String PRESETS_CATEGORIES_FOLDER = "Categories";

    /** @brief Project templates subfolder */
    static const juce::String PRESETS_TEMPLATES_FOLDER = "Templates";

    /** @brief User-created presets subfolder */
    static const juce::String PRESETS_USER_FOLDER = "User";

    /** @brief Vintage style presets subfolder */
    static const juce::String PRESETS_VINTAGE_FOLDER = "Vintage";

    /** @brief Modern style presets subfolder */
    static const juce::String PRESETS_MODERN_FOLDER = "Modern";

    /** @brief Experimental style presets subfolder */
    static const juce::String PRESETS_EXPERIMENTAL_FOLDER = "Experimental";

    // ============================================================================
    // ENHANCED INI FILE NAME CONSTANTS
    // ============================================================================
    // These constants define the comprehensive .ini filenames for the enhanced database system.
    // Used by: INIDataManager.cpp for file I/O operations across the application

    // ========================================================================
    // SYSTEM FOLDER FILES
    // ========================================================================

    /** @brief Application version, build info, startup settings */
    static const juce::String APPLICATION_FILE = "Application.ini";

    /** @brief MIDI input/output device configurations */
    static const juce::String MIDI_DEVICES_FILE = "MidiDevices.ini";

    /** @brief Audio interface settings and routing */
    static const juce::String AUDIO_DEVICES_FILE = "AudioDevices.ini";

    /** @brief User-defined sample/kit/pattern directories */
    static const juce::String EXTERNAL_FOLDERS_FILE = "ExternalFolders.ini";

    /** @brief File system indexing and caching */
    static const juce::String FILE_INDEX_FILE = "FileIndex.ini";

    /** @brief License keys, registration data */
    static const juce::String LICENSING_FILE = "Licensing.ini";

    // ========================================================================
    // SETTINGS FOLDER FILES
    // ========================================================================

    /** @brief Application-wide preferences */
    static const juce::String GLOBAL_SETTINGS_FILE = "Global.ini";

    /** @brief Buffer sizes, sample rates, latency */
    static const juce::String AUDIO_SETTINGS_FILE = "AudioSettings.ini";

    /** @brief MIDI timing, sync, clock settings */
    static const juce::String MIDI_SETTINGS_FILE = "MidiSettings.ini";

    /** @brief CPU usage limits, threading options */
    static const juce::String PERFORMANCE_SETTINGS_FILE = "Performance.ini";

    /** @brief Custom key bindings */
    static const juce::String KEYBOARD_SHORTCUTS_FILE = "KeyboardShortcuts.ini";

    // ========================================================================
    // SETTINGS/UI FOLDER FILES
    // ========================================================================

    /** @brief Active theme, color schemes */
    static const juce::String THEME_MANAGER_FILE = "ThemeManager.ini";

    /** @brief Window positions, sizes, docking */
    static const juce::String WINDOW_LAYOUT_FILE = "WindowLayout.ini";

    /** @brief Analyzer display settings */
    static const juce::String SPECTRUM_ANALYZER_FILE = "SpectrumAnalyzer.ini";

    /** @brief Waveform visualization options */
    static const juce::String WAVEFORM_DISPLAY_FILE = "WaveformDisplay.ini";

    /** @brief Responsive UI configurations */
    static const juce::String ADAPTIVE_LAYOUT_MANAGER_FILE = "AdaptiveLayoutManager.ini";

    /** @brief Touch/mouse gesture mappings */
    static const juce::String GESTURE_RECOGNIZER_FILE = "GestureRecognizer.ini";

    // ========================================================================
    // THEMES FOLDER FILES
    // ========================================================================

    /** @brief Built-in default theme */
    static const juce::String DEFAULT_THEME_FILE = "Default.ini";

    /** @brief Dark mode theme */
    static const juce::String DARK_THEME_FILE = "Dark.ini";

    /** @brief Light mode theme */
    static const juce::String LIGHT_THEME_FILE = "Light.ini";

    /** @brief Theme registry and metadata */
    static const juce::String THEME_INDEX_FILE = "ThemeIndex.ini";

    // ========================================================================
    // PLAYERS FOLDER FILES
    // ========================================================================

    /** @brief Individual player settings */
    static const juce::String PLAYER_CONFIGS_FILE = "PlayerConfigs.ini";

    /** @brief Player grouping and routing */
    static const juce::String PLAYER_GROUPS_FILE = "PlayerGroups.ini";

    /** @brief Default player parameters */
    static const juce::String GLOBAL_PLAYER_SETTINGS_FILE = "GlobalPlayerSettings.ini";

    /** @brief Runtime states (play, mute, solo) */
    static const juce::String PLAYER_STATES_FILE = "PlayerStates.ini";

    // ========================================================================
    // KITS FOLDER FILES
    // ========================================================================

    /** @brief Drum kit definitions and metadata */
    static const juce::String DRUM_KITS_FILE = "DrumKits.ini";

    /** @brief Kit organization and tagging */
    static const juce::String KIT_CATEGORIES_FILE = "KitCategories.ini";

    /** @brief SFZ file associations */
    static const juce::String SAMPLE_MAPPINGS_FILE = "SampleMappings.ini";

    /** @brief Velocity layer configurations */
    static const juce::String VELOCITY_MAPPINGS_FILE = "VelocityMappings.ini";

    /** @brief MIDI note assignments */
    static const juce::String MIDI_MAPPINGS_FILE = "MidiMappings.ini";

    /** @brief Fast lookup and search index */
    static const juce::String KIT_INDEX_FILE = "KitIndex.ini";

    // ========================================================================
    // PATTERNS FOLDER FILES
    // ========================================================================

    /** @brief Pattern group definitions */
    static const juce::String PATTERN_GROUPS_FILE = "PatternGroups.ini";

    /** @brief Pattern properties and tags */
    static const juce::String PATTERN_METADATA_FILE = "PatternMetadata.ini";

    /** @brief Song arrangement chains */
    static const juce::String PATTERN_CHAINS_FILE = "PatternChains.ini";

    /** @brief Pattern search and categorization */
    static const juce::String PATTERN_INDEX_FILE = "PatternIndex.ini";

    /** @brief Tempo changes within patterns */
    static const juce::String TEMPO_MAPS_FILE = "TempoMaps.ini";

    // ========================================================================
    // MIX FOLDER FILES
    // ========================================================================

    /** @brief EQ curve presets */
    static const juce::String EQ_PRESETS_FILE = "EQPresets.ini";

    /** @brief Compression settings */
    static const juce::String COMPRESSOR_PRESETS_FILE = "CompressorPresets.ini";

    /** @brief Reverb configurations */
    static const juce::String REVERB_PRESETS_FILE = "ReverbPresets.ini";

    /** @brief Delay/echo presets */
    static const juce::String DELAY_PRESETS_FILE = "DelayPresets.ini";

    /** @brief Complete effect chain presets */
    static const juce::String EFFECT_CHAIN_PRESETS_FILE = "EffectChainPresets.ini";

    /** @brief Individual channel presets */
    static const juce::String CHANNEL_PRESETS_FILE = "ChannelPresets.ini";

    /** @brief Channel grouping presets */
    static const juce::String CHANNEL_GROUPS_FILE = "ChannelGroups.ini";

    /** @brief Auxiliary bus setups */
    static const juce::String BUS_CONFIGURATIONS_FILE = "BusConfigurations.ini";

    /** @brief Signal routing presets */
    static const juce::String ROUTING_PRESETS_FILE = "RoutingPresets.ini";

    /** @brief Master channel configurations */
    static const juce::String MASTER_CHANNEL_PRESETS_FILE = "MasterChannelPresets.ini";

    /** @brief Master EQ settings */
    static const juce::String MASTER_EQ_FILE = "MasterEQ.ini";

    /** @brief Master limiting presets */
    static const juce::String MASTER_LIMITER_FILE = "MasterLimiter.ini";

    /** @brief Master metering configurations */
    static const juce::String MASTER_METERING_FILE = "MasterMetering.ini";

    /** @brief MIDI input layout file */
    static const juce::String MIDI_IN_LAYOUT_FILE = "MidiInLayout.ini";

    /** @brief MIDI output layout file */
    static const juce::String MIDI_OUT_LAYOUT_FILE = "MidiOutLayout.ini";

    /** @brief Channel preset groups file */
    static const juce::String CHANNEL_PRESET_GROUPS_FILE = "ChannelPresetGroups.ini";

    /** @brief Complete mixer state snapshots */
    static const juce::String MIXER_SNAPSHOTS_FILE = "MixerSnapshots.ini";

    // ========================================================================
    // PRESETS FOLDER FILES
    // ========================================================================

    /** @brief Blues style presets */
    static const juce::String BLUES_PRESETS_FILE = "Blues.ini";

    /** @brief Jazz style presets */
    static const juce::String JAZZ_PRESETS_FILE = "Jazz.ini";

    /** @brief Soul style presets */
    static const juce::String SOUL_PRESETS_FILE = "Soul.ini";

    /** @brief Funk style presets */
    static const juce::String FUNK_PRESETS_FILE = "Funk.ini";

    /** @brief EDM style presets */
    static const juce::String EDM_PRESETS_FILE = "EDM.ini";

    /** @brief Hip-Hop style presets */
    static const juce::String HIPHOP_PRESETS_FILE = "HipHop.ini";

    /** @brief Rock style presets */
    static const juce::String ROCK_PRESETS_FILE = "Rock.ini";

    /** @brief Pop style presets */
    static const juce::String POP_PRESETS_FILE = "Pop.ini";

    /** @brief Ambient style presets */
    static const juce::String AMBIENT_PRESETS_FILE = "Ambient.ini";

    /** @brief Industrial style presets */
    static const juce::String INDUSTRIAL_PRESETS_FILE = "Industrial.ini";

    /** @brief Glitch style presets */
    static const juce::String GLITCH_PRESETS_FILE = "Glitch.ini";

    /** @brief Complete project setups */
    static const juce::String PROJECT_TEMPLATES_FILE = "ProjectTemplates.ini";

    /** @brief Kit configuration templates */
    static const juce::String KIT_TEMPLATES_FILE = "KitTemplates.ini";

    /** @brief Pattern structure templates */
    static const juce::String PATTERN_TEMPLATES_FILE = "PatternTemplates.ini";

    /** @brief Preset search and metadata */
    static const juce::String PRESET_INDEX_FILE = "PresetIndex.ini";

    // ========================================================================
    // PERFORMANCE FOLDER FILES
    // ========================================================================

    /** @brief Performance session configurations */
    static const juce::String SESSIONS_FILE = "Sessions.ini";

    /** @brief Organized preset/pattern lists */
    static const juce::String SET_LISTS_FILE = "SetLists.ini";

    /** @brief MIDI controller mappings */
    static const juce::String MACRO_CONTROLS_FILE = "MacroControls.ini";

    /** @brief Automated scene transitions */
    static const juce::String SCENE_CHANGES_FILE = "SceneChanges.ini";

    /** @brief Emergency backup configurations */
    static const juce::String BACKUP_STATES_FILE = "BackupStates.ini";

    // ========================================================================
    // CACHE FOLDER FILES
    // ========================================================================

    /** @brief Sample loading optimization */
    static const juce::String SAMPLE_CACHE_FILE = "SampleCache.ini";

    /** @brief Pattern parsing cache */
    static const juce::String PATTERN_CACHE_FILE = "PatternCache.ini";

    /** @brief Kit loading cache */
    static const juce::String KIT_CACHE_FILE = "KitCache.ini";

    /** @brief Global search indexing */
    static const juce::String SEARCH_INDEX_FILE = "SearchIndex.ini";

    /** @brief Cached MIDI file analysis data for performance optimization */
    static const juce::String MIDI_ANALYSIS_CACHE_FILE = "MidiAnalysisCache.ini";

    // ========================================================================
    // LEGACY COMPATIBILITY FILES
    // ========================================================================

    /** @brief Legacy themes file for backward compatibility */
    static const juce::String THEMES_FILE = "Themes.ini";

    /** @brief Legacy presets file for backward compatibility */
    static const juce::String PRESETS_FILE = "Presets.ini";

    /** @brief Legacy players file for backward compatibility */
    static const juce::String PLAYERS_FILE = "Players.ini";

    /** @brief Hardware controller preset configurations */
    static const juce::String CONTROLLER_PRESETS_FILE = "ControllerPresets.ini";

    /** @brief Groove template definitions for swing and timing variations */
    static const juce::String GROOVE_TEMPLATES_FILE = "GrooveTemplates.ini";

    // ============================================================================
    // VERSION CONTROL
    // ============================================================================

    /** @brief Current INI file format version for backward compatibility checking */
    static const juce::String FILE_FORMAT_VERSION = "2.0";

    /**
     * @namespace Defaults
     * @brief Default values for all application settings and parameters
     *
     * This namespace contains default values used throughout the application when
     * no user preferences have been set or when resetting to factory defaults.
     * These values are carefully chosen to provide optimal user experience while
     * maintaining compatibility across different platforms and screen sizes.
     *
     * ORGANIZATION:
     * - Interface dimensions and scaling
     * - Audio engine parameters
     * - MIDI settings and timing
     * - Visual and UI preferences
     * - Performance optimization settings
     */
    namespace Defaults {
        // ========================================================================
        // INTERFACE DIMENSIONS AND POSITIONING
        // ========================================================================
        // These values define the base resolution for responsive scaling calculations.
        // All percentage-based layout constants are calculated relative to these dimensions.
        // Used by: ResponsiveLayoutManager.cpp, all component classes for positioning

        /**
         * @brief Base interface width for responsive scaling calculations
         *
         * MINIMUM SYSTEM REQUIREMENT: 1024px width display resolution
         * This is the smallest supported screen width that maintains:
         * - Proper component spacing and alignment
         * - Readable text at all zoom levels
         * - Sufficient space for 8-player layout
         * - Touch target accessibility compliance
         *
         * Referenced by: All LayoutConstants calculations, ResponsiveLayoutManager.cpp
         */
        static const int DEFAULT_INTERFACE_WIDTH = 1366;

        /**
         * @brief Base interface height for responsive scaling calculations
         *
         * MINIMUM SYSTEM REQUIREMENT: 768px height display resolution
         * This is the smallest supported screen height that maintains:
         * - 44px minimum touch targets (accessibility compliance)
         * - 300px minimum pattern matrix height (core functionality)
         * - Proper row-based layout with 6 distinct sections
         * - Adequate vertical spacing between components
         *
         * Referenced by: All LayoutConstants calculations, MainContentComponent.cpp
         */
        static const int DEFAULT_INTERFACE_HEIGHT = 768;

        /** @brief Default window X position on screen (used by PluginEditor.cpp) */
        static const int DEFAULT_INTERFACE_X = 100;

        /** @brief Default window Y position on screen (used by PluginEditor.cpp) */
        static const int DEFAULT_INTERFACE_Y = 100;

        /** @brief Base UI scale factor for interface elements (used by CustomLookAndFeel.cpp) */
        static const float DEFAULT_INTERFACE_SCALE = 1.0f;

        /** @brief Alternative scale factor for component sizing (used by ResponsiveLayoutManager.cpp) */
        static const float DEFAULT_SCALE_FACTOR = 1.0f;

        // ========================================================================
        // THEME AND VISUAL SETTINGS
        // ========================================================================

        /** @brief Default color theme ID (used by ColorScheme.cpp, CustomLookAndFeel.cpp) */
        static const int DEFAULT_THEME_ID = 1;

        // ========================================================================
        // SYNCHRONIZATION AND COLLABORATION SETTINGS
        // ========================================================================
        // These settings control external sync, collaboration, and timing features.
        // Used by: CloudServiceManager.cpp, CollaborationPanel.cpp, MidiEngine.cpp

        /** @brief Default Link sync mode setting ("Off", "Enabled", "StartStop") */
        static const juce::String DEFAULT_LINK_SYNC_MODE = "Off";

        /** @brief Default Link sync value (0=Off, 1=Enabled, 2=StartStop only) */
        static const int DEFAULT_LINK_SYNC_VALUE = 0;

        /** @brief Default cloud service connection ("None", "Ableton", "Custom") */
        static const juce::String DEFAULT_CLOUD_CONNECTION = "None";

        // ========================================================================
        // METRONOME AND TIMING SETTINGS
        // ========================================================================
        // Used by: MidiEngine.cpp, Row1Component.cpp for tempo and timing

        /** @brief Default metronome enabled state */
        static const bool DEFAULT_METRONOME_ENABLED = false;

        /** @brief Default metronome volume level (0.0 to 1.0) */
        static const float DEFAULT_METRONOME_VOLUME = 0.5f;

        /** @brief Default metronome sound type ("Click", "Beep", "Wood") */
        static const juce::String DEFAULT_METRONOME_SOUND = "Click";

        /** @brief Default quantization grid value (16th notes) */
        static const int DEFAULT_QUANTIZE_VALUE = 16;

        /** @brief Default count-in duration in bars */
        static const int DEFAULT_COUNT_IN_BARS = 0;

        // ========================================================================
        // AUDIO ENGINE SETTINGS
        // ========================================================================
        // These settings control the core audio processing configuration.
        // Used by: PluginProcessor.cpp, SFZEngine.cpp, MidiEngine.cpp, SettingsPanel.cpp

        /** @brief Default audio settings profile ID */
        static const int DEFAULT_AUDIO_SETTINGS_ID = 1;

        /** @brief Default MIDI clock output enabled state */
        static const bool DEFAULT_MIDI_CLOCK_OUT = false;

        /** @brief Default MIDI clock input enabled state */
        static const bool DEFAULT_MIDI_CLOCK_IN = false;

        /** @brief Default auto-save interval in seconds (5 minutes) */
        static const int DEFAULT_AUTO_SAVE_INTERVAL = 300;

        /** @brief Default audio sample rate in Hz (48kHz for professional quality) */
        static const int DEFAULT_SAMPLE_RATE = 48000;

        /** @brief Default audio buffer size in samples (balanced latency/performance) */
        static const int DEFAULT_BUFFER_SIZE = 256;

        /** @brief Default audio device name selection */
        static const juce::String DEFAULT_AUDIO_DEVICE = "Default";

        /** @brief Default number of audio input channels */
        static const int DEFAULT_INPUT_CHANNELS = 2;

        /** @brief Default number of audio output channels */
        static const int DEFAULT_OUTPUT_CHANNELS = 2;

        /** @brief Default latency compensation enabled state */
        static const bool DEFAULT_LATENCY_COMPENSATION = false;

        /** @brief Default audio bit depth (24-bit for quality) */
        static const int DEFAULT_BIT_DEPTH = 24;

        /** @brief Default ASIO driver enabled state (Windows-specific) */
        static const bool DEFAULT_ENABLE_ASIO = false;

        /** @brief Default multicore processing enabled state */
        static const bool DEFAULT_ENABLE_MULTICORE = true;

        // ========================================================================
        // PLAYER AND MUSICAL SETTINGS
        // ========================================================================
        // These settings control the musical behavior and player state management.
        // Used by: PlayerTabsComponent.cpp, MainContentComponent.cpp, PatternManager.cpp

        /** @brief Default drum kit selection (Get this list from our 'assets' path and/or use data path) */
        static const juce::String DEFAULT_DRUMKIT = "Default";

        /** @brief Default player enabled state (each of 8 players) */
        static const bool DEFAULT_PLAYER_ENABLED = true;

        /** @brief Default edit mode state (false = performance, true = edit) */
        static const bool DEFAULT_EDIT_MODE = false;

        /** @brief Default swing percentage (0-100, affects timing groove) */
        static const float SWING = 10.0f;

        /** @brief Default energy level (0-100, affects pattern intensity) */
        static const float ENERGY = 75.0f;

        /** @brief Default player volume level (0.0 to 1.0) */
        static const float VOLUME = 0.8f;

        /** @brief Default playback state (false = stopped, true = playing) */
        static const bool DEFAULT_PLAY_STATE = false;

        /** @brief Default active player index (0-7) */
        static const int DEFAULT_CURRENT_PLAYER = 0;

        /** @brief Default drum kit index selection */
        static const int DEFAULT_KIT_INDEX = 0;

        /** @brief Default preset index selection */
        static const int DEFAULT_CURRENT_PRESET = 0;

        /** @brief Default prefix for pattern group names */
        static const juce::String DEFAULT_GROUP_PREFIX = "Group ";

        /** @brief Default selected drum button index (0-15 for 4x4 grid) */
        static const int DEFAULT_SELECTED_BUTTON = 0;

        /** @brief Default favorite state for patterns/presets */
        static const bool DEFAULT_FAVORITE_STATE = false;

        /** @brief Default loop position percentage (0-100) */
        static const float DEFAULT_LOOP_POSITION = 0.0f;

        // ========================================================================
        // MIXING AND EFFECTS SETTINGS
        // ========================================================================
        // These settings control the default mixing levels and effects processing.
        // Used by: Mixer.cpp, PopupWindowsMixer.cpp, channel processing classes

        /** @brief Default master output volume level (0.0 to 1.0) */
        static const float DEFAULT_MASTER_VOLUME = 0.8f;

        /** @brief Default reverb send mix level (0.0 to 1.0) */
        static const float DEFAULT_REVERB_MIX = 0.0f;

        /** @brief Default delay send mix level (0.0 to 1.0) */
        static const float DEFAULT_DELAY_MIX = 0.0f;

        /** @brief Default kick drum volume level (0.0 to 1.0) */
        static const float DEFAULT_KICK_VOLUME = 0.8f;

        /** @brief Default snare drum volume level (0.0 to 1.0) */
        static const float DEFAULT_SNARE_VOLUME = 0.8f;

        /** @brief Default hi-hat volume level (0.0 to 1.0) */
        static const float DEFAULT_HIHAT_VOLUME = 0.8f;

        /** @brief Default mixer channel volume level (0.0 to 1.0) */
        static const float DEFAULT_CHANNEL_VOLUME = 0.8f;

        // ========================================================================
        // PHOSPHOR ICON SYSTEM SETTINGS
        // ========================================================================
        // These settings control the Phosphor icon rendering system behavior.
        // Used by: CustomLookAndFeel.cpp, PhosphorIconButton components

        /** @brief Default Phosphor icon font weight (1=thin, 2=light, 3=regular, 4=bold) */
        static const int DEFAULT_PHOSPHOR_WEIGHT = 1;

        /**
         * @brief Default icon box fit ratio for responsive icon scaling
         *
         * This value (0.99 = 99%) ensures icons scale to fit 99% of their container
         * area, leaving a small margin for visual breathing room. This ratio is
         * applied after the ResponsiveLayoutManager calculates the box dimensions.
         *
         * Referenced by: CustomLookAndFeel.cpp phosphor icon rendering methods
         */
        static constexpr float DEFAULT_PHOSPHOR_ICON_BOX_FIT_RATIO = 0.99f;

        static const int DEFAULT_TEMPO = 120.0f;
        static const juce::String DEFAULT_TIME_SIGNATURE = "4/4";

        static const int MAX_PLAYERS = 8;
        static const int MAX_SCENES = 8;
        static const int MAX_COUNT_IN_BARS = 8;
        static const double BEATS_PER_BAR = 4.0;
        static const double MS_PER_MINUTE = 60000.0;
        static const double MS_PER_SECOND = 1000.0;
        static const double SECONDS_PER_MINUTE = 60.0;
        static const double BEAT_THRESHOLD = 0.01;
        static const float MAX_ENERGY = 100.0f;
        static const int TAP_TEMPO_AVERAGE_COUNT = 4;
        static const int MIDI_TICKS_PER_QUARTER_NOTE = 960;
        static const int MAX_DELAY_SAMPLES = 192000;
        static const int HUMANIZE_VELOCITY_RANGE = 10;
        static const float SCURVE_FACTOR = 3.0f;
        static const float SCURVE_MULTIPLIER = 2.0f;
        static const int FIXED_VELOCITY = 100;
        static const int TIME_SIGNATURE_NUMERATOR = 4;
        static const int TIME_SIGNATURE_DENOMINATOR = 4;

        static const float DEFAULT_FEEDBACK = 0.4f;
        static const float DEFAULT_SPREAD = 0.0f;
        static const float DEFAULT_DRIVE = 0.5f;
        static const float DEFAULT_MIX = 0.5f;
        static const float DEFAULT_ROOM_SIZE = 0.5f;
        static const float DEFAULT_DAMPING = 0.5f;
        static const float DEFAULT_WIDTH = 1.0f;
        static const float DEFAULT_COMPRESSOR_THRESHOLD = -12.0f;
        static const float DEFAULT_COMPRESSOR_RATIO = 4.0f;
        static const float DEFAULT_COMPRESSOR_ATTACK = 10.0f;
        static const float DEFAULT_COMPRESSOR_RELEASE = 100.0f;
        static const float DEFAULT_COMPRESSOR_KNEE = 2.0f;
        static const float DEFAULT_LIMITER_THRESHOLD = -3.0f;
        static const float DEFAULT_LIMITER_RELEASE = 50.0f;
        static const float DEFAULT_COMPLEXITY = 0.5f;
        static const float DEFAULT_HUMANIZATION = 0.3f;
        static const float DEFAULT_PATTERN_BARS = 1;
        static const float DEFAULT_MAKEUPGAIN = 0.0f;
        static const float DEFAULT_LEVELFOLLOW = 0.0f;
        static const float DEFAULT_MATCH_SCORE = 0.0f;
        static const bool DEFAULT_SIDECHAIN_ENABLED = false;
        static const float DEFAULT_LEVELLEFT = 0.0f;
        static const float DEFAULT_LEVELRIGHT = 0.0f;
        static const float DEFAULT_PEAKLEFT = 0.0f;
        static const float DEFAULT_PEAKRIGHT = 0.0f;
        static const bool DEFAULT_PINGPONG = false;
        static const float LIMITER_RELEASE_MULTIPLIER = -2.5f;

        static const int DEFAULT_TIMER_INTERVAL_MS = 50;
        static const float DEFAULT_ALPHA_FULL = 1.0f;
        static const float DEFAULT_DISABLED_ALPHA = 0.5f;
        static const float DEFAULT_HOVER_ALPHA = 0.8f;
        static const int ZERO_VALUE = 0;
        static const int ONE_VALUE = 1;

        static const int NUM_FX_CHANNELS = 2;
        static const int NUM_CHANNELS = MAX_PLAYERS + NUM_FX_CHANNELS;

        static const int MAX_VOICES = 64;
        static const int MAX_VOICES_PER_NOTE = 4;

        static const float DEFAULT_PREDELAY = 20.0f;
        static const float DEFAULT_REVERB_HIGH_CUT = 8000.0f;
        static const float DEFAULT_REVERB_LOW_CUT = 80.0f;
        static const float DEFAULT_DELAY_TIME = 125.0f;
        static const float DEFAULT_DELAY_HIGH_CUT = 4000.0f;
        static const float DEFAULT_DELAY_LOW_CUT = 100.0f;

        static const int DEFAULT_NUM_SUGGESTIONS = 5;

        static const int COLLAB_UPDATE_INTERVAL_MS = 500;
        static const int DEFAULT_THREAD_COUNT = 4;
        static const float DEFAULT_QUALITY = 0.5f;

        static const int DEFAULT_DECIMAL_PLACES = 1;
        static const int DEFAULT_FONT_STYLES_COUNT = 5;
        static const int DEFAULT_COMPONENT_COUNT = 100;
        static const int DEFAULT_ANIMATION_UPDATE_INTERVAL = 20;
        static const float DEFAULT_ANIMATION_SPEED = 0.1f;
        static const int DEFAULT_MAX_ITERATIONS = 1000;
        static const int DEFAULT_CACHE_SIZE = 512;
        static const double DEFAULT_PI_MULTIPLIER = 2.0;
        static const float DEFAULT_CROSSFADE_TIME = 0.5f;
        static const int DEFAULT_FFT_SIZE = 2048;
        static const int DEFAULT_OVERLAP_SIZE = 512;
        static const float DEFAULT_WINDOW_FUNCTION_SCALE = 0.5f;
        static const int DEFAULT_SPECTRUM_BANDS = 32;
        static const float DEFAULT_FREQUENCY_SMOOTHING = 0.8f;
        static const float DEFAULT_DB_FLOOR = -60.0f;
        static const float DEFAULT_DB_CEILING = 0.0f;
        static const int DEFAULT_HISTORY_SIZE = 1000;
        static const float DEFAULT_SMOOTHING_FACTOR = 0.95f;
        static const int DEFAULT_UPDATE_RATE_HZ = 30;
        static const float DEFAULT_JITTER_AMOUNT = 0.001f;
        static const int DEFAULT_INTERPOLATION_SAMPLES = 64;
        static const float DEFAULT_PHASE_INCREMENT = 0.01f;
        static const double DEFAULT_TWO_PI = 6.28318530718;
        static const float DEFAULT_SQRT_TWO = 1.41421356237f;
        static const int DEFAULT_POLYNOMIAL_ORDER = 3;
        static const float DEFAULT_DAMPING_FACTOR = 0.99f;
        static const int DEFAULT_CONVOLUTION_SIZE = 256;
        static const float DEFAULT_WET_DRY_MIX = 0.5f;
        static const int DEFAULT_DELAY_LINE_SIZE = 48000;
        static const float DEFAULT_MODULATION_DEPTH = 0.1f;
        static const float DEFAULT_MODULATION_RATE = 1.0f;
        static const int DEFAULT_LFO_TABLE_SIZE = 512;
        static const float DEFAULT_ENVELOPE_ATTACK = 0.01f;
        static const float DEFAULT_ENVELOPE_DECAY = 0.1f;
        static const float DEFAULT_ENVELOPE_SUSTAIN = 0.7f;
        static const float DEFAULT_ENVELOPE_RELEASE = 0.5f;
        static const int DEFAULT_FILTER_ORDER = 4;
        static const float DEFAULT_RESONANCE = 1.0f;
        static const float DEFAULT_DRIVE_AMOUNT = 0.0f;
        static const int DEFAULT_OVERSAMPLE_FACTOR = 4;
        static const float DEFAULT_GATE_THRESHOLD = -40.0f;
        static const float DEFAULT_GATE_RATIO = 10.0f;
        static const float DEFAULT_GATE_ATTACK = 0.001f;
        static const float DEFAULT_GATE_RELEASE = 0.1f;
        static const int DEFAULT_LOOKAHEAD_SIZE = 512;
        static const float DEFAULT_STEREO_WIDTH = 1.0f;
        static const float DEFAULT_BASS_FREQUENCY = 80.0f;
        static const float DEFAULT_TREBLE_FREQUENCY = 8000.0f;
        static const float DEFAULT_MID_FREQUENCY = 1000.0f;
        static const float DEFAULT_MID_Q = 0.7f;
        static const int DEFAULT_ANALYSIS_WINDOW_SIZE = 4096;
        static const float DEFAULT_PITCH_DETECTION_THRESHOLD = 0.1f;
        static const int DEFAULT_AUTOCORRELATION_SIZE = 2048;
        static const float DEFAULT_TUNING_REFERENCE = 440.0f;
        static const float DEFAULT_MIN_BPM = 60.0f;
        static const float DEFAULT_MAX_BPM = 200.0f;
        static const int DEFAULT_MIDI_LEARN_TIMEOUT = 5000;
        static const float DEFAULT_PARAMETER_SMOOTHING = 0.05f;
        static const int DEFAULT_PRESET_MORPH_TIME = 1000;
        static const float DEFAULT_MORPH_CURVE_POWER = 2.0f;
        static const int DEFAULT_UNDO_STACK_SIZE = 100;
        static const int DEFAULT_AUTOMATION_RESOLUTION = 128;
        static const float DEFAULT_AUTOMATION_CURVE_TENSION = 0.5f;
        static const int DEFAULT_TIME_DISPLAY_PRECISION = 3;
        static const float DEFAULT_ZOOM_FACTOR = 1.2f;
        static const float DEFAULT_MIN_ZOOM = 0.1f;
        static const float DEFAULT_MAX_ZOOM = 10.0f;
        static const int DEFAULT_GRID_SUBDIVISION = 16;
        static const float DEFAULT_GRID_OPACITY = 0.3f;
        static const int DEFAULT_WAVEFORM_CACHE_SECONDS = 30;
        static const float DEFAULT_PEAK_HOLD_TIME = 2.0f;
        static const float DEFAULT_METER_RELEASE_TIME = 0.3f;
        static const int DEFAULT_METER_SEGMENTS = 20;
        static const float DEFAULT_CLIP_THRESHOLD = 0.99f;
        static const int DEFAULT_SPECTRUM_REFRESH_RATE = 25;
        static const float DEFAULT_SPECTRUM_DECAY_RATE = 0.9f;
        static const int DEFAULT_PHASE_SCOPE_SAMPLES = 512;
        static const float DEFAULT_VECTORSCOPE_SCALE = 0.8f;
        static const int DEFAULT_GONIOMETER_DECAY = 100;
        static const float DEFAULT_CORRELATION_THRESHOLD = 0.5f;
        static const int DEFAULT_LOUDNESS_INTEGRATION_TIME = 3000;
        static const float DEFAULT_LUFS_TARGET = -16.0f;
        static const float DEFAULT_TRUE_PEAK_LIMIT = -1.0f;
        static const int DEFAULT_LIMITER_LOOKAHEAD = 5;
        static const float DEFAULT_SOFT_CLIP_THRESHOLD = -3.0f;
        static const float DEFAULT_SOFT_CLIP_RATIO = 0.5f;
        static const int DEFAULT_DITHER_BITS = 16;
        static const float DEFAULT_DITHER_AMPLITUDE = 0.5f;
        static const int DEFAULT_SIDECHAIN_FILTER_FREQ = 100;
        static const float DEFAULT_SIDECHAIN_FILTER_Q = 0.7f;
        static const float DEFAULT_EXPANDER_THRESHOLD = -30.0f;
        static const float DEFAULT_EXPANDER_RATIO = 2.0f;
        static const float DEFAULT_EXPANDER_KNEE = 2.0f;
        static const int DEFAULT_MULTIBAND_BANDS = 4;
        static const float DEFAULT_CROSSOVER_SLOPE = 24.0f;
        static const int DEFAULT_CONVOLUTION_PARTITIONS = 4;
        static const float DEFAULT_EARLY_REFLECTIONS_LEVEL = 0.3f;
        static const float DEFAULT_LATE_REVERB_LEVEL = 0.7f;
        static const float DEFAULT_REVERB_DENSITY = 0.85f;
        static const float DEFAULT_REVERB_BANDWIDTH = 0.8f;
        static const float DEFAULT_REVERB_DECAY = 0.5f;
        static const float DEFAULT_REVERB_PREDELAY_MAX = 100.0f;
        static const int DEFAULT_REVERB_DIFFUSION_STAGES = 4;
        static const float DEFAULT_CHORUS_DEPTH = 0.3f;
        static const float DEFAULT_CHORUS_RATE = 1.5f;
        static const int DEFAULT_CHORUS_VOICES = 3;
        static const float DEFAULT_CHORUS_SPREAD = 0.5f;
        static const float DEFAULT_FLANGER_DEPTH = 0.5f;
        static const float DEFAULT_FLANGER_RATE = 0.5f;
        static const float DEFAULT_FLANGER_FEEDBACK = 0.7f;
        static const float DEFAULT_FLANGER_MIX = 0.5f;
        static const float DEFAULT_PHASER_DEPTH = 0.5f;
        static const float DEFAULT_PHASER_RATE = 0.5f;
        static const int DEFAULT_PHASER_STAGES = 4;
        static const float DEFAULT_PHASER_FEEDBACK = 0.5f;
        static const float DEFAULT_TREMOLO_DEPTH = 0.5f;
        static const float DEFAULT_TREMOLO_RATE = 5.0f;
        static const float DEFAULT_TREMOLO_PHASE = 0.0f;
        static const int DEFAULT_TREMOLO_WAVEFORM = 0;
        static const float DEFAULT_VIBRATO_DEPTH = 0.2f;
        static const float DEFAULT_VIBRATO_RATE = 5.0f;
        static const float DEFAULT_VIBRATO_DELAY = 0.0f;
        static const float DEFAULT_AUTOWAH_DEPTH = 0.7f;
        static const float DEFAULT_AUTOWAH_RATE = 0.5f;
        static const float DEFAULT_AUTOWAH_RESONANCE = 2.0f;
        static const float DEFAULT_AUTOWAH_SENSITIVITY = 0.5f;
        static const float DEFAULT_RINGMOD_FREQUENCY = 440.0f;
        static const float DEFAULT_RINGMOD_MIX = 0.5f;
        static const int DEFAULT_RINGMOD_WAVEFORM = 0;
        static const float DEFAULT_BITCRUSHER_BITS = 8.0f;
        static const float DEFAULT_BITCRUSHER_RATE = 0.5f;
        static const float DEFAULT_BITCRUSHER_MIX = 1.0f;
        static const float DEFAULT_WAVESHAPER_AMOUNT = 0.5f;
        static const int DEFAULT_WAVESHAPER_TYPE = 0;
        static const float DEFAULT_WAVESHAPER_MIX = 0.5f;
        static const float DEFAULT_EXCITER_AMOUNT = 0.3f;
        static const float DEFAULT_EXCITER_FREQUENCY = 3000.0f;
        static const float DEFAULT_EXCITER_MIX = 0.2f;
        static const float DEFAULT_ENHANCER_BASS = 0.0f;
        static const float DEFAULT_ENHANCER_TREBLE = 0.0f;
        static const float DEFAULT_ENHANCER_PRESENCE = 0.0f;
        static const float DEFAULT_ENHANCER_WARMTH = 0.0f;
        static const float DEFAULT_TRANSIENT_ATTACK = 0.0f;
        static const float DEFAULT_TRANSIENT_SUSTAIN = 0.0f;
        static const float DEFAULT_TRANSIENT_SENSITIVITY = 0.5f;
        static const int DEFAULT_PITCH_SHIFT_WINDOW = 2048;
        static const float DEFAULT_PITCH_SHIFT_OVERLAP = 0.75f;
        static const float DEFAULT_TIME_STRETCH_FACTOR = 1.0f;
        static const int DEFAULT_TIME_STRETCH_WINDOW = 2048;
        static const float DEFAULT_FORMANT_SHIFT = 0.0f;
        static const int DEFAULT_FORMANT_ORDER = 50;
        static const float DEFAULT_VOCODER_BANDS = 16.0f;
        static const float DEFAULT_VOCODER_BANDWIDTH = 0.5f;
        static const float DEFAULT_VOCODER_ENVELOPE = 0.1f;
        static const float DEFAULT_TALKBOX_RESONANCE = 0.9f;
        static const float DEFAULT_TALKBOX_DAMPING = 0.1f;
        static const int DEFAULT_TALKBOX_CARRIERS = 64;

        // ========================================================================
        // ENHANCED DATABASE SYSTEM DEFAULTS
        // ========================================================================
        // These defaults support the comprehensive database structure with metadata,
        // indexing, caching, and professional features.

        // ========================================================================
        // SYSTEM CONFIGURATION DEFAULTS
        // ========================================================================

        /** @brief Default application version string */
        static const juce::String DEFAULT_APPLICATION_VERSION = "8.0.1";

        /** @brief Default build number */
        static const juce::String DEFAULT_BUILD_NUMBER = "2024080801";

        /** @brief Default crash reports enabled state */
        static const bool DEFAULT_CRASH_REPORTS_ENABLED = true;

        /** @brief Default auto-update enabled state */
        static const bool DEFAULT_AUTO_UPDATE_ENABLED = true;

        /** @brief Default data path location */
        static const juce::String DEFAULT_DATA_PATH = "%DOCUMENTS%/DrumMachine/Data";

        // ========================================================================
        // ENHANCED PLAYER DEFAULTS
        // ========================================================================

        /** @brief Default player ID format */
        static const juce::String DEFAULT_PLAYER_ID_FORMAT = "001";

        /** @brief Default player name format */
        static const juce::String DEFAULT_PLAYER_NAME = "Main Kit";

        /** @brief Default selected flag state */
        static const bool DEFAULT_SELECTED_FLAG = true;

        /** @brief Default solo state */
        static const bool DEFAULT_SOLO_STATE = false;

        /** @brief Default mute state */
        static const bool DEFAULT_MUTE_STATE = false;

        /** @brief Default drumkit ID */
        static const juce::String DEFAULT_DRUMKIT_ID = "KIT_001";

        /** @brief Default pattern group ID */
        static const juce::String DEFAULT_PATTERN_GROUP_ID = "PG_001";

        /** @brief Default current pattern ID */
        static const juce::String DEFAULT_CURRENT_PATTERN_ID = "PAT_001";

        /** @brief Default pan position (center) */
        static const float DEFAULT_PAN_POSITION = 0.0f;

        /** @brief Default swing value (16th note swing) */
        static const int DEFAULT_SWING_VALUE = 16;

        /** @brief Default swing amount percentage */
        static const float DEFAULT_SWING_AMOUNT = 0.15f;

        /** @brief Default energy value (0-1 range) */
        static const float DEFAULT_ENERGY_VALUE = 0.75f;

        /** @brief Default velocity offset */
        static const int DEFAULT_VELOCITY_OFFSET = 0;

        /** @brief Default pitch offset */
        static const int DEFAULT_PITCH_OFFSET = 0;

        /** @brief Default MIDI channel (channel 10 for drums) */
        static const int DEFAULT_MIDI_CHANNEL = 10;

        /** @brief Default MIDI input enabled state */
        static const bool DEFAULT_MIDI_INPUT_ENABLED = true;

        /** @brief Default audio output bus */
        static const juce::String DEFAULT_AUDIO_OUTPUT = "Main_Bus";

        /** @brief Default reverb send level */
        static const float DEFAULT_SEND_TO_REVERB = 0.25f;

        /** @brief Default delay send level */
        static const float DEFAULT_SEND_TO_DELAY = 0.10f;

        /** @brief Default effect chain ID */
        static const juce::String DEFAULT_EFFECT_CHAIN_ID = "EC_001";

        /** @brief Default MIDI note filter range */
        static const juce::String DEFAULT_MIDI_NOTE_FILTER = "36-81";

        /** @brief Default velocity curve type */
        static const juce::String DEFAULT_VELOCITY_CURVE = "Linear";

        /** @brief Default humanize amount */
        static const float DEFAULT_HUMANIZE_AMOUNT = 0.05f;

        // ========================================================================
        // ENHANCED DRUMKIT DEFAULTS
        // ========================================================================

        /** @brief Default drumkit category */
        static const juce::String DEFAULT_DRUMKIT_CATEGORY = "Vintage";

        /** @brief Default drumkit subcategory */
        static const juce::String DEFAULT_DRUMKIT_SUBCATEGORY = "Rock";

        /** @brief Default drumkit manufacturer */
        static const juce::String DEFAULT_DRUMKIT_MANUFACTURER = "Classic Drums Co";

        /** @brief Default drumkit description */
        static const juce::String DEFAULT_DRUMKIT_DESCRIPTION = "Warm vintage rock sound with natural room ambience";

        /** @brief Default drumkit author */
        static const juce::String DEFAULT_DRUMKIT_AUTHOR = "Studio Engineer";

        /** @brief Default drumkit version */
        static const juce::String DEFAULT_DRUMKIT_VERSION = "2.1";

        /** @brief Default sample count */
        static const int DEFAULT_SAMPLE_COUNT = 128;

        /** @brief Default velocity layers */
        static const int DEFAULT_VELOCITY_LAYERS = 8;

        /** @brief Default round-robin samples */
        static const int DEFAULT_ROUND_ROBIN_SAMPLES = 4;

        /** @brief Default BPM optimization */
        static const int DEFAULT_BPM_OPTIMIZED = 120;

        /** @brief Default drumkit tags */
        static const juce::String DEFAULT_DRUMKIT_TAGS = "vintage,rock,natural,warm,70s";

        /** @brief Default drumkit rating (0-5 stars) */
        static const float DEFAULT_DRUMKIT_RATING = 4.5f;

        /** @brief Default play count */
        static const int DEFAULT_PLAY_COUNT = 0;

        /** @brief Default favorites state */
        static const bool DEFAULT_FAVORITES_STATE = false;

        // ========================================================================
        // ENHANCED PATTERN DEFAULTS
        // ========================================================================

        /** @brief Default pattern category */
        static const juce::String DEFAULT_PATTERN_CATEGORY = "Basic";

        /** @brief Default custom pattern flag */
        static const bool DEFAULT_IS_CUSTOM_PATTERN = false;

        /** @brief Default template pattern flag */
        static const bool DEFAULT_IS_TEMPLATE_PATTERN = false;

        /** @brief Default favorite pattern flag */
        static const bool DEFAULT_IS_FAVORITE_PATTERN = true;

        /** @brief Default pattern creator */
        static const juce::String DEFAULT_PATTERN_CREATED_BY = "System";

        /** @brief Default time signature numerator */
        static const int DEFAULT_TIME_SIGNATURE_NUM = 4;

        /** @brief Default time signature denominator */
        static const int DEFAULT_TIME_SIGNATURE_DEN = 4;

        /** @brief Default BPM range minimum */
        static const int DEFAULT_BPM_RANGE_MIN = 100;

        /** @brief Default BPM range maximum */
        static const int DEFAULT_BPM_RANGE_MAX = 140;

        /** @brief Default groove tightness */
        static const float DEFAULT_GROOVE_TIGHTNESS = 0.85f;

        /** @brief Default quantization setting */
        static const juce::String DEFAULT_QUANTIZATION = "16th";

        /** @brief Default pattern length */
        static const juce::String DEFAULT_PATTERN_LENGTH = "4_bars";

        /** @brief Default average velocity */
        static const int DEFAULT_AVERAGE_VELOCITY = 85;

        /** @brief Default velocity range minimum */
        static const int DEFAULT_VELOCITY_RANGE_MIN = 45;

        /** @brief Default velocity range maximum */
        static const int DEFAULT_VELOCITY_RANGE_MAX = 120;

        /** @brief Default pattern complexity (1-10) */
        static const int DEFAULT_PATTERN_COMPLEXITY = 3;

        /** @brief Default pattern energy (1-10) */
        static const int DEFAULT_PATTERN_ENERGY = 7;

        /** @brief Default pattern density */
        static const juce::String DEFAULT_PATTERN_DENSITY = "moderate";

        /** @brief Default pattern style */
        static const juce::String DEFAULT_PATTERN_STYLE = "rock";

        /** @brief Default pattern tags */
        static const juce::String DEFAULT_PATTERN_TAGS = "basic,rock,4/4,steady";

        /** @brief Default MIDI note count */
        static const int DEFAULT_MIDI_NOTE_COUNT = 64;

        /** @brief Default controller data flag */
        static const bool DEFAULT_CONTROLLER_DATA = false;

        // ========================================================================
        // MIXER AND EFFECTS DEFAULTS
        // ========================================================================

        /** @brief Default mixer snapshot category */
        static const juce::String DEFAULT_MIXER_SNAPSHOT_CATEGORY = "Performance";

        /** @brief Default mixer snapshot description */
        static const juce::String DEFAULT_MIXER_SNAPSHOT_DESCRIPTION = "Optimized for live performance with crowd noise";

        /** @brief Default EQ preset category */
        static const juce::String DEFAULT_EQ_PRESET_CATEGORY = "Kick";

        /** @brief Default EQ type */
        static const juce::String DEFAULT_EQ_TYPE = "Parametric";

        /** @brief Default EQ band 1 frequency */
        static const float DEFAULT_EQ_BAND1_FREQ = 60.0f;

        /** @brief Default EQ band 1 gain */
        static const float DEFAULT_EQ_BAND1_GAIN = 3.0f;

        /** @brief Default EQ band 1 Q factor */
        static const float DEFAULT_EQ_BAND1_Q = 1.2f;

        /** @brief Default EQ band 2 frequency */
        static const float DEFAULT_EQ_BAND2_FREQ = 2500.0f;

        /** @brief Default EQ band 2 gain */
        static const float DEFAULT_EQ_BAND2_GAIN = -2.5f;

        /** @brief Default EQ band 2 Q factor */
        static const float DEFAULT_EQ_BAND2_Q = 0.8f;

        /** @brief Default EQ band 3 frequency */
        static const float DEFAULT_EQ_BAND3_FREQ = 8000.0f;

        /** @brief Default EQ band 3 gain */
        static const float DEFAULT_EQ_BAND3_GAIN = 1.8f;

        /** @brief Default EQ band 3 Q factor */
        static const float DEFAULT_EQ_BAND3_Q = 1.5f;

        // ========================================================================
        // PERFORMANCE AND CACHING DEFAULTS
        // ========================================================================

        /** @brief Default maximum voices for polyphony */
        static const int DEFAULT_MAX_VOICES_ENHANCED = 128;

        /** @brief Default CPU usage limit percentage */
        static const int DEFAULT_CPU_LIMIT = 75;

        /** @brief Default thread count (auto-detect) */
        static const juce::String DEFAULT_THREAD_COUNT_AUTO = "auto";

        /** @brief Default preload samples enabled */
        static const bool DEFAULT_PRELOAD_SAMPLES = true;

        /** @brief Default stream from disk enabled */
        static const bool DEFAULT_STREAM_FROM_DISK = true;

        /** @brief Default cache size in MB */
        static const int DEFAULT_CACHE_SIZE_MB = 256;

        /** @brief Default MIDI clock source */
        static const juce::String DEFAULT_MIDI_CLOCK_SOURCE = "Internal";

        /** @brief Default tempo source */
        static const juce::String DEFAULT_TEMPO_SOURCE = "Internal";

        /** @brief Default MIDI sync enabled */
        static const bool DEFAULT_MIDI_SYNC_ENABLED = false;

        /** @brief Default MIDI thru enabled */
        static const bool DEFAULT_MIDI_THRU_ENABLED = true;

        /** @brief Default velocity for new notes */
        static const int DEFAULT_NEW_NOTE_VELOCITY = 100;

        /** @brief Default note-on delay in milliseconds */
        static const int DEFAULT_NOTE_ON_DELAY = 0;

        /** @brief Default note-off delay in milliseconds */
        static const int DEFAULT_NOTE_OFF_DELAY = 0;

        // ========================================================================
        // METADATA AND INDEXING DEFAULTS
        // ========================================================================

        /** @brief Default file size format */
        static const juce::String DEFAULT_FILE_SIZE_FORMAT = "245MB";

        /** @brief Default thumbnail path format */
        static const juce::String DEFAULT_THUMBNAIL_PATH = "Images/Kits/";

        /** @brief Default samples path format */
        static const juce::String DEFAULT_SAMPLES_PATH = "Samples/";

        /** @brief Default SFZ path format */
        static const juce::String DEFAULT_SFZ_PATH = "Kits/";

        /** @brief Default date format */
        static const juce::String DEFAULT_DATE_FORMAT = "2024-07-15";

        /** @brief Default ISO date format */
        static const juce::String DEFAULT_ISO_DATE_FORMAT = "2024-08-08 12:30:45";

        /** @brief Default MIDI file size in bytes */
        static const int DEFAULT_MIDI_FILE_SIZE = 2048;

        /** @brief Default search index enabled */
        static const bool DEFAULT_SEARCH_INDEX_ENABLED = true;

        /** @brief Default backup enabled */
        static const bool DEFAULT_BACKUP_ENABLED = true;

        /** @brief Default backup interval in minutes */
        static const int DEFAULT_BACKUP_INTERVAL = 30;

        /** @brief Default maximum backup files to keep */
        static const int DEFAULT_MAX_BACKUP_FILES = 10;
    } // end namespace Defaults

/**
 * @namespace LayoutConstants
 * @brief Percentage-based responsive layout system for OTTO's interface components
 *
 * This namespace implements a comprehensive responsive layout system that scales all
 * interface elements proportionally based on the base resolution (1024x768). Every
 * dimension is calculated as a percentage of the default interface size, ensuring
 * consistent appearance across different screen sizes and DPI settings.
 *
 * DESIGN PRINCIPLES:
 * ==================
 * - Percentage-based scaling: All dimensions calculated from DEFAULT_INTERFACE_WIDTH/HEIGHT
 * - Accessibility compliance: Enforces minimum 44px touch targets (MIN_TOUCH_TARGET_PX)
 * - Row-based organization: Interface divided into 6 logical rows with specific purposes
 * - Responsive breakpoints: Automatic scaling with fallback compact modes
 * - Cross-reference integration: Used by ResponsiveLayoutManager.cpp for live scaling
 *
 * ROW ORGANIZATION:
 * =================
 * - Row 1 (10%): Row1Component - Settings, presets, transport controls
 * - Row 2 (8%):  PlayerTabsComponent - Player selection tabs
 * - Row 3 (16%): DrumKit controls - Edit, navigation, kit selection
 * - Row 4 (14%): Pattern management - Groups, labels, status
 * - Row 5 (44%): Main content area - Drum grid, sliders, controls
 * - Row 6 (8%):  Loop/transport section - Scene launcher, performance controls
 *
 * USAGE IN COMPONENTS:
 * ====================
 * - MainContentComponent.cpp: Uses row heights and positioning
 * - Row1Component.cpp: References Row1 namespace constants
 * - PlayerTabsComponent.cpp: Uses Row2 namespace for tab dimensions
 * - ResponsiveLayoutManager.cpp: Applies scaling factors to all constants
 * - CustomLookAndFeel.cpp: Uses spacing and sizing constants for consistency
 *
 * @see ResponsiveLayoutManager.cpp for dynamic scaling implementation
 * @see MainContentComponent.cpp for row-based layout application
 */
namespace LayoutConstants {

    // ============================================================================
    // FUNDAMENTAL SPACING CONSTANTS
    // ============================================================================
    // These base spacing values are used throughout the interface for consistency.
    // All values are calculated as percentages of the default interface width.

    /**
     * @brief Standard margin for component edges (1.67% of interface width = ~17px at 1024w)
     * Used by: All major layout components for outer boundaries
     */
    constexpr int defaultMargin = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0167f);

    /**
     * @brief Internal padding for component content (0.83% of interface width = ~8px at 1024w)
     * Used by: Component interiors, text padding, small element spacing
     */
    constexpr int defaultPadding = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0083f);

    /**
     * @brief General spacing between related elements (1.25% of interface width = ~13px at 1024w)
     *
     * DESIGN NOTE: Updated from 0.01% to 1.25% for better visual separation while
     * maintaining compact layout. This spacing provides clear element boundaries
     * without wasting precious screen real estate.
     *
     * Used by: Button groups, slider arrangements, component separation
     */
    constexpr int defaultSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0125f);

    /**
     * @brief Standard thickness for separator lines and borders (3px fixed)
     * Used by: Row separators, component borders, visual dividers
     */
    constexpr int separatorThickness = 3;

    constexpr int mainHeaderHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.075f);
    constexpr int rhythmLabelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);
    constexpr int rhythmLabelHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.05f);
    constexpr int rhythmLabelX = defaultMargin;
    constexpr int rhythmLabelY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0125f);
    constexpr int playerNumberWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0333f);
    constexpr int playerNumberX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.1f);
    constexpr int playerNumberY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0125f);

    constexpr int topSectionHeightRatio = 5;

    // RESPONSIVE ICON SIZE: 95% of Row 1 height for proper topbar filling
    // This scales with ResponsiveLayoutManager to maintain proportion across all screen sizes
    constexpr int iconButtonSize = static_cast<int>((Defaults::DEFAULT_INTERFACE_HEIGHT * 0.10f) * 0.95f);

    // RESPONSIVE ICON SPACING: Proportional spacing that scales with interface size
    // 2% of screen width provides balanced spacing that maintains proportion at all scales
    constexpr int topBarIconSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.02f);

    constexpr int editButtonSize = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0317f);
    constexpr int editButtonX = defaultMargin;
    constexpr int editButtonY = (mainHeaderHeight - editButtonSize) / 2;

    constexpr int chevronButtonWidth = iconButtonSize;
    constexpr int chevronButtonHeight = iconButtonSize;
    constexpr int leftChevronX = editButtonX + editButtonSize + defaultSpacing;
    constexpr int leftChevronY = (mainHeaderHeight - chevronButtonHeight) / 2;
    constexpr int rightChevronX = leftChevronX + chevronButtonWidth + defaultSpacing;
    constexpr int rightChevronY = leftChevronY;

    constexpr int favoriteButtonSize = iconButtonSize;
    constexpr int favoriteButtonX = rightChevronX + chevronButtonWidth + defaultSpacing;
    constexpr int favoriteButtonY = (mainHeaderHeight - favoriteButtonSize) / 2;

    constexpr int midiGroupMenuX = favoriteButtonX + favoriteButtonSize + defaultSpacing;
    constexpr int midiGroupMenuY = static_cast<int>((mainHeaderHeight - static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f)) / 2);
    constexpr int midiGroupMenuWidthOffset = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.167f);

    constexpr int drumButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0667f);
    constexpr int drumButtonHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0328f);
    constexpr int drumButtonGridStartX = defaultMargin;
    constexpr int drumButtonGridStartY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0125f);
    constexpr int drumButtonGridSpacingX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);
    constexpr int drumButtonGridSpacingY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.05f);
    constexpr int drumButtonsPerRow = 4;
    constexpr int drumButtonRows = 4;

    constexpr int rightSectionLabelY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0125f);
    constexpr int rightSectionLabelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0667f);
    constexpr int rightSectionLabelHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0313f);
    constexpr int togglesLabelX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0192f);
    constexpr int fillsLabelX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.1f);
    constexpr int swingLabelX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.1833f);
    constexpr int energyLabelX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.25f);
    constexpr int volumeLabelX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.3167f);
    constexpr int volumeLabelWidthExtra = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0167f);

    constexpr int rightSectionButtonY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0125f);
    constexpr int rightSectionButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0667f);
    constexpr int rightSectionButtonHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0438f);
    constexpr int rightSectionButtonSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0333f);

    constexpr int sliderWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.05f);
    constexpr int sliderHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.25f);
    constexpr int swingSliderX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.1833f);
    constexpr int energySliderX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.25f);
    constexpr int volumeSliderX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.3167f);

    constexpr int popupWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.25f);
    constexpr int popupSliderHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
    constexpr int popupSliderTextBoxWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0417f);
    constexpr int popupSliderTextBoxHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.025f);

    constexpr int topBarIconY = (mainHeaderHeight - iconButtonSize) / 2;
    constexpr int topBarGearX = defaultMargin;
    constexpr int topBarLinkX = topBarGearX + iconButtonSize + topBarIconSpacing;
    constexpr int topBarCloudX = topBarLinkX + iconButtonSize + topBarIconSpacing;
    constexpr int topBarLeftChevronX = topBarCloudX + iconButtonSize + topBarIconSpacing;
    constexpr int topBarChevronSize = iconButtonSize;
    constexpr int topBarPresetY = static_cast<int>((mainHeaderHeight - static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f)) / 2);
    constexpr int topBarPresetsMenuX = topBarLeftChevronX + topBarChevronSize + topBarIconSpacing;
    constexpr int topBarPresetsMenuY = topBarPresetY;
    constexpr int topBarPresetsMenuWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.2f);
    constexpr int topBarRightChevronX = topBarPresetsMenuX + topBarPresetsMenuWidth + topBarIconSpacing;

    constexpr int topBarPlayX = (Defaults::DEFAULT_INTERFACE_WIDTH - iconButtonSize) / 2;

    constexpr int topBarBpmX = topBarPlayX + iconButtonSize + topBarIconSpacing;
    constexpr int topBarBpmY = static_cast<int>((mainHeaderHeight - static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f)) / 2);
    constexpr int topBarBpmWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.11f);
    constexpr int topBarBpmHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
    constexpr int topBarClockSyncY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.05f);
    constexpr int topBarClockSyncHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.01875f);
    constexpr int topBarTapTempoX = topBarBpmX + topBarBpmWidth + topBarIconSpacing;
    constexpr int topBarRecordX = topBarTapTempoX + iconButtonSize + topBarIconSpacing;
    constexpr int topBarOverdubX = topBarRecordX + iconButtonSize + topBarIconSpacing;
    constexpr int topBarLoopX = topBarOverdubX + iconButtonSize + topBarIconSpacing;

    constexpr int topBarOttoWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);
    constexpr int topBarOttoX = Defaults::DEFAULT_INTERFACE_WIDTH - topBarOttoWidth - defaultSpacing;
    constexpr int topBarOttoY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.00625f);
    constexpr int topBarOttoHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
    constexpr int topBarVersionY = topBarOttoY + topBarOttoHeight;
    constexpr int topBarVersionHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.01875f);

    constexpr int dialogContentMargin = defaultMargin;
    constexpr int dialogRowHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
    constexpr int dialogRowSpacing = defaultSpacing;
    constexpr int dialogButtonAreaHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.05f);
    constexpr int dialogButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);
    constexpr int dialogButtonSpacing = defaultSpacing;

    constexpr int authDialogWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.2917f);
    constexpr int authDialogHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.25f);

    constexpr int shareDialogWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.3333f);
    constexpr int shareDialogHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.4375f);
    constexpr int shareDialogLabelHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.025f);
    constexpr int shareDialogDescHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.1f);

    constexpr int collabDialogWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.2917f);
    constexpr int collabDialogHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.1875f);

    constexpr int sessionCodeLength = 6;

    constexpr int shareCodeLength = 8;
    constexpr int shareCodeChars = 36;

    constexpr int backupProgressWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.25f);
    constexpr int backupProgressHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.125f);
    constexpr int backupProgressBarMargin = defaultMargin;
    constexpr int backupProgressBarY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.05f);
    constexpr int backupProgressTimerMs = 50;
    constexpr double backupProgressIncrement = 0.05;

    constexpr int tapTempoTimeoutMs = 3000;
    constexpr int tapTempoDisplayMs = 1000;

    constexpr int sceneLauncherPadding = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0042f);
    constexpr int sceneLauncherTitleHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
    constexpr int sceneLauncherButtonHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.05f);
    constexpr int sceneLauncherButtonSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0042f);
    constexpr int sceneLauncherBottomMargin = defaultSpacing;
    constexpr int sceneLauncherBorderThickness = 1;
    constexpr float sceneLauncherButtonCornerRadius = 4.0f;
    constexpr float sceneLauncherButtonBorderWidth = 2.0f;
    constexpr float sceneLauncherInactiveAlpha = 0.5f;
    constexpr float sceneLauncherBorderReduction = 0.5f;
    constexpr int sceneLauncherTimerHz = 10;
    constexpr int numScenes = 8;

    constexpr int playerTabsCount = 8;
    constexpr int playerTabWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0917f);
    constexpr int playerTabHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.00625f);
    constexpr int playerTabHighlightHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0025f);
    constexpr int playerTabHighlightMargin = defaultSpacing;
    constexpr int playerTabHighlightWidthReduction = defaultMargin;
    constexpr int playerTabTopOffset = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0025f);
    constexpr float playerTabDisabledAlpha = 0.5f;

    constexpr int drumKitSectionMargin = defaultSpacing;
    constexpr int drumKitLabelHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.025f);
    constexpr int drumKitComboHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0313f);
    constexpr int drumKitButtonHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
    constexpr int drumKitSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0042f);
    constexpr int drumKitPowerButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.05f);
    constexpr int drumKitButtonColumns = 4;
    constexpr int drumKitSectionBorderThickness = 1;
    constexpr int drumKitDoubleSpacing = defaultSpacing;

    constexpr int settingsPanelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.6667f);
    constexpr int settingsPanelHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.75f);
    constexpr int settingsPanelMargin = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0125f);
    constexpr int settingsPanelTitleHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0438f);
    constexpr int settingsPanelCloseButtonSize = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.03f);
    constexpr int settingsPanelCloseButtonX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0375f);
    constexpr int settingsPanelCloseButtonY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0125f);
    constexpr int settingsPanelCornerRadius = 8;

    constexpr int audioTabRowHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0438f);
    constexpr int audioTabLabelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.1f);
    constexpr int audioTabComboWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.125f);
    constexpr int audioTabButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.1f);
    constexpr int audioTabButtonSize = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0313f);

    constexpr int midiTabListHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.125f);
    constexpr int midiTabButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.075f);
    constexpr int midiTabColumnWidths[] = {
        static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.1667f),
        static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.05f),
        static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0667f),
        static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f),
        static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0583f)
    };
    constexpr int midiTabCheckboxSize = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0133f);
    constexpr int midiTabCheckboxReduction = 3;

    constexpr int themeTabComboWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.1667f);
    constexpr int themeTabButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.075f);
    constexpr int themeTabButtonSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0042f);
    constexpr int themeTabRowHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0438f);
    constexpr int themeTabLabelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.125f);
    constexpr int themeTabColorButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.05f);
    constexpr int themeTabColorButtonReduction = 5;
    constexpr int themeTabPreviewHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.125f);
    constexpr int themeTabPreviewButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);
    constexpr int themeTabColorSelectorWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.25f);
    constexpr int themeTabColorSelectorHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.5f);

    constexpr int perfTabValueWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0667f);
    constexpr int perfTabMeterWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.1667f);
    constexpr int perfTabSliderWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.2083f);
    constexpr int perfTabSliderExtraHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.025f);
    constexpr int perfTabButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.1f);
    constexpr int perfTabQualityLabelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0417f);
    constexpr int perfTabQualityLabelHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.025f);
    constexpr int perfTabQualityLabelY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0025f);
    constexpr int perfTabQualityMidOffset = defaultMargin;
    constexpr int perfTabTimerHz = 10;

    constexpr int backupTabButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0917f);
    constexpr int backupTabListBottomMargin = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.15f);
    constexpr int backupTabTimeLabelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.125f);
    constexpr int backupTabSizeLabelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.1083f);
    constexpr int backupTabLabelMargin = defaultSpacing;
    constexpr int backupTabFilenameHeight = 2;
    constexpr int backupTabFontSizeSmall = 12;

    constexpr int mixerWindowDefaultWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.75f);
    constexpr int mixerWindowDefaultHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.75f);
    constexpr int mixerWindowMinWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.3333f);
    constexpr int mixerWindowMinHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.375f);
    constexpr int mixerChannelStripWidth = 8;
    constexpr int mixerChannelStripMargin = 2;
    constexpr int mixerMasterSectionWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);
    constexpr int mixerMasterSectionMargin = 5;
    constexpr int mixerTopMargin = defaultSpacing;
    constexpr int mixerTimerHz = 50;

    constexpr int mixerStripCornerRadius = 4;
    constexpr int mixerStripTitleHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.025f);
    constexpr int mixerStripMeterWidth = 12;
    constexpr int mixerStripMeterMargin = 2;
    constexpr int mixerStripEQSection = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.1125f);
    constexpr int mixerStripEQKnobSize = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0233f);
    constexpr int mixerStripEQSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.025f);
    constexpr int mixerStripEQYOffset = 5;
    constexpr int mixerStripEQRow2Offset = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0438f);
    constexpr int mixerStripSendYOffset = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0875f);
    constexpr int mixerStripButtonHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0313f);
    constexpr int mixerStripButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0208f);
    constexpr int mixerStripButtonSpacing = 5;
    constexpr int mixerStripPanKnobSize = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0417f);
    constexpr int mixerStripPanKnobHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.05f);
    constexpr int mixerStripFaderTopMargin = 5;
    constexpr int mixerStripSectionSpacing = defaultSpacing;
    constexpr int mixerStripControlsMargin = 4;
    constexpr int mixerStripPeakHoldCount = 100;
    constexpr float mixerStripPeakDecay = 0.95f;
    constexpr float mixerStripMeterMinDb = -60.0f;
    constexpr float mixerStripMeterMaxDb = 0.0f;
    constexpr float mixerStripMeterClipDb = 3.0f;
    constexpr float mixerStripMeterDbStep = 6.0f;
    constexpr int mixerStripMeterTickLength = 2;
    constexpr int mixerStripTitleFontSize = 12;

    constexpr int mixerMasterTitleHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0313f);
    constexpr int mixerMasterTitleFontSize = 14;
    constexpr int mixerMasterMeterWidth = 15;
    constexpr int mixerMasterMeterMargin = 2;
    constexpr int mixerMasterControlsMargin = defaultSpacing;
    constexpr int mixerMasterLimiterHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.075f);
    constexpr int mixerMasterLimiterButtonHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0313f);
    constexpr int mixerMasterLimiterSpacing = 5;
    constexpr int mixerMasterFaderTopMargin = defaultMargin;

    constexpr int editorWindowDefaultX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);
    constexpr int editorWindowDefaultY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.125f);
    constexpr int editorWindowDefaultWidth = Defaults::DEFAULT_INTERFACE_WIDTH;
    constexpr int editorWindowDefaultHeight = Defaults::DEFAULT_INTERFACE_HEIGHT;
    constexpr int editorWindowMinWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.3333f);
    constexpr int editorWindowMinHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.375f);

    constexpr int grooveWindowDefaultX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);
    constexpr int grooveWindowDefaultY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.125f);
    constexpr int grooveWindowDefaultWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.6667f);
    constexpr int grooveWindowDefaultHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.75f);
    constexpr int grooveWindowMinWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.3333f);
    constexpr int grooveWindowMinHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.375f);

    constexpr int animatedComponentTimerHz = 60;
    constexpr int animatedComponentDefaultDuration = 300;
    constexpr float animatedComponentQuadraticFactor = 2.0f;
    constexpr float animatedComponentProgressThreshold = 0.5f;
    constexpr float animatedComponentProgressPower = 2.0f;

    constexpr int phosphorIconButtonDefaultSize = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0333f);
    constexpr float phosphorIconButtonHoverScale = 0.05f;
    constexpr float phosphorIconButtonCornerRadius = 4.0f;
    // EUREKA MOMENT: Icons should be configurable % of their BOX area, not responsive-scaled
    // The box already handles responsive scaling, so icons just need to fit nicely within that defined area
    constexpr float phosphorIconButtonBoxFitRatio = Defaults::DEFAULT_PHOSPHOR_ICON_BOX_FIT_RATIO;
    // No artificial font size limits needed - box dimensions provide natural scaling boundaries
    constexpr float phosphorIconButtonAlphaBase = 0.8f;
    constexpr float phosphorIconButtonAlphaHover = 0.2f;
    constexpr float phosphorIconButtonAnimSpeed = 0.15f;
    constexpr float phosphorIconButtonAnimThreshold = 0.01f;

    constexpr int editableNumericMaxLength = 6;
    constexpr int editableNumericDecimalPlaces = 1;

    constexpr int separatorComponentDefaultWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);
    constexpr float separatorComponentDefaultThickness = 3.0f;

    constexpr int meterComponentTimerHz = 30;
    constexpr float meterComponentPadding = 4.0f;
    constexpr float meterComponentBarSpacing = 2.0f;
    constexpr float meterComponentCornerRadius = 4.0f;
    constexpr float meterComponentBarCornerRadius = 2.0f;
    constexpr float meterComponentPeakHeight = 2.0f;
    constexpr float meterComponentDecay = 0.9f;
    constexpr float meterComponentAttack = 0.95f;
    constexpr float meterComponentPeakDecay = 0.95f;
    constexpr int meterComponentPeakHoldDuration = 100;
    constexpr float meterComponentGreenThreshold = 0.6f;

    constexpr float effectButtonCornerRadius = 8.0f;
    constexpr float effectButtonBorderWidth = 1.0f;
    constexpr float effectButtonBorderReduction = 0.5f;
    constexpr float effectButtonDarkenFactor = 0.3f;
    constexpr float effectButtonBypassAlpha = 0.5f;
    constexpr float effectButtonBypassLineWidth = 2.0f;
    constexpr int effectButtonBypassLineMargin = 5;
    constexpr float effectButtonFontSize = 14.0f;

    constexpr int keyboardOverlayReduction = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);
    constexpr float keyboardOverlayAlpha = 0.95f;
    constexpr float keyboardOverlayCornerRadius = 10.0f;
    constexpr float keyboardOverlayBorderWidth = 2.0f;
    constexpr float keyboardOverlayBorderReduction = 1.0f;
    constexpr int keyboardOverlayTitleHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0625f);
    constexpr float keyboardOverlayTitleFontSize = 20.0f;
    constexpr int keyboardOverlayStartY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.075f);
    constexpr int keyboardOverlayLineHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
    constexpr float keyboardOverlayBodyFontSize = 14.0f;
    constexpr int keyboardOverlayKeyX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0417f);
    constexpr int keyboardOverlayKeyWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.125f);
    constexpr int keyboardOverlayDescX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.1833f);
    constexpr int keyboardOverlayDescWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.3333f);
    constexpr int keyboardOverlayFadeDuration = 300;

    constexpr float beveledRectDefaultSize = 1.0f;
    constexpr float beveledRectLightBrightness = 0.3f;
    constexpr float beveledRectDarkBrightness = 0.3f;

    constexpr int waveformThumbnailCache = 512;
    constexpr int waveformStartEndMargin = 1;
    constexpr float waveformLineAlpha = 0.3f;
    constexpr float waveformPositionTolerance = 0.02f;
    constexpr float waveformMinSelectionSize = 0.01f;

    constexpr int velocityEditorNumPoints = 128;
    constexpr float velocityEditorLineWidth = 2.0f;


    constexpr int padEditorTextSize = 16;
    constexpr float padEditorSmallTextSize = 12.0f;

    constexpr int customLookFeelCornerRadius = 6;
    constexpr int customLookFeelShadowRadius = 8;
    constexpr int customLookFeelShadowOffsetX = 2;
    constexpr int customLookFeelShadowOffsetY = 2;
    constexpr float customLookFeelShadowAlpha = 0.3f;
    constexpr int customLookFeelTooltipMaxWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.3333f);
    constexpr int customLookFeelTooltipMargin = 16;
    constexpr float customLookFeelTooltipTextMargin = 8.0f;
    constexpr float customLookFeelTooltipLineSpacing = 3.0f;
    constexpr int customLookFeelTooltipOffsetX = 10;
    constexpr int customLookFeelTooltipOffsetY = 20;
    constexpr float customLookFeelTrackThickness = 8.0f;
    constexpr float customLookFeelThumbSize = 20.0f;
    constexpr float customLookFeelSliderCornerRadius = 4.0f;
    constexpr int customLookFeelButtonImageSlices = 6;
    constexpr int customLookFeelSliderImageSlices = 100;
    constexpr float customLookFeelRotaryThickness = 3.0f;
    constexpr float customLookFeelRotaryDotSize = 8.0f;
    constexpr float customLookFeelRotaryPointerLength = 0.6f;
    constexpr float customLookFeelRotaryInnerRadius = 0.8f;
    constexpr float customLookFeelDarkerAmount = 0.1f;
    constexpr float customLookFeelBrighterAmount = 0.2f;
    constexpr float customLookFeelGradientDarkness = 0.2f;
    constexpr int customLookFeelTextReduction = 2;
    constexpr int customLookFeelComboBoxMargin = 4;
   constexpr int customLookFeelButtonTextMargin = 2;
   constexpr float customLookFeelFontScaleMin = 12.0f;
   constexpr float customLookFeelFontScaleMax = 18.0f;
   constexpr float customLookFeelFontScaleFactor = 0.4f;
   constexpr float customLookFeelTextScaleFactor = 0.9f;
   constexpr float customLookFeelLargeTextMultiplier = 1.2f;
   constexpr float customLookFeelBorderThickness = 1.0f;
   constexpr float customLookFeelSeparatorMargin = 5.0f;
   constexpr float customLookFeelBorderReduction = 0.5f;
   constexpr float customLookFeelHighContrastBorderWidth = 2.0f;
   constexpr float customLookFeelThumbDivisor = 2.0f;

   constexpr int drumKitEditorPadColumns = 4;
   constexpr int drumKitEditorPadRows = 4;
   constexpr int drumKitEditorPadCount = 16;
   constexpr int drumKitEditorBrowserWidthRatio = 3;
   constexpr int drumKitEditorWaveformHeightRatio = 5;
   constexpr int drumKitEditorControlsWidthRatio = 3;
   constexpr int drumKitEditorPadTextMargin = 5;
   constexpr int drumKitEditorChokeGroupCount = 16;

   constexpr int customGroupPopupWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.3333f);
   constexpr int customGroupPopupHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.3125f);
   constexpr int customGroupCloseButtonSize = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.03f);
   constexpr int customGroupCloseButtonX = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0375f);
   constexpr int customGroupCloseButtonY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.01f);
   constexpr int customGroupContentMargin = defaultMargin;
   constexpr int customGroupTitleHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
   constexpr int customGroupTitleSpacing = defaultSpacing;
   constexpr int customGroupMessageHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.05f);
   constexpr int customGroupEditorHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
   constexpr int customGroupEditorSpacing = defaultMargin;
   constexpr int customGroupButtonAreaHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0438f);
   constexpr int customGroupButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0667f);
   constexpr int customGroupButtonSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0125f);
   constexpr float customGroupCornerRadius = 4.0f;
   constexpr float customGroupBorderThickness = 1.0f;
   constexpr int customGroupTimerDelay = 100;
   constexpr float customGroupAlphaOverlay = 0.375f;
   constexpr int customGroupMaxGroupNameLength = 12;

   constexpr float fontSizeBody = 16.0f;
   constexpr float fontSizeBodyLarge = 18.0f;
   constexpr float fontSizeHeader = 18.0f;
   constexpr float fontSizeNumeric = 24.0f;
   constexpr float fontSizeBrand = 48.0f;
   constexpr float fontSizeButton = 14.0f;
   constexpr float fontSizeIcon = 24.0f;
   constexpr float fontSizeLabel = 12.0f;
   constexpr float fontSizeSmall = 12.0f;
   constexpr float fontSizeBodySmall = 14.0f;
   constexpr float fontSizeTiny = 10.0f;
   constexpr float fontSizeIconSmall = 16.0f;
   constexpr float fontSizeIconLarge = 48.0f;
   constexpr float fontSizeTitle = 20.0f;
   constexpr float fontSizePresetLabel = 18.0f;  // Same as header/tempo/logo size
   constexpr float fontSizePresetLabelReduced = 1.44f;  // 60% of ottoHeight * 2.4f for 40% reduction
   constexpr float fontSizePresetMenu = 32.0f;   // Twice the current ComboBox size (16.0f * 2)
   constexpr float fontSizePresetMenuReduced = 25.6f;  // 80% of fontSizePresetMenu for 20% reduction

   constexpr float alphaDisabled = 0.5f;
   constexpr float alphaHover = 0.8f;
   constexpr float alphaSubtle = 0.6f;
   constexpr float alphaTransparent = 0.3f;
   constexpr float alphaFull = 1.0f;

   constexpr float cornerRadiusSmall = 4.0f;
   constexpr float cornerRadiusMedium = 6.0f;
   constexpr float cornerRadiusLarge = 8.0f;
   constexpr float cornerRadiusXLarge = 10.0f;

   constexpr int defaultButtonHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
   constexpr int defaultButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);
   constexpr int defaultComboHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0313f);
   constexpr int defaultLabelHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.025f);
   constexpr int defaultTextEditorHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);

   constexpr int waveformBorderThickness = 1;
   constexpr float waveformDrawAlpha = 1.0f;

   constexpr int drumKitEditorLabelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0667f);
   constexpr int drumKitEditorButtonMargin = 2;
   constexpr int drumKitEditorControlsHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.25f);

   constexpr int sfzOffsetMultiplier = 100000;
   constexpr int sfzBaseMidiNote = 36;
   constexpr int sfzPitchCenterBase = 60;

   // Temporary backward compatibility constants (will be removed once all references are updated)
   constexpr int loopSectionLabelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.085f);
   constexpr int loopSectionLabelHeight = 20;
   constexpr int loopSectionLabelMargin = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.01f);
   constexpr int loopSectionSliderHeight = 30;
   constexpr int loopSectionSliderMargin = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.005f);
   constexpr int loopSectionSliderY = (static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.08f) - 30) / 2;
   constexpr int loopSectionSliderWidthReduction = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.2f);

   constexpr int drumButtonRadioGroupId = 2001;
   constexpr int toggleButtonRadioGroup1 = 1001;
   constexpr int toggleButtonRadioGroup2 = 1002;
   constexpr int fillButtonRadioGroup = 1003;

   constexpr int menuItemClearAssignment = 1;
   constexpr int menuItemStartOffset = 2;
   constexpr int menuItemNoFilesFound = 999;

   constexpr int menuItemAnalyzeGroove = 1;
   constexpr int menuItemFindSimilar = 2;
   constexpr int menuItemCreateVariation = 3;
   constexpr int menuItemSaveTemplate = 4;
   constexpr int menuItemExportAutomation = 5;
   constexpr int menuItemReplace = 6;
   constexpr int menuItemClear = 7;

   constexpr int defaultGroupCount = 8;

   constexpr int sliderTextBoxWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0417f);
   constexpr int sliderTextBoxHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.025f);

   constexpr int variationDialogSliderWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.25f);
   constexpr int variationDialogSliderHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
   constexpr double variationSliderMin = 0.0;
   constexpr double variationSliderMax = 100.0;
   constexpr double variationSliderStep = 1.0;
   constexpr double variationSliderDefault = 50.0;

   constexpr int midiEngineNoteOffDelay = 100;
   constexpr double midiEngineTapTempoTimeout = 2000.0;
   constexpr int midiEngineDefaultDrumNote = 36;
   constexpr int midiEngineMetronomeHighNote = 76;
   constexpr int midiEngineMetronomeLowNote = 77;
   constexpr int midiEngineMetronomeNoteOffDelay = 50;
   constexpr double midiEngineMidiClockPulsesPerBeat = 24.0;
   constexpr int midiEngineMaxMidiChannels = 16;
   constexpr int midiEngineMaxMidiVelocity = 127;
   constexpr int midiEngineMetronomeChannel = 10;

   constexpr float velocityEditorSCurveFactor = 3.0f;
   constexpr int sampleEditControlsLabelWidthDivisor = 2;
   constexpr int sampleEditControlsSliderWidthDivisor = 2;
   constexpr int sampleEditControlsButtonWidthDivisor = 2;
   constexpr int sampleEditControlsButtonSpacingDivisor = 2;
   constexpr int drumKitEditorTopBarSpacingMultiplier = 2;
   constexpr int drumKitEditorEditControlsWidthRatio = 2;
   constexpr int playerTabsRadioGroupId = 1001;

   constexpr int iniManagerTimerInterval = 1000;
   constexpr float midiEngineLastProcessTimeInit = 0.0;
   constexpr float midiEngineLastClockTimeInit = 0.0;
   constexpr int midiEngineClockDivider = 24;
   constexpr float midiEngineSwingRatioBase = 50.0f;
   constexpr float midiEngineSwingThreshold = 0.45f;
   constexpr float midiEngineSwingUpperThreshold = 0.55f;
   constexpr float midiEngineVelocityRandomRange = 20.0f;
   constexpr float midiEngineTimingRandomRange = 0.02f;
   constexpr float midiEngineHumanizeTimingRange = 0.05f;
   constexpr int midiEngineQuantizeDefault = 16;
   constexpr float stateManagerDefaultLoopPosition = 50.0f;
   constexpr int pluginProcessorDebugVoiceCount = 32;
   constexpr float pluginProcessorSampleRateDefault = 48000.0;
   constexpr int pluginProcessorNumPrograms = 1;
   constexpr int pluginProcessorCurrentProgram = 0;
   constexpr double pluginProcessorTailLength = 0.0;
   constexpr int midiFileManagerMaxTruncateLength = 12;
   constexpr float midiFileManagerSwingBase = 50.0f;
   constexpr float midiFileManagerSwingScale = 100.0f;
   constexpr float midiFileManagerSwingMin = 0.0f;
   constexpr float midiFileManagerSwingMax = 100.0f;
   constexpr double midiFileManagerGridResolution = 0.25;
   constexpr float midiFileManagerTightnessScale = 10.0f;
   constexpr float midiFileManagerTempoEstimateMin = 30.0f;
   constexpr float midiFileManagerTempoEstimateMax = 300.0f;
   constexpr float midiFileManagerTempoRoundTo = 5.0f;
   constexpr double midiFileManagerBeatThreshold = 0.5;
   constexpr double midiFileManagerEighthNoteMin = 0.4;
   constexpr double midiFileManagerEighthNoteMax = 0.6;
   constexpr int midiFileManagerTicksPerQuarter = 960;
   constexpr int midiFileManagerTempoMetaEvent = 500000;
   constexpr float midiFileManagerVelocityBase = 0.8f;
   constexpr float midiFileManagerVariationMax = 1.0f;
   constexpr int midiFileManagerVelocityMin = 1;
   constexpr int midiFileManagerVelocityMax = 127;
   constexpr int midiFileManagerGrooveGroups = 16;
   constexpr float midiFileManagerSwingRatioDiv = 50.0f;
   constexpr double midiFileManagerSwingOffset = 0.5;

   constexpr float responsiveLayoutScaleThreshold = 0.001f;
   constexpr int responsiveLayoutCenterDivisor = 2;
   constexpr float responsiveLayoutDefaultAspectRatio = 16.0f / 9.0f;
   constexpr float responsiveLayoutBaseDPI = 96.0f;
   constexpr float responsiveLayoutDPIThreshold = 0.01f;

   constexpr int sfzVoiceStealThreshold = 100;
   constexpr int defaultBufferSize = 512;
   constexpr int defaultOutputChannels = 2;
   constexpr int maxMidiVelocity = 127;

   constexpr int effectSectionHeaderHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
   constexpr int effectButtonHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
   constexpr int effectButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);

   constexpr int minWindowWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.6667f);
   constexpr int minWindowHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.75f);

   // Row height percentages (must total 100%) - CORRECTED OTTO LAYOUT STRUCTURE
    constexpr float ROW_1_HEIGHT_PERCENT = 10.0f;  // Row 1: TopBar (Transport, Tempo, Gear)
    constexpr float ROW_2_HEIGHT_PERCENT = 8.0f;   // Row 2: Player Buttons/Tabs
    constexpr float ROW_3_HEIGHT_PERCENT = 12.0f;  // Row 3: DrumKit Menu Controls
    constexpr float ROW_4_HEIGHT_PERCENT = 12.0f;  // Row 4: Drum Pattern Menu and Labels
    constexpr float ROW_5_HEIGHT_PERCENT = 50.0f;  // Row 5: Large 4x4 Matrix, Buttons, Sliders
    constexpr float ROW_6_HEIGHT_PERCENT = 8.0f;   // Row 6: Small Loop Controls (Start, End, Position)

   // Compile-time validation that percentages total 100%
   static_assert(ROW_1_HEIGHT_PERCENT + ROW_2_HEIGHT_PERCENT + ROW_3_HEIGHT_PERCENT +
                 ROW_4_HEIGHT_PERCENT + ROW_5_HEIGHT_PERCENT + ROW_6_HEIGHT_PERCENT == 100.0f,
                 "Row height percentages must total exactly 100%");

   // Calculate actual row heights based on interface height
   constexpr int ROW_1_HEIGHT = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * (ROW_1_HEIGHT_PERCENT / 100.0f));
   constexpr int ROW_2_HEIGHT = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * (ROW_2_HEIGHT_PERCENT / 100.0f));
   constexpr int ROW_3_HEIGHT = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * (ROW_3_HEIGHT_PERCENT / 100.0f));
   constexpr int ROW_4_HEIGHT = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * (ROW_4_HEIGHT_PERCENT / 100.0f));
   constexpr int ROW_5_HEIGHT = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * (ROW_5_HEIGHT_PERCENT / 100.0f));
   constexpr int ROW_6_HEIGHT = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * (ROW_6_HEIGHT_PERCENT / 100.0f));

   // Calculate cumulative Y positions for each row
   constexpr int ROW_1_Y = 0;
   constexpr int ROW_2_Y = ROW_1_Y + ROW_1_HEIGHT;
   constexpr int ROW_3_Y = ROW_2_Y + ROW_2_HEIGHT;
   constexpr int ROW_4_Y = ROW_3_Y + ROW_3_HEIGHT;
   constexpr int ROW_5_Y = ROW_4_Y + ROW_4_HEIGHT;
   constexpr int ROW_6_Y = ROW_5_Y + ROW_5_HEIGHT;

   // Touch accessibility requirements as percentages of interface dimensions @ 1024x768
   constexpr float MIN_TOUCH_TARGET_HEIGHT_PERCENT = 5.73f;  // 44px / 768px = ~5.729% (rounded up)
   constexpr float MIN_TOUCH_TARGET_WIDTH_PERCENT = 4.30f;   // 44px / 1024px = ~4.297%
   constexpr int MIN_TOUCH_TARGET_PX = 44;                   // iOS/Android accessibility standard

   // Calculate minimum touch target dimensions
   constexpr int MIN_TOUCH_HEIGHT = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * (MIN_TOUCH_TARGET_HEIGHT_PERCENT / 100.0f));
   constexpr int MIN_TOUCH_WIDTH = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (MIN_TOUCH_TARGET_WIDTH_PERCENT / 100.0f));

   // Validate that our percentage calculations meet accessibility standards
   static_assert(MIN_TOUCH_HEIGHT >= MIN_TOUCH_TARGET_PX,
                 "Calculated minimum touch height below 44px accessibility standard");
   static_assert(MIN_TOUCH_WIDTH >= MIN_TOUCH_TARGET_PX,
                 "Calculated minimum touch width below 44px accessibility standard");

   // Pattern matrix minimum space requirement (percentage-based) @ 1024x768
   constexpr float MIN_PATTERN_MATRIX_HEIGHT_PERCENT = 39.06f; // ~300px / 768px
   constexpr int MIN_PATTERN_MATRIX_HEIGHT = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * (MIN_PATTERN_MATRIX_HEIGHT_PERCENT / 100.0f));

   static_assert(ROW_5_HEIGHT >= MIN_PATTERN_MATRIX_HEIGHT,
                 "Row 5 height insufficient for pattern matrix main content");

   // PHASE 9D: Spectrum analyzer positioning constants (percentage-based)
   constexpr float ROW_5_SPECTRUM_X_PERCENT = 70.0f;        // Start at 70% from left
   constexpr float ROW_5_SPECTRUM_Y_PERCENT = 5.0f;         // 5% margin from top
   constexpr float ROW_5_SPECTRUM_WIDTH_PERCENT = 30.0f;    // 30% of row width
   constexpr float ROW_5_SPECTRUM_HEIGHT_PERCENT = 90.0f;   // 90% of row height (10% margins)

   // Validate spectrum analyzer positioning
   static_assert(ROW_5_SPECTRUM_X_PERCENT + ROW_5_SPECTRUM_WIDTH_PERCENT <= 100.0f,
                 "Row 5 spectrum analyzer exceeds row width");
   static_assert(ROW_5_SPECTRUM_Y_PERCENT + ROW_5_SPECTRUM_HEIGHT_PERCENT <= 100.0f,
                 "Row 5 spectrum analyzer exceeds row height");

   // Row-specific namespaces for component positioning
   namespace Row1 {
       // Row 1: TopBar Component (Transport, Tempo, Gear) - 10%
       constexpr int height = ROW_1_HEIGHT;
       constexpr int yPosition = ROW_1_Y;
       constexpr int contentHeight = height - (defaultPadding * 2); // Account for padding
       constexpr int contentY = yPosition + defaultPadding;

       // TopBar-specific component positioning constants using Row 1 bounds
       constexpr int iconY = (height - iconButtonSize) / 2;
       constexpr int gearX = defaultMargin;
       constexpr int linkX = gearX + iconButtonSize + topBarIconSpacing;
       constexpr int cloudX = linkX + iconButtonSize + topBarIconSpacing;
       constexpr int leftChevronX = cloudX + iconButtonSize + topBarIconSpacing;
       constexpr int chevronSize = iconButtonSize;
       constexpr int presetY = static_cast<int>((height - static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f)) / 2);
       constexpr int presetsMenuX = leftChevronX + chevronSize + topBarIconSpacing;
       constexpr int presetsMenuY = presetY;
       constexpr int presetsMenuWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.15f);
       constexpr int rightChevronX = presetsMenuX + presetsMenuWidth + topBarIconSpacing;
       constexpr int playX = (Defaults::DEFAULT_INTERFACE_WIDTH - iconButtonSize) / 2;
       constexpr int bpmX = playX + iconButtonSize + topBarIconSpacing;
       constexpr int bpmY = static_cast<int>((height - static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f)) / 2);
       constexpr int bpmWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);
       constexpr int bpmHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
       constexpr int clockSyncY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.05f);
       constexpr int clockSyncHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.01875f);
       constexpr int tapTempoX = bpmX + bpmWidth + topBarIconSpacing;
       constexpr int recordX = tapTempoX + iconButtonSize + topBarIconSpacing;
       constexpr int overdubX = recordX + iconButtonSize + topBarIconSpacing;
       constexpr int loopX = overdubX + iconButtonSize + topBarIconSpacing;
       constexpr int ottoWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0833f);
       constexpr int ottoX = Defaults::DEFAULT_INTERFACE_WIDTH - ottoWidth - defaultSpacing;
       constexpr int ottoY = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.00625f);
       constexpr int ottoHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.0375f);
       constexpr int versionY = ottoY + ottoHeight;
       constexpr int versionHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * 0.01875f);

       // Touch target validation - Note: iconButtonSize is a global constant used throughout the app
       // If iconButtonSize needs to be larger for accessibility, it should be adjusted globally
       // Row 1 specific components that need minimum touch targets should use larger values
       constexpr int minTouchButtonSize = (iconButtonSize < MIN_TOUCH_TARGET_PX) ? MIN_TOUCH_TARGET_PX : iconButtonSize;
       constexpr int minTouchBpmHeight = (bpmHeight < MIN_TOUCH_TARGET_PX) ? MIN_TOUCH_TARGET_PX : bpmHeight;
   }

   namespace Row2 {
       constexpr int height = ROW_2_HEIGHT;
       constexpr int yPosition = ROW_2_Y;
       constexpr int contentHeight = height - (defaultPadding * 2);
       constexpr int contentY = yPosition + defaultPadding;

       // Tab configuration percentages
       constexpr int tabsCount = 8;
       constexpr float tabHeightReductionPercent = 5.0f;  // 5% reduction in button height

       // Responsive tab width and spacing percentages (based on interface width)
       constexpr float tabWidthPercent = 9.0f;          // Each tab: 9% of interface width
       constexpr float tabSpacingPercent = 2.25f;       // Between tabs: 2.25% of interface width

       constexpr float totalTabsWidthPercent = (tabsCount * tabWidthPercent) + ((tabsCount - 1) * tabSpacingPercent);
       constexpr float horizontalCenteringOffsetPercent = (100.0f - totalTabsWidthPercent) / 2.0f;

       // Calculated dimensions using percentages
       constexpr int originalTabContentHeight = height - (defaultPadding * 2);
       constexpr int tabContentHeight = static_cast<int>(originalTabContentHeight * (100.0f - tabHeightReductionPercent) / 100.0f);
       constexpr int tabTopOffset = (height - tabContentHeight) / 2;  // Center vertically in row

       constexpr int tabWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (tabWidthPercent / 100.0f));
       constexpr int leftMargin = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (horizontalCenteringOffsetPercent / 100.0f));
       constexpr int tabSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (tabSpacingPercent / 100.0f));

       // Highlight bar dimensions (percentage-based)
       constexpr float highlightHeightPercent = 0.1f * ROW_2_HEIGHT;;
       constexpr int highlightHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * (highlightHeightPercent / 100.0f));
       constexpr int highlightMargin = defaultSpacing;
       constexpr int highlightWidthReduction = defaultMargin;

       // Touch accessibility validation using percentage-based calculations
       static_assert(tabContentHeight >= MIN_TOUCH_TARGET_PX || height >= MIN_TOUCH_TARGET_PX,
                     "Row 2 tabs below minimum touch target size (44px)");

       // Layout validation - ensure all tabs fit within interface width
       constexpr int totalTabsWidth = (tabsCount * tabWidth) + ((tabsCount - 1) * tabSpacing) + (2 * leftMargin);
       static_assert(totalTabsWidth <= Defaults::DEFAULT_INTERFACE_WIDTH * 1.05f, // Allow 5% tolerance
                     "Row 2 PlayerTabs exceed interface width - reduce tab percentages");

       // Tab content height validation
       constexpr int minTabHeight = 24;
       static_assert(tabContentHeight >= minTabHeight,
                     "Row 2 insufficient height for player tabs");
   }

   namespace Row3 {
       constexpr int height = ROW_3_HEIGHT;
       constexpr int yPosition = ROW_3_Y;
       constexpr int contentHeight = height - (defaultPadding * 2);
       constexpr int contentY = yPosition + defaultPadding;

       // Layout: [Edit] [<] [DrumKit ▼] [>] [Mixer] [Mute]

       // Large player number font size and dimensions - PERCENTAGE-BASED
       constexpr float largePlayerFontSizePercent = 70.0f;  // 70% of content height
       constexpr float largePlayerFontSize = contentHeight * (largePlayerFontSizePercent / 100.0f);
       constexpr float playerNumberWidthPercent = 6.67f;    // 6.67% of interface width (~80px at 1200w)
       constexpr int playerNumberWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (playerNumberWidthPercent / 100.0f));
       constexpr int playerNumberHeight = contentHeight;

       // Component sizing percentages for responsive scaling
       constexpr float buttonSizePercent = 75.0f;           // 75% of content height
       constexpr float buttonSpacingPercent = 0.83f;        // 0.83% of interface width (~10px at 1200w)
       constexpr int buttonSize = static_cast<int>(contentHeight * (buttonSizePercent / 100.0f));
       constexpr int buttonSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (buttonSpacingPercent / 100.0f));

       // DrumKit dropdown dimensions - PERCENTAGE-BASED
       constexpr float dropdownWidthPercent = 12.5f;        // 12.5% of interface width (~150px at 1200w)
       constexpr float dropdownHeightPercent = 80.0f;       // 80% of content height
       constexpr int dropdownWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (dropdownWidthPercent / 100.0f));
       constexpr int dropdownHeight = static_cast<int>(contentHeight * (dropdownHeightPercent / 100.0f));

       // Layout positioning - left to right flow
       constexpr int startX = defaultMargin; // Start with default margin from left edge

       // Player number positioning
       constexpr int playerNumberX = startX;
       constexpr int playerNumberY = contentY + ((contentHeight - playerNumberHeight) / 2); // Vertically centered

       // Edit button positioning (after player number)
       constexpr int editButtonX = playerNumberX + playerNumberWidth + buttonSpacing;
       constexpr int editButtonY = contentY + ((contentHeight - buttonSize) / 2);
       constexpr int editButtonSize = buttonSize;

       // Left chevron button positioning
       constexpr int leftChevronX = editButtonX + editButtonSize + buttonSpacing;
       constexpr int leftChevronY = editButtonY;
       constexpr int leftChevronSize = buttonSize;

       // DrumKit dropdown positioning (centered between chevrons)
       constexpr int dropdownX = leftChevronX + leftChevronSize + buttonSpacing;
       constexpr int dropdownY = defaultPadding + ((contentHeight - dropdownHeight) / 2);

       // Right chevron button positioning
       constexpr int rightChevronX = dropdownX + dropdownWidth + buttonSpacing;
       constexpr int rightChevronY = editButtonY;
       constexpr int rightChevronSize = buttonSize;

       // Mute button positioning (hugs right side of interface)
       constexpr int muteButtonX = Defaults::DEFAULT_INTERFACE_WIDTH - defaultMargin - buttonSize;
       constexpr int muteButtonY = editButtonY;
       constexpr int muteButtonSize = buttonSize;

       // Mixer button positioning (just to the left of ther Mute button)
       constexpr int mixerButtonX = muteButtonX - buttonSize - buttonSpacing;
       constexpr int mixerButtonY = editButtonY;
       constexpr int mixerButtonSize = buttonSize;

       // DrumKit selected label positioning - centered below dropdown
       constexpr float selectedLabelWidthPercent = 15.0f;        // 15% of interface width
       constexpr float selectedLabelHeightPercent = 80.0f;       // 80% of content height (full Row 3 space)
       constexpr int selectedLabelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (selectedLabelWidthPercent / 100.0f));
       constexpr int selectedLabelHeight = static_cast<int>(contentHeight * (selectedLabelHeightPercent / 100.0f));
       constexpr int selectedLabelX = dropdownX + ((dropdownWidth - selectedLabelWidth) / 2); // Center under dropdown
       constexpr int selectedLabelY = contentY + ((contentHeight - selectedLabelHeight) / 2); // Center in Row 3

       // Total width calculation for validation
       constexpr int totalUsedWidth = mixerButtonX + mixerButtonSize + defaultMargin - startX;

       // Ensure all components fit within interface width
       static_assert(totalUsedWidth <= Defaults::DEFAULT_INTERFACE_WIDTH,
                     "Row 3 components exceed interface width - reduce sizes or spacing");

       // Font size validation for large player number @ 1024x768
       static_assert(largePlayerFontSize >= 20.0f && largePlayerFontSize <= 120.0f,
                     "Large player font size out of reasonable range (20-120px)");

       // Component accessibility - using percentage-based touch target validation
       static_assert(buttonSize >= MIN_TOUCH_TARGET_PX,
                     "Row 3 buttons below minimum touch target size (44px)");
       static_assert(dropdownHeight >= MIN_TOUCH_TARGET_PX,
                     "Row 3 dropdown below minimum touch target size (44px)");

       // Alternative smaller button size for constrained layouts
       constexpr int compactButtonSize = MIN_TOUCH_TARGET_PX;
       constexpr int compactSpacing = static_cast<int>(buttonSpacing * 0.75f);

       // Backup positioning for compact mode (if needed)
       constexpr bool useCompactMode = (totalUsedWidth > Defaults::DEFAULT_INTERFACE_WIDTH * 0.9f);
       constexpr int activeButtonSize = useCompactMode ? compactButtonSize : buttonSize;
       constexpr int activeSpacing = useCompactMode ? compactSpacing : buttonSpacing;
   }


   namespace Row4 {

       constexpr int height = ROW_4_HEIGHT;
       constexpr int yPosition = ROW_4_Y;
       constexpr int contentHeight = height - (defaultPadding * 2);
       constexpr int contentY = yPosition + defaultPadding;

        // Layout: [Drum pattern group edit icon], [left chevron], [drum pattern group menu/label combo], [right chevron], [favorite icon], [toggles buttons label], [fills buttons label], [swing slider label], [Energy slider label], [Volume slider label]

       // Component width percentages (based on interface width)
       constexpr float editIconWidthPercent = 5.0f;       // Edit/Pencil icon: 5% of interface width
       constexpr float chevronWidthPercent = 3.5f;        // Left/Right chevrons: 3.5% each
       constexpr float dropdownWidthPercent = 16.0f;      // Pattern group dropdown: 16% of interface width
       constexpr float favoriteIconWidthPercent = 5.0f;   // Favorite icon: 5% of interface width
       constexpr float labelWidthPercent = 11.0f;         // Labels (toggles, fills, swing, energy, volume): 11% each
       constexpr float componentSpacingPercent = 0.8f;    // Spacing between components: 0.8%

       // Component height percentages (based on row content height)
       constexpr float iconHeightPercent = 75.0f;         // Icons: 75% of content height
       constexpr float labelHeightPercent = 70.0f;        // Labels: 70% of content height
       constexpr float dropdownHeightPercent = 80.0f;     // Dropdown: 80% of content height


       // Calculated dimensions
       constexpr int editIconWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (editIconWidthPercent / 100.0f));
       constexpr int chevronWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (chevronWidthPercent / 100.0f));
       constexpr int dropdownWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (dropdownWidthPercent / 100.0f));
       constexpr int favoriteIconWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (favoriteIconWidthPercent / 100.0f));
       constexpr int labelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (labelWidthPercent / 100.0f));
       constexpr int componentSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (componentSpacingPercent / 100.0f));

       constexpr int iconHeight = static_cast<int>(contentHeight * (iconHeightPercent / 100.0f));
       constexpr int labelHeight = static_cast<int>(contentHeight * (labelHeightPercent / 100.0f));
       constexpr int dropdownHeight = static_cast<int>(contentHeight * (dropdownHeightPercent / 100.0f));


       // Layout positioning - left to right flow
       constexpr int startX = defaultMargin;

       // 1. Edit/Pencil icon positioning
       constexpr int editIconX = startX;
       constexpr int editIconY = defaultPadding + ((contentHeight - iconHeight) / 2);

       // 2. Left chevron positioning

       // 3. Pattern group dropdown positioning
       constexpr int leftChevronX = editIconX + editIconWidth + componentSpacing;
       constexpr int leftChevronY = defaultPadding + ((contentHeight - iconHeight) / 2);

       constexpr int dropdownX = leftChevronX + chevronWidth + componentSpacing;
       constexpr int dropdownY = defaultPadding + ((contentHeight - dropdownHeight) / 2);

       // 4. Right chevron positioning
       constexpr int rightChevronX = dropdownX + dropdownWidth + componentSpacing;
       constexpr int rightChevronY = defaultPadding + ((contentHeight - iconHeight) / 2);

       // 5. Favorite icon positioning
       constexpr int favoriteIconX = rightChevronX + chevronWidth + componentSpacing;
       constexpr int favoriteIconY = defaultPadding + ((contentHeight - iconHeight) / 2);
       // 6. Toggles label positioning
       constexpr int togglesLabelX = favoriteIconX + favoriteIconWidth + componentSpacing;
       constexpr int togglesLabelY = defaultPadding + ((contentHeight - labelHeight) / 2);

       // 7. Fills label positioning
       constexpr int fillsLabelX = togglesLabelX + labelWidth + componentSpacing;
       constexpr int fillsLabelY = defaultPadding + ((contentHeight - labelHeight) / 2);

       // 8. Swing label positioning
       constexpr int swingLabelX = fillsLabelX + labelWidth + componentSpacing;
       constexpr int swingLabelY = defaultPadding + ((contentHeight - labelHeight) / 2);

       // 9. Energy label positioning
       constexpr int energyLabelX = swingLabelX + labelWidth + componentSpacing;
       constexpr int energyLabelY = defaultPadding + ((contentHeight - labelHeight) / 2);

       // 10. Volume label positioning
       constexpr int volumeLabelX = energyLabelX + labelWidth + componentSpacing;
       constexpr int volumeLabelY = defaultPadding + ((contentHeight - labelHeight) / 2);

       // Total width calculation for validation
       constexpr float totalWidthPercent = editIconWidthPercent + (chevronWidthPercent * 2) + dropdownWidthPercent +
                                          favoriteIconWidthPercent + (labelWidthPercent * 5) + (componentSpacingPercent * 9);
       constexpr int totalUsedWidth = volumeLabelX + labelWidth + defaultMargin - startX;

       // Percentage validation
       static_assert(totalWidthPercent <= 100.0f,
                     "Row 4 component percentages exceed 100% - reduce percentages");

       // Layout validation
       static_assert(totalUsedWidth <= Defaults::DEFAULT_INTERFACE_WIDTH,
                     "Row 4 components exceed interface width - reduce percentages");

       // Touch accessibility validation using percentage-based calculations
       static_assert(iconHeight >= MIN_TOUCH_TARGET_PX && dropdownHeight >= MIN_TOUCH_TARGET_PX && labelHeight >= MIN_TOUCH_TARGET_PX,
                     "Row 4 components below minimum touch target size (44px)");

       // Content height validation
       constexpr int minComponentHeight = 20;
       static_assert(iconHeight >= minComponentHeight && dropdownHeight >= minComponentHeight && labelHeight >= minComponentHeight,
                     "Row 4 components too small for usable interaction");
   }

   namespace Row5 {

       // Row 5: Large 4x4 Matrix, Buttons, and Sliders
       constexpr int height = ROW_5_HEIGHT;
       constexpr int yPosition = ROW_5_Y;
       constexpr int contentHeight = height - (defaultPadding * 2);
       constexpr int contentY = yPosition + defaultPadding;

       // Left Section: 4x4 drum matrix (buttons)
       // Right Section: Controls (toggles, fills, sliders)

       // Issue 5.1: Increase drum matrix allocation from 60% to 75%
       constexpr float leftSectionWidthPercent = 75.0f;    // Pattern matrix gets more space (increased)
       constexpr float rightSectionWidthPercent = 24.0f;   // Controls get remaining space (reduced)
       constexpr float sectionMarginPercent = 0.5f;        // Small margin between sections

       // Validate percentages
       static_assert(leftSectionWidthPercent + rightSectionWidthPercent <= 100.0f,
                     "Row 5 section widths exceed 100%");

       // Calculate actual dimensions based on interface width
       constexpr int leftSectionWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (leftSectionWidthPercent / 100.0f));
       constexpr int rightSectionWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (rightSectionWidthPercent / 100.0f));
       constexpr int sectionMargin = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (sectionMarginPercent / 100.0f));

       // Section positioning
       constexpr int leftSectionX = 0;
       constexpr int rightSectionX = leftSectionWidth + sectionMargin;

       // Validation - ensure sections fit within interface width
       constexpr int totalUsedWidth = leftSectionWidth + rightSectionWidth + sectionMargin;
       static_assert(totalUsedWidth <= Defaults::DEFAULT_INTERFACE_WIDTH,
                     "Row 5 sections exceed interface width");

       // Content area definitions for responsive layouts within sections
       constexpr int leftContentWidth = leftSectionWidth - (defaultPadding * 2);
       constexpr int rightContentWidth = rightSectionWidth - (defaultPadding * 2);

       // Minimum size validation for usability
       constexpr int minSectionWidth = 200;  // Minimum for pattern matrix
       static_assert(leftSectionWidth >= minSectionWidth && rightSectionWidth >= minSectionWidth / 2,
                     "Row 5 sections too narrow for usable content");

       // Issue 5.4 & 5.5: Control button and slider layout constants
       constexpr int controlButtonHeight = 30;        // Height for toggle/fill buttons
       constexpr int controlButtonMargin = 10;        // Margin around control buttons
       constexpr int toggleStartY = 20;               // Starting Y position for toggle buttons
       constexpr int fillButtonSpacing = 20;         // Spacing between toggle and fill sections
       constexpr int sliderStartSpacing = 30;        // Spacing before sliders
       constexpr int verticalSliderWidth = 40;       // Width for vertical sliders
       constexpr int verticalSliderHeight = 120;     // Height for vertical sliders

       // Issue 5.2: Font size multiplier for drum buttons (2x increase)
       constexpr float drumButtonFontMultiplier = 2.0f;
   }

   namespace Row6 {
       // Row 6: Small Loop Controls (Start, End, Position Indicator)
       constexpr int height = ROW_6_HEIGHT;
       constexpr int yPosition = ROW_6_Y;
       constexpr int contentHeight = height - (defaultPadding * 2);
       constexpr int contentY = yPosition + defaultPadding;

       // Loop section positioning - full interface width utilization
       constexpr float labelWidthPercent = 8.5f;          // "LOOP START"/"LOOP END" labels
       constexpr float labelMarginPercent = 1.0f;         // Margins for labels
       constexpr float sliderMarginPercent = 0.5f;        // Margins around slider

       // Calculate actual dimensions
       constexpr int labelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (labelWidthPercent / 100.0f));
       constexpr int labelMargin = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (labelMarginPercent / 100.0f));
       constexpr int sliderMargin = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (sliderMarginPercent / 100.0f));

       // Label positioning
       constexpr int startLabelX = labelMargin;
       constexpr int endLabelX = Defaults::DEFAULT_INTERFACE_WIDTH - labelWidth - labelMargin;
       constexpr int labelY = (height - 20) / 2;  // Center vertically in row
       constexpr int labelHeight = 20;            // Standard label height
       constexpr float labelFontSize = 12.0f;     // Font size for loop labels
       constexpr int windowMargin = 20;           // Margin from window edges
       constexpr int labelTopMargin = 5;          // Small margin from top for labels
       constexpr int sliderTopOffset = 5;         // Offset below labels for slider
       constexpr int remainingHeightOffset = 15;  // Offset for remaining height calculation

       // Slider positioning - fills space between labels
       constexpr int sliderX = startLabelX + labelWidth + sliderMargin;
       constexpr int sliderWidth = endLabelX - sliderX - sliderMargin;
       constexpr int sliderY = (height - 44) / 2;  // Center vertically, 44px for accessibility
       constexpr int sliderHeight = 44;            // Touch-friendly slider height (44px minimum)

       // Layout validation
       constexpr int totalUsedWidth = (labelMargin * 2) + (labelWidth * 2) + (sliderMargin * 2) + sliderWidth;
       static_assert(totalUsedWidth <= Defaults::DEFAULT_INTERFACE_WIDTH,
                     "Row 6 loop components exceed interface width");

       // Touch accessibility validation using percentage-based calculations
       static_assert(sliderHeight >= MIN_TOUCH_TARGET_PX || height >= MIN_TOUCH_TARGET_PX,
                     "Row 6 loop slider below minimum touch target size");

       // Content area validation
       constexpr int minSliderWidth = 200;  // Minimum usable slider width
       static_assert(sliderWidth >= minSliderWidth,
                     "Row 6 loop slider too narrow for usable interaction");
   }

   // Verification that all rows fit within interface height
   static_assert(ROW_6_Y + ROW_6_HEIGHT <= Defaults::DEFAULT_INTERFACE_HEIGHT,
                 "Total row heights exceed default interface height");

   // ========================================================================
   // END OTTO GUI 6-ROW LAYOUT SYSTEM - PHASE 1
   // ========================================================================
}

   namespace GMDrums {
       static const int ACOUSTIC_BASS_DRUM = 35;
       static const int BASS_DRUM_1 = 36;
       static const int SIDE_STICK = 37;
       static const int ACOUSTIC_SNARE = 38;
       static const int HAND_CLAP = 39;
       static const int ELECTRIC_SNARE = 40;
       static const int LOW_FLOOR_TOM = 41;
       static const int CLOSED_HI_HAT = 42;
       static const int HIGH_FLOOR_TOM = 43;
       static const int PEDAL_HI_HAT = 44;
       static const int LOW_TOM = 45;
       static const int OPEN_HI_HAT = 46;
       static const int LOW_MID_TOM = 47;
       static const int HI_MID_TOM = 48;
       static const int CRASH_CYMBAL_1 = 49;
       static const int HIGH_TOM = 50;
       static const int RIDE_CYMBAL_1 = 51;
       static const int CHINESE_CYMBAL = 52;
       static const int RIDE_BELL = 53;
       static const int TAMBOURINE = 54;
       static const int SPLASH_CYMBAL = 55;
       static const int COWBELL = 56;
       static const int CRASH_CYMBAL_2 = 57;
       static const int VIBRASLAP = 58;
       static const int RIDE_CYMBAL_2 = 59;
       static const int HI_BONGO = 60;
       static const int LOW_BONGO = 61;
       static const int MUTE_HI_CONGA = 62;
       static const int OPEN_HI_CONGA = 63;
       static const int LOW_CONGA = 64;
       static const int HIGH_TIMBALE = 65;
       static const int LOW_TIMBALE = 66;
       static const int HIGH_AGOGO = 67;
       static const int LOW_AGOGO = 68;
       static const int CABASA = 69;
       static const int MARACAS = 70;
       static const int SHORT_WHISTLE = 71;
       static const int LONG_WHISTLE = 72;
       static const int SHORT_GUIRO = 73;
       static const int LONG_GUIRO = 74;
       static const int CLAVES = 75;
       static const int HI_WOOD_BLOCK = 76;
       static const int LOW_WOOD_BLOCK = 77;
       static const int MUTE_CUICA = 78;
       static const int OPEN_CUICA = 79;
       static const int MUTE_TRIANGLE = 80;
       static const int OPEN_TRIANGLE = 81;
   }

   namespace Validation {
       static const int MIN_THEME_ID = 1;
       static const int MAX_THEME_ID = 99;
       static const int MIN_SETTINGS_ID = 1;
       static const int MAX_SETTINGS_ID = 999;
       static const int MIN_PRESET_ID = 0;
       static const int MAX_PRESET_ID = 999;
       static const int MIN_AUDIO_SAMPLE_RATE = 44100;
       static const int MAX_AUDIO_SAMPLE_RATE = 192000;
       static const int MIN_BUFFER_SIZE = 32;
       static const int MAX_BUFFER_SIZE = 2048;
       static const int MIN_TEMPO = 30.0f;
       static const int MAX_TEMPO = 300.0f;
       static const float MIN_SWING = 0.0f;
       static const float MAX_SWING = 100.0f;
       static const float MIN_ENERGY = 0.0f;
       static const float MAX_ENERGY = 100.0f;
       static const int MIN_MIDI_NOTE = 0;
       static const int MAX_MIDI_NOTE = 127;
       static const int MIN_MIDI_CHANNEL = 1;
       static const int MAX_MIDI_CHANNEL = 16;
       static const int MIN_MIDI_VELOCITY = 0;
       static const int MAX_MIDI_VELOCITY = 127;
       static const int MIN_PATTERN_LENGTH = 1;
       static const int MAX_PATTERN_LENGTH = 64;
       static const int MIN_PATTERN_STEPS = 1;
       static const int MAX_PATTERN_STEPS = 64;
       static const float MIN_LOOP_POSITION = 0.0f;
       static const float MAX_LOOP_POSITION = 100.0f;
       static const float MIN_INTERFACE_SCALE = 0.75f;
       static const float MAX_INTERFACE_SCALE = 3.0f;
       static const float MIN_VOLUME = 0.0f;
       static const float MAX_VOLUME = 1.0f;
       static const int MIN_QUANTIZE_VALUE = 1;
       static const int MAX_QUANTIZE_VALUE = 32;
       static const int MIN_COUNT_IN_BARS = 0;
       static const int MAX_COUNT_IN_BARS = 8;
       static const int MIN_AUTO_SAVE_INTERVAL = 0;
       static const int MAX_AUTO_SAVE_INTERVAL = 3600;
       static const int MIN_PLAYER_INDEX = 0;
       static const int MAX_PLAYER_INDEX = 7;
       static const int MIN_ARRAY_INDEX = 0;
       static const int MIN_BUTTON_INDEX = 0;
       static const int MAX_BUTTON_INDEX = 15;
       static const int MAX_THEME_NAME_LENGTH = 50;
       static const int MIN_PHOSPHOR_WEIGHT = 0;
       static const int MAX_PHOSPHOR_WEIGHT = 5;
       static const int MIN_WINDOW_SIZE = 200;
       static const int MAX_WINDOW_SIZE = 4000;
       static const float MIN_EQ_FREQ = 20.0f;
       static const float MAX_EQ_FREQ = 20000.0f;
       static const float MIN_EQ_GAIN = -20.0f;
       static const float MAX_EQ_GAIN = 20.0f;
       static const float MIN_EQ_Q = 0.1f;
       static const float MAX_EQ_Q = 10.0f;
       static const float MIN_VOLUME_DB = -60.0f;
       static const float MAX_VOLUME_DB = 12.0f;
   };

   // ========================================================================
   // VALIDATION ARRAYS
   // ========================================================================

   static const juce::StringArray VALID_TIME_SIGNATURES = {"4/4", "3/4", "6/8", "2/4", "5/4", "7/8", "9/8", "12/8"};
   static const juce::StringArray KIT_TYPES = {"Acoustic", "Electronic", "808", "909", "Vintage", "Custom"};
   static const juce::StringArray MANUFACTURERS = {"Roland", "Native Instruments", "Ableton", "Custom", "User"};

   inline int clampCountInBars(int bars) {
       return juce::jlimit(Validation::MIN_COUNT_IN_BARS, Validation::MAX_COUNT_IN_BARS, bars);
   }

   inline juce::File getOTTODataDirectory() {
       juce::File documentsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
       return documentsDir.getChildFile(DATABASE_ROOT_FOLDER);
   }

   inline juce::File getSettingsDirectory() {
       return getOTTODataDirectory().getChildFile(SETTINGS_FOLDER);
   }

   inline juce::File getPerformanceDirectory() {
       return getOTTODataDirectory().getChildFile(PERFORMANCE_FOLDER);
   }

   inline juce::File getPatternsDirectory() {
       return getOTTODataDirectory().getChildFile(PATTERNS_FOLDER);
   }

   inline juce::File getKitsDirectory() {
       return getOTTODataDirectory().getChildFile(KITS_FOLDER);
   }

   inline juce::File getMixingDirectory() {
       return getOTTODataDirectory().getChildFile(MIX_FOLDER);
   }

   inline juce::File getSystemDirectory() {
       return getOTTODataDirectory().getChildFile(SYSTEM_FOLDER);
   }

   inline juce::File getPresetsDirectory() {
       return getOTTODataDirectory().getChildFile(PRESETS_FOLDER);
   }

   inline juce::File getINIFilePath(const juce::String& filename) {
       if (filename == GLOBAL_SETTINGS_FILE ||
           filename == AUDIO_SETTINGS_FILE ||
           filename == MIDI_SETTINGS_FILE ||
           filename == PERFORMANCE_SETTINGS_FILE ||
           filename == KEYBOARD_SHORTCUTS_FILE) {
           return getSettingsDirectory().getChildFile(filename);
       }

       if (filename == THEME_MANAGER_FILE ||
           filename == WINDOW_LAYOUT_FILE ||
           filename == SPECTRUM_ANALYZER_FILE ||
           filename == WAVEFORM_DISPLAY_FILE ||
           filename == ADAPTIVE_LAYOUT_MANAGER_FILE ||
           filename == GESTURE_RECOGNIZER_FILE) {
           return getSettingsDirectory().getChildFile(SETTINGS_UI_FOLDER).getChildFile(filename);
       }

       if (filename == DEFAULT_THEME_FILE ||
           filename == DARK_THEME_FILE ||
           filename == LIGHT_THEME_FILE ||
           filename == THEME_INDEX_FILE) {
           return getOTTODataDirectory().getChildFile(THEMES_FOLDER).getChildFile(filename);
       }

       if (filename == PLAYER_CONFIGS_FILE ||
           filename == PLAYER_GROUPS_FILE ||
           filename == GLOBAL_PLAYER_SETTINGS_FILE ||
           filename == PLAYER_STATES_FILE) {
           return getOTTODataDirectory().getChildFile(PLAYERS_FOLDER).getChildFile(filename);
       }

       if (filename == DRUM_KITS_FILE ||
           filename == KIT_CATEGORIES_FILE ||
           filename == SAMPLE_MAPPINGS_FILE ||
           filename == VELOCITY_MAPPINGS_FILE ||
           filename == MIDI_MAPPINGS_FILE ||
           filename == KIT_INDEX_FILE) {
           return getKitsDirectory().getChildFile(filename);
       }

       if (filename == PATTERN_GROUPS_FILE ||
           filename == PATTERN_METADATA_FILE ||
           filename == PATTERN_CHAINS_FILE ||
           filename == PATTERN_INDEX_FILE ||
           filename == TEMPO_MAPS_FILE) {
           return getPatternsDirectory().getChildFile(filename);
       }

       if (filename == EQ_PRESETS_FILE ||
           filename == COMPRESSOR_PRESETS_FILE ||
           filename == REVERB_PRESETS_FILE ||
           filename == DELAY_PRESETS_FILE) {
           return getMixingDirectory().getChildFile(MIX_PRESETS_FOLDER).getChildFile(filename);
       }

       if (filename == CHANNEL_PRESETS_FILE ||
           filename == CHANNEL_GROUPS_FILE ||
           filename == BUS_CONFIGURATIONS_FILE ||
           filename == ROUTING_PRESETS_FILE) {
           return getMixingDirectory().getChildFile(MIX_CHANNELS_FOLDER).getChildFile(filename);
       }

       if (filename == MASTER_CHANNEL_PRESETS_FILE ||
           filename == MASTER_EQ_FILE ||
           filename == MASTER_LIMITER_FILE ||
           filename == MASTER_METERING_FILE) {
           return getMixingDirectory().getChildFile(MIX_MASTER_FOLDER).getChildFile(filename);
       }

       if (filename == MIXER_SNAPSHOTS_FILE) {
           return getMixingDirectory().getChildFile(filename);
       }

       if (filename == PRESET_INDEX_FILE) {
           return getPresetsDirectory().getChildFile(filename);
       }

       if (filename == PROJECT_TEMPLATES_FILE ||
           filename == KIT_TEMPLATES_FILE ||
           filename == PATTERN_TEMPLATES_FILE) {
           return getPresetsDirectory().getChildFile(PRESETS_TEMPLATES_FOLDER).getChildFile(filename);
       }

       if (filename == BLUES_PRESETS_FILE ||
           filename == JAZZ_PRESETS_FILE ||
           filename == SOUL_PRESETS_FILE ||
           filename == FUNK_PRESETS_FILE ||
           filename == EDM_PRESETS_FILE ||
           filename == HIPHOP_PRESETS_FILE ||
           filename == ROCK_PRESETS_FILE ||
           filename == POP_PRESETS_FILE ||
           filename == AMBIENT_PRESETS_FILE ||
           filename == INDUSTRIAL_PRESETS_FILE ||
           filename == GLITCH_PRESETS_FILE) {
           return getPresetsDirectory().getChildFile(PRESETS_CATEGORIES_FOLDER).getChildFile(filename);
       }

       if (filename == SESSIONS_FILE ||
           filename == SET_LISTS_FILE ||
           filename == MACRO_CONTROLS_FILE ||
           filename == SCENE_CHANGES_FILE ||
           filename == BACKUP_STATES_FILE) {
           return getPerformanceDirectory().getChildFile(filename);
       }

       if (filename == SAMPLE_CACHE_FILE ||
           filename == PATTERN_CACHE_FILE ||
           filename == KIT_CACHE_FILE ||
           filename == SEARCH_INDEX_FILE ||
           filename == MIDI_ANALYSIS_CACHE_FILE) {
           return getOTTODataDirectory().getChildFile(CACHE_FOLDER).getChildFile(filename);
       }

       if (filename == APPLICATION_FILE ||
           filename == MIDI_DEVICES_FILE ||
           filename == AUDIO_DEVICES_FILE ||
           filename == EXTERNAL_FOLDERS_FILE ||
           filename == FILE_INDEX_FILE ||
           filename == LICENSING_FILE) {
           return getSystemDirectory().getChildFile(filename);
       }

       return getOTTODataDirectory().getChildFile(filename);
   }

   inline juce::File getGlobalSettingsFile() {
       return getSettingsDirectory().getChildFile(GLOBAL_SETTINGS_FILE);
   }

   inline juce::File getThemesFile() {
       return getSettingsDirectory().getChildFile(THEMES_FILE);
   }

   inline juce::File getAudioSettingsFile() {
       return getSettingsDirectory().getChildFile(AUDIO_SETTINGS_FILE);
   }

   inline juce::File getPlayersFile() {
       return getPerformanceDirectory().getChildFile(PLAYERS_FILE);
   }

   inline juce::File getPatternGroupsFile() {
       return getPatternsDirectory().getChildFile(PATTERN_GROUPS_FILE);
   }

   inline juce::File getPresetsFile() {
       return getPresetsDirectory().getChildFile(PRESETS_FILE);
   }

   inline juce::File getDrumKitsFile() {
       return getKitsDirectory().getChildFile(DRUM_KITS_FILE);
   }

   inline juce::File getMidiInLayoutFile() {
       return getKitsDirectory().getChildFile(MIDI_IN_LAYOUT_FILE);
   }

   inline juce::File getMidiOutLayoutFile() {
       return getKitsDirectory().getChildFile(MIDI_OUT_LAYOUT_FILE);
   }

   inline juce::File getChannelPresetGroupsFile() {
       return getMixingDirectory().getChildFile(CHANNEL_PRESET_GROUPS_FILE);
   }

   inline juce::File getChannelPresetsFile() {
       return getMixingDirectory().getChildFile(CHANNEL_PRESETS_FILE);
   }

   inline juce::File getEQPresetsFile() {
       return getMixingDirectory().getChildFile(EQ_PRESETS_FILE);
   }

   inline juce::File getMasterChannelPresetsFile() {
       return getMixingDirectory().getChildFile(MASTER_CHANNEL_PRESETS_FILE);
   }

   inline juce::File getMidiDevicesFile() {
       return getSystemDirectory().getChildFile(MIDI_DEVICES_FILE);
   }

   inline juce::File getFileIndexFile() {
       return getSystemDirectory().getChildFile(FILE_INDEX_FILE);
   }

   // ========================================================================
   // MISSING NAMESPACES AND FUNCTIONS
   // ========================================================================

   namespace MIDI {
       static const int STRING_LENGTH_SUBTRACT_ONE = 1;
   }

   namespace UI {
       static const int LIGHT_THEME_ID = 1;
       static const int CLASSIC_THEME_ID = 2;
       static const float SPLASH_SCREEN_ON_TIME = 2.0f;
   }

   namespace Fonts {
       static const float BODY_DEFAULT_SIZE = 14.0f;
       static const float HEADER_DEFAULT_SIZE = 18.0f;
       static const float MAX_FONT_SIZE = 72.0f;
   }

   // Validation functions
   inline bool isValidSettingsID(const juce::String& settingsID) {
       return settingsID.isNotEmpty() && settingsID.length() <= 50;
   }

   inline bool isValidThemeID(const juce::String& themeID) {
       return themeID.isNotEmpty() && themeID.length() <= 50;
   }

   inline bool isValidTempo(float tempo) {
       return tempo >= Defaults::DEFAULT_MIN_BPM && tempo <= Defaults::DEFAULT_MAX_BPM;
   }

   inline bool isValidInterfaceScale(float scale) {
       return scale >= 0.5f && scale <= 3.0f;
   }

   inline bool isValidVolume(float volume) {
       return volume >= 0.0f && volume <= 1.0f;
   }

   inline bool isValidQuantizeValue(int quantize) {
       return quantize >= 1 && quantize <= 32;
   }

   inline bool isValidHexColor(const juce::String& color) {
       if (color.isEmpty() || !color.startsWith("#")) return false;
       if (color.length() != 7 && color.length() != 9) return false;
       for (int i = 1; i < color.length(); ++i) {
           char c = color[i];
           if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
               return false;
           }
       }
       return true;
   }

} // namespace INIConfig
