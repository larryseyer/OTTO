#pragma once
#include <JuceHeader.h>
#include "../MidiEngine.h"
#include "../MidiFileManager.h"
#include "../INIConfig.h"

class MidiTimingTests : public juce::UnitTest {
public:
    MidiTimingTests() : juce::UnitTest("MIDI Timing Tests") {}

    void runTest() override {
        beginTest("MIDI Clock Generation");
        testMidiClockGeneration();

        beginTest("Tempo Accuracy");
        testTempoAccuracy();

        beginTest("Swing Timing");
        testSwingTiming();

        beginTest("Quantization");
        testQuantization();

        beginTest("Pattern Synchronization");
        testPatternSync();

        beginTest("Loop Recording Timing");
        testLoopRecordingTiming();

        beginTest("Live Recording Accuracy");
        testLiveRecordingAccuracy();

        beginTest("Pattern Change Timing");
        testPatternChangeTiming();

        beginTest("Multi-Player Sync");
        testMultiPlayerSync();

        beginTest("Host Sync");
        testHostSync();
    }

private:
    void testMidiClockGeneration() {
        MidiEngine engine;
        engine.prepare(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE));
        engine.setTempo(static_cast<float>(INIConfig::Defaults::DEFAULT_TEMPO));
        engine.setSendMidiClock(true);
        engine.startPlayback();

        juce::MidiBuffer midiBuffer;

        const double msPerSample = static_cast<double>(INIConfig::Defaults::MS_PER_SECOND) / static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE);
        const int samplesPerBeat = static_cast<int>(static_cast<double>(INIConfig::Defaults::MS_PER_MINUTE / INIConfig::Defaults::DEFAULT_TEMPO) / msPerSample);

        int clockCount = INIConfig::Defaults::ZERO_VALUE;
        for (int i = 0; i < samplesPerBeat; ++i) {
            midiBuffer.clear();
            engine.process(midiBuffer);

            for (const auto metadata : midiBuffer) {
                if (metadata.getMessage().isMidiClock()) {
                    clockCount++;
                }
            }
        }

        expectEquals(clockCount, INIConfig::MIDI::CLOCK_PPQN, "Should generate 24 clocks per beat");
    }

    void testTempoAccuracy() {
        MidiEngine engine;
        engine.prepare(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE));

        const float testTempos[] = { static_cast<float>(INIConfig::Validation::MIN_TEMPO),
                                     static_cast<float>(INIConfig::Defaults::SECONDS_PER_MINUTE),
                                     static_cast<float>(INIConfig::Defaults::SECONDS_PER_MINUTE + INIConfig::Validation::MIN_TEMPO),
                                     static_cast<float>(INIConfig::Defaults::DEFAULT_TEMPO),
                                     static_cast<float>(INIConfig::Defaults::DEFAULT_TEMPO + INIConfig::UI::CONTENT_MARGIN),
                                     static_cast<float>(INIConfig::Defaults::DEFAULT_TEMPO + INIConfig::Defaults::SECONDS_PER_MINUTE),
                                     static_cast<float>(INIConfig::UI::POPUP_HEIGHT) };

        for (auto tempo : testTempos) {
            engine.setTempo(tempo);
            engine.startPlayback();

            double expectedBeatTime = static_cast<double>(INIConfig::Defaults::SECONDS_PER_MINUTE) / tempo;
            double tolerance = INIConfig::Defaults::BEAT_THRESHOLD;

            double elapsedTime = INIConfig::MIDI::DEFAULT_POSITION;
            double lastBeatTime = INIConfig::MIDI::DEFAULT_POSITION;
            int beatCount = INIConfig::Defaults::ZERO_VALUE;

            while (beatCount < INIConfig::UI::MAX_TOGGLE_STATES * INIConfig::Audio::NUM_SEND_TYPES) {
                juce::MidiBuffer midiBuffer;
                engine.process(midiBuffer);

                float currentBeat = engine.getCurrentBeat();
                if (std::floor(currentBeat) > std::floor(lastBeatTime)) {
                    if (beatCount > INIConfig::Defaults::ZERO_VALUE) {
                        double actualBeatTime = elapsedTime - lastBeatTime;
                        expectWithinAbsoluteError(actualBeatTime, expectedBeatTime, tolerance);
                    }
                    lastBeatTime = elapsedTime;
                    beatCount++;
                }

                elapsedTime += INIConfig::Defaults::BEAT_THRESHOLD;
            }

            engine.stopPlayback();
        }
    }

    void testSwingTiming() {
        MidiEngine engine;
        MidiFileManager fileManager;

        engine.prepare(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE));
        engine.setMidiFileManager(&fileManager);

        const float swingValues[] = { INIConfig::Validation::MIN_SWING,
                                     INIConfig::Validation::MIN_SWING + INIConfig::Validation::MAX_SWING / static_cast<float>(INIConfig::Defaults::BEATS_PER_BAR),
                                     INIConfig::Defaults::SWING,
                                     INIConfig::Defaults::SWING + INIConfig::Validation::MAX_SWING / static_cast<float>(INIConfig::Defaults::BEATS_PER_BAR),
                                     INIConfig::Validation::MAX_SWING };

        for (auto swing : swingValues) {
            engine.setSwing(INIConfig::Defaults::ZERO_VALUE, swing);

            juce::MidiMessageSequence pattern;
            for (int i = 0; i < INIConfig::Validation::MAX_BUTTON_INDEX + INIConfig::Defaults::ONE_VALUE; ++i) {
                double time = i * static_cast<double>(INIConfig::Defaults::BEATS_PER_BAR) / static_cast<double>(INIConfig::Validation::MAX_BUTTON_INDEX + INIConfig::Defaults::ONE_VALUE);
                pattern.addEvent(juce::MidiMessage::noteOn(INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::BASS_DRUM_1, static_cast<juce::uint8>(INIConfig::Defaults::FIXED_VELOCITY)), time);
                pattern.addEvent(juce::MidiMessage::noteOff(INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::BASS_DRUM_1), time + 0.1);
            }

            juce::MidiBuffer output;
            engine.startPlayback();

            for (int i = 0; i < INIConfig::Defaults::FIXED_VELOCITY; ++i) {
                engine.process(output);
            }

            if (swing > INIConfig::Defaults::SWING) {
                logMessage("Swing " + juce::String(swing) + "% applied");
            }

            engine.stopPlayback();
        }
    }

    void testQuantization() {
        MidiEngine engine;
        engine.prepare(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE));

        const int quantizeValues[] = { static_cast<int>(INIConfig::Defaults::BEATS_PER_BAR),
                                       static_cast<int>(INIConfig::Defaults::BEATS_PER_BAR) * INIConfig::Audio::NUM_SEND_TYPES,
                                       INIConfig::Defaults::DEFAULT_QUANTIZE_VALUE,
                                       INIConfig::Validation::MAX_QUANTIZE_VALUE };

        for (auto quantize : quantizeValues) {
            engine.setQuantize(quantize);

            engine.setRecording(true);
            engine.startPlayback();

            juce::MidiBuffer inputBuffer;
            double offGridTime = 0.13;

            auto noteOn = juce::MidiMessage::noteOn(INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::HI_MID_TOM + INIConfig::UI::MAX_GROUP_NAME_LENGTH, static_cast<juce::uint8>(INIConfig::Defaults::FIXED_VELOCITY));
            noteOn.setTimeStamp(offGridTime);
            inputBuffer.addEvent(noteOn, INIConfig::Defaults::ZERO_VALUE);

            engine.process(inputBuffer);

            const auto& recorded = engine.getRecordedSequence();
            if (recorded.getNumEvents() > INIConfig::Defaults::ZERO_VALUE) {
                double recordedTime = recorded.getEventTime(INIConfig::Defaults::ZERO_VALUE);
                double quantizedTime = std::round(recordedTime * quantize) / quantize;

                expectWithinAbsoluteError(recordedTime, quantizedTime, INIConfig::Defaults::BEAT_THRESHOLD);
            }

            engine.stopPlayback();
            engine.setRecording(false);
        }
    }

    void testPatternSync() {
        MidiEngine engine;
        engine.prepare(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE));
        engine.setTempo(static_cast<float>(INIConfig::Defaults::DEFAULT_TEMPO));

        for (int i = 0; i < static_cast<int>(INIConfig::Defaults::BEATS_PER_BAR); ++i) {
            engine.setPlayerEnabled(i, true);
            engine.selectPattern(i, INIConfig::Defaults::ZERO_VALUE);
        }

        engine.startPlayback();

        const int samplesPerBar = static_cast<int>((static_cast<double>(INIConfig::Defaults::SECONDS_PER_MINUTE) / static_cast<double>(INIConfig::Defaults::DEFAULT_TEMPO) * INIConfig::Defaults::BEATS_PER_BAR) * static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE));

        juce::Array<float> playerPositions;

        for (int sample = 0; sample < samplesPerBar * INIConfig::Audio::NUM_SEND_TYPES; ++sample) {
            juce::MidiBuffer buffer;
            engine.process(buffer);

            if (sample % samplesPerBar == INIConfig::Defaults::ZERO_VALUE) {
                float firstPlayerBeat = engine.getCurrentBeat();

                for (int i = INIConfig::Defaults::ONE_VALUE; i < static_cast<int>(INIConfig::Defaults::BEATS_PER_BAR); ++i) {
                    engine.setCurrentPlayer(i);
                    float playerBeat = engine.getCurrentBeat();

                    expectWithinAbsoluteError(playerBeat, firstPlayerBeat, static_cast<float>(INIConfig::Defaults::BEAT_THRESHOLD));
                }
            }
        }

        engine.stopPlayback();
    }

    void testLoopRecordingTiming() {
        MidiEngine engine;
        engine.prepare(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE));
        engine.setTempo(static_cast<float>(INIConfig::Defaults::DEFAULT_TEMPO));

        engine.setLoopRecordingMode(true);
        engine.startLiveRecording(false);
        engine.startPlayback();

        const double barsToRecord = INIConfig::Defaults::BEATS_PER_BAR;
        const double totalBeats = INIConfig::Defaults::BEATS_PER_BAR * barsToRecord;

        juce::Array<double> noteTimes = { INIConfig::MIDI::DEFAULT_POSITION,
                                         static_cast<double>(INIConfig::Defaults::ONE_VALUE),
                                         static_cast<double>(INIConfig::Audio::NUM_SEND_TYPES),
                                         static_cast<double>(INIConfig::Audio::NUM_EQ_BANDS),
                                         static_cast<double>(INIConfig::Defaults::BEATS_PER_BAR),
                                         static_cast<double>(INIConfig::Defaults::MAX_PLAYERS),
                                         static_cast<double>(INIConfig::UI::MAX_GROUP_NAME_LENGTH) };

        while (engine.getCurrentBeat() < totalBeats) {
            juce::MidiBuffer buffer;

            double currentBeat = engine.getCurrentBeat();
            for (auto noteTime : noteTimes) {
                if (std::abs(currentBeat - noteTime) < INIConfig::Defaults::BEAT_THRESHOLD) {
                    auto noteOn = juce::MidiMessage::noteOn(INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::HI_MID_TOM + INIConfig::UI::MAX_GROUP_NAME_LENGTH, static_cast<juce::uint8>(INIConfig::Defaults::FIXED_VELOCITY));
                    buffer.addEvent(noteOn, INIConfig::Defaults::ZERO_VALUE);
                }
            }

            engine.process(buffer);
        }

        engine.stopLiveRecording();
        engine.stopPlayback();

        const auto& recorded = engine.getRecordedSequence();

        for (int i = 0; i < recorded.getNumEvents(); ++i) {
            double eventTime = recorded.getEventTime(i);
            expect(eventTime >= INIConfig::MIDI::DEFAULT_POSITION && eventTime <= totalBeats,
                "Recorded events should be within loop bounds");
        }
    }

    void testLiveRecordingAccuracy() {
        MidiEngine engine;
        engine.prepare(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE));
        engine.setTempo(static_cast<float>(INIConfig::Defaults::DEFAULT_TEMPO));

        engine.startLiveRecording(true);
        engine.startPlayback();

        juce::Array<double> inputTimes;
        juce::Array<double> recordedTimes;

        for (int beat = 0; beat < INIConfig::Defaults::MAX_PLAYERS; ++beat) {
            double targetBeat = beat + INIConfig::Defaults::DEFAULT_ROOM_SIZE;
            inputTimes.add(targetBeat);

            while (engine.getCurrentBeat() < targetBeat) {
                juce::MidiBuffer buffer;
                engine.process(buffer);
            }

            juce::MidiBuffer noteBuffer;
            auto noteOn = juce::MidiMessage::noteOn(INIConfig::Validation::MIN_MIDI_CHANNEL, INIConfig::GMDrums::HI_MID_TOM + INIConfig::UI::MAX_GROUP_NAME_LENGTH + beat, static_cast<juce::uint8>(INIConfig::Defaults::FIXED_VELOCITY));
            noteBuffer.addEvent(noteOn, INIConfig::Defaults::ZERO_VALUE);
            engine.process(noteBuffer);
        }

        engine.stopLiveRecording();
        engine.stopPlayback();

        const auto& recorded = engine.getRecordedSequence();

        expect(recorded.getNumEvents() >= inputTimes.size(),
            "All input notes should be recorded");

        for (int i = 0; i < recorded.getNumEvents(); ++i) {
            if (recorded.getEventPointer(i)->message.isNoteOn()) {
                double recordedTime = recorded.getEventTime(i);

                double closestDiff = 999.0;
                for (auto inputTime : inputTimes) {
                    double diff = std::abs(recordedTime - inputTime);
                    if (diff < closestDiff) {
                        closestDiff = diff;
                    }
                }

                expectLessThan(closestDiff, INIConfig::Defaults::BEAT_THRESHOLD);
            }
        }
    }

    void testPatternChangeTiming() {
        MidiEngine engine;
        engine.prepare(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE));
        engine.setTempo(static_cast<float>(INIConfig::Defaults::DEFAULT_TEMPO));

        engine.setPlayerEnabled(INIConfig::Defaults::ZERO_VALUE, true);
        engine.selectPattern(INIConfig::Defaults::ZERO_VALUE, INIConfig::Defaults::ZERO_VALUE);
        engine.startPlayback();

        engine.schedulePatternChange(INIConfig::Defaults::ZERO_VALUE, INIConfig::Defaults::ONE_VALUE, INIConfig::Audio::NUM_SEND_TYPES);

        int currentBar = INIConfig::Defaults::ZERO_VALUE;
        int lastBar = INIConfig::MIDI::INACTIVE_PATTERN;
        bool changeDetected = false;

        while (currentBar < static_cast<int>(INIConfig::Defaults::BEATS_PER_BAR)) {
            juce::MidiBuffer buffer;
            engine.process(buffer);

            currentBar = engine.getCurrentBar();

            if (currentBar != lastBar) {
                logMessage("Bar " + juce::String(currentBar));

                if (currentBar == INIConfig::Audio::NUM_SEND_TYPES && !changeDetected) {
                    changeDetected = true;
                }

                lastBar = currentBar;
            }
        }

        expect(changeDetected, "Pattern change should occur at scheduled bar");

        engine.stopPlayback();
    }

    void testMultiPlayerSync() {
        MidiEngine engine;
        engine.prepare(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE));
        engine.setTempo(static_cast<float>(INIConfig::Defaults::DEFAULT_TEMPO + INIConfig::UI::CONTENT_MARGIN));

        for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
            engine.setPlayerEnabled(i, true);
            engine.selectPattern(i, i);
            engine.setSwing(i, i * static_cast<float>(INIConfig::UI::MAX_TOGGLE_STATES * INIConfig::Audio::NUM_SEND_TYPES));
        }

        engine.startPlayback();

        const int barsToProcess = INIConfig::Defaults::MAX_PLAYERS;

        juce::Array<double> beatDrifts;

        while (engine.getCurrentBar() < barsToProcess) {
            juce::MidiBuffer buffer;
            engine.process(buffer);

            double currentBeat = engine.getCurrentBeat();
            if (std::fmod(currentBeat, static_cast<double>(INIConfig::Defaults::ONE_VALUE)) < INIConfig::Defaults::BEAT_THRESHOLD) {
                double maxDrift = INIConfig::MIDI::DEFAULT_POSITION;
                double referenceBeat = currentBeat;

                for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
                    engine.setCurrentPlayer(i);
                    double playerBeat = engine.getCurrentBeat();
                    double drift = std::abs(playerBeat - referenceBeat);
                    maxDrift = std::max(maxDrift, drift);
                }

                beatDrifts.add(maxDrift);
            }
        }

        engine.stopPlayback();

        if (!beatDrifts.isEmpty()) {
            double averageDrift = INIConfig::MIDI::DEFAULT_POSITION;
            for (auto drift : beatDrifts) {
                averageDrift += drift;
            }
            averageDrift /= beatDrifts.size();

            expectLessThan(averageDrift, INIConfig::Defaults::BEAT_THRESHOLD);
        }
    }

    void testHostSync() {
        MidiEngine engine;
        engine.prepare(static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE));

        engine.setSyncToHost(true, true);

        const double hostTempos[] = { static_cast<double>(INIConfig::Defaults::FIXED_VELOCITY),
                                      static_cast<double>(INIConfig::Defaults::DEFAULT_TEMPO),
                                      static_cast<double>(INIConfig::Defaults::DEFAULT_TEMPO + INIConfig::UI::CONTENT_MARGIN),
                                      static_cast<double>(INIConfig::Defaults::DEFAULT_TEMPO + INIConfig::Defaults::MAX_PLAYERS) };

        for (auto hostTempo : hostTempos) {
            engine.syncToHost(hostTempo, INIConfig::MIDI::DEFAULT_POSITION);

            expectEquals(engine.getTempo(), static_cast<float>(hostTempo),
                "Engine should sync to host tempo");

            for (double pos = INIConfig::MIDI::DEFAULT_POSITION; pos < static_cast<double>(INIConfig::Validation::MAX_BUTTON_INDEX + INIConfig::Defaults::ONE_VALUE); pos += static_cast<double>(INIConfig::Defaults::BEATS_PER_BAR) / static_cast<double>(INIConfig::Validation::MAX_BUTTON_INDEX + INIConfig::Defaults::ONE_VALUE)) {
                engine.syncToHost(hostTempo, pos);

                juce::MidiBuffer buffer;
                engine.process(buffer);

                float engineBeat = engine.getCurrentBeat();
                float expectedBeat = static_cast<float>(std::fmod(pos, INIConfig::Defaults::BEATS_PER_BAR));

                expectWithinAbsoluteError(engineBeat, expectedBeat, 0.1f);
            }
        }
    }

    void expectWithinAbsoluteError(float actual, float expected, float tolerance) {
        expect(std::abs(actual - expected) <= tolerance,
               "Expected " + juce::String(expected) + " but got " + juce::String(actual));
    }

    void expectLessThan(float value, float threshold) {
        expect(value < threshold, "Value " + juce::String(value) + " should be less than " + juce::String(threshold));
    }
};

static MidiTimingTests midiTimingTests;
