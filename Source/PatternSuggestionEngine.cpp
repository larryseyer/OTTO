#include "PatternSuggestionEngine.h"
#include "INIConfig.h"

PatternSuggestionEngine::PatternSuggestionEngine() {
    initializeGenreProfiles();
}

void PatternSuggestionEngine::initializeGenreProfiles() {
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
PatternSuggestionEngine::suggestPatterns(const SuggestionParams& params, int numSuggestions) {
    juce::Array<PatternSuggestion> suggestions;

    int profileIndex = -1;
    for (int i = 0; i < genreProfiles.size(); ++i) {
        if (genreProfiles[i].genre == params.genre) {
            profileIndex = i;
            break;
        }
    }

    if (profileIndex < 0) {
        profileIndex = 0;
    }

    const GenreProfile& profile = genreProfiles.getReference(profileIndex);

    for (int i = 0; i < numSuggestions; ++i) {
        PatternSuggestion suggestion;
        suggestion.name = "Pattern " + juce::String(i + 1);

        float complexityVariation = params.complexity + (i * 0.1f) - 0.2f;
        complexityVariation = juce::jlimit(0.0f, 1.0f, complexityVariation);

        suggestion.pattern = generateDrumPattern(profile,
                                               params.bars,
                                               params.timeSignature);

        suggestion.analysis.averageSwing = profile.avgSwing + (params.humanization * INIConfig::LayoutConstants::midiEngineSwingRatioBase);
        suggestion.analysis.averageVelocity = profile.avgVelocity;
        suggestion.analysis.grooveTightness = 1.0f - params.humanization;
        suggestion.analysis.timeSignatureNumerator = params.timeSignature;
        suggestion.analysis.timeSignatureDenominator = 4;
        suggestion.analysis.tempo = params.tempo;

        suggestion.matchScore = 1.0f - (std::abs(i - numSuggestions / INIConfig::LayoutConstants::customLookFeelThumbDivisor) /
                                        static_cast<float>(numSuggestions));

        suggestions.add(suggestion);
    }

    return suggestions;
}

PatternSuggestionEngine::PatternSuggestion
PatternSuggestionEngine::generatePattern(const SuggestionParams& params) {
    auto suggestions = suggestPatterns(params, 1);
    return suggestions.isEmpty() ? PatternSuggestion() : suggestions[0];
}

juce::MidiMessageSequence PatternSuggestionEngine::generateDrumPattern(
    const GenreProfile& profile, int bars, int timeSignature) {

    juce::MidiMessageSequence sequence;
    juce::Random random;

    const double ticksPerQuarter = INIConfig::Defaults::MIDI_TICKS_PER_QUARTER_NOTE;
    const double ticksPerBar = ticksPerQuarter * timeSignature;
    const int stepsPerBar = timeSignature * 4;

    for (int bar = 0; bar < bars; ++bar) {
        double barStartTicks = bar * ticksPerBar;

        auto kickPattern = generateKickPattern(profile.genre, stepsPerBar);
        for (int i = 0; i < kickPattern.size(); ++i) {
            if (kickPattern[i] > 0.0f) {
                double ticks = barStartTicks + (i * ticksPerQuarter / 4.0);
                int velocity = static_cast<int>(kickPattern[i] * profile.avgVelocity);
                velocity = juce::jlimit(1, INIConfig::Validation::MAX_MIDI_VELOCITY, velocity);

                auto noteOn = juce::MidiMessage::noteOn(10,
                                                       INIConfig::GMDrums::BASS_DRUM_1,
                                                       static_cast<juce::uint8>(velocity));
                noteOn.setTimeStamp(ticks);
                sequence.addEvent(noteOn);

                auto noteOff = juce::MidiMessage::noteOff(10,
                                                         INIConfig::GMDrums::BASS_DRUM_1);
                noteOff.setTimeStamp(ticks + ticksPerQuarter / 8);
                sequence.addEvent(noteOff);
            }
        }

        auto snarePattern = generateSnarePattern(profile.genre, stepsPerBar);
        for (int i = 0; i < snarePattern.size(); ++i) {
            if (snarePattern[i] > 0.0f) {
                double ticks = barStartTicks + (i * ticksPerQuarter / 4.0);
                int velocity = static_cast<int>(snarePattern[i] * profile.avgVelocity);
                velocity = juce::jlimit(1, INIConfig::Validation::MAX_MIDI_VELOCITY, velocity);

                auto noteOn = juce::MidiMessage::noteOn(10,
                                                       INIConfig::GMDrums::ACOUSTIC_SNARE,
                                                       static_cast<juce::uint8>(velocity));
                noteOn.setTimeStamp(ticks);
                sequence.addEvent(noteOn);

                auto noteOff = juce::MidiMessage::noteOff(10,
                                                         INIConfig::GMDrums::ACOUSTIC_SNARE);
                noteOff.setTimeStamp(ticks + ticksPerQuarter / 8);
                sequence.addEvent(noteOff);
            }
        }

        auto hihatPattern = generateHiHatPattern(profile.genre, stepsPerBar);
        for (int i = 0; i < hihatPattern.size(); ++i) {
            if (hihatPattern[i] > 0.0f) {
                double ticks = barStartTicks + (i * ticksPerQuarter / 4.0);
                int velocity = static_cast<int>(hihatPattern[i] * profile.avgVelocity * 0.8f);
                velocity = juce::jlimit(1, INIConfig::Validation::MAX_MIDI_VELOCITY, velocity);

                auto noteOn = juce::MidiMessage::noteOn(10,
                                                       INIConfig::GMDrums::CLOSED_HI_HAT,
                                                       static_cast<juce::uint8>(velocity));
                noteOn.setTimeStamp(ticks);
                sequence.addEvent(noteOn);

                auto noteOff = juce::MidiMessage::noteOff(10,
                                                         INIConfig::GMDrums::CLOSED_HI_HAT);
                noteOff.setTimeStamp(ticks + ticksPerQuarter / INIConfig::LayoutConstants::drumKitEditorPadCount);
                sequence.addEvent(noteOff);
            }
        }
    }

    sequence.updateMatchedPairs();
    return sequence;
}

juce::Array<float> PatternSuggestionEngine::generateKickPattern(Genre genre, int steps) {
    juce::Array<float> pattern;
    pattern.resize(steps);

    switch (genre) {
        case Genre::Rock:
        case Genre::Pop:
            for (int i = 0; i < steps; ++i) {
                pattern.set(i, (i % 16 == 0 || i % 16 == 8) ? 1.0f : 0.0f);
            }
            break;

        case Genre::Electronic:
        case Genre::HipHop:
            for (int i = 0; i < steps; ++i) {
                pattern.set(i, (i % 4 == 0) ? 1.0f : 0.0f);
            }
            break;

        case Genre::Jazz:
            pattern.set(0, 1.0f);
            if (steps > 6)
                pattern.set(6, 0.8f);
            if (steps > 10)
                pattern.set(10, 0.9f);
            break;

        case Genre::Latin:
            pattern.set(0, 1.0f);
            if (steps > 3)
                pattern.set(3, 0.9f);
            if (steps > 6)
                pattern.set(6, 0.9f);
            break;

        case Genre::Funk:
            pattern.set(0, 1.0f);
            if (steps > 1) pattern.set(1, 0.7f);
            if (steps > 4)
                pattern.set(4, 0.9f);
            if (steps > 10)
                pattern.set(10, 0.8f);
            break;

        case Genre::World:
            for (int i = 0; i < steps; ++i) {
                if (i % 5 == 0 || i % 7 == 0) {
                    pattern.set(i, 0.8f + (i % 4) * 0.05f);
                }
            }
            break;
    }

    return pattern;
}

juce::Array<float> PatternSuggestionEngine::generateSnarePattern(Genre genre, int steps) {
    juce::Array<float> pattern;
    pattern.resize(steps);

    switch (genre) {
        case Genre::Rock:
        case Genre::Pop:
        case Genre::HipHop:
            for (int i = 0; i < steps; ++i) {
                pattern.set(i, (i % 8 == 4) ? 1.0f : 0.0f);
            }
            break;

        case Genre::Electronic:
            for (int i = 0; i < steps; ++i) {
                if (i % 8 == 4) {
                    pattern.set(i, 1.0f);
                } else if (i % 2 == 1) {
                    pattern.set(i, 0.3f);
                }
            }
            break;

        case Genre::Jazz:
            if (steps > 2) pattern.set(2, 0.7f);
            if (steps > 5) pattern.set(5, 0.6f);
            if (steps > 10)
                pattern.set(10, 0.8f);
            break;

        case Genre::Latin:
            if (steps > 2) pattern.set(2, 0.9f);
            if (steps > 6) pattern.set(6, 0.9f);
            if (steps > 8)
                pattern.set(8, 0.8f);
            if (steps > 11)
                pattern.set(11, 0.8f);
            break;

        case Genre::Funk:
            for (int i = 0; i < steps; ++i) {
                if (i % 8 == 4) {
                    pattern.set(i, 1.0f);
                } else if (i % 2 == 0 && i > 0) {
                    pattern.set(i, 0.4f);
                }
            }
            break;

        case Genre::World:
            for (int i = 0; i < steps; ++i) {
                if (i % 5 == 2) {
                    pattern.set(i, 0.8f);
                }
            }
            break;
    }

    return pattern;
}

juce::Array<float> PatternSuggestionEngine::generateHiHatPattern(Genre genre, int steps) {
    juce::Array<float> pattern;
    pattern.resize(steps);

    switch (genre) {
        case Genre::Rock:
        case Genre::Pop:
            for (int i = 0; i < steps; ++i) {
                pattern.set(i, (i % 2 == 0) ? 0.8f : 0.0f);
            }
            break;

        case Genre::Electronic:
            for (int i = 0; i < steps; ++i) {
                pattern.set(i, (i % 4 == 0) ? 0.9f : 0.5f);
            }
            break;

        case Genre::Jazz:
            for (int i = 0; i < steps; ++i) {
                if (i % 4 == 0 || i % 4 == 2) {
                    pattern.set(i, 0.7f);
                }
            }
            break;

        case Genre::HipHop:
            pattern.set(0, 0.8f);
            if (steps > 2) pattern.set(2, 0.6f);
            if (steps > 4) pattern.set(4, 0.8f);
            if (steps > 6) pattern.set(6, 0.7f);
            if (steps > 10)
                pattern.set(10, 0.5f);
            break;

        case Genre::Latin:
            for (int i = 0; i < steps; ++i) {
                pattern.set(i, (i % 2 == 0) ? 0.7f : 0.4f);
            }
            break;

        case Genre::Funk:
            for (int i = 0; i < steps; ++i) {
                pattern.set(i, 0.3f + (i % 4 == 0 ? 0.5f : 0.0f));
            }
            break;

        case Genre::World:
            if (steps > 0) pattern.set(0, 0.6f);
            if (steps > 3)
                pattern.set(3, 0.5f);
            if (steps > 7)
                pattern.set(7, 0.7f);
            break;
    }

    return pattern;
}

juce::MidiMessageSequence PatternSuggestionEngine::transferStyle(
    const juce::MidiMessageSequence& source,
    const juce::MidiMessageSequence& styleReference) {

    juce::MidiMessageSequence result = source;

    float avgVelocity = 0.0f;
    int velocityCount = 0;

    for (int i = 0; i < styleReference.getNumEvents(); ++i) {
        auto event = styleReference.getEventPointer(i);
        if (event->message.isNoteOn()) {
            avgVelocity += event->message.getVelocity();
            velocityCount++;
        }
    }

    if (velocityCount > 0) {
        avgVelocity /= velocityCount;
    } else {
        avgVelocity = 80.0f;
    }

    for (int i = 0; i < result.getNumEvents(); ++i) {
        auto event = result.getEventPointer(i);
        if (event->message.isNoteOn()) {
            int newVelocity = static_cast<int>(avgVelocity *
                                              (event->message.getVelocity() / 100.0f));
            newVelocity = juce::jlimit(1, INIConfig::Validation::MAX_MIDI_VELOCITY, newVelocity);

            event->message = juce::MidiMessage::noteOn(event->message.getChannel(),
                                                      event->message.getNoteNumber(),
                                                      static_cast<juce::uint8>(newVelocity));
        }
    }

    return result;
}

juce::Array<PatternSuggestionEngine::PatternSuggestion>
PatternSuggestionEngine::findSimilarGrooves(const MidiGrooveAnalysis& targetGroove,
                                           const juce::Array<MidiGrooveAnalysis>& library) {
    juce::Array<PatternSuggestion> suggestions;

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
}

float PatternSuggestionEngine::calculatePatternSimilarity(
    const MidiGrooveAnalysis& a, const MidiGrooveAnalysis& b) {

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
}

void PatternSuggestionEngine::adaptToPerformance(
    const juce::Array<float>& recentVelocities,
    const juce::Array<float>& recentTimings) {

    if (recentVelocities.isEmpty() || recentTimings.isEmpty()) return;

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
}

void PatternSuggestionEngine::learnFromPattern(
    const juce::MidiMessageSequence& pattern, Genre genre) {

    int profileIndex = -1;
    for (int i = 0; i < genreProfiles.size(); ++i) {
        if (genreProfiles[i].genre == genre) {
            profileIndex = i;
            break;
        }
    }

    if (profileIndex < 0) return;

    GenreProfile& profile = genreProfiles.getReference(profileIndex);

    float totalVelocity = 0.0f;
    int noteCount = 0;
    juce::Array<int> notesUsed;

    for (int i = 0; i < pattern.getNumEvents(); ++i) {
        auto event = pattern.getEventPointer(i);
        if (event->message.isNoteOn()) {
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
}

void PatternSuggestionEngine::saveLearnedData(const juce::File& file) {
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
}

void PatternSuggestionEngine::loadLearnedData(const juce::File& file) {
    if (!file.existsAsFile()) return;

    auto xml = juce::parseXML(file);
    if (!xml || xml->getTagName() != "PatternSuggestionData") return;

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
}
