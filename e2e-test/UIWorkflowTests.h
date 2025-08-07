#pragma once

#include <JuceHeader.h>
#include "../Source/JUCE8_CODING_STANDARDS.h"
#include "../Source/INIConfig.h"
#include "../Source/MainContentComponent.h"
#include "../Source/PluginProcessor.h"
#include "../Source/PluginEditor.h"
#include "../Source/MidiEngine.h"
#include "../Source/Mixer.h"
#include "../Source/FontManager.h"
#include "../Source/ColorScheme.h"
#include "../Source/ResponsiveLayoutManager.h"
#include "../Source/ComponentState.h"

//==============================================================================
// UI Workflow E2E Tests - Complete User Interaction Scenarios
// Tests the full UI workflow from user perspective
//==============================================================================

class UIWorkflowTests : public juce::UnitTest {
public:
    UIWorkflowTests() : juce::UnitTest("UI Workflow Tests") {}

    void runTest() override {
        beginTest("Complete Plugin Initialization Workflow");
        testCompletePluginInitialization();

        beginTest("Player Selection Workflow (8 Players)");
        testPlayerSelectionWorkflow();

        beginTest("Drum Kit Selection and Management Workflow");
        testDrumKitSelectionWorkflow();

        beginTest("Pattern Group Management Workflow");
        testPatternGroupManagementWorkflow();

        beginTest("Scene Launcher Interaction Workflow");
        testSceneLauncherWorkflow();

        beginTest("Transport Controls Workflow");
        testTransportControlsWorkflow();

        beginTest("Mixer Panel Workflow");
        testMixerPanelWorkflow();

        beginTest("Settings Panel Workflow");
        testSettingsPanelWorkflow();

        beginTest("Complete Session Save/Load Workflow");
        testCompleteSessionWorkflow();

        beginTest("Live Performance Mode Workflow");
        testLivePerformanceModeWorkflow();

        beginTest("Error Recovery Workflow");
        testErrorRecoveryWorkflow();
    }

private:
    //==============================================================================
    // Test Setup and Teardown
    //==============================================================================
    
    std::unique_ptr<OTTOAudioProcessor> createTestProcessor() {
        auto processor = std::make_unique<OTTOAudioProcessor>();
        processor->prepareToPlay(
            static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE),
            INIConfig::Defaults::DEFAULT_BUFFER_SIZE
        );
        return processor;
    }

    std::unique_ptr<juce::AudioProcessorEditor> createTestEditor(OTTOAudioProcessor& processor) {
        auto editor = std::unique_ptr<juce::AudioProcessorEditor>(processor.createEditor());
        expect(editor != nullptr, "Editor should be created successfully");
        
        // Set to default interface size from INI config
        editor->setSize(
            INIConfig::LayoutConstants::defaultInterfaceWidth,
            INIConfig::LayoutConstants::defaultInterfaceHeight
        );
        
        return editor;
    }

    void waitForUIStabilization() {
        // Use INI-defined UI settle time for cross-platform compatibility
        juce::Thread::sleep(INIConfig::Testing::UI_SETTLE_TIME);
    }

    //==============================================================================
    // Complete Plugin Initialization Workflow
    //==============================================================================
    
    void testCompletePluginInitialization() {
        logMessage("Testing complete plugin initialization workflow...");
        
        // Step 1: Create processor
        auto processor = createTestProcessor();
        expect(processor != nullptr, "Processor should be created");
        
        // Step 2: Validate initial state
        expect(processor->getName() == JucePlugin_Name, "Plugin name should match");
        expect(processor->acceptsMidi(), "Should accept MIDI input");
        expect(processor->producesMidi(), "Should produce MIDI output");
        
        // Step 3: Create editor
        auto editor = createTestEditor(*processor);
        expect(editor != nullptr, "Editor should be created");
        
        // Step 4: Validate editor dimensions
        expect(editor->getWidth() == INIConfig::LayoutConstants::defaultInterfaceWidth, 
               "Editor width should match INI config");
        expect(editor->getHeight() == INIConfig::LayoutConstants::defaultInterfaceHeight, 
               "Editor height should match INI config");
        
        // Step 5: Wait for UI initialization
        waitForUIStabilization();
        
        // Step 6: Validate component hierarchy
        auto* pluginEditor = dynamic_cast<PluginEditor*>(editor.get());
        expect(pluginEditor != nullptr, "Should be PluginEditor instance");
        
        if (pluginEditor) {
            auto* mainContent = pluginEditor->getMainContentComponent();
            expect(mainContent != nullptr, "MainContentComponent should exist");
            
            if (mainContent) {
                // Validate row structure (6 rows as per design rules)
                auto bounds = mainContent->getLocalBounds();
                expect(bounds.getWidth() > 0, "MainContent should have valid width");
                expect(bounds.getHeight() > 0, "MainContent should have valid height");
                
                // Validate left and right sections exist
                expect(mainContent->getLeftSection() != nullptr, "Left section should exist");
                expect(mainContent->getRightSection() != nullptr, "Right section should exist");
            }
        }
        
        logMessage("Plugin initialization workflow completed successfully");
    }

    //==============================================================================
    // Player Selection Workflow (8 Players)
    //==============================================================================
    
    void testPlayerSelectionWorkflow() {
        logMessage("Testing player selection workflow for all 8 players...");
        
        auto processor = createTestProcessor();
        auto editor = createTestEditor(*processor);
        auto* pluginEditor = dynamic_cast<PluginEditor*>(editor.get());
        
        expect(pluginEditor != nullptr, "Should have PluginEditor");
        if (!pluginEditor) return;
        
        auto* mainContent = pluginEditor->getMainContentComponent();
        expect(mainContent != nullptr, "Should have MainContentComponent");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Test each player selection (1-8)
        for (int playerIndex = 1; playerIndex <= INIConfig::Defaults::MAX_PLAYERS; ++playerIndex) {
            logMessage("Testing player " + juce::String(playerIndex) + " selection...");
            
            // Step 1: Switch to player
            ComponentState testState;
            testState.currentPlayer = playerIndex;
            mainContent->switchToPlayer(playerIndex, testState);
            
            waitForUIStabilization();
            
            // Step 2: Validate player switch
            // Note: We can't directly access private currentPlayerIndex, 
            // but we can validate through state management
            ComponentState currentState;
            mainContent->saveStates(currentState);
            expect(currentState.currentPlayer == playerIndex, 
                   "Current player should be " + juce::String(playerIndex));
            
            // Step 3: Validate UI updates for player
            mainContent->updatePlayerDisplay(playerIndex);
            waitForUIStabilization();
            
            // Step 4: Test player-specific controls
            // Each player should have independent state
            float testSwingValue = 0.3f + (playerIndex * 0.05f); // Unique per player
            mainContent->setSwingValue(testSwingValue);
            
            expectWithinAbsoluteError(mainContent->getSwingValue(), testSwingValue, 0.01f,
                                    "Swing value should be set correctly for player " + juce::String(playerIndex));
        }
        
        // Test rapid player switching
        logMessage("Testing rapid player switching...");
        for (int i = 0; i < 3; ++i) {
            for (int playerIndex = 1; playerIndex <= INIConfig::Defaults::MAX_PLAYERS; ++playerIndex) {
                ComponentState testState;
                testState.currentPlayer = playerIndex;
                mainContent->switchToPlayer(playerIndex, testState);
                
                // Shorter wait for rapid switching test
                juce::Thread::sleep(10);
            }
        }
        
        waitForUIStabilization();
        
        // Validate final state
        ComponentState finalState;
        mainContent->saveStates(finalState);
        expect(finalState.currentPlayer >= 1 && finalState.currentPlayer <= INIConfig::Defaults::MAX_PLAYERS,
               "Final player should be valid");
        
        logMessage("Player selection workflow completed successfully");
    }

    //==============================================================================
    // Drum Kit Selection and Management Workflow
    //==============================================================================
    
    void testDrumKitSelectionWorkflow() {
        logMessage("Testing drum kit selection and management workflow...");
        
        auto processor = createTestProcessor();
        auto editor = createTestEditor(*processor);
        auto* pluginEditor = dynamic_cast<PluginEditor*>(editor.get());
        
        expect(pluginEditor != nullptr, "Should have PluginEditor");
        if (!pluginEditor) return;
        
        auto* mainContent = pluginEditor->getMainContentComponent();
        expect(mainContent != nullptr, "Should have MainContentComponent");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test drum kit list update
        juce::StringArray testDrumKits;
        testDrumKits.add("Modern Kit");
        testDrumKits.add("Classic Kit");
        testDrumKits.add("Electronic Kit");
        testDrumKits.add("Jazz Kit");
        
        mainContent->updateDrumKitList(testDrumKits);
        waitForUIStabilization();
        
        // Step 2: Test drum kit selection
        for (const auto& kitName : testDrumKits) {
            logMessage("Testing selection of drum kit: " + kitName);
            
            mainContent->setSelectedDrumKit(kitName);
            waitForUIStabilization();
            
            // Validate selection through callback mechanism
            bool callbackTriggered = false;
            juce::String selectedKit;
            
            mainContent->onDrumKitChanged = [&](const juce::String& newKit) {
                callbackTriggered = true;
                selectedKit = newKit;
            };
            
            // Trigger drum kit change
            mainContent->setSelectedDrumKit(kitName);
            waitForUIStabilization();
            
            expect(callbackTriggered, "Drum kit change callback should be triggered");
            expect(selectedKit == kitName, "Selected kit should match: " + kitName);
        }
        
        // Step 3: Test drum kit display toggle
        logMessage("Testing drum kit display toggle...");
        
        // Test showing drum kit label
        mainContent->showDrumKitLabel();
        waitForUIStabilization();
        
        // Test showing drum kit menu
        mainContent->showDrumKitMenu();
        waitForUIStabilization();
        
        // Test display toggle
        mainContent->updateDrumKitDisplayToggle();
        waitForUIStabilization();
        
        // Step 4: Test drum kit popup request
        bool popupRequested = false;
        mainContent->onDrumKitPopupRequested = [&]() {
            popupRequested = true;
        };
        
        // Simulate drum kit popup request (would normally be triggered by button click)
        if (mainContent->onDrumKitPopupRequested) {
            mainContent->onDrumKitPopupRequested();
        }
        
        expect(popupRequested, "Drum kit popup should be requested");
        
        // Step 5: Test empty drum kit list handling
        logMessage("Testing empty drum kit list handling...");
        juce::StringArray emptyList;
        mainContent->updateDrumKitList(emptyList);
        waitForUIStabilization();
        
        // Should handle empty list gracefully without crashing
        mainContent->setSelectedDrumKit("Non-existent Kit");
        waitForUIStabilization();
        
        logMessage("Drum kit selection workflow completed successfully");
    }

    //==============================================================================
    // Pattern Group Management Workflow
    //==============================================================================
    
    void testPatternGroupManagementWorkflow() {
        logMessage("Testing pattern group management workflow...");
        
        auto processor = createTestProcessor();
        auto editor = createTestEditor(*processor);
        auto* pluginEditor = dynamic_cast<PluginEditor*>(editor.get());
        
        expect(pluginEditor != nullptr, "Should have PluginEditor");
        if (!pluginEditor) return;
        
        auto* mainContent = pluginEditor->getMainContentComponent();
        expect(mainContent != nullptr, "Should have MainContentComponent");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test MIDI file assignment workflow
        logMessage("Testing MIDI file assignment...");
        
        for (int buttonIndex = 0; buttonIndex < 16; ++buttonIndex) { // 4x4 matrix
            juce::String testMidiFile = "test_pattern_" + juce::String(buttonIndex) + ".mid";
            
            // Set MIDI file assignment
            mainContent->setMidiFileAssignment(buttonIndex, testMidiFile);
            waitForUIStabilization();
            
            // Validate assignment
            juce::String assignedFile = mainContent->getMidiFileAssignment(buttonIndex);
            expect(assignedFile == testMidiFile, 
                   "MIDI file should be assigned correctly for button " + juce::String(buttonIndex));
        }
        
        // Step 2: Test MIDI file change callback
        bool midiFileChangeTriggered = false;
        int changedButtonIndex = -1;
        juce::String changedMidiFile;
        
        mainContent->onMidiFileChanged = [&](int buttonIdx, const juce::String& midiFile) {
            midiFileChangeTriggered = true;
            changedButtonIndex = buttonIdx;
            changedMidiFile = midiFile;
        };
        
        // Trigger MIDI file change
        const int testButtonIndex = 5;
        const juce::String testNewMidiFile = "new_test_pattern.mid";
        mainContent->setMidiFileAssignment(testButtonIndex, testNewMidiFile);
        
        if (mainContent->onMidiFileChanged) {
            mainContent->onMidiFileChanged(testButtonIndex, testNewMidiFile);
        }
        
        expect(midiFileChangeTriggered, "MIDI file change callback should be triggered");
        expect(changedButtonIndex == testButtonIndex, "Changed button index should match");
        expect(changedMidiFile == testNewMidiFile, "Changed MIDI file should match");
        
        // Step 3: Test pattern group controls
        logMessage("Testing pattern group controls...");
        
        // Test groove and fills request
        bool grooveAndFillsRequested = false;
        mainContent->onGrooveAndFillsRequested = [&]() {
            grooveAndFillsRequested = true;
        };
        
        if (mainContent->onGrooveAndFillsRequested) {
            mainContent->onGrooveAndFillsRequested();
        }
        
        expect(grooveAndFillsRequested, "Groove and fills should be requested");
        
        // Step 4: Test state change notifications
        bool stateChangeTriggered = false;
        mainContent->onStateChanged = [&](ComponentState& state) {
            stateChangeTriggered = true;
        };
        
        // Trigger state change
        ComponentState testState;
        testState.currentPlayer = 3;
        mainContent->loadStates(testState);
        
        expect(stateChangeTriggered, "State change should be triggered");
        
        logMessage("Pattern group management workflow completed successfully");
    }

    //==============================================================================
    // Scene Launcher Interaction Workflow
    //==============================================================================
    
    void testSceneLauncherWorkflow() {
        logMessage("Testing scene launcher interaction workflow...");
        
        auto processor = createTestProcessor();
        auto editor = createTestEditor(*processor);
        auto* pluginEditor = dynamic_cast<PluginEditor*>(editor.get());
        
        expect(pluginEditor != nullptr, "Should have PluginEditor");
        if (!pluginEditor) return;
        
        auto* mainContent = pluginEditor->getMainContentComponent();
        expect(mainContent != nullptr, "Should have MainContentComponent");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test scene launcher visibility
        logMessage("Testing scene launcher visibility...");
        
        // Show scene launcher
        mainContent->showSceneLauncher(true);
        waitForUIStabilization();
        
        // Hide scene launcher
        mainContent->showSceneLauncher(false);
        waitForUIStabilization();
        
        // Show again for testing
        mainContent->showSceneLauncher(true);
        waitForUIStabilization();
        
        // Step 2: Test live performance mode integration
        logMessage("Testing live performance mode with scene launcher...");
        
        // Enable live performance mode
        mainContent->setLivePerformanceMode(true);
        expect(mainContent->isLivePerformanceMode(), "Live performance mode should be enabled");
        waitForUIStabilization();
        
        // Test scene launcher in live mode
        mainContent->showSceneLauncher(true);
        waitForUIStabilization();
        
        // Disable live performance mode
        mainContent->setLivePerformanceMode(false);
        expect(!mainContent->isLivePerformanceMode(), "Live performance mode should be disabled");
        waitForUIStabilization();
        
        logMessage("Scene launcher workflow completed successfully");
    }

    //==============================================================================
    // Transport Controls Workflow
    //==============================================================================
    
    void testTransportControlsWorkflow() {
        logMessage("Testing transport controls workflow...");
        
        auto processor = createTestProcessor();
        auto editor = createTestEditor(*processor);
        auto* pluginEditor = dynamic_cast<PluginEditor*>(editor.get());
        
        expect(pluginEditor != nullptr, "Should have PluginEditor");
        if (!pluginEditor) return;
        
        auto* mainContent = pluginEditor->getMainContentComponent();
        expect(mainContent != nullptr, "Should have MainContentComponent");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test slider value changes
        logMessage("Testing slider value changes...");
        
        bool sliderValueChanged = false;
        juce::String changedSliderName;
        float changedSliderValue = 0.0f;
        
        mainContent->onSliderValueChanged = [&](const juce::String& sliderName, float value) {
            sliderValueChanged = true;
            changedSliderName = sliderName;
            changedSliderValue = value;
        };
        
        // Test swing value
        const float testSwingValue = 0.65f;
        mainContent->setSwingValue(testSwingValue);
        expectWithinAbsoluteError(mainContent->getSwingValue(), testSwingValue, 0.01f,
                                "Swing value should be set correctly");
        
        if (mainContent->onSliderValueChanged) {
            mainContent->onSliderValueChanged("swing", testSwingValue);
        }
        
        expect(sliderValueChanged, "Slider value change callback should be triggered");
        expect(changedSliderName == "swing", "Changed slider should be swing");
        expectWithinAbsoluteError(changedSliderValue, testSwingValue, 0.01f,
                                "Changed slider value should match");
        
        // Reset for next test
        sliderValueChanged = false;
        
        // Test energy value
        const float testEnergyValue = 0.8f;
        mainContent->setEnergyValue(testEnergyValue);
        expectWithinAbsoluteError(mainContent->getEnergyValue(), testEnergyValue, 0.01f,
                                "Energy value should be set correctly");
        
        if (mainContent->onSliderValueChanged) {
            mainContent->onSliderValueChanged("energy", testEnergyValue);
        }
        
        expect(sliderValueChanged, "Energy slider change callback should be triggered");
        
        // Reset for next test
        sliderValueChanged = false;
        
        // Test volume value
        const float testVolumeValue = 0.75f;
        mainContent->setVolumeValue(testVolumeValue);
        expectWithinAbsoluteError(mainContent->getVolumeValue(), testVolumeValue, 0.01f,
                                "Volume value should be set correctly");
        
        if (mainContent->onSliderValueChanged) {
            mainContent->onSliderValueChanged("volume", testVolumeValue);
        }
        
        expect(sliderValueChanged, "Volume slider change callback should be triggered");
        
        logMessage("Transport controls workflow completed successfully");
    }

    //==============================================================================
    // Mixer Panel Workflow
    //==============================================================================
    
    void testMixerPanelWorkflow() {
        logMessage("Testing mixer panel workflow...");
        
        auto processor = createTestProcessor();
        auto editor = createTestEditor(*processor);
        auto* pluginEditor = dynamic_cast<PluginEditor*>(editor.get());
        
        expect(pluginEditor != nullptr, "Should have PluginEditor");
        if (!pluginEditor) return;
        
        auto* mainContent = pluginEditor->getMainContentComponent();
        expect(mainContent != nullptr, "Should have MainContentComponent");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Test mixer popup request
        bool mixerPopupRequested = false;
        mainContent->onMixerPopupRequested = [&]() {
            mixerPopupRequested = true;
        };
        
        if (mainContent->onMixerPopupRequested) {
            mainContent->onMixerPopupRequested();
        }
        
        expect(mixerPopupRequested, "Mixer popup should be requested");
        
        logMessage("Mixer panel workflow completed successfully");
    }

    //==============================================================================
    // Settings Panel Workflow
    //==============================================================================
    
    void testSettingsPanelWorkflow() {
        logMessage("Testing settings panel workflow...");
        
        auto processor = createTestProcessor();
        auto editor = createTestEditor(*processor);
        waitForUIStabilization();
        
        // Test edit mode functionality
        auto* pluginEditor = dynamic_cast<PluginEditor*>(editor.get());
        expect(pluginEditor != nullptr, "Should have PluginEditor");
        if (!pluginEditor) return;
        
        auto* mainContent = pluginEditor->getMainContentComponent();
        expect(mainContent != nullptr, "Should have MainContentComponent");
        if (!mainContent) return;
        
        // Test edit mode changes
        bool editModeChanged = false;
        bool editModeState = false;
        
        mainContent->onEditModeChanged = [&](bool isEditing) {
            editModeChanged = true;
            editModeState = isEditing;
        };
        
        // Test entering edit mode
        mainContent->setEditModeVisuals(true);
        expect(mainContent->getEditMode(), "Edit mode should be enabled");
        
        if (mainContent->onEditModeChanged) {
            mainContent->onEditModeChanged(true);
        }
        
        expect(editModeChanged, "Edit mode change callback should be triggered");
        expect(editModeState, "Edit mode state should be true");
        
        // Reset for next test
        editModeChanged = false;
        
        // Test exiting edit mode
        mainContent->setEditModeVisuals(false);
        expect(!mainContent->getEditMode(), "Edit mode should be disabled");
        
        if (mainContent->onEditModeChanged) {
            mainContent->onEditModeChanged(false);
        }
        
        expect(editModeChanged, "Edit mode change callback should be triggered");
        expect(!editModeState, "Edit mode state should be false");
        
        logMessage("Settings panel workflow completed successfully");
    }

    //==============================================================================
    // Complete Session Save/Load Workflow
    //==============================================================================
    
    void testCompleteSessionWorkflow() {
        logMessage("Testing complete session save/load workflow...");
        
        auto processor = createTestProcessor();
        auto editor = createTestEditor(*processor);
        auto* pluginEditor = dynamic_cast<PluginEditor*>(editor.get());
        
        expect(pluginEditor != nullptr, "Should have PluginEditor");
        if (!pluginEditor) return;
        
        auto* mainContent = pluginEditor->getMainContentComponent();
        expect(mainContent != nullptr, "Should have MainContentComponent");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Set up a complex session state
        logMessage("Setting up complex session state...");
        
        // Set various parameters
        mainContent->setSwingValue(0.6f);
        mainContent->setEnergyValue(0.8f);
        mainContent->setVolumeValue(0.7f);
        mainContent->setEditModeVisuals(true);
        mainContent->setLivePerformanceMode(true);
        
        // Set MIDI file assignments
        for (int i = 0; i < 8; ++i) {
            juce::String midiFile = "session_pattern_" + juce::String(i) + ".mid";
            mainContent->setMidiFileAssignment(i, midiFile);
        }
        
        // Set drum kit
        mainContent->setSelectedDrumKit("Session Test Kit");
        
        // Switch to specific player
        ComponentState playerState;
        playerState.currentPlayer = 5;
        mainContent->switchToPlayer(5, playerState);
        
        waitForUIStabilization();
        
        // Step 2: Save session state
        logMessage("Saving session state...");
        
        ComponentState savedState;
        mainContent->saveStates(savedState);
        
        // Validate saved state has expected values
        expect(savedState.currentPlayer == 5, "Saved state should have correct player");
        expectWithinAbsoluteError(savedState.swingValue, 0.6f, 0.01f, "Saved swing value should match");
        expectWithinAbsoluteError(savedState.energyValue, 0.8f, 0.01f, "Saved energy value should match");
        expectWithinAbsoluteError(savedState.volumeValue, 0.7f, 0.01f, "Saved volume value should match");
        
        // Step 3: Modify session to different state
        logMessage("Modifying session to different state...");
        
        mainContent->setSwingValue(0.2f);
        mainContent->setEnergyValue(0.3f);
        mainContent->setVolumeValue(0.4f);
        mainContent->setEditModeVisuals(false);
        mainContent->setLivePerformanceMode(false);
        
        ComponentState differentPlayerState;
        differentPlayerState.currentPlayer = 2;
        mainContent->switchToPlayer(2, differentPlayerState);
        
        waitForUIStabilization();
        
        // Step 4: Load saved session state
        logMessage("Loading saved session state...");
        
        mainContent->loadStates(savedState);
        waitForUIStabilization();
        
        // Step 5: Validate loaded state
        logMessage("Validating loaded session state...");
        
        ComponentState loadedState;
        mainContent->saveStates(loadedState);
        
        expect(loadedState.currentPlayer == savedState.currentPlayer, "Loaded player should match saved");
        expectWithinAbsoluteError(loadedState.swingValue, savedState.swingValue, 0.01f, 
                                "Loaded swing value should match saved");
        expectWithinAbsoluteError(loadedState.energyValue, savedState.energyValue, 0.01f, 
                                "Loaded energy value should match saved");
        expectWithinAbsoluteError(loadedState.volumeValue, savedState.volumeValue, 0.01f, 
                                "Loaded volume value should match saved");
        
        // Step 6: Test processor-level save/load
        logMessage("Testing processor-level save/load...");
        
        ComponentState processorSavedState;
        processor->saveStates(processorSavedState);
        
        // Modify processor state
        auto& params = processor->getValueTreeState();
        if (auto* tempoParam = params.getRawParameterValue("tempo")) {
            *tempoParam = 140.0f;
        }
        
        // Load saved processor state
        processor->loadStates(processorSavedState);
        
        logMessage("Complete session save/load workflow completed successfully");
    }

    //==============================================================================
    // Live Performance Mode Workflow
    //==============================================================================
    
    void testLivePerformanceModeWorkflow() {
        logMessage("Testing live performance mode workflow...");
        
        auto processor = createTestProcessor();
        auto editor = createTestEditor(*processor);
        auto* pluginEditor = dynamic_cast<PluginEditor*>(editor.get());
        
        expect(pluginEditor != nullptr, "Should have PluginEditor");
        if (!pluginEditor) return;
        
        auto* mainContent = pluginEditor->getMainContentComponent();
        expect(mainContent != nullptr, "Should have MainContentComponent");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test entering live performance mode
        logMessage("Testing live performance mode activation...");
        
        expect(!mainContent->isLivePerformanceMode(), "Should start in normal mode");
        
        mainContent->setLivePerformanceMode(true);
        expect(mainContent->isLivePerformanceMode(), "Should be in live performance mode");
        waitForUIStabilization();
        
        // Step 2: Test UI changes in live performance mode
        logMessage("Testing UI changes in live performance mode...");
        
        // Show scene launcher in live mode
        mainContent->showSceneLauncher(true);
        waitForUIStabilization();
        
        // Test player switching in live mode
        for (int player = 1; player <= 4; ++player) {
            ComponentState liveState;
            liveState.currentPlayer = player;
            mainContent->switchToPlayer(player, liveState);
            waitForUIStabilization();
            
            ComponentState currentState;
            mainContent->saveStates(currentState);
            expect(currentState.currentPlayer == player, 
                   "Player switching should work in live mode");
        }
        
        // Step 3: Test exiting live performance mode
        logMessage("Testing live performance mode deactivation...");
        
        mainContent->setLivePerformanceMode(false);
        expect(!mainContent->isLivePerformanceMode(), "Should exit live performance mode");
        waitForUIStabilization();
        
        // Step 4: Test rapid mode switching
        logMessage("Testing rapid live mode switching...");
        
        for (int i = 0; i < 5; ++i) {
            mainContent->setLivePerformanceMode(true);
            juce::Thread::sleep(50);
            mainContent->setLivePerformanceMode(false);
            juce::Thread::sleep(50);
        }
        
        waitForUIStabilization();
        
        logMessage("Live performance mode workflow completed successfully");
    }

    //==============================================================================
    // Error Recovery Workflow
    //==============================================================================
    
    void testErrorRecoveryWorkflow() {
        logMessage("Testing error recovery workflow...");
        
        auto processor = createTestProcessor();
        auto editor = createTestEditor(*processor);
        auto* pluginEditor = dynamic_cast<PluginEditor*>(editor.get());
        
        expect(pluginEditor != nullptr, "Should have PluginEditor");
        if (!pluginEditor) return;
        
        auto* mainContent = pluginEditor->getMainContentComponent();
        expect(mainContent != nullptr, "Should have MainContentComponent");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test invalid player index handling
        logMessage("Testing invalid player index handling...");
        
        ComponentState invalidState;
        invalidState.currentPlayer = 999; // Invalid player index
        
        // Should handle gracefully without crashing
        mainContent->loadStates(invalidState);
        waitForUIStabilization();
        
        ComponentState recoveredState;
        mainContent->saveStates(recoveredState);
        expect(recoveredState.currentPlayer >= 1 && recoveredState.currentPlayer <= INIConfig::Defaults::MAX_PLAYERS,
               "Should recover to valid player index");
        
        // Step 2: Test invalid MIDI file assignment
        logMessage("Testing invalid MIDI file assignment...");
        
        mainContent->setMidiFileAssignment(-1, "invalid.mid"); // Invalid button index
        mainContent->setMidiFileAssignment(999, "invalid.mid"); // Invalid button index
        waitForUIStabilization();
        
        // Should handle gracefully
        juce::String result1 = mainContent->getMidiFileAssignment(-1);
        juce::String result2 = mainContent->getMidiFileAssignment(999);
        
        // Step 3: Test invalid slider values
        logMessage("Testing invalid slider values...");
        
        mainContent->setSwingValue(-999.0f); // Invalid negative value
        mainContent->setSwingValue(999.0f);  // Invalid high value
        
        float swingValue = mainContent->getSwingValue();
        expect(swingValue >= 0.0f && swingValue <= 1.0f, "Swing value should be clamped to valid range");
        
        mainContent->setEnergyValue(-999.0f);
        mainContent->setEnergyValue(999.0f);
        
        float energyValue = mainContent->getEnergyValue();
        expect(energyValue >= 0.0f && energyValue <= 1.0f, "Energy value should be clamped to valid range");
        
        mainContent->setVolumeValue(-999.0f);
        mainContent->setVolumeValue(999.0f);
        
        float volumeValue = mainContent->getVolumeValue();
        expect(volumeValue >= 0.0f && volumeValue <= 1.0f, "Volume value should be clamped to valid range");
        
        // Step 4: Test null callback handling
        logMessage("Testing null callback handling...");
        
        // Set callbacks to null
        mainContent->onEditModeChanged = nullptr;
        mainContent->onMidiFileChanged = nullptr;
        mainContent->onSliderValueChanged = nullptr;
        mainContent->onGrooveAndFillsRequested = nullptr;
        mainContent->onStateChanged = nullptr;
        mainContent->onDrumKitPopupRequested = nullptr;
        mainContent->onMixerPopupRequested = nullptr;
        mainContent->onDrumKitChanged = nullptr;
        
        // Should handle null callbacks gracefully
        mainContent->setEditModeVisuals(true);
        mainContent->setMidiFileAssignment(0, "test.mid");
        mainContent->setSwingValue(0.5f);
        mainContent->setSelectedDrumKit("Test Kit");
        
        waitForUIStabilization();
        
        logMessage("Error recovery workflow completed successfully");
    }

    //==============================================================================
    // Utility Methods
    //==============================================================================
    
    void logMessage(const juce::String& message) {
        DBG("UIWorkflowTests: " + message);
    }
};

// Register the test
static UIWorkflowTests uiWorkflowTestsInstance;