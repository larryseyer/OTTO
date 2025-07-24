#include "PatternManager.h"

void PatternManager::loadPattern(const juce::MidiMessageSequence& pattern) {
    currentPattern = pattern;

    if (currentPattern.getNumEvents() > 0) {
        currentPattern.sort();
        currentPattern.updateMatchedPairs();
    }
}

void PatternManager::savePattern(const juce::String& name, const juce::MidiMessageSequence& pattern) {
    currentPattern = pattern;

}

juce::MidiMessageSequence PatternManager::getCurrentPattern() const {
    return currentPattern;
}
