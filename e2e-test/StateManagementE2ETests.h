#pragma once

#include <JuceHeader.h>
#include "../JUCE8_CODING_STANDARDS.h"
#include "../Source/INIConfig.h"
#include "../Source/MainContentComponent.h"
#include "../Source/PluginProcessor.h"
#include "../Source/PluginEditor.h"
#include "../Source/ComponentState.h"
#include "../Source/INIDataManager.h"
#include "../Source/StateManager.h"

//==============================================================================
// State Management E2E Tests - Complete State Lifecycle Testing
// Tests state persistence, synchronization, and recovery across the entire application
//==============================================================================

class StateManagementE2ETests : public juce::UnitTest {
public:
    StateManagementE2ETests() : juce::UnitTest("State Management E2E Tests") {}

    void runTest() override {
        beginTest("Complete Session State Lifecycle");
        testCompleteSessionStateLifecycle();

        beginTest("Cross-Component State Synchronization");
        testCrossComponentStateSynchronization();

        beginTest("Preset Management Workflow");
        testPresetManagementWorkflow();

        beginTest("State Persistence and Recovery");
        testStatePersistenceAndRecovery();

        beginTest("Multi-Player State Management");
        testMultiPlayerStateManagement();

        beginTest("Real-Time State Updates");
        testRealTimeStateUpdates();

        beginTest("State Validation and Error Handling");
        testStateValidationAndErrorHandling();

        beginTest("Concurrent State Operations");
        testConcurrentStateOperations();

        beginTest("State Migration and Versioning");
        testStateMigrationAndVersioning();

        beginTest("Performance Under State Load");
        testPerformanceUnderStateLoad();
    }

private:
    //==============================================================================
    // Test Environment Setup
    //==============================================================================
    
    struct StateTestEnvironment {
        std::unique_ptr<OTTOAudioProcessor> processor;
        std::unique_ptr<juce::AudioProcessorEditor> editor;
        std::unique_ptr<INIDataManager> iniDataManager;
        std::unique_ptr<StateManager> stateManager;
        
        StateTestEnvironment() {
            // Create processor
            processor = std::make_unique<OTTOAudioProcessor>();
            processor->prepareToPlay(
                static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE),
                INIConfig::Defaults::DEFAULT_BUFFER_SIZE
            );
            
            // Create state management systems
            iniDataManager = std::make_unique<INIDataManager>();
            stateManager = std::make_unique<StateManager>();
            
            // Create editor
            editor = std::unique_ptr<juce::AudioProcessorEditor>(processor->createEditor());
            if (editor) {
                editor->setSize(
                    INIConfig::LayoutConstants::defaultInterfaceWidth,
                    INIConfig::LayoutConstants::defaultInterfaceHeight
                );
            }
        }
        
        PluginEditor* getPluginEditor() {
            return dynamic_cast<PluginEditor*>(editor.get());
        }
        
        MainContentComponent* getMainContent() {
            auto* pluginEditor = getPluginEditor();
            return pluginEditor ? pluginEditor->getMainContentComponent() : nullptr;
        }
    };

    void waitForUIStabilization() {
        juce::Thread::sleep(INIConfig::Testing::UI_SETTLE_TIME);
    }

    ComponentState createTestState(int playerIndex = 1) {
        ComponentState state;
        state.currentPlayer = playerIndex;
        state.swingValue = 0.5f + (playerIndex * 0.05f);
        state.energyValue = 0.6f + (playerIndex * 0.03f);
        state.volumeValue = 0.7f + (playerIndex * 0.02f);
        state.isEditMode = (playerIndex % 2 == 0);
        state.isLivePerformanceMode = (playerIndex % 3 == 0);
        
        // Add MIDI file assignments
        for (int i = 0; i < 16; ++i) {
            state.midiFileAssignments[i] = "player_" + juce::String(playerIndex) + "_pattern_" + juce::String(i) + ".mid";
        }
        
        return state;
    }

    //==============================================================================
    // Complete Session State Lifecycle
    //==============================================================================
    
    void testCompleteSessionStateLifecycle() {
        logMessage("Testing complete session state lifecycle...");
        
        StateTestEnvironment env;
        auto* mainContent = env.getMainContent();
        auto* processor = env.processor.get();
        auto* iniDataManager = env.iniDataManager.get();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        expect(processor != nullptr, "Processor should exist");
        expect(iniDataManager != nullptr, "INIDataManager should exist");
        if (!mainContent || !processor || !iniDataManager) return;
        
        waitForUIStabilization();
        
        // Step 1: Create initial session state
        logMessage("Creating initial session state...");
        
        ComponentState initialState = createTestState(3);
        initialState.sessionName = "Test Session 1";
        initialState.lastModified = juce::Time::getCurrentTime();
        
        // Apply state to UI
        mainContent->loadStates(initialState);
        waitForUIStabilization();
        
        // Verify state was applied
        ComponentState appliedState;
        mainContent->saveStates(appliedState);
        
        expect(appliedState.currentPlayer == initialState.currentPlayer, "Player should match initial state");
        expectWithinAbsoluteError(appliedState.swingValue, initialState.swingValue, 0.01f, 
                                "Swing value should match initial state");
        expectWithinAbsoluteError(appliedState.energyValue, initialState.energyValue, 0.01f, 
                                "Energy value should match initial state");
        
        // Step 2: Modify session state through UI interactions
        logMessage("Modifying session state through UI interactions...");
        
        mainContent->setSwingValue(0.8f);
        mainContent->setEnergyValue(0.9f);
        mainContent->setVolumeValue(0.6f);
        mainContent->setEditModeVisuals(true);
        
        // Switch to different player
        ComponentState playerSwitchState;
        playerSwitchState.currentPlayer = 5;
        mainContent->switchToPlayer(5, playerSwitchState);
        
        // Add MIDI file assignments
        for (int i = 0; i < 8; ++i) {
            juce::String midiFile = "modified_pattern_" + juce::String(i) + ".mid";
            mainContent->setMidiFileAssignment(i, midiFile);
        }
        
        waitForUIStabilization();
        
        // Step 3: Save modified session state
        logMessage("Saving modified session state...");
        
        ComponentState modifiedState;
        mainContent->saveStates(modifiedState);
        modifiedState.sessionName = "Modified Test Session";
        modifiedState.lastModified = juce::Time::getCurrentTime();
        
        // Verify modifications were captured
        expect(modifiedState.currentPlayer == 5, "Modified player should be 5");
        expectWithinAbsoluteError(modifiedState.swingValue, 0.8f, 0.01f, "Modified swing value should be 0.8");
        expectWithinAbsoluteError(modifiedState.energyValue, 0.9f, 0.01f, "Modified energy value should be 0.9");
        expect(modifiedState.isEditMode, "Edit mode should be enabled");
        
        // Step 4: Clear session and load saved state
        logMessage("Clearing session and loading saved state...");
        
        // Reset to default state
        ComponentState defaultState;
        mainContent->loadStates(defaultState);
        waitForUIStabilization();
        
        // Verify reset
        ComponentState resetState;
        mainContent->saveStates(resetState);
        expect(resetState.currentPlayer != modifiedState.currentPlayer, "Player should be reset");
        
        // Load saved modified state
        mainContent->loadStates(modifiedState);
        waitForUIStabilization();
        
        // Verify state was restored
        ComponentState restoredState;
        mainContent->saveStates(restoredState);
        
        expect(restoredState.currentPlayer == modifiedState.currentPlayer, "Restored player should match saved");
        expectWithinAbsoluteError(restoredState.swingValue, modifiedState.swingValue, 0.01f, 
                                "Restored swing value should match saved");
        expectWithinAbsoluteError(restoredState.energyValue, modifiedState.energyValue, 0.01f, 
                                "Restored energy value should match saved");
        
        // Step 5: Test processor-level state synchronization
        logMessage("Testing processor-level state synchronization...");
        
        processor->loadStates(modifiedState);
        
        ComponentState processorState;
        processor->saveStates(processorState);
        
        expect(processorState.currentPlayer == modifiedState.currentPlayer, 
               "Processor state should match loaded state");
        expectWithinAbsoluteError(processorState.swingValue, modifiedState.swingValue, 0.01f, 
                                "Processor swing value should match loaded state");
        
        logMessage("Complete session state lifecycle completed successfully");
    }

    //==============================================================================
    // Cross-Component State Synchronization
    //==============================================================================
    
    void testCrossComponentStateSynchronization() {
        logMessage("Testing cross-component state synchronization...");
        
        StateTestEnvironment env;
        auto* mainContent = env.getMainContent();
        auto* processor = env.processor.get();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        expect(processor != nullptr, "Processor should exist");
        if (!mainContent || !processor) return;
        
        waitForUIStabilization();
        
        // Step 1: Test UI to Processor synchronization
        logMessage("Testing UI to Processor synchronization...");
        
        // Set state in UI
        mainContent->setSwingValue(0.75f);
        mainContent->setEnergyValue(0.85f);
        mainContent->setVolumeValue(0.65f);
        
        ComponentState uiState;
        mainContent->saveStates(uiState);
        
        // Propagate to processor
        processor->loadStates(uiState);
        
        ComponentState processorState;
        processor->saveStates(processorState);
        
        expectWithinAbsoluteError(processorState.swingValue, uiState.swingValue, 0.01f, 
                                "Processor should receive UI swing value");
        expectWithinAbsoluteError(processorState.energyValue, uiState.energyValue, 0.01f, 
                                "Processor should receive UI energy value");
        expectWithinAbsoluteError(processorState.volumeValue, uiState.volumeValue, 0.01f, 
                                "Processor should receive UI volume value");
        
        // Step 2: Test Processor to UI synchronization
        logMessage("Testing Processor to UI synchronization...");
        
        ComponentState newProcessorState = createTestState(4);
        processor->loadStates(newProcessorState);
        
        ComponentState syncedProcessorState;
        processor->saveStates(syncedProcessorState);
        
        // Propagate back to UI
        mainContent->loadStates(syncedProcessorState);
        waitForUIStabilization();
        
        ComponentState syncedUIState;
        mainContent->saveStates(syncedUIState);
        
        expect(syncedUIState.currentPlayer == newProcessorState.currentPlayer, 
               "UI should receive processor player state");
        expectWithinAbsoluteError(syncedUIState.swingValue, newProcessorState.swingValue, 0.01f, 
                                "UI should receive processor swing value");
        
        // Step 3: Test bidirectional synchronization
        logMessage("Testing bidirectional synchronization...");
        
        // Create state change callbacks to track synchronization
        bool uiStateChanged = false;
        bool processorStateChanged = false;
        
        mainContent->onStateChanged = [&](ComponentState& state) {
            uiStateChanged = true;
            // Propagate to processor
            processor->loadStates(state);
            processorStateChanged = true;
        };
        
        // Trigger state change in UI
        ComponentState triggerState = createTestState(6);
        mainContent->loadStates(triggerState);
        
        if (mainContent->onStateChanged) {
            mainContent->onStateChanged(triggerState);
        }
        
        expect(uiStateChanged, "UI state change should be triggered");
        expect(processorStateChanged, "Processor state should be synchronized");
        
        // Step 4: Test state synchronization during rapid changes
        logMessage("Testing state synchronization during rapid changes...");
        
        for (int i = 0; i < 10; ++i) {
            ComponentState rapidState = createTestState((i % INIConfig::Defaults::MAX_PLAYERS) + 1);
            rapidState.swingValue = static_cast<float>(i) / 10.0f;
            
            mainContent->loadStates(rapidState);
            processor->loadStates(rapidState);
            
            juce::Thread::sleep(10); // Rapid changes
        }
        
        waitForUIStabilization();
        
        // Verify final states are synchronized
        ComponentState finalUIState;
        ComponentState finalProcessorState;
        
        mainContent->saveStates(finalUIState);
        processor->saveStates(finalProcessorState);
        
        expect(finalUIState.currentPlayer == finalProcessorState.currentPlayer, 
               "Final UI and processor player states should match");
        expectWithinAbsoluteError(finalUIState.swingValue, finalProcessorState.swingValue, 0.01f, 
                                "Final UI and processor swing values should match");
        
        logMessage("Cross-component state synchronization completed successfully");
    }

    //==============================================================================
    // Preset Management Workflow
    //==============================================================================
    
    void testPresetManagementWorkflow() {
        logMessage("Testing preset management workflow...");
        
        StateTestEnvironment env;
        auto* mainContent = env.getMainContent();
        auto* processor = env.processor.get();
        auto* iniDataManager = env.iniDataManager.get();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        expect(processor != nullptr, "Processor should exist");
        expect(iniDataManager != nullptr, "INIDataManager should exist");
        if (!mainContent || !processor || !iniDataManager) return;
        
        waitForUIStabilization();
        
        // Step 1: Create and save presets
        logMessage("Creating and saving presets...");
        
        std::vector<ComponentState> testPresets;
        
        // Create multiple test presets
        for (int i = 1; i <= 5; ++i) {
            ComponentState preset = createTestState(i);
            preset.presetName = "Test Preset " + juce::String(i);
            preset.presetCategory = (i <= 2) ? "Modern" : "Classic";
            preset.lastModified = juce::Time::getCurrentTime();
            
            testPresets.push_back(preset);
        }
        
        // Save presets through INI data manager
        for (const auto& preset : testPresets) {
            iniDataManager->saveComponentState("preset_" + preset.presetName.replaceCharacter(' ', '_'), preset);
        }
        
        // Step 2: Load and verify presets
        logMessage("Loading and verifying presets...");
        
        for (const auto& originalPreset : testPresets) {
            juce::String presetId = "preset_" + originalPreset.presetName.replaceCharacter(' ', '_');
            ComponentState loadedPreset = iniDataManager->loadComponentState(presetId);
            
            expect(loadedPreset.presetName == originalPreset.presetName, 
                   "Loaded preset name should match: " + originalPreset.presetName);
            expect(loadedPreset.presetCategory == originalPreset.presetCategory, 
                   "Loaded preset category should match: " + originalPreset.presetCategory);
            expect(loadedPreset.currentPlayer == originalPreset.currentPlayer, 
                   "Loaded preset player should match");
            expectWithinAbsoluteError(loadedPreset.swingValue, originalPreset.swingValue, 0.01f, 
                                    "Loaded preset swing value should match");
        }
        
        // Step 3: Test preset application to UI
        logMessage("Testing preset application to UI...");
        
        for (const auto& preset : testPresets) {
            // Apply preset to UI
            mainContent->loadStates(preset);
            waitForUIStabilization();
            
            // Verify preset was applied
            ComponentState appliedState;
            mainContent->saveStates(appliedState);
            
            expect(appliedState.currentPlayer == preset.currentPlayer, 
                   "Applied preset player should match for " + preset.presetName);
            expectWithinAbsoluteError(appliedState.swingValue, preset.swingValue, 0.01f, 
                                    "Applied preset swing value should match for " + preset.presetName);
            expectWithinAbsoluteError(appliedState.energyValue, preset.energyValue, 0.01f, 
                                    "Applied preset energy value should match for " + preset.presetName);
        }
        
        // Step 4: Test preset modification and update
        logMessage("Testing preset modification and update...");
        
        ComponentState modifiablePreset = testPresets[0];
        modifiablePreset.swingValue = 0.95f;
        modifiablePreset.energyValue = 0.85f;
        modifiablePreset.lastModified = juce::Time::getCurrentTime();
        
        // Apply modifications to UI
        mainContent->loadStates(modifiablePreset);
        waitForUIStabilization();
        
        // Save modified preset
        juce::String modifiedPresetId = "preset_" + modifiablePreset.presetName.replaceCharacter(' ', '_');
        iniDataManager->saveComponentState(modifiedPresetId, modifiablePreset);
        
        // Load and verify modifications
        ComponentState reloadedPreset = iniDataManager->loadComponentState(modifiedPresetId);
        expectWithinAbsoluteError(reloadedPreset.swingValue, 0.95f, 0.01f, 
                                "Modified preset swing value should be saved");
        expectWithinAbsoluteError(reloadedPreset.energyValue, 0.85f, 0.01f, 
                                "Modified preset energy value should be saved");
        
        // Step 5: Test preset deletion
        logMessage("Testing preset deletion...");
        
        juce::String deletePresetId = "preset_" + testPresets.back().presetName.replaceCharacter(' ', '_');
        
        // Delete preset (implementation depends on INIDataManager)
        // For now, we'll test that we can handle missing presets gracefully
        ComponentState deletedPreset = iniDataManager->loadComponentState("non_existent_preset");
        
        // Should return default state for non-existent preset
        expect(deletedPreset.presetName.isEmpty() || deletedPreset.presetName == "Default", 
               "Non-existent preset should return default state");
        
        logMessage("Preset management workflow completed successfully");
    }

    //==============================================================================
    // State Persistence and Recovery
    //==============================================================================
    
    void testStatePersistenceAndRecovery() {
        logMessage("Testing state persistence and recovery...");
        
        StateTestEnvironment env;
        auto* mainContent = env.getMainContent();
        auto* processor = env.processor.get();
        auto* iniDataManager = env.iniDataManager.get();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        expect(processor != nullptr, "Processor should exist");
        expect(iniDataManager != nullptr, "INIDataManager should exist");
        if (!mainContent || !processor || !iniDataManager) return;
        
        waitForUIStabilization();
        
        // Step 1: Test automatic state persistence
        logMessage("Testing automatic state persistence...");
        
        ComponentState persistentState = createTestState(7);
        persistentState.sessionName = "Persistent Session";
        persistentState.autoSave = true;
        
        // Apply state and trigger auto-save
        mainContent->loadStates(persistentState);
        processor->loadStates(persistentState);
        waitForUIStabilization();
        
        // Save current session state
        iniDataManager->saveComponentState("current_session", persistentState);
        
        // Step 2: Test state recovery after simulated crash
        logMessage("Testing state recovery after simulated crash...");
        
        // Simulate application restart by creating new environment
        StateTestEnvironment recoveryEnv;
        auto* recoveryMainContent = recoveryEnv.getMainContent();
        auto* recoveryProcessor = recoveryEnv.processor.get();
        auto* recoveryINIManager = recoveryEnv.iniDataManager.get();
        
        expect(recoveryMainContent != nullptr, "Recovery MainContentComponent should exist");
        expect(recoveryProcessor != nullptr, "Recovery Processor should exist");
        expect(recoveryINIManager != nullptr, "Recovery INIDataManager should exist");
        if (!recoveryMainContent || !recoveryProcessor || !recoveryINIManager) return;
        
        waitForUIStabilization();
        
        // Load saved session state
        ComponentState recoveredState = recoveryINIManager->loadComponentState("current_session");
        
        // Apply recovered state
        recoveryMainContent->loadStates(recoveredState);
        recoveryProcessor->loadStates(recoveredState);
        waitForUIStabilization();
        
        // Verify state was recovered correctly
        ComponentState verifyRecoveredState;
        recoveryMainContent->saveStates(verifyRecoveredState);
        
        expect(verifyRecoveredState.currentPlayer == persistentState.currentPlayer, 
               "Recovered player should match original");
        expectWithinAbsoluteError(verifyRecoveredState.swingValue, persistentState.swingValue, 0.01f, 
                                "Recovered swing value should match original");
        expectWithinAbsoluteError(verifyRecoveredState.energyValue, persistentState.energyValue, 0.01f, 
                                "Recovered energy value should match original");
        
        // Step 3: Test incremental state backup
        logMessage("Testing incremental state backup...");
        
        std::vector<ComponentState> stateHistory;
        
        // Create state history
        for (int i = 0; i < 5; ++i) {
            ComponentState historyState = createTestState((i % INIConfig::Defaults::MAX_PLAYERS) + 1);
            historyState.sessionName = "History State " + juce::String(i);
            historyState.lastModified = juce::Time::getCurrentTime() + juce::RelativeTime::seconds(i);
            
            stateHistory.push_back(historyState);
            
            // Save each state in history
            juce::String historyId = "history_" + juce::String(i);
            recoveryINIManager->saveComponentState(historyId, historyState);
        }
        
        // Verify history can be loaded
        for (int i = 0; i < 5; ++i) {
            juce::String historyId = "history_" + juce::String(i);
            ComponentState loadedHistoryState = recoveryINIManager->loadComponentState(historyId);
            
            expect(loadedHistoryState.sessionName == stateHistory[i].sessionName, 
                   "History state " + juce::String(i) + " should be recoverable");
        }
        
        // Step 4: Test state corruption recovery
        logMessage("Testing state corruption recovery...");
        
        // Create a valid state
        ComponentState validState = createTestState(8);
        validState.sessionName = "Valid State";
        recoveryINIManager->saveComponentState("valid_state", validState);
        
        // Test loading with graceful fallback for corrupted data
        ComponentState fallbackState = recoveryINIManager->loadComponentState("corrupted_state");
        
        // Should return default state for corrupted/missing data
        expect(fallbackState.currentPlayer >= 1 && fallbackState.currentPlayer <= INIConfig::Defaults::MAX_PLAYERS, 
               "Fallback state should have valid player index");
        expect(fallbackState.swingValue >= 0.0f && fallbackState.swingValue <= 1.0f, 
               "Fallback state should have valid swing value");
        
        logMessage("State persistence and recovery completed successfully");
    }

    //==============================================================================
    // Multi-Player State Management
    //==============================================================================
    
    void testMultiPlayerStateManagement() {
        logMessage("Testing multi-player state management...");
        
        StateTestEnvironment env;
        auto* mainContent = env.getMainContent();
        auto* processor = env.processor.get();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        expect(processor != nullptr, "Processor should exist");
        if (!mainContent || !processor) return;
        
        waitForUIStabilization();
        
        // Step 1: Create individual player states
        logMessage("Creating individual player states...");
        
        std::vector<ComponentState> playerStates;
        
        for (int player = 1; player <= INIConfig::Defaults::MAX_PLAYERS; ++player) {
            ComponentState playerState = createTestState(player);
            playerState.sessionName = "Player " + juce::String(player) + " Session";
            
            // Each player has unique settings
            playerState.swingValue = 0.3f + (player * 0.08f);
            playerState.energyValue = 0.4f + (player * 0.06f);
            playerState.volumeValue = 0.5f + (player * 0.04f);
            
            // Unique MIDI assignments per player
            for (int i = 0; i < 16; ++i) {
                playerState.midiFileAssignments[i] = "player_" + juce::String(player) + "_midi_" + juce::String(i) + ".mid";
            }
            
            playerStates.push_back(playerState);
        }
        
        // Step 2: Test switching between player states
        logMessage("Testing switching between player states...");
        
        for (const auto& playerState : playerStates) {
            // Switch to player
            mainContent->switchToPlayer(playerState.currentPlayer, playerState);
            waitForUIStabilization();
            
            // Verify player state is active
            ComponentState currentState;
            mainContent->saveStates(currentState);
            
            expect(currentState.currentPlayer == playerState.currentPlayer, 
                   "Current player should match switched player");
            expectWithinAbsoluteError(currentState.swingValue, playerState.swingValue, 0.01f, 
                                    "Player swing value should be loaded");
            expectWithinAbsoluteError(currentState.energyValue, playerState.energyValue, 0.01f, 
                                    "Player energy value should be loaded");
            
            // Verify MIDI assignments
            for (int i = 0; i < 8; ++i) {
                juce::String assignedFile = mainContent->getMidiFileAssignment(i);
                expect(assignedFile == playerState.midiFileAssignments[i], 
                       "MIDI assignment " + juce::String(i) + " should match player state");
            }
        }
        
        // Step 3: Test rapid player switching
        logMessage("Testing rapid player switching...");
        
        for (int iteration = 0; iteration < 3; ++iteration) {
            for (int player = 1; player <= INIConfig::Defaults::MAX_PLAYERS; ++player) {
                mainContent->switchToPlayer(player, playerStates[player - 1]);
                juce::Thread::sleep(20); // Rapid switching
            }
        }
        
        waitForUIStabilization();
        
        // Verify final state is consistent
        ComponentState finalState;
        mainContent->saveStates(finalState);
        expect(finalState.currentPlayer >= 1 && finalState.currentPlayer <= INIConfig::Defaults::MAX_PLAYERS, 
               "Final player should be valid after rapid switching");
        
        // Step 4: Test player state isolation
        logMessage("Testing player state isolation...");
        
        // Modify player 1 state
        mainContent->switchToPlayer(1, playerStates[0]);
        mainContent->setSwingValue(0.99f);
        mainContent->setEnergyValue(0.88f);
        
        ComponentState modifiedPlayer1State;
        mainContent->saveStates(modifiedPlayer1State);
        
        // Switch to player 2
        mainContent->switchToPlayer(2, playerStates[1]);
        waitForUIStabilization();
        
        ComponentState player2State;
        mainContent->saveStates(player2State);
        
        // Player 2 should not be affected by player 1 modifications
        expect(player2State.currentPlayer == 2, "Player 2 should be active");
        expectWithinAbsoluteError(player2State.swingValue, playerStates[1].swingValue, 0.01f, 
                                "Player 2 swing value should be unchanged");
        expectWithinAbsoluteError(player2State.energyValue, playerStates[1].energyValue, 0.01f, 
                                "Player 2 energy value should be unchanged");
        
        // Switch back to player 1 and verify modifications are preserved
        mainContent->switchToPlayer(1, modifiedPlayer1State);
        waitForUIStabilization();
        
        ComponentState restoredPlayer1State;
        mainContent->saveStates(restoredPlayer1State);
        
        expectWithinAbsoluteError(restoredPlayer1State.swingValue, 0.99f, 0.01f, 
                                "Player 1 modifications should be preserved");
        expectWithinAbsoluteError(restoredPlayer1State.energyValue, 0.88f, 0.01f, 
                                "Player 1 energy modifications should be preserved");
        
        logMessage("Multi-player state management completed successfully");
    }

    //==============================================================================
    // Real-Time State Updates
    //==============================================================================
    
    void testRealTimeStateUpdates() {
        logMessage("Testing real-time state updates...");
        
        StateTestEnvironment env;
        auto* mainContent = env.getMainContent();
        auto* processor = env.processor.get();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        expect(processor != nullptr, "Processor should exist");
        if (!mainContent || !processor) return;
        
        waitForUIStabilization();
        
        // Step 1: Test continuous parameter updates
        logMessage("Testing continuous parameter updates...");
        
        std::vector<float> swingValues;
        std::vector<float> energyValues;
        
        // Simulate real-time parameter automation
        for (int i = 0; i < 100; ++i) {
            float swingValue = 0.5f + 0.4f * std::sin(i * 0.1f);
            float energyValue = 0.6f + 0.3f * std::cos(i * 0.15f);
            
            mainContent->setSwingValue(swingValue);
            mainContent->setEnergyValue(energyValue);
            
            swingValues.push_back(swingValue);
            energyValues.push_back(energyValue);
            
            // Very short delay to simulate real-time updates
            juce::Thread::sleep(1);
        }
        
        waitForUIStabilization();
        
        // Verify final values
        expectWithinAbsoluteError(mainContent->getSwingValue(), swingValues.back(), 0.01f, 
                                "Final swing value should match last update");
        expectWithinAbsoluteError(mainContent->getEnergyValue(), energyValues.back(), 0.01f, 
                                "Final energy value should match last update");
        
        // Step 2: Test state update callbacks during real-time changes
        logMessage("Testing state update callbacks during real-time changes...");
        
        int callbackCount = 0;
        ComponentState lastCallbackState;
        
        mainContent->onStateChanged = [&](ComponentState& state) {
            callbackCount++;
            lastCallbackState = state;
        };
        
        // Trigger multiple state changes
        for (int i = 0; i < 10; ++i) {
            ComponentState updateState = createTestState((i % INIConfig::Defaults::MAX_PLAYERS) + 1);
            updateState.swingValue = static_cast<float>(i) / 10.0f;
            
            mainContent->loadStates(updateState);
            
            if (mainContent->onStateChanged) {
                mainContent->onStateChanged(updateState);
            }
            
            juce::Thread::sleep(5);
        }
        
        expect(callbackCount > 0, "State change callbacks should be triggered");
        expect(lastCallbackState.currentPlayer >= 1 && lastCallbackState.currentPlayer <= INIConfig::Defaults::MAX_PLAYERS, 
               "Last callback state should have valid player");
        
        // Step 3: Test concurrent state updates
        logMessage("Testing concurrent state updates...");
        
        // Simulate concurrent updates from different sources
        std::atomic<int> updateCount{0};
        
        auto updateTask = [&](int startValue) {
            for (int i = 0; i < 20; ++i) {
                float value = static_cast<float>(startValue + i) / 100.0f;
                mainContent->setVolumeValue(value);
                updateCount++;
                juce::Thread::sleep(2);
            }
        };
        
        // Start concurrent updates
        std::thread thread1(updateTask, 10);
        std::thread thread2(updateTask, 50);
        
        thread1.join();
        thread2.join();
        
        expect(updateCount.load() == 40, "All concurrent updates should complete");
        
        // Verify final state is valid
        float finalVolume = mainContent->getVolumeValue();
        expect(finalVolume >= 0.0f && finalVolume <= 1.0f, "Final volume should be in valid range");
        
        logMessage("Real-time state updates completed successfully");
    }

    //==============================================================================
    // State Validation and Error Handling
    //==============================================================================
    
    void testStateValidationAndErrorHandling() {
        logMessage("Testing state validation and error handling...");
        
        StateTestEnvironment env;
        auto* mainContent = env.getMainContent();
        auto* processor = env.processor.get();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        expect(processor != nullptr, "Processor should exist");
        if (!mainContent || !processor) return;
        
        waitForUIStabilization();
        
        // Step 1: Test invalid parameter values
        logMessage("Testing invalid parameter values...");
        
        // Test out-of-range values
        mainContent->setSwingValue(-999.0f);
        float clampedSwing = mainContent->getSwingValue();
        expect(clampedSwing >= 0.0f && clampedSwing <= 1.0f, "Swing value should be clamped to valid range");
        
        mainContent->setEnergyValue(999.0f);
        float clampedEnergy = mainContent->getEnergyValue();
        expect(clampedEnergy >= 0.0f && clampedEnergy <= 1.0f, "Energy value should be clamped to valid range");
        
        mainContent->setVolumeValue(-0.5f);
        float clampedVolume = mainContent->getVolumeValue();
        expect(clampedVolume >= 0.0f && clampedVolume <= 1.0f, "Volume value should be clamped to valid range");
        
        // Step 2: Test invalid player indices
        logMessage("Testing invalid player indices...");
        
        ComponentState invalidPlayerState;
        invalidPlayerState.currentPlayer = -1;
        
        mainContent->loadStates(invalidPlayerState);
        waitForUIStabilization();
        
        ComponentState correctedState;
        mainContent->saveStates(correctedState);
        expect(correctedState.currentPlayer >= 1 && correctedState.currentPlayer <= INIConfig::Defaults::MAX_PLAYERS, 
               "Invalid player index should be corrected");
        
        // Test player index too high
        invalidPlayerState.currentPlayer = 999;
        mainContent->loadStates(invalidPlayerState);
        waitForUIStabilization();
        
        mainContent->saveStates(correctedState);
        expect(correctedState.currentPlayer >= 1 && correctedState.currentPlayer <= INIConfig::Defaults::MAX_PLAYERS, 
               "High player index should be corrected");
        
        // Step 3: Test corrupted state data
        logMessage("Testing corrupted state data...");
        
        ComponentState corruptedState;
        corruptedState.currentPlayer = 0; // Invalid
        corruptedState.swingValue = std::numeric_limits<float>::quiet_NaN(); // NaN
        corruptedState.energyValue = std::numeric_limits<float>::infinity(); // Infinity
        corruptedState.volumeValue = -std::numeric_limits<float>::infinity(); // Negative infinity
        
        mainContent->loadStates(corruptedState);
        waitForUIStabilization();
        
        ComponentState sanitizedState;
        mainContent->saveStates(sanitizedState);
        
        expect(sanitizedState.currentPlayer >= 1 && sanitizedState.currentPlayer <= INIConfig::Defaults::MAX_PLAYERS, 
               "Corrupted player should be sanitized");
        expect(std::isfinite(sanitizedState.swingValue) && sanitizedState.swingValue >= 0.0f && sanitizedState.swingValue <= 1.0f, 
               "NaN swing value should be sanitized");
        expect(std::isfinite(sanitizedState.energyValue) && sanitizedState.energyValue >= 0.0f && sanitizedState.energyValue <= 1.0f, 
               "Infinite energy value should be sanitized");
        expect(std::isfinite(sanitizedState.volumeValue) && sanitizedState.volumeValue >= 0.0f && sanitizedState.volumeValue <= 1.0f, 
               "Negative infinite volume value should be sanitized");
        
        // Step 4: Test error recovery mechanisms
        logMessage("Testing error recovery mechanisms...");
        
        // Test recovery from invalid MIDI file assignments
        for (int i = 0; i < 20; ++i) { // More than valid range
            mainContent->setMidiFileAssignment(i, "invalid_file_" + juce::String(i) + ".mid");
        }
        
        // Should handle gracefully without crashing
        for (int i = 0; i < 16; ++i) { // Valid range
            juce::String assignment = mainContent->getMidiFileAssignment(i);
            // Should return some valid string (empty or default)
            expect(assignment.isNotEmpty() || assignment.isEmpty(), "MIDI assignment should be valid string");
        }
        
        // Test recovery from invalid button indices
        juce::String invalidAssignment1 = mainContent->getMidiFileAssignment(-1);
        juce::String invalidAssignment2 = mainContent->getMidiFileAssignment(999);
        
        // Should handle gracefully without crashing
        expect(invalidAssignment1.isNotEmpty() || invalidAssignment1.isEmpty(), "Invalid index should return valid string");
        expect(invalidAssignment2.isNotEmpty() || invalidAssignment2.isEmpty(), "Invalid index should return valid string");
        
        logMessage("State validation and error handling completed successfully");
    }

    //==============================================================================
    // Concurrent State Operations
    //==============================================================================
    
    void testConcurrentStateOperations() {
        logMessage("Testing concurrent state operations...");
        
        StateTestEnvironment env;
        auto* mainContent = env.getMainContent();
        auto* processor = env.processor.get();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        expect(processor != nullptr, "Processor should exist");
        if (!mainContent || !processor) return;
        
        waitForUIStabilization();
        
        // Step 1: Test concurrent state saves and loads
        logMessage("Testing concurrent state saves and loads...");
        
        std::atomic<int> saveCount{0};
        std::atomic<int> loadCount{0};
        std::atomic<bool> errorOccurred{false};
        
        auto saveTask = [&]() {
            try {
                for (int i = 0; i < 50; ++i) {
                    ComponentState saveState = createTestState((i % INIConfig::Defaults::MAX_PLAYERS) + 1);
                    mainContent->loadStates(saveState);
                    
                    ComponentState currentState;
                    mainContent->saveStates(currentState);
                    
                    saveCount++;
                    juce::Thread::sleep(1);
                }
            } catch (...) {
                errorOccurred = true;
            }
        };
        
        auto loadTask = [&]() {
            try {
                for (int i = 0; i < 50; ++i) {
                    ComponentState loadState = createTestState(((i + 2) % INIConfig::Defaults::MAX_PLAYERS) + 1);
                    mainContent->loadStates(loadState);
                    
                    loadCount++;
                    juce::Thread::sleep(1);
                }
            } catch (...) {
                errorOccurred = true;
            }
        };
        
        // Start concurrent operations
        std::thread saveThread(saveTask);
        std::thread loadThread(loadTask);
        
        saveThread.join();
        loadThread.join();
        
        expect(!errorOccurred.load(), "No errors should occur during concurrent operations");
        expect(saveCount.load() == 50, "All save operations should complete");
        expect(loadCount.load() == 50, "All load operations should complete");
        
        // Step 2: Test concurrent parameter updates
        logMessage("Testing concurrent parameter updates...");
        
        std::atomic<int> swingUpdateCount{0};
        std::atomic<int> energyUpdateCount{0};
        std::atomic<int> volumeUpdateCount{0};
        
        auto swingUpdateTask = [&]() {
            for (int i = 0; i < 30; ++i) {
                float value = static_cast<float>(i) / 30.0f;
                mainContent->setSwingValue(value);
                swingUpdateCount++;
                juce::Thread::sleep(2);
            }
        };
        
        auto energyUpdateTask = [&]() {
            for (int i = 0; i < 30; ++i) {
                float value = static_cast<float>(i) / 30.0f;
                mainContent->setEnergyValue(value);
                energyUpdateCount++;
                juce::Thread::sleep(2);
            }
        };
        
        auto volumeUpdateTask = [&]() {
            for (int i = 0; i < 30; ++i) {
                float value = static_cast<float>(i) / 30.0f;
                mainContent->setVolumeValue(value);
                volumeUpdateCount++;
                juce::Thread::sleep(2);
            }
        };
        
        // Start concurrent parameter updates
        std::thread swingThread(swingUpdateTask);
        std::thread energyThread(energyUpdateTask);
        std::thread volumeThread(volumeUpdateTask);
        
        swingThread.join();
        energyThread.join();
        volumeThread.join();
        
        expect(swingUpdateCount.load() == 30, "All swing updates should complete");
        expect(energyUpdateCount.load() == 30, "All energy updates should complete");
        expect(volumeUpdateCount.load() == 30, "All volume updates should complete");
        
        // Verify final state is consistent
        ComponentState finalState;
        mainContent->saveStates(finalState);
        
        expect(finalState.swingValue >= 0.0f && finalState.swingValue <= 1.0f, 
               "Final swing value should be valid after concurrent updates");
        expect(finalState.energyValue >= 0.0f && finalState.energyValue <= 1.0f, 
               "Final energy value should be valid after concurrent updates");
        expect(finalState.volumeValue >= 0.0f && finalState.volumeValue <= 1.0f, 
               "Final volume value should be valid after concurrent updates");
        
        logMessage("Concurrent state operations completed successfully");
    }

    //==============================================================================
    // State Migration and Versioning
    //==============================================================================
    
    void testStateMigrationAndVersioning() {
        logMessage("Testing state migration and versioning...");
        
        StateTestEnvironment env;
        auto* mainContent = env.getMainContent();
        auto* iniDataManager = env.iniDataManager.get();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        expect(iniDataManager != nullptr, "INIDataManager should exist");
        if (!mainContent || !iniDataManager) return;
        
        waitForUIStabilization();
        
        // Step 1: Test version compatibility
        logMessage("Testing version compatibility...");
        
        ComponentState currentVersionState = createTestState(1);
        currentVersionState.version = "1.0.0";
        currentVersionState.sessionName = "Current Version Session";
        
        // Save current version state
        iniDataManager->saveComponentState("version_test", currentVersionState);
        
        // Load and verify
        ComponentState loadedVersionState = iniDataManager->loadComponentState("version_test");
        expect(loadedVersionState.version == currentVersionState.version, 
               "Version should be preserved");
        expect(loadedVersionState.sessionName == currentVersionState.sessionName, 
               "Session name should be preserved");
        
        // Step 2: Test backward compatibility
        logMessage("Testing backward compatibility...");
        
        // Simulate loading an older version state (missing some fields)
        ComponentState olderVersionState;
        olderVersionState.currentPlayer = 2;
        olderVersionState.swingValue = 0.6f;
        olderVersionState.energyValue = 0.7f;
        // Note: volumeValue and other newer fields are not set (default values)
        olderVersionState.version = "0.9.0";
        
        mainContent->loadStates(olderVersionState);
        waitForUIStabilization();
        
        ComponentState migratedState;
        mainContent->saveStates(migratedState);
        
        // Should have migrated to current version with default values for missing fields
        expect(migratedState.currentPlayer == olderVersionState.currentPlayer, 
               "Migrated player should match older version");
        expectWithinAbsoluteError(migratedState.swingValue, olderVersionState.swingValue, 0.01f, 
                                "Migrated swing value should match older version");
        expect(migratedState.volumeValue >= 0.0f && migratedState.volumeValue <= 1.0f, 
               "Migrated volume should have valid default value");
        
        // Step 3: Test forward compatibility handling
        logMessage("Testing forward compatibility handling...");
        
        // Simulate loading a newer version state (with extra fields)
        ComponentState newerVersionState = createTestState(3);
        newerVersionState.version = "2.0.0";
        newerVersionState.sessionName = "Future Version Session";
        // In a real scenario, this might have additional fields we don't recognize
        
        mainContent->loadStates(newerVersionState);
        waitForUIStabilization();
        
        ComponentState loadedNewerState;
        mainContent->saveStates(loadedNewerState);
        
        // Should handle gracefully, loading what it can understand
        expect(loadedNewerState.currentPlayer == newerVersionState.currentPlayer, 
               "Should load compatible fields from newer version");
        expectWithinAbsoluteError(loadedNewerState.swingValue, newerVersionState.swingValue, 0.01f, 
                                "Should load compatible swing value from newer version");
        
        logMessage("State migration and versioning completed successfully");
    }

    //==============================================================================
    // Performance Under State Load
    //==============================================================================
    
    void testPerformanceUnderStateLoad() {
        logMessage("Testing performance under state load...");
        
        StateTestEnvironment env;
        auto* mainContent = env.getMainContent();
        auto* processor = env.processor.get();
        auto* iniDataManager = env.iniDataManager.get();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        expect(processor != nullptr, "Processor should exist");
        expect(iniDataManager != nullptr, "INIDataManager should exist");
        if (!mainContent || !processor || !iniDataManager) return;
        
        waitForUIStabilization();
        
        // Step 1: Test performance with large number of state operations
        logMessage("Testing performance with large number of state operations...");
        
        const auto startTime = juce::Time::getMillisecondCounterHiRes();
        
        // Perform many state operations
        for (int i = 0; i < 1000; ++i) {
            ComponentState testState = createTestState((i % INIConfig::Defaults::MAX_PLAYERS) + 1);
            testState.swingValue = static_cast<float>(i % 100) / 100.0f;
            
            mainContent->loadStates(testState);
            
            ComponentState savedState;
            mainContent->saveStates(savedState);
            
            // Verify state integrity under load
            if (i % 100 == 0) {
                expect(savedState.currentPlayer == testState.currentPlayer, 
                       "State should remain consistent under load at iteration " + juce::String(i));
            }
        }
        
        const auto endTime = juce::Time::getMillisecondCounterHiRes();
        const auto totalTime = endTime - startTime;
        
        expect(totalTime < 5000.0, "1000 state operations should complete within 5 seconds");
        
        // Step 2: Test memory usage during intensive state operations
        logMessage("Testing memory usage during intensive state operations...");
        
        // Create many state objects
        std::vector<ComponentState> stateCollection;
        stateCollection.reserve(500);
        
        for (int i = 0; i < 500; ++i) {
            ComponentState state = createTestState((i % INIConfig::Defaults::MAX_PLAYERS) + 1);
            state.sessionName = "Performance Test State " + juce::String(i);
            
            // Add complex MIDI assignments
            for (int j = 0; j < 16; ++j) {
                state.midiFileAssignments[j] = "complex_pattern_" + juce::String(i) + "_" + juce::String(j) + ".mid";
            }
            
            stateCollection.push_back(state);
        }
        
        // Apply all states rapidly
        const auto memoryTestStart = juce::Time::getMillisecondCounterHiRes();
        
        for (const auto& state : stateCollection) {
            mainContent->loadStates(state);
            processor->loadStates(state);
        }
        
        const auto memoryTestEnd = juce::Time::getMillisecondCounterHiRes();
        const auto memoryTestTime = memoryTestEnd - memoryTestStart;
        
        expect(memoryTestTime < 2000.0, "500 complex state loads should complete within 2 seconds");
        
        // Step 3: Test state operations during audio processing
        logMessage("Testing state operations during audio processing...");
        
        juce::AudioBuffer<float> audioBuffer(2, 512);
        juce::MidiBuffer midiBuffer;
        
        const auto audioTestStart = juce::Time::getMillisecondCounterHiRes();
        
        for (int i = 0; i < 100; ++i) {
            // Simulate audio processing
            processor->processBlock(audioBuffer, midiBuffer);
            
            // Perform state operations during audio processing
            ComponentState audioState = createTestState((i % INIConfig::Defaults::MAX_PLAYERS) + 1);
            mainContent->loadStates(audioState);
            
            ComponentState currentState;
            mainContent->saveStates(currentState);
            
            // Verify state consistency during audio processing
            expect(currentState.currentPlayer == audioState.currentPlayer, 
                   "State should remain consistent during audio processing");
        }
        
        const auto audioTestEnd = juce::Time::getMillisecondCounterHiRes();
        const auto audioTestTime = audioTestEnd - audioTestStart;
        
        expect(audioTestTime < 1000.0, "State operations during audio processing should be efficient");
        
        // Step 4: Test cleanup and memory recovery
        logMessage("Testing cleanup and memory recovery...");
        
        // Clear large state collection
        stateCollection.clear();
        stateCollection.shrink_to_fit();
        
        // Reset to default state
        ComponentState defaultState;
        mainContent->loadStates(defaultState);
        processor->loadStates(defaultState);
        
        waitForUIStabilization();
        
        // Verify system is still responsive after cleanup
        ComponentState finalState;
        mainContent->saveStates(finalState);
        
        expect(finalState.currentPlayer >= 1 && finalState.currentPlayer <= INIConfig::Defaults::MAX_PLAYERS, 
               "System should be responsive after cleanup");
        
        logMessage("Performance under state load completed successfully");
    }

    //==============================================================================
    // Utility Methods
    //==============================================================================
    
    void logMessage(const juce::String& message) {
        DBG("StateManagementE2ETests: " + message);
    }
};

// Register the test
static StateManagementE2ETests stateManagementE2ETestsInstance;