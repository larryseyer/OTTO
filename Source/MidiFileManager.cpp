#include "MidiFileManager.h"
#include "INIConfig.h"
#include <algorithm>
#include <cmath>

MidiFileManager::MidiFileManager() {
    juce::File assetsPath = getAssetsPath();
    if (assetsPath.exists()) {
        midiFilesFolder = assetsPath.getChildFile("MidiFiles/Grooves");
        grooveTemplatesFolder = INIConfig::getOTTODataDirectory().getChildFile("GrooveTemplates");

        if (!grooveTemplatesFolder.exists()) {
            grooveTemplatesFolder.createDirectory();
        }

        if (midiFilesFolder.exists()) {
            scanMidiFiles();
            createUnlimitedGroupsWithSixteenMidiFiles();
            initializeBeatsButtonGroups();
        }
    }
}

juce::File MidiFileManager::getAssetsPath() {
    juce::File exePath = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
    juce::File bundleContents = exePath.getParentDirectory().getParentDirectory();

    if (bundleContents.getChildFile("Resources").exists()) {
        juce::File bundleAssets = bundleContents.getChildFile("Resources/Assets");
        if (bundleAssets.exists()) {
            return bundleAssets;
        }
    }

    juce::File execAssets = exePath.getParentDirectory().getChildFile("Assets");
    if (execAssets.exists()) {
        return execAssets;
    }

    juce::File parentAssets = exePath.getParentDirectory().getParentDirectory().getChildFile("Assets");
    if (parentAssets.exists()) {
        return parentAssets;
    }

    return juce::File();
}

MidiGrooveAnalysis MidiFileManager::analyzeMidiFile(const juce::String& fileName) {
    if (analysisCache.count(fileName) > 0) {
        return analysisCache[fileName];
    }

    MidiGrooveAnalysis analysis;
    juce::File midiFile = getMidiFile(fileName);

    if (!midiFile.existsAsFile()) {
        return analysis;
    }

    juce::FileInputStream fileStream(midiFile);
    if (!fileStream.openedOk()) {
        return analysis;
    }

    juce::MidiFile midiFileData;
    if (!midiFileData.readFrom(fileStream)) {
        return analysis;
    }

    juce::MidiMessageSequence allEvents;
    for (int track = 0; track < midiFileData.getNumTracks(); ++track) {
        const juce::MidiMessageSequence* sequence = midiFileData.getTrack(track);
        if (sequence) {
            for (int i = 0; i < sequence->getNumEvents(); ++i) {
                allEvents.addEvent(sequence->getEventPointer(i)->message);
            }
        }
    }

    allEvents.sort();

    analysis.tempo = estimateTempo(allEvents);

    detectTimeSignature(allEvents, analysis.timeSignatureNumerator, analysis.timeSignatureDenominator);

    analysis.averageSwing = calculateSwing(allEvents);

    analysis.grooveTightness = calculateGrooveTightness(allEvents);

    float totalVelocity = 0.0f;
    float minVelocity = 127.0f;
    float maxVelocity = 0.0f;
    int noteCount = 0;
    juce::Array<float> velocities;

    for (int i = 0; i < allEvents.getNumEvents(); ++i) {
        const auto* event = allEvents.getEventPointer(i);
        if (event && event->message.isNoteOn()) {
            float velocity = static_cast<float>(event->message.getVelocity());
            totalVelocity += velocity;
            minVelocity = juce::jmin(minVelocity, velocity);
            maxVelocity = juce::jmax(maxVelocity, velocity);
            velocities.add(velocity);
            noteCount++;
        }
    }

    if (noteCount > 0) {
        analysis.averageVelocity = totalVelocity / noteCount;
        analysis.velocityRange = maxVelocity - minVelocity;

        float variance = 0.0f;
        for (float v : velocities) {
            variance += (v - analysis.averageVelocity) * (v - analysis.averageVelocity);
        }
        analysis.velocityVariation = std::sqrt(variance / noteCount);

        double sequenceDuration = allEvents.getEndTime() - allEvents.getStartTime();
        if (sequenceDuration > 0) {
            double beatsInSequence = sequenceDuration * (analysis.tempo / 60.0);
            analysis.noteDensity = noteCount / beatsInSequence;
            analysis.numberOfBars = static_cast<int>(beatsInSequence / analysis.timeSignatureNumerator);
        }
    }

    std::map<int, int> noteDistribution;
    for (int i = 0; i < allEvents.getNumEvents(); ++i) {
        const auto* event = allEvents.getEventPointer(i);
        if (event && event->message.isNoteOn()) {
            int note = event->message.getNoteNumber();
            noteDistribution[note]++;
        }
    }

    for (const auto& pair : noteDistribution) {
        analysis.noteDistribution.add(pair.second);
    }

    analysisCache[fileName] = analysis;

    return analysis;
}

float MidiFileManager::calculateSwing(const juce::MidiMessageSequence& sequence) {
    juce::Array<double> eighthNoteTimes;

    for (int i = 0; i < sequence.getNumEvents(); ++i) {
        const auto* event = sequence.getEventPointer(i);
        if (event && event->message.isNoteOn()) {
            double time = event->message.getTimeStamp();
            double beatPosition = std::fmod(time * 2.0, 1.0);

            if (beatPosition > INIConfig::LayoutConstants::midiFileManagerEighthNoteMin &&
                beatPosition < INIConfig::LayoutConstants::midiFileManagerEighthNoteMax) {
                eighthNoteTimes.add(beatPosition);
            }
        }
    }

    if (eighthNoteTimes.isEmpty()) {
        return INIConfig::LayoutConstants::midiFileManagerSwingBase;
    }

    double averagePosition = 0.0;
    for (double pos : eighthNoteTimes) {
        averagePosition += pos;
    }
    averagePosition /= eighthNoteTimes.size();

    float swing = INIConfig::LayoutConstants::midiFileManagerSwingBase +
                  (averagePosition - INIConfig::LayoutConstants::midiFileManagerSwingOffset) *
                  INIConfig::LayoutConstants::midiFileManagerSwingScale;
    return juce::jlimit(INIConfig::LayoutConstants::midiFileManagerSwingMin,
                       INIConfig::LayoutConstants::midiFileManagerSwingMax, swing);
}

float MidiFileManager::calculateGrooveTightness(const juce::MidiMessageSequence& sequence) {
    juce::Array<double> timingDeviations;
    double gridResolution = INIConfig::LayoutConstants::midiFileManagerGridResolution;

    for (int i = 0; i < sequence.getNumEvents(); ++i) {
        const auto* event = sequence.getEventPointer(i);
        if (event && event->message.isNoteOn()) {
            double time = event->message.getTimeStamp();
            double quantizedTime = std::round(time / gridResolution) * gridResolution;
            double deviation = std::abs(time - quantizedTime);
            timingDeviations.add(deviation);
        }
    }

    if (timingDeviations.isEmpty()) {
        return 1.0f;
    }

    double totalDeviation = 0.0;
    for (double dev : timingDeviations) {
        totalDeviation += dev;
    }
    double averageDeviation = totalDeviation / timingDeviations.size();

    float tightness = 1.0f - juce::jlimit(0.0f, 1.0f,
        static_cast<float>(averageDeviation * INIConfig::LayoutConstants::midiFileManagerTightnessScale));
    return tightness;
}



void MidiFileManager::detectTimeSignature(const juce::MidiMessageSequence& sequence,
                                         int& numerator, int& denominator) {

    juce::Array<double> kickTimes;
    for (int i = 0; i < sequence.getNumEvents(); ++i) {
        const auto* event = sequence.getEventPointer(i);
        if (event && event->message.isNoteOn()) {
            int note = event->message.getNoteNumber();
            if (note == 36 || note == 35) {
                kickTimes.add(event->message.getTimeStamp());
            }
        }
    }

    if (kickTimes.size() >= 2) {
        double avgInterval = 0.0;
        for (int i = 1; i < kickTimes.size(); ++i) {
            avgInterval += kickTimes[i] - kickTimes[i-1];
        }
        avgInterval /= (kickTimes.size() - 1);

        if (avgInterval > 0.9 && avgInterval < 1.1) {
            numerator = 4;
            denominator = 4;
        } else if (avgInterval > 0.65 && avgInterval < 0.85) {
            numerator = 3;
            denominator = 4;
        } else if (avgInterval > 1.4 && avgInterval < 1.6) {
            numerator = 6;
            denominator = 8;
        } else {
            numerator = 4;
            denominator = 4;
        }
    } else {
        numerator = 4;
        denominator = 4;
    }
}

float MidiFileManager::estimateTempo(const juce::MidiMessageSequence& sequence) {
    juce::Array<double> noteTimes;

    for (int i = 0; i < sequence.getNumEvents(); ++i) {
        const auto* event = sequence.getEventPointer(i);
        if (event && event->message.isNoteOn()) {
            noteTimes.add(event->message.getTimeStamp());
        }
    }

    if (noteTimes.size() < 2) {
        return INIConfig::Defaults::DEFAULT_TEMPO;
    }

    juce::Array<double> intervals;
    for (int i = 1; i < noteTimes.size(); ++i) {
        double interval = noteTimes[i] - noteTimes[i-1];
        if (interval > 0.1 && interval < 2.0) {
            intervals.add(interval);
        }
    }

    if (intervals.isEmpty()) {
        return INIConfig::Defaults::DEFAULT_TEMPO;
    }

    intervals.sort();
    double modeInterval = intervals[intervals.size() / 2];

    float tempo = 60.0f / static_cast<float>(modeInterval);

    tempo = std::round(tempo / INIConfig::LayoutConstants::midiFileManagerTempoRoundTo) *
            INIConfig::LayoutConstants::midiFileManagerTempoRoundTo;

    return juce::jlimit(INIConfig::LayoutConstants::midiFileManagerTempoEstimateMin,
                       INIConfig::LayoutConstants::midiFileManagerTempoEstimateMax, tempo);
}

void MidiFileManager::autoMapMidiFileToKit(const juce::String& fileName, int playerIndex) {
    MidiGrooveAnalysis analysis = analyzeMidiFile(fileName);

}

juce::StringArray MidiFileManager::suggestSimilarGrooves(const juce::String& fileName, int maxSuggestions) {
    juce::StringArray suggestions;
    MidiGrooveAnalysis targetAnalysis = analyzeMidiFile(fileName);

    juce::Array<std::pair<float, juce::String>> scores;

    for (const auto& group : availableGroups) {
        for (const auto& file : group.midiFiles) {
            if (file == fileName) continue;

            MidiGrooveAnalysis analysis = analyzeMidiFile(file);

            float tempoScore = 1.0f - std::abs(analysis.tempo - targetAnalysis.tempo) / 100.0f;
            float swingScore = 1.0f - std::abs(analysis.averageSwing - targetAnalysis.averageSwing) / 100.0f;
            float tightnessScore = 1.0f - std::abs(analysis.grooveTightness - targetAnalysis.grooveTightness);
            float velocityScore = 1.0f - std::abs(analysis.averageVelocity - targetAnalysis.averageVelocity) / 127.0f;

            float totalScore = (tempoScore + swingScore + tightnessScore + velocityScore) / 4.0f;

            scores.add({totalScore, file});
        }
    }

    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    for (int i = 0; i < juce::jmin(maxSuggestions, scores.size()); ++i) {
        suggestions.add(scores[i].second);
    }

    return suggestions;
}

juce::MidiMessageSequence MidiFileManager::extractGrooveFromMidiFile(const juce::String& fileName) {
    juce::MidiMessageSequence groove;
    juce::File midiFile = getMidiFile(fileName);

    if (!midiFile.existsAsFile()) {
        return groove;
    }

    juce::FileInputStream fileStream(midiFile);
    if (!fileStream.openedOk()) {
        return groove;
    }

    juce::MidiFile midiFileData;
    if (!midiFileData.readFrom(fileStream)) {
        return groove;
    }

    if (midiFileData.getNumTracks() > 0) {
        const juce::MidiMessageSequence* track = midiFileData.getTrack(0);
        if (track) {
            for (int i = 0; i < track->getNumEvents(); ++i) {
                const auto* event = track->getEventPointer(i);
                if (event) {
                    juce::MidiMessage msg = event->message;

                    if (msg.isNoteOn() || msg.isNoteOff()) {
                        msg.setNoteNumber(60);
                    }

                    groove.addEvent(msg, event->message.getTimeStamp());
                }
            }
        }
    }

    groove.sort();
    return groove;
}

bool MidiFileManager::saveGrooveAsTemplate(const juce::String& sourceMidiFile, const juce::String& templateName) {
    juce::MidiMessageSequence groove = extractGrooveFromMidiFile(sourceMidiFile);

    if (groove.getNumEvents() == 0) {
        return false;
    }

    juce::MidiFile templateFile;
    templateFile.setTicksPerQuarterNote(INIConfig::LayoutConstants::midiFileManagerTicksPerQuarter);

    juce::MidiMessageSequence tempoTrack;
    tempoTrack.addEvent(juce::MidiMessage::tempoMetaEvent(INIConfig::LayoutConstants::midiFileManagerTempoMetaEvent));
    templateFile.addTrack(tempoTrack);

    templateFile.addTrack(groove);

    juce::File outputFile = grooveTemplatesFolder.getChildFile(templateName + ".mid");
    juce::FileOutputStream stream(outputFile);

    if (stream.openedOk()) {
        return templateFile.writeTo(stream);
    }

    return false;
}

juce::StringArray MidiFileManager::getGrooveTemplates() const {
    juce::StringArray templates;

    auto files = grooveTemplatesFolder.findChildFiles(juce::File::findFiles, false, "*.mid");
    for (const auto& file : files) {
        templates.add(file.getFileNameWithoutExtension());
    }

    return templates;
}

bool MidiFileManager::exportMidiWithMixerAutomation(const juce::String& outputFileName,
                                                   const juce::Array<juce::MidiMessageSequence>& tracks,
                                                   const juce::Array<float>& volumeAutomation,
                                                   const juce::Array<float>& panAutomation) {
    juce::MidiFile outputFile;
    outputFile.setTicksPerQuarterNote(INIConfig::LayoutConstants::midiFileManagerTicksPerQuarter);

    juce::MidiMessageSequence tempoTrack;
    tempoTrack.addEvent(juce::MidiMessage::tempoMetaEvent(INIConfig::LayoutConstants::midiFileManagerTempoMetaEvent));
    outputFile.addTrack(tempoTrack);

    for (int i = 0; i < tracks.size(); ++i) {
        juce::MidiMessageSequence trackWithAutomation = tracks[i];

        if (i < volumeAutomation.size()) {
            int volumeCC = static_cast<int>(volumeAutomation[i] * 127.0f);
            trackWithAutomation.addEvent(juce::MidiMessage::controllerEvent(i + 1, 7, volumeCC), 0.0);
        }

        if (i < panAutomation.size()) {
            int panCC = static_cast<int>((panAutomation[i] + 1.0f) * 63.5f);
            trackWithAutomation.addEvent(juce::MidiMessage::controllerEvent(i + 1, 10, panCC), 0.0);
        }

        trackWithAutomation.sort();
        outputFile.addTrack(trackWithAutomation);
    }

    juce::File output = juce::File::getCurrentWorkingDirectory().getChildFile(outputFileName);
    juce::FileOutputStream stream(output);

    if (stream.openedOk()) {
        return outputFile.writeTo(stream);
    }

    return false;
}

bool MidiFileManager::isREXFile(const juce::File& file) const {
    return file.hasFileExtension("rex") || file.hasFileExtension("rex2") || file.hasFileExtension("rx2");
}

juce::Array<MidiFileManager::REXSlice> MidiFileManager::parseREXFile(const juce::File& rexFile) {
    juce::Array<REXSlice> slices;

    return slices;
}

juce::Array<juce::MidiMessageSequence> MidiFileManager::loadREXFile(const juce::File& rexFile) {
    juce::Array<juce::MidiMessageSequence> tracks;

    if (!isREXFile(rexFile)) {
        return tracks;
    }

    auto slices = parseREXFile(rexFile);

    juce::MidiMessageSequence track;
    double currentTime = 0.0;

    for (const auto& slice : slices) {
        int noteNumber = 60 + slice.pitchOffset;
        track.addEvent(juce::MidiMessage::noteOn(1, noteNumber, INIConfig::LayoutConstants::midiFileManagerVelocityBase), currentTime);
        track.addEvent(juce::MidiMessage::noteOff(1, noteNumber), currentTime + slice.length);
        currentTime += slice.length;
    }

    track.sort();
    tracks.add(track);

    return tracks;
}

bool MidiFileManager::convertREXToMidi(const juce::File& rexFile, const juce::File& outputMidiFile) {
    auto tracks = loadREXFile(rexFile);

    if (tracks.isEmpty()) {
        return false;
    }

    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(960);

    for (const auto& track : tracks) {
        midiFile.addTrack(track);
    }

    juce::FileOutputStream stream(outputMidiFile);
    if (stream.openedOk()) {
        return midiFile.writeTo(stream);
    }

    return false;
}

juce::MidiMessageSequence MidiFileManager::mergePatterns(const juce::Array<juce::MidiMessageSequence>& patterns) {
    juce::MidiMessageSequence merged;

    for (const auto& pattern : patterns) {
        for (int i = 0; i < pattern.getNumEvents(); ++i) {
            const auto* event = pattern.getEventPointer(i);
            if (event) {
                merged.addEvent(event->message, event->message.getTimeStamp());
            }
        }
    }

    merged.sort();
    return merged;
}

juce::Array<juce::MidiMessageSequence> MidiFileManager::splitPattern(const juce::MidiMessageSequence& pattern,
                                                                   int numberOfParts) {
    juce::Array<juce::MidiMessageSequence> parts;

    if (numberOfParts <= 0 || pattern.getNumEvents() == 0) {
        return parts;
    }

    double totalDuration = pattern.getEndTime() - pattern.getStartTime();
    double partDuration = totalDuration / numberOfParts;

    for (int part = 0; part < numberOfParts; ++part) {
        juce::MidiMessageSequence partSequence;
        double partStart = part * partDuration;
        double partEnd = (part + 1) * partDuration;

        for (int i = 0; i < pattern.getNumEvents(); ++i) {
            const auto* event = pattern.getEventPointer(i);
            if (event) {
                double eventTime = event->message.getTimeStamp();
                if (eventTime >= partStart && eventTime < partEnd) {
                    juce::MidiMessage msg = event->message;
                    msg.setTimeStamp(eventTime - partStart);
                    partSequence.addEvent(msg);
                }
            }
        }

        partSequence.sort();
        parts.add(partSequence);
    }

    return parts;
}

juce::MidiMessageSequence MidiFileManager::createVariation(const juce::MidiMessageSequence& pattern,
                                                          float variationAmount) {
    juce::MidiMessageSequence variation = pattern;
    juce::Random random;

    variationAmount = juce::jlimit(0.0f, INIConfig::LayoutConstants::midiFileManagerVariationMax, variationAmount);

    for (int i = 0; i < variation.getNumEvents(); ++i) {
        auto* event = variation.getEventPointer(i);
        if (event && event->message.isNoteOn()) {
            int originalVelocity = event->message.getVelocity();
            int velocityVariation = static_cast<int>(random.nextFloat() *
                INIConfig::LayoutConstants::midiEngineVelocityRandomRange * variationAmount -
                10 * variationAmount);
            int newVelocity = juce::jlimit(INIConfig::LayoutConstants::midiFileManagerVelocityMin,
                                          INIConfig::LayoutConstants::midiFileManagerVelocityMax,
                                          originalVelocity + velocityVariation);

            double timingVariation = (random.nextFloat() - 0.5) *
                INIConfig::LayoutConstants::midiEngineHumanizeTimingRange * variationAmount;
            double newTime = event->message.getTimeStamp() + timingVariation;

            juce::MidiMessage newMsg = juce::MidiMessage::noteOn(
                event->message.getChannel(),
                event->message.getNoteNumber(),
                static_cast<juce::uint8>(newVelocity)
            );
            newMsg.setTimeStamp(juce::jmax(0.0, newTime));

            event->message = newMsg;

            if (variationAmount > 0.5f && random.nextFloat() < (variationAmount - 0.5f) * 0.3f) {
                event->message = juce::MidiMessage();
            }
        }
    }

    for (int i = variation.getNumEvents() - 1; i >= 0; --i) {
        auto* event = variation.getEventPointer(i);
        if (event && event->message.getRawDataSize() == 0) {
            variation.deleteEvent(i, false);
        }
    }

    variation.sort();
    return variation;
}

void MidiFileManager::quantizeToGrid(juce::MidiMessageSequence& sequence, int gridSubdivision) {
    double gridInterval = 4.0 / gridSubdivision;

    for (int i = 0; i < sequence.getNumEvents(); ++i) {
        auto* event = sequence.getEventPointer(i);
        if (event) {
            double originalTime = event->message.getTimeStamp();
            double quantizedTime = std::round(originalTime / gridInterval) * gridInterval;
            event->message.setTimeStamp(quantizedTime);
        }
    }

    sequence.sort();
}

void MidiFileManager::humanizePattern(juce::MidiMessageSequence& sequence, float amount) {
    juce::Random random;
    amount = juce::jlimit(0.0f, 1.0f, amount);

    for (int i = 0; i < sequence.getNumEvents(); ++i) {
        auto* event = sequence.getEventPointer(i);
        if (event && event->message.isNoteOn()) {
            double timingOffset = (random.nextFloat() - 0.5) *
                INIConfig::LayoutConstants::midiEngineTimingRandomRange * amount;
            double newTime = event->message.getTimeStamp() + timingOffset;
            event->message.setTimeStamp(juce::jmax(0.0, newTime));

            int velocity = event->message.getVelocity();
            int velocityOffset = static_cast<int>((random.nextFloat() - 0.5) *
                INIConfig::LayoutConstants::midiEngineVelocityRandomRange * amount);
            int newVelocity = juce::jlimit(INIConfig::LayoutConstants::midiFileManagerVelocityMin,
                                          INIConfig::LayoutConstants::midiFileManagerVelocityMax,
                                          velocity + velocityOffset);

            event->message = juce::MidiMessage::noteOn(
                event->message.getChannel(),
                event->message.getNoteNumber(),
                static_cast<juce::uint8>(newVelocity)
            );
            event->message.setTimeStamp(newTime);
        }
    }

    sequence.sort();
}

void MidiFileManager::applySwing(juce::MidiMessageSequence& sequence, float swingAmount) {
    swingAmount = juce::jlimit(0.0f, 100.0f, swingAmount);
    float swingRatio = swingAmount / INIConfig::LayoutConstants::midiFileManagerSwingRatioDiv;

    for (int i = 0; i < sequence.getNumEvents(); ++i) {
        auto* event = sequence.getEventPointer(i);
        if (event) {
            double time = event->message.getTimeStamp();
            double beat = std::floor(time);
            double subdivision = time - beat;

            if (subdivision > INIConfig::LayoutConstants::midiEngineSwingThreshold &&
                subdivision < INIConfig::LayoutConstants::midiEngineSwingUpperThreshold) {
                double swingOffset = (0.5 - subdivision) * (swingRatio - 1.0);
                event->message.setTimeStamp(time + swingOffset);
            }
        }
    }

    sequence.sort();
}

void MidiFileManager::organizeFilesByTempo() {
    for (int i = availableGroups.size() - 1; i >= 0; --i) {
        if (!availableGroups[i].isCustomGroup) {
            availableGroups.remove(i);
        }
    }

    std::map<int, juce::Array<juce::String>> tempoGroups;

    for (const auto& fileName : getAllMidiFilesAlphabetically()) {
        MidiGrooveAnalysis analysis = analyzeMidiFile(fileName);
        int tempoRange = static_cast<int>(analysis.tempo / 10) * 10;
        tempoGroups[tempoRange].add(fileName);
    }

    for (const auto& pair : tempoGroups) {
        if (pair.second.size() > 0) {
            juce::String groupName = juce::String(pair.first) + "-" + juce::String(pair.first + 9) + " BPM";
            MidiFileGroup group(groupName, "", false);
            group.midiFiles = pair.second;

            for (const auto& file : pair.second) {
                group.displayNames.add(truncateTextForButton(file));
            }

            availableGroups.add(group);
        }
    }
}

void MidiFileManager::organizeFilesByGroove() {
    for (int i = availableGroups.size() - 1; i >= 0; --i) {
        if (!availableGroups[i].isCustomGroup) {
            availableGroups.remove(i);
        }
    }

    juce::Array<juce::String> straightGrooves;
    juce::Array<juce::String> swungGrooves;
    juce::Array<juce::String> shuffleGrooves;

    for (const auto& fileName : getAllMidiFilesAlphabetically()) {
        MidiGrooveAnalysis analysis = analyzeMidiFile(fileName);

        if (analysis.averageSwing < 55.0f) {
            straightGrooves.add(fileName);
        } else if (analysis.averageSwing < 65.0f) {
            swungGrooves.add(fileName);
        } else {
            shuffleGrooves.add(fileName);
        }
    }

    if (straightGrooves.size() > 0) {
        MidiFileGroup group("Straight Grooves", "", false);
        group.midiFiles = straightGrooves;
        for (const auto& file : straightGrooves) {
            group.displayNames.add(truncateTextForButton(file));
        }
        availableGroups.add(group);
    }

    if (swungGrooves.size() > 0) {
        MidiFileGroup group("Swung Grooves", "", false);
        group.midiFiles = swungGrooves;
        for (const auto& file : swungGrooves) {
            group.displayNames.add(truncateTextForButton(file));
        }
        availableGroups.add(group);
    }

    if (shuffleGrooves.size() > 0) {
        MidiFileGroup group("Shuffle Grooves", "", false);
        group.midiFiles = shuffleGrooves;
        for (const auto& file : shuffleGrooves) {
            group.displayNames.add(truncateTextForButton(file));
        }
        availableGroups.add(group);
    }
}

void MidiFileManager::createSmartGroups() {
    struct SmartGroupCriteria {
        juce::String name;
        float minTempo, maxTempo;
        float minSwing, maxSwing;
        float minTightness, maxTightness;
        float minDensity, maxDensity;
    };

    SmartGroupCriteria criteria[] = {
        {"Tight & Fast", 140.0f, 200.0f, 0.0f, 100.0f, 0.8f, 1.0f, 0.0f, 10.0f},
        {"Loose & Slow", 60.0f, 100.0f, 0.0f, 100.0f, 0.0f, 0.5f, 0.0f, 10.0f},
        {"Dense Patterns", 0.0f, 200.0f, 0.0f, 100.0f, 0.0f, 1.0f, 4.0f, 10.0f},
        {"Sparse Patterns", 0.0f, 200.0f, 0.0f, 100.0f, 0.0f, 1.0f, 0.0f, 2.0f},
        {"Hip Hop Feel", 80.0f, 100.0f, 55.0f, 75.0f, 0.3f, 0.7f, 1.0f, 4.0f},
        {"Electronic", 120.0f, 140.0f, 45.0f, 55.0f, 0.9f, 1.0f, 2.0f, 8.0f},
        {"Jazz/Swing", 100.0f, 140.0f, 60.0f, 80.0f, 0.2f, 0.6f, 2.0f, 6.0f}
    };

    for (const auto& criterion : criteria) {
        juce::Array<juce::String> matchingFiles;

        for (const auto& fileName : getAllMidiFilesAlphabetically()) {
            MidiGrooveAnalysis analysis = analyzeMidiFile(fileName);

            if (analysis.tempo >= criterion.minTempo && analysis.tempo <= criterion.maxTempo &&
                analysis.averageSwing >= criterion.minSwing && analysis.averageSwing <= criterion.maxSwing &&
                analysis.grooveTightness >= criterion.minTightness && analysis.grooveTightness <= criterion.maxTightness &&
                analysis.noteDensity >= criterion.minDensity && analysis.noteDensity <= criterion.maxDensity) {
                matchingFiles.add(fileName);
            }
        }

        if (matchingFiles.size() > 0) {
            MidiFileGroup group(criterion.name, "", false);
            group.midiFiles = matchingFiles;
            for (const auto& file : matchingFiles) {
                group.displayNames.add(truncateTextForButton(file));
            }
            availableGroups.add(group);
        }
    }
}

void MidiFileManager::setMidiFilesFolder(const juce::File& folder) {
    midiFilesFolder = folder;
    if (midiFilesFolder.exists()) {
        scanMidiFiles();
        createUnlimitedGroupsWithSixteenMidiFiles();
        initializeBeatsButtonGroups();
    }
}

void MidiFileManager::scanMidiFiles() {
    availableGroups.clear();
    rootMidiFileNames.clear();

    if (!midiFilesFolder.exists()) {
        return;
    }

    juce::Array<juce::File> subFolders;
    midiFilesFolder.findChildFiles(subFolders, juce::File::findDirectories, false);

    for (const auto& subFolder : subFolders) {
        MidiFileGroup group(subFolder.getFileName(), subFolder.getFullPathName(), false);
        juce::Array<juce::File> midiFiles;
        subFolder.findChildFiles(midiFiles, juce::File::findFiles, false, "*.mid;*.MID;*.midi;*.MIDI");

        juce::StringArray fileNames;
        for (const auto& midiFile : midiFiles) {
            fileNames.add(midiFile.getFileNameWithoutExtension());
        }

        fileNames.sort(true);

        for (const auto& fileName : fileNames) {
            group.midiFiles.add(fileName);
            group.displayNames.add(truncateTextForButton(fileName));
        }

        if (!group.midiFiles.isEmpty()) {
            availableGroups.add(group);
        }
    }

    juce::Array<juce::File> rootMidiFiles;
    midiFilesFolder.findChildFiles(rootMidiFiles, juce::File::findFiles, false, "*.mid;*.MID;*.midi;*.MIDI");

    if (!rootMidiFiles.isEmpty()) {
        juce::StringArray rootFileNames;
        for (const auto& midiFile : rootMidiFiles) {
            rootFileNames.add(midiFile.getFileNameWithoutExtension());
        }

        rootFileNames.sort(true);
        rootMidiFileNames = rootFileNames;
    }
}

juce::Array<juce::String> MidiFileManager::getAllMidiFilesAlphabetically() const {
    juce::Array<juce::String> allMidiFiles;

    if (!rootMidiFileNames.isEmpty()) {
        for (const auto& fileName : rootMidiFileNames) {
            allMidiFiles.add(fileName);
        }
    }

    for (const auto& group : availableGroups) {
        if (!group.isCustomGroup) {
            for (const auto& file : group.midiFiles) {
                allMidiFiles.addIfNotAlreadyThere(file);
            }
        }
    }

    allMidiFiles.sort();
    return allMidiFiles;
}

int MidiFileManager::getNumberOfRequiredGroups() const {
    auto allFiles = getAllMidiFilesAlphabetically();
    int numFiles = allFiles.size();
    int numGroups = (numFiles + 15) / 16;
    return numGroups;
}

void MidiFileManager::createGroupsFromSortedMidiFiles(const juce::Array<juce::String>& sortedFiles) {
    for (int i = availableGroups.size() - 1; i >= 0; --i) {
        if (availableGroups[i].isCustomGroup) {
            availableGroups.remove(i);
        }
    }

    int numFiles = sortedFiles.size();
    if (numFiles == 0) {
        return;
    }

    int groupNumber = 1;
    int currentFileIndex = 0;

    while (currentFileIndex < numFiles) {
        juce::String groupName = INIConfig::Defaults::DEFAULT_GROUP_PREFIX + juce::String(groupNumber);
        juce::Array<juce::String> groupFiles;

        for (int i = 0; i < INIConfig::LayoutConstants::midiFileManagerGrooveGroups && currentFileIndex < numFiles; ++i) {
            groupFiles.add(sortedFiles[currentFileIndex]);
            currentFileIndex++;
        }

        addBeatsButtonGroup(groupName, groupFiles, false, 0);
        groupNumber++;
    }
}

void MidiFileManager::createUnlimitedGroupsWithSixteenMidiFiles() {
    auto allMidiFiles = getAllMidiFilesAlphabetically();
    if (allMidiFiles.isEmpty()) {
        return;
    }

    createGroupsFromSortedMidiFiles(allMidiFiles);
}

void MidiFileManager::initializeBeatsButtonGroups() {
    bool hasBeatsButtonGroups = false;
    for (const auto& group : availableGroups) {
        if (group.isCustomGroup) {
            hasBeatsButtonGroups = true;
            break;
        }
    }

    if (!hasBeatsButtonGroups) {
        createUnlimitedGroupsWithSixteenMidiFiles();
    }

    if (!availableGroups.isEmpty()) {
        bool groupSelected = false;
        for (const auto& group : availableGroups) {
            if (group.isCustomGroup) {
                currentGroupName = group.groupName;
                groupSelected = true;
                break;
            }
        }
        if (!groupSelected) {
            currentGroupName = availableGroups.getFirst().groupName;
        }
    }
}

void MidiFileManager::createInitialBeatsButtonGroups() {
    createUnlimitedGroupsWithSixteenMidiFiles();
}

void MidiFileManager::autoPopulateGroupsFromMidiFiles() {
    createUnlimitedGroupsWithSixteenMidiFiles();
}

void MidiFileManager::assignMidiFilesToButtons(const juce::Array<juce::String>& midiFiles) {
    createUnlimitedGroupsWithSixteenMidiFiles();
}

void MidiFileManager::addBeatsButtonGroup(const juce::String& groupName, const juce::Array<juce::String>& fileNames, bool isFavorite, int selectedButton) {
    if (!INIConfig::isValidButtonIndex(selectedButton)) {
        selectedButton = INIConfig::clampButtonIndex(selectedButton);
    }

    for (auto& group : availableGroups) {
        if (group.groupName == groupName && group.isCustomGroup) {
            group.midiFiles.clear();
            group.displayNames.clear();
            group.isFavorite = isFavorite;
            group.selectedButton = selectedButton;

            for (const auto& fileName : fileNames) {
                group.midiFiles.add(fileName);
                group.displayNames.add(truncateTextForButton(fileName));
            }
            return;
        }
    }

    MidiFileGroup customGroup(groupName, "", true);
    customGroup.isFavorite = isFavorite;
    customGroup.selectedButton = selectedButton;

    for (const auto& fileName : fileNames) {
        customGroup.midiFiles.add(fileName);
        customGroup.displayNames.add(truncateTextForButton(fileName));
    }

    availableGroups.add(customGroup);
}

void MidiFileManager::removeBeatsButtonGroup(const juce::String& groupName) {
    for (int i = availableGroups.size() - 1; i >= 0; --i) {
        if (availableGroups[i].groupName == groupName && availableGroups[i].isCustomGroup) {
            availableGroups.remove(i);

            if (currentGroupName == groupName && !availableGroups.isEmpty()) {
                bool foundNewGroup = false;
                for (const auto& group : availableGroups) {
                    if (group.isCustomGroup) {
                        currentGroupName = group.groupName;
                        foundNewGroup = true;
                        break;
                    }
                }
                if (!foundNewGroup) {
                    currentGroupName = availableGroups.getFirst().groupName;
                }
            }
            break;
        }
    }
}

bool MidiFileManager::isBeatsButtonGroup(const juce::String& groupName) const {
    for (const auto& group : availableGroups) {
        if (group.groupName == groupName) {
            return group.isCustomGroup;
        }
    }
    return false;
}

juce::Array<juce::String> MidiFileManager::getBeatsButtonGroupFiles(const juce::String& groupName) const {
    for (const auto& group : availableGroups) {
        if (group.groupName == groupName && group.isCustomGroup) {
            return group.midiFiles;
        }
    }
    return {};
}

void MidiFileManager::setBeatsButtonGroupFiles(const juce::String& groupName, const juce::Array<juce::String>& fileNames) {
    for (auto& group : availableGroups) {
        if (group.groupName == groupName && group.isCustomGroup) {
            group.midiFiles.clear();
            group.displayNames.clear();

            for (const auto& fileName : fileNames) {
                group.midiFiles.add(fileName);
                group.displayNames.add(truncateTextForButton(fileName));
            }
            break;
        }
    }
}

void MidiFileManager::setBeatsButtonGroupFavorite(const juce::String& groupName, bool isFavorite) {
    for (auto& group : availableGroups) {
        if (group.groupName == groupName && group.isCustomGroup) {
            group.isFavorite = isFavorite;
            break;
        }
    }
}

bool MidiFileManager::getBeatsButtonGroupFavorite(const juce::String& groupName) const {
    for (const auto& group : availableGroups) {
        if (group.groupName == groupName && group.isCustomGroup) {
            return group.isFavorite;
        }
    }
    return false;
}

void MidiFileManager::setBeatsButtonGroupSelectedButton(const juce::String& groupName, int buttonIndex) {
    buttonIndex = INIConfig::clampButtonIndex(buttonIndex);

    for (auto& group : availableGroups) {
        if (group.groupName == groupName && group.isCustomGroup) {
            group.selectedButton = buttonIndex;
            break;
        }
    }
}

int MidiFileManager::getBeatsButtonGroupSelectedButton(const juce::String& groupName) const {
    for (const auto& group : availableGroups) {
        if (group.groupName == groupName && group.isCustomGroup) {
            return INIConfig::clampButtonIndex(group.selectedButton);
        }
    }
    return 0;
}

juce::StringArray MidiFileManager::getGroupNames() const {
    juce::StringArray names;
    for (const auto& group : availableGroups) {
        names.add(group.groupName);
    }
    return names;
}

juce::Array<juce::String> MidiFileManager::getCurrentGroupFiles() const {
    for (const auto& group : availableGroups) {
        if (group.groupName == currentGroupName) {
            return group.midiFiles;
        }
    }
    return {};
}

juce::Array<juce::String> MidiFileManager::getCurrentGroupDisplayNames() const {
    for (const auto& group : availableGroups) {
        if (group.groupName == currentGroupName) {
            return group.displayNames;
        }
    }
    return {};
}

bool MidiFileManager::selectGroup(const juce::String& groupName) {
    for (const auto& group : availableGroups) {
        if (group.groupName == groupName) {
            currentGroupName = groupName;
            return true;
        }
    }
    return false;
}

juce::File MidiFileManager::getMidiFile(const juce::String& fileName) const {
    for (const auto& group : availableGroups) {
        if (group.groupName == currentGroupName) {
            if (group.isCustomGroup) {
                juce::Array<juce::File> allMidiFiles;
                midiFilesFolder.findChildFiles(allMidiFiles, juce::File::findFilesAndDirectories, true, "*.mid;*.MID;*.midi;*.MIDI");

                for (const auto& file : allMidiFiles) {
                    if (file.getFileNameWithoutExtension() == fileName) {
                        return file;
                    }
                }
            } else {
                juce::File groupFolder(group.folderPath);
                juce::String extensions[] = {".mid", ".MID", ".midi", ".MIDI"};

                for (const auto& ext : extensions) {
                    juce::File midiFile = groupFolder.getChildFile(fileName + ext);
                    if (midiFile.existsAsFile()) {
                        return midiFile;
                    }
                }
            }
        }
    }
    return juce::File();
}

void MidiFileManager::addCustomGroup(const juce::String& groupName, const juce::Array<juce::String>& fileNames) {
    for (auto& group : availableGroups) {
        if (group.groupName == groupName) {
            group.midiFiles.clear();
            group.displayNames.clear();

            for (const auto& fileName : fileNames) {
                group.midiFiles.add(fileName);
                group.displayNames.add(truncateTextForButton(fileName));
            }
            return;
        }
    }

    MidiFileGroup customGroup(groupName, "", false);
    for (const auto& fileName : fileNames) {
        customGroup.midiFiles.add(fileName);
        customGroup.displayNames.add(truncateTextForButton(fileName));
    }

    availableGroups.add(customGroup);
}

void MidiFileManager::removeCustomGroup(const juce::String& groupName) {
    for (int i = availableGroups.size() - 1; i >= 0; --i) {
        if (availableGroups[i].groupName == groupName && availableGroups[i].folderPath.isEmpty()) {
            availableGroups.remove(i);

            if (currentGroupName == groupName && !availableGroups.isEmpty()) {
                currentGroupName = availableGroups.getFirst().groupName;
            }
            break;
        }
    }
}

juce::String MidiFileManager::truncateTextForButton(const juce::String& text, int maxLength) {
    if (text.length() <= maxLength) {
        return text;
    }

    if (maxLength > 3) {
        juce::String truncated = text.substring(0, maxLength - 3);
        int lastSpace = truncated.lastIndexOf(" ");
        if (lastSpace > maxLength / 2) {
            return truncated.substring(0, lastSpace) + "...";
        }
    }

    return text.substring(0, maxLength - 3) + "...";
}

void MidiFileManager::saveStates(ComponentState& state) {
    state.beatsButtonGroups.clear();

    for (const auto& group : availableGroups) {
        if (group.isCustomGroup) {
            BeatsButtonGroup beatsGroup(group.groupName);
            beatsGroup.isCustomGroup = true;
            beatsGroup.selectedButton = INIConfig::clampButtonIndex(group.selectedButton);
            beatsGroup.isFavorite = group.isFavorite;

            beatsGroup.midiFiles.clear();
            for (const auto& file : group.midiFiles) {
                beatsGroup.midiFiles.add(file);
            }

            if (!group.midiFiles.isEmpty()) {
                MidiGrooveAnalysis analysis = analyzeMidiFile(group.midiFiles[0]);
                beatsGroup.averageSwing = analysis.averageSwing;
                beatsGroup.averageVelocity = analysis.averageVelocity;
                beatsGroup.timeSignatureNumerator = analysis.timeSignatureNumerator;
                beatsGroup.timeSignatureDenominator = analysis.timeSignatureDenominator;
                beatsGroup.grooveTightness = analysis.grooveTightness;
            }

            state.beatsButtonGroups.add(beatsGroup);
        }
    }
}

void MidiFileManager::loadStates(const ComponentState& state) {
    for (int i = availableGroups.size() - 1; i >= 0; --i) {
        if (availableGroups[i].isCustomGroup) {
            availableGroups.remove(i);
        }
    }

    for (const auto& beatsGroup : state.beatsButtonGroups) {
        if (beatsGroup.isCustomGroup) {
            MidiFileGroup group(beatsGroup.groupName, "", true);
            group.selectedButton = INIConfig::clampButtonIndex(beatsGroup.selectedButton);
            group.isFavorite = beatsGroup.isFavorite;

            for (const auto& file : beatsGroup.midiFiles) {
                group.midiFiles.add(file);
                group.displayNames.add(truncateTextForButton(file));
            }

            availableGroups.add(group);
        }
    }

    bool currentGroupExists = false;
    for (const auto& group : availableGroups) {
        if (group.groupName == currentGroupName) {
            currentGroupExists = true;
            break;
        }
    }

    if (!currentGroupExists && !availableGroups.isEmpty()) {
        currentGroupName = availableGroups.getFirst().groupName;
    }
}
