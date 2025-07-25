#include "AdvancedAILearning.h"
#include "INIConfig.h"
#include <algorithm>
#include <cmath>

AdvancedAILearning::AdvancedAILearning() {
    userProfile.lastActivity = juce::Time::getCurrentTime();
    
    for (int i = 0; i < 8; ++i) {
        GenreLearningData genreData;
        genreData.genre = static_cast<PatternSuggestionEngine::Genre>(i);
        genreLearning.add(genreData);
    }
    
    realTimeState.isActive = false;
    recentMidiBuffer.ensureStorageAllocated(MAX_MIDI_BUFFER_SIZE);
}

void AdvancedAILearning::updateUserProfile(const juce::MidiMessageSequence& userInput,
                                         PatternSuggestionEngine::Genre genre,
                                         double playDuration) {
    try {
        userProfile.totalPatternsPlayed++;
        userProfile.totalPlayTime += playDuration;
        userProfile.lastActivity = juce::Time::getCurrentTime();

        if (!userProfile.favoriteGenres.contains(genre)) {
            userProfile.favoriteGenres.add(genre);
        }

        float totalVelocity = 0.0f;
        int noteCount = 0;
        juce::Array<double> timings;

        for (int i = 0; i < userInput.getNumEvents(); ++i) {
            auto event = userInput.getEventPointer(i);
            if (event->message.isNoteOn()) {
                totalVelocity += event->message.getVelocity();
                noteCount++;
                timings.add(event->message.getTimeStamp());
                
                int note = event->message.getNoteNumber();
                if (!userProfile.commonDrumNotes.contains(note)) {
                    userProfile.commonDrumNotes.add(note);
                }
            }
        }

        if (noteCount > 0) {
            float avgVelocity = totalVelocity / noteCount;
            userProfile.averageVelocity = userProfile.averageVelocity * (1.0f - userProfile.adaptationRate) +
                                        avgVelocity * userProfile.adaptationRate;

            if (timings.size() > 1) {
                float timingVariance = 0.0f;
                double avgTiming = 0.0;
                for (auto timing : timings) {
                    avgTiming += timing;
                }
                avgTiming /= timings.size();

                for (auto timing : timings) {
                    timingVariance += std::pow(timing - avgTiming, 2);
                }
                timingVariance = std::sqrt(timingVariance / timings.size());

                float consistency = juce::jlimit(0.0f, 1.0f, 1.0f - (timingVariance / 100.0f));
                userProfile.timingConsistency = userProfile.timingConsistency * (1.0f - userProfile.adaptationRate) +
                                              consistency * userProfile.adaptationRate;
            }
        }

        updateGenreLearning(genre, userInput);
        
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to update user profile: " + juce::String(e.what()), "AdvancedAILearning");
    }
}

void AdvancedAILearning::learnFromUserFeedback(const juce::String& patternId,
                                             float rating,
                                             bool isFavorite) {
    try {
        if (!patternStats.contains(patternId)) {
            PatternUsageStats stats;
            patternStats.set(patternId, stats);
        }

        auto& stats = patternStats.getReference(patternId);
        stats.userRatings.add(rating);
        stats.isFavorite = isFavorite;
        stats.lastUsed = juce::Time::getCurrentTime();

        float totalRating = 0.0f;
        for (auto r : stats.userRatings) {
            totalRating += r;
        }
        stats.averageRating = totalRating / stats.userRatings.size();

        float complexityFromRating = rating / 5.0f;
        userProfile.preferredComplexity = userProfile.preferredComplexity * 0.9f +
                                        complexityFromRating * 0.1f;
        
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to learn from user feedback: " + juce::String(e.what()), "AdvancedAILearning");
    }
}

void AdvancedAILearning::adaptToRealTimePerformance(const juce::Array<juce::MidiMessage>& recentInput,
                                                   double timeWindow) {
    if (!realTimeState.isActive) return;

    try {
        for (const auto& msg : recentInput) {
            recentMidiBuffer.add(msg);
            if (recentMidiBuffer.size() > MAX_MIDI_BUFFER_SIZE) {
                recentMidiBuffer.removeRange(0, recentMidiBuffer.size() - MAX_MIDI_BUFFER_SIZE);
            }
        }

        float performanceScore = calculatePerformanceScore(recentInput, timeWindow);
        realTimeState.recentPerformanceScores.add(performanceScore);
        
        if (realTimeState.recentPerformanceScores.size() > 20) {
            realTimeState.recentPerformanceScores.removeRange(0, 1);
        }

        if (performanceScore > 0.7f) {
            realTimeState.consecutiveGoodBeats++;
            realTimeState.consecutiveMissedBeats = 0;
        } else if (performanceScore < 0.3f) {
            realTimeState.consecutiveMissedBeats++;
            realTimeState.consecutiveGoodBeats = 0;
        }

        adaptComplexityPreference(performanceScore);
        adaptTempoPreference(recentInput);

        float avgScore = 0.0f;
        for (auto score : realTimeState.recentPerformanceScores) {
            avgScore += score;
        }
        avgScore /= realTimeState.recentPerformanceScores.size();
        
        realTimeState.adaptationConfidence = juce::jlimit(0.0, 1.0, 
            static_cast<double>(avgScore * 0.7 + realTimeState.consecutiveGoodBeats * 0.1));

    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to adapt to real-time performance: " + juce::String(e.what()), "AdvancedAILearning");
    }
}

PatternSuggestionEngine::SuggestionParams 
AdvancedAILearning::getPersonalizedSuggestionParams(PatternSuggestionEngine::Genre genre) const {
    PatternSuggestionEngine::SuggestionParams params;
    params.genre = genre;
    params.complexity = userProfile.preferredComplexity;
    params.humanization = 1.0f - userProfile.timingConsistency;
    params.tempo = INIConfig::Defaults::DEFAULT_TEMPO;

    if (realTimeState.isActive && realTimeState.adaptationConfidence > MIN_ADAPTATION_CONFIDENCE) {
        params.complexity += realTimeState.currentEnergyLevel * 0.2f;
        params.complexity = juce::jlimit(0.0f, 1.0f, params.complexity);
        
        if (realTimeState.tempoTrend > 0.1f) {
            params.tempo *= 1.05f;
        } else if (realTimeState.tempoTrend < -0.1f) {
            params.tempo *= 0.95f;
        }
    }

    return params;
}

juce::Array<PatternSuggestionEngine::PatternSuggestion>
AdvancedAILearning::generatePersonalizedPatterns(const PatternSuggestionEngine::SuggestionParams& baseParams,
                                                int numSuggestions) {
    juce::Array<PatternSuggestionEngine::PatternSuggestion> suggestions;

    try {
        auto* genreLearningData = findOrCreateGenreLearning(baseParams.genre);
        
        for (int i = 0; i < numSuggestions; ++i) {
            PatternSuggestionEngine::PatternSuggestion suggestion;
            suggestion.name = "Personalized " + juce::String(static_cast<int>(baseParams.genre)) + 
                            " Pattern " + juce::String(i + 1);

            float complexityVariation = baseParams.complexity + (i * 0.1f) - 0.2f;
            complexityVariation = juce::jlimit(0.0f, 1.0f, complexityVariation);

            if (!genreLearningData->learnedPatterns.isEmpty()) {
                int patternIndex = i % genreLearningData->learnedPatterns.size();
                suggestion.pattern = generateVariation(genreLearningData->learnedPatterns[patternIndex],
                                                     complexityVariation);
            } else {
                suggestion.pattern = juce::MidiMessageSequence();
                auto noteOn = juce::MidiMessage::noteOn(10, INIConfig::GMDrums::BASS_DRUM_1, 
                                                      static_cast<juce::uint8>(userProfile.averageVelocity));
                noteOn.setTimeStamp(0.0);
                suggestion.pattern.addEvent(noteOn);
            }

            suggestion.matchScore = calculatePatternCompatibility(suggestion.pattern, baseParams.genre);
            
            MidiGrooveAnalysis analysis;
            analysis.averageVelocity = userProfile.averageVelocity;
            analysis.grooveTightness = userProfile.timingConsistency;
            analysis.tempo = baseParams.tempo;
            suggestion.analysis = analysis;

            suggestions.add(suggestion);
        }

    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to generate personalized patterns: " + juce::String(e.what()), "AdvancedAILearning");
    }

    return suggestions;
}

void AdvancedAILearning::enableRealTimeAdaptation(bool enable) {
    realTimeState.isActive = enable;
    if (enable) {
        realTimeState.recentPerformanceScores.clear();
        realTimeState.consecutiveGoodBeats = 0;
        realTimeState.consecutiveMissedBeats = 0;
        realTimeState.adaptationConfidence = 0.5;
    }
}

void AdvancedAILearning::updateGenreLearning(PatternSuggestionEngine::Genre genre,
                                           const juce::MidiMessageSequence& pattern) {
    auto* genreData = findOrCreateGenreLearning(genre);
    if (genreData) {
        genreData->learnedPatterns.add(pattern);
        genreData->totalExposure++;
        genreData->userPreferenceScore = genreData->userPreferenceScore * LEARNING_DECAY_RATE + 0.1f;

        if (genreData->learnedPatterns.size() > 50) {
            genreData->learnedPatterns.removeRange(0, 10);
        }
    }
}

float AdvancedAILearning::calculatePatternCompatibility(const juce::MidiMessageSequence& pattern,
                                                      PatternSuggestionEngine::Genre genre) const {
    float compatibility = 0.5f;

    auto* genreData = const_cast<AdvancedAILearning*>(this)->findOrCreateGenreLearning(genre);
    if (genreData && !genreData->learnedPatterns.isEmpty()) {
        compatibility = genreData->userPreferenceScore;
    }

    int userNoteMatches = 0;
    for (int i = 0; i < pattern.getNumEvents(); ++i) {
        auto event = pattern.getEventPointer(i);
        if (event->message.isNoteOn()) {
            int note = event->message.getNoteNumber();
            if (userProfile.commonDrumNotes.contains(note)) {
                userNoteMatches++;
            }
        }
    }

    if (pattern.getNumEvents() > 0) {
        float noteCompatibility = static_cast<float>(userNoteMatches) / pattern.getNumEvents();
        compatibility = compatibility * 0.7f + noteCompatibility * 0.3f;
    }

    return juce::jlimit(0.0f, 1.0f, compatibility);
}

float AdvancedAILearning::calculatePerformanceScore(const juce::Array<juce::MidiMessage>& midiData,
                                                   double timeWindow) const {
    if (midiData.isEmpty()) return 0.5f;

    float velocityScore = 0.5f;
    float timingScore = 0.5f;
    
    float totalVelocity = 0.0f;
    int noteCount = 0;
    juce::Array<double> timings;

    for (const auto& msg : midiData) {
        if (msg.isNoteOn()) {
            totalVelocity += msg.getVelocity();
            noteCount++;
            timings.add(msg.getTimeStamp());
        }
    }

    if (noteCount > 0) {
        float avgVelocity = totalVelocity / noteCount;
        float velocityDiff = std::abs(avgVelocity - userProfile.averageVelocity);
        velocityScore = juce::jlimit(0.0f, 1.0f, 1.0f - (velocityDiff / 127.0f));

        if (timings.size() > 1) {
            float expectedInterval = timeWindow / timings.size();
            float timingVariance = 0.0f;
            
            for (int i = 1; i < timings.size(); ++i) {
                float actualInterval = timings[i] - timings[i-1];
                timingVariance += std::abs(actualInterval - expectedInterval);
            }
            
            timingVariance /= (timings.size() - 1);
            timingScore = juce::jlimit(0.0f, 1.0f, 1.0f - (timingVariance / expectedInterval));
        }
    }

    return (velocityScore + timingScore) * 0.5f;
}

void AdvancedAILearning::adaptComplexityPreference(float performanceScore) {
    if (performanceScore > 0.8f && realTimeState.consecutiveGoodBeats > 5) {
        userProfile.preferredComplexity = juce::jlimit(0.0f, 1.0f, 
            userProfile.preferredComplexity + 0.05f);
    } else if (performanceScore < 0.4f && realTimeState.consecutiveMissedBeats > 3) {
        userProfile.preferredComplexity = juce::jlimit(0.0f, 1.0f, 
            userProfile.preferredComplexity - 0.1f);
    }
}

void AdvancedAILearning::adaptTempoPreference(const juce::Array<juce::MidiMessage>& midiData) {
    if (midiData.size() < 2) return;

    juce::Array<double> intervals;
    for (int i = 1; i < midiData.size(); ++i) {
        if (midiData[i].isNoteOn() && midiData[i-1].isNoteOn()) {
            intervals.add(midiData[i].getTimeStamp() - midiData[i-1].getTimeStamp());
        }
    }

    if (!intervals.isEmpty()) {
        double avgInterval = 0.0;
        for (auto interval : intervals) {
            avgInterval += interval;
        }
        avgInterval /= intervals.size();

        double expectedInterval = 60.0 / INIConfig::Defaults::DEFAULT_TEMPO;
        realTimeState.tempoTrend = (avgInterval - expectedInterval) / expectedInterval;
    }
}

AdvancedAILearning::GenreLearningData* 
AdvancedAILearning::findOrCreateGenreLearning(PatternSuggestionEngine::Genre genre) {
    for (auto& data : genreLearning) {
        if (data.genre == genre) {
            return &data;
        }
    }
    return nullptr;
}

juce::MidiMessageSequence 
AdvancedAILearning::generateVariation(const juce::MidiMessageSequence& basePattern,
                                    float variationAmount) const {
    juce::MidiMessageSequence variation = basePattern;
    
    juce::Random random;
    
    for (int i = 0; i < variation.getNumEvents(); ++i) {
        auto event = variation.getEventPointer(i);
        if (event->message.isNoteOn() && const_cast<juce::Random&>(random).nextFloat() < variationAmount) {
            int currentVelocity = event->message.getVelocity();
            int newVelocity = currentVelocity + random.nextInt(21) - 10;
            newVelocity = juce::jlimit(1, 127, newVelocity);
            
            event->message = juce::MidiMessage::noteOn(event->message.getChannel(),
                                                     event->message.getNoteNumber(),
                                                     static_cast<juce::uint8>(newVelocity));
        }
    }
    
    return variation;
}

void AdvancedAILearning::saveUserProfile(const juce::File& file) {
    try {
        juce::XmlElement root("AdvancedAILearning");
        
        auto* profileElement = root.createNewChildElement("UserProfile");
        profileElement->setAttribute("averageVelocity", userProfile.averageVelocity);
        profileElement->setAttribute("timingConsistency", userProfile.timingConsistency);
        profileElement->setAttribute("preferredComplexity", userProfile.preferredComplexity);
        profileElement->setAttribute("adaptationRate", userProfile.adaptationRate);
        profileElement->setAttribute("totalPatternsPlayed", userProfile.totalPatternsPlayed);
        profileElement->setAttribute("totalPlayTime", userProfile.totalPlayTime);
        
        auto* genresElement = profileElement->createNewChildElement("FavoriteGenres");
        for (auto genre : userProfile.favoriteGenres) {
            auto* genreElement = genresElement->createNewChildElement("Genre");
            genreElement->setAttribute("value", static_cast<int>(genre));
        }
        
        root.writeTo(file);
        
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to save user profile: " + juce::String(e.what()), "AdvancedAILearning");
    }
}

void AdvancedAILearning::loadUserProfile(const juce::File& file) {
    if (!file.existsAsFile()) return;
    
    try {
        auto xml = juce::parseXML(file);
        if (!xml || xml->getTagName() != "AdvancedAILearning") return;
        
        if (auto* profileElement = xml->getChildByName("UserProfile")) {
            userProfile.averageVelocity = static_cast<float>(
                profileElement->getDoubleAttribute("averageVelocity", 80.0));
            userProfile.timingConsistency = static_cast<float>(
                profileElement->getDoubleAttribute("timingConsistency", 0.8));
            userProfile.preferredComplexity = static_cast<float>(
                profileElement->getDoubleAttribute("preferredComplexity", 0.5));
            userProfile.adaptationRate = static_cast<float>(
                profileElement->getDoubleAttribute("adaptationRate", 0.1));
            userProfile.totalPatternsPlayed = profileElement->getIntAttribute("totalPatternsPlayed", 0);
            userProfile.totalPlayTime = profileElement->getDoubleAttribute("totalPlayTime", 0.0);
            
            if (auto* genresElement = profileElement->getChildByName("FavoriteGenres")) {
                userProfile.favoriteGenres.clear();
                for (auto* genreElement : genresElement->getChildIterator()) {
                    int genreValue = genreElement->getIntAttribute("value");
                    if (genreValue >= 0 && genreValue < 8) {
                        userProfile.favoriteGenres.add(static_cast<PatternSuggestionEngine::Genre>(genreValue));
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to load user profile: " + juce::String(e.what()), "AdvancedAILearning");
    }
}

CollaborativeAI::CollaborativeAI() {
    userId = juce::SystemStats::getComputerName() + "_" + 
             juce::String(juce::Time::getCurrentTime().toMilliseconds());
    cloudConnected = false;
}

void CollaborativeAI::sharePattern(const juce::MidiMessageSequence& pattern,
                                 const juce::String& name,
                                 PatternSuggestionEngine::Genre genre,
                                 const juce::Array<juce::String>& tags) {
    try {
        CommunityPattern communityPattern;
        communityPattern.id = juce::Uuid().toString();
        communityPattern.name = name;
        communityPattern.pattern = pattern;
        communityPattern.genre = genre;
        communityPattern.creatorId = userId;
        communityPattern.uploadTime = juce::Time::getCurrentTime();
        communityPattern.tags = tags;
        
        localPatternCache.add(communityPattern);
        
        UserContribution contribution;
        contribution.patternId = communityPattern.id;
        contribution.contributionTime = juce::Time::getCurrentTime();
        userContributions.add(contribution);
        
        if (cloudConnected) {
            syncWithCloud();
        }
        
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to share pattern: " + juce::String(e.what()), "CollaborativeAI");
    }
}

juce::Array<CollaborativeAI::CommunityPattern> 
CollaborativeAI::searchCommunityPatterns(PatternSuggestionEngine::Genre genre,
                                        const juce::String& searchQuery,
                                        int maxResults) {
    juce::Array<CommunityPattern> results;
    
    try {
        for (const auto& pattern : localPatternCache) {
            bool genreMatch = (genre == pattern.genre);
            bool queryMatch = searchQuery.isEmpty() || 
                            pattern.name.containsIgnoreCase(searchQuery) ||
                            pattern.tags.contains(searchQuery);
            
            if (genreMatch && queryMatch && results.size() < maxResults) {
                results.add(pattern);
            }
        }
        
        std::sort(results.begin(), results.end(), 
                 [](const CommunityPattern& a, const CommunityPattern& b) {
                     return a.communityRating > b.communityRating;
                 });
                 
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to search community patterns: " + juce::String(e.what()), "CollaborativeAI");
    }
    
    return results;
}

IntelligentPatternGenerator::IntelligentPatternGenerator() {
    creativityLevel = 0.5f;
    randomGenerator.setSeedRandomly();
}

juce::MidiMessageSequence 
IntelligentPatternGenerator::generateIntelligentPattern(
    const GenerationConstraints& constraints,
    const AdvancedAILearning::UserPerformanceProfile& userProfile) {
    
    try {
        juce::MidiMessageSequence pattern = generateRhythmicFoundation(
            constraints.primaryGenre, 4, 4);
        
        addGenreSpecificElements(pattern, constraints.primaryGenre);
        
        if (constraints.adaptToUserStyle) {
            applyUserStyleAdaptation(pattern, userProfile);
        }
        
        if (constraints.humanizationLevel > 0.0f) {
            applyHumanization(pattern, constraints.humanizationLevel);
        }
        
        if (constraints.enforceGroovePocket) {
            enforceGroovePocket(pattern);
        }
        
        return pattern;
        
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to generate intelligent pattern: " + juce::String(e.what()), 
            "IntelligentPatternGenerator");
        return juce::MidiMessageSequence();
    }
}

juce::MidiMessageSequence 
IntelligentPatternGenerator::generateRhythmicFoundation(PatternSuggestionEngine::Genre genre,
                                                       int bars, int timeSignature) const {
    juce::MidiMessageSequence foundation;
    
    int stepsPerBar = timeSignature * 4;
    int totalSteps = bars * stepsPerBar;
    double stepDuration = 480.0 / 4.0;
    
    for (int step = 0; step < totalSteps; ++step) {
        double timestamp = step * stepDuration;
        
        if (step % stepsPerBar == 0) {
            auto kickOn = juce::MidiMessage::noteOn(10, INIConfig::GMDrums::BASS_DRUM_1, static_cast<juce::uint8>(100));
            kickOn.setTimeStamp(timestamp);
            foundation.addEvent(kickOn);
        }
        
        if (step % (stepsPerBar / 2) == (stepsPerBar / 4)) {
            auto snareOn = juce::MidiMessage::noteOn(10, INIConfig::GMDrums::ACOUSTIC_SNARE, static_cast<juce::uint8>(90));
            snareOn.setTimeStamp(timestamp);
            foundation.addEvent(snareOn);
        }
        
        if (step % 2 == 0) {
            int velocity = (step % 4 == 0) ? 80 : 60;
            auto hihatOn = juce::MidiMessage::noteOn(10, INIConfig::GMDrums::CLOSED_HI_HAT, static_cast<juce::uint8>(velocity));
            hihatOn.setTimeStamp(timestamp);
            foundation.addEvent(hihatOn);
        }
    }
    
    return foundation;
}

void IntelligentPatternGenerator::addGenreSpecificElements(juce::MidiMessageSequence& pattern,
                                                         PatternSuggestionEngine::Genre genre) const {
    switch (genre) {
        case PatternSuggestionEngine::Genre::Jazz: {
            for (int i = 0; i < pattern.getNumEvents(); ++i) {
                auto event = pattern.getEventPointer(i);
                if (event->message.isNoteOn() && 
                    event->message.getNoteNumber() == INIConfig::GMDrums::CLOSED_HI_HAT) {
                    
                    double swingOffset = 20.0 * creativityLevel;
                    event->message.setTimeStamp(event->message.getTimeStamp() + swingOffset);
                }
            }
            break;
        }
        
        case PatternSuggestionEngine::Genre::Electronic: {
            double timestamp = 0.0;
            for (int i = 0; i < 16; ++i) {
                if (i % 4 == 2) {
                    auto clapOn = juce::MidiMessage::noteOn(10, INIConfig::GMDrums::HAND_CLAP, static_cast<juce::uint8>(85));
                    clapOn.setTimeStamp(timestamp);
                    pattern.addEvent(clapOn);
                }
                timestamp += 120.0;
            }
            break;
        }
        
        default:
            break;
    }
}

void IntelligentPatternGenerator::applyHumanization(juce::MidiMessageSequence& pattern,
                                                   float humanizationLevel) const {
    for (int i = 0; i < pattern.getNumEvents(); ++i) {
        auto event = pattern.getEventPointer(i);
        if (event->message.isNoteOn()) {
            double timingVariation = (const_cast<juce::Random&>(randomGenerator).nextFloat() - 0.5f) * 
                                   humanizationLevel * 20.0;
            event->message.setTimeStamp(event->message.getTimeStamp() + timingVariation);
            
            int velocityVariation = static_cast<int>((const_cast<juce::Random&>(randomGenerator).nextFloat() - 0.5f) * 
                                                   humanizationLevel * 20.0f);
            int newVelocity = juce::jlimit(1, 127, 
                event->message.getVelocity() + velocityVariation);
            
            event->message = juce::MidiMessage::noteOn(event->message.getChannel(),
                                                     event->message.getNoteNumber(),
                                                     static_cast<juce::uint8>(newVelocity));
        }
    }
}
