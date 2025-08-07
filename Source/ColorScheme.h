#pragma once
#include <JuceHeader.h>
#include "ComponentState.h"
#include "INIConfig.h"

class ColorScheme
{
public:
    enum class ColorRole {
        WindowBackground,
        ComponentBackground,
        ControlBackground,
        PrimaryText,
        SecondaryText,
        ButtonBackground,
        ButtonBackgroundHover,
        ButtonBackgroundPressed,
        ButtonBackgroundToggled,
        ButtonText,
        IconButtonText,
        SliderTrack,
        SliderThumb,
        Accent,
        AccentHover,
        Separator,
        Error,
        Warning,
        Success,
        MeterLow,
        MeterMid,
        MeterHigh,
        GridLine,
        PatternActive,
        PatternInactive
    };

    static constexpr uint32_t DEFAULT_ERROR_COLOR = INIConfig::Color::DEFAULT_ERROR_COLOR;
    static constexpr uint32_t DEFAULT_WARNING_COLOR = INIConfig::Color::DEFAULT_WARNING_COLOR;
    static constexpr uint32_t DEFAULT_SUCCESS_COLOR = INIConfig::Color::DEFAULT_SUCCESS_COLOR;
    static constexpr uint32_t DEFAULT_METER_LOW_COLOR = INIConfig::Color::DEFAULT_METER_LOW_COLOR;
    static constexpr uint32_t DEFAULT_METER_MID_COLOR = INIConfig::Color::DEFAULT_METER_MID_COLOR;
    static constexpr uint32_t DEFAULT_METER_HIGH_COLOR = INIConfig::Color::DEFAULT_METER_HIGH_COLOR;

    ColorScheme();
    ~ColorScheme();

    void saveStates(ComponentState& state) const;
    void loadStates(const ComponentState& state);

    void setTheme(const juce::String& themeName);
    juce::String getCurrentThemeName() const { return currentThemeName; }
    juce::StringArray getAvailableThemes() const;

    juce::Colour getColor(ColorRole role) const;

    juce::Colour getBackgroundColor() const;
    juce::Colour getTopBarBackgroundColor() const;
    juce::Colour getPlayerStripBackgroundColor() const;
    juce::Colour getTransportBackgroundColor() const;
    juce::Colour getPatternGridBackgroundColor() const;
    juce::Colour getButtonBackgroundColor() const;
    juce::Colour getButtonTextColor() const;
    juce::Colour getButtonHoverColor() const;
    juce::Colour getButtonPressedColor() const;
    juce::Colour getSliderBackgroundColor() const;
    juce::Colour getSliderThumbColor() const;
    juce::Colour getSliderTrackColor() const;
    juce::Colour getLabelTextColor() const;
    juce::Colour getTextEditorBackgroundColor() const;
    juce::Colour getTextEditorTextColor() const;
    juce::Colour getTextEditorBorderColor() const;
    juce::Colour getComboBoxBackgroundColor() const;
    juce::Colour getComboBoxTextColor() const;
    juce::Colour getComboBoxArrowColor() const;
    juce::Colour getToggleButtonBackgroundColor() const;
    juce::Colour getToggleButtonOnColor() const;
    juce::Colour getToggleButtonOffColor() const;
    juce::Colour getProgressBarBackgroundColor() const;
    juce::Colour getProgressBarForegroundColor() const;

    // ROW-SPECIFIC DEBUG BACKGROUND COLORS
    // These methods return darker background colors for easy row identification during debugging
    juce::Colour getRow1DebugBackgroundColor() const;
    juce::Colour getRow2DebugBackgroundColor() const;
    juce::Colour getRow3DebugBackgroundColor() const;
    juce::Colour getRow4DebugBackgroundColor() const;
    juce::Colour getRow5DebugBackgroundColor() const;
    juce::Colour getRow6DebugBackgroundColor() const;

    void setColor(const juce::String& component, const juce::String& property, const juce::Colour& color, float alpha = INIConfig::Defaults::DEFAULT_ALPHA_FULL);
    void resetToDefaults();

    bool createNewTheme(const juce::String& themeName, const juce::String& baseTheme = "Dark");
    bool deleteTheme(const juce::String& themeName);
    bool duplicateTheme(const juce::String& sourceName, const juce::String& newName);
    bool saveCurrentThemeSettings();

    void applyToLookAndFeel(juce::LookAndFeel& lookAndFeel) const;
    void forceUpdateLookAndFeel(juce::LookAndFeel& lookAndFeel) const;

    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void themeChanged(const juce::String& newThemeName) {}
        virtual void colorChanged(const juce::String& component, const juce::String& property, const juce::Colour& newColor) {}
    };

    void addListener(Listener* listener);
    void removeListener(Listener* listener);

    bool isValidTheme(const juce::String& themeName) const;
    juce::String getLastError() const { return lastError; }

    void setDarkTheme() { setTheme("Dark"); }
    void setLightTheme() { setTheme("Light"); }
    void setClassicTheme() { setTheme("Classic"); }

    int getCurrentTheme() const;

    static ColorScheme& getInstance() {
        static ColorScheme instance;
        return instance;
    }

private:
    ThemeSettings currentThemeSettings;
    juce::String currentThemeName;
    juce::String lastError;
    juce::ListenerList<Listener> listeners;

    void ensureValidTheme();
    juce::Colour stringToColor(const juce::String& colorString) const;
    juce::String colorToString(const juce::Colour& color) const;
    void notifyThemeChanged();
    void notifyColorChanged(const juce::String& component, const juce::String& property, const juce::Colour& color);
    void setError(const juce::String& error);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorScheme)
};
