#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "../../ColorScheme.h"
#include "../../INIConfig.h"

/**
 * @file ThemePresets.h
 * @brief Built-in theme presets for OTTO JUCE 8 application
 * 
 * This component provides a comprehensive collection of professionally designed
 * theme presets that users can apply instantly. Includes studio-focused themes,
 * accessibility themes, and seasonal variations.
 * 
 * FEATURES:
 * - Professional studio themes (Dark Studio, Light Studio, Midnight)
 * - Accessibility themes (High Contrast, Colorblind-friendly)
 * - Seasonal themes (Spring, Summer, Autumn, Winter)
 * - Genre-specific themes (Electronic, Hip-Hop, Jazz, Rock)
 * - Custom theme validation and migration
 * 
 * INTEGRATION:
 * - Works with ThemeManager for seamless theme switching
 * - Uses ColorScheme system for consistent color management
 * - INI-driven configuration for all theme parameters
 * 
 * @author OTTO Development Team
 * @version 1.0
 * @date 2025
 */

namespace OTTO {
namespace UI {
namespace Themes {

/**
 * @class ThemePresets
 * @brief Manages built-in theme presets and theme validation
 * 
 * This class provides a comprehensive collection of professionally designed
 * theme presets that can be applied instantly to the OTTO interface.
 * It also handles theme validation, migration, and accessibility compliance.
 */
class ThemePresets
{
public:
    /**
     * @enum PresetCategory
     * @brief Categories for organizing theme presets
     */
    enum class PresetCategory {
        Studio,         ///< Professional studio themes
        Accessibility,  ///< High contrast and colorblind-friendly themes
        Seasonal,       ///< Time-based seasonal themes
        Genre,          ///< Music genre-specific themes
        Custom          ///< User-created themes
    };

    /**
     * @enum StudioTheme
     * @brief Built-in studio theme presets
     */
    enum class StudioTheme {
        DarkStudio,     ///< Professional dark theme for low-light environments
        LightStudio,    ///< Clean light theme for bright environments
        Midnight,       ///< Ultra-dark theme for late-night sessions
        Charcoal,       ///< Medium-dark theme with warm accents
        Arctic         ///< Cool-toned light theme
    };

    /**
     * @enum AccessibilityTheme
     * @brief Accessibility-focused theme presets
     */
    enum class AccessibilityTheme {
        HighContrast,   ///< Maximum contrast for visual impairments
        Deuteranopia,   ///< Red-green colorblind friendly
        Protanopia,     ///< Red-green colorblind friendly (variant)
        Tritanopia,     ///< Blue-yellow colorblind friendly
        LowVision       ///< Large text and high contrast
    };

    /**
     * @enum SeasonalTheme
     * @brief Seasonal theme presets
     */
    enum class SeasonalTheme {
        Spring,         ///< Fresh greens and light colors
        Summer,         ///< Warm and vibrant colors
        Autumn,         ///< Rich oranges and browns
        Winter          ///< Cool blues and whites
    };

    /**
     * @enum GenreTheme
     * @brief Music genre-specific theme presets
     */
    enum class GenreTheme {
        Electronic,     ///< Neon and cyber-inspired colors
        HipHop,         ///< Urban and street-inspired colors
        Jazz,           ///< Warm and sophisticated colors
        Rock,           ///< Bold and energetic colors
        Ambient         ///< Soft and atmospheric colors
    };

    /**
     * @struct ThemePreset
     * @brief Complete theme preset definition
     */
    struct ThemePreset {
        juce::String name;                    ///< Display name
        juce::String description;             ///< Theme description
        PresetCategory category;              ///< Theme category
        juce::String author;                  ///< Theme creator
        juce::String version;                 ///< Theme version
        bool isAccessible;                    ///< Accessibility compliance
        float contrastRatio;                  ///< Text contrast ratio
        juce::StringPairArray colorValues;    ///< Color definitions
        juce::StringPairArray gradientValues; ///< Gradient definitions
        juce::StringPairArray fontSettings;   ///< Font configurations
        juce::Time createdTime;               ///< Creation timestamp
        juce::Time modifiedTime;              ///< Last modification
    };

    // Constructor and Destructor
    ThemePresets();
    ~ThemePresets();

    // Preset Management
    /**
     * @brief Get all available theme presets
     * @return Array of all theme presets
     */
    juce::Array<ThemePreset> getAllPresets() const;

    /**
     * @brief Get presets by category
     * @param category The category to filter by
     * @return Array of presets in the specified category
     */
    juce::Array<ThemePreset> getPresetsByCategory(PresetCategory category) const;

    /**
     * @brief Get a specific preset by name
     * @param presetName The name of the preset
     * @return The theme preset, or empty preset if not found
     */
    ThemePreset getPreset(const juce::String& presetName) const;

    /**
     * @brief Check if a preset exists
     * @param presetName The name of the preset
     * @return True if preset exists
     */
    bool hasPreset(const juce::String& presetName) const;

    // Studio Themes
    /**
     * @brief Apply a studio theme preset
     * @param theme The studio theme to apply
     * @param colorScheme The color scheme to update
     * @return True if theme was applied successfully
     */
    bool applyStudioTheme(StudioTheme theme, ColorScheme& colorScheme);

    /**
     * @brief Get studio theme preset
     * @param theme The studio theme
     * @return The theme preset
     */
    ThemePreset getStudioTheme(StudioTheme theme) const;

    // Accessibility Themes
    /**
     * @brief Apply an accessibility theme preset
     * @param theme The accessibility theme to apply
     * @param colorScheme The color scheme to update
     * @return True if theme was applied successfully
     */
    bool applyAccessibilityTheme(AccessibilityTheme theme, ColorScheme& colorScheme);

    /**
     * @brief Get accessibility theme preset
     * @param theme The accessibility theme
     * @return The theme preset
     */
    ThemePreset getAccessibilityTheme(AccessibilityTheme theme) const;

    /**
     * @brief Validate theme for accessibility compliance
     * @param preset The theme preset to validate
     * @return True if theme meets accessibility standards
     */
    bool validateAccessibility(const ThemePreset& preset) const;

    // Seasonal Themes
    /**
     * @brief Apply a seasonal theme preset
     * @param theme The seasonal theme to apply
     * @param colorScheme The color scheme to update
     * @return True if theme was applied successfully
     */
    bool applySeasonalTheme(SeasonalTheme theme, ColorScheme& colorScheme);

    /**
     * @brief Get seasonal theme preset
     * @param theme The seasonal theme
     * @return The theme preset
     */
    ThemePreset getSeasonalTheme(SeasonalTheme theme) const;

    /**
     * @brief Get current seasonal theme based on date
     * @return The appropriate seasonal theme for current time
     */
    SeasonalTheme getCurrentSeasonalTheme() const;

    // Genre Themes
    /**
     * @brief Apply a genre theme preset
     * @param theme The genre theme to apply
     * @param colorScheme The color scheme to update
     * @return True if theme was applied successfully
     */
    bool applyGenreTheme(GenreTheme theme, ColorScheme& colorScheme);

    /**
     * @brief Get genre theme preset
     * @param theme The genre theme
     * @return The theme preset
     */
    ThemePreset getGenreTheme(GenreTheme theme) const;

    // Custom Theme Management
    /**
     * @brief Add a custom theme preset
     * @param preset The custom theme preset
     * @return True if theme was added successfully
     */
    bool addCustomPreset(const ThemePreset& preset);

    /**
     * @brief Remove a custom theme preset
     * @param presetName The name of the preset to remove
     * @return True if theme was removed successfully
     */
    bool removeCustomPreset(const juce::String& presetName);

    /**
     * @brief Update a custom theme preset
     * @param preset The updated theme preset
     * @return True if theme was updated successfully
     */
    bool updateCustomPreset(const ThemePreset& preset);

    // Theme Validation and Migration
    /**
     * @brief Validate a theme preset
     * @param preset The theme preset to validate
     * @return True if theme is valid
     */
    bool validateTheme(const ThemePreset& preset) const;

    /**
     * @brief Migrate theme from older version
     * @param oldPreset The old theme preset
     * @param targetVersion The target version to migrate to
     * @return Migrated theme preset
     */
    ThemePreset migrateTheme(const ThemePreset& oldPreset, const juce::String& targetVersion) const;

    /**
     * @brief Check if theme needs migration
     * @param preset The theme preset to check
     * @return True if migration is needed
     */
    bool needsMigration(const ThemePreset& preset) const;

    // Import/Export
    /**
     * @brief Export theme preset to file
     * @param preset The theme preset to export
     * @param file The file to export to
     * @return True if export was successful
     */
    bool exportTheme(const ThemePreset& preset, const juce::File& file) const;

    /**
     * @brief Import theme preset from file
     * @param file The file to import from
     * @return The imported theme preset, or empty preset if failed
     */
    ThemePreset importTheme(const juce::File& file) const;

    /**
     * @brief Export all presets to directory
     * @param directory The directory to export to
     * @return Number of themes exported successfully
     */
    int exportAllThemes(const juce::File& directory) const;

    /**
     * @brief Import all themes from directory
     * @param directory The directory to import from
     * @return Number of themes imported successfully
     */
    int importAllThemes(const juce::File& directory);

    // Utility Functions
    /**
     * @brief Get preset names by category
     * @param category The category to get names for
     * @return Array of preset names
     */
    juce::StringArray getPresetNames(PresetCategory category) const;

    /**
     * @brief Get all preset names
     * @return Array of all preset names
     */
    juce::StringArray getAllPresetNames() const;

    /**
     * @brief Get category name as string
     * @param category The category
     * @return Category name
     */
    static juce::String getCategoryName(PresetCategory category);

    /**
     * @brief Parse category from string
     * @param categoryName The category name
     * @return The category enum value
     */
    static PresetCategory parseCategoryName(const juce::String& categoryName);

    // State Management
    /**
     * @brief Save presets to INI file
     * @return True if save was successful
     */
    bool saveToINI() const;

    /**
     * @brief Load presets from INI file
     * @return True if load was successful
     */
    bool loadFromINI();

    /**
     * @brief Reset to factory presets
     */
    void resetToFactoryPresets();

    // Listeners
    /**
     * @class Listener
     * @brief Interface for theme preset change notifications
     */
    class Listener {
    public:
        virtual ~Listener() = default;
        
        /**
         * @brief Called when a preset is added
         * @param preset The added preset
         */
        virtual void presetAdded(const ThemePreset& preset) {}
        
        /**
         * @brief Called when a preset is removed
         * @param presetName The name of the removed preset
         */
        virtual void presetRemoved(const juce::String& presetName) {}
        
        /**
         * @brief Called when a preset is updated
         * @param preset The updated preset
         */
        virtual void presetUpdated(const ThemePreset& preset) {}
        
        /**
         * @brief Called when presets are reset to factory defaults
         */
        virtual void presetsReset() {}
    };

    /**
     * @brief Add a listener for preset changes
     * @param listener The listener to add
     */
    void addListener(Listener* listener);

    /**
     * @brief Remove a listener
     * @param listener The listener to remove
     */
    void removeListener(Listener* listener);

private:
    // Internal Methods
    void initializeFactoryPresets();
    void createStudioPresets();
    void createAccessibilityPresets();
    void createSeasonalPresets();
    void createGenrePresets();
    
    ThemePreset createPreset(const juce::String& name, 
                           const juce::String& description,
                           PresetCategory category,
                           const juce::StringPairArray& colors,
                           const juce::StringPairArray& gradients = {},
                           const juce::StringPairArray& fonts = {}) const;
    
    void applyPresetToColorScheme(const ThemePreset& preset, ColorScheme& colorScheme) const;
    float calculateContrastRatio(const juce::Colour& foreground, const juce::Colour& background) const;
    bool meetsAccessibilityStandards(const ThemePreset& preset) const;
    
    void notifyListeners(std::function<void(Listener*)> notification);

    // Member Variables
    juce::Array<ThemePreset> factoryPresets;
    juce::Array<ThemePreset> customPresets;
    juce::ListenerList<Listener> listeners;
    
    // Constants
    static constexpr float MIN_CONTRAST_RATIO = 4.5f;  ///< WCAG AA standard
    static constexpr float MIN_LARGE_TEXT_CONTRAST = 3.0f;  ///< WCAG AA for large text
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThemePresets)
};

} // namespace Themes
} // namespace UI
} // namespace OTTO