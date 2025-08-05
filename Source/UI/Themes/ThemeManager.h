#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "ComponentState.h"
#include "INIConfig.h"
#include "ColorScheme.h"

/**
 * @class ThemeManager
 * @brief Advanced theme management system for OTTO Phase 9D
 * 
 * Provides comprehensive theme management including:
 * - Dynamic theme switching without restart
 * - Custom theme creation and editing
 * - Theme import/export functionality
 * - Accessibility and seasonal themes
 * - Real-time theme preview
 * 
 * Integrates with existing ColorScheme system while extending capabilities
 * for modern UI/UX requirements.
 */
class ThemeManager : public juce::ChangeBroadcaster {
public:
    /**
     * @enum ThemeType
     * @brief Categories of available themes
     */
    enum class ThemeType {
        System,        // System-provided themes (Dark, Light, Classic)
        User,          // User-created custom themes
        Accessibility, // High contrast, colorblind-friendly themes
        Seasonal,      // Time-based automatic themes
        Community      // Downloaded/shared themes
    };

    /**
     * @enum AccessibilityMode
     * @brief Accessibility enhancement modes
     */
    enum class AccessibilityMode {
        None,
        HighContrast,
        Protanopia,    // Red-blind
        Deuteranopia,  // Green-blind
        Tritanopia,    // Blue-blind
        Monochrome     // Complete colorblind support
    };

    /**
     * @struct ThemeMetadata
     * @brief Complete theme information
     */
    struct ThemeMetadata {
        juce::String name;
        juce::String author;
        juce::String description;
        juce::String version;
        ThemeType type;
        AccessibilityMode accessibilityMode = AccessibilityMode::None;
        juce::Time createdDate;
        juce::Time modifiedDate;
        juce::StringArray tags;
        bool isValid = true;
        juce::String filePath;
        
        ThemeMetadata() : createdDate(juce::Time::getCurrentTime()), modifiedDate(juce::Time::getCurrentTime()) {}
        ThemeMetadata(const juce::String& themeName, ThemeType themeType = ThemeType::User)
            : name(themeName), type(themeType), accessibilityMode(AccessibilityMode::None),
              createdDate(juce::Time::getCurrentTime()), modifiedDate(juce::Time::getCurrentTime()) {}
    };

    /**
     * @struct ThemeColors
     * @brief Extended color palette for advanced theming
     */
    struct ThemeColors {
        // Base colors
        juce::Colour primary;
        juce::Colour secondary;
        juce::Colour accent;
        juce::Colour background;
        juce::Colour surface;
        juce::Colour error;
        juce::Colour warning;
        juce::Colour success;
        
        // Text colors
        juce::Colour onPrimary;
        juce::Colour onSecondary;
        juce::Colour onBackground;
        juce::Colour onSurface;
        juce::Colour onError;
        
        // State colors
        juce::Colour hover;
        juce::Colour pressed;
        juce::Colour disabled;
        juce::Colour selected;
        
        // Audio-specific colors
        juce::Colour meterLow;
        juce::Colour meterMid;
        juce::Colour meterHigh;
        juce::Colour meterPeak;
        juce::Colour waveform;
        juce::Colour spectrum;
        
        // Gradient support
        juce::ColourGradient backgroundGradient;
        juce::ColourGradient accentGradient;
        bool useGradients = false;
        
        ThemeColors() { setDefaults(); }
        void setDefaults();
        void applyAccessibilityMode(AccessibilityMode mode);
    };

    /**
     * @struct SeasonalSettings
     * @brief Automatic seasonal theme switching
     */
    struct SeasonalSettings {
        bool enabled = false;
        juce::String springTheme = "Light";
        juce::String summerTheme = "Light";
        juce::String autumnTheme = "Classic";
        juce::String winterTheme = "Dark";
        bool useLocationBasedSeasons = false;
        float latitude = 0.0f;
        float longitude = 0.0f;
    };

    ThemeManager(ColorScheme& colorScheme);
    ~ThemeManager() override;

    // Theme Management
    bool createTheme(const juce::String& name, const ThemeColors& colors, const ThemeMetadata& metadata = ThemeMetadata());
    bool deleteTheme(const juce::String& name);
    bool duplicateTheme(const juce::String& sourceName, const juce::String& newName);
    bool renameTheme(const juce::String& oldName, const juce::String& newName);
    
    // Theme Application
    void setCurrentTheme(const juce::String& name);
    juce::String getCurrentThemeName() const { return currentThemeName; }
    ThemeMetadata getCurrentThemeMetadata() const;
    ThemeColors getCurrentThemeColors() const;
    
    // Theme Discovery
    juce::StringArray getAvailableThemes() const;
    juce::StringArray getThemesByType(ThemeType type) const;
    juce::Array<ThemeMetadata> getAllThemeMetadata() const;
    ThemeMetadata getThemeMetadata(const juce::String& name) const;
    
    // Theme Import/Export
    bool exportTheme(const juce::String& name, const juce::File& destination);
    bool importTheme(const juce::File& themeFile);
    bool importThemeFromString(const juce::String& themeData, const juce::String& name);
    juce::String exportThemeToString(const juce::String& name);
    
    // Real-time Preview
    void startPreview(const juce::String& themeName);
    void stopPreview();
    bool isPreviewActive() const { return previewActive; }
    juce::String getPreviewThemeName() const { return previewThemeName; }
    
    // Accessibility
    void setAccessibilityMode(AccessibilityMode mode);
    AccessibilityMode getAccessibilityMode() const { return currentAccessibilityMode; }
    juce::StringArray getAccessibilityModeNames() const;
    
    // Seasonal Themes
    void setSeasonalSettings(const SeasonalSettings& settings);
    SeasonalSettings getSeasonalSettings() const { return seasonalSettings; }
    void updateSeasonalTheme();
    juce::String getCurrentSeasonalTheme() const;
    
    // Theme Validation
    bool validateTheme(const juce::String& name) const;
    bool validateThemeColors(const ThemeColors& colors) const;
    juce::StringArray getThemeValidationErrors(const juce::String& name) const;
    
    // State Management
    void saveState(ComponentState& state) const;
    void loadState(const ComponentState& state);
    
    // Integration with existing ColorScheme
    void syncWithColorScheme();
    void updateColorScheme(const ThemeColors& colors);
    
    // Theme Editor Support
    void beginThemeEdit(const juce::String& name);
    void endThemeEdit(bool saveChanges = true);
    bool isEditingTheme() const { return editingTheme; }
    juce::String getEditingThemeName() const { return editingThemeName; }
    void setEditingThemeColors(const ThemeColors& colors);
    ThemeColors getEditingThemeColors() const;
    
    // Performance
    void preloadThemes();
    void clearThemeCache();
    int getCachedThemeCount() const;
    
    // Listener Interface
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void themeChanged(const juce::String& newThemeName) {}
        virtual void themePreviewStarted(const juce::String& previewThemeName) {}
        virtual void themePreviewStopped() {}
        virtual void accessibilityModeChanged(AccessibilityMode newMode) {}
        virtual void seasonalThemeUpdated(const juce::String& seasonalTheme) {}
        virtual void themeValidationFailed(const juce::String& themeName, const juce::StringArray& errors) {}
    };
    
    void addListener(Listener* listener);
    void removeListener(Listener* listener);

private:
    ColorScheme& colorScheme;
    
    // Current state
    juce::String currentThemeName;
    ThemeColors currentThemeColors;
    AccessibilityMode currentAccessibilityMode = AccessibilityMode::None;
    SeasonalSettings seasonalSettings;
    
    // Preview state
    bool previewActive = false;
    juce::String previewThemeName;
    ThemeColors originalThemeColors;
    
    // Editing state
    bool editingTheme = false;
    juce::String editingThemeName;
    ThemeColors editingThemeColors;
    ThemeColors originalEditingColors;
    
    // Theme storage
    std::unordered_map<juce::String, ThemeMetadata> themeMetadata;
    std::unordered_map<juce::String, ThemeColors> themeColors;
    std::unordered_map<juce::String, juce::Time> themeLoadTimes;
    
    // Listeners
    juce::ListenerList<Listener> listeners;
    
    // File management
    juce::File getThemesDirectory() const;
    juce::File getThemeFile(const juce::String& name) const;
    bool saveThemeToFile(const juce::String& name, const ThemeColors& colors, const ThemeMetadata& metadata);
    bool loadThemeFromFile(const juce::String& name);
    
    // Built-in themes
    void initializeBuiltInThemes();
    ThemeColors createDarkThemeColors();
    ThemeColors createLightThemeColors();
    ThemeColors createClassicThemeColors();
    ThemeColors createHighContrastThemeColors();
    
    // Accessibility helpers
    juce::Colour adjustColorForAccessibility(const juce::Colour& color, AccessibilityMode mode);
    float calculateContrastRatio(const juce::Colour& foreground, const juce::Colour& background);
    bool meetsAccessibilityStandards(const juce::Colour& foreground, const juce::Colour& background);
    
    // Seasonal helpers
    int getCurrentSeason() const;
    juce::String getSeasonName(int season) const;
    bool shouldUpdateSeasonalTheme() const;
    
    // Validation helpers
    bool isValidThemeName(const juce::String& name) const;
    bool isValidColor(const juce::Colour& color) const;
    juce::StringArray validateThemeColorsInternal(const ThemeColors& colors) const;
    
    // Notification helpers
    void notifyThemeChanged();
    void notifyPreviewStarted();
    void notifyPreviewStopped();
    void notifyAccessibilityModeChanged();
    void notifySeasonalThemeUpdated();
    void notifyValidationFailed(const juce::String& themeName, const juce::StringArray& errors);
    
    // Timer for seasonal updates
    std::unique_ptr<juce::Timer> seasonalTimer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThemeManager)
};