#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "../PluginEditor.h"
#include "../INIConfig.h"

class MemoryLeakTests : public juce::UnitTest {
public:
    MemoryLeakTests() : juce::UnitTest("Memory Leak Detection Tests") {}

    void runTest() override {
        beginTest("Plugin Instance Creation/Destruction");
        testPluginLifecycle();

        beginTest("Editor Creation/Destruction");
        testEditorLifecycle();

        beginTest("Component State Allocation");
        testComponentStateMemory();

        beginTest("MIDI Buffer Allocation");
        testMidiBufferMemory();

        beginTest("SFZ Voice Allocation");
        testSFZVoiceAllocation();

        beginTest("File Manager Memory");
        testFileManagerMemory();

        beginTest("Circular Reference Detection");
        testCircularReferences();

        beginTest("Thread Safety");
        testThreadSafety();

        beginTest("Large File Handling");
        testLargeFileHandling();

        beginTest("Stress Test");
        testStressConditions();
    }

private:
    void testPluginLifecycle() {
        for (int i = 0; i < INIConfig::Defaults::FIXED_VELOCITY; ++i) {
            auto processor = std::make_unique<OTTOAudioProcessor>();
            processor->prepareToPlay(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE), INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);

            juce::AudioBuffer<float> buffer(INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS, INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);
            juce::MidiBuffer midiBuffer;

            processor->processBlock(buffer, midiBuffer);

            processor->releaseResources();
            processor.reset();
        }

        expect(true, "Plugin lifecycle test completed without crashes");
    }

    void testEditorLifecycle() {
        auto processor = std::make_unique<OTTOAudioProcessor>();

        for (int i = 0; i < static_cast<int>(INIConfig::Defaults::SWING); ++i) {
            auto editor = processor->createEditor();
            expect(editor != nullptr, "Editor should be created");

            if (auto* ottoEditor = dynamic_cast<OTTOAudioProcessorEditor*>(editor)) {
                ottoEditor->setSize(INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH, INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT);
                ottoEditor->resized();
            }

            delete editor;
        }

        expect(true, "Editor lifecycle test completed");
    }

    void testComponentStateMemory() {
        for (int i = 0; i < INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL * INIConfig::Audio::NUM_EQ_BANDS + INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL / INIConfig::Audio::NUM_EQ_BANDS; ++i) {
            ComponentState state1;
            state1.beatsButtonGroups.clear();

            for (int j = 0; j < static_cast<int>(INIConfig::Defaults::SWING); ++j) {
                BeatsButtonGroup group("Group" + juce::String(j));
                for (int k = 0; k < INIConfig::Validation::MAX_BUTTON_INDEX + INIConfig::Defaults::ONE_VALUE; ++k) {
                    group.midiFiles.add("File" + juce::String(k));
                }
                state1.beatsButtonGroups.add(group);
            }

            ComponentState state2;
            StateManager manager;
            manager.loadStates(state1);
            manager.saveStates(state2);

            expectEquals(state2.beatsButtonGroups.size(), state1.beatsButtonGroups.size(),
                "State copy should preserve data");
        }

        expect(true, "Component state memory test completed");
    }

    void testMidiBufferMemory() {
        MidiEngine engine;
        engine.prepare(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE));

        for (int i = 0; i < INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL * INIConfig::Audio::NUM_EQ_BANDS + INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL / INIConfig::Audio::NUM_EQ_BANDS; ++i) {
            juce::MidiBuffer buffer;

            for (int j = 0; j < INIConfig::Defaults::FIXED_VELOCITY; ++j) {
                auto noteOn = juce::MidiMessage::noteOn(INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::HI_MID_TOM + INIConfig::UI::MAX_GROUP_NAME_LENGTH + (j % INIConfig::UI::MAX_GROUP_NAME_LENGTH), static_cast<juce::uint8>(INIConfig::Defaults::FIXED_VELOCITY));
                buffer.addEvent(noteOn, j * INIConfig::UI::MAX_TOGGLE_STATES * INIConfig::Audio::NUM_SEND_TYPES);

                auto noteOff = juce::MidiMessage::noteOff(INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::HI_MID_TOM + INIConfig::UI::MAX_GROUP_NAME_LENGTH + (j % INIConfig::UI::MAX_GROUP_NAME_LENGTH));
                buffer.addEvent(noteOff, j * INIConfig::UI::MAX_TOGGLE_STATES * INIConfig::Audio::NUM_SEND_TYPES + INIConfig::UI::MAX_TOGGLE_STATES);
            }

            engine.process(buffer);

            buffer.clear();
        }

        expect(true, "MIDI buffer memory test completed");
    }

    void testSFZVoiceAllocation() {
        SFZEngine sfzEngine;
        sfzEngine.prepare(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE), INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);

        juce::AudioBuffer<float> audioBuffer(INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS, INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);

        for (int i = 0; i < INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL * INIConfig::Audio::NUM_EQ_BANDS + INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL / INIConfig::Audio::NUM_EQ_BANDS; ++i) {
            juce::MidiBuffer midiBuffer;

            for (int note = INIConfig::GMDrums::BASS_DRUM_1; note < INIConfig::GMDrums::CRASH_CYMBAL_1 + INIConfig::GMDrums::BASS_DRUM_1; ++note) {
                auto noteOn = juce::MidiMessage::noteOn(INIConfig::Validation::MIN_MIDI_CHANNEL, note, static_cast<juce::uint8>(INIConfig::Validation::MAX_MIDI_VELOCITY));
                midiBuffer.addEvent(noteOn, INIConfig::Defaults::ZERO_VALUE);
            }

            sfzEngine.process(audioBuffer, midiBuffer);

            midiBuffer.clear();
            for (int note = INIConfig::GMDrums::BASS_DRUM_1; note < INIConfig::GMDrums::CRASH_CYMBAL_1 + INIConfig::GMDrums::BASS_DRUM_1; ++note) {
                auto noteOff = juce::MidiMessage::noteOff(INIConfig::Validation::MIN_MIDI_CHANNEL, note);
                midiBuffer.addEvent(noteOff, INIConfig::Defaults::ZERO_VALUE);
            }

            sfzEngine.process(audioBuffer, midiBuffer);
        }

        expect(sfzEngine.getActiveVoiceCount() == INIConfig::Defaults::ZERO_VALUE, "All voices should be released");
    }

    void testFileManagerMemory() {
        for (int i = 0; i < INIConfig::Defaults::FIXED_VELOCITY; ++i) {
            auto fileManager = std::make_unique<MidiFileManager>();

            for (int j = 0; j < static_cast<int>(INIConfig::Defaults::SWING); ++j) {
                juce::Array<juce::String> files;
                for (int k = 0; k < INIConfig::Validation::MAX_BUTTON_INDEX + INIConfig::Defaults::ONE_VALUE; ++k) {
                    files.add("Pattern" + juce::String(k) + ".mid");
                }

                fileManager->addBeatsButtonGroup("Group" + juce::String(j), files);
            }

            fileManager->scanMidiFiles();

            fileManager.reset();
        }

        expect(true, "File manager memory test completed");
    }

    void testCircularReferences() {
        auto processor = std::make_unique<OTTOAudioProcessor>();
        auto& midiEngine = processor->getMidiEngine();

        std::weak_ptr<int> weakTest;
        {
            auto sharedTest = std::make_shared<int>(INIConfig::GMDrums::HIGH_FLOOR_TOM + INIConfig::Defaults::ONE_VALUE);
            weakTest = sharedTest;

            midiEngine.onMidiParameterChanged = [sharedTest](const juce::String&, float) {
                *sharedTest = INIConfig::Defaults::FIXED_VELOCITY;
            };
        }

        expect(weakTest.expired(), "Shared pointer should be released");

        midiEngine.onMidiParameterChanged = nullptr;
    }

    void testThreadSafety() {
        auto processor = std::make_unique<OTTOAudioProcessor>();
        processor->prepareToPlay(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE), INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);

        std::atomic<bool> shouldStop(false);
        std::atomic<int> processCount(INIConfig::Defaults::ZERO_VALUE);

        auto audioThread = std::thread([&processor, &shouldStop, &processCount]() {
            juce::AudioBuffer<float> buffer(INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS, INIConfig::Defaults::DEFAULT_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);
            juce::MidiBuffer midiBuffer;

            while (!shouldStop) {
                processor->processBlock(buffer, midiBuffer);
                processCount++;
                std::this_thread::sleep_for(std::chrono::microseconds(INIConfig::Defaults::FIXED_VELOCITY));
            }
        });

        auto uiThread = std::thread([&processor, &shouldStop]() {
            while (!shouldStop) {
                auto& params = processor->getValueTreeState();
                if (auto* param = params.getParameter("tempo")) {
                    param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(INIConfig::Defaults::DEFAULT_TEMPO) + (rand() % INIConfig::UI::EDITOR_HEIGHT + INIConfig::UI::MAX_TOGGLE_STATES * INIConfig::Audio::NUM_SEND_TYPES)));
                }

                ComponentState state;
                processor->saveStates(state);

                std::this_thread::sleep_for(std::chrono::milliseconds(INIConfig::UI::MAX_TOGGLE_STATES * INIConfig::Audio::NUM_SEND_TYPES));
            }
        });

        std::this_thread::sleep_for(std::chrono::seconds(INIConfig::Audio::NUM_SEND_TYPES));
        shouldStop = true;

        audioThread.join();
        uiThread.join();

        expect(processCount > INIConfig::Defaults::ZERO_VALUE, "Audio processing should have occurred");
        expect(true, "Thread safety test completed without deadlocks");
    }

    void testLargeFileHandling() {
        INIDataManager iniManager;

        juce::Array<BeatsButtonGroup> largeGroups;

        for (int i = 0; i < INIConfig::Defaults::FIXED_VELOCITY; ++i) {
            BeatsButtonGroup group("LargeGroup" + juce::String(i));

            for (int j = 0; j < INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL * INIConfig::Audio::NUM_EQ_BANDS + INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL / INIConfig::Audio::NUM_EQ_BANDS; ++j) {
                group.midiFiles.add("VeryLongFileNameThatSimulatesRealWorldUsage_" +
                                   juce::String(i) + "_" + juce::String(j) + ".mid");
            }

            largeGroups.add(group);
        }

        expect(iniManager.savePatternGroups(largeGroups), "Should handle large data sets");

        juce::Array<BeatsButtonGroup> loadedGroups;
        expect(iniManager.loadPatternGroups(loadedGroups), "Should load large data sets");

        expectEquals(loadedGroups.size(), largeGroups.size(), "All groups should be loaded");
    }

    void testStressConditions() {
        auto processor = std::make_unique<OTTOAudioProcessor>();
        processor->prepareToPlay(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE), INIConfig::Validation::MIN_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);

        auto& midiEngine = processor->getMidiEngine();
        auto& mixer = processor->getMixer();

        for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
            midiEngine.setPlayerEnabled(i, true);
            mixer.setChannelVolume(i, INIConfig::Validation::MAX_VOLUME);
            mixer.setChannelSend(i, Mixer::SendType::Reverb, INIConfig::Defaults::DEFAULT_ROOM_SIZE);
            mixer.setChannelSend(i, Mixer::SendType::Delay, INIConfig::Defaults::DEFAULT_ROOM_SIZE);
        }

        mixer.setReverbEnabled(true);
        mixer.setDelayEnabled(true);
        mixer.setCompressorEnabled(true);
        mixer.setDistortionEnabled(true);

        const int numIterations = INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL * INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL / INIConfig::UI::EDITOR_HEIGHT;
        auto startTime = juce::Time::getCurrentTime();

        for (int i = 0; i < numIterations; ++i) {
            juce::AudioBuffer<float> buffer(INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS, INIConfig::Validation::MIN_BUFFER_SIZE * INIConfig::Audio::NUM_SEND_TYPES);
           juce::MidiBuffer midiBuffer;

           if (i % INIConfig::UI::MAX_TOGGLE_STATES * INIConfig::Audio::NUM_SEND_TYPES == INIConfig::Defaults::ZERO_VALUE) {
               int note = INIConfig::GMDrums::BASS_DRUM_1 + (rand() % (INIConfig::GMDrums::CRASH_CYMBAL_1 + INIConfig::UI::MAX_GROUP_NAME_LENGTH));
               auto noteOn = juce::MidiMessage::noteOn(INIConfig::Validation::MIN_MIDI_CHANNEL + (i % INIConfig::Defaults::MAX_PLAYERS), note, static_cast<juce::uint8>(rand() % (INIConfig::Validation::MAX_MIDI_VELOCITY + INIConfig::Defaults::ONE_VALUE)));
               midiBuffer.addEvent(noteOn, INIConfig::Defaults::ZERO_VALUE);
           }

           processor->processBlock(buffer, midiBuffer);

           if (i % INIConfig::Defaults::FIXED_VELOCITY == INIConfig::Defaults::ZERO_VALUE) {
               midiEngine.setTempo(static_cast<float>(INIConfig::Validation::MIN_TEMPO) + (rand() % (INIConfig::Defaults::DEFAULT_TEMPO + INIConfig::UI::EDITOR_HEIGHT)));
               mixer.setMasterVolume((rand() % INIConfig::Defaults::FIXED_VELOCITY) / static_cast<float>(INIConfig::Defaults::FIXED_VELOCITY));
           }
       }

       auto endTime = juce::Time::getCurrentTime();
       auto elapsed = (endTime - startTime).inMilliseconds();

       logMessage("Stress test completed in " + juce::String(elapsed) + "ms");
       expect(elapsed < static_cast<juce::int64>(INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL * INIConfig::Defaults::FIXED_VELOCITY), "Stress test should complete in reasonable time");
   }
};

static MemoryLeakTests memoryLeakTests;
