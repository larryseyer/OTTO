#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "INIConfig.h"

const juce::StringArray OTTOAudioProcessor::parameterIDs = {
    "masterVolume",
    "tempo",
    "swing",
    "energy",
    "player1Volume", "player2Volume", "player3Volume", "player4Volume",
    "player5Volume", "player6Volume", "player7Volume", "player8Volume",
    "player1Pan", "player2Pan", "player3Pan", "player4Pan",
    "player5Pan", "player6Pan", "player7Pan", "player8Pan"
};

OTTOAudioProcessor::OTTOAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    parameters(*this, nullptr, juce::Identifier("OTTOParameters"), createParameterLayout()),
    presetManager(midiEngine, mixer)
{
    initializeParameters();
    setupMidiEngine();
    midiEngine.setTempo(INIConfig::Defaults::DEFAULT_TEMPO);
    mixer.setMasterVolume(INIConfig::Defaults::VOLUME);
    deviceManager.initialiseWithDefaultDevices(2, 2);
    refreshMidiDevices();

    #ifdef DEBUG
        juce::UnitTestRunner testRunner;
        testRunner.setAssertOnFailure(false);
        testRunner.setPassesAreLogged(true);
        testRunner.runAllTests();

        int totalPasses = 0;
        int totalFailures = 0;

        for (int i = 0; i < testRunner.getNumResults(); ++i) {
            auto* result = testRunner.getResult(i);
            if (result) {
                totalPasses += result->passes;
                totalFailures += result->failures;
                if (result->failures > 0) {

                }
            }
        }

    #endif
}
OTTOAudioProcessor::~OTTOAudioProcessor() {
    if (midiInput) {
        midiInput->stop();
    }
}

void OTTOAudioProcessor::setupMidiEngine() {
    midiEngine.onMidiParameterChanged = [this](const juce::String& parameterID, float value) {
        handleMidiParameterChange(parameterID, value);
    };
}

void OTTOAudioProcessor::handleMidiParameterChange(const juce::String& parameterID, float value) {
    // Null-pointer safety: Validate parameter ID
    if (parameterID.isEmpty()) {
        DBG("AudioProcessor: Empty parameter ID in MIDI parameter change");
        return;
    }

    // Null-pointer safety: Validate parameter value range
    if (!std::isfinite(value)) {
        DBG("AudioProcessor: Invalid parameter value: " + juce::String(value) + " for parameter: " + parameterID);
        return;
    }

    // Null-pointer safety: Check parameter exists before use
    if (auto* param = parameters.getParameter(parameterID)) {
        try {
            const juce::MessageManagerLock mmLock;
            // Null-pointer safety: Verify param is still valid after lock
            if (param != nullptr) {
                float normalizedValue = param->convertTo0to1(value);
                // Additional validation for normalized value
                if (std::isfinite(normalizedValue) && normalizedValue >= 0.0f && normalizedValue <= 1.0f) {
                    param->setValueNotifyingHost(normalizedValue);
                } else {
                    DBG("AudioProcessor: Invalid normalized value for parameter: " + parameterID);
                }
            }
        } catch (const std::exception& e) {
            DBG("AudioProcessor: Exception in parameter change - " + juce::String(e.what()));
        }
    } else {
        DBG("AudioProcessor: Parameter not found: " + parameterID);
    }

    // Handle special playback state parameter
    if (parameterID == "playState") {
        try {
            if (value > 0.5f) {
                midiEngine.startPlayback();
            } else {
                midiEngine.stopPlayback();
            }
        } catch (const std::exception& e) {
            DBG("AudioProcessor: Exception in playback state change - " + juce::String(e.what()));
        }
    }
}

void OTTOAudioProcessor::refreshMidiDevices() {
    auto midiInputs = juce::MidiInput::getAvailableDevices();
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();

    bool inputStillAvailable = false;
    bool outputStillAvailable = false;

    for (const auto& device : midiInputs) {
        if (device.name == currentMidiInput) {
            inputStillAvailable = true;
            break;
        }
    }

    for (const auto& device : midiOutputs) {
        if (device.name == currentMidiOutput) {
            outputStillAvailable = true;
            break;
        }
    }

    if (!inputStillAvailable) {
        currentMidiInput.clear();
        midiInput.reset();
    }

    if (!outputStillAvailable) {
        currentMidiOutput.clear();
        midiOutput.reset();
    }
}

juce::StringArray OTTOAudioProcessor::getAvailableMidiInputs() const {
    juce::StringArray devices;
    for (const auto& device : juce::MidiInput::getAvailableDevices()) {
        devices.add(device.name);
    }
    return devices;
}

juce::StringArray OTTOAudioProcessor::getAvailableMidiOutputs() const {
    juce::StringArray devices;
    for (const auto& device : juce::MidiOutput::getAvailableDevices()) {
        devices.add(device.name);
    }
    return devices;
}

void OTTOAudioProcessor::setMidiInput(const juce::String& deviceName) {
    if (deviceName == currentMidiInput) return;

    // Null-pointer safety: Safely close existing MIDI input
    if (midiInput != nullptr) {
        try {
            midiInput->stop();
            midiInput.reset();
        } catch (const std::exception& e) {
            DBG("AudioProcessor: Error stopping MIDI input - " + juce::String(e.what()));
        }
    }

    currentMidiInput = deviceName;

    if (deviceName.isNotEmpty()) {
        try {
            auto devices = juce::MidiInput::getAvailableDevices();
            bool deviceFound = false;
            
            for (const auto& device : devices) {
                if (device.name == deviceName) {
                    // Null-pointer safety: Try to open MIDI device with error handling
                    midiInput = juce::MidiInput::openDevice(device.identifier, nullptr);
                    if (midiInput != nullptr) {
                        try {
                            midiInput->start();
                            DBG("AudioProcessor: Successfully opened MIDI input: " + deviceName);
                        } catch (const std::exception& e) {
                            DBG("AudioProcessor: Error starting MIDI input - " + juce::String(e.what()));
                            midiInput.reset();
                        }
                    } else {
                        DBG("AudioProcessor: Failed to open MIDI input device: " + deviceName);
                    }
                    break;
                }
            }
            
            if (!deviceFound) {
                DBG("AudioProcessor: MIDI input device not found: " + deviceName);
                currentMidiInput.clear();
            }
        } catch (const std::exception& e) {
            DBG("AudioProcessor: Exception in MIDI input setup - " + juce::String(e.what()));
            currentMidiInput.clear();
        }
    }
}

void OTTOAudioProcessor::setMidiOutput(const juce::String& deviceName) {
    if (deviceName == currentMidiOutput) return;

    // Null-pointer safety: Safely reset existing MIDI output
    if (midiOutput != nullptr) {
        try {
            midiOutput.reset();
        } catch (const std::exception& e) {
            DBG("AudioProcessor: Error closing MIDI output - " + juce::String(e.what()));
        }
    }
    
    currentMidiOutput = deviceName;

    if (deviceName.isNotEmpty()) {
        try {
            auto devices = juce::MidiOutput::getAvailableDevices();
            bool deviceFound = false;
            
            for (const auto& device : devices) {
                if (device.name == deviceName) {
                    deviceFound = true;
                    // Null-pointer safety: Try to open MIDI output device with error handling
                    midiOutput = juce::MidiOutput::openDevice(device.identifier);
                    if (midiOutput != nullptr) {
                        DBG("AudioProcessor: Successfully opened MIDI output: " + deviceName);
                    } else {
                        DBG("AudioProcessor: Failed to open MIDI output device: " + deviceName);
                    }
                    break;
                }
            }
            
            if (!deviceFound) {
                DBG("AudioProcessor: MIDI output device not found: " + deviceName);
                currentMidiOutput.clear();
            }
        } catch (const std::exception& e) {
            DBG("AudioProcessor: Exception in MIDI output setup - " + juce::String(e.what()));
            currentMidiOutput.clear();
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout OTTOAudioProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("masterVolume", 1),
        "Master Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f),
        INIConfig::Defaults::VOLUME));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("tempo", 1),
        "Tempo",
        juce::NormalisableRange<float>(static_cast<float>(INIConfig::Validation::MIN_TEMPO),
                                       static_cast<float>(INIConfig::Validation::MAX_TEMPO)),
        static_cast<float>(INIConfig::Defaults::DEFAULT_TEMPO)));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("swing", 1),
        "Swing",
        juce::NormalisableRange<float>(INIConfig::Validation::MIN_SWING,
                                       INIConfig::Validation::MAX_SWING),
        INIConfig::Defaults::SWING));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("energy", 1),
        "Energy",
        juce::NormalisableRange<float>(INIConfig::Validation::MIN_ENERGY,
                                       INIConfig::Validation::MAX_ENERGY),
        INIConfig::Defaults::ENERGY));

    for (int i = 1; i <= 8; ++i) {
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("player" + juce::String(i) + "Volume", 1),
            "Player " + juce::String(i) + " Volume",
            juce::NormalisableRange<float>(0.0f, 1.0f),
            INIConfig::Defaults::VOLUME));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("player" + juce::String(i) + "Pan", 1),
            "Player " + juce::String(i) + " Pan",
            juce::NormalisableRange<float>(-1.0f, 1.0f),
            0.0f));
    }

    return { params.begin(), params.end() };
}

void OTTOAudioProcessor::initializeParameters() {
    parameters.addParameterListener("masterVolume", this);
    parameters.addParameterListener("tempo", this);
    parameters.addParameterListener("swing", this);
    parameters.addParameterListener("energy", this);

    for (int i = 1; i <= 8; ++i) {
        parameters.addParameterListener("player" + juce::String(i) + "Volume", this);
        parameters.addParameterListener("player" + juce::String(i) + "Pan", this);
    }
}

void OTTOAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue) {
    // Null-pointer safety: Validate parameter ID and value
    if (parameterID.isEmpty()) {
        DBG("AudioProcessor: Empty parameter ID in parameter change callback");
        return;
    }

    if (!std::isfinite(newValue)) {
        DBG("AudioProcessor: Invalid parameter value: " + juce::String(newValue) + " for parameter: " + parameterID);
        return;
    }

    try {
        if (parameterID == "masterVolume") {
            // Validate volume range before applying
            float clampedVolume = juce::jlimit(0.0f, 1.0f, newValue);
            mixer.setMasterVolume(clampedVolume);
        }
        else if (parameterID == "tempo") {
            // Validate tempo range before applying
            float clampedTempo = juce::jlimit(
                static_cast<float>(INIConfig::Validation::MIN_TEMPO),
                static_cast<float>(INIConfig::Validation::MAX_TEMPO),
                newValue
            );
            midiEngine.setTempo(clampedTempo);
        }
        else if (parameterID == "swing") {
            // Null-pointer safety: Validate current player before use
            int currentPlayer = midiEngine.getCurrentPlayer();
            if (currentPlayer >= 0 && currentPlayer < 8) {  // Assuming 8 players max
                float clampedSwing = juce::jlimit(
                    INIConfig::Validation::MIN_SWING,
                    INIConfig::Validation::MAX_SWING,
                    newValue
                );
                midiEngine.setSwing(currentPlayer, clampedSwing);
            } else {
                DBG("AudioProcessor: Invalid current player index: " + juce::String(currentPlayer));
            }
        }
        else if (parameterID == "energy") {
            // Null-pointer safety: Validate current player before use
            int currentPlayer = midiEngine.getCurrentPlayer();
            if (currentPlayer >= 0 && currentPlayer < 8) {  // Assuming 8 players max
                float clampedEnergy = juce::jlimit(
                    INIConfig::Validation::MIN_ENERGY,
                    INIConfig::Validation::MAX_ENERGY,
                    newValue
                );
                midiEngine.setEnergy(currentPlayer, clampedEnergy);
            } else {
                DBG("AudioProcessor: Invalid current player index: " + juce::String(currentPlayer));
            }
        }
        else {
            // Handle player-specific parameters with null-pointer safety
            for (int i = 1; i <= 8; ++i) {
                if (parameterID == "player" + juce::String(i) + "Volume") {
                    // Validate player index and volume range
                    int playerIndex = i - 1;
                    if (playerIndex >= 0 && playerIndex < 8) {
                        float clampedVolume = juce::jlimit(0.0f, 1.0f, newValue);
                        mixer.setChannelVolume(playerIndex, clampedVolume);
                    } else {
                        DBG("AudioProcessor: Invalid player index: " + juce::String(playerIndex));
                    }
                    return;  // Exit loop early once parameter is found
                }
                else if (parameterID == "player" + juce::String(i) + "Pan") {
                    // Validate player index and pan range
                    int playerIndex = i - 1;
                    if (playerIndex >= 0 && playerIndex < 8) {
                        float clampedPan = juce::jlimit(-1.0f, 1.0f, newValue);
                        mixer.setChannelPan(playerIndex, clampedPan);
                    } else {
                        DBG("AudioProcessor: Invalid player index: " + juce::String(playerIndex));
                    }
                    return;  // Exit loop early once parameter is found
                }
            }
            // If we reach here, parameter wasn't recognized
            DBG("AudioProcessor: Unrecognized parameter: " + parameterID);
        }
    } catch (const std::exception& e) {
        DBG("AudioProcessor: Exception in parameter change - " + juce::String(e.what()) + 
            " for parameter: " + parameterID);
    }
}

const juce::String OTTOAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool OTTOAudioProcessor::acceptsMidi() const {
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OTTOAudioProcessor::producesMidi() const {
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OTTOAudioProcessor::isMidiEffect() const {
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OTTOAudioProcessor::getTailLengthSeconds() const {
    return INIConfig::LayoutConstants::pluginProcessorTailLength;
}

int OTTOAudioProcessor::getNumPrograms() {
    return INIConfig::LayoutConstants::pluginProcessorNumPrograms;
}

int OTTOAudioProcessor::getCurrentProgram() {
    return INIConfig::LayoutConstants::pluginProcessorCurrentProgram;
}

void OTTOAudioProcessor::setCurrentProgram(int index) {
    juce::ignoreUnused(index);
}

const juce::String OTTOAudioProcessor::getProgramName(int index) {
    juce::ignoreUnused(index);
    return {};
}

void OTTOAudioProcessor::changeProgramName(int index, const juce::String& newName) {
    juce::ignoreUnused(index, newName);
}

void OTTOAudioProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock) {
    sampleRate = newSampleRate;
    
    #if JUCE_MAC
        if (auto* device = deviceManager.getCurrentAudioDevice()) {
            auto setup = deviceManager.getAudioDeviceSetup();
            setup.bufferSize = juce::jmin(setup.bufferSize, 256);
            deviceManager.setAudioDeviceSetup(setup, false);
        }
    #elif JUCE_WINDOWS
        auto& deviceTypes = deviceManager.getAvailableDeviceTypes();
        for (auto* type : deviceTypes) {
            if (type->getTypeName() == "ASIO") {
                deviceManager.setCurrentAudioDeviceType(type->getTypeName(), true);
                break;
            }
        }
    #elif JUCE_LINUX
        if (auto* device = deviceManager.getCurrentAudioDevice()) {
            auto setup = deviceManager.getAudioDeviceSetup();
            setup.bufferSize = juce::jmax(setup.bufferSize, 512);
            deviceManager.setAudioDeviceSetup(setup, false);
        }
    #elif JUCE_ANDROID
        if (auto* device = deviceManager.getCurrentAudioDevice()) {
            auto setup = deviceManager.getAudioDeviceSetup();
            setup.sampleRate = juce::jmin(setup.sampleRate, 48000.0);
            deviceManager.setAudioDeviceSetup(setup, false);
        }
    #endif

    midiEngine.prepare(newSampleRate);
    sfzEngine.prepare(newSampleRate, samplesPerBlock);
    mixer.prepare(newSampleRate, samplesPerBlock);
    presetManager.prepare();

    auto* device = deviceManager.getCurrentAudioDevice();
    if (device != nullptr) {
        auto setup = deviceManager.getAudioDeviceSetup();
        setup.sampleRate = newSampleRate;
        setup.bufferSize = samplesPerBlock;
        deviceManager.setAudioDeviceSetup(setup, false);
    }
}

void OTTOAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    auto customData = xml->createNewChildElement("CustomData");
    ComponentState componentState;
    saveStates(componentState);

    customData->setAttribute("currentPlayer", componentState.currentPlayer);
    customData->setAttribute("playState", componentState.playState);
    customData->setAttribute("editMode", componentState.editMode);
    customData->setAttribute("tempo", componentState.globalSettings.tempo);
    customData->setAttribute("interfaceScale", componentState.globalSettings.interfaceScale);

    auto audioData = customData->createNewChildElement("AudioSettings");
    auto setup = deviceManager.getAudioDeviceSetup();
    audioData->setAttribute("deviceName", setup.outputDeviceName);
    audioData->setAttribute("sampleRate", setup.sampleRate);
    audioData->setAttribute("bufferSize", setup.bufferSize);
    audioData->setAttribute("inputChannels", setup.inputChannels.toString(2));
    audioData->setAttribute("outputChannels", setup.outputChannels.toString(2));

    audioData->setAttribute("midiInputDevice", currentMidiInput);
    audioData->setAttribute("midiOutputDevice", currentMidiOutput);

    auto midiData = customData->createNewChildElement("MidiMappings");
    const auto& mappings = midiEngine.getAllMidiMappings();
    for (int i = 0; i < mappings.size(); ++i) {
        auto mappingElement = midiData->createNewChildElement("Mapping" + juce::String(i));
        mappingElement->setAttribute("cc", mappings[i].ccNumber);
        mappingElement->setAttribute("channel", mappings[i].channel);
        mappingElement->setAttribute("parameter", mappings[i].parameterID);
        mappingElement->setAttribute("min", mappings[i].minValue);
        mappingElement->setAttribute("max", mappings[i].maxValue);
        mappingElement->setAttribute("enabled", mappings[i].enabled);
        mappingElement->setAttribute("feedback", mappings[i].sendFeedback);
    }

    for (int i = 0; i < 8; ++i) {
        auto playerElement = customData->createNewChildElement("Player" + juce::String(i));
        const auto& player = componentState.playerSettings[i];

        playerElement->setAttribute("enabled", player.enabled);
        playerElement->setAttribute("drumkit", player.selectedDrumkit);
        playerElement->setAttribute("volume", player.volume);
        playerElement->setAttribute("pan", player.pan);
        playerElement->setAttribute("muted", player.muted);
        playerElement->setAttribute("soloed", player.soloed);
        playerElement->setAttribute("midiChannel", player.midiChannel);
        playerElement->setAttribute("swingValue", player.swingValue);
        playerElement->setAttribute("energyValue", player.energyValue);
        playerElement->setAttribute("selectedButton", player.selectedButton);
    }

    copyXmlToBinary(*xml, destData);
}

void OTTOAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(parameters.state.getType())) {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));

            if (auto* customData = xmlState->getChildByName("CustomData")) {
                ComponentState componentState;

                componentState.currentPlayer = customData->getIntAttribute("currentPlayer", 0);
                componentState.playState = customData->getBoolAttribute("playState", false);
                componentState.editMode = customData->getBoolAttribute("editMode", false);
                componentState.globalSettings.tempo = customData->getIntAttribute("tempo", INIConfig::Defaults::DEFAULT_TEMPO);
                componentState.globalSettings.interfaceScale = static_cast<float>(customData->getDoubleAttribute("interfaceScale", 1.0));

                if (auto* audioData = customData->getChildByName("AudioSettings")) {
                    auto setup = deviceManager.getAudioDeviceSetup();
                    setup.outputDeviceName = audioData->getStringAttribute("deviceName");
                    setup.inputDeviceName = setup.outputDeviceName;
                    setup.sampleRate = audioData->getDoubleAttribute("sampleRate", static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE));
                    setup.bufferSize = audioData->getIntAttribute("bufferSize", 256);

                    auto inputChannelsString = audioData->getStringAttribute("inputChannels");
                    if (inputChannelsString.isNotEmpty()) {
                        setup.inputChannels.parseString(inputChannelsString, 2);
                    }

                    auto outputChannelsString = audioData->getStringAttribute("outputChannels");
                    if (outputChannelsString.isNotEmpty()) {
                        setup.outputChannels.parseString(outputChannelsString, 2);
                    }

                    deviceManager.setAudioDeviceSetup(setup, false);

                    setMidiInput(audioData->getStringAttribute("midiInputDevice"));
                    setMidiOutput(audioData->getStringAttribute("midiOutputDevice"));
                }

                if (auto* midiData = customData->getChildByName("MidiMappings")) {
                    midiEngine.clearAllMidiMappings();

                    for (auto* mappingElement : midiData->getChildIterator()) {
                        MidiEngine::MidiMapping mapping;
                        mapping.ccNumber = mappingElement->getIntAttribute("cc", -1);
                        mapping.channel = mappingElement->getIntAttribute("channel", 0);
                        mapping.parameterID = mappingElement->getStringAttribute("parameter");
                        mapping.minValue = static_cast<float>(mappingElement->getDoubleAttribute("min", 0.0));
                        mapping.maxValue = static_cast<float>(mappingElement->getDoubleAttribute("max", 1.0));
                        mapping.enabled = mappingElement->getBoolAttribute("enabled", true);
                        mapping.sendFeedback = mappingElement->getBoolAttribute("feedback", false);

                        if (mapping.ccNumber >= 0 && mapping.parameterID.isNotEmpty()) {
                            midiEngine.addMidiMapping(mapping);
                        }
                    }
                }

                for (int i = 0; i < 8; ++i) {
                    if (auto* playerElement = customData->getChildByName("Player" + juce::String(i))) {
                        auto& player = componentState.playerSettings[i];

                        player.enabled = playerElement->getBoolAttribute("enabled", true);
                        player.selectedDrumkit = playerElement->getStringAttribute("drumkit", INIConfig::Defaults::DEFAULT_DRUMKIT);
                        player.volume = static_cast<float>(playerElement->getDoubleAttribute("volume", 1.0));
                        player.pan = static_cast<float>(playerElement->getDoubleAttribute("pan", 0.0));
                        player.muted = playerElement->getBoolAttribute("muted", false);
                        player.soloed = playerElement->getBoolAttribute("soloed", false);
                        player.midiChannel = playerElement->getIntAttribute("midiChannel", i + 1);
                        player.swingValue = static_cast<float>(playerElement->getDoubleAttribute("swingValue", INIConfig::Defaults::SWING));
                        player.energyValue = static_cast<float>(playerElement->getDoubleAttribute("energyValue", INIConfig::Defaults::ENERGY));
                        player.selectedButton = playerElement->getIntAttribute("selectedButton", 0);
                    }
                }

                loadStates(componentState);
            }
        }
    }
}

void OTTOAudioProcessor::releaseResources() {
    sfzEngine.release();
    mixer.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OTTOAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OTTOAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    // Null-pointer safety: Verify buffer integrity
    if (buffer.getNumChannels() == 0 || buffer.getNumSamples() == 0) {
        DBG("AudioProcessor: Invalid buffer dimensions - channels: " + juce::String(buffer.getNumChannels()) + 
            ", samples: " + juce::String(buffer.getNumSamples()));
        return;
    }

    // Null-pointer safety: Check for valid buffer data
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        if (buffer.getReadPointer(ch) == nullptr) {
            DBG("AudioProcessor: Null buffer data pointer for channel " + juce::String(ch));
            return;
        }
    }

    juce::ScopedNoDenormals noDenormals;
    
    #if JUCE_MAC || JUCE_IOS
        juce::FloatVectorOperations::disableDenormalisedNumberSupport();
    #elif JUCE_WINDOWS
        // Null-pointer safety: Verify current thread before setting priority
        if (juce::Thread::getCurrentThread() != nullptr) {
            juce::Thread::getCurrentThread()->setPriority(10);
        }
    #elif JUCE_LINUX
        static juce::CriticalSection audioProcessingLock;
        juce::ScopedLock lock(audioProcessingLock);
    #elif JUCE_ANDROID
        buffer.clear();
    #endif

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Null-pointer safety: Validate channel counts
    if (totalNumInputChannels < 0 || totalNumOutputChannels < 0) {
        DBG("AudioProcessor: Invalid channel configuration");
        return;
    }

    // Clear any unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        if (i < buffer.getNumChannels()) {
            buffer.clear(i, 0, buffer.getNumSamples());
        }
    }

    // Process MIDI input - null-pointer safety handled in MidiEngine::process
    try {
        midiEngine.process(midiMessages);
    } catch (const std::exception& e) {
        DBG("AudioProcessor: MIDI processing error - " + juce::String(e.what()));
        // Continue processing to avoid audio dropouts
    }

    // Process SFZ engine - null-pointer safety handled in SFZEngine::process
    try {
        sfzEngine.process(buffer, midiMessages);
    } catch (const std::exception& e) {
        DBG("AudioProcessor: SFZ processing error - " + juce::String(e.what()));
        // Clear buffer on error to prevent audio artifacts
        buffer.clear();
    }

    // Process mixer - null-pointer safety handled in Mixer::processBlock
    try {
        mixer.processBlock(buffer);
    } catch (const std::exception& e) {
        DBG("AudioProcessor: Mixer processing error - " + juce::String(e.what()));
        // Apply emergency volume reduction to prevent damage
        buffer.applyGain(0.1f);
    }

    // Send MIDI output with null-pointer safety
    if (midiOutput != nullptr && midiMessages.getNumEvents() > 0) {
        try {
            midiOutput->sendBlockOfMessages(midiMessages, juce::Time::getMillisecondCounterHiRes(), sampleRate);
        } catch (const std::exception& e) {
            DBG("AudioProcessor: MIDI output error - " + juce::String(e.what()));
        }
    }
}

bool OTTOAudioProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor* OTTOAudioProcessor::createEditor() {
    // Null-pointer safety: Safely create editor with exception handling
    try {
        auto* editor = new OTTOAudioProcessorEditor(*this);
        if (editor != nullptr) {
            DBG("AudioProcessor: Successfully created editor");
            return editor;
        } else {
            DBG("AudioProcessor: Failed to create editor - null pointer returned");
            return nullptr;
        }
    } catch (const std::exception& e) {
        DBG("AudioProcessor: Exception creating editor - " + juce::String(e.what()));
        return nullptr;
    } catch (...) {
        DBG("AudioProcessor: Unknown exception creating editor");
        return nullptr;
    }
}

void OTTOAudioProcessor::saveStates(ComponentState& state) {
    try {
        // Null-pointer safety: Check each parameter before dereferencing
        if (auto* tempoParam = parameters.getRawParameterValue("tempo")) {
            if (tempoParam != nullptr) {
                float tempoValue = *tempoParam;
                if (std::isfinite(tempoValue)) {
                    state.globalSettings.tempo = INIConfig::clampTempo(static_cast<int>(tempoValue));
                } else {
                    DBG("AudioProcessor: Invalid tempo value, using default");
                    state.globalSettings.tempo = INIConfig::Defaults::DEFAULT_TEMPO;
                }
            }
        } else {
            DBG("AudioProcessor: Tempo parameter not found, using default");
            state.globalSettings.tempo = INIConfig::Defaults::DEFAULT_TEMPO;
        }

        if (auto* masterVolParam = parameters.getRawParameterValue("masterVolume")) {
            if (masterVolParam != nullptr && std::isfinite(*masterVolParam)) {
                state.sliderValues["masterVolume"] = INIConfig::clampVolume(*masterVolParam);
            } else {
                state.sliderValues["masterVolume"] = INIConfig::Defaults::VOLUME;
            }
        }

        if (auto* swingParam = parameters.getRawParameterValue("swing")) {
            if (swingParam != nullptr && std::isfinite(*swingParam)) {
                state.sliderValues["swing"] = INIConfig::clampSwing(*swingParam);
            } else {
                state.sliderValues["swing"] = INIConfig::Defaults::SWING;
            }
        }

        if (auto* energyParam = parameters.getRawParameterValue("energy")) {
            if (energyParam != nullptr && std::isfinite(*energyParam)) {
                state.sliderValues["energy"] = INIConfig::clampEnergy(*energyParam);
            } else {
                state.sliderValues["energy"] = INIConfig::Defaults::ENERGY;
            }
        }

        // Null-pointer safety: Validate player settings array bounds
        for (int i = 0; i < juce::jmin(8, INIConfig::Defaults::MAX_PLAYERS); ++i) {
            auto& player = state.playerSettings[i];

            if (auto* volParam = parameters.getRawParameterValue("player" + juce::String(i + 1) + "Volume")) {
                if (volParam != nullptr && std::isfinite(*volParam)) {
                    player.volume = INIConfig::clampVolume(*volParam);
                    player.volumeValue = player.volume;
                } else {
                    player.volume = INIConfig::Defaults::VOLUME;
                    player.volumeValue = player.volume;
                }
            }

            if (auto* panParam = parameters.getRawParameterValue("player" + juce::String(i + 1) + "Pan")) {
                if (panParam != nullptr && std::isfinite(*panParam)) {
                    player.pan = INIConfig::clampPan(*panParam);
                } else {
                    player.pan = 0.0f;
                }
            }
        }
    } catch (const std::exception& e) {
        DBG("AudioProcessor: Exception in saveStates - " + juce::String(e.what()));
        // Set safe defaults on error
        state.globalSettings.tempo = INIConfig::Defaults::DEFAULT_TEMPO;
        state.sliderValues["masterVolume"] = INIConfig::Defaults::VOLUME;
    }

    state.audioSettings.midiInputDevice = currentMidiInput;
    state.audioSettings.midiOutputDevice = currentMidiOutput;

    midiEngine.saveStates(state);
    sfzEngine.saveStates(state);
    presetManager.saveStates(state);
    mixer.saveState(state);
}

void OTTOAudioProcessor::loadStates(const ComponentState& state) {
    if (auto* tempoParam = parameters.getRawParameterValue("tempo")) {
        *tempoParam = static_cast<float>(state.globalSettings.tempo);
    }

    if (state.sliderValues.count("masterVolume") > 0) {
        if (auto* masterVolParam = parameters.getRawParameterValue("masterVolume")) {
            *masterVolParam = state.sliderValues.at("masterVolume");
        }
    }

    if (state.sliderValues.count("swing") > 0) {
        if (auto* swingParam = parameters.getRawParameterValue("swing")) {
            *swingParam = state.sliderValues.at("swing");
        }
    }

    if (state.sliderValues.count("energy") > 0) {
        if (auto* energyParam = parameters.getRawParameterValue("energy")) {
            *energyParam = state.sliderValues.at("energy");
        }
    }

    for (int i = 0; i < 8; ++i) {
        const auto& player = state.playerSettings[i];

        if (auto* volParam = parameters.getRawParameterValue("player" + juce::String(i + 1) + "Volume")) {
            *volParam = player.volume;
        }

        if (auto* panParam = parameters.getRawParameterValue("player" + juce::String(i + 1) + "Pan")) {
            *panParam = player.pan;
        }
    }

    setMidiInput(state.audioSettings.midiInputDevice);
    setMidiOutput(state.audioSettings.midiOutputDevice);

    midiEngine.loadStates(state);
    sfzEngine.loadStates(state);
    presetManager.loadStates(state);
    mixer.loadState(state);
}

void OTTOAudioProcessor::updateParametersFromState(const ComponentState& state) {
    if (auto* tempoParam = parameters.getRawParameterValue("tempo")) {
        *tempoParam = static_cast<float>(state.globalSettings.tempo);
    }

    for (int i = 0; i < 8; ++i) {
        const auto& player = state.playerSettings[i];

        if (auto* volParam = parameters.getRawParameterValue("player" + juce::String(i + 1) + "Volume")) {
            *volParam = player.volume;
        }

        if (auto* panParam = parameters.getRawParameterValue("player" + juce::String(i + 1) + "Pan")) {
            *panParam = player.pan;
        }
    }
}

void OTTOAudioProcessor::updateStateFromParameters(ComponentState& state) {
    if (auto* tempoParam = parameters.getRawParameterValue("tempo")) {
        state.globalSettings.tempo = INIConfig::clampTempo(static_cast<int>(*tempoParam));
    }

    for (int i = 0; i < 8; ++i) {
        auto& player = state.playerSettings[i];

        if (auto* volParam = parameters.getRawParameterValue("player" + juce::String(i + 1) + "Volume")) {
            player.volume = INIConfig::clampVolume(*volParam);
        }

        if (auto* panParam = parameters.getRawParameterValue("player" + juce::String(i + 1) + "Pan")) {
            player.pan = INIConfig::clampPan(*panParam);
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new OTTOAudioProcessor();
}
