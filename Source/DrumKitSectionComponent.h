#pragma once

#include <JuceHeader.h>
#include "ColorScheme.h"
#include "ComponentState.h"
#include "SFZEngine.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "INIDataManager.h"
#include "PopupWindows.h"
#include "PopupWindowsMixer.h"
#include "DrumKitEditorContent.h"

class PresetManager;

class DrumKitSectionComponent : public juce::Component,
                               public juce::ComboBox::Listener,
                               public ColorScheme::Listener
{
public:
    DrumKitSectionComponent(PresetManager& presetManager,
                           SFZEngine& sfzEngine,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme,
                           INIDataManager& dataManager,
                           Mixer* mixer = nullptr);
    ~DrumKitSectionComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    void themeChanged(const juce::String& newThemeName) override;
    void colorChanged(const juce::String& component, const juce::String& property, const juce::Colour& newColor) override;

    void lookAndFeelChanged() override;

    std::function<void()> onEditRequested;
    std::function<void()> onMixerRequested;
    std::function<void(bool)> onPowerStateChanged;
    std::function<void(int)> onKitChanged;

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    void setCurrentPlayerIndex(int index);
    int getCurrentPlayerIndex() const { return currentPlayerIndex; }

private:
    PresetManager& presetManager;
    SFZEngine& sfzEngine;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;
    INIDataManager& dataManager;
    Mixer* mixer;

    std::unique_ptr<juce::ComboBox> drumkitComboBox;
    std::unique_ptr<juce::ComboBox> sfzFileComboBox;
    std::unique_ptr<juce::Label> drumkitLabel;
    std::unique_ptr<juce::Label> sfzFileLabel;
    std::unique_ptr<juce::TextButton> refreshButton;
    std::unique_ptr<juce::TextButton> loadButton;
    std::unique_ptr<juce::TextButton> editButton;
    std::unique_ptr<juce::TextButton> mixerButton;
    std::unique_ptr<juce::ToggleButton> powerButton;
    std::unique_ptr<DrumKitEditorWindow> drumKitEditorWindow;
    std::unique_ptr<DrumKitMixerWindow> drumKitMixerWindow;

    int currentPlayerIndex = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;

    void updateDrumkitList();
    void updateSFZFileList();
    void loadSelectedDrumkit();
    void refreshDrumkits();
    void applyColorScheme();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumKitSectionComponent)
};
