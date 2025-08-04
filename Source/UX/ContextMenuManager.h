#pragma once
#include <JuceHeader.h>
#include "../ColorScheme.h"
#include "../FontManager.h"

class ContextMenuManager {
public:
    ContextMenuManager(ColorScheme& colorScheme, FontManager& fontManager);
    
    void showPatternGroupMenu(juce::Component* parent, juce::Point<int> position);
    void showDrumPadMenu(juce::Component* parent, juce::Point<int> position, int padIndex);
    void showPlayerMenu(juce::Component* parent, juce::Point<int> position, int playerIndex);
    
    std::function<void(const juce::String&)> onPatternGroupAction;
    std::function<void(int, const juce::String&)> onDrumPadAction;
    std::function<void(int, const juce::String&)> onPlayerAction;
    
private:
    ColorScheme& colorScheme;
    FontManager& fontManager;
    
    void stylePopupMenu(juce::PopupMenu& menu);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContextMenuManager)
};
