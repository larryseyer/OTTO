#pragma once

#include <JuceHeader.h>

class PatternManager {
public:
    PatternManager() = default;
    ~PatternManager() = default;

    void loadPattern(const juce::MidiMessageSequence& pattern);
    void savePattern(const juce::String& name, const juce::MidiMessageSequence& pattern);
    juce::MidiMessageSequence getCurrentPattern() const;

private:
    juce::MidiMessageSequence currentPattern;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternManager)
};
