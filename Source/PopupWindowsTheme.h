#pragma once
#include <JuceHeader.h>
#include "ColorScheme.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "INIDataManager.h"
#include "ComponentState.h"

class ThemeEditorTab : public juce::Component,
                      public juce::ComboBox::Listener,
                      public juce::Slider::Listener,
                      public juce::Button::Listener,
                      public juce::ChangeListener {
public:
    ThemeEditorTab(ColorScheme& colorScheme,
                  ResponsiveLayoutManager& layoutManager,
                  FontManager& fontManager,
                  INIDataManager& INIManager);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void saveThemeSettings(ThemeSettings& settings);
    void loadThemeSettings(const ThemeSettings& settings);

    std::function<void()> onThemeChanged;

private:
    ColorScheme& colorScheme;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    INIDataManager& INIManager;

    juce::ComboBox themePresetCombo;
    juce::TextButton saveThemeButton, newThemeButton, deleteThemeButton;

    struct ColorEditor {
        juce::Label label;
        juce::TextButton colorButton;
        juce::String propertyName;
        juce::Colour currentColor;
    };

    juce::OwnedArray<ColorEditor> colorEditors;
    std::unique_ptr<juce::ColourSelector> colorSelector;
    ColorEditor* currentColorEditor = nullptr;

    juce::Viewport scrollViewport;
    juce::Component colorEditorsContainer;

    void setupComponents();
    void createColorEditors();
    void showColorPicker(ColorEditor* editor);
    void updatePreview();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThemeEditorTab)
};
