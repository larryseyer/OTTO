#include "ContextMenuManager.h"

ContextMenuManager::ContextMenuManager(ColorScheme& colorScheme, FontManager& fontManager)
    : colorScheme(colorScheme), fontManager(fontManager) {
}

void ContextMenuManager::showPatternGroupMenu(juce::Component* parent, juce::Point<int> position) {
    juce::PopupMenu menu;
    stylePopupMenu(menu);
    
    menu.addItem(1, "Duplicate Pattern Group");
    menu.addItem(2, "Delete Pattern Group");
    menu.addSeparator();
    menu.addItem(3, "Add to Favorites");
    menu.addItem(4, "Export Pattern Group");
    
    auto result = menu.show();
    if (result > 0 && onPatternGroupAction) {
        juce::String action;
        switch (result) {
            case 1: action = "duplicate"; break;
            case 2: action = "delete"; break;
            case 3: action = "favorite"; break;
            case 4: action = "export"; break;
        }
        onPatternGroupAction(action);
    }
}

void ContextMenuManager::showDrumPadMenu(juce::Component* parent, juce::Point<int> position, int padIndex) {
    juce::PopupMenu menu;
    stylePopupMenu(menu);
    
    menu.addItem(1, "Assign MIDI File...");
    menu.addItem(2, "Clear Assignment");
    menu.addSeparator();
    menu.addItem(3, "Copy Pad Settings");
    menu.addItem(4, "Paste Pad Settings");
    menu.addSeparator();
    menu.addItem(5, "Mute Pad");
    menu.addItem(6, "Solo Pad");
    
    auto result = menu.show();
    if (result > 0 && onDrumPadAction) {
        juce::String action;
        switch (result) {
            case 1: action = "assign"; break;
            case 2: action = "clear"; break;
            case 3: action = "copy"; break;
            case 4: action = "paste"; break;
            case 5: action = "mute"; break;
            case 6: action = "solo"; break;
        }
        onDrumPadAction(padIndex, action);
    }
}

void ContextMenuManager::showPlayerMenu(juce::Component* parent, juce::Point<int> position, int playerIndex) {
    juce::PopupMenu menu;
    stylePopupMenu(menu);
    
    menu.addItem(1, "Duplicate Player");
    menu.addItem(2, "Clear Player");
    menu.addSeparator();
    menu.addItem(3, "Mute Player");
    menu.addItem(4, "Solo Player");
    menu.addSeparator();
    menu.addItem(5, "Export Player Settings");
    
    auto result = menu.show();
    if (result > 0 && onPlayerAction) {
        juce::String action;
        switch (result) {
            case 1: action = "duplicate"; break;
            case 2: action = "clear"; break;
            case 3: action = "mute"; break;
            case 4: action = "solo"; break;
            case 5: action = "export"; break;
        }
        onPlayerAction(playerIndex, action);
    }
}

void ContextMenuManager::stylePopupMenu(juce::PopupMenu& menu) {
    juce::ignoreUnused(menu);
}
