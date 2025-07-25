#pragma once
#include <JuceHeader.h>
#include "MidiAnalysisTypes.h"
#include "PatternSuggestionEngine.h"
#include "ErrorHandling.h"
#include <unordered_map>
#include <vector>
#include <memory>

class AdvancedAILearning {
public:
    struct UserPerformanceProfile {
        float averageVelocity = 80.0f;
        float timingConsistency = 0.8f;
        float preferredComplexity = 0.5f;
        juce::Array<PatternSuggestionEngine::Genre> favoriteGenres;
        juce::Array<int> commonDrumNotes;
        float adaptationRate = 0.1f;
        int totalPatternsPlayed = 0;
        double totalPlayTime = 0.0;
        juce::Time lastActivity;
    };

    struct PatternUsageStats {
        int timesUsed = 0;
        float averageRating = 0.0f;
        juce::Array<float> userRatings;
        double totalPlayTime = 0.0;
        juce::Time lastUsed;
        bool isFavorite = false;
    };

    struct GenreLearningData {
        PatternSuggestionEngine::Genre genre;
        juce::Array<juce::MidiMessageSequence> learnedPatterns;
        juce::Array<MidiGrooveAnalysis> grooveVariations;
        float userPreferenceScore = 0.5f;
        int totalExposure = 0;
        std::unordered_map<int, float> instrumentPreferences;
        juce::Array<float> complexityDistribution;
        
        GenreLearningData() = default;
        GenreLearningData(const GenreLearningData& other) 
            : genre(other.genre)
            , learnedPatterns(other.learnedPatterns)
            , grooveVariations(other.grooveVariations)
            , userPreferenceScore(other.userPreferenceScore)
            , totalExposure(other.totalExposure)
            , instrumentPreferences(other.instrumentPreferences)
            , complexityDistribution(other.complexityDistribution) {}
            
        GenreLearningData& operator=(const GenreLearningData& other) {
            if (this != &other) {
                genre = other.genre;
                learnedPatterns = other.learnedPatterns;
                grooveVariations = other.grooveVariations;
                userPreferenceScore = other.userPreferenceScore;
                totalExposure = other.totalExposure;
                instrumentPreferences = other.instrumentPreferences;
                complexityDistribution = other.complexityDistribution;
            }
            return *this;
        }
    };

    struct RealTimeAdaptation {
        bool isActive = false;
        float currentEnergyLevel = 0.5f;
        float tempoTrend = 0.0f;
        float velocityTrend = 0.0f;
        juce::Array<float> recentPerformanceScores;
        int consecutiveGoodBeats = 0;
        int consecutiveMissedBeats = 0;
        double adaptationConfidence = 0.5;
    };

    AdvancedAILearning();
    ~AdvancedAILearning() = default;

    void updateUserProfile(const juce::MidiMessageSequence& userInput, 
                          PatternSuggestionEngine::Genre genre,
                          double playDuration);

    void learnFromUserFeedback(const juce::String& patternId, 
                              float rating, 
                              bool isFavorite = false);

    void adaptToRealTimePerformance(const juce::Array<juce::MidiMessage>& recentInput,
                                   double timeWindow = 2.0);

    PatternSuggestionEngine::SuggestionParams getPersonalizedSuggestionParams(
        PatternSuggestionEngine::Genre genre) const;

    juce::Array<PatternSuggestionEngine::PatternSuggestion> 
    generatePersonalizedPatterns(const PatternSuggestionEngine::SuggestionParams& baseParams,
                                int numSuggestions = 5);

    void analyzePlayingStyle(const juce::Array<juce::MidiMessage>& midiData,
                           double timeSpan);

    float calculatePatternCompatibility(const juce::MidiMessageSequence& pattern,
                                      PatternSuggestionEngine::Genre genre) const;

    void enableRealTimeAdaptation(bool enable);
    RealTimeAdaptation getCurrentAdaptationState() const { return realTimeState; }

    void saveUserProfile(const juce::File& file);
    void loadUserProfile(const juce::File& file);

    void resetLearning();
    void exportLearningData(const juce::File& exportFile);

    UserPerformanceProfile getUserProfile() const { return userProfile; }
    juce::Array<GenreLearningData> getGenreLearningData() const;

private:
    UserPerformanceProfile userProfile;
    juce::HashMap<juce::String, PatternUsageStats> patternStats;
    juce::Array<GenreLearningData> genreLearning;
    RealTimeAdaptation realTimeState;

    juce::Array<juce::MidiMessage> recentMidiBuffer;
    static constexpr int MAX_MIDI_BUFFER_SIZE = 1000;
    static constexpr double LEARNING_DECAY_RATE = 0.95;
    static constexpr float MIN_ADAPTATION_CONFIDENCE = 0.3f;

    void updateGenreLearning(PatternSuggestionEngine::Genre genre,
                           const juce::MidiMessageSequence& pattern);

    void analyzeVelocityPatterns(const juce::Array<juce::MidiMessage>& midiData);
    void analyzeTimingPatterns(const juce::Array<juce::MidiMessage>& midiData);
    void analyzeInstrumentUsage(const juce::Array<juce::MidiMessage>& midiData);

    float calculatePerformanceScore(const juce::Array<juce::MidiMessage>& midiData,
                                  double timeWindow) const;

    void adaptComplexityPreference(float performanceScore);
    void adaptTempoPreference(const juce::Array<juce::MidiMessage>& midiData);

    GenreLearningData* findOrCreateGenreLearning(PatternSuggestionEngine::Genre genre);

    juce::MidiMessageSequence generateVariation(const juce::MidiMessageSequence& basePattern,
                                               float variationAmount) const;

    void pruneOldData();
    void normalizePreferences();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdvancedAILearning)
};

class CollaborativeAI {
public:
    struct CommunityPattern {
        juce::String id;
        juce::String name;
        juce::MidiMessageSequence pattern;
        PatternSuggestionEngine::Genre genre;
        float communityRating = 0.0f;
        int downloadCount = 0;
        juce::String creatorId;
        juce::Time uploadTime;
        juce::Array<juce::String> tags;
        MidiGrooveAnalysis analysis;
    };

    struct UserContribution {
        juce::String patternId;
        juce::Time contributionTime;
        float communityFeedback = 0.0f;
        int downloads = 0;
        bool isFeatured = false;
    };

    CollaborativeAI();
    ~CollaborativeAI() = default;

    void sharePattern(const juce::MidiMessageSequence& pattern,
                     const juce::String& name,
                     PatternSuggestionEngine::Genre genre,
                     const juce::Array<juce::String>& tags = {});

    juce::Array<CommunityPattern> searchCommunityPatterns(
        PatternSuggestionEngine::Genre genre = PatternSuggestionEngine::Genre::Rock,
        const juce::String& searchQuery = "",
        int maxResults = 20);

    juce::Array<CommunityPattern> getRecommendedPatterns(
        const AdvancedAILearning::UserPerformanceProfile& userProfile,
        int maxResults = 10);

    void ratePattern(const juce::String& patternId, float rating);
    void downloadPattern(const juce::String& patternId);

    bool isConnected() const { return cloudConnected; }
    void setCloudConnection(bool connected) { cloudConnected = connected; }

    juce::Array<UserContribution> getUserContributions() const { return userContributions; }

private:
    bool cloudConnected = false;
    juce::Array<CommunityPattern> localPatternCache;
    juce::Array<UserContribution> userContributions;
    juce::String userId;

    void syncWithCloud();
    void cachePattern(const CommunityPattern& pattern);
    float calculatePatternSimilarity(const CommunityPattern& a, const CommunityPattern& b) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CollaborativeAI)
};

class IntelligentPatternGenerator {
public:
    struct GenerationConstraints {
        PatternSuggestionEngine::Genre primaryGenre = PatternSuggestionEngine::Genre::Rock;
        juce::Array<PatternSuggestionEngine::Genre> influenceGenres;
        float crossGenreBlend = 0.0f;
        int minComplexity = 1;
        int maxComplexity = 10;
        bool allowPolyrhythms = false;
        bool enforceGroovePocket = true;
        juce::Array<int> requiredInstruments;
        juce::Array<int> forbiddenInstruments;
        float humanizationLevel = 0.5f;
        bool adaptToUserStyle = true;
    };

    struct PatternAnalysis {
        float rhythmicComplexity = 0.0f;
        float melodicContent = 0.0f;
        float grooveFactor = 0.0f;
        float energyLevel = 0.0f;
        juce::Array<int> dominantInstruments;
        float polyrhythmicContent = 0.0f;
        int timeSignatureComplexity = 0;
        float swingAmount = 0.0f;
        float velocityVariation = 0.0f;
        float timingTightness = 0.0f;
    };

    IntelligentPatternGenerator();
    ~IntelligentPatternGenerator() = default;

    juce::MidiMessageSequence generateIntelligentPattern(
        const GenerationConstraints& constraints,
        const AdvancedAILearning::UserPerformanceProfile& userProfile);

    juce::Array<juce::MidiMessageSequence> generatePatternVariations(
        const juce::MidiMessageSequence& basePattern,
        int numVariations = 5,
        float variationIntensity = 0.5f);

    juce::MidiMessageSequence blendPatterns(
        const juce::Array<juce::MidiMessageSequence>& patterns,
        const juce::Array<float>& weights);

    PatternAnalysis analyzePattern(const juce::MidiMessageSequence& pattern) const;

    juce::MidiMessageSequence adaptPatternToUser(
        const juce::MidiMessageSequence& pattern,
        const AdvancedAILearning::UserPerformanceProfile& userProfile) const;

    void setCreativityLevel(float level) { creativityLevel = juce::jlimit(0.0f, 1.0f, level); }
    float getCreativityLevel() const { return creativityLevel; }

private:
    float creativityLevel = 0.5f;
    mutable juce::Random randomGenerator;

    juce::MidiMessageSequence generateRhythmicFoundation(
        PatternSuggestionEngine::Genre genre,
        int bars, int timeSignature) const;

    void addGenreSpecificElements(juce::MidiMessageSequence& pattern,
                                PatternSuggestionEngine::Genre genre) const;

    void applyHumanization(juce::MidiMessageSequence& pattern,
                         float humanizationLevel) const;

    void applyUserStyleAdaptation(juce::MidiMessageSequence& pattern,
                                const AdvancedAILearning::UserPerformanceProfile& userProfile) const;

    juce::MidiMessageSequence createPolyrhythmicLayer(
        const juce::MidiMessageSequence& basePattern,
        int polyrhythmRatio) const;

    void enforceGroovePocket(juce::MidiMessageSequence& pattern) const;

    float calculateRhythmicComplexity(const juce::MidiMessageSequence& pattern) const;
    float calculateGrooveFactor(const juce::MidiMessageSequence& pattern) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IntelligentPatternGenerator)
};
