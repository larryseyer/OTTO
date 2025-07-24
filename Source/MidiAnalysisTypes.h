#pragma once
#include <JuceHeader.h>
#include "INIConfig.h"

struct MidiGrooveAnalysis {
    float averageSwing = INIConfig::Defaults::SWING;
    float averageVelocity = static_cast<float>(INIConfig::Defaults::FIXED_VELOCITY);
    float grooveTightness = INIConfig::Audio::DEFAULT_GROOVE_TIGHTNESS;
    int timeSignatureNumerator = INIConfig::Defaults::TIME_SIGNATURE_NUMERATOR;
    int timeSignatureDenominator = INIConfig::Defaults::TIME_SIGNATURE_DENOMINATOR;
    float tempo = INIConfig::Defaults::DEFAULT_TEMPO;
    int numberOfBars = static_cast<int>(INIConfig::Defaults::DEFAULT_PATTERN_BARS);

    float velocityRange = INIConfig::MIDI::DEFAULT_VELOCITY_RANGE;
    float velocityVariation = INIConfig::MIDI::DEFAULT_VELOCITY_VARIATION;

    float timingDeviation = INIConfig::MIDI::DEFAULT_TIMING_DEVIATION;
    juce::Array<float> microTiming;

    float noteDensity = INIConfig::MIDI::DEFAULT_NOTE_DENSITY;
    juce::Array<int> noteDistribution;
};
