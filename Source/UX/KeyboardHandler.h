#pragma once
#include <JuceHeader.h>
#include "../UtilityComponents.h"

class KeyboardHandler {
public:
    KeyboardHandler(FontManager& fontManager, ColorScheme& colorScheme);
    
    bool handlePatternNavigation(const juce::KeyPress& key);
    bool handlePlayerSwitching(const juce::KeyPress& key);
    bool handleDrumPadTriggers(const juce::KeyPress& key);
    bool handleEditModeToggle(const juce::KeyPress& key);
    bool handleQuickSettings(const juce::KeyPress& key);
    
    std::function<void(bool)> onEditModeToggle;
    std::function<void(int)> onPlayerSwitch;
    std::function<void(int)> onDrumPadTrigger;
    std::function<void(bool)> onPatternNavigation;
    
    void showShortcutsOverlay();
    void hideShortcutsOverlay();
    
private:
    std::unique_ptr<KeyboardShortcutsOverlay> shortcutsOverlay;
    bool shortcutsVisible = false;
    
    void setupDefaultShortcuts();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardHandler)
};
