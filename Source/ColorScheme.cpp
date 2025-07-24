#include "ColorScheme.h"
#include "INIConfig.h"
ColorScheme::ColorScheme() : currentThemeName("Dark")
{
    currentThemeSettings.setDefaults();
    currentThemeSettings.backgroundColor = "#2a2a2a";  
    currentThemeSettings.foregroundColor = "#3a3a3a";  
    currentThemeSettings.buttonColor = "#5a5a5a";      
    currentThemeSettings.buttonHoverColor = "#6a6a6a"; 
    currentThemeSettings.buttonActiveColor = "#7a7a7a"; 
    currentThemeSettings.textColor = "#d0d0d0";        
    currentThemeSettings.accentColor = "#808080";      
    currentThemeSettings.sliderTrackColor = "#4a4a4a"; 
    currentThemeSettings.sliderThumbColor = "#808080"; 
    currentThemeSettings.borderColor = "#1a1a1a";      
    currentThemeSettings.meterColorLow = "#44ff44";    
    currentThemeSettings.meterColorMid = "#ffaa44";    
    currentThemeSettings.meterColorHigh = "#ff4444";   
    currentThemeSettings.gridLineColor = "#3a3a3a";
    currentThemeSettings.patternActiveColor = "#5a5a5a";
    currentThemeSettings.patternInactiveColor = "#3a3a3a";
    ensureValidTheme();
}
ColorScheme::~ColorScheme()
{
}
void ColorScheme::saveStates(ComponentState& state) const
{
    state.themeSettings = currentThemeSettings;
    state.globalSettings.currentThemeName = currentThemeName;
}
void ColorScheme::loadStates(const ComponentState& state)
{
    if (state.themeSettings.isValid())
    {
        currentThemeSettings = state.themeSettings;
        currentThemeName = state.themeSettings.themeName;
        if (!INIConfig::isValidThemeName(currentThemeName))
        {
            currentThemeName = "Dark";
            currentThemeSettings.setDefaults();
        }
        ensureValidTheme();
        notifyThemeChanged();
    }
    else
    {
        if (state.globalSettings.currentThemeName.isNotEmpty())
        {
            setTheme(state.globalSettings.currentThemeName);
        }
        else
        {
            currentThemeSettings.setDefaults();
            currentThemeName = "Dark";
            ensureValidTheme();
        }
    }
}
void ColorScheme::setTheme(const juce::String& themeName)
{
    if (!INIConfig::isValidThemeName(themeName))
    {
        setError("Invalid theme name: " + themeName);
        return;
    }
    if (themeName == "Dark")
    {
        currentThemeSettings.setDefaults();
        currentThemeName = "Dark";
    }
    else if (themeName == "Light")
    {
        currentThemeSettings.setLightThemeDefaults();
        currentThemeName = "Light";
    }
    else if (themeName == "Classic")
    {
        currentThemeSettings.setClassicThemeDefaults();
        currentThemeName = "Classic";
    }
    else
    {
        setError("Custom theme loading requires theme to be in ComponentState");
        return;
    }
    ensureValidTheme();
    notifyThemeChanged();
}
void ColorScheme::ensureValidTheme()
{
    if (!currentThemeSettings.isValid())
    {
        currentThemeSettings.setDefaults();
        currentThemeName = "Dark";
    }
}
juce::StringArray ColorScheme::getAvailableThemes() const
{
    juce::StringArray themes;
    themes.add("Dark");
    themes.add("Light");
    themes.add("Classic");
    return themes;
}
juce::Colour ColorScheme::getColor(ColorRole role) const
{
    const_cast<ColorScheme*>(this)->ensureValidTheme();
    juce::Colour backgroundColor = stringToColor(currentThemeSettings.backgroundColor);
    juce::Colour buttonColor = stringToColor(currentThemeSettings.buttonColor);
    juce::Colour textColor = stringToColor(currentThemeSettings.textColor);
    juce::Colour buttonHoverColor = stringToColor(currentThemeSettings.buttonHoverColor);
    juce::Colour buttonActiveColor = stringToColor(currentThemeSettings.buttonActiveColor);
    juce::Colour sliderTrackColor = stringToColor(currentThemeSettings.sliderTrackColor);
    juce::Colour sliderThumbColor = stringToColor(currentThemeSettings.sliderThumbColor);
    juce::Colour accentColor = stringToColor(currentThemeSettings.accentColor);
    bool isDark = currentThemeName == "Dark";
    switch (role)
    {
        case ColorRole::WindowBackground:
            return backgroundColor;
        case ColorRole::ComponentBackground:
            return backgroundColor;
        case ColorRole::ControlBackground:
            return isDark ? backgroundColor.darker(0.3f) : backgroundColor.brighter(0.1f);
        case ColorRole::PrimaryText:
            return textColor;
        case ColorRole::SecondaryText:
            return textColor.withAlpha(0.7f);
        case ColorRole::ButtonBackground:
            return buttonColor;
        case ColorRole::ButtonBackgroundHover:
            return buttonHoverColor;
        case ColorRole::ButtonBackgroundPressed:
            return buttonActiveColor;
        case ColorRole::ButtonBackgroundToggled:
            return isDark ? buttonColor.brighter(0.3f) : buttonColor.darker(0.2f);
        case ColorRole::ButtonText:
            return textColor;
        case ColorRole::SliderTrack:
            return sliderTrackColor;
        case ColorRole::SliderThumb:
            return sliderThumbColor;
        case ColorRole::Accent:
            return accentColor;
        case ColorRole::AccentHover:
            return accentColor.brighter(0.2f);
        case ColorRole::Separator:
            return isDark ? juce::Colour(0xff1a1a1a) : juce::Colour(0xffe0e0e0);
        case ColorRole::Error:
            return juce::Colour(DEFAULT_ERROR_COLOR);
        case ColorRole::Warning:
            return juce::Colour(DEFAULT_WARNING_COLOR);
        case ColorRole::Success:
            return juce::Colour(DEFAULT_SUCCESS_COLOR);
        case ColorRole::MeterLow:
            return stringToColor(currentThemeSettings.meterColorLow);
        case ColorRole::MeterMid:
            return stringToColor(currentThemeSettings.meterColorMid);
        case ColorRole::MeterHigh:
            return stringToColor(currentThemeSettings.meterColorHigh);
        case ColorRole::GridLine:
            return isDark ? backgroundColor.darker(0.3f) : backgroundColor.brighter(0.2f);
        case ColorRole::PatternActive:
            return isDark ? backgroundColor.brighter(0.3f) : backgroundColor.darker(0.2f);
        case ColorRole::PatternInactive:
            return backgroundColor;
        default:
            return textColor;
    }
}
juce::Colour ColorScheme::stringToColor(const juce::String& colorString) const
{
    if (colorString.isEmpty())
        return juce::Colour(0xff2a2a2a); 
    juce::Colour color;
    if (colorString.startsWithChar('#'))
    {
        color = juce::Colour::fromString(colorString);
    }
    else if (colorString.startsWith("0x") || colorString.startsWith("0X"))
    {
        color = juce::Colour(colorString.getHexValue32());
    }
    else
    {
        color = juce::Colour(colorString.getHexValue32());
    }
    if (color.getAlpha() < 255) {
        color = color.withAlpha(1.0f);  
    }
    return color;
}
juce::String ColorScheme::colorToString(const juce::Colour& color) const
{
    return color.toString();
}
void ColorScheme::applyToLookAndFeel(juce::LookAndFeel& lookAndFeel) const
{
    forceUpdateLookAndFeel(lookAndFeel);
}
void ColorScheme::forceUpdateLookAndFeel(juce::LookAndFeel& lookAndFeel) const
{
    const_cast<ColorScheme*>(this)->ensureValidTheme();
    lookAndFeel.setColour(juce::ResizableWindow::backgroundColourId, getColor(ColorRole::WindowBackground));
    lookAndFeel.setColour(juce::DocumentWindow::backgroundColourId, getColor(ColorRole::WindowBackground));
    lookAndFeel.setColour(juce::TextButton::buttonColourId, getColor(ColorRole::ButtonBackground));
    lookAndFeel.setColour(juce::TextButton::buttonOnColourId, getColor(ColorRole::ButtonBackgroundToggled));
    lookAndFeel.setColour(juce::TextButton::textColourOnId, getColor(ColorRole::ButtonText));
    lookAndFeel.setColour(juce::TextButton::textColourOffId, getColor(ColorRole::ButtonText));
    lookAndFeel.setColour(juce::Label::textColourId, getColor(ColorRole::PrimaryText));
    lookAndFeel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    lookAndFeel.setColour(juce::Slider::backgroundColourId, getColor(ColorRole::SliderTrack));
    lookAndFeel.setColour(juce::Slider::thumbColourId, getColor(ColorRole::SliderThumb));
    lookAndFeel.setColour(juce::Slider::trackColourId, getColor(ColorRole::SliderTrack));
    lookAndFeel.setColour(juce::Slider::textBoxTextColourId, getColor(ColorRole::PrimaryText));
    lookAndFeel.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    lookAndFeel.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    lookAndFeel.setColour(juce::ComboBox::backgroundColourId, getColor(ColorRole::ButtonBackground));
    lookAndFeel.setColour(juce::ComboBox::textColourId, getColor(ColorRole::ButtonText));
    lookAndFeel.setColour(juce::ComboBox::arrowColourId, getColor(ColorRole::ButtonText));
    lookAndFeel.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    lookAndFeel.setColour(juce::ComboBox::buttonColourId, getColor(ColorRole::ButtonBackground));
    lookAndFeel.setColour(juce::TextEditor::backgroundColourId, getColor(ColorRole::ComponentBackground));
    lookAndFeel.setColour(juce::TextEditor::textColourId, getColor(ColorRole::PrimaryText));
    lookAndFeel.setColour(juce::TextEditor::outlineColourId, getColor(ColorRole::Separator));
    lookAndFeel.setColour(juce::TextEditor::focusedOutlineColourId, getColor(ColorRole::Accent));
    lookAndFeel.setColour(juce::TextEditor::highlightColourId, getColor(ColorRole::Accent).withAlpha(0.4f));
    lookAndFeel.setColour(juce::TextEditor::highlightedTextColourId, getColor(ColorRole::PrimaryText));
    lookAndFeel.setColour(juce::PopupMenu::backgroundColourId, getColor(ColorRole::WindowBackground));
    lookAndFeel.setColour(juce::PopupMenu::textColourId, getColor(ColorRole::SecondaryText));
    lookAndFeel.setColour(juce::PopupMenu::highlightedBackgroundColourId, getColor(ColorRole::ButtonBackgroundHover));
    lookAndFeel.setColour(juce::PopupMenu::highlightedTextColourId, getColor(ColorRole::PrimaryText));
    lookAndFeel.setColour(juce::PopupMenu::headerTextColourId, getColor(ColorRole::PrimaryText));
    lookAndFeel.setColour(juce::ScrollBar::backgroundColourId, getColor(ColorRole::ComponentBackground));
    lookAndFeel.setColour(juce::ScrollBar::thumbColourId, getColor(ColorRole::SliderThumb));
    lookAndFeel.setColour(juce::ScrollBar::trackColourId, getColor(ColorRole::SliderTrack));
    lookAndFeel.setColour(juce::TreeView::backgroundColourId, getColor(ColorRole::ComponentBackground));
    lookAndFeel.setColour(juce::TreeView::linesColourId, getColor(ColorRole::Separator));
    lookAndFeel.setColour(juce::TreeView::dragAndDropIndicatorColourId, getColor(ColorRole::Accent));
    lookAndFeel.setColour(juce::TreeView::selectedItemBackgroundColourId, getColor(ColorRole::ButtonBackgroundHover));
    lookAndFeel.setColour(juce::PropertyComponent::backgroundColourId, getColor(ColorRole::ComponentBackground));
    lookAndFeel.setColour(juce::PropertyComponent::labelTextColourId, getColor(ColorRole::PrimaryText));
    lookAndFeel.setColour(juce::ToggleButton::textColourId, getColor(ColorRole::ButtonText));
    lookAndFeel.setColour(juce::ToggleButton::tickColourId, getColor(ColorRole::Accent));
    lookAndFeel.setColour(juce::ToggleButton::tickDisabledColourId, getColor(ColorRole::SecondaryText));
    lookAndFeel.setColour(juce::AlertWindow::backgroundColourId, getColor(ColorRole::WindowBackground));
    lookAndFeel.setColour(juce::AlertWindow::textColourId, getColor(ColorRole::PrimaryText));
    lookAndFeel.setColour(juce::AlertWindow::outlineColourId, getColor(ColorRole::Separator));
    lookAndFeel.setColour(juce::TableHeaderComponent::textColourId, getColor(ColorRole::PrimaryText));
    lookAndFeel.setColour(juce::TableHeaderComponent::backgroundColourId, getColor(ColorRole::ComponentBackground));
    lookAndFeel.setColour(juce::TableHeaderComponent::outlineColourId, getColor(ColorRole::Separator));
    lookAndFeel.setColour(juce::TableHeaderComponent::highlightColourId, getColor(ColorRole::ButtonBackgroundHover));
}
juce::Colour ColorScheme::getBackgroundColor() const
{
    juce::Colour bgColor = getColor(ColorRole::WindowBackground);
    if (bgColor.getAlpha() < 1.0f) {
        bgColor = bgColor.withAlpha(1.0f);
    }
    if (bgColor == juce::Colours::transparentBlack) {
        bgColor = juce::Colour(0xff2a2a2a); 
    }
    return bgColor;
}
juce::Colour ColorScheme::getTopBarBackgroundColor() const
{
    return getBackgroundColor();
}
juce::Colour ColorScheme::getPlayerStripBackgroundColor() const
{
    return getBackgroundColor();
}
juce::Colour ColorScheme::getTransportBackgroundColor() const
{
    return getBackgroundColor();
}
juce::Colour ColorScheme::getPatternGridBackgroundColor() const
{
    return getBackgroundColor();
}
juce::Colour ColorScheme::getButtonBackgroundColor() const
{
    return getColor(ColorRole::ButtonBackground);
}
juce::Colour ColorScheme::getButtonTextColor() const
{
    return getColor(ColorRole::ButtonText);
}
juce::Colour ColorScheme::getButtonHoverColor() const
{
    return getColor(ColorRole::ButtonBackgroundHover);
}
juce::Colour ColorScheme::getButtonPressedColor() const
{
    return getColor(ColorRole::ButtonBackgroundPressed);
}
juce::Colour ColorScheme::getSliderBackgroundColor() const
{
    return getColor(ColorRole::SliderTrack);
}
juce::Colour ColorScheme::getSliderThumbColor() const
{
    return getColor(ColorRole::SliderThumb);
}
juce::Colour ColorScheme::getSliderTrackColor() const
{
    return getColor(ColorRole::SliderTrack);
}
juce::Colour ColorScheme::getLabelTextColor() const
{
    return getColor(ColorRole::PrimaryText);
}
juce::Colour ColorScheme::getTextEditorBackgroundColor() const
{
    return getColor(ColorRole::ComponentBackground);
}
juce::Colour ColorScheme::getTextEditorTextColor() const
{
    return getColor(ColorRole::PrimaryText);
}
juce::Colour ColorScheme::getTextEditorBorderColor() const
{
    return getColor(ColorRole::Separator);
}
juce::Colour ColorScheme::getComboBoxBackgroundColor() const
{
    return getColor(ColorRole::ButtonBackground);
}
juce::Colour ColorScheme::getComboBoxTextColor() const
{
    return getColor(ColorRole::ButtonText);
}
juce::Colour ColorScheme::getComboBoxArrowColor() const
{
    return getColor(ColorRole::ButtonText);
}
juce::Colour ColorScheme::getToggleButtonBackgroundColor() const
{
    return getButtonBackgroundColor();
}
juce::Colour ColorScheme::getToggleButtonOnColor() const
{
    return getColor(ColorRole::ButtonBackgroundToggled);
}
juce::Colour ColorScheme::getToggleButtonOffColor() const
{
    return getColor(ColorRole::ButtonBackground);
}
juce::Colour ColorScheme::getProgressBarBackgroundColor() const
{
    return getColor(ColorRole::Separator);
}
juce::Colour ColorScheme::getProgressBarForegroundColor() const
{
    return getColor(ColorRole::Accent);
}
bool ColorScheme::saveCurrentThemeSettings()
{
    if (currentThemeName.isEmpty())
    {
        setError("No current theme to save");
        return false;
    }
    currentThemeSettings.modifiedDate = juce::Time::getCurrentTime().toString(true, true);
    return true;
}
void ColorScheme::setColor(const juce::String& component, const juce::String& property, const juce::Colour& color, float alpha)
{
    juce::String colorString = colorToString(color.withAlpha(alpha));
    if (component == "Background" && property == "BackgroundColor")
        currentThemeSettings.backgroundColor = colorString;
    else if (component == "Button" && property == "BackgroundColor")
        currentThemeSettings.buttonColor = colorString;
    else if (component == "Button" && property == "TextColor")
        currentThemeSettings.textColor = colorString;
    else if (component == "Button" && property == "HoverColor")
        currentThemeSettings.buttonHoverColor = colorString;
    else if (component == "Button" && property == "ActiveColor")
        currentThemeSettings.buttonActiveColor = colorString;
    else if (component == "Slider" && property == "TrackColor")
        currentThemeSettings.sliderTrackColor = colorString;
    else if (component == "Slider" && property == "ThumbColor")
        currentThemeSettings.sliderThumbColor = colorString;
    notifyColorChanged(component, property, color.withAlpha(alpha));
}
void ColorScheme::resetToDefaults()
{
    currentThemeSettings.setDefaults();
    currentThemeName = "Dark";
    notifyThemeChanged();
}
bool ColorScheme::createNewTheme(const juce::String& themeName, const juce::String& baseTheme)
{
    if (!INIConfig::isValidThemeName(themeName))
    {
        setError("Invalid theme name");
        return false;
    }
    ThemeSettings newTheme;
    if (baseTheme == "Light")
    {
        newTheme.setLightThemeDefaults();
    }
    else if (baseTheme == "Classic")
    {
        newTheme.setClassicThemeDefaults();
    }
    else
    {
        newTheme.setDefaults();
    }
    newTheme.themeName = themeName;
    auto now = juce::Time::getCurrentTime().toString(true, true);
    newTheme.createdDate = now;
    newTheme.modifiedDate = now;
    currentThemeSettings = newTheme;
    currentThemeName = themeName;
    notifyThemeChanged();
    return true;
}
bool ColorScheme::deleteTheme(const juce::String& themeName)
{
    if (themeName == "Dark" || themeName == "Light" || themeName == "Classic")
    {
        setError("Cannot delete built-in theme");
        return false;
    }
    if (currentThemeName == themeName)
    {
        setTheme("Dark");
    }
    return true;
}
bool ColorScheme::duplicateTheme(const juce::String& sourceName, const juce::String& newName)
{
    if (!INIConfig::isValidThemeName(newName))
    {
        setError("Invalid new theme name");
        return false;
    }
    ThemeSettings sourceTheme;
    if (sourceName == currentThemeName)
    {
        sourceTheme = currentThemeSettings;
    }
    else if (sourceName == "Dark")
    {
        sourceTheme.setDefaults();
    }
    else if (sourceName == "Light")
    {
        sourceTheme.setLightThemeDefaults();
    }
    else if (sourceName == "Classic")
    {
        sourceTheme.setClassicThemeDefaults();
    }
    else
    {
        setError("Source theme not found: " + sourceName);
        return false;
    }
    ThemeSettings newTheme = sourceTheme;
    newTheme.themeName = newName;
    auto now = juce::Time::getCurrentTime().toString(true, true);
    newTheme.createdDate = now;
    newTheme.modifiedDate = now;
    currentThemeSettings = newTheme;
    currentThemeName = newName;
    notifyThemeChanged();
    return true;
}
void ColorScheme::addListener(Listener* listener)
{
    listeners.add(listener);
}
void ColorScheme::removeListener(Listener* listener)
{
    listeners.remove(listener);
}
bool ColorScheme::isValidTheme(const juce::String& themeName) const
{
    return themeName == "Dark" || themeName == "Light" || themeName == "Classic" ||
           INIConfig::isValidThemeName(themeName);
}
int ColorScheme::getCurrentTheme() const
{
    if (currentThemeName == "Dark") return 0;
    if (currentThemeName == "Light") return 1;
    if (currentThemeName == "Classic") return 2;
    return 0;
}
void ColorScheme::notifyThemeChanged()
{
    listeners.call([this](Listener& l) { l.themeChanged(currentThemeName); });
}
void ColorScheme::notifyColorChanged(const juce::String& component, const juce::String& property, const juce::Colour& color)
{
    listeners.call([&](Listener& l) { l.colorChanged(component, property, color); });
}
void ColorScheme::setError(const juce::String& error)
{
    lastError = error;
}
