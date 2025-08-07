#pragma once
#include <JuceHeader.h>
#include <memory>
#include "INIConfig.h"

class SFZVoice {
public:
    enum class State {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release,
        Finished
    };

    struct ADSRParameters {
        float attackTime = INIConfig::Audio::DEFAULT_ATTACK_TIME;
        float decayTime = INIConfig::Audio::DEFAULT_DECAY_TIME;
        float sustainLevel = INIConfig::Audio::DEFAULT_SUSTAIN_LEVEL;
        float releaseTime = INIConfig::Audio::DEFAULT_RELEASE_TIME;
    };

    SFZVoice();
    ~SFZVoice() = default;

    void startNote(int midiNote, float velocity, double sampleRate,
                   juce::AudioFormatReaderSource* source,
                   const ADSRParameters& adsr, float volumeDb);
    void stopNote();
    void reset();

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

    bool isActive() const { return state != State::Idle && state != State::Finished; }
    bool isReleasing() const { return state == State::Release; }
    bool canBeStolen() const;
    int getCurrentNote() const { return currentNote; }
    float getVelocity() const { return velocity; }
    State getState() const { return state; }
    int64_t getStartTime() const { return startTime; }

private:
    State state = State::Idle;
    int currentNote = INIConfig::MIDI::INACTIVE_PATTERN;
    float velocity = INIConfig::Validation::MIN_VOLUME;
    int64_t startTime = INIConfig::Defaults::ZERO_VALUE;
    double sampleRate = static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE);

    juce::AudioFormatReaderSource* audioSource = nullptr;
    juce::AudioBuffer<float> sourceBuffer;
    int64_t sourcePosition = INIConfig::Defaults::ZERO_VALUE;

    ADSRParameters adsrParams;
    float currentEnvelopeValue = INIConfig::Validation::MIN_VOLUME;
    float envelopeIncrement = INIConfig::Validation::MIN_VOLUME;
    float targetEnvelopeValue = INIConfig::Validation::MIN_VOLUME;

    float baseVolume = INIConfig::Defaults::VOLUME;

    void updateEnvelope();
    void calculateEnvelopeIncrement(float targetValue, float timeInSeconds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SFZVoice)
};
