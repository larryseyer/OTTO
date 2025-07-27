#include "PatternSuggestionEngine.h"
#include "ErrorHandling.h"
#include "INIConfig.h"
#include "PerformanceOptimizations.h"

PatternSuggestionEngine::PatternSuggestionEngine() {
    try {
        initializeGenreProfiles();
    } catch (const std::exception& e) {
        setError("Failed to initialize genre profiles: " + juce::String(e.what()));
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Critical,
            "PatternSuggestionEngine initialization failed", "PatternSuggestionEngine");
    } catch (...) {
        setError("Unknown error during initialization");
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Critical,
            "Unknown initialization error", "PatternSuggestionEngine");
    }
}

void PatternSuggestionEngine::initializeGenreProfiles() noexcept {
    GenreProfile rock;
    rock.genre = Genre::Rock;
    rock.avgSwing = INIConfig::LayoutConstants::midiFileManagerSwingBase;
    rock.avgVelocity = 80.0f;
    rock.complexity = 0.5f;
    rock.commonNotes = {INIConfig::GMDrums::BASS_DRUM_1, INIConfig::GMDrums::ACOUSTIC_SNARE,
                       INIConfig::GMDrums::CLOSED_HI_HAT, INIConfig::GMDrums::CRASH_CYMBAL_1};
    rock.grooveTightness = 1.0f;
    genreProfiles.add(rock);

    GenreProfile jazz;
    jazz.genre = Genre::Jazz;
    jazz.avgSwing = 65.0f;
    jazz.avgVelocity = 70.0f;
    jazz.complexity = 0.7f;
    jazz.commonNotes = {INIConfig::GMDrums::BASS_DRUM_1, INIConfig::GMDrums::ACOUSTIC_SNARE,
                       INIConfig::GMDrums::RIDE_CYMBAL_1, INIConfig::GMDrums::HI_BONGO};
    jazz.grooveTightness = 0.9f;
    genreProfiles.add(jazz);

    GenreProfile electronic;
    electronic.genre = Genre::Electronic;
    electronic.avgSwing = INIConfig::LayoutConstants::midiFileManagerSwingBase;
    electronic.avgVelocity = 90.0f;
    electronic.complexity = 0.6f;
    electronic.commonNotes = {INIConfig::GMDrums::BASS_DRUM_1, INIConfig::GMDrums::ELECTRIC_SNARE,
                            INIConfig::GMDrums::CLOSED_HI_HAT, INIConfig::GMDrums::HAND_CLAP};
    electronic.grooveTightness = 1.0f;
    genreProfiles.add(electronic);

    GenreProfile hiphop;
    hiphop.genre = Genre::HipHop;
    hiphop.avgSwing = 55.0f;
    hiphop.avgVelocity = 85.0f;
    hiphop.complexity = 0.4f;
    hiphop.commonNotes = {INIConfig::GMDrums::BASS_DRUM_1, INIConfig::GMDrums::ACOUSTIC_SNARE,
                         INIConfig::GMDrums::CLOSED_HI_HAT, INIConfig::GMDrums::OPEN_HI_HAT};
    hiphop.grooveTightness = 0.95f;
    genreProfiles.add(hiphop);

    GenreProfile latin;
    latin.genre = Genre::Latin;
    latin.avgSwing = INIConfig::LayoutConstants::midiFileManagerSwingBase;
    latin.avgVelocity = 75.0f;
    latin.complexity = 0.8f;
    latin.commonNotes = {INIConfig::GMDrums::BASS_DRUM_1, INIConfig::GMDrums::HIGH_TIMBALE,
                        INIConfig::GMDrums::LOW_TIMBALE, INIConfig::GMDrums::MARACAS};
    latin.grooveTightness = 0.85f;
    genreProfiles.add(latin);

    GenreProfile funk;
    funk.genre = Genre::Funk;
    funk.avgSwing = 52.0f;
    funk.avgVelocity = 82.0f;
    funk.complexity = 0.6f;
    funk.commonNotes = {INIConfig::GMDrums::BASS_DRUM_1, INIConfig::GMDrums::ACOUSTIC_SNARE,
                       INIConfig::GMDrums::CLOSED_HI_HAT, INIConfig::GMDrums::OPEN_HI_HAT};
    funk.grooveTightness = 0.9f;
    genreProfiles.add(funk);

    GenreProfile pop;
    pop.genre = Genre::Pop;
    pop.avgSwing = INIConfig::LayoutConstants::midiFileManagerSwingBase;
    pop.avgVelocity = 78.0f;
    pop.complexity = 0.3f;
    pop.commonNotes = {INIConfig::GMDrums::BASS_DRUM_1, INIConfig::GMDrums::ACOUSTIC_SNARE,
                      INIConfig::GMDrums::CLOSED_HI_HAT, INIConfig::GMDrums::CRASH_CYMBAL_1};
    pop.grooveTightness = 1.0f;
    genreProfiles.add(pop);

    GenreProfile world;
    world.genre = Genre::World;
    world.avgSwing = INIConfig::LayoutConstants::midiFileManagerSwingBase;
    world.avgVelocity = 72.0f;
    world.complexity = 0.9f;
    world.commonNotes = {INIConfig::GMDrums::LOW_CONGA, INIConfig::GMDrums::OPEN_HI_CONGA,
                        INIConfig::GMDrums::CABASA, INIConfig::GMDrums::CLAVES};
    world.grooveTightness = 0.8f;
    genreProfiles.add(world);
}

juce::Array<PatternSuggestionEngine::PatternSuggestion>
PatternSuggestionEngine::suggestPatterns(const SuggestionParams& params, int numSuggestions) noexcept {
    clearError();
    
    // Input validation and sanitization
    if (!validateParams(params)) {
        setError("Invalid suggestion parameters provided");
        return createBasicPatterns(params.genre, numSuggestions);
    }
    
    auto sanitizedParams = sanitizeParams(params);
    auto sanitizedNumSuggestions = juce::jlimit(1, 20, numSuggestions);
    
    juce::Array<PatternSuggestion> suggestions;
    
    try {
        // Find genre profile with bounds checking
        int profileIndex = -1;
        for (int i = 0; i < genreProfiles.size(); ++i) {
            if (genreProfiles[i].genre == sanitizedParams.genre) {
                profileIndex = i;
                break;
            }
        }

        if (profileIndex < 0 || profileIndex >= genreProfiles.size()) {
            profileIndex = 0; // Default to first profile (Rock)
        }

        const GenreProfile& profile = genreProfiles.getReference(profileIndex);

        auto& stringCache = StringCache::getInstance();
        
        for (int i = 0; i < sanitizedNumSuggestions; ++i) {
            try {
                PatternSuggestion suggestion;
                suggestion.name = stringCache.getPatternString(i + 1);

                float complexityVariation = sanitizedParams.complexity + (i * 0.1f) - 0.2f;
                complexityVariation = juce::jlimit(0.0f, 1.0f, complexityVariation);

                suggestion.pattern = generateDrumPattern(profile,
                                                       sanitizedParams.bars,
                                                       sanitizedParams.timeSignature);

                suggestion.analysis.averageSwing = profile.avgSwing + 
                    (sanitizedParams.humanization * INIConfig::LayoutConstants::midiEngineSwingRatioBase);
                suggestion.analysis.averageVelocity = profile.avgVelocity;
                suggestion.analysis.grooveTightness = 1.0f - sanitizedParams.humanization;
                suggestion.analysis.timeSignatureNumerator = sanitizedParams.timeSignature;
                suggestion.analysis.timeSignatureDenominator = 4;
                suggestion.analysis.tempo = sanitizedParams.tempo;

                suggestion.matchScore = 1.0f - (std::abs(i - sanitizedNumSuggestions / 
                    INIConfig::LayoutConstants::customLookFeelThumbDivisor) / static_cast<float>(sanitizedNumSuggestions));

                suggestions.add(suggestion);
                
            } catch (const std::exception& e) {
                // If individual pattern generation fails, create fallback
                ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
                    "Individual pattern generation failed, using fallback: " + juce::String(e.what()), 
                    "PatternSuggestionEngine");
                
                auto fallback = createFallbackPattern(sanitizedParams);
                fallback.name = stringCache.getPatternString(i + 1) + " (Fallback)";
                suggestions.add(fallback);
            } catch (...) {
                // Unknown error - create basic fallback
                auto fallback = createFallbackPattern(sanitizedParams);
                fallback.name = stringCache.getPatternString(i + 1) + " (Basic)";
                suggestions.add(fallback);
            }
        }

        // If we have no patterns, return basic patterns
        if (suggestions.isEmpty()) {
            setError("All pattern generation failed, returning basic patterns");
            return createBasicPatterns(sanitizedParams.genre, sanitizedNumSuggestions);
        }

        return suggestions;
        
    } catch (const std::exception& e) {
        setError("Pattern suggestion generation failed: " + juce::String(e.what()));
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Pattern suggestion generation failed: " + juce::String(e.what()), 
            "PatternSuggestionEngine");
        return createBasicPatterns(sanitizedParams.genre, sanitizedNumSuggestions);
    } catch (...) {
        setError("Unknown error during pattern suggestion generation");
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Unknown error during pattern suggestion generation", 
            "PatternSuggestionEngine");
        return createBasicPatterns(sanitizedParams.genre, sanitizedNumSuggestions);
    }
}

PatternSuggestionEngine::PatternSuggestion
PatternSuggestionEngine::generatePattern(const SuggestionParams& params) noexcept {
    clearError();
    
    try {
        auto suggestions = suggestPatterns(params, 1);
        if (!suggestions.isEmpty()) {
            return suggestions[0];
        }
        
        // If suggestions failed, create fallback
        setError("Pattern generation failed, using fallback");
        return createFallbackPattern(params);
        
    } catch (const std::exception& e) {
        setError("Exception in generatePattern: " + juce::String(e.what()));
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Exception in generatePattern: " + juce::String(e.what()), 
            "PatternSuggestionEngine");
        return createFallbackPattern(params);
    } catch (...) {
        setError("Unknown error in generatePattern");
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Unknown error in generatePattern", 
            "PatternSuggestionEngine");
        return createFallbackPattern(params);
    }
}

juce::MidiMessageSequence PatternSuggestionEngine::generateDrumPattern(
    const GenreProfile& profile, int bars, int timeSignature) noexcept {

    juce::MidiMessageSequence sequence;
    
    try {
        // Input validation
        if (bars <= 0 || bars > 16 || timeSignature <= 0 || timeSignature > 16) {
            setError("Invalid parameters for drum pattern generation");
            return sequence; // Return empty sequence
        }
        
        juce::Random random;

        const double ticksPerQuarter = INIConfig::Defaults::MIDI_TICKS_PER_QUARTER_NOTE;
        const double ticksPerBar = ticksPerQuarter * timeSignature;
        const int stepsPerBar = timeSignature * 4;

        for (int bar = 0; bar < bars; ++bar) {
            double barStartTicks = bar * ticksPerBar;

            // Generate kick pattern with exception safety
            auto kickPattern = generateKickPattern(profile.genre, stepsPerBar);
            for (int i = 0; i < kickPattern.size(); ++i) {
                if (kickPattern[i] > 0.0f) {
                    double ticks = barStartTicks + (i * ticksPerQuarter / 4.0);
                    int velocity = static_cast<int>(kickPattern[i] * profile.avgVelocity);
                    velocity = juce::jlimit(1, INIConfig::Validation::MAX_MIDI_VELOCITY, velocity);

                    try {
                        auto noteOn = juce::MidiMessage::noteOn(10,
                                                               INIConfig::GMDrums::BASS_DRUM_1,
                                                               static_cast<juce::uint8>(velocity));
                        noteOn.setTimeStamp(ticks);
                        sequence.addEvent(noteOn);

                        auto noteOff = juce::MidiMessage::noteOff(10,
                                                                 INIConfig::GMDrums::BASS_DRUM_1);
                        noteOff.setTimeStamp(ticks + ticksPerQuarter / 8);
                        sequence.addEvent(noteOff);
                    } catch (...) {
                        // Skip this note if MIDI message creation fails
                        continue;
                    }
                }
            }

            // Generate snare pattern with exception safety
            auto snarePattern = generateSnarePattern(profile.genre, stepsPerBar);
            for (int i = 0; i < snarePattern.size(); ++i) {
                if (snarePattern[i] > 0.0f) {
                    double ticks = barStartTicks + (i * ticksPerQuarter / 4.0);
                    int velocity = static_cast<int>(snarePattern[i] * profile.avgVelocity);
                    velocity = juce::jlimit(1, INIConfig::Validation::MAX_MIDI_VELOCITY, velocity);

                    try {
                        auto noteOn = juce::MidiMessage::noteOn(10,
                                                               INIConfig::GMDrums::ACOUSTIC_SNARE,
                                                               static_cast<juce::uint8>(velocity));
                        noteOn.setTimeStamp(ticks);
                        sequence.addEvent(noteOn);

                        auto noteOff = juce::MidiMessage::noteOff(10,
                                                                 INIConfig::GMDrums::ACOUSTIC_SNARE);
                        noteOff.setTimeStamp(ticks + ticksPerQuarter / 8);
                        sequence.addEvent(noteOff);
                    } catch (...) {
                        // Skip this note if MIDI message creation fails
                        continue;
                    }
                }
            }

            // Generate hihat pattern with exception safety
            auto hihatPattern = generateHiHatPattern(profile.genre, stepsPerBar);
            for (int i = 0; i < hihatPattern.size(); ++i) {
                if (hihatPattern[i] > 0.0f) {
                    double ticks = barStartTicks + (i * ticksPerQuarter / 4.0);
                    int velocity = static_cast<int>(hihatPattern[i] * profile.avgVelocity * 0.8f);
                    velocity = juce::jlimit(1, INIConfig::Validation::MAX_MIDI_VELOCITY, velocity);

                    try {
                        auto noteOn = juce::MidiMessage::noteOn(10,
                                                               INIConfig::GMDrums::CLOSED_HI_HAT,
                                                               static_cast<juce::uint8>(velocity));
                        noteOn.setTimeStamp(ticks);
                        sequence.addEvent(noteOn);

                        auto noteOff = juce::MidiMessage::noteOff(10,
                                                                 INIConfig::GMDrums::CLOSED_HI_HAT);
                        noteOff.setTimeStamp(ticks + ticksPerQuarter / INIConfig::LayoutConstants::drumKitEditorPadCount);
                        sequence.addEvent(noteOff);
                    } catch (...) {
                        // Skip this note if MIDI message creation fails
                        continue;
                    }
                }
            }
        }

        try {
            sequence.updateMatchedPairs();
        } catch (...) {
            // If update fails, return sequence as-is
        }
        
        return sequence;
        
    } catch (const std::exception& e) {
        setError("Exception in generateDrumPattern: " + juce::String(e.what()));
        return juce::MidiMessageSequence(); // Return empty sequence
    } catch (...) {
        setError("Unknown error in generateDrumPattern");
        return juce::MidiMessageSequence(); // Return empty sequence
    }
}

juce::Array<float> PatternSuggestionEngine::generateKickPattern(Genre genre, int steps) const noexcept {
    juce::Array<float> pattern;
    
    try {
        // Input validation
        auto sanitizedSteps = juce::jlimit(1, 64, steps);
        pattern.resize(sanitizedSteps);

        switch (genre) {
            case Genre::Rock:
            case Genre::Pop:
                for (int i = 0; i < sanitizedSteps; ++i) {
                    pattern.set(i, (i % 16 == 0 || i % 16 == 8) ? 1.0f : 0.0f);
                }
                break;

            case Genre::Electronic:
            case Genre::HipHop:
                for (int i = 0; i < sanitizedSteps; ++i) {
                    pattern.set(i, (i % 4 == 0) ? 1.0f : 0.0f);
                }
                break;

            case Genre::Jazz:
                if (sanitizedSteps > 0) pattern.set(0, 1.0f);
                if (sanitizedSteps > 6) pattern.set(6, 0.8f);
                if (sanitizedSteps > 10) pattern.set(10, 0.9f);
                break;

            case Genre::Latin:
                if (sanitizedSteps > 0) pattern.set(0, 1.0f);
                if (sanitizedSteps > 3) pattern.set(3, 0.9f);
                if (sanitizedSteps > 6) pattern.set(6, 0.9f);
                break;

            case Genre::Funk:
                if (sanitizedSteps > 0) pattern.set(0, 1.0f);
                if (sanitizedSteps > 1) pattern.set(1, 0.7f);
                if (sanitizedSteps > 4) pattern.set(4, 0.9f);
                if (sanitizedSteps > 10) pattern.set(10, 0.8f);
                break;

            case Genre::World:
                for (int i = 0; i < sanitizedSteps; ++i) {
                    if (i % 5 == 0 || i % 7 == 0) {
                        pattern.set(i, 0.8f + (i % 4) * 0.05f);
                    }
                }
                break;
        }

        return pattern;
        
    } catch (const std::exception& e) {
        setError("Exception in generateKickPattern: " + juce::String(e.what()));
        // Return basic kick pattern as fallback
        juce::Array<float> fallback;
        fallback.resize(juce::jmax(1, juce::jmin(steps, 16)));
        if (fallback.size() > 0) fallback.set(0, 1.0f);
        return fallback;
    } catch (...) {
        setError("Unknown error in generateKickPattern");
        // Return minimal fallback
        juce::Array<float> fallback;
        fallback.resize(1);
        fallback.set(0, 1.0f);
        return fallback;
    }
}

juce::Array<float> PatternSuggestionEngine::generateSnarePattern(Genre genre, int steps) const noexcept {
    juce::Array<float> pattern;
    
    try {
        // Input validation
        auto sanitizedSteps = juce::jlimit(1, 64, steps);
        pattern.resize(sanitizedSteps);

        switch (genre) {
            case Genre::Rock:
            case Genre::Pop:
            case Genre::HipHop:
                for (int i = 0; i < sanitizedSteps; ++i) {
                    pattern.set(i, (i % 8 == 4) ? 1.0f : 0.0f);
                }
                break;

            case Genre::Electronic:
                for (int i = 0; i < sanitizedSteps; ++i) {
                    if (i % 8 == 4) {
                        pattern.set(i, 1.0f);
                    } else if (i % 2 == 1) {
                        pattern.set(i, 0.3f);
                    }
                }
                break;

            case Genre::Jazz:
                if (sanitizedSteps > 2) pattern.set(2, 0.7f);
                if (sanitizedSteps > 5) pattern.set(5, 0.6f);
                if (sanitizedSteps > 10) pattern.set(10, 0.8f);
                break;

            case Genre::Latin:
                if (sanitizedSteps > 2) pattern.set(2, 0.9f);
                if (sanitizedSteps > 6) pattern.set(6, 0.9f);
                if (sanitizedSteps > 8) pattern.set(8, 0.8f);
                if (sanitizedSteps > 11) pattern.set(11, 0.8f);
                break;

            case Genre::Funk:
                for (int i = 0; i < sanitizedSteps; ++i) {
                    if (i % 8 == 4) {
                        pattern.set(i, 1.0f);
                    } else if (i % 2 == 0 && i > 0) {
                        pattern.set(i, 0.4f);
                    }
                }
                break;

            case Genre::World:
                for (int i = 0; i < sanitizedSteps; ++i) {
                    if (i % 5 == 2) {
                        pattern.set(i, 0.8f);
                    }
                }
                break;
        }

        return pattern;
        
    } catch (const std::exception& e) {
        setError("Exception in generateSnarePattern: " + juce::String(e.what()));
        // Return basic snare pattern as fallback
        juce::Array<float> fallback;
        fallback.resize(juce::jmax(1, juce::jmin(steps, 16)));
        if (fallback.size() > 4) fallback.set(4, 1.0f); // Basic snare on beat 2
        return fallback;
    } catch (...) {
        setError("Unknown error in generateSnarePattern");
        // Return minimal fallback
        juce::Array<float> fallback;
        fallback.resize(8);
        fallback.set(4, 1.0f);
        return fallback;
    }
}

juce::Array<float> PatternSuggestionEngine::generateHiHatPattern(Genre genre, int steps) const noexcept {
    juce::Array<float> pattern;
    
    try {
        // Input validation
        auto sanitizedSteps = juce::jlimit(1, 64, steps);
        pattern.resize(sanitizedSteps);

        switch (genre) {
            case Genre::Rock:
            case Genre::Pop:
                for (int i = 0; i < sanitizedSteps; ++i) {
                    pattern.set(i, (i % 2 == 0) ? 0.8f : 0.0f);
                }
                break;

            case Genre::Electronic:
                for (int i = 0; i < sanitizedSteps; ++i) {
                    pattern.set(i, (i % 4 == 0) ? 0.9f : 0.5f);
                }
                break;

            case Genre::Jazz:
                for (int i = 0; i < sanitizedSteps; ++i) {
                    if (i % 4 == 0 || i % 4 == 2) {
                        pattern.set(i, 0.7f);
                    }
                }
                break;

            case Genre::HipHop:
                if (sanitizedSteps > 0) pattern.set(0, 0.8f);
                if (sanitizedSteps > 2) pattern.set(2, 0.6f);
                if (sanitizedSteps > 4) pattern.set(4, 0.8f);
                if (sanitizedSteps > 6) pattern.set(6, 0.7f);
                if (sanitizedSteps > 10) pattern.set(10, 0.5f);
                break;

            case Genre::Latin:
                for (int i = 0; i < sanitizedSteps; ++i) {
                    pattern.set(i, (i % 2 == 0) ? 0.7f : 0.4f);
                }
                break;

            case Genre::Funk:
                for (int i = 0; i < sanitizedSteps; ++i) {
                    pattern.set(i, 0.3f + (i % 4 == 0 ? 0.5f : 0.0f));
                }
                break;

            case Genre::World:
                if (sanitizedSteps > 0) pattern.set(0, 0.6f);
                if (sanitizedSteps > 3) pattern.set(3, 0.5f);
                if (sanitizedSteps > 7) pattern.set(7, 0.7f);
                break;
        }

        return pattern;
        
    } catch (const std::exception& e) {
        setError("Exception in generateHiHatPattern: " + juce::String(e.what()));
        // Return basic hihat pattern as fallback
        juce::Array<float> fallback;
        fallback.resize(juce::jmax(1, juce::jmin(steps, 8)));
        for (int i = 0; i < fallback.size(); i += 2) {
            fallback.set(i, 0.7f);
        }
        return fallback;
    } catch (...) {
        setError("Unknown error in generateHiHatPattern");
        // Return minimal fallback
        juce::Array<float> fallback;
        fallback.resize(4);
        fallback.set(0, 0.7f);
        fallback.set(2, 0.7f);
        return fallback;
    }
}

juce::MidiMessageSequence PatternSuggestionEngine::transferStyle(
    const juce::MidiMessageSequence& source,
    const juce::MidiMessageSequence& styleReference) noexcept {

    clearError();
    
    try {
        juce::MidiMessageSequence result = source;

        float avgVelocity = 0.0f;
        int velocityCount = 0;

        // Analyze style reference with bounds checking
        for (int i = 0; i < styleReference.getNumEvents(); ++i) {
            auto event = styleReference.getEventPointer(i);
            if (event && event->message.isNoteOn()) {
                avgVelocity += event->message.getVelocity();
                velocityCount++;
            }
        }

        if (velocityCount > 0) {
            avgVelocity /= velocityCount;
        } else {
            avgVelocity = 80.0f;
        }

        // Apply style to result with bounds checking
        for (int i = 0; i < result.getNumEvents(); ++i) {
            auto event = result.getEventPointer(i);
            if (event && event->message.isNoteOn()) {
                int newVelocity = static_cast<int>(avgVelocity *
                                                  (event->message.getVelocity() / 100.0f));
                newVelocity = juce::jlimit(1, INIConfig::Validation::MAX_MIDI_VELOCITY, newVelocity);

                event->message = juce::MidiMessage::noteOn(event->message.getChannel(),
                                                          event->message.getNoteNumber(),
                                                          static_cast<juce::uint8>(newVelocity));
            }
        }

        return result;
        
    } catch (const std::exception& e) {
        setError("Exception in transferStyle: " + juce::String(e.what()));
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "transferStyle failed: " + juce::String(e.what()), "PatternSuggestionEngine");
        return source; // Return original source as fallback
    } catch (...) {
        setError("Unknown error in transferStyle");
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Unknown error in transferStyle", "PatternSuggestionEngine");
        return source; // Return original source as fallback
    }
}

juce::Array<PatternSuggestionEngine::PatternSuggestion>
PatternSuggestionEngine::findSimilarGrooves(const MidiGrooveAnalysis& targetGroove,
                                           const juce::Array<MidiGrooveAnalysis>& library) noexcept {
    clearError();
    juce::Array<PatternSuggestion> suggestions;
    
    try {
        for (const auto& groove : library) {
            float similarity = calculatePatternSimilarity(targetGroove, groove);

            if (similarity > 0.5f) {
                PatternSuggestion suggestion;
                suggestion.matchScore = similarity;
                suggestion.analysis = groove;
                suggestions.add(suggestion);
            }
        }

        struct MatchScoreComparator {
            int compareElements(const PatternSuggestion& a, const PatternSuggestion& b) const {
                if (a.matchScore > b.matchScore) return -1;
                if (a.matchScore < b.matchScore) return 1;
                return 0;
            }
        };
        MatchScoreComparator comparator;
        suggestions.sort(comparator);

        return suggestions;
        
    } catch (const std::exception& e) {
        setError("Exception in findSimilarGrooves: " + juce::String(e.what()));
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "findSimilarGrooves failed: " + juce::String(e.what()), "PatternSuggestionEngine");
        return suggestions; // Return what we have so far
    } catch (...) {
        setError("Unknown error in findSimilarGrooves");
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Unknown error in findSimilarGrooves", "PatternSuggestionEngine");
        return suggestions; // Return what we have so far
    }
}

float PatternSuggestionEngine::calculatePatternSimilarity(
    const MidiGrooveAnalysis& a, const MidiGrooveAnalysis& b) const noexcept {
    
    try {

        float swingDiff = std::abs(a.averageSwing - b.averageSwing) / 100.0f;
        float velocityDiff = std::abs(a.averageVelocity - b.averageVelocity) /
                            static_cast<float>(INIConfig::Validation::MAX_MIDI_VELOCITY);
        float tightnessDiff = std::abs(a.grooveTightness - b.grooveTightness);
        float tempoDiff = std::abs(a.tempo - b.tempo) / 200.0f;

        float timeSigMatch = (a.timeSignatureNumerator == b.timeSignatureNumerator &&
                             a.timeSignatureDenominator == b.timeSignatureDenominator) ? 1.0f : 0.5f;

        float similarity = 1.0f - (swingDiff * 0.3f + velocityDiff * 0.2f +
                                  tightnessDiff * 0.2f + tempoDiff * 0.2f +
                                  (1.0f - timeSigMatch) * 0.1f);

        return juce::jlimit(0.0f, 1.0f, similarity);
        
    } catch (const std::exception& e) {
        setError("Exception in calculatePatternSimilarity: " + juce::String(e.what()));
        return 0.0f; // Return no similarity
    } catch (...) {
        setError("Unknown error in calculatePatternSimilarity");
        return 0.0f; // Return no similarity
    }
}

bool PatternSuggestionEngine::adaptToPerformance(
    const juce::Array<float>& recentVelocities,
    const juce::Array<float>& recentTimings) noexcept {
    
    clearError();
    
    try {
        if (recentVelocities.isEmpty() || recentTimings.isEmpty()) return true;

        float avgVelocity = 0.0f;
        for (auto vel : recentVelocities) {
            avgVelocity += vel;
        }
        avgVelocity /= recentVelocities.size();

        float timingDeviation = 0.0f;
        if (recentTimings.size() > 1) {
            float avgTiming = 0.0f;
            for (auto timing : recentTimings) {
                avgTiming += timing;
            }
            avgTiming /= recentTimings.size();

            for (auto timing : recentTimings) {
                timingDeviation += std::abs(timing - avgTiming);
            }
            timingDeviation /= recentTimings.size();
        }

        for (auto& profile : genreProfiles) {
            profile.avgVelocity = profile.avgVelocity * 0.9f + avgVelocity * 0.1f;
            profile.grooveTightness = juce::jlimit(0.1f, 1.0f,
                                                  1.0f - timingDeviation / INIConfig::LayoutConstants::defaultMargin);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        setError("Exception in adaptToPerformance: " + juce::String(e.what()));
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "adaptToPerformance failed: " + juce::String(e.what()), "PatternSuggestionEngine");
        return false;
    } catch (...) {
        setError("Unknown error in adaptToPerformance");
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Unknown error in adaptToPerformance", "PatternSuggestionEngine");
        return false;
    }
}

bool PatternSuggestionEngine::learnFromPattern(
    const juce::MidiMessageSequence& pattern, Genre genre) noexcept {
    
    clearError();
    
    try {
        int profileIndex = -1;
        for (int i = 0; i < genreProfiles.size(); ++i) {
            if (genreProfiles[i].genre == genre) {
                profileIndex = i;
                break;
            }
        }

        if (profileIndex < 0) return true; // Not an error, just no matching profile

        GenreProfile& profile = genreProfiles.getReference(profileIndex);

        float totalVelocity = 0.0f;
        int noteCount = 0;
        juce::Array<int> notesUsed;

        for (int i = 0; i < pattern.getNumEvents(); ++i) {
            auto event = pattern.getEventPointer(i);
            if (event && event->message.isNoteOn()) {
                totalVelocity += event->message.getVelocity();
                noteCount++;

                int note = event->message.getNoteNumber();
                if (!notesUsed.contains(note)) {
                    notesUsed.add(note);
                }
            }
        }

        if (noteCount > 0) {
            float avgVel = totalVelocity / noteCount;
            profile.avgVelocity = profile.avgVelocity * 0.8f + avgVel * 0.2f;

            for (auto note : notesUsed) {
                if (!profile.commonNotes.contains(note)) {
                    profile.commonNotes.add(note);
                }
            }
        }

        MidiGrooveAnalysis analysis;
        analysis.averageVelocity = noteCount > 0 ? totalVelocity / noteCount : profile.avgVelocity;
        analysis.averageSwing = profile.avgSwing;
        analysis.grooveTightness = profile.grooveTightness;
        patternLibrary.add(analysis);
        
        return true;
        
    } catch (const std::exception& e) {
        setError("Exception in learnFromPattern: " + juce::String(e.what()));
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "learnFromPattern failed: " + juce::String(e.what()), "PatternSuggestionEngine");
        return false;
    } catch (...) {
        setError("Unknown error in learnFromPattern");
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Unknown error in learnFromPattern", "PatternSuggestionEngine");
        return false;
    }
}

bool PatternSuggestionEngine::saveLearnedData(const juce::File& file) noexcept {
    clearError();
    
    try {
        juce::XmlElement root("PatternSuggestionData");

        auto* profilesElement = root.createNewChildElement("GenreProfiles");
        for (const auto& profile : genreProfiles) {
            auto* profileElement = profilesElement->createNewChildElement("Profile");
            profileElement->setAttribute("genre", static_cast<int>(profile.genre));
            profileElement->setAttribute("avgSwing", profile.avgSwing);
            profileElement->setAttribute("avgVelocity", profile.avgVelocity);
            profileElement->setAttribute("complexity", profile.complexity);
            profileElement->setAttribute("grooveTightness", profile.grooveTightness);

            auto* notesElement = profileElement->createNewChildElement("CommonNotes");
            for (auto note : profile.commonNotes) {
                auto* noteElement = notesElement->createNewChildElement("Note");
                noteElement->setAttribute("value", note);
            }
        }

        auto* libraryElement = root.createNewChildElement("PatternLibrary");
        for (const auto& pattern : patternLibrary) {
            auto* patternElement = libraryElement->createNewChildElement("Pattern");
            patternElement->setAttribute("avgSwing", pattern.averageSwing);
            patternElement->setAttribute("avgVelocity", pattern.averageVelocity);
            patternElement->setAttribute("grooveTightness", pattern.grooveTightness);
            patternElement->setAttribute("tempo", pattern.tempo);
            patternElement->setAttribute("timeSigNum", pattern.timeSignatureNumerator);
            patternElement->setAttribute("timeSigDen", pattern.timeSignatureDenominator);
        }

        root.writeTo(file);
        return true;
        
    } catch (const std::exception& e) {
        setError("Exception in saveLearnedData: " + juce::String(e.what()));
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "saveLearnedData failed: " + juce::String(e.what()), "PatternSuggestionEngine");
        return false;
    } catch (...) {
        setError("Unknown error in saveLearnedData");
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Unknown error in saveLearnedData", "PatternSuggestionEngine");
        return false;
    }
}

bool PatternSuggestionEngine::loadLearnedData(const juce::File& file) noexcept {
    clearError();
    
    try {
        if (!file.existsAsFile()) return true; // Not an error

        auto xml = juce::parseXML(file);
        if (!xml || xml->getTagName() != "PatternSuggestionData") return true; // Not an error

        if (auto* profilesElement = xml->getChildByName("GenreProfiles")) {
            for (auto* profileElement : profilesElement->getChildIterator()) {
                int genreInt = profileElement->getIntAttribute("genre");
                if (genreInt >= 0 && genreInt < static_cast<int>(Genre::World) + 1) {
                    Genre genre = static_cast<Genre>(genreInt);

                    for (auto& profile : genreProfiles) {
                        if (profile.genre == genre) {
                            profile.avgSwing = static_cast<float>(
                                profileElement->getDoubleAttribute("avgSwing", profile.avgSwing));
                            profile.avgVelocity = static_cast<float>(
                                profileElement->getDoubleAttribute("avgVelocity", profile.avgVelocity));
                            profile.complexity = static_cast<float>(
                                profileElement->getDoubleAttribute("complexity", profile.complexity));
                            profile.grooveTightness = static_cast<float>(
                                profileElement->getDoubleAttribute("grooveTightness", profile.grooveTightness));

                            if (auto* notesElement = profileElement->getChildByName("CommonNotes")) {
                                profile.commonNotes.clear();
                                for (auto* noteElement : notesElement->getChildIterator()) {
                                    profile.commonNotes.add(noteElement->getIntAttribute("value"));
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }

        if (auto* libraryElement = xml->getChildByName("PatternLibrary")) {
            patternLibrary.clear();
            for (auto* patternElement : libraryElement->getChildIterator()) {
                MidiGrooveAnalysis pattern;
                pattern.averageSwing = static_cast<float>(
                    patternElement->getDoubleAttribute("avgSwing", INIConfig::LayoutConstants::midiFileManagerSwingBase));
                pattern.averageVelocity = static_cast<float>(
                    patternElement->getDoubleAttribute("avgVelocity", 80.0));
                pattern.grooveTightness = static_cast<float>(
                    patternElement->getDoubleAttribute("grooveTightness", 1.0));
                pattern.tempo = static_cast<float>(
                    patternElement->getDoubleAttribute("tempo", INIConfig::Defaults::DEFAULT_TEMPO));
                pattern.timeSignatureNumerator = patternElement->getIntAttribute("timeSigNum", 4);
                pattern.timeSignatureDenominator = patternElement->getIntAttribute("timeSigDen", 4);

                patternLibrary.add(pattern);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        setError("Exception in loadLearnedData: " + juce::String(e.what()));
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "loadLearnedData failed: " + juce::String(e.what()), "PatternSuggestionEngine");
        return false;
    } catch (...) {
        setError("Unknown error in loadLearnedData");
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Unknown error in loadLearnedData", "PatternSuggestionEngine");
        return false;
    }
}

// Exception-safe helper methods implementation

void PatternSuggestionEngine::setError(const juce::String& message) const noexcept {
    hasInternalError = true;
    lastErrorMessage = message;
}

bool PatternSuggestionEngine::validateParams(const SuggestionParams& params) const noexcept {
    return params.isValid();
}

PatternSuggestionEngine::SuggestionParams PatternSuggestionEngine::sanitizeParams(const SuggestionParams& params) const noexcept {
    SuggestionParams sanitized = params;
    
    // Ensure all values are within safe bounds
    sanitized.complexity = juce::jlimit(0.0f, 1.0f, sanitized.complexity);
    sanitized.humanization = juce::jlimit(0.0f, 1.0f, sanitized.humanization);
    sanitized.timeSignature = juce::jlimit(1, 16, sanitized.timeSignature);
    sanitized.tempo = juce::jlimit(60.0f, 300.0f, sanitized.tempo);
    sanitized.bars = juce::jlimit(1, 16, sanitized.bars);
    
    return sanitized;
}

PatternSuggestionEngine::PatternSuggestion PatternSuggestionEngine::createFallbackPattern(const SuggestionParams& params) const noexcept {
    PatternSuggestion fallback;
    
    try {
        fallback.name = "Basic Pattern";
        fallback.matchScore = 0.5f;
        
        // Create a very basic drum pattern
        juce::MidiMessageSequence sequence;
        const double ticksPerQuarter = INIConfig::Defaults::MIDI_TICKS_PER_QUARTER_NOTE;
        const double ticksPerBar = ticksPerQuarter * params.timeSignature;
        
        for (int bar = 0; bar < params.bars; ++bar) {
            double barStartTicks = bar * ticksPerBar;
            
            // Basic kick on beats 1 and 3
            for (int beat = 0; beat < 2; ++beat) {
                double ticks = barStartTicks + (beat * ticksPerQuarter * 2);
                
                auto noteOn = juce::MidiMessage::noteOn(10, INIConfig::GMDrums::BASS_DRUM_1, juce::uint8(80));
                noteOn.setTimeStamp(ticks);
                sequence.addEvent(noteOn);
                
                auto noteOff = juce::MidiMessage::noteOff(10, INIConfig::GMDrums::BASS_DRUM_1);
                noteOff.setTimeStamp(ticks + ticksPerQuarter / 8);
                sequence.addEvent(noteOff);
            }
            
            // Basic snare on beats 2 and 4
            for (int beat = 1; beat < 4; beat += 2) {
                double ticks = barStartTicks + (beat * ticksPerQuarter);
                
                auto noteOn = juce::MidiMessage::noteOn(10, INIConfig::GMDrums::ACOUSTIC_SNARE, juce::uint8(90));
                noteOn.setTimeStamp(ticks);
                sequence.addEvent(noteOn);
                
                auto noteOff = juce::MidiMessage::noteOff(10, INIConfig::GMDrums::ACOUSTIC_SNARE);
                noteOff.setTimeStamp(ticks + ticksPerQuarter / 8);
                sequence.addEvent(noteOff);
            }
        }
        
        sequence.updateMatchedPairs();
        fallback.pattern = sequence;
        
        // Set basic analysis
        fallback.analysis.averageSwing = INIConfig::LayoutConstants::midiFileManagerSwingBase;
        fallback.analysis.averageVelocity = 85.0f;
        fallback.analysis.grooveTightness = 1.0f;
        fallback.analysis.timeSignatureNumerator = params.timeSignature;
        fallback.analysis.timeSignatureDenominator = 4;
        fallback.analysis.tempo = params.tempo;
        
    } catch (...) {
        // If even the fallback fails, return empty pattern
        fallback.name = "Empty Pattern";
        fallback.matchScore = 0.0f;
        fallback.pattern = juce::MidiMessageSequence();
    }
    
    return fallback;
}

juce::Array<PatternSuggestionEngine::PatternSuggestion> PatternSuggestionEngine::createBasicPatterns(Genre genre, int numSuggestions) const noexcept {
    juce::Array<PatternSuggestion> patterns;
    
    try {
        auto sanitizedNum = juce::jlimit(1, 10, numSuggestions);
        SuggestionParams basicParams;
        basicParams.genre = genre;
        basicParams.complexity = 0.3f;
        basicParams.humanization = 0.1f;
        basicParams.timeSignature = 4;
        basicParams.tempo = 120.0f;
        basicParams.bars = 1;
        
        for (int i = 0; i < sanitizedNum; ++i) {
            auto pattern = createFallbackPattern(basicParams);
            pattern.name = "Basic " + juce::String(i + 1);
            pattern.matchScore = 1.0f - (static_cast<float>(i) / static_cast<float>(sanitizedNum));
            patterns.add(pattern);
        }
        
    } catch (...) {
        // Return empty array if everything fails
    }
    
    return patterns;
}
