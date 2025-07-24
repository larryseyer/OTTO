#pragma once
#include <JuceHeader.h>
#include "Mixer.h"
#include "INIConfig.h"

class SFZEngine;

class AutoMixAssistant {
public:
    struct MixSuggestion {
        float channelVolumes[INIConfig::Defaults::MAX_PLAYERS];
        float channelPans[INIConfig::Defaults::MAX_PLAYERS];
        float eqSettings[INIConfig::Defaults::MAX_PLAYERS][INIConfig::Audio::NUM_EQ_BANDS];
        float compressionSettings[INIConfig::Defaults::MAX_PLAYERS];
        float confidence = INIConfig::Validation::MIN_VOLUME;
        juce::String description;
    };

    struct MixPreset {
        juce::String name;
        juce::String type;
        float channelVolumes[INIConfig::Defaults::MAX_PLAYERS];
        float channelPans[INIConfig::Defaults::MAX_PLAYERS];
        bool channelMutes[INIConfig::Defaults::MAX_PLAYERS];
        bool channelSolos[INIConfig::Defaults::MAX_PLAYERS];
        float eqSettings[INIConfig::Defaults::MAX_PLAYERS][INIConfig::Audio::NUM_EQ_BANDS];
        float sendLevels[INIConfig::Defaults::MAX_PLAYERS][INIConfig::Audio::NUM_SEND_TYPES];
        float masterVolume;
        float masterEQ[INIConfig::Audio::NUM_EQ_BANDS];
        juce::Time createdTime;
    };

    struct RoomAnalysis {
        float lowFreqResonance = INIConfig::Validation::MIN_VOLUME;
        float highFreqAbsorption = INIConfig::Validation::MIN_VOLUME;
        float roomSize = INIConfig::Defaults::DEFAULT_ROOM_SIZE;
    };

    AutoMixAssistant(Mixer& mixerRef, SFZEngine& sfzEngineRef);
    ~AutoMixAssistant();

    MixSuggestion analyzeMix();
    void applyMixSuggestion(const MixSuggestion& suggestion, float blendAmount = INIConfig::Validation::MAX_VOLUME);

    MixPreset getCurrentMixAsPreset(const juce::String& name);
    void loadMixPreset(const MixPreset& preset);

    float analyzeMixBalance();
    void createSpace();
    void preventFrequencyMasking();
    void applyGenreSpecificMixing(const juce::String& genre);
    void optimizeHeadroom();
    void adaptMixToRoom(const RoomAnalysis& roomAnalysis);
    void learnFromUserAdjustments();

    juce::Array<MixPreset> getPresets() const { return mixPresets; }

private:
    Mixer& mixer;
    SFZEngine& sfzEngine;
    juce::Array<MixPreset> mixPresets;

    struct MixAnalysis {
        struct FrequencyBalance {
            float low = INIConfig::Validation::MIN_VOLUME;
            float mid = INIConfig::Validation::MIN_VOLUME;
            float high = INIConfig::Validation::MIN_VOLUME;
        } frequencyBalance;

        float stereoWidth = INIConfig::Validation::MIN_VOLUME;
        float dynamicRange = INIConfig::Validation::MIN_VOLUME;
        int activeChannels = INIConfig::Validation::MIN_PLAYER_INDEX;
        bool channelActivity[INIConfig::Defaults::MAX_PLAYERS] = {false};
    };

    struct UserPreferences {
        float volumeTendencies[INIConfig::Defaults::MAX_PLAYERS] = {INIConfig::Validation::MIN_VOLUME};
        float panTendencies[INIConfig::Defaults::MAX_PLAYERS] = {INIConfig::Validation::MIN_VOLUME};
    };

    UserPreferences userPreferences;
    juce::Array<MixPreset> userAdjustmentHistory;

    void initializePresets();
    MixAnalysis analyzeCurrentMix();
    MixSuggestion generateSuggestions(const MixAnalysis& analysis);
    float calculateConfidence(const MixAnalysis& analysis);
    float calculateStereoWidth();
    float calculateDynamicRange();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutoMixAssistant)
};
