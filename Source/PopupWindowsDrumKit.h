#pragma once
#include <JuceHeader.h>
#include "PopupWindows.h"
#include "SFZEngine.h"
#include "ColorScheme.h"
#include "FontManager.h"
#include "ResponsiveLayoutManager.h"
#include "INIDataManager.h"
#include "INIConfig.h"
#include "ComponentState.h"

class PresetManager;
class Mixer;

/**
 * DrumKit Selection Popup Window
 * 
 * This popup window contains all drumkit-related controls that were previously
 * in the main GUI's right sidebar. Following JUCE 8 patterns and the existing
 * popup window architecture.
 * 
 * Controls included:
 * - DrumKit selection dropdown
 * - SFZ file selection dropdown  
 * - Refresh, Load, Edit, Mixer buttons
 * - Power toggle
 */
class DrumKitSelectionWindow : public juce::DocumentWindow {
public:
    DrumKitSelectionWindow(PresetManager& presetManager,
                          SFZEngine& sfzEngine,
                          ColorScheme& colorScheme,
                          FontManager& fontManager,
                          ResponsiveLayoutManager& layoutManager,
                          INIDataManager& iniManager,
                          Mixer* mixer = nullptr);

    ~DrumKitSelectionWindow() override;

    void closeButtonPressed() override;
    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);
    void setCurrentPlayerIndex(int playerIndex);
    
    // Callbacks for main GUI integration
    std::function<void()> onEditRequested;
    std::function<void()> onMixerRequested;
    std::function<void(bool)> onPowerStateChanged;
    std::function<void(int)> onKitChanged;

private:
    class DrumKitContent;
    std::unique_ptr<DrumKitContent> content;

    PresetManager& presetManager;
    SFZEngine& sfzEngine;
    ColorScheme& colorScheme;
    FontManager& fontManager;
    ResponsiveLayoutManager& layoutManager;
    INIDataManager& iniManager;
    Mixer* mixer;
    
    int currentPlayerIndex = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumKitSelectionWindow)
};

/**
 * Internal content component for the DrumKit selection window
 * Handles the actual UI layout and interactions
 */
class DrumKitSelectionWindow::DrumKitContent : public juce::Component,
                                               public juce::ComboBox::Listener,
                                               public juce::Button::Listener,
                                               public ColorScheme::Listener
{
public:
    DrumKitContent(PresetManager& presetManager,
                   SFZEngine& sfzEngine,
                   ColorScheme& colorScheme,
                   FontManager& fontManager,
                   ResponsiveLayoutManager& layoutManager,
                   INIDataManager& iniManager,
                   Mixer* mixer);

    ~DrumKitContent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void lookAndFeelChanged() override;

    // juce::ComboBox::Listener
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    
    // juce::Button::Listener  
    void buttonClicked(juce::Button* button) override;

    // ColorScheme::Listener
    void themeChanged(const juce::String& newThemeName) override;
    void colorChanged(const juce::String& component, const juce::String& property, const juce::Colour& newColor) override;

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);
    void setCurrentPlayerIndex(int playerIndex);
    
    // Forward callbacks to parent window
    std::function<void()> onEditRequested;
    std::function<void()> onMixerRequested;
    std::function<void(bool)> onPowerStateChanged;
    std::function<void(int)> onKitChanged;

private:
    PresetManager& presetManager;
    SFZEngine& sfzEngine;
    ColorScheme& colorScheme;
    FontManager& fontManager;
    ResponsiveLayoutManager& layoutManager;
    INIDataManager& iniManager;
    Mixer* mixer;

    // UI Components (moved from DrumKitSectionComponent)
    std::unique_ptr<juce::ComboBox> drumkitComboBox;
    std::unique_ptr<juce::ComboBox> sfzFileComboBox;
    std::unique_ptr<juce::Label> drumkitLabel;
    std::unique_ptr<juce::Label> sfzFileLabel;
    std::unique_ptr<juce::TextButton> refreshButton;
    std::unique_ptr<juce::TextButton> loadButton;
    std::unique_ptr<juce::TextButton> editButton;
    std::unique_ptr<juce::TextButton> mixerButton;
    std::unique_ptr<juce::ToggleButton> powerButton;
    
    // Popup windows for editor and mixer
    std::unique_ptr<DrumKitEditorWindow> drumKitEditorWindow;
    std::unique_ptr<DrumKitMixerWindow> drumKitMixerWindow;

    int currentPlayerIndex = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;

    void updateDrumkitList();
    void updateSFZFileList();
    void loadSelectedDrumkit();
    void refreshDrumkits();
    void applyColorScheme();
    void setupComponents();
    void updateFontsAndColors();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumKitContent)
};