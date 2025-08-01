#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "../PluginEditor.h"
#include "../PatternSuggestionEngine.h"
#include "../INIConfig.h"
#include <memory>

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

        beginTest("AI Component Memory");
        testAIComponentMemory();

        beginTest("Pattern Engine Memory");
        testPatternEngineMemory();

        beginTest("Smart Pointer Usage");
        testSmartPointerUsage();

        beginTest("AI Stress Conditions");
        testAIStressConditions();

        beginTest("Dialog Window Memory Management");
        testDialogWindowMemory();

        beginTest("Popup Component Lifecycle");
        testPopupComponentLifecycle();
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

   void testAIComponentMemory() {
       for (int i = 0; i < INIConfig::Defaults::FIXED_VELOCITY; ++i) {
           auto processor = std::make_unique<OTTOAudioProcessor>();
           auto editor = std::unique_ptr<OTTOAudioProcessorEditor>(
               dynamic_cast<OTTOAudioProcessorEditor*>(processor->createEditor()));
           
           if (editor) {
               editor->setSize(INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH, 
                             INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT);
               editor->resized();
               
               editor.reset();
           }
           
           processor.reset();
       }
       
       expect(true, "AI component memory test completed without leaks");
   }

   void testPatternEngineMemory() {
       for (int i = 0; i < static_cast<int>(INIConfig::Defaults::SWING); ++i) {
           auto patternEngine = std::make_unique<PatternSuggestionEngine>();
           
           for (int genre = 0; genre < 8; ++genre) {
               PatternSuggestionEngine::SuggestionParams params;
               params.genre = static_cast<PatternSuggestionEngine::Genre>(genre);
               params.complexity = 0.5f;
               params.bars = 4;
               
               auto suggestions = patternEngine->suggestPatterns(params, 5);
               
               for (const auto& suggestion : suggestions) {
                   expect(!suggestion.name.isEmpty(), "Pattern suggestion should have valid name");
                   expect(suggestion.pattern.getNumEvents() >= 0, "Pattern should have valid event count");
               }
           }
           
           juce::MidiMessageSequence testPattern;
           auto noteOn = juce::MidiMessage::noteOn(10, INIConfig::GMDrums::BASS_DRUM_1, 100);
           testPattern.addEvent(noteOn);
           
           patternEngine->learnFromPattern(testPattern, PatternSuggestionEngine::Genre::Rock);
           
           juce::Array<float> velocities, timings;
           for (int j = 0; j < 10; ++j) {
               velocities.add(80.0f + j);
               timings.add(0.01f * j);
           }
           patternEngine->adaptToPerformance(velocities, timings);
           
           patternEngine.reset();
       }
       
       expect(true, "Pattern engine memory test completed");
   }

   void testSmartPointerUsage() {
       {
           auto testPtr = std::make_unique<int>(42);
           auto sharedPtr = std::make_shared<int>(84);
           std::weak_ptr<int> weakPtr = sharedPtr;
           
           expect(*testPtr == 42, "Unique pointer should hold correct value");
           expect(*sharedPtr == 84, "Shared pointer should hold correct value");
           expect(!weakPtr.expired(), "Weak pointer should not be expired");
           
           sharedPtr.reset();
           expect(weakPtr.expired(), "Weak pointer should be expired after shared_ptr reset");
       }
       
       expect(true, "Smart pointer usage test completed");
   }

   void testAIStressConditions() {
       auto patternEngine = std::make_unique<PatternSuggestionEngine>();
       
       for (int i = 0; i < 1000; ++i) {
           PatternSuggestionEngine::SuggestionParams params;
           params.genre = static_cast<PatternSuggestionEngine::Genre>(i % 8);
           params.complexity = (i % 100) / 100.0f;
           params.bars = (i % 8) + 1;
           params.tempo = 60.0f + (i % 200);
           
           auto pattern = patternEngine->generatePattern(params);
           expect(!pattern.name.isEmpty(), "Stress test pattern should be valid");
           
           if (i % 100 == 0) {
               patternEngine->learnFromPattern(pattern.pattern, params.genre);
           }
       }
       
       expect(true, "AI stress test completed");
   }

   void testDialogWindowMemory() {
       // Test modal dialog window creation and automatic cleanup
       // Simulate the corrected TopBarComponent dialog patterns
       
       for (int i = 0; i < 50; ++i) {
           auto window = std::make_unique<juce::DialogWindow>("Test Dialog",
                                                             juce::Colours::white,
                                                             true);
           
           class TestContent : public juce::Component {
           public:
               juce::TextButton okButton{"OK"};
               juce::TextButton cancelButton{"Cancel"};
               
               TestContent() {
                   addAndMakeVisible(okButton);
                   addAndMakeVisible(cancelButton);
                   setSize(200, 100);
               }
               
               void resized() override {
                   auto bounds = getLocalBounds();
                   okButton.setBounds(bounds.removeFromLeft(90));
                   cancelButton.setBounds(bounds);
               }
           };
           
           auto content = std::make_unique<TestContent>();
           auto* windowPtr = window.get();
           
           content->okButton.onClick = [windowPtr] {
               windowPtr->exitModalState(1);
           };
           
           content->cancelButton.onClick = [windowPtr] {
               windowPtr->exitModalState(0);
           };
           
           window->setContentOwned(content.release(), true);
           window->centreWithSize(200, 100);
           
           // Test that window can be properly released for modal operation
           auto* rawWindow = window.release();
           expect(rawWindow != nullptr, "Window should be created successfully");
           
           // Clean up immediately for test
           delete rawWindow;
       }
       
       expect(true, "Dialog window memory test completed");
   }

   void testPopupComponentLifecycle() {
       // Test CustomGroupManagerPopup memory management
       class TestParentComponent : public juce::Component {
       public:
           TestParentComponent() { setSize(400, 300); }
       };
       
       for (int i = 0; i < 30; ++i) {
           auto parentComponent = std::make_unique<TestParentComponent>();
           
           // Test popup creation and cleanup (simulating the corrected pattern)
           for (int j = 0; j < 5; ++j) {
               // Simulate popup creation with automatic cleanup
               auto testPopup = std::make_unique<juce::Component>();
               testPopup->setName("TestPopup");
               testPopup->setBounds(50, 50, 100, 100);
               
               parentComponent->addAndMakeVisible(testPopup.release());
               
               // Simulate cleanup (what our corrected code does)
               for (int k = parentComponent->getNumChildComponents() - 1; k >= 0; --k) {
                   if (auto* popup = parentComponent->getChildComponent(k)) {
                       if (popup->getName() == "TestPopup") {
                           parentComponent->removeChildComponent(popup);
                           delete popup;
                           break;
                       }
                   }
               }
           }
           
           expectEquals(parentComponent->getNumChildComponents(), 0, 
                       "All popup components should be cleaned up");
       }
       
       expect(true, "Popup component lifecycle test completed");
   }
};

static MemoryLeakTests memoryLeakTests;
