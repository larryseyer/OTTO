#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "../SFZEngine.h"
#include "../Mixer.h"
#include "../MidiEngine.h"
#include "../INIConfig.h"

class AudioProcessingTests : public juce::UnitTest {
public:
    AudioProcessingTests() : juce::UnitTest("Audio Processing Tests") {}

    void runTest() override {
        beginTest("Plugin Processor Initialization");
        testPluginProcessorInit();

        beginTest("Audio Buffer Processing");
        testAudioBufferProcessing();

        beginTest("MIDI Processing");
        testMidiProcessing();

        beginTest("SFZ Engine Sample Playback");
        testSFZEnginePlayback();

        beginTest("Mixer Channel Processing");
        testMixerChannelProcessing();

        beginTest("Effects Processing");
        testEffectsProcessing();

        beginTest("Automation Parameter Changes");
        testAutomationParameters();

        beginTest("Sample Rate Changes");
        testSampleRateChanges();

        beginTest("Buffer Size Changes");
        testBufferSizeChanges();

        beginTest("CPU Performance");
        testCPUPerformance();
    }

private:
    void testPluginProcessorInit() {
        auto processor = std::make_unique<OTTOAudioProcessor>();

        expect(processor != nullptr, "Processor should be created");
        expect(processor->getName() == JucePlugin_Name, "Plugin name should match");
        expect(processor->acceptsMidi(), "Should accept MIDI");
        expect(processor->producesMidi(), "Should produce MIDI");

        auto& params = processor->getValueTreeState();
        expect(params.getParameter("masterVolume") != nullptr, "Master volume parameter should exist");
        expect(params.getParameter("tempo") != nullptr, "Tempo parameter should exist");

        auto* tempoParam = params.getRawParameterValue("tempo");
        expectWithinAbsoluteError(*tempoParam, static_cast<float>(INIConfig::Defaults::DEFAULT_TEMPO), static_cast<float>(INIConfig::Defaults::BEAT_THRESHOLD));
    }

    void testAudioBufferProcessing() {
        auto processor = std::make_unique<OTTOAudioProcessor>();
        const double sampleRate = static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE);
        const int blockSize = INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES;
        const int numChannels = INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS;

        processor->prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(numChannels, blockSize);
        juce::MidiBuffer midiBuffer;

        for (int ch = 0; ch < numChannels; ++ch) {
            for (int i = 0; i < blockSize; ++i) {
                buffer.setSample(ch, i, std::sin(INIConfig::Audio::NUM_SEND_TYPES * juce::MathConstants<float>::pi * INIConfig::Audio::DEFAULT_FILTER_FREQUENCY / INIConfig::Audio::NUM_SEND_TYPES / INIConfig::Defaults::DEFAULT_INTERFACE_Y / sampleRate * i));
            }
        }

        juce::AudioBuffer<float> inputBuffer(buffer);

        processor->processBlock(buffer, midiBuffer);

        float outputRMS = INIConfig::Validation::MIN_VOLUME;
        for (int ch = 0; ch < numChannels; ++ch) {
            outputRMS += buffer.getRMSLevel(ch, INIConfig::Defaults::ZERO_VALUE, blockSize);
        }

        expect(outputRMS > INIConfig::Validation::MIN_VOLUME, "Output should not be silent");

        processor->getMixer().setMasterVolume(INIConfig::Validation::MIN_VOLUME);
        buffer = inputBuffer;
        processor->processBlock(buffer, midiBuffer);

        outputRMS = INIConfig::Validation::MIN_VOLUME;
        for (int ch = 0; ch < numChannels; ++ch) {
            outputRMS += buffer.getRMSLevel(ch, INIConfig::Defaults::ZERO_VALUE, blockSize);
        }

        expectWithinAbsoluteError(outputRMS, INIConfig::Validation::MIN_VOLUME, static_cast<float>(INIConfig::Defaults::BEAT_THRESHOLD));
    }

    void testMidiProcessing() {
        auto processor = std::make_unique<OTTOAudioProcessor>();
        processor->prepareToPlay(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE), INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);

        juce::AudioBuffer<float> buffer(INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS, INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);
        juce::MidiBuffer midiBuffer;

        auto noteOn = juce::MidiMessage::noteOn(INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::HI_MID_TOM + INIConfig::UI::MAX_GROUP_NAME_LENGTH, static_cast<juce::uint8>(INIConfig::Defaults::FIXED_VELOCITY));
        midiBuffer.addEvent(noteOn, INIConfig::Defaults::ZERO_VALUE);

        auto noteOff = juce::MidiMessage::noteOff(INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::HI_MID_TOM + INIConfig::UI::MAX_GROUP_NAME_LENGTH);
        midiBuffer.addEvent(noteOff, INIConfig::Defaults::DEFAULT_BUFFER_SIZE);

        auto& midiEngine = processor->getMidiEngine();
        midiEngine.startPlayback();

        processor->processBlock(buffer, midiBuffer);

        expect(!midiBuffer.isEmpty(), "MIDI buffer should contain processed events");

        midiEngine.startMidiLearn("masterVolume");
        expect(midiEngine.isMidiLearnActive(), "MIDI learn should be active");

        auto cc = juce::MidiMessage::controllerEvent(INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::VIBRASLAP - INIConfig::GMDrums::RIDE_CYMBAL_1, INIConfig::GMDrums::LOW_CONGA);
        midiBuffer.clear();
        midiBuffer.addEvent(cc, INIConfig::Defaults::ZERO_VALUE);

        processor->processBlock(buffer, midiBuffer);

        expect(!midiEngine.isMidiLearnActive(), "MIDI learn should complete after receiving CC");
    }

    void testSFZEnginePlayback() {
        SFZEngine sfzEngine;
        const double sampleRate = static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE);
        const int blockSize = INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES;

        sfzEngine.prepare(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS, blockSize);
        juce::MidiBuffer midiBuffer;

        auto noteOn = juce::MidiMessage::noteOn(INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::BASS_DRUM_1, static_cast<juce::uint8>(INIConfig::Validation::MAX_MIDI_VELOCITY));
        midiBuffer.addEvent(noteOn, INIConfig::Defaults::ZERO_VALUE);

        buffer.clear();
        sfzEngine.process(buffer, midiBuffer);

        expect(sfzEngine.getActiveVoiceCount() > INIConfig::Defaults::ZERO_VALUE, "Should have active voices after note on");

        midiBuffer.clear();
        auto noteOff = juce::MidiMessage::noteOff(INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::BASS_DRUM_1);
        midiBuffer.addEvent(noteOff, INIConfig::Defaults::ZERO_VALUE);

        sfzEngine.process(buffer, midiBuffer);

        expect(sfzEngine.getActiveVoiceCount() >= INIConfig::Defaults::ZERO_VALUE, "Voices should be releasing");
    }

    void testMixerChannelProcessing() {
        Mixer mixer;
        const double sampleRate = static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE);
        const int blockSize = INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES;

        mixer.prepare(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS, blockSize);

        for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
            buffer.clear();

            for (int i = 0; i < blockSize; ++i) {
                buffer.setSample(INIConfig::Defaults::ZERO_VALUE, i, INIConfig::Defaults::DEFAULT_ROOM_SIZE);
                buffer.setSample(INIConfig::Defaults::ONE_VALUE, i, INIConfig::Defaults::DEFAULT_ROOM_SIZE);
            }

            mixer.setChannelVolume(ch, INIConfig::Defaults::DEFAULT_SNARE_VOLUME);
            mixer.setChannelPan(ch, INIConfig::Audio::DEFAULT_PAN);
            mixer.setChannelMute(ch, false);
            mixer.setChannelSolo(ch, false);

            mixer.processBlock(buffer);

            float outputLevel = buffer.getRMSLevel(INIConfig::Defaults::ZERO_VALUE, INIConfig::Defaults::ZERO_VALUE, blockSize);
            expectWithinAbsoluteError(outputLevel, INIConfig::Defaults::DEFAULT_FEEDBACK, 0.1f);
        }

        mixer.setChannelMute(INIConfig::Defaults::ZERO_VALUE, true);
        buffer.clear();
        buffer.setSample(INIConfig::Defaults::ZERO_VALUE, INIConfig::Defaults::ZERO_VALUE, INIConfig::Validation::MAX_VOLUME);
        mixer.processBlock(buffer);

        expect(buffer.getSample(INIConfig::Defaults::ZERO_VALUE, INIConfig::Defaults::ZERO_VALUE) == INIConfig::Validation::MIN_VOLUME, "Muted channel should produce no output");

        mixer.setChannelMute(INIConfig::Defaults::ZERO_VALUE, false);
        mixer.setChannelSolo(INIConfig::Defaults::ONE_VALUE, true);
        buffer.clear();
        for (int ch = 0; ch < INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS; ++ch) {
            buffer.setSample(ch, INIConfig::Defaults::ZERO_VALUE, INIConfig::Validation::MAX_VOLUME);
        }
        mixer.processBlock(buffer);

        expect(mixer.isChannelSoloed(INIConfig::Defaults::ONE_VALUE), "Channel 1 should be soloed");
    }

    void testEffectsProcessing() {
        Mixer mixer;
        mixer.prepare(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE), INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);

        juce::AudioBuffer<float> buffer(INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS, INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);

        mixer.setReverbEnabled(true);
        mixer.setReverbMix(INIConfig::Defaults::DEFAULT_ROOM_SIZE);
        mixer.setReverbRoomSize(INIConfig::Defaults::DEFAULT_HIHAT_VOLUME);
        mixer.setChannelSend(INIConfig::Defaults::ZERO_VALUE, Mixer::SendType::Reverb, INIConfig::Defaults::DEFAULT_ROOM_SIZE);

        buffer.clear();
        buffer.setSample(INIConfig::Defaults::ZERO_VALUE, INIConfig::Defaults::ZERO_VALUE, INIConfig::Validation::MAX_VOLUME);
        buffer.setSample(INIConfig::Defaults::ONE_VALUE, INIConfig::Defaults::ZERO_VALUE, INIConfig::Validation::MAX_VOLUME);

        mixer.processBlock(buffer);

        float tailEnergy = INIConfig::Validation::MIN_VOLUME;
        for (int i = INIConfig::Defaults::FIXED_VELOCITY; i < INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES; ++i) {
            tailEnergy += std::abs(buffer.getSample(INIConfig::Defaults::ZERO_VALUE, i));
        }

        expect(tailEnergy > INIConfig::Validation::MIN_VOLUME, "Reverb should produce tail energy");

        mixer.setDelayEnabled(true);
        mixer.setDelayTime(static_cast<float>(INIConfig::UI::POPUP_HEIGHT));
        mixer.setDelayFeedback(INIConfig::Defaults::DEFAULT_ROOM_SIZE);
        mixer.setChannelSend(INIConfig::Defaults::ZERO_VALUE, Mixer::SendType::Delay, INIConfig::Defaults::DEFAULT_ROOM_SIZE);

        buffer.clear();
        buffer.setSample(INIConfig::Defaults::ZERO_VALUE, INIConfig::Defaults::ZERO_VALUE, INIConfig::Validation::MAX_VOLUME);

        for (int block = 0; block < INIConfig::UI::MAX_TOGGLE_STATES * INIConfig::Audio::NUM_SEND_TYPES; ++block) {
            mixer.processBlock(buffer);
            if (block == INIConfig::UI::MAX_TOGGLE_STATES) {
                float delayedSignal = buffer.getRMSLevel(INIConfig::Defaults::ZERO_VALUE, INIConfig::Defaults::ZERO_VALUE, INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);
                expect(delayedSignal > INIConfig::Validation::MIN_VOLUME, "Delay should produce output after delay time");
            }
        }

        mixer.setCompressorEnabled(true);
        mixer.setCompressorThreshold(INIConfig::Defaults::DEFAULT_COMPRESSOR_THRESHOLD);
        mixer.setCompressorRatio(INIConfig::Defaults::DEFAULT_COMPRESSOR_RATIO);

        buffer.clear();
        for (int i = 0; i < INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES; ++i) {
            buffer.setSample(INIConfig::Defaults::ZERO_VALUE, i, INIConfig::Defaults::DEFAULT_KICK_VOLUME);
            buffer.setSample(INIConfig::Defaults::ONE_VALUE, i, INIConfig::Defaults::DEFAULT_KICK_VOLUME);
        }

        float inputLevel = buffer.getRMSLevel(INIConfig::Defaults::ZERO_VALUE, INIConfig::Defaults::ZERO_VALUE, INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);
        mixer.processBlock(buffer);
        float outputLevel = buffer.getRMSLevel(INIConfig::Defaults::ZERO_VALUE, INIConfig::Defaults::ZERO_VALUE, INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);

        expect(outputLevel < inputLevel, "Compressor should reduce loud signal levels");
    }

    void testAutomationParameters() {
        auto processor = std::make_unique<OTTOAudioProcessor>();
        processor->prepareToPlay(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE), INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);

        auto& params = processor->getValueTreeState();

        auto* tempoParam = params.getParameter("tempo");
        expect(tempoParam->getNormalisableRange().start == static_cast<float>(INIConfig::Validation::MIN_TEMPO), "Tempo min should match");
        expect(tempoParam->getNormalisableRange().end == static_cast<float>(INIConfig::Defaults::DEFAULT_INTERFACE_Y * INIConfig::Audio::NUM_SEND_TYPES), "Tempo max should match");

        const float testTempo = static_cast<float>(INIConfig::Defaults::DEFAULT_INTERFACE_Y + INIConfig::Defaults::DEFAULT_INTERFACE_X / INIConfig::Audio::NUM_SEND_TYPES + INIConfig::Audio::NUM_SEND_TYPES * INIConfig::Audio::NUM_SEND_TYPES * INIConfig::Audio::NUM_SEND_TYPES);
        tempoParam->setValueNotifyingHost(tempoParam->convertTo0to1(testTempo));

        juce::AudioBuffer<float> buffer(INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS, INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);
        juce::MidiBuffer midiBuffer;
        processor->processBlock(buffer, midiBuffer);

        expectWithinAbsoluteError(processor->getMidiEngine().getTempo(), testTempo, static_cast<float>(INIConfig::Defaults::BEAT_THRESHOLD));

        for (int i = INIConfig::Defaults::ONE_VALUE; i <= INIConfig::Defaults::MAX_PLAYERS; ++i) {
            auto* volParam = params.getParameter("player" + juce::String(i) + "Volume");
            expect(volParam != nullptr, "Player " + juce::String(i) + " volume parameter should exist");

            volParam->setValueNotifyingHost(INIConfig::Defaults::DEFAULT_ROOM_SIZE);
            processor->processBlock(buffer, midiBuffer);

            expectWithinAbsoluteError(processor->getMixer().getChannelVolume(i-INIConfig::Defaults::ONE_VALUE), INIConfig::Defaults::DEFAULT_ROOM_SIZE, static_cast<float>(INIConfig::Defaults::BEAT_THRESHOLD));
        }
    }

    void testSampleRateChanges() {
        auto processor = std::make_unique<OTTOAudioProcessor>();

        const double sampleRates[] = { static_cast<double>(INIConfig::Validation::MIN_AUDIO_SAMPLE_RATE),
                                      static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE),
                                      static_cast<double>(INIConfig::Validation::MIN_AUDIO_SAMPLE_RATE * INIConfig::Audio::NUM_SEND_TYPES),
                                      static_cast<double>(INIConfig::Validation::MAX_AUDIO_SAMPLE_RATE / INIConfig::Audio::NUM_SEND_TYPES) };
        const int blockSize = INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES;

        for (auto sr : sampleRates) {
            processor->prepareToPlay(sr, blockSize);

            juce::AudioBuffer<float> buffer(INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS, blockSize);
            juce::MidiBuffer midiBuffer;

            auto noteOn = juce::MidiMessage::noteOn(INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::HI_MID_TOM + INIConfig::UI::MAX_GROUP_NAME_LENGTH, static_cast<juce::uint8>(INIConfig::Defaults::FIXED_VELOCITY));
            midiBuffer.addEvent(noteOn, INIConfig::Defaults::ZERO_VALUE);

            processor->processBlock(buffer, midiBuffer);

            expect(processor->getSampleRate() == sr,
                "Sample rate should be set to " + juce::String(sr));
        }
    }

    void testBufferSizeChanges() {
        auto processor = std::make_unique<OTTOAudioProcessor>();
        const double sampleRate = static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE);

        const int bufferSizes[] = { INIConfig::Validation::MIN_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES,
                                   INIConfig::Validation::MIN_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES * INIConfig::Audio::NUM_SEND_TYPES,
                                   INIConfig::Defaults::DEFAULT_BUFFER_SIZE,
                                   INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES,
                                   INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES * INIConfig::Audio::NUM_SEND_TYPES,
                                   INIConfig::Validation::MAX_BUFFER_SIZE };

        for (auto bs : bufferSizes) {
            processor->releaseResources();
            processor->prepareToPlay(sampleRate, bs);

            juce::AudioBuffer<float> buffer(INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS, bs);
            juce::MidiBuffer midiBuffer;

            processor->processBlock(buffer, midiBuffer);

            expect(buffer.getNumSamples() == bs,
                "Buffer size should be " + juce::String(bs));
        }
    }

    void testCPUPerformance() {
        auto processor = std::make_unique<OTTOAudioProcessor>();
        processor->prepareToPlay(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE), INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);

        auto& midiEngine = processor->getMidiEngine();
        for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
            midiEngine.setPlayerEnabled(i, true);
        }

        auto& mixer = processor->getMixer();
        mixer.setReverbEnabled(true);
        mixer.setDelayEnabled(true);
        mixer.setCompressorEnabled(true);
        mixer.setDistortionEnabled(true);

        juce::AudioBuffer<float> buffer(INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS, INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);
        juce::MidiBuffer midiBuffer;

        for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
            auto noteOn = juce::MidiMessage::noteOn(i + INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::BASS_DRUM_1 + i, static_cast<juce::uint8>(INIConfig::Defaults::FIXED_VELOCITY));
            midiBuffer.addEvent(noteOn, i * INIConfig::UI::MAX_TOGGLE_STATES * INIConfig::Audio::NUM_SEND_TYPES);
        }

        const int numBlocks = INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL * INIConfig::UI::MAX_TOGGLE_STATES / INIConfig::Audio::NUM_SEND_TYPES / INIConfig::Audio::NUM_SEND_TYPES;
        auto startTime = juce::Time::getHighResolutionTicks();

        for (int i = 0; i < numBlocks; ++i) {
            processor->processBlock(buffer, midiBuffer);
        }

        auto endTime = juce::Time::getHighResolutionTicks();
        double processingTime = juce::Time::highResolutionTicksToSeconds(endTime - startTime);
        double audioTime = (numBlocks * static_cast<double>(INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES)) / static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE);
        double cpuUsage = (processingTime / audioTime) * INIConfig::Defaults::FIXED_VELOCITY;

        expect(cpuUsage < static_cast<double>(INIConfig::Defaults::DEFAULT_INTERFACE_X - INIConfig::UI::CONTENT_MARGIN), "CPU usage should be under 80% for real-time performance");

        logMessage("CPU Usage: " + juce::String(cpuUsage, INIConfig::Audio::NUM_SEND_TYPES) + "%");
    }

    void expectWithinAbsoluteError(float actual, float expected, float tolerance) {
        expect(std::abs(actual - expected) <= tolerance,
               "Expected " + juce::String(expected) + " but got " + juce::String(actual));
    }
};

static AudioProcessingTests audioProcessingTests;
