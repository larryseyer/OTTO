#pragma once
#include <JuceHeader.h>
#include "MidiAnalysisTypes.h"
#include "MidiFileManager.h"
#include "INIConfig.h"

class PatternSuggestionEngine {
public:
    enum class Genre {
        Rock, Jazz, Electronic, HipHop, Latin, Funk, Pop, World
    };

    struct SuggestionParams {
        Genre genre = Genre::Rock;
        float complexity = INIConfig::Defaults::DEFAULT_COMPLEXITY;
        float humanization = INIConfig::Defaults::DEFAULT_HUMANIZATION;
        int timeSignature = INIConfig::Defaults::TIME_SIGNATURE_NUMERATOR;
        float tempo = INIConfig::Defaults::DEFAULT_TEMPO;
        int bars = static_cast<int>(INIConfig::Defaults::DEFAULT_PATTERN_BARS);
    };

    struct PatternSuggestion {
        juce::String name;
        juce::MidiMessageSequence pattern;
        float matchScore = INIConfig::Defaults::DEFAULT_MATCH_SCORE;
        MidiGrooveAnalysis analysis;
    };

    PatternSuggestionEngine();
    ~PatternSuggestionEngine() = default;

    juce::Array<PatternSuggestion> suggestPatterns(const SuggestionParams& params, int numSuggestions = INIConfig::Defaults::DEFAULT_NUM_SUGGESTIONS);
    PatternSuggestion generatePattern(const SuggestionParams& params);

    juce::MidiMessageSequence transferStyle(const juce::MidiMessageSequence& source,
                                           const juce::MidiMessageSequence& styleReference);

    juce::Array<PatternSuggestion> findSimilarGrooves(const MidiGrooveAnalysis& targetGroove,
                                                      const juce::Array<MidiGrooveAnalysis>& library);

    void adaptToPerformance(const juce::Array<float>& recentVelocities,
                           const juce::Array<float>& recentTimings);

    void learnFromPattern(const juce::MidiMessageSequence& pattern, Genre genre);
    void saveLearnedData(const juce::File& file);
    void loadLearnedData(const juce::File& file);

private:
    struct GenreProfile {
        Genre genre;
        float avgSwing = INIConfig::Defaults::SWING;
        float avgVelocity = static_cast<float>(INIConfig::Defaults::FIXED_VELOCITY);
        float complexity = INIConfig::Defaults::DEFAULT_COMPLEXITY;
        float grooveTightness = INIConfig::Audio::DEFAULT_GROOVE_TIGHTNESS;
        juce::Array<float> velocityDistribution;
        juce::Array<float> timingPatterns;
        juce::Array<int> commonNotes;
    };

    juce::Array<GenreProfile> genreProfiles;
    juce::Array<MidiGrooveAnalysis> patternLibrary;

    juce::MidiMessageSequence generateDrumPattern(const GenreProfile& profile,
                                                  int bars, int timeSignature);

    float calculatePatternSimilarity(const MidiGrooveAnalysis& a,
                                   const MidiGrooveAnalysis& b);

    juce::Array<float> generateKickPattern(Genre genre, int steps);
    juce::Array<float> generateSnarePattern(Genre genre, int steps);
    juce::Array<float> generateHiHatPattern(Genre genre, int steps);

    void initializeGenreProfiles();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternSuggestionEngine)
};
