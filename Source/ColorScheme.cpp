/**
 * @file ColorScheme.cpp
 * @brief Implementation of OTTO's comprehensive color theme management system
 * 
 * This file implements the ColorScheme class which manages all visual theming
 * for the OTTO application. It provides centralized color management with
 * support for multiple themes, real-time theme switching, and INI-based
 * persistence through the INIConfig system.
 * 
 * DESIGN ARCHITECTURE:
 * ====================
 * - Theme Management: Loads/saves themes via INIConfig::THEMES_FILE
 * - Color Consistency: Ensures all UI elements use consistent color schemes
 * - Real-time Updates: Notifies components when themes change for immediate refresh
 * - Default Fallbacks: Provides robust fallback to default theme if corruption occurs
 * 
 * INTEGRATION POINTS:
 * ===================
 * - INIConfig.h: Uses default theme constants and file paths
 * - ComponentState.h: Integrates with state management system
 * - CustomLookAndFeel.cpp: Provides colors for JUCE component rendering
 * - All UI components: Reference this class for consistent color application
 * 
 * THEME SYSTEM FLOW:
 * ==================
 * 1. Constructor initializes with default "Dark" theme
 * 2. loadStates() restores user's saved theme preference
 * 3. Components query colors via getter methods
 * 4. setTheme() allows runtime theme switching with immediate updates
 * 5. saveStates() persists theme choice via ComponentState system
 * 
 * @author OTTO Development Team
 * @version 2.0
 * @date 2024
 */

#include "ColorScheme.h"
#include "INIConfig.h"

/**
 * @brief ColorScheme constructor - initializes default "Dark" theme
 * 
 * Sets up the default dark theme configuration with carefully chosen colors
 * that provide good contrast, readability, and visual hierarchy. All color
 * values are stored as hex strings for easy editing and theme customization.
 * 
 * DEFAULT THEME COLORS:
 * - Background: Dark gray (#2a2a2a) for reduced eye strain
 * - Foreground: Medium gray (#3a3a3a) for component backgrounds  
 * - Buttons: Progressive gray scale for hover/active states
 * - Text: Light gray (#d0d0d0) for high contrast readability
 * - Accents: Neutral gray (#808080) for secondary elements
 * - Meters: Traffic light colors (green/yellow/red) for audio levels
 * 
 * Called by: Application startup, theme reset operations
 * References: INIConfig::Defaults::DEFAULT_THEME_ID for theme selection
 */
ColorScheme::ColorScheme() : currentThemeName("Dark")
{
    // Initialize theme settings with default values
    // Referenced by: CustomLookAndFeel.cpp for component styling
    currentThemeSettings.setDefaults();
    
    // BACKGROUND COLORS: Progressive darkness for visual hierarchy
    currentThemeSettings.backgroundColor = "#2a2a2a";     // Main window background - darkest for minimal eye strain
    currentThemeSettings.foregroundColor = "#3a3a3a";    // Component panel backgrounds - slightly lighter for definition
    
    // INTERACTIVE ELEMENT COLORS: Button state progression for clear user feedback
    currentThemeSettings.buttonColor = "#5a5a5a";        // Default button state - mid-gray for neutral appearance
    currentThemeSettings.buttonHoverColor = "#6a6a6a";   // Hover state - lighter for feedback (used by CustomLookAndFeel.cpp)
    currentThemeSettings.buttonActiveColor = "#7a7a7a";  // Active/pressed state - lightest for strong feedback
    
    // TEXT AND ACCENT COLORS: High contrast for accessibility compliance
    currentThemeSettings.textColor = "#d0d0d0";          // Primary text - light gray for readability on dark backgrounds
    currentThemeSettings.accentColor = "#808080";        // Secondary elements, icons, borders
    
    // SLIDER AND CONTROL COLORS: Consistent with button color scheme
    currentThemeSettings.sliderTrackColor = "#4a4a4a";   // Slider background tracks - darker than buttons
    currentThemeSettings.sliderThumbColor = "#808080";   // Slider handles - accent color for visibility
    
    // STRUCTURAL COLORS: Borders and separation elements
    currentThemeSettings.borderColor = "#1a1a1a";        // Component borders - very dark for subtle separation
    
    // AUDIO METER COLORS: Traffic light system for intuitive level monitoring
    currentThemeSettings.meterColorLow = "#44ff44";      // Green for safe levels (used by Mixer.cpp, meter components)
    currentThemeSettings.meterColorMid = "#ffaa44";      // Orange for moderate levels - warning zone
    currentThemeSettings.meterColorHigh = "#ff4444";     // Red for high levels - danger zone
    
    // PATTERN GRID COLORS: Visual distinction for pattern matrix (used by MainContentComponent.cpp)
    currentThemeSettings.gridLineColor = "#3a3a3a";         // Grid line separation - matches foreground
    currentThemeSettings.patternActiveColor = "#5a5a5a";    // Active pattern cells - button color consistency
    currentThemeSettings.patternInactiveColor = "#3a3a3a";  // Inactive pattern cells - foreground color consistency
    
    // Validate all color assignments and ensure theme integrity
    ensureValidTheme();
}
/**
 * @brief ColorScheme destructor - cleanup resources
 * 
 * Currently no special cleanup required as all resources are managed
 * by JUCE's automatic memory management and ComponentState system.
 */
ColorScheme::~ColorScheme()
{
}

/**
 * @brief Save current theme settings to application state
 * 
 * Persists the current theme configuration and theme name to the ComponentState
 * system, which handles serialization to INI files via INIDataManager.cpp.
 * This ensures user theme preferences survive application restarts.
 * 
 * PERSISTENCE FLOW:
 * 1. Current theme settings copied to ComponentState
 * 2. Theme name stored in global settings
 * 3. ComponentState automatically serializes via INIDataManager.cpp
 * 4. Data written to INIConfig::THEMES_FILE
 * 
 * @param state ComponentState reference to store theme data
 * 
 * Called by: StateManager.cpp during application shutdown or settings changes
 * References: ComponentState.h for data structure, INIDataManager.cpp for persistence
 */
void ColorScheme::saveStates(ComponentState& state) const
{
    // Copy complete theme settings structure for persistence
    state.themeSettings = currentThemeSettings;
    
    // Store theme name in global settings for quick theme identification
    state.globalSettings.currentThemeName = currentThemeName;
}
/**
 * @brief Load theme settings from saved application state
 * 
 * Restores previously saved theme configuration from ComponentState, with robust
 * error handling and fallback mechanisms. This method is called during application
 * startup to restore user's theme preferences.
 * 
 * LOADING STRATEGY:
 * 1. Attempt to load complete theme settings if available
 * 2. Fallback to theme name if settings corrupted
 * 3. Final fallback to default "Dark" theme
 * 4. Always validate theme integrity and notify UI components
 * 
 * ERROR HANDLING:
 * - Invalid theme names revert to "Dark" theme
 * - Corrupted theme settings trigger default reconstruction
 * - All failures are gracefully handled with safe defaults
 * 
 * @param state ComponentState containing saved theme data
 * 
 * Called by: StateManager.cpp during application startup
 * References: INIConfig::isValidThemeName() for theme validation
 */
void ColorScheme::loadStates(const ComponentState& state)
{
    // PRIMARY LOADING PATH: Complete theme settings available
    if (state.themeSettings.isValid())
    {
        // Restore complete theme configuration
        currentThemeSettings = state.themeSettings;
        currentThemeName = state.themeSettings.themeName;
        
        // Validate theme name against known themes (INIConfig.h validation)
        if (!INIConfig::isValidThemeName(currentThemeName))
        {
            // Invalid theme - revert to safe default
            currentThemeName = "Dark";
            currentThemeSettings.setDefaults();
        }
        
        // Ensure all color values are valid and notify UI of changes
        ensureValidTheme();
        notifyThemeChanged();  // Triggers UI refresh in all components
    }
    // FALLBACK PATH: Only theme name available, settings corrupted/missing
    else
    {
        if (state.globalSettings.currentThemeName.isNotEmpty())
        {
            // Attempt to reconstruct theme from name
            setTheme(state.globalSettings.currentThemeName);
        }
        else
        {
            // Final fallback - complete default initialization
            currentThemeSettings.setDefaults();
            currentThemeName = "Dark";
            ensureValidTheme();
        }
    }
}
/**
 * @brief Set active theme by name with immediate UI updates
 * 
 * Changes the current theme to the specified theme name, loading appropriate
 * color configurations and notifying all UI components of the change. This
 * method provides real-time theme switching capability for user preferences.
 * 
 * SUPPORTED THEMES:
 * - "Dark": Default dark theme (constructor defaults)
 * - "Light": Light theme variant for bright environments
 * - "Classic": Retro/vintage inspired color scheme
 * - Custom themes: Must be loaded via ComponentState system
 * 
 * THEME SWITCHING FLOW:
 * 1. Validate theme name against INIConfig::isValidThemeName()
 * 2. Load appropriate theme color configuration
 * 3. Validate all color values for integrity
 * 4. Notify all UI components via notifyThemeChanged()
 * 5. Components automatically refresh with new colors
 * 
 * @param themeName Name of theme to activate ("Dark", "Light", "Classic")
 * 
 * Called by: SettingsPanel.cpp theme selection, loadStates() restoration
 * References: INIConfig::isValidThemeName() for validation
 */
void ColorScheme::setTheme(const juce::String& themeName)
{
    // Validate theme name against known theme list (from INIConfig.h)
    if (!INIConfig::isValidThemeName(themeName))
    {
        setError("Invalid theme name: " + themeName);
        return;
    }
    
    // DARK THEME: Default theme configuration (matches constructor)
    if (themeName == "Dark")
    {
        currentThemeSettings.setDefaults();
        currentThemeName = "Dark";
    }
    // LIGHT THEME: High-contrast light variant for bright environments
    else if (themeName == "Light")
    {
        currentThemeSettings.setLightThemeDefaults();
        currentThemeName = "Light";
    }
    // CLASSIC THEME: Retro/vintage color scheme for nostalgic feel
    else if (themeName == "Classic")
    {
        currentThemeSettings.setClassicThemeDefaults();
        currentThemeName = "Classic";
    }
    // CUSTOM THEMES: Require full theme data via ComponentState loading
    else
    {
        setError("Custom theme loading requires theme to be in ComponentState");
        return;
    }
    
    // Validate theme integrity and trigger UI updates
    ensureValidTheme();
    notifyThemeChanged();  // Causes all UI components to repaint with new colors
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

// ROW-SPECIFIC DEBUG BACKGROUND COLORS
// These methods return the darker background colors defined in INIConfig::Color
// for easy row identification during layout debugging
juce::Colour ColorScheme::getRow1DebugBackgroundColor() const
{
    return juce::Colour(INIConfig::Color::ROW_1_DEBUG_BACKGROUND);
}

juce::Colour ColorScheme::getRow2DebugBackgroundColor() const
{
    return juce::Colour(INIConfig::Color::ROW_2_DEBUG_BACKGROUND);
}

juce::Colour ColorScheme::getRow3DebugBackgroundColor() const
{
    return juce::Colour(INIConfig::Color::ROW_3_DEBUG_BACKGROUND);
}

juce::Colour ColorScheme::getRow4DebugBackgroundColor() const
{
    return juce::Colour(INIConfig::Color::ROW_4_DEBUG_BACKGROUND);
}

juce::Colour ColorScheme::getRow5DebugBackgroundColor() const
{
    return juce::Colour(INIConfig::Color::ROW_5_DEBUG_BACKGROUND);
}

juce::Colour ColorScheme::getRow6DebugBackgroundColor() const
{
    return juce::Colour(INIConfig::Color::ROW_6_DEBUG_BACKGROUND);
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
