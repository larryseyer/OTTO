#pragma once
#include <JuceHeader.h>
#include "MidiAnalysisTypes.h"
#include "MidiFileManager.h"
#include "INIConfig.h"
#include "ErrorHandling.h"

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
        
        // Validation method for exception safety
        bool isValid() const noexcept {
            return complexity >= 0.0f && complexity <= 1.0f &&
                   humanization >= 0.0f && humanization <= 1.0f &&
                   timeSignature > 0 && timeSignature <= 16 &&
                   tempo > 0.0f && tempo <= 300.0f &&
                   bars > 0 && bars <= 16;
        }
    };

    struct PatternSuggestion {
        juce::String name;
        juce::MidiMessageSequence pattern;
        float matchScore = INIConfig::Defaults::DEFAULT_MATCH_SCORE;
        MidiGrooveAnalysis analysis;
    };

    PatternSuggestionEngine();
    ~PatternSuggestionEngine() = default;

    // Exception-safe pattern generation methods
    juce::Array<PatternSuggestion> suggestPatterns(const SuggestionParams& params, int numSuggestions = INIConfig::Defaults::DEFAULT_NUM_SUGGESTIONS) noexcept;
    PatternSuggestion generatePattern(const SuggestionParams& params) noexcept;
    
    // Fallback methods for when AI generation fails
    PatternSuggestion createFallbackPattern(const SuggestionParams& params) const noexcept;
    juce::Array<PatternSuggestion> createBasicPatterns(Genre genre, int numSuggestions) const noexcept;

    // Exception-safe advanced methods
    juce::MidiMessageSequence transferStyle(const juce::MidiMessageSequence& source,
                                           const juce::MidiMessageSequence& styleReference) noexcept;

    juce::Array<PatternSuggestion> findSimilarGrooves(const MidiGrooveAnalysis& targetGroove,
                                                      const juce::Array<MidiGrooveAnalysis>& library) noexcept;

    bool adaptToPerformance(const juce::Array<float>& recentVelocities,
                           const juce::Array<float>& recentTimings) noexcept;

    bool learnFromPattern(const juce::MidiMessageSequence& pattern, Genre genre) noexcept;
    bool saveLearnedData(const juce::File& file) noexcept;
    bool loadLearnedData(const juce::File& file) noexcept;
    
    // Error state and recovery methods
    bool hasError() const noexcept { return hasInternalError; }
    juce::String getLastError() const noexcept { return lastErrorMessage; }
    void clearError() noexcept { hasInternalError = false; lastErrorMessage.clear(); }

private:
    struct GenreProfile {
        Genre genre;
        float avgSwing = INIConfig::Defaults::SWING;
        float avgVelocity = static_cast<float>(INIConfig::Defaults::FIXED_VELOCITY);
        float complexity = INIConfig::Defaults::DEFAULT_COMPLEXITY;
        float grooveTightness = INIConfig::Defaults::DEFAULT_GROOVE_TIGHTNESS;
        juce::Array<float> velocityDistribution;
        juce::Array<float> timingPatterns;
        juce::Array<int> commonNotes;
    };

    juce::Array<GenreProfile> genreProfiles;
    juce::Array<MidiGrooveAnalysis> patternLibrary;
    
    // Error state management
    mutable bool hasInternalError = false;
    mutable juce::String lastErrorMessage;

    // Exception-safe internal methods
    juce::MidiMessageSequence generateDrumPattern(const GenreProfile& profile,
                                                  int bars, int timeSignature) noexcept;

    float calculatePatternSimilarity(const MidiGrooveAnalysis& a,
                                   const MidiGrooveAnalysis& b) const noexcept;

    juce::Array<float> generateKickPattern(Genre genre, int steps) const noexcept;
    juce::Array<float> generateSnarePattern(Genre genre, int steps) const noexcept;
    juce::Array<float> generateHiHatPattern(Genre genre, int steps) const noexcept;

    void initializeGenreProfiles() noexcept;
    
    // Error handling utilities
    void setError(const juce::String& message) const noexcept;
    bool validateParams(const SuggestionParams& params) const noexcept;
    SuggestionParams sanitizeParams(const SuggestionParams& params) const noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternSuggestionEngine)
};
