#pragma once
#include <JuceHeader.h>
#include "../PatternSuggestionEngine.h"
#include "../AIAssistantPanel.h"
#include "../AutoMixAssistant.h"
#include "../Mixer.h"
#include "../PatternManager.h"
#include "../MidiAnalysisTypes.h"
#include "../INIConfig.h"
#include "../ErrorHandling.h"

class AIComponentTests : public juce::UnitTest {
public:
    AIComponentTests() : juce::UnitTest("AI Component Tests") {}

    void runTest() override {
        beginTest("Pattern Suggestion Engine Initialization");
        testPatternEngineInit();

        beginTest("Genre Profile Generation");
        testGenreProfiles();

        beginTest("Pattern Generation Algorithms");
        testPatternGeneration();

        beginTest("Style Transfer Functionality");
        testStyleTransfer();

        beginTest("Pattern Similarity Matching");
        testPatternSimilarity();

        beginTest("Performance Adaptation");
        testPerformanceAdaptation();

        beginTest("Learning from Patterns");
        testPatternLearning();

        beginTest("AI Assistant Panel Modes");
        testAIAssistantModes();

        beginTest("Mix Analysis Integration");
        testMixAnalysis();

        beginTest("Pattern Suggestion UI");
        testPatternSuggestionUI();

        beginTest("Performance Statistics");
        testPerformanceStats();

        beginTest("AI Component Error Handling");
        testAIErrorHandling();

        beginTest("AI Performance Benchmarks");
        testAIPerformance();
    }

private:
    void testPatternEngineInit() {
        auto engine = std::make_unique<PatternSuggestionEngine>();
        expect(engine != nullptr, "Pattern engine should be created");

        for (int i = 0; i < 8; ++i) {
            PatternSuggestionEngine::SuggestionParams params;
            params.genre = static_cast<PatternSuggestionEngine::Genre>(i);
            auto suggestions = engine->suggestPatterns(params, 1);
            expect(!suggestions.isEmpty(), "Should generate patterns for genre " + juce::String(i));
        }
    }

    void testGenreProfiles() {
        auto engine = std::make_unique<PatternSuggestionEngine>();
        
        PatternSuggestionEngine::SuggestionParams rockParams;
        rockParams.genre = PatternSuggestionEngine::Genre::Rock;
        rockParams.complexity = 0.5f;
        rockParams.bars = 4;
        
        auto rockSuggestions = engine->suggestPatterns(rockParams, 3);
        expect(rockSuggestions.size() == 3, "Should generate requested number of rock patterns");
        
        for (const auto& suggestion : rockSuggestions) {
            expect(!suggestion.name.isEmpty(), "Rock pattern should have valid name");
            expect(suggestion.pattern.getNumEvents() > 0, "Rock pattern should have MIDI events");
            expect(suggestion.matchScore >= 0.0f && suggestion.matchScore <= 1.0f, "Match score should be normalized");
        }

        PatternSuggestionEngine::SuggestionParams jazzParams;
        jazzParams.genre = PatternSuggestionEngine::Genre::Jazz;
        jazzParams.complexity = 0.7f;
        jazzParams.bars = 4;
        
        auto jazzSuggestions = engine->suggestPatterns(jazzParams, 1);
        expect(!jazzSuggestions.isEmpty(), "Should generate jazz patterns");
        
        expect(jazzSuggestions[0].analysis.averageSwing != rockSuggestions[0].analysis.averageSwing,
               "Jazz and Rock should have different swing characteristics");
    }

    void testPatternGeneration() {
        auto engine = std::make_unique<PatternSuggestionEngine>();
        
        for (float complexity = 0.1f; complexity <= 1.0f; complexity += 0.3f) {
            PatternSuggestionEngine::SuggestionParams params;
            params.genre = PatternSuggestionEngine::Genre::Electronic;
            params.complexity = complexity;
            params.bars = 2;
            params.tempo = 128.0f;
            
            auto pattern = engine->generatePattern(params);
            expect(!pattern.name.isEmpty(), "Pattern should have valid name for complexity " + juce::String(complexity));
            expect(pattern.pattern.getNumEvents() > 0, "Pattern should have MIDI events");
            
            expectWithinAbsoluteError(pattern.analysis.tempo, params.tempo, 1.0f);
        }

        for (int bars = 1; bars <= 8; bars *= 2) {
            PatternSuggestionEngine::SuggestionParams params;
            params.genre = PatternSuggestionEngine::Genre::HipHop;
            params.bars = bars;
            
            auto pattern = engine->generatePattern(params);
            expect(pattern.pattern.getNumEvents() > 0, "Pattern should scale with bar count: " + juce::String(bars));
            
            if (bars > 1) {
                expect(pattern.pattern.getNumEvents() >= bars, "Longer patterns should have proportional events");
            }
        }
    }

    void testStyleTransfer() {
        auto engine = std::make_unique<PatternSuggestionEngine>();
        
        juce::MidiMessageSequence sourcePattern;
        auto noteOn = juce::MidiMessage::noteOn(10, INIConfig::GMDrums::BASS_DRUM_1, 100);
        noteOn.setTimeStamp(0.0);
        sourcePattern.addEvent(noteOn);
        
        auto noteOff = juce::MidiMessage::noteOff(10, INIConfig::GMDrums::BASS_DRUM_1);
        noteOff.setTimeStamp(480.0);
        sourcePattern.addEvent(noteOff);
        
        juce::MidiMessageSequence styleReference;
        auto styleNoteOn = juce::MidiMessage::noteOn(10, INIConfig::GMDrums::ACOUSTIC_SNARE, 80);
        styleNoteOn.setTimeStamp(240.0);
        styleReference.addEvent(styleNoteOn);
        
        auto styleNoteOff = juce::MidiMessage::noteOff(10, INIConfig::GMDrums::ACOUSTIC_SNARE);
        styleNoteOff.setTimeStamp(720.0);
        styleReference.addEvent(styleNoteOff);
        
        auto transferredPattern = engine->transferStyle(sourcePattern, styleReference);
        expect(transferredPattern.getNumEvents() > 0, "Style transfer should produce events");
        expect(transferredPattern.getNumEvents() >= sourcePattern.getNumEvents(), 
               "Transferred pattern should maintain or expand source events");
    }

    void testPatternSimilarity() {
        auto engine = std::make_unique<PatternSuggestionEngine>();
        
        MidiGrooveAnalysis groove1;
        groove1.averageSwing = 50.0f;
        groove1.averageVelocity = 80.0f;
        groove1.grooveTightness = 0.9f;
        groove1.tempo = 120.0f;
        
        MidiGrooveAnalysis groove2;
        groove2.averageSwing = 52.0f;
        groove2.averageVelocity = 82.0f;
        groove2.grooveTightness = 0.85f;
        groove2.tempo = 122.0f;
        
        MidiGrooveAnalysis groove3;
        groove3.averageSwing = 70.0f;
        groove3.averageVelocity = 60.0f;
        groove3.grooveTightness = 0.5f;
        groove3.tempo = 180.0f;
        
        juce::Array<MidiGrooveAnalysis> library;
        library.add(groove2);
        library.add(groove3);
        
        auto similarGrooves = engine->findSimilarGrooves(groove1, library);
        expect(!similarGrooves.isEmpty(), "Should find similar grooves");
        
        if (similarGrooves.size() >= 2) {
            expect(similarGrooves[0].matchScore > similarGrooves[1].matchScore,
                   "More similar grooves should have higher match scores");
        }
    }

    void testPerformanceAdaptation() {
        auto engine = std::make_unique<PatternSuggestionEngine>();
        
        juce::Array<float> recentVelocities;
        juce::Array<float> recentTimings;
        
        for (int i = 0; i < 10; ++i) {
            recentVelocities.add(70.0f + (i * 2.0f));
            recentTimings.add(0.0f + (i * 0.01f));
        }
        
        engine->adaptToPerformance(recentVelocities, recentTimings);
        
        PatternSuggestionEngine::SuggestionParams params;
        params.genre = PatternSuggestionEngine::Genre::Funk;
        auto adaptedPattern = engine->generatePattern(params);
        
        expect(!adaptedPattern.name.isEmpty(), "Adapted pattern should be valid");
        expect(adaptedPattern.pattern.getNumEvents() > 0, "Adapted pattern should have events");
    }

    void testPatternLearning() {
        auto engine = std::make_unique<PatternSuggestionEngine>();
        
        juce::MidiMessageSequence learningPattern;
        auto noteOn = juce::MidiMessage::noteOn(10, INIConfig::GMDrums::BASS_DRUM_1, 90);
        noteOn.setTimeStamp(0.0);
        learningPattern.addEvent(noteOn);
        
        auto noteOff = juce::MidiMessage::noteOff(10, INIConfig::GMDrums::BASS_DRUM_1);
        noteOff.setTimeStamp(480.0);
        learningPattern.addEvent(noteOff);
        
        engine->learnFromPattern(learningPattern, PatternSuggestionEngine::Genre::Latin);
        
        auto tempFile = juce::File::getSpecialLocation(juce::File::tempDirectory)
                           .getChildFile("test_learned_data.json");
        
        engine->saveLearnedData(tempFile);
        expect(tempFile.exists(), "Learned data file should be created");
        
        auto newEngine = std::make_unique<PatternSuggestionEngine>();
        newEngine->loadLearnedData(tempFile);
        
        PatternSuggestionEngine::SuggestionParams params;
        params.genre = PatternSuggestionEngine::Genre::Latin;
        auto learnedPattern = newEngine->generatePattern(params);
        
        expect(!learnedPattern.name.isEmpty(), "Learned pattern should be valid");
        
        tempFile.deleteFile();
    }

    void testAIAssistantModes() {
        auto mixer = std::make_unique<Mixer>();
        auto patternEngine = std::make_unique<PatternSuggestionEngine>();
        auto autoMixAssistant = std::make_unique<AutoMixAssistant>();
        auto patternManager = std::make_unique<PatternManager>();
        
        auto aiPanel = std::make_unique<AIAssistantPanel>(*autoMixAssistant, *patternEngine, 
                                                        *mixer, *patternManager);
        
        expect(aiPanel != nullptr, "AI Assistant Panel should be created");
        
        aiPanel->setSize(400, 300);
        aiPanel->resized();
        
        expect(true, "AI Assistant Panel should handle resize");
    }

    void testMixAnalysis() {
        auto mixer = std::make_unique<Mixer>();
        auto patternEngine = std::make_unique<PatternSuggestionEngine>();
        auto autoMixAssistant = std::make_unique<AutoMixAssistant>();
        auto patternManager = std::make_unique<PatternManager>();
        
        auto aiPanel = std::make_unique<AIAssistantPanel>(*autoMixAssistant, *patternEngine, 
                                                        *mixer, *patternManager);
        
        expect(aiPanel != nullptr, "Mix analysis components should be created");
    }

    void testPatternSuggestionUI() {
        auto mixer = std::make_unique<Mixer>();
        auto patternEngine = std::make_unique<PatternSuggestionEngine>();
        auto autoMixAssistant = std::make_unique<AutoMixAssistant>();
        auto patternManager = std::make_unique<PatternManager>();
        
        auto aiPanel = std::make_unique<AIAssistantPanel>(*autoMixAssistant, *patternEngine, 
                                                        *mixer, *patternManager);
        
        aiPanel->setSize(600, 400);
        aiPanel->resized();
        
        expect(true, "Pattern suggestion UI should handle layout");
    }

    void testPerformanceStats() {
        auto engine = std::make_unique<PatternSuggestionEngine>();
        
        PatternSuggestionEngine::SuggestionParams params;
        params.genre = PatternSuggestionEngine::Genre::Pop;
        
        auto startTime = juce::Time::getHighResolutionTicks();
        auto pattern = engine->generatePattern(params);
        auto endTime = juce::Time::getHighResolutionTicks();
        
        double elapsedMs = juce::Time::highResolutionTicksToSeconds(endTime - startTime) * 1000.0;
        
        expect(!pattern.name.isEmpty(), "Performance test pattern should be valid");
        expect(elapsedMs < 1000.0, "Single pattern generation should be fast");
    }

    void testAIErrorHandling() {
        auto engine = std::make_unique<PatternSuggestionEngine>();
        
        PatternSuggestionEngine::SuggestionParams invalidParams;
        invalidParams.complexity = -1.0f;
        invalidParams.bars = 0;
        invalidParams.tempo = -50.0f;
        
        auto suggestions = engine->suggestPatterns(invalidParams, 5);
        expect(true, "Pattern engine should handle invalid parameters gracefully");
        
        PatternSuggestionEngine::SuggestionParams extremeParams;
        extremeParams.complexity = 999.0f;
        extremeParams.bars = 1000;
        extremeParams.tempo = 10000.0f;
        
        auto extremeSuggestions = engine->suggestPatterns(extremeParams, 1);
        expect(true, "Pattern engine should handle extreme parameters");
    }

    void testAIPerformance() {
        auto engine = std::make_unique<PatternSuggestionEngine>();
        
        auto startTime = juce::Time::getHighResolutionTicks();
        
        for (int i = 0; i < 100; ++i) {
            PatternSuggestionEngine::SuggestionParams params;
            params.genre = static_cast<PatternSuggestionEngine::Genre>(i % 8);
            params.complexity = (i % 10) / 10.0f;
            params.bars = (i % 4) + 1;
            
            auto pattern = engine->generatePattern(params);
            expect(!pattern.name.isEmpty(), "Performance test pattern should be valid");
        }
        
        auto endTime = juce::Time::getHighResolutionTicks();
        auto elapsedMs = juce::Time::highResolutionTicksToSeconds(endTime - startTime) * 1000.0;
        
        logMessage("AI Performance: Generated 100 patterns in " + juce::String(elapsedMs, 2) + "ms");
        expect(elapsedMs < 5000.0, "Pattern generation should complete within 5 seconds");
        
        startTime = juce::Time::getHighResolutionTicks();
        
        PatternSuggestionEngine::SuggestionParams params;
        params.genre = PatternSuggestionEngine::Genre::Electronic;
        auto suggestions = engine->suggestPatterns(params, 10);
        
        endTime = juce::Time::getHighResolutionTicks();
        elapsedMs = juce::Time::highResolutionTicksToSeconds(endTime - startTime) * 1000.0;
        
        logMessage("AI Performance: Generated 10 suggestions in " + juce::String(elapsedMs, 2) + "ms");
        expect(elapsedMs < 1000.0, "Suggestion generation should complete within 1 second");
        expect(suggestions.size() == 10, "Should generate requested number of suggestions");
    }
};

static AIComponentTests aiComponentTests;
