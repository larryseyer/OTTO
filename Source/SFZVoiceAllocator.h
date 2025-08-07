#pragma once
#include "SFZVoice.h"
#include <vector>
#include <memory>
#include "INIConfig.h"

class SFZVoiceAllocator {
public:
    static constexpr int MAX_VOICES = INIConfig::Defaults::MAX_VOICES;
    static constexpr int MAX_VOICES_PER_NOTE = INIConfig::Defaults::MAX_VOICES_PER_NOTE;

    SFZVoiceAllocator();
    ~SFZVoiceAllocator() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void reset();

    SFZVoice* allocateVoice(int midiNote);
    void releaseVoicesForNote(int midiNote);
    void releaseAllVoices();

    void renderNextBlock(juce::AudioBuffer<float>& buffer);

    int getActiveVoiceCount() const;
    void setMaxVoices(int maxVoices) {
        maxPolyphony = juce::jlimit(INIConfig::Defaults::ONE_VALUE, MAX_VOICES, maxVoices);
    }
    int getMaxVoices() const { return maxPolyphony; }

private:
    std::vector<std::unique_ptr<SFZVoice>> voices;
    std::vector<int> roundRobinCounters;
    int maxPolyphony = INIConfig::Audio::NUM_DRUM_PADS;
    double sampleRate = static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE);

    SFZVoice* findIdleVoice();
    SFZVoice* stealVoice();
    SFZVoice* findOldestVoice();
    SFZVoice* findQuietestVoice();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SFZVoiceAllocator)
};
