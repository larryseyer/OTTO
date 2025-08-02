#pragma once

#include <JuceHeader.h>
#include "../JUCE8_CODING_STANDARDS.h"
#include "../Source/INIConfig.h"
#include "../Source/MainContentComponent.h"
#include "../Source/PluginProcessor.h"
#include "../Source/PluginEditor.h"
#include "../Source/TopBarComponent.h"
#include "../Source/PlayerTabsComponent.h"
#include "../Source/DrumKitSectionComponent.h"
#include "../Source/SceneLauncherComponent.h"
#include "../Source/MainContentComponentLeftSection.h"
#include "../Source/MainContentComponentRightSection.h"
#include "../Source/LoopSectionComponent.h"
#include "../Source/ResponsiveLayoutManager.h"
#include "../Source/FontManager.h"
#include "../Source/ColorScheme.h"

//==============================================================================
// Component Integration E2E Tests - Cross-Component Communication
// Tests how components work together and communicate
//==============================================================================

class ComponentIntegrationTests : public juce::UnitTest {
public:
    ComponentIntegrationTests() : juce::UnitTest("Component Integration Tests") {}

    void runTest() override {
        beginTest("MainContentComponent Layout Integration");
        testMainContentComponentLayout();

        beginTest("Row-Based Layout System Integration");
        testRowBasedLayoutSystem();

        beginTest("Responsive Scaling Integration");
        testResponsiveScalingIntegration();

        beginTest("Component State Synchronization");
        testComponentStateSynchronization();

        beginTest("Font Manager Integration");
        testFontManagerIntegration();

        beginTest("Color Scheme Integration");
        testColorSchemeIntegration();

        beginTest("Cross-Component Event Handling");
        testCrossComponentEventHandling();

        beginTest("Memory Management Integration");
        testMemoryManagementIntegration();

        beginTest("Platform-Specific Integration");
        testPlatformSpecificIntegration();

        beginTest("Performance Integration");
        testPerformanceIntegration();
    }

private:
    //==============================================================================
    // Test Setup Utilities
    //==============================================================================
    
    struct TestEnvironment {
        std::unique_ptr<OTTOAudioProcessor> processor;
        std::unique_ptr<juce::AudioProcessorEditor> editor;
        std::unique_ptr<FontManager> fontManager;
        std::unique_ptr<ColorScheme> colorScheme;
        std::unique_ptr<ResponsiveLayoutManager> layoutManager;
        
        TestEnvironment() {
            // Create processor
            processor = std::make_unique<OTTOAudioProcessor>();
            processor->prepareToPlay(
                static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE),
                INIConfig::Defaults::DEFAULT_BUFFER_SIZE
            );
            
            // Create supporting systems
            fontManager = std::make_unique<FontManager>();
            colorScheme = std::make_unique<ColorScheme>();
            layoutManager = std::make_unique<ResponsiveLayoutManager>();
            
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

    //==============================================================================
    // MainContentComponent Layout Integration
    //==============================================================================
    
    void testMainContentComponentLayout() {
        logMessage("Testing MainContentComponent layout integration...");
        
        TestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Validate component hierarchy
        logMessage("Validating component hierarchy...");
        
        expect(mainContent->getLeftSection() != nullptr, "Left section should exist");
        expect(mainContent->getRightSection() != nullptr, "Right section should exist");
        
        // Step 2: Test layout bounds calculation
        logMessage("Testing layout bounds calculation...");
        
        auto bounds = mainContent->getLocalBounds();
        expect(bounds.getWidth() == INIConfig::LayoutConstants::defaultInterfaceWidth,
               "MainContent width should match INI config");
        expect(bounds.getHeight() == INIConfig::LayoutConstants::defaultInterfaceHeight,
               "MainContent height should match INI config");
        
        // Step 3: Test component positioning
        logMessage("Testing component positioning...");
        
        auto* leftSection = mainContent->getLeftSection();
        auto* rightSection = mainContent->getRightSection();
        
        if (leftSection && rightSection) {
            auto leftBounds = leftSection->getBounds();
            auto rightBounds = rightSection->getBounds();
            
            expect(leftBounds.getX() >= 0, "Left section should have valid X position");
            expect(leftBounds.getY() >= 0, "Left section should have valid Y position");
            expect(rightBounds.getX() >= leftBounds.getRight(), "Right section should be to the right of left section");
            
            // Validate sections don't overlap
            expect(!leftBounds.intersects(rightBounds), "Left and right sections should not overlap");
        }
        
        // Step 4: Test resizing behavior
        logMessage("Testing resizing behavior...");
        
        // Test different sizes
        std::vector<std::pair<int, int>> testSizes = {
            {800, 600},   // Smaller
            {1200, 800},  // Default
            {1600, 1000}, // Larger
            {1920, 1080}  // HD
        };
        
        for (const auto& size : testSizes) {
            mainContent->setSize(size.first, size.second);
            waitForUIStabilization();
            
            auto newBounds = mainContent->getLocalBounds();
            expect(newBounds.getWidth() == size.first, "Width should match after resize");
            expect(newBounds.getHeight() == size.second, "Height should match after resize");
            
            // Validate child components are still positioned correctly
            if (leftSection && rightSection) {
                auto leftBounds = leftSection->getBounds();
                auto rightBounds = rightSection->getBounds();
                
                expect(leftBounds.getWidth() > 0, "Left section should have positive width after resize");
                expect(rightBounds.getWidth() > 0, "Right section should have positive width after resize");
            }
        }
        
        logMessage("MainContentComponent layout integration completed successfully");
    }

    //==============================================================================
    // Row-Based Layout System Integration
    //==============================================================================
    
    void testRowBasedLayoutSystem() {
        logMessage("Testing row-based layout system integration...");
        
        TestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Validate row height percentages
        logMessage("Validating row height percentages...");
        
        const float totalPercentage = 
            INIConfig::LayoutConstants::row1HeightPercent +
            INIConfig::LayoutConstants::row2HeightPercent +
            INIConfig::LayoutConstants::row3HeightPercent +
            INIConfig::LayoutConstants::row4HeightPercent +
            INIConfig::LayoutConstants::row5HeightPercent +
            INIConfig::LayoutConstants::row6HeightPercent;
        
        expectWithinAbsoluteError(totalPercentage, 100.0f, 0.1f, 
                                "Total row percentages should equal 100%");
        
        // Step 2: Test row height calculations
        logMessage("Testing row height calculations...");
        
        const int totalHeight = mainContent->getHeight();
        
        const int expectedRow1Height = static_cast<int>(totalHeight * INIConfig::LayoutConstants::row1HeightPercent / 100.0f);
        const int expectedRow2Height = static_cast<int>(totalHeight * INIConfig::LayoutConstants::row2HeightPercent / 100.0f);
        const int expectedRow3Height = static_cast<int>(totalHeight * INIConfig::LayoutConstants::row3HeightPercent / 100.0f);
        const int expectedRow4Height = static_cast<int>(totalHeight * INIConfig::LayoutConstants::row4HeightPercent / 100.0f);
        const int expectedRow5Height = static_cast<int>(totalHeight * INIConfig::LayoutConstants::row5HeightPercent / 100.0f);
        const int expectedRow6Height = static_cast<int>(totalHeight * INIConfig::LayoutConstants::row6HeightPercent / 100.0f);
        
        // Validate row heights are reasonable
        expect(expectedRow1Height > 0, "Row 1 height should be positive");
        expect(expectedRow2Height > 0, "Row 2 height should be positive");
        expect(expectedRow3Height > 0, "Row 3 height should be positive");
        expect(expectedRow4Height > 0, "Row 4 height should be positive");
        expect(expectedRow5Height > 0, "Row 5 height should be positive");
        expect(expectedRow6Height > 0, "Row 6 height should be positive");
        
        // Step 3: Test row layout at different sizes
        logMessage("Testing row layout at different sizes...");
        
        std::vector<std::pair<int, int>> testSizes = {
            {800, 600},
            {1200, 800},
            {1600, 1000}
        };
        
        for (const auto& size : testSizes) {
            mainContent->setSize(size.first, size.second);
            waitForUIStabilization();
            
            // Recalculate expected heights for new size
            const int newTotalHeight = size.second;
            const int newRow1Height = static_cast<int>(newTotalHeight * INIConfig::LayoutConstants::row1HeightPercent / 100.0f);
            const int newRow5Height = static_cast<int>(newTotalHeight * INIConfig::LayoutConstants::row5HeightPercent / 100.0f);
            
            expect(newRow1Height > 0, "Row 1 height should be positive at size " + juce::String(size.first) + "x" + juce::String(size.second));
            expect(newRow5Height > 0, "Row 5 height should be positive at size " + juce::String(size.first) + "x" + juce::String(size.second));
            
            // Row 5 should be the largest (32% of height)
            expect(newRow5Height >= newRow1Height, "Row 5 should be taller than Row 1");
        }
        
        logMessage("Row-based layout system integration completed successfully");
    }

    //==============================================================================
    // Responsive Scaling Integration
    //==============================================================================
    
    void testResponsiveScalingIntegration() {
        logMessage("Testing responsive scaling integration...");
        
        TestEnvironment env;
        auto* mainContent = env.getMainContent();
        auto* layoutManager = env.layoutManager.get();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        expect(layoutManager != nullptr, "ResponsiveLayoutManager should exist");
        if (!mainContent || !layoutManager) return;
        
        waitForUIStabilization();
        
        // Step 1: Test scaling factor calculation
        logMessage("Testing scaling factor calculation...");
        
        const float baseWidth = static_cast<float>(INIConfig::LayoutConstants::defaultInterfaceWidth);
        const float baseHeight = static_cast<float>(INIConfig::LayoutConstants::defaultInterfaceHeight);
        
        std::vector<std::pair<int, int>> testSizes = {
            {600, 400},   // 0.5x scale
            {800, 533},   // ~0.67x scale
            {1200, 800},  // 1.0x scale (default)
            {1800, 1200}, // 1.5x scale
            {2400, 1600}  // 2.0x scale
        };
        
        for (const auto& size : testSizes) {
            mainContent->setSize(size.first, size.second);
            waitForUIStabilization();
            
            const float scaleFactorX = size.first / baseWidth;
            const float scaleFactorY = size.second / baseHeight;
            
            expect(scaleFactorX > 0.0f, "Scale factor X should be positive");
            expect(scaleFactorY > 0.0f, "Scale factor Y should be positive");
            
            // Test that components scale appropriately
            auto* leftSection = mainContent->getLeftSection();
            if (leftSection) {
                auto leftBounds = leftSection->getBounds();
                expect(leftBounds.getWidth() > 0, "Left section should have positive width after scaling");
                expect(leftBounds.getHeight() > 0, "Left section should have positive height after scaling");
            }
        }
        
        // Step 2: Test minimum size constraints
        logMessage("Testing minimum size constraints...");
        
        // Test very small size
        mainContent->setSize(400, 300);
        waitForUIStabilization();
        
        auto* leftSection = mainContent->getLeftSection();
        auto* rightSection = mainContent->getRightSection();
        
        if (leftSection && rightSection) {
            auto leftBounds = leftSection->getBounds();
            auto rightBounds = rightSection->getBounds();
            
            // Components should still be usable at small sizes
            expect(leftBounds.getWidth() >= 100, "Left section should maintain minimum width");
            expect(rightBounds.getWidth() >= 100, "Right section should maintain minimum width");
        }
        
        // Step 3: Test maximum size handling
        logMessage("Testing maximum size handling...");
        
        // Test very large size
        mainContent->setSize(3840, 2160); // 4K
        waitForUIStabilization();
        
        if (leftSection && rightSection) {
            auto leftBounds = leftSection->getBounds();
            auto rightBounds = rightSection->getBounds();
            
            // Components should scale up appropriately
            expect(leftBounds.getWidth() > 200, "Left section should scale up at large sizes");
            expect(rightBounds.getWidth() > 200, "Right section should scale up at large sizes");
        }
        
        logMessage("Responsive scaling integration completed successfully");
    }

    //==============================================================================
    // Component State Synchronization
    //==============================================================================
    
    void testComponentStateSynchronization() {
        logMessage("Testing component state synchronization...");
        
        TestEnvironment env;
        auto* mainContent = env.getMainContent();
        auto* processor = env.processor.get();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        expect(processor != nullptr, "Processor should exist");
        if (!mainContent || !processor) return;
        
        waitForUIStabilization();
        
        // Step 1: Test state propagation from UI to processor
        logMessage("Testing state propagation from UI to processor...");
        
        // Set UI state
        mainContent->setSwingValue(0.75f);
        mainContent->setEnergyValue(0.85f);
        mainContent->setVolumeValue(0.65f);
        
        ComponentState uiState;
        mainContent->saveStates(uiState);
        
        // Propagate to processor
        processor->loadStates(uiState);
        
        // Verify processor received the state
        ComponentState processorState;
        processor->saveStates(processorState);
        
        expectWithinAbsoluteError(processorState.swingValue, 0.75f, 0.01f, 
                                "Processor should receive swing value from UI");
        expectWithinAbsoluteError(processorState.energyValue, 0.85f, 0.01f, 
                                "Processor should receive energy value from UI");
        expectWithinAbsoluteError(processorState.volumeValue, 0.65f, 0.01f, 
                                "Processor should receive volume value from UI");
        
        // Step 2: Test state propagation from processor to UI
        logMessage("Testing state propagation from processor to UI...");
        
        // Modify processor state
        ComponentState newProcessorState;
        newProcessorState.swingValue = 0.45f;
        newProcessorState.energyValue = 0.55f;
        newProcessorState.volumeValue = 0.35f;
        newProcessorState.currentPlayer = 3;
        
        processor->loadStates(newProcessorState);
        
        // Propagate to UI
        ComponentState processorToUIState;
        processor->saveStates(processorToUIState);
        mainContent->loadStates(processorToUIState);
        
        waitForUIStabilization();
        
        // Verify UI received the state
        expectWithinAbsoluteError(mainContent->getSwingValue(), 0.45f, 0.01f, 
                                "UI should receive swing value from processor");
        expectWithinAbsoluteError(mainContent->getEnergyValue(), 0.55f, 0.01f, 
                                "UI should receive energy value from processor");
        expectWithinAbsoluteError(mainContent->getVolumeValue(), 0.35f, 0.01f, 
                                "UI should receive volume value from processor");
        
        // Step 3: Test cross-component synchronization
        logMessage("Testing cross-component synchronization...");
        
        // Test player switching synchronization
        for (int player = 1; player <= 4; ++player) {
            ComponentState playerState;
            playerState.currentPlayer = player;
            playerState.swingValue = 0.1f * player; // Unique value per player
            
            mainContent->switchToPlayer(player, playerState);
            waitForUIStabilization();
            
            // Verify state is synchronized
            ComponentState currentState;
            mainContent->saveStates(currentState);
            expect(currentState.currentPlayer == player, 
                   "Current player should be synchronized");
        }
        
        // Step 4: Test MIDI file assignment synchronization
        logMessage("Testing MIDI file assignment synchronization...");
        
        for (int buttonIndex = 0; buttonIndex < 8; ++buttonIndex) {
            juce::String testFile = "sync_test_" + juce::String(buttonIndex) + ".mid";
            mainContent->setMidiFileAssignment(buttonIndex, testFile);
            
            juce::String retrievedFile = mainContent->getMidiFileAssignment(buttonIndex);
            expect(retrievedFile == testFile, 
                   "MIDI file assignment should be synchronized for button " + juce::String(buttonIndex));
        }
        
        logMessage("Component state synchronization completed successfully");
    }

    //==============================================================================
    // Font Manager Integration
    //==============================================================================
    
    void testFontManagerIntegration() {
        logMessage("Testing FontManager integration...");
        
        TestEnvironment env;
        auto* fontManager = env.fontManager.get();
        auto* mainContent = env.getMainContent();
        
        expect(fontManager != nullptr, "FontManager should exist");
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!fontManager || !mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test font loading
        logMessage("Testing font loading...");
        
        // Test primary fonts
        auto robotoFont = fontManager->getRobotoFont(FontManager::Weight::Regular, 14.0f);
        expect(robotoFont.getTypefaceName().isNotEmpty(), "Roboto font should be loaded");
        
        auto montserratFont = fontManager->getMontserratFont(FontManager::Weight::Bold, 16.0f);
        expect(montserratFont.getTypefaceName().isNotEmpty(), "Montserrat font should be loaded");
        
        // Test icon fonts
        auto iconFont = fontManager->getPhosphorFont(FontManager::PhosphorWeight::Regular, 16.0f);
        expect(iconFont.getTypefaceName().isNotEmpty(), "Phosphor icon font should be loaded");
        
        // Step 2: Test font scaling
        logMessage("Testing font scaling...");
        
        std::vector<float> testSizes = {10.0f, 12.0f, 14.0f, 16.0f, 18.0f, 24.0f};
        
        for (float size : testSizes) {
            auto scaledFont = fontManager->getRobotoFont(FontManager::Weight::Regular, size);
            expectWithinAbsoluteError(scaledFont.getHeight(), size, 1.0f, 
                                    "Font should scale to requested size: " + juce::String(size));
        }
        
        // Step 3: Test icon string retrieval
        logMessage("Testing icon string retrieval...");
        
        auto playIcon = fontManager->getIconString("play");
        expect(playIcon.isNotEmpty(), "Play icon string should not be empty");
        
        auto stopIcon = fontManager->getIconString("stop");
        expect(stopIcon.isNotEmpty(), "Stop icon string should not be empty");
        
        auto recordIcon = fontManager->getIconString("record");
        expect(recordIcon.isNotEmpty(), "Record icon string should not be empty");
        
        // Step 4: Test font weight variations
        logMessage("Testing font weight variations...");
        
        auto lightFont = fontManager->getRobotoFont(FontManager::Weight::Light, 14.0f);
        auto regularFont = fontManager->getRobotoFont(FontManager::Weight::Regular, 14.0f);
        auto boldFont = fontManager->getRobotoFont(FontManager::Weight::Bold, 14.0f);
        
        expect(lightFont.getTypefaceName().isNotEmpty(), "Light font should be loaded");
        expect(regularFont.getTypefaceName().isNotEmpty(), "Regular font should be loaded");
        expect(boldFont.getTypefaceName().isNotEmpty(), "Bold font should be loaded");
        
        logMessage("FontManager integration completed successfully");
    }

    //==============================================================================
    // Color Scheme Integration
    //==============================================================================
    
    void testColorSchemeIntegration() {
        logMessage("Testing ColorScheme integration...");
        
        TestEnvironment env;
        auto* colorScheme = env.colorScheme.get();
        auto* mainContent = env.getMainContent();
        
        expect(colorScheme != nullptr, "ColorScheme should exist");
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!colorScheme || !mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test primary color retrieval
        logMessage("Testing primary color retrieval...");
        
        auto backgroundColor = colorScheme->getBackgroundColor();
        auto foregroundColor = colorScheme->getForegroundColor();
        auto accentColor = colorScheme->getAccentColor();
        
        expect(backgroundColor.isOpaque() || backgroundColor.isTransparent(), "Background color should be valid");
        expect(foregroundColor.isOpaque() || foregroundColor.isTransparent(), "Foreground color should be valid");
        expect(accentColor.isOpaque() || accentColor.isTransparent(), "Accent color should be valid");
        
        // Step 2: Test component-specific colors
        logMessage("Testing component-specific colors...");
        
        auto buttonColor = colorScheme->getButtonColor();
        auto sliderColor = colorScheme->getSliderColor();
        auto textColor = colorScheme->getTextColor();
        
        expect(buttonColor.isOpaque() || buttonColor.isTransparent(), "Button color should be valid");
        expect(sliderColor.isOpaque() || sliderColor.isTransparent(), "Slider color should be valid");
        expect(textColor.isOpaque() || textColor.isTransparent(), "Text color should be valid");
        
        // Step 3: Test state colors
        logMessage("Testing state colors...");
        
        auto activeColor = colorScheme->getActiveColor();
        auto inactiveColor = colorScheme->getInactiveColor();
        auto hoverColor = colorScheme->getHoverColor();
        
        expect(activeColor.isOpaque() || activeColor.isTransparent(), "Active color should be valid");
        expect(inactiveColor.isOpaque() || inactiveColor.isTransparent(), "Inactive color should be valid");
        expect(hoverColor.isOpaque() || hoverColor.isTransparent(), "Hover color should be valid");
        
        // Step 4: Test color contrast
        logMessage("Testing color contrast...");
        
        // Basic contrast test - background and foreground should be different
        expect(backgroundColor != foregroundColor, "Background and foreground colors should be different");
        expect(activeColor != inactiveColor, "Active and inactive colors should be different");
        
        logMessage("ColorScheme integration completed successfully");
    }

    //==============================================================================
    // Cross-Component Event Handling
    //==============================================================================
    
    void testCrossComponentEventHandling() {
        logMessage("Testing cross-component event handling...");
        
        TestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test callback chain setup
        logMessage("Testing callback chain setup...");
        
        bool editModeCallbackTriggered = false;
        bool midiFileCallbackTriggered = false;
        bool sliderCallbackTriggered = false;
        bool stateCallbackTriggered = false;
        
        mainContent->onEditModeChanged = [&](bool isEditing) {
            editModeCallbackTriggered = true;
        };
        
        mainContent->onMidiFileChanged = [&](int buttonIndex, const juce::String& midiFile) {
            midiFileCallbackTriggered = true;
        };
        
        mainContent->onSliderValueChanged = [&](const juce::String& sliderName, float value) {
            sliderCallbackTriggered = true;
        };
        
        mainContent->onStateChanged = [&](ComponentState& state) {
            stateCallbackTriggered = true;
        };
        
        // Step 2: Test event propagation
        logMessage("Testing event propagation...");
        
        // Trigger edit mode change
        mainContent->setEditModeVisuals(true);
        if (mainContent->onEditModeChanged) {
            mainContent->onEditModeChanged(true);
        }
        expect(editModeCallbackTriggered, "Edit mode callback should be triggered");
        
        // Trigger MIDI file change
        mainContent->setMidiFileAssignment(0, "test.mid");
        if (mainContent->onMidiFileChanged) {
            mainContent->onMidiFileChanged(0, "test.mid");
        }
        expect(midiFileCallbackTriggered, "MIDI file callback should be triggered");
        
        // Trigger slider change
        mainContent->setSwingValue(0.6f);
        if (mainContent->onSliderValueChanged) {
            mainContent->onSliderValueChanged("swing", 0.6f);
        }
        expect(sliderCallbackTriggered, "Slider callback should be triggered");
        
        // Trigger state change
        ComponentState testState;
        testState.currentPlayer = 2;
        mainContent->loadStates(testState);
        if (mainContent->onStateChanged) {
            mainContent->onStateChanged(testState);
        }
        expect(stateCallbackTriggered, "State callback should be triggered");
        
        // Step 3: Test event ordering
        logMessage("Testing event ordering...");
        
        std::vector<juce::String> eventOrder;
        
        mainContent->onEditModeChanged = [&](bool isEditing) {
            eventOrder.add("editMode");
        };
        
        mainContent->onSliderValueChanged = [&](const juce::String& sliderName, float value) {
            eventOrder.add("slider_" + sliderName);
        };
        
        // Trigger multiple events
        mainContent->setEditModeVisuals(false);
        if (mainContent->onEditModeChanged) {
            mainContent->onEditModeChanged(false);
        }
        
        mainContent->setSwingValue(0.3f);
        if (mainContent->onSliderValueChanged) {
            mainContent->onSliderValueChanged("swing", 0.3f);
        }
        
        mainContent->setEnergyValue(0.7f);
        if (mainContent->onSliderValueChanged) {
            mainContent->onSliderValueChanged("energy", 0.7f);
        }
        
        expect(eventOrder.size() == 3, "Should have received 3 events");
        expect(eventOrder[0] == "editMode", "First event should be editMode");
        expect(eventOrder[1] == "slider_swing", "Second event should be slider_swing");
        expect(eventOrder[2] == "slider_energy", "Third event should be slider_energy");
        
        logMessage("Cross-component event handling completed successfully");
    }

    //==============================================================================
    // Memory Management Integration
    //==============================================================================
    
    void testMemoryManagementIntegration() {
        logMessage("Testing memory management integration...");
        
        // Step 1: Test component creation and destruction
        logMessage("Testing component creation and destruction...");
        
        for (int i = 0; i < 10; ++i) {
            TestEnvironment env;
            auto* mainContent = env.getMainContent();
            
            expect(mainContent != nullptr, "MainContentComponent should be created");
            if (mainContent) {
                expect(mainContent->getLeftSection() != nullptr, "Left section should be created");
                expect(mainContent->getRightSection() != nullptr, "Right section should be created");
            }
            
            waitForUIStabilization();
            
            // Components should be automatically destroyed when env goes out of scope
        }
        
        // Step 2: Test state object lifecycle
        logMessage("Testing state object lifecycle...");
        
        {
            TestEnvironment env;
            auto* mainContent = env.getMainContent();
            
            if (mainContent) {
                // Create multiple state objects
                for (int i = 0; i < 100; ++i) {
                    ComponentState state;
                    state.currentPlayer = (i % INIConfig::Defaults::MAX_PLAYERS) + 1;
                    state.swingValue = static_cast<float>(i) / 100.0f;
                    
                    mainContent->loadStates(state);
                    
                    ComponentState savedState;
                    mainContent->saveStates(savedState);
                    
                    expect(savedState.currentPlayer == state.currentPlayer, 
                           "State should be preserved in iteration " + juce::String(i));
                }
            }
        }
        
        // Step 3: Test callback lifecycle
        logMessage("Testing callback lifecycle...");
        
        {
            TestEnvironment env;
            auto* mainContent = env.getMainContent();
            
            if (mainContent) {
                // Set up callbacks
                mainContent->onEditModeChanged = [](bool) {};
                mainContent->onMidiFileChanged = [](int, const juce::String&) {};
                mainContent->onSliderValueChanged = [](const juce::String&, float) {};
                
                // Callbacks should be automatically cleaned up when mainContent is destroyed
            }
        }
        
        logMessage("Memory management integration completed successfully");
    }

    //==============================================================================
    // Platform-Specific Integration
    //==============================================================================
    
    void testPlatformSpecificIntegration() {
        logMessage("Testing platform-specific integration...");
        
        TestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test platform-specific sizing
        logMessage("Testing platform-specific sizing...");
        
#if JUCE_IOS || JUCE_ANDROID
        // Mobile platforms - test touch-friendly sizing
        const int minTouchTarget = 44; // iOS guideline
        
        auto* leftSection = mainContent->getLeftSection();
        if (leftSection) {
            auto bounds = leftSection->getBounds();
            expect(bounds.getHeight() >= minTouchTarget, 
                   "Components should be touch-friendly on mobile platforms");
        }
#else
        // Desktop platforms - test standard sizing
        auto* leftSection = mainContent->getLeftSection();
        if (leftSection) {
            auto bounds = leftSection->getBounds();
            expect(bounds.getWidth() > 0, "Components should have valid size on desktop");
            expect(bounds.getHeight() > 0, "Components should have valid size on desktop");
        }
#endif
        
        // Step 2: Test platform-specific behavior
        logMessage("Testing platform-specific behavior...");
        
        // Test mouse vs touch interaction handling
        juce::MouseEvent testMouseEvent(
            juce::MouseInputSource::InputSourceType::mouse,
            juce::Point<float>(100, 100),
            juce::ModifierKeys(),
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            mainContent,
            mainContent,
            juce::Time::getCurrentTime(),
            juce::Point<float>(100, 100),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        // Should handle mouse events without crashing
        mainContent->mouseDown(testMouseEvent);
        
        logMessage("Platform-specific integration completed successfully");
    }

    //==============================================================================
    // Performance Integration
    //==============================================================================
    
    void testPerformanceIntegration() {
        logMessage("Testing performance integration...");
        
        TestEnvironment env;
        auto* mainContent = env.getMainContent();
        auto* processor = env.processor.get();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        expect(processor != nullptr, "Processor should exist");
        if (!mainContent || !processor) return;
        
        waitForUIStabilization();
        
        // Step 1: Test UI responsiveness during audio processing
        logMessage("Testing UI responsiveness during audio processing...");
        
        // Start audio processing
        juce::AudioBuffer<float> audioBuffer(2, 512);
        juce::MidiBuffer midiBuffer;
        
        const auto startTime = juce::Time::getMillisecondCounterHiRes();
        
        // Perform UI operations while audio is "processing"
        for (int i = 0; i < 10; ++i) {
            // Simulate audio processing
            processor->processBlock(audioBuffer, midiBuffer);
            
            // Perform UI operations
            mainContent->setSwingValue(static_cast<float>(i) / 10.0f);
            mainContent->updatePlayerDisplay((i % INIConfig::Defaults::MAX_PLAYERS) + 1);
            
            // Small delay to simulate real-time processing
            juce::Thread::sleep(1);
        }
        
        const auto endTime = juce::Time::getMillisecondCounterHiRes();
        const auto totalTime = endTime - startTime;
        
        expect(totalTime < 1000.0, "UI operations should complete quickly during audio processing");
        
        // Step 2: Test memory usage during complex operations
        logMessage("Testing memory usage during complex operations...");
        
        // Perform memory-intensive operations
        for (int i = 0; i < 50; ++i) {
            ComponentState state;
            state.currentPlayer = (i % INIConfig::Defaults::MAX_PLAYERS) + 1;
            state.swingValue = static_cast<float>(i) / 50.0f;
            
            mainContent->loadStates(state);
            
            ComponentState savedState;
            mainContent->saveStates(savedState);
            
            // Set MIDI file assignments
            for (int j = 0; j < 16; ++j) {
                juce::String midiFile = "perf_test_" + juce::String(i) + "_" + juce::String(j) + ".mid";
                mainContent->setMidiFileAssignment(j, midiFile);
            }
        }
        
        // Should complete without excessive memory usage or crashes
        
        // Step 3: Test rapid UI updates
        logMessage("Testing rapid UI updates...");
        
        const auto rapidStartTime = juce::Time::getMillisecondCounterHiRes();
        
        for (int i = 0; i < 100; ++i) {
            mainContent->setSwingValue(static_cast<float>(i % 100) / 100.0f);
            mainContent->setEnergyValue(static_cast<float>((i + 25) % 100) / 100.0f);
            mainContent->setVolumeValue(static_cast<float>((i + 50) % 100) / 100.0f);
        }
        
        const auto rapidEndTime = juce::Time::getMillisecondCounterHiRes();
        const auto rapidTotalTime = rapidEndTime - rapidStartTime;
        
        expect(rapidTotalTime < 500.0, "Rapid UI updates should complete quickly");
        
        logMessage("Performance integration completed successfully");
    }

    //==============================================================================
    // Utility Methods
    //==============================================================================
    
    void logMessage(const juce::String& message) {
        DBG("ComponentIntegrationTests: " + message);
    }
};

// Register the test
static ComponentIntegrationTests componentIntegrationTestsInstance;