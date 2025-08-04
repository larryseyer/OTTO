#include "KeyboardHandler.h"

KeyboardHandler::KeyboardHandler(FontManager& fontManager, ColorScheme& colorScheme) {
    shortcutsOverlay = std::make_unique<KeyboardShortcutsOverlay>(fontManager, colorScheme);
    setupDefaultShortcuts();
}

bool KeyboardHandler::handlePatternNavigation(const juce::KeyPress& key) {
    if (key == juce::KeyPress::leftKey) {
        if (onPatternNavigation) onPatternNavigation(false);
        return true;
    }
    if (key == juce::KeyPress::rightKey) {
        if (onPatternNavigation) onPatternNavigation(true);
        return true;
    }
    return false;
}

bool KeyboardHandler::handlePlayerSwitching(const juce::KeyPress& key) {
    auto keyCode = key.getKeyCode();
    if (keyCode >= '1' && keyCode <= '8') {
        int playerIndex = keyCode - '1';
        if (onPlayerSwitch) onPlayerSwitch(playerIndex);
        return true;
    }
    return false;
}

bool KeyboardHandler::handleDrumPadTriggers(const juce::KeyPress& key) {
    auto keyCode = key.getKeyCode();
    
    const char* drumPadKeys = "qwertyuiopasdfghjk";
    for (int i = 0; i < 16 && drumPadKeys[i] != '\0'; ++i) {
        if (keyCode == drumPadKeys[i]) {
            if (onDrumPadTrigger) onDrumPadTrigger(i);
            return true;
        }
    }
    return false;
}

bool KeyboardHandler::handleEditModeToggle(const juce::KeyPress& key) {
    if (key == juce::KeyPress::spaceKey) {
        if (onEditModeToggle) onEditModeToggle(true);
        return true;
    }
    return false;
}

bool KeyboardHandler::handleQuickSettings(const juce::KeyPress& key) {
    if (key.getKeyCode() == '?' || (key.getKeyCode() == '/' && key.getModifiers().isShiftDown())) {
        if (shortcutsVisible) {
            hideShortcutsOverlay();
        } else {
            showShortcutsOverlay();
        }
        return true;
    }
    return false;
}

void KeyboardHandler::showShortcutsOverlay() {
    if (shortcutsOverlay) {
        shortcutsOverlay->show();
        shortcutsVisible = true;
    }
}

void KeyboardHandler::hideShortcutsOverlay() {
    if (shortcutsOverlay) {
        shortcutsOverlay->hide();
        shortcutsVisible = false;
    }
}

void KeyboardHandler::setupDefaultShortcuts() {
    if (shortcutsOverlay) {
        shortcutsOverlay->clearShortcuts();
        shortcutsOverlay->addShortcut("Space", "Toggle Edit Mode");
        shortcutsOverlay->addShortcut("1-8", "Select Player");
        shortcutsOverlay->addShortcut("Q-P", "Trigger Drum Pad");
        shortcutsOverlay->addShortcut("Left/Right", "Navigate Patterns");
        shortcutsOverlay->addShortcut("?", "Show/Hide Shortcuts");
    }
}
