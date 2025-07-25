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
    if (auto* param = parameters.getParameter(parameterID)) {
        const juce::MessageManagerLock mmLock;
        param->setValueNotifyingHost(param->convertTo0to1(value));
    }

    if (parameterID == "playState") {
        if (value > 0.5f) {
            midiEngine.startPlayback();
        } else {
            midiEngine.stopPlayback();
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

    if (midiInput) {
        midiInput->stop();
        midiInput.reset();
    }

    currentMidiInput = deviceName;

    if (deviceName.isNotEmpty()) {
        auto devices = juce::MidiInput::getAvailableDevices();
        for (const auto& device : devices) {
            if (device.name == deviceName) {
                midiInput = juce::MidiInput::openDevice(device.identifier, nullptr);
                if (midiInput) {
                    midiInput->start();
                }
                break;
            }
        }
    }
}

void OTTOAudioProcessor::setMidiOutput(const juce::String& deviceName) {
    if (deviceName == currentMidiOutput) return;

    midiOutput.reset();
    currentMidiOutput = deviceName;

    if (deviceName.isNotEmpty()) {
        auto devices = juce::MidiOutput::getAvailableDevices();
        for (const auto& device : devices) {
            if (device.name == deviceName) {
                midiOutput = juce::MidiOutput::openDevice(device.identifier);
                break;
            }
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
    if (parameterID == "masterVolume") {
        mixer.setMasterVolume(newValue);
    }
    else if (parameterID == "tempo") {
        midiEngine.setTempo(newValue);
    }
    else if (parameterID == "swing") {
        midiEngine.setSwing(midiEngine.getCurrentPlayer(), newValue);
    }
    else if (parameterID == "energy") {
        midiEngine.setEnergy(midiEngine.getCurrentPlayer(), newValue);
    }
    else {
        for (int i = 1; i <= 8; ++i) {
            if (parameterID == "player" + juce::String(i) + "Volume") {
                mixer.setChannelVolume(i - 1, newValue);
                break;
            }
            else if (parameterID == "player" + juce::String(i) + "Pan") {
                mixer.setChannelPan(i - 1, newValue);
                break;
            }
        }
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
    juce::ScopedNoDenormals noDenormals;
    
    #if JUCE_MAC || JUCE_IOS
        juce::FloatVectorOperations::disableDenormalisedNumberSupport();
    #elif JUCE_WINDOWS
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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (midiInput) {
    }

    midiEngine.process(midiMessages);

    sfzEngine.process(buffer, midiMessages);

    mixer.processBlock(buffer);

    if (midiOutput && midiMessages.getNumEvents() > 0) {
        midiOutput->sendBlockOfMessages(midiMessages, juce::Time::getMillisecondCounterHiRes(), sampleRate);
    }
}

bool OTTOAudioProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor* OTTOAudioProcessor::createEditor() {
    return new OTTOAudioProcessorEditor(*this);
}

void OTTOAudioProcessor::saveStates(ComponentState& state) {
    if (auto* tempoParam = parameters.getRawParameterValue("tempo")) {
        state.globalSettings.tempo = INIConfig::clampTempo(static_cast<int>(*tempoParam));
    }

    if (auto* masterVolParam = parameters.getRawParameterValue("masterVolume")) {
        state.sliderValues["masterVolume"] = INIConfig::clampVolume(*masterVolParam);
    }

    if (auto* swingParam = parameters.getRawParameterValue("swing")) {
        state.sliderValues["swing"] = INIConfig::clampSwing(*swingParam);
    }

    if (auto* energyParam = parameters.getRawParameterValue("energy")) {
        state.sliderValues["energy"] = INIConfig::clampEnergy(*energyParam);
    }

    for (int i = 0; i < 8; ++i) {
        auto& player = state.playerSettings[i];

        if (auto* volParam = parameters.getRawParameterValue("player" + juce::String(i + 1) + "Volume")) {
            player.volume = INIConfig::clampVolume(*volParam);
            player.volumeValue = player.volume;
        }

        if (auto* panParam = parameters.getRawParameterValue("player" + juce::String(i + 1) + "Pan")) {
            player.pan = INIConfig::clampPan(*panParam);
        }
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
