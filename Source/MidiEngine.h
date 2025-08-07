#pragma once

#include <JuceHeader.h>
#include "ComponentState.h"
#include "INIConfig.h"

class MidiFileManager;

class MidiEngine {
public:
    enum class VelocityCurve {
        Linear = INIConfig::Defaults::ZERO_VALUE,
        Exponential,
        Logarithmic,
        SCurve,
        Fixed
    };

    struct EngineState {
        float tempo = INIConfig::Defaults::DEFAULT_TEMPO;
        bool isPlaying = false;
        float swingValue = INIConfig::Defaults::SWING;
        float energyValue = INIConfig::Defaults::ENERGY;
        double currentPosition = INIConfig::MIDI::DEFAULT_POSITION;
        bool isFillActive = false;

        struct PatternInfo {
            int group = INIConfig::Defaults::ZERO_VALUE;
            int index = INIConfig::Defaults::ZERO_VALUE;
            juce::String name;
            juce::String midiFileName;
        };

        juce::Array<PatternInfo> patterns;
    };

    struct MidiMapping {
        int ccNumber = INIConfig::MIDI::DEFAULT_CC_NUMBER;
        int channel = INIConfig::Validation::MIN_MIDI_CHANNEL;
        juce::String parameterID;
        float minValue = INIConfig::Validation::MIN_VOLUME;
        float maxValue = INIConfig::Validation::MAX_VOLUME;
        bool isLearning = false;
        bool enabled = true;
        bool sendFeedback = false;
        int feedbackType = INIConfig::MIDI::DEFAULT_FEEDBACK_TYPE;
    };

    struct MidiControllerPreset {
        juce::String name;
        juce::String manufacturer;
        juce::Array<MidiMapping> mappings;
        bool hasLEDFeedback = false;
        int sysexDeviceID = INIConfig::MIDI::DEFAULT_SYSEX_DEVICE_ID;
    };

    struct Scene {
        juce::String name;
        struct ClipState {
            bool active = false;
            int patternIndex = INIConfig::MIDI::INACTIVE_PATTERN;
            juce::String midiFileName;
            float volume = INIConfig::Defaults::VOLUME;
            bool muted = false;
        };
        ClipState clips[INIConfig::Defaults::MAX_PLAYERS];
        float tempo = INIConfig::Defaults::DEFAULT_TEMPO;
    };

    MidiEngine();
    ~MidiEngine();

    void prepare(double sampleRate);
    void process(juce::MidiBuffer& midiMessages);
    void startPlayback();
    void stopPlayback();
    bool isPlaybackActive() const { return isPlaying; }

    void setTempo(float newTempo);
    float getTempo() const { return tempo; }
    void syncToHost(double hostBpm, double hostPosition);
    void setSyncToHost(bool syncTempo, bool syncPosition);
    bool isSyncedToHostTempo() const;
    bool isSyncedToHostPosition() const;

    void setCurrentPlayer(int playerIndex);
    int getCurrentPlayer() const { return currentPlayerIndex; }
    void setPlayerEnabled(int playerIndex, bool enabled);
    bool isPlayerEnabled(int playerIndex) const;
    void setPlayerOutputChannel(int playerIndex, int channel);
    int getPlayerOutputChannel(int playerIndex) const;

    void selectPattern(int playerIndex, int patternIndex);
    void triggerFill(int playerIndex);
    void playMidiFile(int playerIndex, const juce::String& filename);
    void schedulePatternChange(int playerIndex, int patternIndex, int barNumber);
    void clearPendingPatternChanges(int playerIndex = INIConfig::MIDI::ALL_PLAYERS);

    void setSwing(int playerIndex, float swing);
    void setEnergy(int playerIndex, float energy);
    float getSwing(int playerIndex) const;
    float getEnergy(int playerIndex) const;
    void setQuantize(int quantizeValue);
    int getQuantize() const;
    void applyHumanization(int playerIndex, float amount);
    float getHumanization(int playerIndex) const;
    void setVelocityCurve(int playerIndex, VelocityCurve curve);
    VelocityCurve getVelocityCurve(int playerIndex) const;

    void triggerScene(int sceneIndex);
    void triggerClip(int sceneIndex, int playerIndex);
    void queueSceneChange(int sceneIndex, int quantization = INIConfig::Defaults::ZERO_VALUE);
    void queueClipChange(int playerIndex, int patternIndex, int quantization = INIConfig::Defaults::ZERO_VALUE);
    void startLiveRecording(bool overdub = false);
    void stopLiveRecording();
    bool isLiveRecording() const { return liveRecording; }
    void setOverdubMode(bool enabled) { overdubMode = enabled; }
    bool isOverdubMode() const { return overdubMode; }
    void setLoopRecordingMode(bool enabled);
    bool isLoopRecordingMode() const { return loopRecordingMode; }
    void tapTempo();
    void resetTapTempo();
    float getTapTempoAveraging() const;

    void saveScene(int sceneIndex, const juce::String& name = "");
    Scene getScene(int sceneIndex) const;
    void loadScene(int sceneIndex);
    void clearScene(int sceneIndex);
    int getActiveSceneIndex() const { return activeSceneIndex; }

    struct QueuedChange {
        enum Type { Pattern, Scene, Clip };
        Type type;
        int targetIndex;
        int playerIndex;
        int quantizationBars;
        double triggerTime;
    };

    const juce::Array<QueuedChange>& getQueuedChanges() const { return queuedChanges; }

    void startMidiLearn(const juce::String& parameterID);
    void cancelMidiLearn();
    void enableMidiLearnMode(bool enable);
    bool isMidiLearnActive() const;
    juce::String getCurrentLearnParameter() const;

    void addMidiMapping(const MidiMapping& mapping);
    void removeMidiMapping(const juce::String& parameterID);
    void clearAllMidiMappings();
    MidiMapping getMidiMapping(const juce::String& parameterID) const;
    const juce::Array<MidiMapping>& getAllMidiMappings() const;
    void setMidiMappingEnabled(const juce::String& parameterID, bool enabled);
    bool isMidiMappingEnabled(const juce::String& parameterID) const;

    void loadControllerPreset(const MidiControllerPreset& preset);
    void saveControllerPreset(const juce::String& name);
    juce::StringArray getAvailableControllerPresets() const;

    void sendMidiFeedback(const juce::String& parameterID, float value);

    void setSendMidiClock(bool send);
    bool isSendingMidiClock() const;
    void setReceiveMidiClock(bool receive);
    bool isReceivingMidiClock() const;
    void handleMidiClock(const juce::MidiMessage& message);

    void setLoopEnabled(bool enabled);
    bool isLoopEnabled() const;
    void setLoopStartBar(int bar);
    void setLoopEndBar(int bar);
    int getLoopStartBar() const;
    int getLoopEndBar() const;
    void setCountIn(int bars);
    int getCountIn() const;
    void startWithCountIn();
    bool isCountingIn() const;
    float getCountInProgress() const;

    void setMetronomeEnabled(bool enabled);
    bool isMetronomeEnabled() const;
    void setMetronomeVolume(float volume);
    float getMetronomeVolume() const;

    void setRecording(bool shouldRecord);
    bool isCurrentlyRecording() const;
    void clearRecordBuffer();
    const juce::MidiMessageSequence& getRecordedSequence() const;
    void exportRecording(const juce::File& file);

    float getCurrentBeat() const;
    int getCurrentBar() const;
    void panic();
    void freezePlayback();
    void unfreezePlayback();
    bool isPlaybackFrozen() const;
    juce::String getEngineStatus() const;

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);
    void loadFromXml(const juce::XmlElement* xml);
    std::unique_ptr<juce::XmlElement> saveToXml() const;

    EngineState getCurrentEngineState() const;
    void applyEngineState(const EngineState& state);

    std::function<void(const juce::String&, float)> onMidiParameterChanged;
    std::function<void(const MidiMapping&)> onMidiLearnComplete;
    std::function<void(int channel, int cc, int value)> onMidiFeedbackRequired;
    std::function<void(const juce::MidiMessage&)> onPanicRequired;

    void setMidiFileManager(MidiFileManager* manager) { midiFileManager = manager; }

private:
    struct PlayerState {
        juce::MidiMessageSequence currentPattern;
        juce::MidiMessageSequence recordedPattern;
        juce::String selectedMidiGroup;
        float swingValue = INIConfig::Defaults::SWING;
        float energyValue = INIConfig::Defaults::ENERGY;
        bool enabled = INIConfig::Defaults::DEFAULT_PLAYER_ENABLED;
        int selectedPattern = INIConfig::Defaults::ZERO_VALUE;
        int outputChannel = INIConfig::Validation::MIN_MIDI_CHANNEL;
        double playbackPosition = INIConfig::MIDI::DEFAULT_POSITION;
        double lastProcessTime = INIConfig::MIDI::DEFAULT_LAST_TIME;
        bool fillActive = false;
        int playerIndex = INIConfig::Defaults::ZERO_VALUE;
        float swing = INIConfig::Defaults::SWING;
        float energy = INIConfig::Defaults::ENERGY;
        VelocityCurve velocityCurve = VelocityCurve::Linear;
        float humanizationAmount = INIConfig::Validation::MIN_VOLUME;
        bool hasQueuedChange = false;
        int queuedPattern = INIConfig::MIDI::INACTIVE_PATTERN;
    };

    struct PatternChange {
        int playerIndex;
        int patternIndex;
        int triggerBar;
    };

    PlayerState players[INIConfig::Defaults::MAX_PLAYERS];
    int currentPlayerIndex = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;
    bool isPlaying = INIConfig::Defaults::DEFAULT_PLAY_STATE;
    float tempo = INIConfig::Defaults::DEFAULT_TEMPO;
    double sampleRate = static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE);
    double lastProcessTime = INIConfig::MIDI::DEFAULT_LAST_TIME;
    double lastClockTime = INIConfig::MIDI::DEFAULT_LAST_TIME;
    bool playbackFrozen = false;

    bool syncToHostTempo = false;
    bool syncToHostPosition = false;
    double hostTempo = INIConfig::MIDI::DEFAULT_POSITION;
    bool sendMidiClock = INIConfig::Defaults::DEFAULT_MIDI_CLOCK_OUT;
    bool receiveMidiClock = INIConfig::Defaults::DEFAULT_MIDI_CLOCK_IN;

    bool loopEnabled = false;
    int loopStartBar = INIConfig::Defaults::ZERO_VALUE;
    int loopEndBar = static_cast<int>(INIConfig::Defaults::BEATS_PER_BAR);

    int quantization = INIConfig::Defaults::DEFAULT_QUANTIZE_VALUE;

    int countInBars = INIConfig::Defaults::DEFAULT_COUNT_IN_BARS;
    double countInPosition = INIConfig::MIDI::DEFAULT_POSITION;
    bool countingIn = false;

    bool metronomeEnabled = INIConfig::Defaults::DEFAULT_METRONOME_ENABLED;
    float metronomeVolume = INIConfig::Defaults::DEFAULT_METRONOME_VOLUME;

    bool isRecording = false;
    juce::MidiMessageSequence recordBuffer;
    double recordStartTime = INIConfig::MIDI::DEFAULT_LAST_TIME;

    bool liveRecording = false;
    bool overdubMode = false;
    bool loopRecordingMode = false;
    int loopRecordingBars = static_cast<int>(INIConfig::Defaults::BEATS_PER_BAR);
    double loopRecordingStartTime = INIConfig::MIDI::DEFAULT_LAST_TIME;

    juce::Array<Scene> scenes;
    int activeSceneIndex = INIConfig::MIDI::INACTIVE_SCENE;
    juce::Array<QueuedChange> queuedChanges;

    juce::Array<double> tapTimes;
    double lastTapTime = INIConfig::MIDI::DEFAULT_LAST_TIME;
    int tapTempoAverageCount = INIConfig::Defaults::TAP_TEMPO_AVERAGE_COUNT;

    juce::Array<PatternChange> pendingPatternChanges;

    bool midiLearnActive = false;
    juce::String currentLearnParameter;
    juce::Array<MidiMapping> midiMappings;
    juce::Array<MidiControllerPreset> controllerPresets;
    juce::String currentControllerPreset;

    MidiFileManager* midiFileManager = nullptr;

    void processPlayer(int playerIndex, juce::MidiBuffer& midiMessages, double deltaTime);
    void generatePatternNotes(int playerIndex, juce::MidiBuffer& midiMessages);
    void generateMidiClock(juce::MidiBuffer& midiMessages);
    void generateMetronome(juce::MidiBuffer& midiMessages);
    void processMidiInput(const juce::MidiBuffer& midiMessages);
    void handleMidiCC(int channel, int ccNumber, int value);
    void processPendingPatternChanges();
    void processCountIn(juce::MidiBuffer& midiMessages);
    void handleLoop();
    void recordMidiMessage(const juce::MidiMessage& message);
    juce::MidiMessage humanizeMessage(const juce::MidiMessage& message, float amount);
    int applyVelocityCurve(int velocity, VelocityCurve curve);

    void processQueuedChanges();
    void processLiveRecording(const juce::MidiBuffer& midiMessages);
    void processLoopRecording();
    void initializeScenes();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiEngine)
};
