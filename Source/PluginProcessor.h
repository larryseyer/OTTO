#pragma once
#include <JuceHeader.h>
#include "MidiEngine.h"
#include "SFZEngine.h"
#include "PresetManager.h"
#include "Mixer.h"
#include "ComponentState.h"
#include "INIConfig.h"

class OTTOAudioProcessor : public juce::AudioProcessor,
                           public juce::AudioProcessorValueTreeState::Listener {
public:
    OTTOAudioProcessor();
    ~OTTOAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    #endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    MidiEngine& getMidiEngine() { return midiEngine; }
    SFZEngine& getSFZEngine() { return sfzEngine; }
    PresetManager& getPresetManager() { return presetManager; }
    Mixer& getMixer() { return mixer; }
    juce::AudioProcessorValueTreeState& getValueTreeState() { return parameters; }
    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    void refreshMidiDevices();
    juce::StringArray getAvailableMidiInputs() const;
    juce::StringArray getAvailableMidiOutputs() const;
    void setMidiInput(const juce::String& deviceName);
    void setMidiOutput(const juce::String& deviceName);
    juce::String getCurrentMidiInput() const { return currentMidiInput; }
    juce::String getCurrentMidiOutput() const { return currentMidiOutput; }

private:
    juce::AudioDeviceManager deviceManager;
    MidiEngine midiEngine;
    SFZEngine sfzEngine;
    Mixer mixer;
    juce::AudioProcessorValueTreeState parameters;
    PresetManager presetManager;

    juce::String currentMidiInput;
    juce::String currentMidiOutput;
    std::unique_ptr<juce::MidiInput> midiInput;
    std::unique_ptr<juce::MidiOutput> midiOutput;

    double sampleRate = static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE);

    static const juce::StringArray parameterIDs;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void initializeParameters();
    void updateParametersFromState(const ComponentState& state);
    void updateStateFromParameters(ComponentState& state);
    void handleMidiParameterChange(const juce::String& parameterID, float value);
    void setupMidiEngine();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OTTOAudioProcessor)
};
