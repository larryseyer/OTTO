#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "../../ColorScheme.h"
#include "../../INIConfig.h"
#include "ThemePresets.h"

/**
 * @file ThemeValidator.h
 * @brief Theme validation and migration system for OTTO JUCE 8 application
 * 
 * This component provides comprehensive theme validation, accessibility checking,
 * and automatic migration between theme format versions. Ensures all themes
 * meet OTTO's quality and accessibility standards.
 * 
 * FEATURES:
 * - Comprehensive theme validation (colors, gradients, fonts)
 * - WCAG 2.1 AA accessibility compliance checking
 * - Automatic theme migration between versions
 * - Color harmony and contrast analysis
 * - Performance impact assessment
 * - Theme compatibility verification
 * 
 * INTEGRATION:
 * - Works with ThemePresets for validation before storage
 * - Integrates with ThemeManager for runtime validation
 * - Uses ColorScheme system for color analysis
 * - INI-driven configuration for validation rules
 * 
 * @author OTTO Development Team
 * @version 1.0
 * @date 2025
 */

namespace OTTO {
namespace UI {
namespace Themes {

/**
 * @class ThemeValidator
 * @brief Validates themes for quality, accessibility, and compatibility
 * 
 * This class provides comprehensive validation of theme presets to ensure
 * they meet OTTO's quality standards, accessibility requirements, and
 * performance criteria. It also handles migration between theme versions.
 */
class ThemeValidator
{
public:
    /**
     * @enum ValidationResult
     * @brief Results of theme validation
     */
    enum class ValidationResult {
        Valid,              ///< Theme passes all validation checks
        Warning,            ///< Theme has minor issues but is usable
        Error,              ///< Theme has critical issues and should not be used
        AccessibilityFail,  ///< Theme fails accessibility requirements
        PerformanceFail,    ///< Theme may cause performance issues
        CompatibilityFail   ///< Theme is incompatible with current version
    };

    /**
     * @enum ValidationCategory
     * @brief Categories of validation checks
     */
    enum class ValidationCategory {
        Colors,         ///< Color value validation
        Gradients,      ///< Gradient definition validation
        Fonts,          ///< Font configuration validation
        Accessibility,  ///< WCAG compliance validation
        Performance,    ///< Performance impact validation
        Compatibility   ///< Version compatibility validation
    };

    /**
     * @struct ValidationIssue
     * @brief Represents a validation issue found in a theme
     */
    struct ValidationIssue {
        ValidationCategory category;    ///< Issue category
        ValidationResult severity;      ///< Issue severity
        juce::String property;          ///< Property that failed validation
        juce::String message;           ///< Human-readable issue description
        juce::String suggestion;        ///< Suggested fix for the issue
        bool canAutoFix;               ///< Whether issue can be automatically fixed
    };

    /**
     * @struct ValidationReport
     * @brief Complete validation report for a theme
     */
    struct ValidationReport {
        ValidationResult overallResult;         ///< Overall validation result
        juce::Array<ValidationIssue> issues;   ///< List of validation issues
        float accessibilityScore;               ///< Accessibility score (0-100)
        float performanceScore;                 ///< Performance score (0-100)
        float qualityScore;                     ///< Overall quality score (0-100)
        juce::Time validationTime;              ///< When validation was performed
        juce::String validatorVersion;          ///< Version of validator used
        
        /**
         * @brief Check if theme passed validation
         * @return True if theme is valid
         */
        bool isValid() const { 
            return overallResult == ValidationResult::Valid || 
                   overallResult == ValidationResult::Warning; 
        }
        
        /**
         * @brief Get issues by category
         * @param category The category to filter by
         * @return Array of issues in the specified category
         */
        juce::Array<ValidationIssue> getIssuesByCategory(ValidationCategory category) const;
        
        /**
         * @brief Get issues by severity
         * @param severity The severity to filter by
         * @return Array of issues with the specified severity
         */
        juce::Array<ValidationIssue> getIssuesBySeverity(ValidationResult severity) const;
    };

    /**
     * @struct MigrationRule
     * @brief Rule for migrating themes between versions
     */
    struct MigrationRule {
        juce::String fromVersion;       ///< Source version
        juce::String toVersion;         ///< Target version
        juce::String property;          ///< Property to migrate
        juce::String oldName;           ///< Old property name
        juce::String newName;           ///< New property name
        juce::String defaultValue;      ///< Default value if missing
        bool required;                  ///< Whether property is required
    };

    // Constructor and Destructor
    ThemeValidator();
    ~ThemeValidator();

    // Theme Validation
    /**
     * @brief Validate a complete theme preset
     * @param preset The theme preset to validate
     * @return Complete validation report
     */
    ValidationReport validateTheme(const ThemePresets::ThemePreset& preset) const;

    /**
     * @brief Quick validation check
     * @param preset The theme preset to validate
     * @return True if theme passes basic validation
     */
    bool isThemeValid(const ThemePresets::ThemePreset& preset) const;

    /**
     * @brief Validate specific category
     * @param preset The theme preset to validate
     * @param category The category to validate
     * @return Array of issues found in the category
     */
    juce::Array<ValidationIssue> validateCategory(const ThemePresets::ThemePreset& preset, 
                                                 ValidationCategory category) const;

    // Color Validation
    /**
     * @brief Validate color definitions
     * @param colorValues The color values to validate
     * @return Array of color validation issues
     */
    juce::Array<ValidationIssue> validateColors(const juce::StringPairArray& colorValues) const;

    /**
     * @brief Validate a single color value
     * @param colorName The name of the color
     * @param colorValue The color value string
     * @return Validation issue if invalid, empty if valid
     */
    ValidationIssue validateColor(const juce::String& colorName, const juce::String& colorValue) const;

    /**
     * @brief Check color contrast ratio
     * @param foreground The foreground color
     * @param background The background color
     * @return Contrast ratio (1.0 to 21.0)
     */
    float calculateContrastRatio(const juce::Colour& foreground, const juce::Colour& background) const;

    /**
     * @brief Validate color harmony
     * @param colors Array of colors to check for harmony
     * @return True if colors work well together
     */
    bool validateColorHarmony(const juce::Array<juce::Colour>& colors) const;

    // Accessibility Validation
    /**
     * @brief Validate WCAG 2.1 AA compliance
     * @param preset The theme preset to validate
     * @return Accessibility validation report
     */
    ValidationReport validateAccessibility(const ThemePresets::ThemePreset& preset) const;

    /**
     * @brief Check minimum contrast requirements
     * @param preset The theme preset to check
     * @return Array of contrast issues
     */
    juce::Array<ValidationIssue> checkContrastRequirements(const ThemePresets::ThemePreset& preset) const;

    /**
     * @brief Validate colorblind accessibility
     * @param preset The theme preset to validate
     * @return Array of colorblind accessibility issues
     */
    juce::Array<ValidationIssue> validateColorblindAccessibility(const ThemePresets::ThemePreset& preset) const;

    /**
     * @brief Check touch target accessibility
     * @param preset The theme preset to check
     * @return Array of touch target issues
     */
    juce::Array<ValidationIssue> checkTouchTargetAccessibility(const ThemePresets::ThemePreset& preset) const;

    // Performance Validation
    /**
     * @brief Validate theme performance impact
     * @param preset The theme preset to validate
     * @return Performance validation report
     */
    ValidationReport validatePerformance(const ThemePresets::ThemePreset& preset) const;

    /**
     * @brief Check gradient complexity
     * @param gradientValues The gradient definitions to check
     * @return Array of performance issues
     */
    juce::Array<ValidationIssue> checkGradientComplexity(const juce::StringPairArray& gradientValues) const;

    /**
     * @brief Validate font performance
     * @param fontSettings The font settings to validate
     * @return Array of font performance issues
     */
    juce::Array<ValidationIssue> validateFontPerformance(const juce::StringPairArray& fontSettings) const;

    // Theme Migration
    /**
     * @brief Migrate theme to current version
     * @param oldPreset The theme preset to migrate
     * @return Migrated theme preset
     */
    ThemePresets::ThemePreset migrateTheme(const ThemePresets::ThemePreset& oldPreset) const;

    /**
     * @brief Migrate theme to specific version
     * @param oldPreset The theme preset to migrate
     * @param targetVersion The target version
     * @return Migrated theme preset
     */
    ThemePresets::ThemePreset migrateTheme(const ThemePresets::ThemePreset& oldPreset, 
                                          const juce::String& targetVersion) const;

    /**
     * @brief Check if theme needs migration
     * @param preset The theme preset to check
     * @return True if migration is needed
     */
    bool needsMigration(const ThemePresets::ThemePreset& preset) const;

    /**
     * @brief Get available migration path
     * @param fromVersion Source version
     * @param toVersion Target version
     * @return Array of migration rules
     */
    juce::Array<MigrationRule> getMigrationPath(const juce::String& fromVersion, 
                                               const juce::String& toVersion) const;

    // Auto-Fix Capabilities
    /**
     * @brief Automatically fix theme issues
     * @param preset The theme preset to fix
     * @param report The validation report with issues
     * @return Fixed theme preset
     */
    ThemePresets::ThemePreset autoFixTheme(const ThemePresets::ThemePreset& preset, 
                                          const ValidationReport& report) const;

    /**
     * @brief Fix specific validation issue
     * @param preset The theme preset to fix
     * @param issue The issue to fix
     * @return Fixed theme preset
     */
    ThemePresets::ThemePreset fixIssue(const ThemePresets::ThemePreset& preset, 
                                      const ValidationIssue& issue) const;

    /**
     * @brief Check if issue can be automatically fixed
     * @param issue The validation issue
     * @return True if issue can be auto-fixed
     */
    bool canAutoFix(const ValidationIssue& issue) const;

    // Validation Rules Management
    /**
     * @brief Add custom validation rule
     * @param category The validation category
     * @param ruleName The name of the rule
     * @param ruleFunction The validation function
     */
    void addValidationRule(ValidationCategory category, 
                          const juce::String& ruleName,
                          std::function<ValidationIssue(const ThemePresets::ThemePreset&)> ruleFunction);

    /**
     * @brief Remove validation rule
     * @param category The validation category
     * @param ruleName The name of the rule to remove
     */
    void removeValidationRule(ValidationCategory category, const juce::String& ruleName);

    /**
     * @brief Enable/disable validation category
     * @param category The category to enable/disable
     * @param enabled Whether to enable the category
     */
    void setCategoryEnabled(ValidationCategory category, bool enabled);

    /**
     * @brief Check if validation category is enabled
     * @param category The category to check
     * @return True if category is enabled
     */
    bool isCategoryEnabled(ValidationCategory category) const;

    // Utility Functions
    /**
     * @brief Get validation result name
     * @param result The validation result
     * @return Human-readable result name
     */
    static juce::String getValidationResultName(ValidationResult result);

    /**
     * @brief Get validation category name
     * @param category The validation category
     * @return Human-readable category name
     */
    static juce::String getValidationCategoryName(ValidationCategory category);

    /**
     * @brief Parse validation result from string
     * @param resultName The result name
     * @return The validation result enum
     */
    static ValidationResult parseValidationResult(const juce::String& resultName);

    /**
     * @brief Parse validation category from string
     * @param categoryName The category name
     * @return The validation category enum
     */
    static ValidationCategory parseValidationCategory(const juce::String& categoryName);

    // Configuration
    /**
     * @brief Set minimum contrast ratio requirement
     * @param ratio The minimum contrast ratio (1.0 to 21.0)
     */
    void setMinimumContrastRatio(float ratio);

    /**
     * @brief Get minimum contrast ratio requirement
     * @return The minimum contrast ratio
     */
    float getMinimumContrastRatio() const;

    /**
     * @brief Set accessibility validation enabled
     * @param enabled Whether to enable accessibility validation
     */
    void setAccessibilityValidationEnabled(bool enabled);

    /**
     * @brief Check if accessibility validation is enabled
     * @return True if accessibility validation is enabled
     */
    bool isAccessibilityValidationEnabled() const;

    /**
     * @brief Set performance validation enabled
     * @param enabled Whether to enable performance validation
     */
    void setPerformanceValidationEnabled(bool enabled);

    /**
     * @brief Check if performance validation is enabled
     * @return True if performance validation is enabled
     */
    bool isPerformanceValidationEnabled() const;

    // State Management
    /**
     * @brief Save validator settings to INI
     * @return True if save was successful
     */
    bool saveToINI() const;

    /**
     * @brief Load validator settings from INI
     * @return True if load was successful
     */
    bool loadFromINI();

    /**
     * @brief Reset to default validation settings
     */
    void resetToDefaults();

private:
    // Internal Validation Methods
    ValidationIssue createIssue(ValidationCategory category, 
                               ValidationResult severity,
                               const juce::String& property,
                               const juce::String& message,
                               const juce::String& suggestion = {},
                               bool canAutoFix = false) const;

    juce::Array<ValidationIssue> validateRequiredColors(const juce::StringPairArray& colorValues) const;
    juce::Array<ValidationIssue> validateGradients(const juce::StringPairArray& gradientValues) const;
    juce::Array<ValidationIssue> validateFonts(const juce::StringPairArray& fontSettings) const;
    
    float calculateAccessibilityScore(const ThemePresets::ThemePreset& preset) const;
    float calculatePerformanceScore(const ThemePresets::ThemePreset& preset) const;
    float calculateQualityScore(const ValidationReport& report) const;
    
    bool isValidColorValue(const juce::String& colorValue) const;
    bool isValidGradientValue(const juce::String& gradientValue) const;
    bool isValidFontValue(const juce::String& fontValue) const;
    
    juce::Colour parseColor(const juce::String& colorValue) const;
    juce::Array<juce::Colour> parseGradient(const juce::String& gradientValue) const;
    
    void initializeMigrationRules();
    void initializeValidationRules();
    
    ThemePresets::ThemePreset applyMigrationRules(const ThemePresets::ThemePreset& preset,
                                                 const juce::Array<MigrationRule>& rules) const;

    // Member Variables
    float minimumContrastRatio;
    bool accessibilityValidationEnabled;
    bool performanceValidationEnabled;
    
    // Category enabled states - using Array instead of HashMap for enum compatibility
    struct CategoryEnabledState {
        ValidationCategory category;
        bool enabled;
    };
    juce::Array<CategoryEnabledState> categoryEnabled;
    juce::Array<MigrationRule> migrationRules;
    
    // Custom validation rules
    struct CustomValidationRule {
        juce::String name;
        std::function<ValidationIssue(const ThemePresets::ThemePreset&)> function;
    };
    
    struct CategoryCustomRules {
        ValidationCategory category;
        juce::Array<CustomValidationRule> rules;
    };
    juce::Array<CategoryCustomRules> customRules;
    
    // Required color properties
    juce::StringArray requiredColors;
    
    // Constants
    static constexpr float DEFAULT_MIN_CONTRAST_RATIO = 4.5f;  ///< WCAG AA standard
    static constexpr float LARGE_TEXT_MIN_CONTRAST_RATIO = 3.0f;  ///< WCAG AA for large text
    static constexpr int MAX_GRADIENT_STOPS = 10;  ///< Maximum gradient stops for performance
    static constexpr int MAX_FONT_VARIANTS = 20;  ///< Maximum font variants for performance
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThemeValidator)
};

} // namespace Themes
} // namespace UI
} // namespace OTTO