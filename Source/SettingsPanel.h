#pragma once
#include <JuceHeader.h>
#include "FontManager.h"
#include "ColorScheme.h"
#include "ResponsiveLayoutManager.h"
#include "INIDataManager.h"
#include "ComponentState.h"
#include "UtilityComponents.h"

class SFZEngine;

class SettingsPanelWindow : public juce::Component {
public:
    SettingsPanelWindow(FontManager& fontManager,
                       ColorScheme& colorScheme,
                       ResponsiveLayoutManager& layoutManager,
                       INIDataManager& INIManager);
    ~SettingsPanelWindow() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    std::function<void(FontManager::PhosphorWeight)> onPhosphorWeightChanged;
    std::function<void()> onColorSchemeChanged;
    std::function<void()> onClose;

private:
    FontManager& fontManager;
    ColorScheme& colorScheme;
    ResponsiveLayoutManager& layoutManager;
    INIDataManager& INIManager;

    juce::Label titleLabel;
    juce::Label phosphorLabel;
    juce::ComboBox phosphorWeightCombo;
    juce::TextButton darkThemeButton;
    juce::TextButton lightThemeButton;
    PhosphorIconButton closeButton;

    void setupComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPanelWindow)
};
