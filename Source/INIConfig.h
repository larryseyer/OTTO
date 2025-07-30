#pragma once

#include <JuceHeader.h>

namespace INIConfig {
    static const juce::String OTTO_DATA_FOLDER = "OTTO_Data";
    static const juce::String SETTINGS_FOLDER = "Settings";
    static const juce::String PERFORMANCE_FOLDER = "Performance";
    static const juce::String PATTERNS_FOLDER = "Patterns";
    static const juce::String KITS_FOLDER = "Kits";
    static const juce::String MIXING_FOLDER = "Mix";
    static const juce::String SYSTEM_FOLDER = "System";
    static const juce::String PRESETS_FOLDER = "Presets";

    static const juce::String GLOBAL_SETTINGS_FILE = "Global.ini";
    static const juce::String THEMES_FILE = "Themes.ini";
    static const juce::String AUDIO_SETTINGS_FILE = "AudioSettings.ini";
    static const juce::String PRESETS_FILE = "Presets.ini";
    static const juce::String PLAYERS_FILE = "Players.ini";
    static const juce::String PATTERN_CHAINS_FILE = "PatternChains.ini";
    static const juce::String PATTERN_GROUPS_FILE = "PatternGroups.ini";
    static const juce::String DRUM_KITS_FILE = "DrumKits.ini";
    static const juce::String MIDI_IN_LAYOUT_FILE = "MidiInLayout.ini";
    static const juce::String MIDI_OUT_LAYOUT_FILE = "MidiOutLayout.ini";
    static const juce::String CHANNEL_PRESET_GROUPS_FILE = "ChannelPresetGroups.ini";
    static const juce::String CHANNEL_PRESETS_FILE = "ChannelPresets.ini";
    static const juce::String EQ_PRESETS_FILE = "EQPresets.ini";
    static const juce::String MASTER_CHANNEL_PRESETS_FILE = "MasterChannelPresets.ini";
    static const juce::String MIDI_DEVICES_FILE = "MidiDevices.ini";
    static const juce::String FILE_INDEX_FILE = "FileIndex.ini";
    static const juce::String MIDI_ANALYSIS_CACHE_FILE = "MidiAnalysisCache.ini";
    static const juce::String CONTROLLER_PRESETS_FILE  = "ControllerPresets.ini";
    static const juce::String MIDI_MAPPINGS_FILE       = "MidiMappings.ini";
    static const juce::String GROOVE_TEMPLATES_FILE    = "GrooveTemplates.ini";

    static const juce::String FILE_FORMAT_VERSION = "2.0";

    namespace Defaults {
        // MINIMUM SYSTEM REQUIREMENT: 1024x768 display resolution
        // This is the smallest supported screen size that maintains:
        // - 44px minimum touch targets (accessibility compliance)
        // - 300px minimum pattern matrix height (core functionality)
        // - Proper component spacing and text readability
        // See: MINIMUM_SYSTEM_REQUIREMENTS.md for full details
        static const int DEFAULT_INTERFACE_WIDTH = 1024;
        static const int DEFAULT_INTERFACE_HEIGHT = 768;
        static const int DEFAULT_INTERFACE_X = 100;
        static const int DEFAULT_INTERFACE_Y = 100;
        static const int DEFAULT_WINDOW_X = 100;
        static const int DEFAULT_WINDOW_Y = 100;
        static const float DEFAULT_INTERFACE_SCALE = 1.0f;
        static const float DEFAULT_SCALE_FACTOR = 1.0f;

        static const int DEFAULT_THEME_ID = 1;

        static const juce::String DEFAULT_LINK_SYNC_MODE = "Off";
        static const int DEFAULT_LINK_SYNC_VALUE = 0;
        static const juce::String DEFAULT_CLOUD_CONNECTION = "None";
        static const bool DEFAULT_METRONOME_ENABLED = false;
        static const float DEFAULT_METRONOME_VOLUME = 0.5f;
        static const juce::String DEFAULT_METRONOME_SOUND = "Click";
        static const int DEFAULT_QUANTIZE_VALUE = 16;
        static const int DEFAULT_COUNT_IN_BARS = 1;
        static const int DEFAULT_AUDIO_SETTINGS_ID = 1;
        static const bool DEFAULT_MIDI_CLOCK_OUT = false;
        static const bool DEFAULT_MIDI_CLOCK_IN = false;
        static const int DEFAULT_AUTO_SAVE_INTERVAL = 300;
        static const int DEFAULT_SAMPLE_RATE = 48000;
        static const int DEFAULT_BUFFER_SIZE = 256;
        static const juce::String DEFAULT_AUDIO_DEVICE = "Default";
        static const int DEFAULT_INPUT_CHANNELS = 2;
        static const int DEFAULT_OUTPUT_CHANNELS = 2;
        static const bool DEFAULT_LATENCY_COMPENSATION = false;
        static const int DEFAULT_BIT_DEPTH = 24;
        static const bool DEFAULT_ENABLE_ASIO = false;
        static const bool DEFAULT_ENABLE_MULTICORE = true;
        static const juce::String DEFAULT_DRUMKIT = "Acoustic";
        static const bool DEFAULT_PLAYER_ENABLED = true;
        static const bool DEFAULT_EDIT_MODE = false;
        static const float SWING = 10.0f;
        static const float ENERGY = 75.0f;
        static const float VOLUME = 0.8f;
        static const bool DEFAULT_PLAY_STATE = false;
        static const int DEFAULT_CURRENT_PLAYER = 0;
        static const int DEFAULT_KIT_INDEX = 0;
        static const int DEFAULT_CURRENT_PRESET = 0;
        static const juce::String DEFAULT_GROUP_PREFIX = "Group ";
        static const int DEFAULT_SELECTED_BUTTON = 0;
        static const bool DEFAULT_FAVORITE_STATE = false;
        static const float DEFAULT_LOOP_POSITION = 50.0f;
        static const float DEFAULT_MASTER_VOLUME = 0.8f;
        static const float DEFAULT_REVERB_MIX = 0.2f;
        static const float DEFAULT_DELAY_MIX = 0.1f;
        static const float DEFAULT_KICK_VOLUME = 0.9f;
        static const float DEFAULT_SNARE_VOLUME = 0.8f;
        static const float DEFAULT_HIHAT_VOLUME = 0.7f;
        static const int DEFAULT_PHOSPHOR_WEIGHT = 1;
        static constexpr float DEFAULT_PHOSPHOR_ICON_BOX_FIT_RATIO = 0.99f; // 99% of box area for perfect icon fit

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
    }

namespace LayoutConstants {
    constexpr int defaultMargin = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0167f);
    constexpr int defaultPadding = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0083f);
    
    // GENERAL SPACING: Updated from 0.01% to 1.25% for better visual separation
    constexpr int defaultSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * 0.0125f);
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

   // DEPRECATED: Loop section constants moved to Row6 namespace for Phase 5 integration
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

   // ========================================================================
   // OTTO GUI 6-ROW LAYOUT SYSTEM - PHASE 1: FOUNDATION
   // ========================================================================
   // Percentage-based row structure for consistent GUI layout across all platforms
   // Base calculations use Defaults::DEFAULT_INTERFACE_HEIGHT for responsive scaling

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
       // Row 2: Player Buttons/Tabs - 6%
       constexpr int height = ROW_2_HEIGHT;
       constexpr int yPosition = ROW_2_Y;
       constexpr int contentHeight = height - (defaultPadding * 2);
       constexpr int contentY = yPosition + defaultPadding;

       // PHASE 4: PlayerTabs Component Layout Definition - PERCENTAGE-BASED
       // =================================================================
       // Layout: 8 player tabs with responsive percentage-based positioning
       // Button 1: [4.5% margin][Button1] (left margin)
       // Buttons 2-7: [2.25% margin][Button] (spacing between tabs)
       // Button 8: [2.25% margin][Button8][4.5% margin] (right margin)

       // Tab configuration percentages
       constexpr int tabsCount = 8;
       constexpr float tabTopOffsetPercent = 0.25f;      // 0.25% of interface height
       constexpr float tabBottomMarginPercent = 0.625f;  // 0.625% of interface height

       // Responsive tab width and spacing percentages (based on interface width)
       constexpr float tabWidthPercent = 9.0f;          // Each tab: 9% of interface width
       constexpr float leftMarginPercent = 4.5f;        // Left margin: 4.5% of interface width
       constexpr float tabSpacingPercent = 2.25f;       // Between tabs: 2.25% of interface width
       constexpr float rightMarginPercent = 4.5f;       // Right margin: 4.5% of interface width

       // Calculated dimensions using percentages
       constexpr int tabTopOffset = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * (tabTopOffsetPercent / 100.0f));
       constexpr int tabBottomMargin = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * (tabBottomMarginPercent / 100.0f));
       constexpr int tabContentHeight = height - tabTopOffset - tabBottomMargin;
       constexpr int tabWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (tabWidthPercent / 100.0f));
       constexpr int leftMargin = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (leftMarginPercent / 100.0f));
       constexpr int tabSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (tabSpacingPercent / 100.0f));

       // Highlight bar dimensions (percentage-based)
       constexpr float highlightHeightPercent = 0.25f;  // 0.25% of interface height
       constexpr int highlightHeight = static_cast<int>(Defaults::DEFAULT_INTERFACE_HEIGHT * (highlightHeightPercent / 100.0f));
       constexpr int highlightMargin = defaultSpacing;
       constexpr int highlightWidthReduction = defaultMargin;

       // Touch accessibility validation using percentage-based calculations
       static_assert(tabContentHeight >= MIN_TOUCH_TARGET_PX || height >= MIN_TOUCH_TARGET_PX,
                     "Row 2 tabs below minimum touch target size (44px)");

       // Layout validation - ensure all tabs fit within interface width
       constexpr int totalTabsWidth = (tabsCount * tabWidth) + ((tabsCount - 1) * tabSpacing) + leftMargin + rightMarginPercent;
       static_assert(totalTabsWidth <= Defaults::DEFAULT_INTERFACE_WIDTH * 1.05f, // Allow 5% tolerance
                     "Row 2 PlayerTabs exceed interface width - reduce tab percentages");

       // Tab content height validation
       constexpr int minTabHeight = 24;
       static_assert(tabContentHeight >= minTabHeight,
                     "Row 2 insufficient height for player tabs");
   }

   namespace Row3 {
       // Row 3: DrumKit Menu Controls - 16%
       constexpr int height = ROW_3_HEIGHT;
       constexpr int yPosition = ROW_3_Y;
       constexpr int contentHeight = height - (defaultPadding * 2);
       constexpr int contentY = yPosition + defaultPadding;

       // CORRECTED LAYOUT: DrumKit Menu Row
       // =====================================
       // Layout: [Edit] [<] [DrumKit ▼] [>] [Mute] [Mixer]

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
       constexpr int dropdownY = contentY + ((contentHeight - dropdownHeight) / 2);

       // Right chevron button positioning
       constexpr int rightChevronX = dropdownX + dropdownWidth + buttonSpacing;
       constexpr int rightChevronY = editButtonY;
       constexpr int rightChevronSize = buttonSize;

       // Mute button positioning
       constexpr int muteButtonX = rightChevronX + rightChevronSize + buttonSpacing;
       constexpr int muteButtonY = editButtonY;
       constexpr int muteButtonSize = buttonSize;

       // Mixer button positioning (rightmost)
       constexpr int mixerButtonX = muteButtonX + muteButtonSize + buttonSpacing;
       constexpr int mixerButtonY = editButtonY;
       constexpr int mixerButtonSize = buttonSize;

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
       // Row 4: Drum Pattern Menu and Labels - 14%
       constexpr int height = ROW_4_HEIGHT;
       constexpr int yPosition = ROW_4_Y;
       constexpr int contentHeight = height - (defaultPadding * 2);
       constexpr int contentY = yPosition + defaultPadding;

       // CORRECTED LAYOUT: Drum Pattern Menu and Labels
       // ===================================================================
       // Layout: [Pattern Label] [Pattern Group Dropdown] [Status Info] [Actions]

       // Component width percentages (based on interface width)
       constexpr float groupLabelWidthPercent = 8.0f;     // "Group" label: 8% of interface width
       constexpr float dropdownWidthPercent = 20.0f;      // Pattern group dropdown: 20% of interface width
       constexpr float statusWidthPercent = 15.0f;        // Status display: 15% of interface width
       constexpr float actionButtonWidthPercent = 8.0f;   // Action buttons: 8% each
       constexpr float componentSpacingPercent = 2.0f;    // Spacing between components: 2%

       // Component height percentages (based on row content height)
       constexpr float labelHeightPercent = 70.0f;        // Labels: 70% of content height
       constexpr float dropdownHeightPercent = 80.0f;     // Dropdown: 80% of content height
       constexpr float buttonHeightPercent = 75.0f;       // Buttons: 75% of content height

       // Calculated dimensions
       constexpr int groupLabelWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (groupLabelWidthPercent / 100.0f));
       constexpr int dropdownWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (dropdownWidthPercent / 100.0f));
       constexpr int statusWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (statusWidthPercent / 100.0f));
       constexpr int actionButtonWidth = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (actionButtonWidthPercent / 100.0f));
       constexpr int componentSpacing = static_cast<int>(Defaults::DEFAULT_INTERFACE_WIDTH * (componentSpacingPercent / 100.0f));

       constexpr int labelHeight = static_cast<int>(contentHeight * (labelHeightPercent / 100.0f));
       constexpr int dropdownHeight = static_cast<int>(contentHeight * (dropdownHeightPercent / 100.0f));
       constexpr int buttonHeight = static_cast<int>(contentHeight * (buttonHeightPercent / 100.0f));

       // Layout positioning - left to right flow
       constexpr int startX = defaultMargin;

       // Group label positioning
       constexpr int groupLabelX = startX;
       constexpr int groupLabelY = contentY + ((contentHeight - labelHeight) / 2);

       // Pattern group dropdown positioning
       constexpr int dropdownX = groupLabelX + groupLabelWidth + componentSpacing;
       constexpr int dropdownY = contentY + ((contentHeight - dropdownHeight) / 2);

       // Status display positioning
       constexpr int statusX = dropdownX + dropdownWidth + componentSpacing;
       constexpr int statusY = contentY + ((contentHeight - labelHeight) / 2);

       // Action buttons positioning (add, delete, etc.)
       constexpr int firstActionButtonX = statusX + statusWidth + componentSpacing;
       constexpr int actionButtonY = contentY + ((contentHeight - buttonHeight) / 2);
       constexpr int secondActionButtonX = firstActionButtonX + actionButtonWidth + componentSpacing;

       // Total width calculation for validation
       constexpr int totalUsedWidth = secondActionButtonX + actionButtonWidth + defaultMargin - startX;

       // Layout validation
       static_assert(totalUsedWidth <= Defaults::DEFAULT_INTERFACE_WIDTH,
                     "Row 4 Pattern Group controls exceed interface width - reduce percentages");

       // Touch accessibility validation using percentage-based calculations
       static_assert(dropdownHeight >= MIN_TOUCH_TARGET_PX && buttonHeight >= MIN_TOUCH_TARGET_PX,
                     "Row 4 components below minimum touch target size (44px)");

       // Content height validation
       constexpr int minComponentHeight = 20;
       static_assert(labelHeight >= minComponentHeight && dropdownHeight >= minComponentHeight,
                     "Row 4 components too small for usable interaction");
   }

   namespace Row5 {
       // Row 5: Large 4x4 Matrix, Buttons, and Sliders (MAIN CONTENT) - 45%
       constexpr int height = ROW_5_HEIGHT;
       constexpr int yPosition = ROW_5_Y;
       constexpr int contentHeight = height - (defaultPadding * 2);
       constexpr int contentY = yPosition + defaultPadding;

       // CORRECTED LAYOUT: 4x4 Matrix + Controls (Main Content Area)
       // ================================================================
       // Left Section: 4x4 drum matrix (buttons) needs more space
       // Right Section: Controls (toggles, fills, sliders) needs less space
       // Split optimized for content: 65% left (pattern matrix) / 35% right (controls)

       constexpr float leftSectionWidthPercent = 63.5f;    // Pattern matrix gets more space
       constexpr float rightSectionWidthPercent = 35.0f;   // Controls get remaining space
       constexpr float sectionMarginPercent = 0.3f;        // Small margin between sections

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
   }

   namespace Row6 {
       // Row 6: Small Loop Controls (Start, End, Position Indicator) - 12%
       constexpr int height = ROW_6_HEIGHT;
       constexpr int yPosition = ROW_6_Y;
       constexpr int contentHeight = height - (defaultPadding * 2);
       constexpr int contentY = yPosition + defaultPadding;

       // CORRECTED LAYOUT: Loop Start/End Controls + Position Indicator
       // ===================================================================
       // Layout: [LOOP START] [================== Loop Slider ==================] [LOOP END]
       // Compact row for loop timeline control

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
       static const int MIN_AUDIO_CHANNELS = 0;
       static const int MAX_AUDIO_CHANNELS = 32;
       static const float MIN_PAN = -1.0f;
       static const float MAX_PAN = 1.0f;
       static const int MIN_ARRAY_INDEX = 0;
       static const int MAX_THREAD_COUNT = 16;
       static const int MIN_THREAD_COUNT = 1;
       static const float MIN_QUALITY = 0.0f;
       static const float MAX_QUALITY = 1.0f;
       static const int MIN_TIME_SIG_NUMERATOR = 1;
       static const int MAX_TIME_SIG_NUMERATOR = 99;
       static const int MIN_TIME_SIG_DENOMINATOR = 1;
       static const int MAX_TIME_SIG_DENOMINATOR = 64;
       static const float MIN_PHOSPHOR_ICON_BOX_FIT_RATIO = 0.5f;  // 50% minimum - still readable
       static const float MAX_PHOSPHOR_ICON_BOX_FIT_RATIO = 1.0f;  // 100% maximum - fills entire box
   }

   namespace UI {
       static const float SPLASH_SCREEN_ON_TIME = 1.0f;
       static const float DEFAULT_SEPARATOR_THICKNESS = 1.0f;
       static const int MAX_TOGGLE_STATES = 5;
       static const int MAX_FILL_STATES = 5;
       static const int MAX_PHOSPHOR_TYPEFACES = 6;
       static const int HEX_COLOR_LENGTH = 7;
       static const int HEX_COLOR_START_INDEX = 1;
       static const int ISO_DATE_LENGTH = 19;
       static const char ISO_DATE_SEPARATOR = '-';
       static const int ISO_DATE_YEAR_POS = 4;
       static const int ISO_DATE_MONTH_POS = 7;
       static const int ISO_DATE_DAY_POS = 10;
       static const int ISO_DATE_HOUR_POS = 13;
       static const int ISO_DATE_MINUTE_POS = 16;
       static const int BUTTON_TEXT_MAX_LENGTH = 50;
       static const int BEATS_BUTTON_TEXT_LENGTH_DIVISOR = 4;
       static const int LIGHT_THEME_ID = 2;
       static const int CLASSIC_THEME_ID = 3;
       static const int CLOSE_BUTTON_SIZE = 36;
       static const int CLOSE_BUTTON_X = 45;
       static const int CLOSE_BUTTON_Y = 8;
       static const int CONTENT_MARGIN = 20;
       static const int TITLE_HEIGHT = 30;
       static const int MESSAGE_HEIGHT = 40;
       static const int EDITOR_HEIGHT = 30;
       static const int BUTTON_AREA_HEIGHT = 35;
       static const int BUTTON_WIDTH = 80;
       static const int BUTTON_SPACING = 15;
       static const float CORNER_RADIUS = 4.0f;
       static const float BORDER_THICKNESS = 1.0f;
       static const int TIMER_DELAY_MS = 100;
       static const int MAX_GROUP_NAME_LENGTH = 12;
       static const int POPUP_WIDTH = 400;
       static const int POPUP_HEIGHT = 250;
       static const int TITLE_SPACING = 10;
       static const int EDITOR_SPACING = 20;

       static const int AI_PANEL_TIMER_HZ = 10;
       static const float AI_PANEL_BLEND_MIN = 0.0f;
       static const float AI_PANEL_BLEND_MAX = 1.0f;
       static const float AI_PANEL_COMPLEXITY_MIN = 0.0f;
       static const float AI_PANEL_COMPLEXITY_MAX = 1.0f;
       static const float AI_PANEL_DEFAULT_COMPLEXITY = 0.5f;
       static const int AI_PANEL_PERFORMANCE_UPDATE_INTERVAL = 1000;

       static const int COMMUNITY_BROWSER_TIMER_INTERVAL = 5000;
       static const int COMMUNITY_BROWSER_PREVIEW_WIDTH = 300;
       static const int COMMUNITY_BROWSER_ITEM_HEIGHT = 60;
       static const int COMMUNITY_BROWSER_MAX_RATING = 5;
       static const float COMMUNITY_BROWSER_MIN_RATING = 0.0f;

       static const int COLLAB_PANEL_TIMER_HZ = 2;
       static const int COLLAB_PANEL_MAX_COLLABORATORS = 8;
       static const int COLLAB_PANEL_SESSION_ID_LENGTH = 6;
       static const int COLLAB_PANEL_CHAT_HISTORY_LINES = 100;

       static const int DRUMKIT_EDITOR_WINDOW_WIDTH = 1200;
       static const int DRUMKIT_EDITOR_WINDOW_HEIGHT = 800;
       static const int DRUMKIT_EDITOR_MIN_WIDTH = 800;
       static const int DRUMKIT_EDITOR_MIN_HEIGHT = 600;

       static const int SCENE_LAUNCHER_NUM_SCENES = 8;
       static const float SCENE_LAUNCHER_BUTTON_ALPHA = 0.8f;
       static const float SCENE_LAUNCHER_INACTIVE_ALPHA = 0.4f;
       static const int SCENE_LAUNCHER_ANIMATION_INTERVAL = 50;

       static const juce::String TOGGLE_BUTTON_NAMES[MAX_TOGGLE_STATES] = {"Auto", "Manual", "Stick", "Ride", "Lock"};
       static const juce::String FILL_BUTTON_NAMES[MAX_FILL_STATES] = {"4", "8", "16", "32", "Solo"};

       static const double SWING_ENERGY_SLIDER_STEP = 1.0;
       static const double VOLUME_SLIDER_STEP = 0.01;

       static const float HORIZONTAL_SEPARATOR_THICKNESS = 2.0f;
   }

   namespace Color {
       static const uint32_t DEFAULT_ERROR_COLOR = 0xffff4444;
       static const uint32_t DEFAULT_WARNING_COLOR = 0xffffaa44;
       static const uint32_t DEFAULT_SUCCESS_COLOR = 0xff44ff44;
       static const uint32_t DEFAULT_METER_LOW_COLOR = 0xff44ff44;
       static const uint32_t DEFAULT_METER_MID_COLOR = 0xffffaa44;
       static const uint32_t DEFAULT_METER_HIGH_COLOR = 0xffff4444;
       static const float ALPHA_OVERLAY = 0.375f;

       // ROW-SPECIFIC DEBUG BACKGROUND COLORS (Dark variants for easy identification)
       // These darker colors help distinguish rows during layout debugging
       static const uint32_t ROW_1_DEBUG_BACKGROUND = 0xff1a1a2e;  // Dark blue-purple (TopBar)
       static const uint32_t ROW_2_DEBUG_BACKGROUND = 0xff2e1a1a;  // Dark red-brown (PlayerTabs)
       static const uint32_t ROW_3_DEBUG_BACKGROUND = 0xff1a2e1a;  // Dark green (Player# + DrumKit)
       static const uint32_t ROW_4_DEBUG_BACKGROUND = 0xff2e2e1a;  // Dark yellow-brown (Pattern Groups)
       static const uint32_t ROW_5_DEBUG_BACKGROUND = 0xff1a1a1a;  // Darkest grey (Pattern Matrix - main content)
       static const uint32_t ROW_6_DEBUG_BACKGROUND = 0xff2e1a2e;  // Dark purple (Loop Section)
   }

   namespace Audio {
       static const float DEFAULT_START_POSITION = 0.0f;
       static const float DEFAULT_END_POSITION = 1.0f;
       static const float DEFAULT_PITCH_SHIFT = 0.0f;
       static const int DEFAULT_FILTER_TYPE = 0;
       static const float DEFAULT_FILTER_FREQUENCY = 1000.0f;
       static const float DEFAULT_FILTER_RESONANCE = 1.0f;
       static const int DEFAULT_CHOKE_GROUP = 0;
       static const float DEFAULT_CURVE_AMOUNT = 0.5f;
       static const float DEFAULT_GROOVE_TIGHTNESS = 1.0f;
       static const int NUM_DRUM_PADS = 16;
       static const int BIT_DEPTH_16 = 16;
       static const int BIT_DEPTH_24 = 24;
       static const int BIT_DEPTH_32 = 32;
       static const int NUM_EQ_BANDS = 3;
       static const int NUM_SEND_TYPES = 2;
       static const float DEFAULT_PRE_GAIN = 0.0f;
       static const float DEFAULT_POST_GAIN = 0.0f;
       static const float DEFAULT_SAMPLERATE_REDUCTION = 1.0f;
       static const float REVERB_HIGH_CUT_FACTOR = 0.4f;
       static const float DELAY_HIGH_CUT_FACTOR = 0.2f;
       static const int DEFAULT_SYNC_DIVISION = 8;
       static const int DEFAULT_SIDECHAIN_SOURCE = 0;
       static const float EQ_ATOMIC_INIT = 0.0f;
       static const float SEND_ATOMIC_INIT = 0.0f;
       static const float DEFAULT_PAN = 0.0f;
       static const bool DEFAULT_MUTE = false;
       static const bool DEFAULT_SOLO = false;

       static const int WAVEFORM_THUMBNAIL_CACHE_SIZE = 512;
       static const float WAVEFORM_POSITION_TOLERANCE = 0.02f;
       static const float WAVEFORM_MIN_SELECTION_SIZE = 0.01f;

       static const int VELOCITY_CURVE_NUM_POINTS = 128;
       static const float VELOCITY_CURVE_LINE_WIDTH = 2.0f;

       static const int CHOKE_GROUP_COUNT = 16;
       static const float SAMPLE_BROWSER_FILE_PREVIEW_TIME = 2.0f;

       static const float DEFAULT_ATTACK_TIME = 0.001f;
       static const float DEFAULT_DECAY_TIME = 0.1f;
       static const float DEFAULT_SUSTAIN_LEVEL = 0.7f;
       static const float DEFAULT_RELEASE_TIME = 0.05f;
   }

   namespace Fonts {
       static const float BODY_DEFAULT_SIZE = 16.0f;
       static const float HEADER_DEFAULT_SIZE = 18.0f;
       static const float NUMERIC_DEFAULT_SIZE = 16.0f;
       static const float BRAND_DEFAULT_SIZE = 24.0f;
       static const float BUTTON_DEFAULT_SIZE = 16.0f;
       static const float ICON_DEFAULT_SIZE = 20.0f;
       static const float VERSION_DEFAULT_SIZE = 12.0f;
       static const float POPUP_DEFAULT_SIZE = 14.0f;
       static const float MAX_FONT_SIZE = 72.0f;

       static const float PHOSPHOR_ICON_SIZE_SMALL = 16.0f;
       static const float PHOSPHOR_ICON_SIZE_MEDIUM = 24.0f;
       static const float PHOSPHOR_ICON_SIZE_LARGE = 32.0f;
       static const float PAD_EDITOR_TEXT_SIZE = 16.0f;
       static const float PAD_EDITOR_SMALL_TEXT_SIZE = 12.0f;
   }

   namespace MIDI {
       static const float VELOCITY_DIVISOR = 2.0f;
       static const int CLOCK_PPQN = 24;
       static const int INACTIVE_PATTERN = -1;
       static const int INACTIVE_SCENE = -1;
       static const int ALL_PLAYERS = -1;
       static const int DEFAULT_CC_NUMBER = -1;
       static const int DEFAULT_FEEDBACK_TYPE = 0;
       static const int DEFAULT_SYSEX_DEVICE_ID = 0;
       static const float DEFAULT_POSITION = 0.0;
       static const float DEFAULT_LAST_TIME = 0.0;
       static const float DEFAULT_VELOCITY_RANGE = 0.0f;
       static const float DEFAULT_VELOCITY_VARIATION = 0.0f;
       static const float DEFAULT_TIMING_DEVIATION = 0.0f;
       static const float DEFAULT_NOTE_DENSITY = 0.0f;
       static const int HEX_BASE = 16;
       static const char HEX_PREFIX = '#';
       static const char HEX_DIGIT_0 = '0';
       static const char HEX_DIGIT_9 = '9';
       static const char HEX_DIGIT_A_UPPER = 'A';
       static const char HEX_DIGIT_F_UPPER = 'F';
       static const char HEX_DIGIT_A_LOWER = 'a';
       static const char HEX_DIGIT_F_LOWER = 'f';
       static const int STRING_LENGTH_SUBTRACT_ONE = 1;
   }

   class TimeSignature
   {
   public:
       static bool isValidTimeSignature(const juce::String& timeSignature)
       {
           auto parts = juce::StringArray::fromTokens(timeSignature, "/", "");

           if (parts.size() != 2)
               return false;

           auto numerator = parts[0].getIntValue();
           auto denominator = parts[1].getIntValue();

           if (numerator < Validation::MIN_TIME_SIG_NUMERATOR ||
               numerator > Validation::MAX_TIME_SIG_NUMERATOR)
               return false;

           if (!isPowerOfTwo(denominator) ||
               denominator < Validation::MIN_TIME_SIG_DENOMINATOR ||
               denominator > Validation::MAX_TIME_SIG_DENOMINATOR)
               return false;

           return true;
       }

       static bool isCommonTimeSignature(const juce::String& timeSignature)
       {
           static const juce::StringArray COMMON_TIME_SIGNATURES = {
               "4/4", "3/4", "6/8", "2/4", "5/4", "7/8", "9/8", "12/8",
               "2/2", "3/8", "5/8", "7/4", "11/8", "13/8", "15/8"
           };

           return COMMON_TIME_SIGNATURES.contains(timeSignature);
       }

       static juce::StringArray getSuggestedTimeSignatures()
       {
           return {
               "4/4", "3/4", "6/8", "2/4", "2/2",
               "5/4", "7/8", "5/8", "7/4",
               "9/8", "12/8", "11/8", "13/8", "15/8",
               "3/8", "4/8", "6/4", "9/4",
               "15/16", "17/16", "19/16", "21/16"
           };
       }

       struct TimeSignatureInfo
       {
           int numerator;
           int denominator;
           float beatsPerBar;
           bool isCompound;
           bool isOddMeter;
           juce::String grouping;
       };

       static TimeSignatureInfo parseTimeSignature(const juce::String& timeSignature)
       {
           TimeSignatureInfo info;

           auto parts = juce::StringArray::fromTokens(timeSignature, "/", "");
           if (parts.size() == 2)
           {
               info.numerator = parts[0].getIntValue();
               info.denominator = parts[1].getIntValue();

               if (info.numerator % 3 == 0 && info.numerator > 3 && info.denominator >= 8)
               {
                   info.isCompound = true;
                   info.beatsPerBar = static_cast<float>(info.numerator) / 3.0f;
               }
               else
               {
                   info.isCompound = false;
                   info.beatsPerBar = static_cast<float>(info.numerator);
               }

               info.isOddMeter = (info.numerator % 2 != 0 && info.numerator != 3) ||
                                 (info.numerator == 5 || info.numerator == 7 || info.numerator == 11);

               info.grouping = getDefaultGrouping(info.numerator, info.denominator);
           }

           return info;
       }

   private:
       static bool isPowerOfTwo(int n)
       {
           return n > 0 && (n & (n - 1)) == 0;
       }

       static juce::String getDefaultGrouping(int numerator, int denominator)
       {
           switch (numerator)
           {
               case 5:  return "3+2";
               case 7:  return "3+2+2";
               case 9:  return denominator >= 8 ? "3+3+3" : "3+3+3";
               case 11: return "3+3+3+2";
               case 13: return "3+3+3+2+2";
               case 15: return denominator >= 8 ? "3+3+3+3+3" : "4+4+4+3";
               default: return "";
           }
       }
   };

   static const juce::StringArray VALID_TIME_SIGNATURES = {"4/4", "3/4", "6/8", "2/4", "5/4", "7/8", "9/8", "12/8"};
   static const juce::StringArray KIT_TYPES = {"Acoustic", "Electronic", "808", "909", "Vintage", "Custom"};
   static const juce::StringArray MANUFACTURERS = {"Roland", "Native Instruments", "Ableton", "Custom", "User"};

   inline int clampCountInBars(int bars) {
       return juce::jlimit(Validation::MIN_COUNT_IN_BARS, Validation::MAX_COUNT_IN_BARS, bars);
   }

   inline juce::File getOTTODataDirectory() {
       juce::File documentsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
       return documentsDir.getChildFile(OTTO_DATA_FOLDER);
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
       return getOTTODataDirectory().getChildFile(MIXING_FOLDER);
   }

   inline juce::File getSystemDirectory() {
       return getOTTODataDirectory().getChildFile(SYSTEM_FOLDER);
   }

   inline juce::File getPresetsDirectory() {
       return getOTTODataDirectory().getChildFile(PRESETS_FOLDER);
   }

   inline juce::File getINIFilePath(const juce::String& filename) {
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

   inline juce::File getPresetsFile() {
       return getPerformanceDirectory().getChildFile(PRESETS_FILE);
   }

   inline juce::File getPlayersFile() {
       return getPerformanceDirectory().getChildFile(PLAYERS_FILE);
   }

   inline juce::File getPatternChainsFile() {
       return getPerformanceDirectory().getChildFile(PATTERN_CHAINS_FILE);
   }

   inline juce::File getPatternGroupsFile() {
       return getPatternsDirectory().getChildFile(PATTERN_GROUPS_FILE);
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

   inline bool isValidThemeName(const juce::String& themeName) {
       if (themeName.isEmpty() || themeName.length() > Validation::MAX_THEME_NAME_LENGTH) {
           return false;
       }
       return true;
   }

   inline bool isValidThemeID(int id) {
       return id >= Validation::MIN_THEME_ID && id <= Validation::MAX_THEME_ID;
   }

   inline bool isValidTempo(int tempo) {
       return tempo >= Validation::MIN_TEMPO && tempo <= Validation::MAX_TEMPO;
   }

   inline bool isValidTempo(float tempo) {
       return tempo >= static_cast<float>(Validation::MIN_TEMPO) && tempo <= static_cast<float>(Validation::MAX_TEMPO);
  }

  inline bool isValidSampleRate(int sampleRate) {
      return sampleRate >= Validation::MIN_AUDIO_SAMPLE_RATE &&
             sampleRate <= Validation::MAX_AUDIO_SAMPLE_RATE;
  }

  inline bool isValidBufferSize(int bufferSize) {
      return bufferSize >= Validation::MIN_BUFFER_SIZE &&
             bufferSize <= Validation::MAX_BUFFER_SIZE &&
             (bufferSize & (bufferSize - 1)) == 0;
  }

  inline bool isValidInterfaceScale(float scale) {
      return scale >= Validation::MIN_INTERFACE_SCALE &&
             scale <= Validation::MAX_INTERFACE_SCALE;
  }

  inline bool isValidVolume(float volume) {
      return volume >= Validation::MIN_VOLUME && volume <= Validation::MAX_VOLUME;
  }

  inline bool isValidQuantizeValue(int value) {
      return value >= Validation::MIN_QUANTIZE_VALUE &&
             value <= Validation::MAX_QUANTIZE_VALUE;
  }

  inline bool isValidSettingsID(int id) {
      return id >= Validation::MIN_SETTINGS_ID && id <= Validation::MAX_SETTINGS_ID;
  }

  inline bool isValidPlayerIndex(int index) {
      return index >= Validation::MIN_PLAYER_INDEX && index <= Validation::MAX_PLAYER_INDEX;
  }

  inline bool isValidButtonIndex(int index) {
      return index >= Validation::MIN_BUTTON_INDEX && index <= Validation::MAX_BUTTON_INDEX;
  }

  inline bool isValidPresetIndex(int index) {
      return index >= Validation::MIN_PRESET_ID && index <= Validation::MAX_PRESET_ID;
  }

  inline bool isValidSwing(float swing) {
      return swing >= Validation::MIN_SWING && swing <= Validation::MAX_SWING;
  }

  inline bool isValidEnergy(float energy) {
      return energy >= Validation::MIN_ENERGY && energy <= Validation::MAX_ENERGY;
  }

  inline bool isValidMidiNote(int note) {
      return note >= Validation::MIN_MIDI_NOTE && note <= Validation::MAX_MIDI_NOTE;
  }

  inline bool isValidMidiChannel(int channel) {
      return channel >= Validation::MIN_MIDI_CHANNEL && channel <= Validation::MAX_MIDI_CHANNEL;
  }

  inline bool isValidPatternLength(int length) {
      return length >= Validation::MIN_PATTERN_LENGTH && length <= Validation::MAX_PATTERN_LENGTH;
  }

  inline bool isValidTimeSignature(const juce::String& timeSig) {
      return TimeSignature::isValidTimeSignature(timeSig);
  }

  inline bool isValidPan(float pan) {
      return pan >= Validation::MIN_PAN && pan <= Validation::MAX_PAN;
  }

  inline bool isValidEQFreq(float freq) {
      return freq >= Validation::MIN_EQ_FREQ && freq <= Validation::MAX_EQ_FREQ;
  }

  inline bool isValidEQGain(float gain) {
      return gain >= Validation::MIN_EQ_GAIN && gain <= Validation::MAX_EQ_GAIN;
  }

  inline bool isValidEQQ(float q) {
      return q >= Validation::MIN_EQ_Q && q <= Validation::MAX_EQ_Q;
  }

  inline bool isValidLoopPosition(float position) {
      return position >= Validation::MIN_LOOP_POSITION && position <= Validation::MAX_LOOP_POSITION;
  }

  inline bool isValidHexColor(const juce::String& color) {
      if (!color.startsWith("#") || color.length() != UI::HEX_COLOR_LENGTH) return false;
      for (int i = UI::HEX_COLOR_START_INDEX; i < color.length(); ++i) {
          char c = color[i];
          if (!((c >= MIDI::HEX_DIGIT_0 && c <= MIDI::HEX_DIGIT_9) ||
                (c >= MIDI::HEX_DIGIT_A_UPPER && c <= MIDI::HEX_DIGIT_F_UPPER) ||
                (c >= MIDI::HEX_DIGIT_A_LOWER && c <= MIDI::HEX_DIGIT_F_LOWER))) {
              return false;
          }
      }
      return true;
  }

  inline bool isValidISODate(const juce::String& dateStr) {
      if (dateStr.length() != UI::ISO_DATE_LENGTH) return false;
      if (dateStr[UI::ISO_DATE_YEAR_POS] != UI::ISO_DATE_SEPARATOR ||
          dateStr[UI::ISO_DATE_MONTH_POS] != UI::ISO_DATE_SEPARATOR ||
          dateStr[UI::ISO_DATE_DAY_POS] != UI::ISO_DATE_SEPARATOR ||
          dateStr[UI::ISO_DATE_HOUR_POS] != UI::ISO_DATE_SEPARATOR ||
          dateStr[UI::ISO_DATE_MINUTE_POS] != UI::ISO_DATE_SEPARATOR) return false;
      return true;
  }

  inline float clampScale(float scale) {
      return juce::jlimit(Validation::MIN_INTERFACE_SCALE, Validation::MAX_INTERFACE_SCALE, scale);
  }

  inline int clampTempo(int tempo) {
      return juce::jlimit(Validation::MIN_TEMPO, Validation::MAX_TEMPO, tempo);
  }

  inline float clampTempo(float tempo) {
      return juce::jlimit(static_cast<float>(Validation::MIN_TEMPO),
                         static_cast<float>(Validation::MAX_TEMPO), tempo);
  }

  inline float clampVolume(float volume) {
      return juce::jlimit(Validation::MIN_VOLUME, Validation::MAX_VOLUME, volume);
  }

  inline int clampPlayerIndex(int index) {
      return juce::jlimit(Validation::MIN_PLAYER_INDEX, Validation::MAX_PLAYER_INDEX, index);
  }

  inline int clampButtonIndex(int index) {
      return juce::jlimit(Validation::MIN_BUTTON_INDEX, Validation::MAX_BUTTON_INDEX, index);
  }

  inline float clampSwing(float swing) {
      return juce::jlimit(Validation::MIN_SWING, Validation::MAX_SWING, swing);
  }

  inline float clampEnergy(float energy) {
      return juce::jlimit(Validation::MIN_ENERGY, Validation::MAX_ENERGY, energy);
  }

  inline int clampPresetIndex(int index) {
      return juce::jlimit(Validation::MIN_PRESET_ID, Validation::MAX_PRESET_ID, index);
  }

  inline int clampMidiNote(int note) {
      return juce::jlimit(Validation::MIN_MIDI_NOTE, Validation::MAX_MIDI_NOTE, note);
  }

  inline int clampMidiChannel(int channel) {
      return juce::jlimit(Validation::MIN_MIDI_CHANNEL, Validation::MAX_MIDI_CHANNEL, channel);
  }

  inline float clampPan(float pan) {
      return juce::jlimit(Validation::MIN_PAN, Validation::MAX_PAN, pan);
  }

  inline int clampPhosphorWeight(int weight) {
      return juce::jlimit(Validation::MIN_PHOSPHOR_WEIGHT, Validation::MAX_PHOSPHOR_WEIGHT, weight);
  }

  inline float clampPhosphorIconBoxFitRatio(float ratio) {
      return juce::jlimit(Validation::MIN_PHOSPHOR_ICON_BOX_FIT_RATIO, Validation::MAX_PHOSPHOR_ICON_BOX_FIT_RATIO, ratio);
  }

  inline float clampLoopPosition(float position) {
      return juce::jlimit(Validation::MIN_LOOP_POSITION, Validation::MAX_LOOP_POSITION, position);
  }

  inline int clampWindowSize(int size) {
      return juce::jlimit(Validation::MIN_WINDOW_SIZE, Validation::MAX_WINDOW_SIZE, size);
  }

  inline bool createDirectoryStructure() {
      bool success = true;

      auto dataDir = getOTTODataDirectory();
      if (!dataDir.exists()) success = success && dataDir.createDirectory().wasOk();

      auto settingsDir = getSettingsDirectory();
      if (!settingsDir.exists()) success = success && settingsDir.createDirectory().wasOk();

      auto performanceDir = getPerformanceDirectory();
      if (!performanceDir.exists()) success = success && performanceDir.createDirectory().wasOk();

      auto patternsDir = getPatternsDirectory();
      if (!patternsDir.exists()) success = success && patternsDir.createDirectory().wasOk();

      auto kitsDir = getKitsDirectory();
      if (!kitsDir.exists()) success = success && kitsDir.createDirectory().wasOk();

      auto mixingDir = getMixingDirectory();
      if (!mixingDir.exists()) success = success && mixingDir.createDirectory().wasOk();

      auto systemDir = getSystemDirectory();
      if (!systemDir.exists()) success = success && systemDir.createDirectory().wasOk();

      return success;
  }
}

