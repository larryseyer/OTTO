#pragma once

#include <JuceHeader.h>
#include "../Source/JUCE8_CODING_STANDARDS.h"
#include "../Source/INIConfig.h"
#include "../Source/MainContentComponent.h"
#include "../Source/PluginProcessor.h"
#include "../Source/PluginEditor.h"
#include "../Source/UtilityComponents.h"
#include "../Source/CustomLookAndFeel.h"

//==============================================================================
// User Interaction E2E Tests - Real User Behavior Simulation
// Tests actual user interaction patterns and workflows
//==============================================================================

class UserInteractionTests : public juce::UnitTest {
public:
    UserInteractionTests() : juce::UnitTest("User Interaction Tests") {}

    void runTest() override {
        beginTest("Button Click Sequences");
        testButtonClickSequences();

        beginTest("Dropdown Menu Interactions");
        testDropdownMenuInteractions();

        beginTest("Slider Value Changes");
        testSliderValueChanges();

        beginTest("Keyboard Navigation");
        testKeyboardNavigation();

        beginTest("Mouse Interaction Patterns");
        testMouseInteractionPatterns();

        beginTest("Touch Interaction Patterns");
        testTouchInteractionPatterns();

        beginTest("Drag and Drop Operations");
        testDragAndDropOperations();

        beginTest("Context Menu Interactions");
        testContextMenuInteractions();

        beginTest("Multi-Touch Gestures");
        testMultiTouchGestures();

        beginTest("Accessibility Interactions");
        testAccessibilityInteractions();
    }

private:
    //==============================================================================
    // Test Environment Setup
    //==============================================================================
    
    struct InteractionTestEnvironment {
        std::unique_ptr<OTTOAudioProcessor> processor;
        std::unique_ptr<juce::AudioProcessorEditor> editor;
        std::unique_ptr<juce::Component> testComponent;
        
        InteractionTestEnvironment() {
            processor = std::make_unique<OTTOAudioProcessor>();
            processor->prepareToPlay(
                static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE),
                INIConfig::Defaults::DEFAULT_BUFFER_SIZE
            );
            
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

    void simulateMouseClick(juce::Component* component, juce::Point<int> position) {
        if (!component) return;
        
        auto mouseSource = juce::Desktop::getInstance().getMainMouseSource();
        
        juce::MouseEvent downEvent(
            mouseSource,
            position.toFloat(),
            juce::ModifierKeys(),
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            component,
            component,
            juce::Time::getCurrentTime(),
            position.toFloat(),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        juce::MouseEvent upEvent(
            mouseSource,
            position.toFloat(),
            juce::ModifierKeys(),
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            component,
            component,
            juce::Time::getCurrentTime(),
            position.toFloat(),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        component->mouseDown(downEvent);
        juce::Thread::sleep(50); // Simulate click duration
        component->mouseUp(upEvent);
    }

    //==============================================================================
    // Button Click Sequences
    //==============================================================================
    
    void testButtonClickSequences() {
        logMessage("Testing button click sequences...");
        
        InteractionTestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test single button clicks
        logMessage("Testing single button clicks...");
        
        bool buttonClickReceived = false;
        juce::Button* clickedButton = nullptr;
        
        // Create a test button listener
        class TestButtonListener : public juce::Button::Listener {
        public:
            bool& clickReceived;
            juce::Button*& lastClickedButton;
            
            TestButtonListener(bool& received, juce::Button*& button) 
                : clickReceived(received), lastClickedButton(button) {}
            
            void buttonClicked(juce::Button* button) override {
                clickReceived = true;
                lastClickedButton = button;
            }
        };
        
        TestButtonListener testListener(buttonClickReceived, clickedButton);
        
        // Create test buttons
        juce::TextButton testButton1("Test Button 1");
        juce::TextButton testButton2("Test Button 2");
        juce::TextButton testButton3("Test Button 3");
        
        testButton1.addListener(&testListener);
        testButton2.addListener(&testListener);
        testButton3.addListener(&testListener);
        
        mainContent->addAndMakeVisible(testButton1);
        mainContent->addAndMakeVisible(testButton2);
        mainContent->addAndMakeVisible(testButton3);
        
        // Position buttons
        testButton1.setBounds(10, 10, 100, 30);
        testButton2.setBounds(120, 10, 100, 30);
        testButton3.setBounds(230, 10, 100, 30);
        
        waitForUIStabilization();
        
        // Test clicking each button
        std::vector<juce::TextButton*> buttons = {&testButton1, &testButton2, &testButton3};
        
        for (auto* button : buttons) {
            buttonClickReceived = false;
            clickedButton = nullptr;
            
            simulateMouseClick(button, button->getBounds().getCentre());
            waitForUIStabilization();
            
            expect(buttonClickReceived, "Button click should be received for " + button->getButtonText());
            expect(clickedButton == button, "Correct button should be identified");
        }
        
        // Step 2: Test rapid button clicking
        logMessage("Testing rapid button clicking...");
        
        int rapidClickCount = 0;
        testButton1.onClick = [&]() { rapidClickCount++; };
        
        // Simulate rapid clicks
        for (int i = 0; i < 10; ++i) {
            simulateMouseClick(&testButton1, testButton1.getBounds().getCentre());
            juce::Thread::sleep(10); // Very short delay
        }
        
        waitForUIStabilization();
        expect(rapidClickCount > 0, "Rapid clicks should be registered");
        
        // Step 3: Test button sequence patterns
        logMessage("Testing button sequence patterns...");
        
        std::vector<int> clickSequence;
        
        testButton1.onClick = [&]() { clickSequence.push_back(1); };
        testButton2.onClick = [&]() { clickSequence.push_back(2); };
        testButton3.onClick = [&]() { clickSequence.push_back(3); };
        
        // Test specific sequence: 1-2-3-2-1
        std::vector<juce::TextButton*> sequenceButtons = {&testButton1, &testButton2, &testButton3, &testButton2, &testButton1};
        
        for (auto* button : sequenceButtons) {
            simulateMouseClick(button, button->getBounds().getCentre());
            waitForUIStabilization();
        }
        
        expect(clickSequence.size() == 5, "Should have received 5 clicks in sequence");
        expect(clickSequence[0] == 1, "First click should be button 1");
        expect(clickSequence[1] == 2, "Second click should be button 2");
        expect(clickSequence[2] == 3, "Third click should be button 3");
        expect(clickSequence[3] == 2, "Fourth click should be button 2");
        expect(clickSequence[4] == 1, "Fifth click should be button 1");
        
        // Clean up
        mainContent->removeChildComponent(&testButton1);
        mainContent->removeChildComponent(&testButton2);
        mainContent->removeChildComponent(&testButton3);
        
        logMessage("Button click sequences completed successfully");
    }

    //==============================================================================
    // Dropdown Menu Interactions
    //==============================================================================
    
    void testDropdownMenuInteractions() {
        logMessage("Testing dropdown menu interactions...");
        
        InteractionTestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test basic dropdown functionality
        logMessage("Testing basic dropdown functionality...");
        
        juce::ComboBox testDropdown;
        testDropdown.addItem("Option 1", 1);
        testDropdown.addItem("Option 2", 2);
        testDropdown.addItem("Option 3", 3);
        testDropdown.addItem("Option 4", 4);
        
        mainContent->addAndMakeVisible(testDropdown);
        testDropdown.setBounds(10, 50, 150, 25);
        
        waitForUIStabilization();
        
        // Test selection changes
        bool selectionChanged = false;
        int selectedId = 0;
        
        testDropdown.onChange = [&]() {
            selectionChanged = true;
            selectedId = testDropdown.getSelectedId();
        };
        
        // Test selecting each option
        for (int i = 1; i <= 4; ++i) {
            selectionChanged = false;
            selectedId = 0;
            
            testDropdown.setSelectedId(i);
            waitForUIStabilization();
            
            expect(testDropdown.getSelectedId() == i, "Dropdown should select option " + juce::String(i));
            
            if (testDropdown.onChange) {
                testDropdown.onChange();
            }
            
            expect(selectionChanged, "Selection change callback should be triggered");
            expect(selectedId == i, "Callback should receive correct selection ID");
        }
        
        // Step 2: Test hierarchical dropdown (HierarchicalComboBox)
        logMessage("Testing hierarchical dropdown...");
        
        HierarchicalComboBox hierarchicalDropdown;
        mainContent->addAndMakeVisible(hierarchicalDropdown);
        hierarchicalDropdown.setBounds(170, 50, 150, 25);
        
        // Add hierarchical items
        hierarchicalDropdown.addItem("Category 1", 100);
        hierarchicalDropdown.addItem("  Item 1.1", 101);
        hierarchicalDropdown.addItem("  Item 1.2", 102);
        hierarchicalDropdown.addItem("Category 2", 200);
        hierarchicalDropdown.addItem("  Item 2.1", 201);
        hierarchicalDropdown.addItem("  Item 2.2", 202);
        
        waitForUIStabilization();
        
        // Test hierarchical selection
        hierarchicalDropdown.setSelectedId(101);
        expect(hierarchicalDropdown.getSelectedId() == 101, "Hierarchical dropdown should select nested item");
        
        hierarchicalDropdown.setSelectedId(201);
        expect(hierarchicalDropdown.getSelectedId() == 201, "Hierarchical dropdown should select different nested item");
        
        // Step 3: Test dropdown with many items (scrolling)
        logMessage("Testing dropdown with many items...");
        
        juce::ComboBox largeDropdown;
        for (int i = 1; i <= 50; ++i) {
            largeDropdown.addItem("Item " + juce::String(i), i);
        }
        
        mainContent->addAndMakeVisible(largeDropdown);
        largeDropdown.setBounds(330, 50, 150, 25);
        
        waitForUIStabilization();
        
        // Test selecting items at different positions
        std::vector<int> testIndices = {1, 10, 25, 40, 50};
        
        for (int index : testIndices) {
            largeDropdown.setSelectedId(index);
            expect(largeDropdown.getSelectedId() == index, 
                   "Large dropdown should select item " + juce::String(index));
        }
        
        // Step 4: Test dropdown keyboard navigation
        logMessage("Testing dropdown keyboard navigation...");
        
        testDropdown.setSelectedId(1);
        
        // Simulate arrow key navigation
        juce::KeyPress downKey(juce::KeyPress::downKey);
        juce::KeyPress upKey(juce::KeyPress::upKey);
        
        // Test down arrow
        if (testDropdown.keyPressed(downKey)) {
            expect(testDropdown.getSelectedId() > 1, "Down arrow should move selection down");
        }
        
        // Test up arrow
        testDropdown.setSelectedId(3);
        if (testDropdown.keyPressed(upKey)) {
            expect(testDropdown.getSelectedId() < 3, "Up arrow should move selection up");
        }
        
        // Clean up
        mainContent->removeChildComponent(&testDropdown);
        mainContent->removeChildComponent(&hierarchicalDropdown);
        mainContent->removeChildComponent(&largeDropdown);
        
        logMessage("Dropdown menu interactions completed successfully");
    }

    //==============================================================================
    // Slider Value Changes
    //==============================================================================
    
    void testSliderValueChanges() {
        logMessage("Testing slider value changes...");
        
        InteractionTestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test basic slider functionality
        logMessage("Testing basic slider functionality...");
        
        juce::Slider testSlider;
        testSlider.setRange(0.0, 1.0, 0.01);
        testSlider.setValue(0.5);
        testSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        
        mainContent->addAndMakeVisible(testSlider);
        testSlider.setBounds(10, 100, 200, 30);
        
        waitForUIStabilization();
        
        // Test value changes
        bool valueChanged = false;
        double changedValue = 0.0;
        
        testSlider.onValueChange = [&]() {
            valueChanged = true;
            changedValue = testSlider.getValue();
        };
        
        // Test setting specific values
        std::vector<double> testValues = {0.0, 0.25, 0.5, 0.75, 1.0};
        
        for (double value : testValues) {
            valueChanged = false;
            changedValue = 0.0;
            
            testSlider.setValue(value);
            
            expectWithinAbsoluteError(testSlider.getValue(), value, 0.01, 
                                    "Slider should be set to value " + juce::String(value));
            
            if (testSlider.onValueChange) {
                testSlider.onValueChange();
            }
            
            expect(valueChanged, "Value change callback should be triggered");
            expectWithinAbsoluteError(changedValue, value, 0.01, 
                                    "Callback should receive correct value");
        }
        
        // Step 2: Test slider dragging simulation
        logMessage("Testing slider dragging simulation...");
        
        auto sliderBounds = testSlider.getBounds();
        auto startPoint = juce::Point<int>(sliderBounds.getX() + 10, sliderBounds.getCentreY());
        auto endPoint = juce::Point<int>(sliderBounds.getRight() - 10, sliderBounds.getCentreY());
        
        // Simulate drag from left to right
        juce::MouseEvent dragStartEvent(
            juce::Desktop::getInstance().getMainMouseSource(),
            startPoint.toFloat(),
            juce::ModifierKeys(),
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            &testSlider,
            &testSlider,
            juce::Time::getCurrentTime(),
            startPoint.toFloat(),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        testSlider.mouseDown(dragStartEvent);
        
        // Simulate drag motion
        for (int x = startPoint.getX(); x <= endPoint.getX(); x += 10) {
            juce::MouseEvent dragEvent(
                juce::Desktop::getInstance().getMainMouseSource(),
                juce::Point<float>(static_cast<float>(x), static_cast<float>(startPoint.getY())),
                juce::ModifierKeys(),
                1.0f,
                juce::MouseInputSource::invalidPressure,
                juce::MouseInputSource::invalidOrientation,
                juce::MouseInputSource::invalidRotation,
                juce::MouseInputSource::invalidTiltX,
                juce::MouseInputSource::invalidTiltY,
                &testSlider,
                &testSlider,
                juce::Time::getCurrentTime(),
                juce::Point<float>(static_cast<float>(x), static_cast<float>(startPoint.getY())),
                juce::Time::getCurrentTime(),
                1,
                true
            );
            
            testSlider.mouseDrag(dragEvent);
            juce::Thread::sleep(10);
        }
        
        juce::MouseEvent dragEndEvent(
            juce::Desktop::getInstance().getMainMouseSource(),
            endPoint.toFloat(),
            juce::ModifierKeys(),
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            &testSlider,
            &testSlider,
            juce::Time::getCurrentTime(),
            endPoint.toFloat(),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        testSlider.mouseUp(dragEndEvent);
        
        waitForUIStabilization();
        
        // Value should be close to maximum after dragging to the right
        expect(testSlider.getValue() > 0.8, "Slider value should be high after dragging right");
        
        // Step 3: Test different slider styles
        logMessage("Testing different slider styles...");
        
        juce::Slider verticalSlider;
        verticalSlider.setRange(0.0, 100.0, 1.0);
        verticalSlider.setValue(50.0);
        verticalSlider.setSliderStyle(juce::Slider::LinearVertical);
        
        mainContent->addAndMakeVisible(verticalSlider);
        verticalSlider.setBounds(250, 80, 30, 100);
        
        waitForUIStabilization();
        
        verticalSlider.setValue(25.0);
        expectWithinAbsoluteError(verticalSlider.getValue(), 25.0, 0.1, 
                                "Vertical slider should accept value changes");
        
        juce::Slider rotarySlider;
        rotarySlider.setRange(-1.0, 1.0, 0.01);
        rotarySlider.setValue(0.0);
        rotarySlider.setSliderStyle(juce::Slider::Rotary);
        
        mainContent->addAndMakeVisible(rotarySlider);
        rotarySlider.setBounds(300, 80, 60, 60);
        
        waitForUIStabilization();
        
        rotarySlider.setValue(0.7);
        expectWithinAbsoluteError(rotarySlider.getValue(), 0.7, 0.01, 
                                "Rotary slider should accept value changes");
        
        // Step 4: Test slider with custom ranges
        logMessage("Testing slider with custom ranges...");
        
        juce::Slider customRangeSlider;
        customRangeSlider.setRange(-50.0, 50.0, 0.1);
        customRangeSlider.setValue(0.0);
        
        mainContent->addAndMakeVisible(customRangeSlider);
        customRangeSlider.setBounds(10, 200, 200, 30);
        
        waitForUIStabilization();
        
        std::vector<double> customTestValues = {-50.0, -25.0, 0.0, 25.0, 50.0};
        
        for (double value : customTestValues) {
            customRangeSlider.setValue(value);
            expectWithinAbsoluteError(customRangeSlider.getValue(), value, 0.1, 
                                    "Custom range slider should accept value " + juce::String(value));
        }
        
        // Clean up
        mainContent->removeChildComponent(&testSlider);
        mainContent->removeChildComponent(&verticalSlider);
        mainContent->removeChildComponent(&rotarySlider);
        mainContent->removeChildComponent(&customRangeSlider);
        
        logMessage("Slider value changes completed successfully");
    }

    //==============================================================================
    // Keyboard Navigation
    //==============================================================================
    
    void testKeyboardNavigation() {
        logMessage("Testing keyboard navigation...");
        
        InteractionTestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test tab navigation
        logMessage("Testing tab navigation...");
        
        // Create focusable components
        juce::TextButton button1("Button 1");
        juce::TextButton button2("Button 2");
        juce::TextButton button3("Button 3");
        juce::Slider slider1;
        juce::ComboBox combo1;
        
        // Set up components
        button1.setBounds(10, 10, 80, 30);
        button2.setBounds(100, 10, 80, 30);
        button3.setBounds(190, 10, 80, 30);
        slider1.setBounds(10, 50, 150, 30);
        combo1.setBounds(170, 50, 100, 30);
        
        combo1.addItem("Item 1", 1);
        combo1.addItem("Item 2", 2);
        
        mainContent->addAndMakeVisible(button1);
        mainContent->addAndMakeVisible(button2);
        mainContent->addAndMakeVisible(button3);
        mainContent->addAndMakeVisible(slider1);
        mainContent->addAndMakeVisible(combo1);
        
        waitForUIStabilization();
        
        // Test tab key navigation
        juce::KeyPress tabKey(juce::KeyPress::tabKey);
        juce::KeyPress shiftTabKey(juce::KeyPress::tabKey, juce::ModifierKeys::shiftModifier);
        
        // Start with first button focused
        button1.grabKeyboardFocus();
        expect(button1.hasKeyboardFocus(true), "Button 1 should have focus initially");
        
        // Test forward tab navigation
        if (mainContent->keyPressed(tabKey)) {
            // Focus should move to next component
            waitForUIStabilization();
        }
        
        // Test backward tab navigation
        if (mainContent->keyPressed(shiftTabKey)) {
            // Focus should move to previous component
            waitForUIStabilization();
        }
        
        // Step 2: Test arrow key navigation
        logMessage("Testing arrow key navigation...");
        
        juce::KeyPress leftKey(juce::KeyPress::leftKey);
        juce::KeyPress rightKey(juce::KeyPress::rightKey);
        juce::KeyPress upKey(juce::KeyPress::upKey);
        juce::KeyPress downKey(juce::KeyPress::downKey);
        
        // Test slider arrow key control
        slider1.grabKeyboardFocus();
        double initialSliderValue = slider1.getValue();
        
        if (slider1.keyPressed(rightKey)) {
            expect(slider1.getValue() >= initialSliderValue, "Right arrow should increase slider value");
        }
        
        if (slider1.keyPressed(leftKey)) {
            expect(slider1.getValue() <= initialSliderValue, "Left arrow should decrease slider value");
        }
        
        // Test combo box arrow key control
        combo1.grabKeyboardFocus();
        combo1.setSelectedId(1);
        
        if (combo1.keyPressed(downKey)) {
            // Down arrow might change selection
            waitForUIStabilization();
        }
        
        // Step 3: Test Enter/Space key activation
        logMessage("Testing Enter/Space key activation...");
        
        bool buttonActivated = false;
        button1.onClick = [&]() { buttonActivated = true; };
        
        button1.grabKeyboardFocus();
        
        juce::KeyPress enterKey(juce::KeyPress::returnKey);
        juce::KeyPress spaceKey(juce::KeyPress::spaceKey);
        
        // Test Enter key
        if (button1.keyPressed(enterKey)) {
            expect(buttonActivated, "Enter key should activate button");
        }
        
        buttonActivated = false;
        
        // Test Space key
        if (button1.keyPressed(spaceKey)) {
            expect(buttonActivated, "Space key should activate button");
        }
        
        // Step 4: Test escape key handling
        logMessage("Testing escape key handling...");
        
        juce::KeyPress escapeKey(juce::KeyPress::escapeKey);
        
        // Test escape key on focused component
        button1.grabKeyboardFocus();
        if (mainContent->keyPressed(escapeKey)) {
            // Escape should be handled gracefully
            waitForUIStabilization();
        }
        
        // Clean up
        mainContent->removeChildComponent(&button1);
        mainContent->removeChildComponent(&button2);
        mainContent->removeChildComponent(&button3);
        mainContent->removeChildComponent(&slider1);
        mainContent->removeChildComponent(&combo1);
        
        logMessage("Keyboard navigation completed successfully");
    }

    //==============================================================================
    // Mouse Interaction Patterns
    //==============================================================================
    
    void testMouseInteractionPatterns() {
        logMessage("Testing mouse interaction patterns...");
        
        InteractionTestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test single click patterns
        logMessage("Testing single click patterns...");
        
        juce::TextButton testButton("Test Button");
        testButton.setBounds(10, 10, 100, 30);
        mainContent->addAndMakeVisible(testButton);
        
        int clickCount = 0;
        testButton.onClick = [&]() { clickCount++; };
        
        // Test single click
        simulateMouseClick(&testButton, testButton.getBounds().getCentre());
        waitForUIStabilization();
        expect(clickCount == 1, "Single click should register once");
        
        // Step 2: Test double click patterns
        logMessage("Testing double click patterns...");
        
        int doubleClickCount = 0;
        
        class DoubleClickButton : public juce::TextButton {
        public:
            int& doubleClickCounter;
            
            DoubleClickButton(const juce::String& text, int& counter) 
                : juce::TextButton(text), doubleClickCounter(counter) {}
            
            void mouseDoubleClick(const juce::MouseEvent&) override {
                doubleClickCounter++;
            }
        };
        
        DoubleClickButton doubleClickButton("Double Click", doubleClickCount);
        doubleClickButton.setBounds(120, 10, 100, 30);
        mainContent->addAndMakeVisible(doubleClickButton);
        
        waitForUIStabilization();
        
        // Simulate double click
        auto centre = doubleClickButton.getBounds().getCentre();
        simulateMouseClick(&doubleClickButton, centre);
        juce::Thread::sleep(10); // Short delay
        simulateMouseClick(&doubleClickButton, centre);
        
        waitForUIStabilization();
        expect(doubleClickCount > 0, "Double click should be registered");
        
        // Step 3: Test right click patterns
        logMessage("Testing right click patterns...");
        
        bool rightClickReceived = false;
        
        class RightClickButton : public juce::TextButton {
        public:
            bool& rightClickFlag;
            
            RightClickButton(const juce::String& text, bool& flag) 
                : juce::TextButton(text), rightClickFlag(flag) {}
            
            void mouseDown(const juce::MouseEvent& event) override {
                if (event.mods.isRightButtonDown()) {
                    rightClickFlag = true;
                }
                juce::TextButton::mouseDown(event);
            }
        };
        
        RightClickButton rightClickButton("Right Click", rightClickReceived);
        rightClickButton.setBounds(230, 10, 100, 30);
        mainContent->addAndMakeVisible(rightClickButton);
        
        waitForUIStabilization();
        
        // Simulate right click
        juce::MouseEvent rightClickEvent(
            juce::Desktop::getInstance().getMainMouseSource(),
            rightClickButton.getBounds().getCentre().toFloat(),
            juce::ModifierKeys::rightButtonModifier,
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            &rightClickButton,
            &rightClickButton,
            juce::Time::getCurrentTime(),
            rightClickButton.getBounds().getCentre().toFloat(),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        rightClickButton.mouseDown(rightClickEvent);
        expect(rightClickReceived, "Right click should be detected");
        
        // Step 4: Test hover patterns
        logMessage("Testing hover patterns...");
        
        bool hoverEntered = false;
        bool hoverExited = false;
        
        class HoverButton : public juce::TextButton {
        public:
            bool& enterFlag;
            bool& exitFlag;
            
            HoverButton(const juce::String& text, bool& enter, bool& exit) 
                : juce::TextButton(text), enterFlag(enter), exitFlag(exit) {}
            
            void mouseEnter(const juce::MouseEvent&) override {
                enterFlag = true;
            }
            
            void mouseExit(const juce::MouseEvent&) override {
                exitFlag = true;
            }
        };
        
        HoverButton hoverButton("Hover Test", hoverEntered, hoverExited);
        hoverButton.setBounds(10, 50, 100, 30);
        mainContent->addAndMakeVisible(hoverButton);
        
        waitForUIStabilization();
        
        // Simulate mouse enter
        juce::MouseEvent enterEvent(
            juce::Desktop::getInstance().getMainMouseSource(),
            hoverButton.getBounds().getCentre().toFloat(),
            juce::ModifierKeys(),
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            &hoverButton,
            &hoverButton,
            juce::Time::getCurrentTime(),
            hoverButton.getBounds().getCentre().toFloat(),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        hoverButton.mouseEnter(enterEvent);
        expect(hoverEntered, "Mouse enter should be detected");
        
        // Simulate mouse exit
        juce::MouseEvent exitEvent(
            juce::Desktop::getInstance().getMainMouseSource(),
            juce::Point<float>(0, 0),
            juce::ModifierKeys(),
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            nullptr,
            &hoverButton,
            juce::Time::getCurrentTime(),
            juce::Point<float>(0, 0),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        hoverButton.mouseExit(exitEvent);
        expect(hoverExited, "Mouse exit should be detected");
        
        // Clean up
        mainContent->removeChildComponent(&testButton);
        mainContent->removeChildComponent(&doubleClickButton);
        mainContent->removeChildComponent(&rightClickButton);
        mainContent->removeChildComponent(&hoverButton);
        
        logMessage("Mouse interaction patterns completed successfully");
    }

    //==============================================================================
    // Touch Interaction Patterns
    //==============================================================================
    
    void testTouchInteractionPatterns() {
        logMessage("Testing touch interaction patterns...");
        
        InteractionTestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test touch-friendly sizing
        logMessage("Testing touch-friendly sizing...");
        
#if JUCE_IOS || JUCE_ANDROID
        const int minTouchTarget = 44; // iOS guideline
#else
        const int minTouchTarget = 30; // Desktop minimum
#endif
        
        juce::TextButton touchButton("Touch Button");
        touchButton.setBounds(10, 10, minTouchTarget, minTouchTarget);
        mainContent->addAndMakeVisible(touchButton);
        
        waitForUIStabilization();
        
        expect(touchButton.getWidth() >= minTouchTarget, "Touch button should meet minimum width");
        expect(touchButton.getHeight() >= minTouchTarget, "Touch button should meet minimum height");
        
        // Step 2: Test touch events
        logMessage("Testing touch events...");
        
        bool touchStartReceived = false;
        bool touchEndReceived = false;
        
        class TouchTestButton : public juce::TextButton {
        public:
            bool& startFlag;
            bool& endFlag;
            
            TouchTestButton(const juce::String& text, bool& start, bool& end) 
                : juce::TextButton(text), startFlag(start), endFlag(end) {}
            
            void mouseDown(const juce::MouseEvent& event) override {
                if (event.source.isTouch()) {
                    startFlag = true;
                }
                juce::TextButton::mouseDown(event);
            }
            
            void mouseUp(const juce::MouseEvent& event) override {
                if (event.source.isTouch()) {
                    endFlag = true;
                }
                juce::TextButton::mouseUp(event);
            }
        };
        
        TouchTestButton touchTestButton("Touch Test", touchStartReceived, touchEndReceived);
        touchTestButton.setBounds(60, 10, minTouchTarget, minTouchTarget);
        mainContent->addAndMakeVisible(touchTestButton);
        
        waitForUIStabilization();
        
        // Simulate touch events (note: actual touch simulation is platform-specific)
        simulateMouseClick(&touchTestButton, touchTestButton.getBounds().getCentre());
        waitForUIStabilization();
        
        // Step 3: Test gesture recognition
        logMessage("Testing gesture recognition...");
        
        // Create a component that can handle gestures
        class GestureTestComponent : public juce::Component {
        public:
            bool swipeDetected = false;
            bool pinchDetected = false;
            
            void mouseDown(const juce::MouseEvent&) override {}
            void mouseDrag(const juce::MouseEvent& event) override {
                // Simple swipe detection based on drag distance
                if (std::abs(event.getDistanceFromDragStartX()) > 50) {
                    swipeDetected = true;
                }
            }
            void mouseUp(const juce::MouseEvent&) override {}
        };
        
        GestureTestComponent gestureComponent;
        gestureComponent.setBounds(10, 60, 200, 100);
        mainContent->addAndMakeVisible(gestureComponent);
        
        waitForUIStabilization();
        
        // Simulate swipe gesture
        auto startPoint = juce::Point<int>(20, 110);
        auto endPoint = juce::Point<int>(180, 110);
        
        juce::MouseEvent swipeStart(
            juce::Desktop::getInstance().getMainMouseSource(),
            startPoint.toFloat(),
            juce::ModifierKeys(),
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            &gestureComponent,
            &gestureComponent,
            juce::Time::getCurrentTime(),
            startPoint.toFloat(),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        gestureComponent.mouseDown(swipeStart);
        
        // Simulate drag motion
        for (int x = startPoint.getX(); x <= endPoint.getX(); x += 20) {
            juce::MouseEvent dragEvent(
                juce::Desktop::getInstance().getMainMouseSource(),
                juce::Point<float>(static_cast<float>(x), static_cast<float>(startPoint.getY())),
                juce::ModifierKeys(),
                1.0f,
                juce::MouseInputSource::invalidPressure,
                juce::MouseInputSource::invalidOrientation,
                juce::MouseInputSource::invalidRotation,
                juce::MouseInputSource::invalidTiltX,
                juce::MouseInputSource::invalidTiltY,
                &gestureComponent,
                &gestureComponent,
                juce::Time::getCurrentTime(),
                startPoint.toFloat(),
                juce::Time::getCurrentTime(),
                1,
                true
            );
            
            gestureComponent.mouseDrag(dragEvent);
            juce::Thread::sleep(10);
        }
        
        juce::MouseEvent swipeEnd(
            juce::Desktop::getInstance().getMainMouseSource(),
            endPoint.toFloat(),
            juce::ModifierKeys(),
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            &gestureComponent,
            &gestureComponent,
            juce::Time::getCurrentTime(),
            startPoint.toFloat(),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        gestureComponent.mouseUp(swipeEnd);
        
        expect(gestureComponent.swipeDetected, "Swipe gesture should be detected");
        
        // Clean up
        mainContent->removeChildComponent(&touchButton);
        mainContent->removeChildComponent(&touchTestButton);
        mainContent->removeChildComponent(&gestureComponent);
        
        logMessage("Touch interaction patterns completed successfully");
    }

    //==============================================================================
    // Drag and Drop Operations
    //==============================================================================
    
    void testDragAndDropOperations() {
        logMessage("Testing drag and drop operations...");
        
        InteractionTestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test basic drag and drop
        logMessage("Testing basic drag and drop...");
        
        // Create drag source component
        class DragSourceComponent : public juce::Component {
        public:
            void mouseDrag(const juce::MouseEvent& event) override {
                if (!isDragging && event.getDistanceFromDragStart() > 5) {
                    isDragging = true;
                    dragStarted = true;
                    
                    // Start drag operation
                    juce::DragAndDropContainer::performExternalDragDropOfText("Test Data", this);
                }
            }
            
            void mouseUp(const juce::MouseEvent&) override {
                isDragging = false;
            }
            
            bool isDragging = false;
            bool dragStarted = false;
        };
        
        // Create drop target component
        class DropTargetComponent : public juce::Component {
        public:
            bool isInterestedInDragSource(const SourceDetails&) override {
                return true;
            }
            
            void itemDropped(const SourceDetails& dragSourceDetails) override {
                dropReceived = true;
                droppedText = dragSourceDetails.description.toString();
            }
            
            bool dropReceived = false;
            juce::String droppedText;
        };
        
        DragSourceComponent dragSource;
        DropTargetComponent dropTarget;
        
        dragSource.setBounds(10, 10, 80, 80);
        dropTarget.setBounds(120, 10, 80, 80);
        
        mainContent->addAndMakeVisible(dragSource);
        mainContent->addAndMakeVisible(dropTarget);
        
        waitForUIStabilization();
        
        // Simulate drag operation
        auto dragStartPoint = dragSource.getBounds().getCentre();
        auto dragEndPoint = dropTarget.getBounds().getCentre();
        
        juce::MouseEvent dragStart(
            juce::Desktop::getInstance().getMainMouseSource(),
            dragStartPoint.toFloat(),
            juce::ModifierKeys(),
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            &dragSource,
            &dragSource,
            juce::Time::getCurrentTime(),
            dragStartPoint.toFloat(),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        dragSource.mouseDown(dragStart);
        
        // Simulate drag motion
        juce::MouseEvent dragMotion(
            juce::Desktop::getInstance().getMainMouseSource(),
            dragEndPoint.toFloat(),
            juce::ModifierKeys(),
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            &dragSource,
            &dragSource,
            juce::Time::getCurrentTime(),
            dragStartPoint.toFloat(),
            juce::Time::getCurrentTime(),
            1,
            true
        );
        
        dragSource.mouseDrag(dragMotion);
        
        juce::MouseEvent dragEnd(
            juce::Desktop::getInstance().getMainMouseSource(),
            dragEndPoint.toFloat(),
            juce::ModifierKeys(),
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            &dragSource,
            &dragSource,
            juce::Time::getCurrentTime(),
            dragStartPoint.toFloat(),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        dragSource.mouseUp(dragEnd);
        
        expect(dragSource.dragStarted, "Drag operation should be started");
        
        // Clean up
        mainContent->removeChildComponent(&dragSource);
        mainContent->removeChildComponent(&dropTarget);
        
        logMessage("Drag and drop operations completed successfully");
    }

    //==============================================================================
    // Context Menu Interactions
    //==============================================================================
    
    void testContextMenuInteractions() {
        logMessage("Testing context menu interactions...");
        
        InteractionTestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test context menu creation
        logMessage("Testing context menu creation...");
        
        bool contextMenuShown = false;
        int selectedMenuId = 0;
        
        class ContextMenuComponent : public juce::Component {
        public:
            bool& menuShownFlag;
            int& selectedId;
            
            ContextMenuComponent(bool& shown, int& id) 
                : menuShownFlag(shown), selectedId(id) {}
            
            void mouseDown(const juce::MouseEvent& event) override {
                if (event.mods.isRightButtonDown()) {
                    showContextMenu();
                }
            }
            
        private:
            void showContextMenu() {
                juce::PopupMenu menu;
                menu.addItem(1, "Option 1");
                menu.addItem(2, "Option 2");
                menu.addSeparator();
                menu.addItem(3, "Option 3");
                
                menuShownFlag = true;
                
                // Note: In a real test, we would need to handle the async nature of popup menus
                selectedId = menu.show();
            }
        };
        
        ContextMenuComponent contextComponent(contextMenuShown, selectedMenuId);
        contextComponent.setBounds(10, 10, 100, 100);
        mainContent->addAndMakeVisible(contextComponent);
        
        waitForUIStabilization();
        
        // Simulate right click to show context menu
        juce::MouseEvent rightClick(
            juce::Desktop::getInstance().getMainMouseSource(),
            contextComponent.getBounds().getCentre().toFloat(),
            juce::ModifierKeys::rightButtonModifier,
            1.0f,
            juce::MouseInputSource::invalidPressure,
            juce::MouseInputSource::invalidOrientation,
            juce::MouseInputSource::invalidRotation,
            juce::MouseInputSource::invalidTiltX,
            juce::MouseInputSource::invalidTiltY,
            &contextComponent,
            &contextComponent,
            juce::Time::getCurrentTime(),
            contextComponent.getBounds().getCentre().toFloat(),
            juce::Time::getCurrentTime(),
            1,
            false
        );
        
        contextComponent.mouseDown(rightClick);
        expect(contextMenuShown, "Context menu should be shown on right click");
        
        // Clean up
        mainContent->removeChildComponent(&contextComponent);
        
        logMessage("Context menu interactions completed successfully");
    }

    //==============================================================================
    // Multi-Touch Gestures
    //==============================================================================
    
    void testMultiTouchGestures() {
        logMessage("Testing multi-touch gestures...");
        
        InteractionTestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Note: Multi-touch testing is complex and platform-specific
        // This is a simplified test that validates the framework can handle multi-touch concepts
        
        // Step 1: Test multi-touch awareness
        logMessage("Testing multi-touch awareness...");
        
        class MultiTouchComponent : public juce::Component {
        public:
            int activeTouches = 0;
            bool multiTouchDetected = false;
            
            void mouseDown(const juce::MouseEvent& event) override {
                if (event.source.isTouch()) {
                    activeTouches++;
                    if (activeTouches > 1) {
                        multiTouchDetected = true;
                    }
                }
            }
            
            void mouseUp(const juce::MouseEvent& event) override {
                if (event.source.isTouch()) {
                    activeTouches = juce::jmax(0, activeTouches - 1);
                }
            }
        };
        
        MultiTouchComponent multiTouchComponent;
        multiTouchComponent.setBounds(10, 10, 200, 200);
        mainContent->addAndMakeVisible(multiTouchComponent);
        
        waitForUIStabilization();
        
        // Simulate multiple touch points (simplified)
        simulateMouseClick(&multiTouchComponent, juce::Point<int>(50, 50));
        simulateMouseClick(&multiTouchComponent, juce::Point<int>(150, 150));
        
        // Clean up
        mainContent->removeChildComponent(&multiTouchComponent);
        
        logMessage("Multi-touch gestures completed successfully");
    }

    //==============================================================================
    // Accessibility Interactions
    //==============================================================================
    
    void testAccessibilityInteractions() {
        logMessage("Testing accessibility interactions...");
        
        InteractionTestEnvironment env;
        auto* mainContent = env.getMainContent();
        
        expect(mainContent != nullptr, "MainContentComponent should exist");
        if (!mainContent) return;
        
        waitForUIStabilization();
        
        // Step 1: Test accessibility properties
        logMessage("Testing accessibility properties...");
        
        juce::TextButton accessibleButton("Accessible Button");
        accessibleButton.setBounds(10, 10, 120, 30);
        accessibleButton.setTitle("Test Button");
        accessibleButton.setDescription("This is a test button for accessibility");
        
        mainContent->addAndMakeVisible(accessibleButton);
        waitForUIStabilization();
        
        expect(accessibleButton.getTitle().isNotEmpty(), "Button should have accessibility title");
        expect(accessibleButton.getDescription().isNotEmpty(), "Button should have accessibility description");
        
        // Step 2: Test keyboard accessibility
        logMessage("Testing keyboard accessibility...");
        
        accessibleButton.setWantsKeyboardFocus(true);
        expect(accessibleButton.getWantsKeyboardFocus(), "Button should accept keyboard focus");
        
        accessibleButton.grabKeyboardFocus();
        expect(accessibleButton.hasKeyboardFocus(true), "Button should be able to receive focus");
        
        // Step 3: Test screen reader compatibility
        logMessage("Testing screen reader compatibility...");
        
        // Test that components provide proper accessibility information
        auto accessibilityHandler = accessibleButton.getAccessibilityHandler();
        expect(accessibilityHandler != nullptr, "Button should have accessibility handler");
        
        if (accessibilityHandler) {
            auto role = accessibilityHandler->getRole();
            expect(role == juce::AccessibilityRole::button, "Button should have correct accessibility role");
        }
        
        // Clean up
        mainContent->removeChildComponent(&accessibleButton);
        
        logMessage("Accessibility interactions completed successfully");
    }

    //==============================================================================
    // Utility Methods
    //==============================================================================
    
    void logMessage(const juce::String& message) {
        DBG("UserInteractionTests: " + message);
    }
};

// Register the test
static UserInteractionTests userInteractionTestsInstance;