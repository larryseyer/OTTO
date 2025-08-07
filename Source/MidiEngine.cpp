#include "MidiEngine.h"
#include "INIConfig.h"
#include "ErrorHandling.h"
#include "MidiFileManager.h"

MidiEngine::MidiEngine() : currentPlayerIndex(0), isPlaying(false), tempo(120.0f), hostTempo(0.0) {
    for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
        players[i].playerIndex = i;
        players[i].swing = INIConfig::Defaults::SWING;
        players[i].energy = INIConfig::Defaults::ENERGY;
        players[i].outputChannel = i + 1;
    }

    initializeScenes();
}

MidiEngine::~MidiEngine() {
    stopPlayback();
}

void MidiEngine::initializeScenes() {
    scenes.clear();
    for (int i = 0; i < INIConfig::Defaults::MAX_SCENES; ++i) {
        Scene scene;
        scene.name = "";
        scene.tempo = tempo;
        scenes.add(scene);
    }
}

void MidiEngine::prepare(double sampleRate) {
    juce::ignoreUnused(sampleRate);
    this->sampleRate = sampleRate;

    for (auto& player : players) {
        player.lastProcessTime = 0;
        player.playbackPosition = 0;
    }
}

void MidiEngine::process(juce::MidiBuffer& midiMessages) {
    try {
        if (!isPlaying) {
            midiMessages.clear();
            return;
        }

        #if JUCE_MAC || JUCE_IOS
            const double currentTime = juce::Time::getHighResolutionTicks() / juce::Time::getHighResolutionTicksPerSecond() * 1000.0;
        #elif JUCE_WINDOWS
            const double currentTime = juce::Time::getMillisecondCounterHiRes();
        #else
            const double currentTime = juce::Time::getMillisecondCounterHiRes();
        #endif
        
        const double deltaTime = currentTime - lastProcessTime;
        lastProcessTime = currentTime;

        static thread_local juce::MidiBuffer tempBuffer;
        tempBuffer.clear();

        processMidiInput(midiMessages);

        if (liveRecording) {
            processLiveRecording(midiMessages);
        }

        if (loopRecordingMode) {
            processLoopRecording();
        }

        midiMessages.clear();

        processQueuedChanges();

        for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
            if (players[i].enabled) {
                processPlayer(i, midiMessages, deltaTime);
            }
        }

        if (sendMidiClock) {
            generateMidiClock(midiMessages);
        }
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Failed to process MIDI: " + juce::String(e.what()), "MidiEngine");
        midiMessages.clear();
    }
}

void MidiEngine::processLiveRecording(const juce::MidiBuffer& midiMessages) {
    if (!liveRecording) return;

    for (const auto metadata : midiMessages) {
        const auto message = metadata.getMessage();

        if (message.isNoteOnOrOff()) {
            double timestamp = juce::Time::getMillisecondCounterHiRes() - recordStartTime;

            if (overdubMode && currentPlayerIndex >= 0 && currentPlayerIndex < INIConfig::Defaults::MAX_PLAYERS) {
                players[currentPlayerIndex].recordedPattern.addEvent(message, timestamp);
            } else {
                recordBuffer.addEvent(message, timestamp);
            }
        }
    }
}

void MidiEngine::processLoopRecording() {
    if (!loopRecordingMode || !liveRecording) return;

    double currentTime = juce::Time::getMillisecondCounterHiRes();
    double elapsedTime = currentTime - loopRecordingStartTime;
    double loopLengthMs = (loopRecordingBars * INIConfig::Defaults::BEATS_PER_BAR * INIConfig::Defaults::MS_PER_MINUTE) / tempo;

    if (elapsedTime >= loopLengthMs) {
        if (currentPlayerIndex >= 0 && currentPlayerIndex < INIConfig::Defaults::MAX_PLAYERS) {
            players[currentPlayerIndex].currentPattern = recordBuffer;
            players[currentPlayerIndex].recordedPattern.clear();
        }

        loopRecordingStartTime = currentTime;
        recordBuffer.clear();
    }
}

void MidiEngine::processQueuedChanges() {
    double currentBeat = getCurrentBeat();
    int currentBar = getCurrentBar();

    for (int i = queuedChanges.size() - 1; i >= 0; --i) {
        const auto& change = queuedChanges[i];

        bool shouldTrigger = false;

        if (change.quantizationBars == 0) {
            shouldTrigger = true;
        } else {
            if (currentBar % change.quantizationBars == 0 &&
                std::fmod(currentBeat, INIConfig::Defaults::BEATS_PER_BAR) < INIConfig::Defaults::BEAT_THRESHOLD) {
                shouldTrigger = true;
            }
        }

        if (shouldTrigger) {
            switch (change.type) {
                case QueuedChange::Scene:
                    loadScene(change.targetIndex);
                    break;

                case QueuedChange::Clip:
                    if (change.playerIndex >= 0 && change.playerIndex < INIConfig::Defaults::MAX_PLAYERS) {
                        selectPattern(change.playerIndex, change.targetIndex);
                    }
                    break;

                case QueuedChange::Pattern:
                    selectPattern(change.playerIndex, change.targetIndex);
                    break;
            }

            queuedChanges.remove(i);
        }
    }
}

void MidiEngine::startLiveRecording(bool overdub) {
    liveRecording = true;
    overdubMode = overdub;
    recordStartTime = juce::Time::getMillisecondCounterHiRes();

    if (!overdub) {
        recordBuffer.clear();
    }

    if (loopRecordingMode) {
        loopRecordingStartTime = recordStartTime;
    }
}

void MidiEngine::stopLiveRecording() {
    liveRecording = false;

    if (recordBuffer.getNumEvents() > 0 && currentPlayerIndex >= 0 && currentPlayerIndex < INIConfig::Defaults::MAX_PLAYERS) {
        players[currentPlayerIndex].currentPattern = recordBuffer;
    }
}

void MidiEngine::setLoopRecordingMode(bool enabled) {
    loopRecordingMode = enabled;

    if (enabled && liveRecording) {
        loopRecordingStartTime = juce::Time::getMillisecondCounterHiRes();
    }
}

void MidiEngine::tapTempo() {
    const double currentTime = juce::Time::getMillisecondCounterHiRes();

    if (currentTime - lastTapTime > INIConfig::LayoutConstants::tapTempoTimeoutMs) {
        tapTimes.clear();
    }

    tapTimes.add(currentTime);
    lastTapTime = currentTime;

    if (tapTimes.size() >= 2) {
        double totalInterval = 0.0;
        for (int i = 1; i < tapTimes.size(); ++i) {
            totalInterval += tapTimes[i] - tapTimes[i-1];
        }

        double avgInterval = totalInterval / (tapTimes.size() - 1);
        float newTempo = static_cast<float>(INIConfig::Defaults::MS_PER_MINUTE / avgInterval);

        setTempo(newTempo);

        while (tapTimes.size() > tapTempoAverageCount) {
            tapTimes.remove(0);
        }
    }
}

void MidiEngine::resetTapTempo() {
    tapTimes.clear();
    lastTapTime = 0;
}

float MidiEngine::getTapTempoAveraging() const {
    if (tapTimes.size() < 2) {
        return tempo;
    }

    double totalInterval = 0.0;
    for (int i = 1; i < tapTimes.size(); ++i) {
        totalInterval += tapTimes[i] - tapTimes[i-1];
    }

    double avgInterval = totalInterval / (tapTimes.size() - 1);
    return static_cast<float>(INIConfig::Defaults::MS_PER_MINUTE / avgInterval);
}

void MidiEngine::triggerScene(int sceneIndex) {
    if (sceneIndex >= 0 && sceneIndex < scenes.size()) {
        loadScene(sceneIndex);
    }
}

void MidiEngine::triggerClip(int sceneIndex, int playerIndex) {
    if (sceneIndex >= 0 && sceneIndex < scenes.size() &&
        playerIndex >= 0 && playerIndex < INIConfig::Defaults::MAX_PLAYERS) {

        const auto& scene = scenes[sceneIndex];
        if (scene.clips[playerIndex].active) {
            selectPattern(playerIndex, scene.clips[playerIndex].patternIndex);
        }
    }
}

void MidiEngine::queueSceneChange(int sceneIndex, int quantization) {
    if (sceneIndex < 0 || sceneIndex >= scenes.size()) return;

    QueuedChange change;
    change.type = QueuedChange::Scene;
    change.targetIndex = sceneIndex;
    change.playerIndex = -1;
    change.quantizationBars = quantization;
    change.triggerTime = juce::Time::getMillisecondCounterHiRes();

    queuedChanges.removeIf([](const QueuedChange& c) {
        return c.type == QueuedChange::Scene;
    });

    queuedChanges.add(change);
}

void MidiEngine::queueClipChange(int playerIndex, int patternIndex, int quantization) {
    if (playerIndex < 0 || playerIndex >= INIConfig::Defaults::MAX_PLAYERS) return;

    QueuedChange change;
    change.type = QueuedChange::Clip;
    change.targetIndex = patternIndex;
    change.playerIndex = playerIndex;
    change.quantizationBars = quantization;
    change.triggerTime = juce::Time::getMillisecondCounterHiRes();

    queuedChanges.removeIf([playerIndex](const QueuedChange& c) {
        return c.type == QueuedChange::Clip && c.playerIndex == playerIndex;
    });

    queuedChanges.add(change);
}

void MidiEngine::saveScene(int sceneIndex, const juce::String& name) {
    if (sceneIndex < 0 || sceneIndex >= scenes.size()) return;

    auto& scene = scenes.getReference(sceneIndex);
    scene.name = name.isEmpty() ? ("Scene " + juce::String(sceneIndex + 1)) : name;
    scene.tempo = tempo;

    for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
        scene.clips[i].active = players[i].enabled;
        scene.clips[i].patternIndex = players[i].selectedPattern;
        scene.clips[i].midiFileName = players[i].selectedMidiGroup;
        scene.clips[i].volume = players[i].energy / INIConfig::Defaults::MAX_ENERGY;
        scene.clips[i].muted = false;
    }
}

MidiEngine::Scene MidiEngine::getScene(int sceneIndex) const {
    if (sceneIndex >= 0 && sceneIndex < scenes.size()) {
        return scenes[sceneIndex];
    }
    return Scene();
}

void MidiEngine::loadScene(int sceneIndex) {
    if (sceneIndex < 0 || sceneIndex >= scenes.size()) return;

    const auto& scene = scenes[sceneIndex];
    activeSceneIndex = sceneIndex;

    if (scene.tempo > 0) {
        setTempo(scene.tempo);
    }

    for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
        const auto& clip = scene.clips[i];

        if (clip.active) {
            players[i].enabled = true;
            players[i].selectedPattern = clip.patternIndex;
            players[i].selectedMidiGroup = clip.midiFileName;
            players[i].energy = clip.volume * INIConfig::Defaults::MAX_ENERGY;

            if (!clip.midiFileName.isEmpty() && midiFileManager) {
                playMidiFile(i, clip.midiFileName);
            }
        } else {
            players[i].enabled = false;
        }
    }
}

void MidiEngine::clearScene(int sceneIndex) {
    if (sceneIndex >= 0 && sceneIndex < scenes.size()) {
        scenes.getReference(sceneIndex) = Scene();
        scenes.getReference(sceneIndex).name = "";
    }
}

void MidiEngine::processPlayer(int playerIndex, juce::MidiBuffer& midiMessages, double deltaTime) {
    if (playerIndex < 0 || playerIndex >= INIConfig::Defaults::MAX_PLAYERS) return;

    auto& player = players[playerIndex];
    if (!player.enabled || (player.currentPattern.getNumEvents() == 0 && player.recordedPattern.getNumEvents() == 0)) return;

    const double beatsPerMs = (tempo / INIConfig::Defaults::SECONDS_PER_MINUTE) / INIConfig::Defaults::MS_PER_SECOND;
    const double beats = deltaTime * beatsPerMs;
    player.playbackPosition += beats;

    const double patternLength = INIConfig::Defaults::BEATS_PER_BAR;
    if (player.playbackPosition >= patternLength) {
        player.playbackPosition = std::fmod(player.playbackPosition, patternLength);
    }

    generatePatternNotes(playerIndex, midiMessages);
}

void MidiEngine::generatePatternNotes(int playerIndex, juce::MidiBuffer& midiMessages) {
    auto& player = players[playerIndex];

    float velocityScale = player.energy / INIConfig::Defaults::MAX_ENERGY;

    const double beatPosition = std::fmod(player.playbackPosition, 1.0);
    if (beatPosition < INIConfig::Defaults::BEAT_THRESHOLD) {
        int velocity = static_cast<int>(INIConfig::LayoutConstants::midiEngineMaxMidiVelocity * velocityScale);
        velocity = juce::jlimit(1, INIConfig::LayoutConstants::midiEngineMaxMidiVelocity, velocity);

        auto noteOn = juce::MidiMessage::noteOn(player.outputChannel, INIConfig::LayoutConstants::midiEngineDefaultDrumNote, (juce::uint8)velocity);
        midiMessages.addEvent(noteOn, 0);

        auto noteOff = juce::MidiMessage::noteOff(player.outputChannel, INIConfig::LayoutConstants::midiEngineDefaultDrumNote);
        midiMessages.addEvent(noteOff, INIConfig::LayoutConstants::midiEngineNoteOffDelay);
    }
}

void MidiEngine::generateMidiClock(juce::MidiBuffer& midiMessages) {
    const double pulsesPerBeat = INIConfig::LayoutConstants::midiEngineMidiClockPulsesPerBeat;
    const double msPerPulse = (INIConfig::Defaults::MS_PER_MINUTE / tempo) / pulsesPerBeat;

    if (juce::Time::getMillisecondCounterHiRes() - lastClockTime >= msPerPulse) {
        auto clockMessage = juce::MidiMessage::midiClock();
        midiMessages.addEvent(clockMessage, 0);
        lastClockTime = juce::Time::getMillisecondCounterHiRes();
    }
}

void MidiEngine::startPlayback() {
    isPlaying = true;
    lastProcessTime = juce::Time::getMillisecondCounterHiRes();
    lastClockTime = lastProcessTime;

    for (auto& player : players) {
        player.playbackPosition = 0.0;
    }
}

void MidiEngine::stopPlayback() {
    isPlaying = false;

    if (liveRecording) {
        stopLiveRecording();
    }
}

void MidiEngine::setTempo(float newTempo) {
    tempo = INIConfig::clampTempo(newTempo);
}

void MidiEngine::setCurrentPlayer(int index) {
    currentPlayerIndex = INIConfig::clampPlayerIndex(index);
}

void MidiEngine::selectPattern(int playerIndex, int patternIndex) {
    if (!INIConfig::isValidPlayerIndex(playerIndex) ||
        !INIConfig::isValidButtonIndex(patternIndex)) {
        return;
    }

    players[playerIndex].selectedPattern = patternIndex;
}

void MidiEngine::playMidiFile(int playerIndex, const juce::String& filename) {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return;

    players[playerIndex].selectedMidiGroup = filename;
}

void MidiEngine::setSwing(int playerIndex, float swing) {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return;

    players[playerIndex].swing = INIConfig::clampSwing(swing);
}

void MidiEngine::setEnergy(int playerIndex, float energy) {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return;

    players[playerIndex].energy = INIConfig::clampEnergy(energy);
}

float MidiEngine::getSwing(int playerIndex) const {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return INIConfig::Defaults::SWING;

    return players[playerIndex].swing;
}

float MidiEngine::getEnergy(int playerIndex) const {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return INIConfig::Defaults::ENERGY;

    return players[playerIndex].energy;
}

void MidiEngine::triggerFill(int playerIndex) {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return;

    players[playerIndex].fillActive = true;
}

void MidiEngine::processMidiInput(const juce::MidiBuffer& midiMessages) {
    try {
        // Null-pointer safety: Validate MIDI buffer
        if (midiMessages.getNumEvents() == 0) {
            return;
        }

        for (const auto metadata : midiMessages) {
            // Null-pointer safety: Validate MIDI message
            if (!metadata.data) {
                continue;
            }

            const auto message = metadata.getMessage();
            
            // Null-pointer safety: Validate message before processing
            if (message.getRawDataSize() == 0) {
                continue;
            }

            if (message.isController()) {
                // Null-pointer safety: Validate controller values
                int channel = message.getChannel();
                int ccNumber = message.getControllerNumber();
                int value = message.getControllerValue();
                
                if (channel >= 1 && channel <= 16 && 
                    ccNumber >= 0 && ccNumber <= 127 &&
                    value >= 0 && value <= 127) {
                    
                    ErrorHandler::safeExecute([&]() {
                        handleMidiCC(channel, ccNumber, value);
                    }, "MidiEngine MIDI CC handling");
                } else {
                    DBG("MidiEngine: Invalid MIDI CC values - channel: " + 
                        juce::String(channel) + ", CC: " + juce::String(ccNumber) + 
                        ", value: " + juce::String(value));
                }
            }
        }
    } catch (const std::exception& e) {
        DBG("MidiEngine: Exception in processMidiInput - " + juce::String(e.what()));
    }
}

void MidiEngine::handleMidiCC(int channel, int ccNumber, int value) {
    try {
        // Null-pointer safety: Validate input parameters
        if (channel < 1 || channel > 16 ||
            ccNumber < 0 || ccNumber > 127 ||
            value < 0 || value > 127) {
            DBG("MidiEngine: Invalid MIDI CC parameters - channel: " + 
                juce::String(channel) + ", CC: " + juce::String(ccNumber) + 
                ", value: " + juce::String(value));
            return;
        }

        // Handle MIDI learn mode with null-pointer safety
        if (midiLearnActive && currentLearnParameter.isNotEmpty()) {
            MidiMapping newMapping;
            newMapping.ccNumber = ccNumber;
            newMapping.channel = channel;
            newMapping.parameterID = currentLearnParameter;
            newMapping.minValue = 0.0f;
            newMapping.maxValue = 1.0f;
            newMapping.enabled = true;

            ErrorHandler::safeExecute([&]() {
                addMidiMapping(newMapping);
            }, "MidiEngine add mapping");
            
            midiLearnActive = false;
            currentLearnParameter.clear();

            // Null-pointer safety: Check callback before calling
            if (onMidiLearnComplete) {
                try {
                    onMidiLearnComplete(newMapping);
                } catch (const std::exception& e) {
                    DBG("MidiEngine: Exception in MIDI learn callback - " + juce::String(e.what()));
                }
            }
            return;
        }

        // Process existing MIDI mappings with null-pointer safety
        if (midiMappings.isEmpty()) {
            return;
        }

        for (const auto& mapping : midiMappings) {
            // Null-pointer safety: Validate mapping data
            if (!mapping.enabled ||
                mapping.ccNumber != ccNumber ||
                (mapping.channel != 0 && mapping.channel != channel) ||
                mapping.parameterID.isEmpty()) {
                continue;
            }

            // Null-pointer safety: Safe value conversion with bounds checking
            float maxVelocity = static_cast<float>(INIConfig::LayoutConstants::midiEngineMaxMidiVelocity);
            if (maxVelocity <= 0.0f) {
                DBG("MidiEngine: Invalid max MIDI velocity configuration");
                continue;
            }

            float normalizedValue = juce::jlimit(0.0f, 1.0f, value / maxVelocity);
            float valueRange = mapping.maxValue - mapping.minValue;
            
            // Validate mapping range
            if (!std::isfinite(mapping.minValue) || !std::isfinite(mapping.maxValue) ||
                !std::isfinite(valueRange)) {
                DBG("MidiEngine: Invalid mapping range for parameter: " + mapping.parameterID);
                continue;
            }

            float mappedValue = mapping.minValue + (normalizedValue * valueRange);
            mappedValue = juce::jlimit(mapping.minValue, mapping.maxValue, mappedValue);

            // Null-pointer safety: Check callback before calling
            if (onMidiParameterChanged) {
                try {
                    onMidiParameterChanged(mapping.parameterID, mappedValue);
                } catch (const std::exception& e) {
                    DBG("MidiEngine: Exception in parameter change callback - " + juce::String(e.what()));
                }
            }
        }
    } catch (const std::exception& e) {
        DBG("MidiEngine: Exception in handleMidiCC - " + juce::String(e.what()));
    }
}

void MidiEngine::startMidiLearn(const juce::String& parameterID) {
    midiLearnActive = true;
    currentLearnParameter = parameterID;
}

void MidiEngine::cancelMidiLearn() {
    midiLearnActive = false;
    currentLearnParameter.clear();
}

void MidiEngine::addMidiMapping(const MidiMapping& mapping) {
    midiMappings.removeIf([&mapping](const MidiMapping& m) {
        return m.parameterID == mapping.parameterID;
    });

    midiMappings.add(mapping);
}

void MidiEngine::removeMidiMapping(const juce::String& parameterID) {
    midiMappings.removeIf([&parameterID](const MidiMapping& m) {
        return m.parameterID == parameterID;
    });
}

void MidiEngine::clearAllMidiMappings() {
    midiMappings.clear();
}

MidiEngine::MidiMapping MidiEngine::getMidiMapping(const juce::String& parameterID) const {
    for (const auto& mapping : midiMappings) {
        if (mapping.parameterID == parameterID) {
            return mapping;
        }
    }

    return MidiMapping();
}

void MidiEngine::sendMidiFeedback(const juce::String& parameterID, float value) {
    for (const auto& mapping : midiMappings) {
        if (mapping.parameterID == parameterID && mapping.sendFeedback) {
            float normalizedValue = (value - mapping.minValue) /
                                   (mapping.maxValue - mapping.minValue);
            int ccValue = static_cast<int>(normalizedValue * static_cast<float>(INIConfig::LayoutConstants::midiEngineMaxMidiVelocity));
            ccValue = juce::jlimit(0, INIConfig::LayoutConstants::midiEngineMaxMidiVelocity, ccValue);

            if (onMidiFeedbackRequired) {
                onMidiFeedbackRequired(mapping.channel, mapping.ccNumber, ccValue);
            }
        }
    }
}

void MidiEngine::saveStates(ComponentState& state) {
    state.sliderValues["tempo"] = tempo;
    state.toggleStates[100] = isPlaying;
    state.toggleStates[101] = sendMidiClock;
    state.currentPlayer = currentPlayerIndex;

    state.toggleStates[110] = liveRecording;
    state.toggleStates[111] = overdubMode;
    state.toggleStates[112] = loopRecordingMode;

    for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
        const auto& player = players[i];
        const juce::String playerPrefix = "player_" + juce::String(i) + "_";

        state.toggleStates[200 + i] = player.enabled;
        state.sliderValues[playerPrefix + "swing"] = player.swing;
        state.sliderValues[playerPrefix + "energy"] = player.energy;
        state.dropdownSelections[playerPrefix + "pattern"] = player.selectedPattern;
        state.dropdownSelections[playerPrefix + "output_channel"] = player.outputChannel;
    }

    state.midiMappings.clear();
    for (const auto& mapping : midiMappings) {
        MidiMappingState mappingState;
        mappingState.ccNumber = mapping.ccNumber;
        mappingState.channel = mapping.channel;
        mappingState.parameterID = mapping.parameterID;
        mappingState.minValue = mapping.minValue;
        mappingState.maxValue = mapping.maxValue;
        mappingState.enabled = mapping.enabled;
        state.midiMappings.add(mappingState);
    }
}

void MidiEngine::loadStates(const ComponentState& state) {
    if (state.sliderValues.count("tempo")) {
        tempo = INIConfig::clampTempo(state.sliderValues.at("tempo"));
    }

    if (state.toggleStates.count(100)) {
        isPlaying = state.toggleStates.at(100);
    }

    if (state.toggleStates.count(101)) {
        sendMidiClock = state.toggleStates.at(101);
    }

    if (state.toggleStates.count(110)) {
        liveRecording = state.toggleStates.at(110);
    }
    if (state.toggleStates.count(111)) {
        overdubMode = state.toggleStates.at(111);
    }
    if (state.toggleStates.count(112)) {
        loopRecordingMode = state.toggleStates.at(112);
    }

    currentPlayerIndex = INIConfig::clampPlayerIndex(state.currentPlayer);

    for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
        auto& player = players[i];
        const juce::String playerPrefix = "player_" + juce::String(i) + "_";

        if (state.toggleStates.count(200 + i)) {
            player.enabled = state.toggleStates.at(200 + i);
        }

        if (state.sliderValues.count(playerPrefix + "swing")) {
            player.swing = INIConfig::clampSwing(state.sliderValues.at(playerPrefix + "swing"));
        }

        if (state.sliderValues.count(playerPrefix + "energy")) {
            player.energy = INIConfig::clampEnergy(state.sliderValues.at(playerPrefix + "energy"));
        }

        if (state.dropdownSelections.count(playerPrefix + "pattern")) {
            player.selectedPattern = state.dropdownSelections.at(playerPrefix + "pattern");
        }

        if (state.dropdownSelections.count(playerPrefix + "output_channel")) {
            player.outputChannel = state.dropdownSelections.at(playerPrefix + "output_channel");
        }
    }

    clearAllMidiMappings();
    for (const auto& mappingState : state.midiMappings) {
        MidiMapping mapping;
        mapping.ccNumber = mappingState.ccNumber;
        mapping.channel = mappingState.channel;
        mapping.parameterID = mappingState.parameterID;
        mapping.minValue = mappingState.minValue;
        mapping.maxValue = mappingState.maxValue;
        mapping.enabled = mappingState.enabled;
        addMidiMapping(mapping);
    }
}

void MidiEngine::loadFromXml(const juce::XmlElement* xml) {
    if (xml == nullptr) return;

    if (xml->hasAttribute("tempo")) {
        tempo = INIConfig::clampTempo(static_cast<float>(xml->getDoubleAttribute("tempo", INIConfig::Defaults::DEFAULT_TEMPO)));
    }

    if (xml->hasAttribute("playing")) {
        isPlaying = xml->getBoolAttribute("playing", false);
    }

    if (xml->hasAttribute("sendMidiClock")) {
        sendMidiClock = xml->getBoolAttribute("sendMidiClock", false);
    }

    if (xml->hasAttribute("currentPlayer")) {
        currentPlayerIndex = INIConfig::clampPlayerIndex(xml->getIntAttribute("currentPlayer", 0));
    }

    for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
        if (auto* playerXml = xml->getChildByName("Player" + juce::String(i))) {
            auto& player = players[i];
            player.enabled = playerXml->getBoolAttribute("enabled", true);
            player.swing = INIConfig::clampSwing(static_cast<float>(playerXml->getDoubleAttribute("swing", INIConfig::Defaults::SWING)));
            player.energy = INIConfig::clampEnergy(static_cast<float>(playerXml->getDoubleAttribute("energy", INIConfig::Defaults::ENERGY)));
            player.selectedPattern = playerXml->getIntAttribute("selectedPattern", 0);
            player.outputChannel = playerXml->getIntAttribute("outputChannel", i + 1);
        }
    }

    if (auto* midiData = xml->getChildByName("MidiMappings")) {
        clearAllMidiMappings();

        for (auto* mappingXml : midiData->getChildIterator()) {
            if (mappingXml && mappingXml->hasTagName("Mapping")) {
                MidiMapping mapping;
                mapping.ccNumber = mappingXml->getIntAttribute("cc", -1);
                mapping.channel = mappingXml->getIntAttribute("channel", 0);
                mapping.parameterID = mappingXml->getStringAttribute("parameter");
                mapping.minValue = static_cast<float>(mappingXml->getDoubleAttribute("min", 0.0));
                mapping.maxValue = static_cast<float>(mappingXml->getDoubleAttribute("max", 1.0));
                mapping.enabled = mappingXml->getBoolAttribute("enabled", true);
                mapping.sendFeedback = mappingXml->getBoolAttribute("feedback", false);

                if (mapping.ccNumber >= 0 && mapping.parameterID.isNotEmpty()) {
                    addMidiMapping(mapping);
                }
            }
        }
    }
}

std::unique_ptr<juce::XmlElement> MidiEngine::saveToXml() const {
    auto xml = std::make_unique<juce::XmlElement>("MidiEngine");

    xml->setAttribute("tempo", tempo);
    xml->setAttribute("playing", isPlaying);
    xml->setAttribute("sendMidiClock", sendMidiClock);
    xml->setAttribute("currentPlayer", currentPlayerIndex);

    for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
        const auto& player = players[i];
        auto playerXml = xml->createNewChildElement("Player" + juce::String(i));
        playerXml->setAttribute("enabled", player.enabled);
        playerXml->setAttribute("swing", player.swing);
        playerXml->setAttribute("energy", player.energy);
        playerXml->setAttribute("selectedPattern", player.selectedPattern);
        playerXml->setAttribute("outputChannel", player.outputChannel);
    }

    auto midiData = xml->createNewChildElement("MidiMappings");
    for (const auto& mapping : midiMappings) {
        auto mappingXml = midiData->createNewChildElement("Mapping");
        mappingXml->setAttribute("cc", mapping.ccNumber);
        mappingXml->setAttribute("channel", mapping.channel);
        mappingXml->setAttribute("parameter", mapping.parameterID);
        mappingXml->setAttribute("min", mapping.minValue);
        mappingXml->setAttribute("max", mapping.maxValue);
        mappingXml->setAttribute("enabled", mapping.enabled);
        mappingXml->setAttribute("feedback", mapping.sendFeedback);
    }

    return xml;
}

void MidiEngine::loadControllerPreset(const MidiControllerPreset& preset) {
    clearAllMidiMappings();

    for (const auto& mapping : preset.mappings) {
        addMidiMapping(mapping);
    }

    currentControllerPreset = preset.name;
}

void MidiEngine::saveControllerPreset(const juce::String& name) {
    MidiControllerPreset preset;
    preset.name = name;
    preset.manufacturer = "User";
    preset.hasLEDFeedback = false;
    preset.mappings = midiMappings;

    controllerPresets.add(preset);
}

juce::StringArray MidiEngine::getAvailableControllerPresets() const {
    juce::StringArray names;

    names.add("Akai APC40");
    names.add("Novation Launch Control");
    names.add("Korg nanoKONTROL2");
    names.add("Arturia BeatStep");

    for (const auto& preset : controllerPresets) {
        names.add(preset.name);
    }

    return names;
}

void MidiEngine::enableMidiLearnMode(bool enable) {
    if (!enable) {
        cancelMidiLearn();
    }
    midiLearnActive = enable;
}

bool MidiEngine::isMidiLearnActive() const {
    return midiLearnActive;
}

juce::String MidiEngine::getCurrentLearnParameter() const {
    return currentLearnParameter;
}

const juce::Array<MidiEngine::MidiMapping>& MidiEngine::getAllMidiMappings() const {
    return midiMappings;
}

void MidiEngine::setMidiMappingEnabled(const juce::String& parameterID, bool enabled) {
    for (auto& mapping : midiMappings) {
        if (mapping.parameterID == parameterID) {
            mapping.enabled = enabled;
            break;
        }
    }
}

bool MidiEngine::isMidiMappingEnabled(const juce::String& parameterID) const {
    for (const auto& mapping : midiMappings) {
        if (mapping.parameterID == parameterID) {
            return mapping.enabled;
        }
    }
    return false;
}

void MidiEngine::setPlayerEnabled(int playerIndex, bool enabled) {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return;

    players[playerIndex].enabled = enabled;
}

bool MidiEngine::isPlayerEnabled(int playerIndex) const {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return false;

    return players[playerIndex].enabled;
}

void MidiEngine::setPlayerOutputChannel(int playerIndex, int channel) {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return;
    if (!INIConfig::isValidMidiChannel(channel)) return;

    players[playerIndex].outputChannel = channel;
}

int MidiEngine::getPlayerOutputChannel(int playerIndex) const {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return 1;

    return players[playerIndex].outputChannel;
}

void MidiEngine::syncToHost(double hostBpm, double hostPosition) {
    if (hostBpm > 0) {
        hostTempo = hostBpm;
        if (syncToHostTempo) {
            tempo = static_cast<float>(hostBpm);
        }
    }

    if (syncToHostPosition && hostPosition >= 0) {
        for (auto& player : players) {
            player.playbackPosition = std::fmod(hostPosition, INIConfig::Defaults::BEATS_PER_BAR);
        }
    }
}

void MidiEngine::setSyncToHost(bool syncTempo, bool syncPosition) {
    syncToHostTempo = syncTempo;
    syncToHostPosition = syncPosition;
}

bool MidiEngine::isSyncedToHostTempo() const {
    return syncToHostTempo;
}

bool MidiEngine::isSyncedToHostPosition() const {
    return syncToHostPosition;
}

void MidiEngine::setQuantize(int quantizeValue) {
    if (INIConfig::isValidQuantizeValue(quantizeValue)) {
        quantization = quantizeValue;
    }
}

int MidiEngine::getQuantize() const {
    return quantization;
}

void MidiEngine::panic() {
    for (int channel = 1; channel <= INIConfig::LayoutConstants::midiEngineMaxMidiChannels; ++channel) {
        auto allNotesOff = juce::MidiMessage::allNotesOff(channel);
        if (onPanicRequired) {
            onPanicRequired(allNotesOff);
        }
    }

    for (auto& player : players) {
        player.playbackPosition = 0.0;
        player.fillActive = false;
        player.lastProcessTime = 0;
    }
}

void MidiEngine::setMetronomeEnabled(bool enabled) {
    metronomeEnabled = enabled;
}

bool MidiEngine::isMetronomeEnabled() const {
    return metronomeEnabled;
}

void MidiEngine::setMetronomeVolume(float volume) {
    metronomeVolume = INIConfig::clampVolume(volume);
}

float MidiEngine::getMetronomeVolume() const {
    return metronomeVolume;
}

void MidiEngine::generateMetronome(juce::MidiBuffer& midiMessages) {
    if (!metronomeEnabled || !isPlaying) return;

    const double beatPosition = std::fmod(players[0].playbackPosition, 1.0);

    if (beatPosition < INIConfig::Defaults::BEAT_THRESHOLD) {
        int note = (std::fmod(players[0].playbackPosition, INIConfig::Defaults::BEATS_PER_BAR) < INIConfig::Defaults::BEAT_THRESHOLD) ? INIConfig::LayoutConstants::midiEngineMetronomeHighNote : INIConfig::LayoutConstants::midiEngineMetronomeLowNote;
        int velocity = static_cast<int>(INIConfig::LayoutConstants::midiEngineMaxMidiVelocity * metronomeVolume);

        auto noteOn = juce::MidiMessage::noteOn(INIConfig::LayoutConstants::midiEngineMetronomeChannel, note, (juce::uint8)velocity);
        midiMessages.addEvent(noteOn, 0);

        auto noteOff = juce::MidiMessage::noteOff(INIConfig::LayoutConstants::midiEngineMetronomeChannel, note);
        midiMessages.addEvent(noteOff, INIConfig::LayoutConstants::midiEngineMetronomeNoteOffDelay);
    }
}

void MidiEngine::setRecording(bool shouldRecord) {
    isRecording = shouldRecord;

    if (shouldRecord) {
        recordBuffer.clear();
        recordStartTime = juce::Time::getMillisecondCounterHiRes();
    }
}

bool MidiEngine::isCurrentlyRecording() const {
    return isRecording;
}

void MidiEngine::clearRecordBuffer() {
    recordBuffer.clear();
}

const juce::MidiMessageSequence& MidiEngine::getRecordedSequence() const {
    return recordBuffer;
}

void MidiEngine::recordMidiMessage(const juce::MidiMessage& message) {
    if (!isRecording) return;

    double timestamp = juce::Time::getMillisecondCounterHiRes() - recordStartTime;
    recordBuffer.addEvent(message, timestamp);
}

void MidiEngine::exportRecording(const juce::File& file) {
    if (recordBuffer.getNumEvents() == 0) return;

    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(INIConfig::Defaults::MIDI_TICKS_PER_QUARTER_NOTE);

    const juce::MidiMessageSequence* sequence = &recordBuffer;
    midiFile.addTrack(*sequence);

    juce::FileOutputStream stream(file);
    if (stream.openedOk()) {
        midiFile.writeTo(stream);
    }
}

void MidiEngine::applyHumanization(int playerIndex, float amount) {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return;

    players[playerIndex].humanizationAmount = juce::jlimit(0.0f, 1.0f, amount);
}

float MidiEngine::getHumanization(int playerIndex) const {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return 0.0f;

    return players[playerIndex].humanizationAmount;
}

juce::MidiMessage MidiEngine::humanizeMessage(const juce::MidiMessage& message, float amount) {
    if (amount <= 0.0f) return message;

    juce::Random random;

    if (message.isNoteOn()) {
        int velocity = message.getVelocity();
        int variation = static_cast<int>(INIConfig::Defaults::HUMANIZE_VELOCITY_RANGE * amount);
        velocity += random.nextInt(variation * 2 + 1) - variation;
        velocity = juce::jlimit(1, INIConfig::LayoutConstants::midiEngineMaxMidiVelocity, velocity);

        return juce::MidiMessage::noteOn(message.getChannel(),
                                        message.getNoteNumber(),
                                        (juce::uint8)velocity);
    }

    return message;
}

void MidiEngine::setLoopEnabled(bool enabled) {
    loopEnabled = enabled;
}

bool MidiEngine::isLoopEnabled() const {
    return loopEnabled;
}

void MidiEngine::setLoopStartBar(int bar) {
    loopStartBar = juce::jmax(0, bar);
}

void MidiEngine::setLoopEndBar(int bar) {
    loopEndBar = juce::jmax(loopStartBar + 1, bar);
}

int MidiEngine::getLoopStartBar() const {
    return loopStartBar;
}

int MidiEngine::getLoopEndBar() const {
    return loopEndBar;
}

void MidiEngine::handleLoop() {
    if (!loopEnabled) return;

    const double currentBar = players[currentPlayerIndex].playbackPosition / INIConfig::Defaults::BEATS_PER_BAR;

    if (currentBar >= loopEndBar) {
        for (auto& player : players) {
            player.playbackPosition = loopStartBar * INIConfig::Defaults::BEATS_PER_BAR;
        }
    }
}

void MidiEngine::setSendMidiClock(bool send) {
    sendMidiClock = send;
}

bool MidiEngine::isSendingMidiClock() const {
    return sendMidiClock;
}

void MidiEngine::setReceiveMidiClock(bool receive) {
    receiveMidiClock = receive;
}

bool MidiEngine::isReceivingMidiClock() const {
    return receiveMidiClock;
}

void MidiEngine::handleMidiClock(const juce::MidiMessage& message) {
    if (!receiveMidiClock) return;

    if (message.isMidiClock()) {
        const double ticksPerBeat = INIConfig::LayoutConstants::midiEngineMidiClockPulsesPerBeat;
        const double beatsPerTick = 1.0 / ticksPerBeat;

        for (auto& player : players) {
            player.playbackPosition += beatsPerTick;
        }
    } else if (message.isMidiStart()) {
        startPlayback();
    } else if (message.isMidiStop()) {
        stopPlayback();
    } else if (message.isMidiContinue()) {
        isPlaying = true;
    }
}

float MidiEngine::getCurrentBeat() const {
    if (!isPlaying) return 0.0f;

    return static_cast<float>(players[currentPlayerIndex].playbackPosition);
}

int MidiEngine::getCurrentBar() const {
    if (!isPlaying) return 0;

    return static_cast<int>(players[currentPlayerIndex].playbackPosition / INIConfig::Defaults::BEATS_PER_BAR);
}

void MidiEngine::schedulePatternChange(int playerIndex, int patternIndex, int barNumber) {
    if (!INIConfig::isValidPlayerIndex(playerIndex) ||
        !INIConfig::isValidButtonIndex(patternIndex)) {
        return;
    }

    PatternChange change;
    change.playerIndex = playerIndex;
    change.patternIndex = patternIndex;
    change.triggerBar = barNumber;

    pendingPatternChanges.add(change);
}

void MidiEngine::processPendingPatternChanges() {
    const int currentBar = getCurrentBar();

    for (int i = pendingPatternChanges.size() - 1; i >= 0; --i) {
        const auto& change = pendingPatternChanges[i];

        if (currentBar >= change.triggerBar) {
            selectPattern(change.playerIndex, change.patternIndex);
            pendingPatternChanges.remove(i);
        }
    }
}

void MidiEngine::clearPendingPatternChanges(int playerIndex) {
    if (playerIndex < 0) {
        pendingPatternChanges.clear();
    } else {
        pendingPatternChanges.removeIf([playerIndex](const PatternChange& change) {
            return change.playerIndex == playerIndex;
        });
    }
}

void MidiEngine::setCountIn(int bars) {
    countInBars = juce::jlimit(0, INIConfig::Defaults::MAX_COUNT_IN_BARS, bars);
}

int MidiEngine::getCountIn() const {
    return countInBars;
}

void MidiEngine::startWithCountIn() {
    if (countInBars > 0) {
        countingIn = true;
        countInPosition = 0;
        startPlayback();
    } else {
        startPlayback();
    }
}

void MidiEngine::processCountIn(juce::MidiBuffer& midiMessages) {
    if (!countingIn) return;

    const double beatsPerBar = INIConfig::Defaults::BEATS_PER_BAR;
    const double totalCountInBeats = countInBars * beatsPerBar;

    if (countInPosition >= totalCountInBeats) {
        countingIn = false;
        countInPosition = 0;

        for (auto& player : players) {
            player.playbackPosition = 0.0;
        }
    } else {
        generateMetronome(midiMessages);
        countInPosition += (tempo / INIConfig::Defaults::SECONDS_PER_MINUTE) / INIConfig::Defaults::MS_PER_SECOND;
    }
}

bool MidiEngine::isCountingIn() const {
    return countingIn;
}

float MidiEngine::getCountInProgress() const {
    if (!countingIn || countInBars <= 0) return 0.0f;

    const double totalBeats = countInBars * INIConfig::Defaults::BEATS_PER_BAR;
    return static_cast<float>(countInPosition / totalBeats);
}

void MidiEngine::setVelocityCurve(int playerIndex, VelocityCurve curve) {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return;

    players[playerIndex].velocityCurve = curve;
}

MidiEngine::VelocityCurve MidiEngine::getVelocityCurve(int playerIndex) const {
    if (!INIConfig::isValidPlayerIndex(playerIndex)) return VelocityCurve::Linear;

    return players[playerIndex].velocityCurve;
}

int MidiEngine::applyVelocityCurve(int velocity, VelocityCurve curve) {
    float normalized = velocity / static_cast<float>(INIConfig::LayoutConstants::midiEngineMaxMidiVelocity);
    float curved = normalized;

    switch (curve) {
        case VelocityCurve::Linear:
            curved = normalized;
            break;

        case VelocityCurve::Exponential:
            curved = normalized * normalized;
            break;

        case VelocityCurve::Logarithmic:
            curved = std::sqrt(normalized);
            break;

        case VelocityCurve::SCurve:
            curved = normalized * normalized * (INIConfig::Defaults::SCURVE_FACTOR - INIConfig::Defaults::SCURVE_MULTIPLIER * normalized);
            break;

        case VelocityCurve::Fixed:
            return INIConfig::Defaults::FIXED_VELOCITY;
    }

    return juce::jlimit(1, INIConfig::LayoutConstants::midiEngineMaxMidiVelocity, static_cast<int>(curved * static_cast<float>(INIConfig::LayoutConstants::midiEngineMaxMidiVelocity)));
}

void MidiEngine::freezePlayback() {
    playbackFrozen = true;
}

void MidiEngine::unfreezePlayback() {
    playbackFrozen = false;
}

bool MidiEngine::isPlaybackFrozen() const {
    return playbackFrozen;
}

juce::String MidiEngine::getEngineStatus() const {
    juce::String status;

    status << "Engine: " << (isPlaying ? "Playing" : "Stopped") << "\n";
    status << "Tempo: " << tempo << " BPM\n";
    status << "Current Player: " << (currentPlayerIndex + 1) << "\n";
    status << "Active Players: ";

    int activeCount = 0;
    for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
        if (players[i].enabled) {
            if (activeCount > 0) status << ", ";
            status << (i + 1);
            activeCount++;
        }
    }

    if (activeCount == 0) status << "None";

    status << "\n";
    status << "MIDI Learn: " << (midiLearnActive ? "Active" : "Inactive") << "\n";
    status << "MIDI Mappings: " << midiMappings.size() << "\n";
    status << "Sync to Host: " << (syncToHostTempo ? "Yes" : "No") << "\n";
    status << "Live Recording: " << (liveRecording ? "Active" : "Inactive") << "\n";
    status << "Loop Recording: " << (loopRecordingMode ? "Enabled" : "Disabled") << "\n";

    return status;
}

MidiEngine::EngineState MidiEngine::getCurrentEngineState() const {
    EngineState state;
    state.tempo = tempo;
    state.isPlaying = isPlaying;
    state.swingValue = players[currentPlayerIndex].swing;
    state.energyValue = players[currentPlayerIndex].energy;
    state.currentPosition = players[currentPlayerIndex].playbackPosition;
    state.isFillActive = players[currentPlayerIndex].fillActive;

    for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
        if (players[i].enabled) {
            if (players[i].currentPattern.getNumEvents() > 0) {
                EngineState::PatternInfo pattern;
                pattern.group = i;
                pattern.index = players[i].selectedPattern;
                pattern.name = "Pattern " + juce::String(players[i].selectedPattern + 1);
                pattern.midiFileName = players[i].selectedMidiGroup;
                state.patterns.add(pattern);
            }
        }
    }

    return state;
}

void MidiEngine::applyEngineState(const EngineState& state) {
    tempo = INIConfig::clampTempo(state.tempo);
    isPlaying = state.isPlaying;

    if (currentPlayerIndex >= 0 && currentPlayerIndex < INIConfig::Defaults::MAX_PLAYERS) {
        players[currentPlayerIndex].swing = INIConfig::clampSwing(state.swingValue);
        players[currentPlayerIndex].energy = INIConfig::clampEnergy(state.energyValue);
        players[currentPlayerIndex].playbackPosition = state.currentPosition;
        players[currentPlayerIndex].fillActive = state.isFillActive;
    }

    for (const auto& pattern : state.patterns) {
        if (pattern.group >= 0 && pattern.group < INIConfig::Defaults::MAX_PLAYERS) {
            players[pattern.group].selectedPattern = pattern.index;
            players[pattern.group].selectedMidiGroup = pattern.midiFileName;
        }
    }
}
