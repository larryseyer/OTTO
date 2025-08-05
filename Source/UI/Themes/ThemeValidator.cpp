#include "ThemeValidator.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "../../INIDataManager.h"

namespace OTTO {
namespace UI {
namespace Themes {

//==============================================================================
// ValidationReport Methods
//==============================================================================

juce::Array<ThemeValidator::ValidationIssue> 
ThemeValidator::ValidationReport::getIssuesByCategory(ValidationCategory category) const
{
    juce::Array<ValidationIssue> categoryIssues;
    
    for (const auto& issue : issues) {
        if (issue.category == category) {
            categoryIssues.add(issue);
        }
    }
    
    return categoryIssues;
}

juce::Array<ThemeValidator::ValidationIssue> 
ThemeValidator::ValidationReport::getIssuesBySeverity(ValidationResult severity) const
{
    juce::Array<ValidationIssue> severityIssues;
    
    for (const auto& issue : issues) {
        if (issue.severity == severity) {
            severityIssues.add(issue);
        }
    }
    
    return severityIssues;
}

//==============================================================================
// Constructor and Destructor
//==============================================================================

ThemeValidator::ThemeValidator()
    : minimumContrastRatio(DEFAULT_MIN_CONTRAST_RATIO)
    , accessibilityValidationEnabled(true)
    , performanceValidationEnabled(true)
{
    // Initialize category enabled states
    categoryEnabled.set(ValidationCategory::Colors, true);
    categoryEnabled.set(ValidationCategory::Gradients, true);
    categoryEnabled.set(ValidationCategory::Fonts, true);
    categoryEnabled.set(ValidationCategory::Accessibility, true);
    categoryEnabled.set(ValidationCategory::Performance, true);
    categoryEnabled.set(ValidationCategory::Compatibility, true);
    
    // Initialize required colors
    requiredColors.add("WindowBackground");
    requiredColors.add("ComponentBackground");
    requiredColors.add("PrimaryText");
    requiredColors.add("ButtonBackground");
    requiredColors.add("ButtonText");
    requiredColors.add("Accent");
    
    initializeMigrationRules();
    initializeValidationRules();
    loadFromINI();
}

ThemeValidator::~ThemeValidator()
{
    saveToINI();
}

//==============================================================================
// Theme Validation
//==============================================================================

ThemeValidator::ValidationReport ThemeValidator::validateTheme(const ThemePresets::ThemePreset& preset) const
{
    ValidationReport report;
    report.validationTime = juce::Time::getCurrentTime();
    report.validatorVersion = INIConfig::FILE_FORMAT_VERSION;
    
    // Validate each category
    if (isCategoryEnabled(ValidationCategory::Colors)) {
        auto colorIssues = validateCategory(preset, ValidationCategory::Colors);
        report.issues.addArray(colorIssues);
    }
    
    if (isCategoryEnabled(ValidationCategory::Gradients)) {
        auto gradientIssues = validateCategory(preset, ValidationCategory::Gradients);
        report.issues.addArray(gradientIssues);
    }
    
    if (isCategoryEnabled(ValidationCategory::Fonts)) {
        auto fontIssues = validateCategory(preset, ValidationCategory::Fonts);
        report.issues.addArray(fontIssues);
    }
    
    if (isCategoryEnabled(ValidationCategory::Accessibility)) {
        auto accessibilityReport = validateAccessibility(preset);
        report.issues.addArray(accessibilityReport.issues);
        report.accessibilityScore = accessibilityReport.accessibilityScore;
    }
    
    if (isCategoryEnabled(ValidationCategory::Performance)) {
        auto performanceReport = validatePerformance(preset);
        report.issues.addArray(performanceReport.issues);
        report.performanceScore = performanceReport.performanceScore;
    }
    
    if (isCategoryEnabled(ValidationCategory::Compatibility)) {
        auto compatibilityIssues = validateCategory(preset, ValidationCategory::Compatibility);
        report.issues.addArray(compatibilityIssues);
    }
    
    // Determine overall result
    bool hasErrors = false;
    bool hasWarnings = false;
    bool hasAccessibilityFails = false;
    bool hasPerformanceFails = false;
    bool hasCompatibilityFails = false;
    
    for (const auto& issue : report.issues) {
        switch (issue.severity) {
            case ValidationResult::Error:
                hasErrors = true;
                break;
            case ValidationResult::Warning:
                hasWarnings = true;
                break;
            case ValidationResult::AccessibilityFail:
                hasAccessibilityFails = true;
                break;
            case ValidationResult::PerformanceFail:
                hasPerformanceFails = true;
                break;
            case ValidationResult::CompatibilityFail:
                hasCompatibilityFails = true;
                break;
            default:
                break;
        }
    }
    
    // Set overall result based on most severe issue
    if (hasErrors || hasAccessibilityFails || hasPerformanceFails || hasCompatibilityFails) {
        if (hasAccessibilityFails) {
            report.overallResult = ValidationResult::AccessibilityFail;
        } else if (hasPerformanceFails) {
            report.overallResult = ValidationResult::PerformanceFail;
        } else if (hasCompatibilityFails) {
            report.overallResult = ValidationResult::CompatibilityFail;
        } else {
            report.overallResult = ValidationResult::Error;
        }
    } else if (hasWarnings) {
        report.overallResult = ValidationResult::Warning;
    } else {
        report.overallResult = ValidationResult::Valid;
    }
    
    // Calculate quality score
    report.qualityScore = calculateQualityScore(report);
    
    return report;
}

bool ThemeValidator::isThemeValid(const ThemePresets::ThemePreset& preset) const
{
    auto report = validateTheme(preset);
    return report.isValid();
}

juce::Array<ThemeValidator::ValidationIssue> 
ThemeValidator::validateCategory(const ThemePresets::ThemePreset& preset, ValidationCategory category) const
{
    juce::Array<ValidationIssue> issues;
    
    switch (category) {
        case ValidationCategory::Colors:
            issues.addArray(validateColors(preset.colorValues));
            break;
            
        case ValidationCategory::Gradients:
            issues.addArray(validateGradients(preset.gradientValues));
            break;
            
        case ValidationCategory::Fonts:
            issues.addArray(validateFonts(preset.fontSettings));
            break;
            
        case ValidationCategory::Compatibility:
            if (needsMigration(preset)) {
                issues.add(createIssue(ValidationCategory::Compatibility,
                                     ValidationResult::CompatibilityFail,
                                     "version",
                                     "Theme version " + preset.version + " is outdated",
                                     "Migrate theme to version " + INIConfig::FILE_FORMAT_VERSION,
                                     true));
            }
            break;
            
        default:
            break;
    }
    
    // Apply custom validation rules
    if (customRules.contains(category)) {
        for (const auto& rule : customRules[category]) {
            auto issue = rule.function(preset);
            if (!issue.message.isEmpty()) {
                issues.add(issue);
            }
        }
    }
    
    return issues;
}

//==============================================================================
// Color Validation
//==============================================================================

juce::Array<ThemeValidator::ValidationIssue> 
ThemeValidator::validateColors(const juce::StringPairArray& colorValues) const
{
    juce::Array<ValidationIssue> issues;
    
    // Check required colors
    issues.addArray(validateRequiredColors(colorValues));
    
    // Validate each color value
    for (const auto& colorName : colorValues.getAllKeys()) {
        auto colorValue = colorValues[colorName];
        auto issue = validateColor(colorName, colorValue);
        
        if (!issue.message.isEmpty()) {
            issues.add(issue);
        }
    }
    
    // Check color harmony
    juce::Array<juce::Colour> colors;
    for (const auto& colorValue : colorValues.getAllValues()) {
        auto color = parseColor(colorValue);
        if (color != juce::Colour()) {
            colors.add(color);
        }
    }
    
    if (!validateColorHarmony(colors)) {
        issues.add(createIssue(ValidationCategory::Colors,
                             ValidationResult::Warning,
                             "harmony",
                             "Color combination may not be harmonious",
                             "Consider using complementary or analogous colors"));
    }
    
    return issues;
}

ThemeValidator::ValidationIssue 
ThemeValidator::validateColor(const juce::String& colorName, const juce::String& colorValue) const
{
    if (!isValidColorValue(colorValue)) {
        return createIssue(ValidationCategory::Colors,
                         ValidationResult::Error,
                         colorName,
                         "Invalid color value: " + colorValue,
                         "Use format AARRGGBB (e.g., FF0088FF)",
                         true);
    }
    
    auto color = parseColor(colorValue);
    
    // Check for pure black or white (may cause accessibility issues)
    if (color == juce::Colours::black || color == juce::Colours::white) {
        return createIssue(ValidationCategory::Colors,
                         ValidationResult::Warning,
                         colorName,
                         "Pure black/white may cause accessibility issues",
                         "Consider using near-black/near-white colors");
    }
    
    // Check alpha channel
    if (color.getAlpha() < 0.1f && !colorName.contains("Transparent")) {
        return createIssue(ValidationCategory::Colors,
                         ValidationResult::Warning,
                         colorName,
                         "Color is nearly transparent",
                         "Increase alpha value or use explicit transparent naming");
    }
    
    return ValidationIssue{};  // No issues
}

float ThemeValidator::calculateContrastRatio(const juce::Colour& foreground, const juce::Colour& background) const
{
    auto getLuminance = [](const juce::Colour& color) {
        auto r = color.getFloatRed();
        auto g = color.getFloatGreen();
        auto b = color.getFloatBlue();
        
        // Convert to linear RGB
        auto toLinear = [](float c) {
            return c <= 0.03928f ? c / 12.92f : std::pow((c + 0.055f) / 1.055f, 2.4f);
        };
        
        r = toLinear(r);
        g = toLinear(g);
        b = toLinear(b);
        
        // Calculate relative luminance
        return 0.2126f * r + 0.7152f * g + 0.0722f * b;
    };
    
    auto l1 = getLuminance(foreground);
    auto l2 = getLuminance(background);
    
    // Ensure l1 is the lighter color
    if (l1 < l2) {
        std::swap(l1, l2);
    }
    
    return (l1 + 0.05f) / (l2 + 0.05f);
}

bool ThemeValidator::validateColorHarmony(const juce::Array<juce::Colour>& colors) const
{
    if (colors.size() < 2) {
        return true;  // Can't evaluate harmony with less than 2 colors
    }
    
    // Simple harmony check based on hue relationships
    juce::Array<float> hues;
    for (const auto& color : colors) {
        hues.add(color.getHue());
    }
    
    // Check for complementary colors (opposite on color wheel)
    for (int i = 0; i < hues.size(); ++i) {
        for (int j = i + 1; j < hues.size(); ++j) {
            auto hueDiff = std::abs(hues[i] - hues[j]);
            if (hueDiff > 180.0f) {
                hueDiff = 360.0f - hueDiff;
            }
            
            // Complementary colors (150-210 degrees apart)
            if (hueDiff >= 150.0f && hueDiff <= 210.0f) {
                return true;
            }
            
            // Analogous colors (0-60 degrees apart)
            if (hueDiff <= 60.0f) {
                return true;
            }
        }
    }
    
    return false;  // No clear harmony detected
}

//==============================================================================
// Accessibility Validation
//==============================================================================

ThemeValidator::ValidationReport ThemeValidator::validateAccessibility(const ThemePresets::ThemePreset& preset) const
{
    ValidationReport report;
    report.validationTime = juce::Time::getCurrentTime();
    report.validatorVersion = INIConfig::FILE_FORMAT_VERSION;
    
    if (!accessibilityValidationEnabled) {
        report.overallResult = ValidationResult::Valid;
        report.accessibilityScore = 100.0f;
        return report;
    }
    
    // Check contrast requirements
    auto contrastIssues = checkContrastRequirements(preset);
    report.issues.addArray(contrastIssues);
    
    // Check colorblind accessibility
    auto colorblindIssues = validateColorblindAccessibility(preset);
    report.issues.addArray(colorblindIssues);
    
    // Check touch target accessibility
    auto touchTargetIssues = checkTouchTargetAccessibility(preset);
    report.issues.addArray(touchTargetIssues);
    
    // Calculate accessibility score
    report.accessibilityScore = calculateAccessibilityScore(preset);
    
    // Determine overall result
    bool hasAccessibilityFails = false;
    for (const auto& issue : report.issues) {
        if (issue.severity == ValidationResult::AccessibilityFail) {
            hasAccessibilityFails = true;
            break;
        }
    }
    
    report.overallResult = hasAccessibilityFails ? ValidationResult::AccessibilityFail : ValidationResult::Valid;
    
    return report;
}

juce::Array<ThemeValidator::ValidationIssue> 
ThemeValidator::checkContrastRequirements(const ThemePresets::ThemePreset& preset) const
{
    juce::Array<ValidationIssue> issues;
    
    // Check primary text contrast
    auto primaryText = parseColor(preset.colorValues["PrimaryText"]);
    auto background = parseColor(preset.colorValues["WindowBackground"]);
    
    if (primaryText != juce::Colour() && background != juce::Colour()) {
        auto contrastRatio = calculateContrastRatio(primaryText, background);
        
        if (contrastRatio < minimumContrastRatio) {
            issues.add(createIssue(ValidationCategory::Accessibility,
                                 ValidationResult::AccessibilityFail,
                                 "PrimaryText",
                                 "Insufficient contrast ratio: " + juce::String(contrastRatio, 2) + 
                                 " (minimum: " + juce::String(minimumContrastRatio, 1) + ")",
                                 "Increase contrast between text and background colors",
                                 true));
        }
    }
    
    // Check button text contrast
    auto buttonText = parseColor(preset.colorValues["ButtonText"]);
    auto buttonBackground = parseColor(preset.colorValues["ButtonBackground"]);
    
    if (buttonText != juce::Colour() && buttonBackground != juce::Colour()) {
        auto contrastRatio = calculateContrastRatio(buttonText, buttonBackground);
        
        if (contrastRatio < minimumContrastRatio) {
            issues.add(createIssue(ValidationCategory::Accessibility,
                                 ValidationResult::AccessibilityFail,
                                 "ButtonText",
                                 "Insufficient button text contrast ratio: " + juce::String(contrastRatio, 2),
                                 "Increase contrast between button text and background",
                                 true));
        }
    }
    
    return issues;
}

juce::Array<ThemeValidator::ValidationIssue> 
ThemeValidator::validateColorblindAccessibility(const ThemePresets::ThemePreset& preset) const
{
    juce::Array<ValidationIssue> issues;
    
    // Check for red-green color combinations that may be problematic
    auto accent = parseColor(preset.colorValues["Accent"]);
    auto error = parseColor(preset.colorValues["Error"]);
    auto success = parseColor(preset.colorValues["Success"]);
    
    if (accent != juce::Colour() && error != juce::Colour()) {
        // Check if accent and error colors are too similar in hue
        auto accentHue = accent.getHue();
        auto errorHue = error.getHue();
        auto hueDiff = std::abs(accentHue - errorHue);
        
        if (hueDiff < 30.0f || hueDiff > 330.0f) {
            issues.add(createIssue(ValidationCategory::Accessibility,
                                 ValidationResult::Warning,
                                 "Accent/Error",
                                 "Accent and error colors may be indistinguishable to colorblind users",
                                 "Use different hues or add additional visual indicators"));
        }
    }
    
    return issues;
}

juce::Array<ThemeValidator::ValidationIssue> 
ThemeValidator::checkTouchTargetAccessibility(const ThemePresets::ThemePreset& preset) const
{
    juce::Array<ValidationIssue> issues;
    
    // This would typically check font sizes and spacing, but since we don't have
    // layout information in the theme preset, we'll check font settings
    
    for (const auto& fontKey : preset.fontSettings.getAllKeys()) {
        auto fontValue = preset.fontSettings[fontKey];
        
        // Parse font size from font value (simplified)
        auto sizeStart = fontValue.indexOfChar(',');
        if (sizeStart > 0) {
            auto sizeStr = fontValue.substring(sizeStart + 1).trim();
            auto fontSize = sizeStr.getFloatValue();
            
            if (fontSize < 12.0f) {
                issues.add(createIssue(ValidationCategory::Accessibility,
                                     ValidationResult::Warning,
                                     fontKey,
                                     "Font size " + juce::String(fontSize) + " may be too small for accessibility",
                                     "Consider using minimum 12pt font size"));
            }
        }
    }
    
    return issues;
}

//==============================================================================
// Performance Validation
//==============================================================================

ThemeValidator::ValidationReport ThemeValidator::validatePerformance(const ThemePresets::ThemePreset& preset) const
{
    ValidationReport report;
    report.validationTime = juce::Time::getCurrentTime();
    report.validatorVersion = INIConfig::FILE_FORMAT_VERSION;
    
    if (!performanceValidationEnabled) {
        report.overallResult = ValidationResult::Valid;
        report.performanceScore = 100.0f;
        return report;
    }
    
    // Check gradient complexity
    auto gradientIssues = checkGradientComplexity(preset.gradientValues);
    report.issues.addArray(gradientIssues);
    
    // Check font performance
    auto fontIssues = validateFontPerformance(preset.fontSettings);
    report.issues.addArray(fontIssues);
    
    // Calculate performance score
    report.performanceScore = calculatePerformanceScore(preset);
    
    // Determine overall result
    bool hasPerformanceFails = false;
    for (const auto& issue : report.issues) {
        if (issue.severity == ValidationResult::PerformanceFail) {
            hasPerformanceFails = true;
            break;
        }
    }
    
    report.overallResult = hasPerformanceFails ? ValidationResult::PerformanceFail : ValidationResult::Valid;
    
    return report;
}

juce::Array<ThemeValidator::ValidationIssue> 
ThemeValidator::checkGradientComplexity(const juce::StringPairArray& gradientValues) const
{
    juce::Array<ValidationIssue> issues;
    
    for (const auto& gradientName : gradientValues.getAllKeys()) {
        auto gradientValue = gradientValues[gradientName];
        auto colors = parseGradient(gradientValue);
        
        if (colors.size() > MAX_GRADIENT_STOPS) {
            issues.add(createIssue(ValidationCategory::Performance,
                                 ValidationResult::PerformanceFail,
                                 gradientName,
                                 "Gradient has " + juce::String(colors.size()) + " stops (maximum: " + 
                                 juce::String(MAX_GRADIENT_STOPS) + ")",
                                 "Reduce number of gradient stops for better performance",
                                 true));
        }
    }
    
    return issues;
}

juce::Array<ThemeValidator::ValidationIssue> 
ThemeValidator::validateFontPerformance(const juce::StringPairArray& fontSettings) const
{
    juce::Array<ValidationIssue> issues;
    
    if (fontSettings.size() > MAX_FONT_VARIANTS) {
        issues.add(createIssue(ValidationCategory::Performance,
                             ValidationResult::Warning,
                             "fonts",
                             "Theme defines " + juce::String(fontSettings.size()) + " font variants",
                             "Consider reducing font variants for better performance"));
    }
    
    return issues;
}

//==============================================================================
// Theme Migration
//==============================================================================

ThemePresets::ThemePreset ThemeValidator::migrateTheme(const ThemePresets::ThemePreset& oldPreset) const
{
    return migrateTheme(oldPreset, INIConfig::FILE_FORMAT_VERSION);
}

ThemePresets::ThemePreset ThemeValidator::migrateTheme(const ThemePresets::ThemePreset& oldPreset, 
                                                      const juce::String& targetVersion) const
{
    auto migrationPath = getMigrationPath(oldPreset.version, targetVersion);
    return applyMigrationRules(oldPreset, migrationPath);
}

bool ThemeValidator::needsMigration(const ThemePresets::ThemePreset& preset) const
{
    return preset.version != INIConfig::FILE_FORMAT_VERSION;
}

juce::Array<ThemeValidator::MigrationRule> 
ThemeValidator::getMigrationPath(const juce::String& fromVersion, const juce::String& toVersion) const
{
    juce::Array<MigrationRule> path;
    
    // Find migration rules that apply to this version transition
    for (const auto& rule : migrationRules) {
        if (rule.fromVersion == fromVersion && rule.toVersion == toVersion) {
            path.add(rule);
        }
    }
    
    return path;
}

//==============================================================================
// Auto-Fix Capabilities
//==============================================================================

ThemePresets::ThemePreset ThemeValidator::autoFixTheme(const ThemePresets::ThemePreset& preset, 
                                                      const ValidationReport& report) const
{
    auto fixedPreset = preset;
    
    for (const auto& issue : report.issues) {
        if (canAutoFix(issue)) {
            fixedPreset = fixIssue(fixedPreset, issue);
        }
    }
    
    return fixedPreset;
}

ThemePresets::ThemePreset ThemeValidator::fixIssue(const ThemePresets::ThemePreset& preset, 
                                                  const ValidationIssue& issue) const
{
    auto fixedPreset = preset;
    
    if (issue.category == ValidationCategory::Colors && issue.property.isNotEmpty()) {
        // Fix color issues
        if (issue.message.contains("Invalid color value")) {
            // Set to default color
            fixedPreset.colorValues.set(issue.property, "FF808080");  // Default gray
        }
    } else if (issue.category == ValidationCategory::Accessibility) {
        // Fix accessibility issues
        if (issue.message.contains("Insufficient contrast")) {
            // Adjust color for better contrast
            auto color = parseColor(preset.colorValues[issue.property]);
            if (color != juce::Colour()) {
                // Make text color more contrasting
                if (issue.property.contains("Text")) {
                    auto background = parseColor(preset.colorValues["WindowBackground"]);
                    if (background.getBrightness() > 0.5f) {
                        // Light background, use dark text
                        fixedPreset.colorValues.set(issue.property, "FF000000");
                    } else {
                        // Dark background, use light text
                        fixedPreset.colorValues.set(issue.property, "FFFFFFFF");
                    }
                }
            }
        }
    } else if (issue.category == ValidationCategory::Performance) {
        // Fix performance issues
        if (issue.message.contains("Gradient has") && issue.message.contains("stops")) {
            // Simplify gradient
            auto colors = parseGradient(preset.gradientValues[issue.property]);
            if (colors.size() > MAX_GRADIENT_STOPS) {
                // Keep only first and last colors
                auto simplifiedGradient = colors[0].toString() + "," + colors.getLast().toString();
                fixedPreset.gradientValues.set(issue.property, simplifiedGradient);
            }
        }
    }
    
    return fixedPreset;
}

bool ThemeValidator::canAutoFix(const ValidationIssue& issue) const
{
    return issue.canAutoFix;
}

//==============================================================================
// Validation Rules Management
//==============================================================================

void ThemeValidator::addValidationRule(ValidationCategory category, 
                                      const juce::String& ruleName,
                                      std::function<ValidationIssue(const ThemePresets::ThemePreset&)> ruleFunction)
{
    CustomValidationRule rule;
    rule.name = ruleName;
    rule.function = ruleFunction;
    
    if (!customRules.contains(category)) {
        customRules.set(category, juce::Array<CustomValidationRule>());
    }
    
    customRules.getReference(category).add(rule);
}

void ThemeValidator::removeValidationRule(ValidationCategory category, const juce::String& ruleName)
{
    if (customRules.contains(category)) {
        auto& rules = customRules.getReference(category);
        for (int i = rules.size() - 1; i >= 0; --i) {
            if (rules[i].name == ruleName) {
                rules.remove(i);
                break;
            }
        }
    }
}

void ThemeValidator::setCategoryEnabled(ValidationCategory category, bool enabled)
{
    categoryEnabled.set(category, enabled);
}

bool ThemeValidator::isCategoryEnabled(ValidationCategory category) const
{
    return categoryEnabled[category];
}

//==============================================================================
// Utility Functions
//==============================================================================

juce::String ThemeValidator::getValidationResultName(ValidationResult result)
{
    switch (result) {
        case ValidationResult::Valid:
            return "Valid";
        case ValidationResult::Warning:
            return "Warning";
        case ValidationResult::Error:
            return "Error";
        case ValidationResult::AccessibilityFail:
            return "Accessibility Fail";
        case ValidationResult::PerformanceFail:
            return "Performance Fail";
        case ValidationResult::CompatibilityFail:
            return "Compatibility Fail";
        default:
            return "Unknown";
    }
}

juce::String ThemeValidator::getValidationCategoryName(ValidationCategory category)
{
    switch (category) {
        case ValidationCategory::Colors:
            return "Colors";
        case ValidationCategory::Gradients:
            return "Gradients";
        case ValidationCategory::Fonts:
            return "Fonts";
        case ValidationCategory::Accessibility:
            return "Accessibility";
        case ValidationCategory::Performance:
            return "Performance";
        case ValidationCategory::Compatibility:
            return "Compatibility";
        default:
            return "Unknown";
    }
}

//==============================================================================
// Configuration
//==============================================================================

void ThemeValidator::setMinimumContrastRatio(float ratio)
{
    minimumContrastRatio = juce::jlimit(1.0f, 21.0f, ratio);
}

float ThemeValidator::getMinimumContrastRatio() const
{
    return minimumContrastRatio;
}

void ThemeValidator::setAccessibilityValidationEnabled(bool enabled)
{
    accessibilityValidationEnabled = enabled;
}

bool ThemeValidator::isAccessibilityValidationEnabled() const
{
    return accessibilityValidationEnabled;
}

void ThemeValidator::setPerformanceValidationEnabled(bool enabled)
{
    performanceValidationEnabled = enabled;
}

bool ThemeValidator::isPerformanceValidationEnabled() const
{
    return performanceValidationEnabled;
}

//==============================================================================
// State Management
//==============================================================================

bool ThemeValidator::saveToINI() const
{
    try {
        ComponentState state;
        
        state.setValue("MinimumContrastRatio", minimumContrastRatio);
        state.setValue("AccessibilityValidationEnabled", accessibilityValidationEnabled);
        state.setValue("PerformanceValidationEnabled", performanceValidationEnabled);
        
        // Save category enabled states
        state.setValue("ColorsEnabled", categoryEnabled[ValidationCategory::Colors]);
        state.setValue("GradientsEnabled", categoryEnabled[ValidationCategory::Gradients]);
        state.setValue("FontsEnabled", categoryEnabled[ValidationCategory::Fonts]);
        state.setValue("AccessibilityEnabled", categoryEnabled[ValidationCategory::Accessibility]);
        state.setValue("PerformanceEnabled", categoryEnabled[ValidationCategory::Performance]);
        state.setValue("CompatibilityEnabled", categoryEnabled[ValidationCategory::Compatibility]);
        
        return INIDataManager::saveComponentState("ThemeValidator", state);
    } catch (...) {
        return false;
    }
}

bool ThemeValidator::loadFromINI()
{
    try {
        auto state = INIDataManager::loadComponentState("ThemeValidator");
        
        minimumContrastRatio = static_cast<float>(state.getDoubleValue("MinimumContrastRatio", DEFAULT_MIN_CONTRAST_RATIO));
        accessibilityValidationEnabled = state.getBoolValue("AccessibilityValidationEnabled", true);
        performanceValidationEnabled = state.getBoolValue("PerformanceValidationEnabled", true);
        
        // Load category enabled states
        categoryEnabled.set(ValidationCategory::Colors, state.getBoolValue("ColorsEnabled", true));
        categoryEnabled.set(ValidationCategory::Gradients, state.getBoolValue("GradientsEnabled", true));
        categoryEnabled.set(ValidationCategory::Fonts, state.getBoolValue("FontsEnabled", true));
        categoryEnabled.set(ValidationCategory::Accessibility, state.getBoolValue("AccessibilityEnabled", true));
        categoryEnabled.set(ValidationCategory::Performance, state.getBoolValue("PerformanceEnabled", true));
        categoryEnabled.set(ValidationCategory::Compatibility, state.getBoolValue("CompatibilityEnabled", true));
        
        return true;
    } catch (...) {
        return false;
    }
}

void ThemeValidator::resetToDefaults()
{
    minimumContrastRatio = DEFAULT_MIN_CONTRAST_RATIO;
    accessibilityValidationEnabled = true;
    performanceValidationEnabled = true;
    
    // Reset all categories to enabled
    for (auto& pair : categoryEnabled) {
        pair.second = true;
    }
    
    customRules.clear();
}

//==============================================================================
// Private Methods
//==============================================================================

ThemeValidator::ValidationIssue ThemeValidator::createIssue(ValidationCategory category, 
                                                           ValidationResult severity,
                                                           const juce::String& property,
                                                           const juce::String& message,
                                                           const juce::String& suggestion,
                                                           bool canAutoFix) const
{
    ValidationIssue issue;
    issue.category = category;
    issue.severity = severity;
    issue.property = property;
    issue.message = message;
    issue.suggestion = suggestion;
    issue.canAutoFix = canAutoFix;
    
    return issue;
}

juce::Array<ThemeValidator::ValidationIssue> 
ThemeValidator::validateRequiredColors(const juce::StringPairArray& colorValues) const
{
    juce::Array<ValidationIssue> issues;
    
    for (const auto& requiredColor : requiredColors) {
        if (!colorValues.containsKey(requiredColor)) {
            issues.add(createIssue(ValidationCategory::Colors,
                                 ValidationResult::Error,
                                 requiredColor,
                                 "Required color '" + requiredColor + "' is missing",
                                 "Add the required color definition",
                                 true));
        }
    }
    
    return issues;
}

juce::Array<ThemeValidator::ValidationIssue> 
ThemeValidator::validateGradients(const juce::StringPairArray& gradientValues) const
{
    juce::Array<ValidationIssue> issues;
    
    for (const auto& gradientName : gradientValues.getAllKeys()) {
        auto gradientValue = gradientValues[gradientName];
        
        if (!isValidGradientValue(gradientValue)) {
            issues.add(createIssue(ValidationCategory::Gradients,
                                 ValidationResult::Error,
                                 gradientName,
                                 "Invalid gradient value: " + gradientValue,
                                 "Use format: color1,color2[,color3...]",
                                 true));
        }
    }
    
    return issues;
}

juce::Array<ThemeValidator::ValidationIssue> 
ThemeValidator::validateFonts(const juce::StringPairArray& fontSettings) const
{
    juce::Array<ValidationIssue> issues;
    
    for (const auto& fontName : fontSettings.getAllKeys()) {
        auto fontValue = fontSettings[fontName];
        
        if (!isValidFontValue(fontValue)) {
            issues.add(createIssue(ValidationCategory::Fonts,
                                 ValidationResult::Error,
                                 fontName,
                                 "Invalid font value: " + fontValue,
                                 "Use format: fontName,size[,style]"));
        }
    }
    
    return issues;
}

float ThemeValidator::calculateAccessibilityScore(const ThemePresets::ThemePreset& preset) const
{
    float score = 100.0f;
    
    // Check contrast ratios
    auto primaryText = parseColor(preset.colorValues["PrimaryText"]);
    auto background = parseColor(preset.colorValues["WindowBackground"]);
    
    if (primaryText != juce::Colour() && background != juce::Colour()) {
        auto contrastRatio = calculateContrastRatio(primaryText, background);
        if (contrastRatio < minimumContrastRatio) {
            score -= 30.0f;  // Major penalty for insufficient contrast
        } else if (contrastRatio < 7.0f) {
            score -= 10.0f;  // Minor penalty for not meeting AAA standard
        }
    }
    
    return juce::jmax(0.0f, score);
}

float ThemeValidator::calculatePerformanceScore(const ThemePresets::ThemePreset& preset) const
{
    float score = 100.0f;
    
    // Check gradient complexity
    for (const auto& gradientValue : preset.gradientValues.getAllValues()) {
        auto colors = parseGradient(gradientValue);
        if (colors.size() > MAX_GRADIENT_STOPS) {
            score -= 20.0f;  // Penalty for complex gradients
        }
    }
    
    // Check font count
    if (preset.fontSettings.size() > MAX_FONT_VARIANTS) {
        score -= 10.0f;  // Penalty for too many font variants
    }
    
    return juce::jmax(0.0f, score);
}

float ThemeValidator::calculateQualityScore(const ValidationReport& report) const
{
    float score = 100.0f;
    
    for (const auto& issue : report.issues) {
        switch (issue.severity) {
            case ValidationResult::Error:
            case ValidationResult::AccessibilityFail:
            case ValidationResult::PerformanceFail:
            case ValidationResult::CompatibilityFail:
                score -= 25.0f;
                break;
            case ValidationResult::Warning:
                score -= 10.0f;
                break;
            default:
                break;
        }
    }
    
    return juce::jmax(0.0f, score);
}

bool ThemeValidator::isValidColorValue(const juce::String& colorValue) const
{
    if (colorValue.length() != 8) {
        return false;
    }
    
    for (int i = 0; i < colorValue.length(); ++i) {
        auto c = colorValue[i];
        if (!juce::CharacterFunctions::isHexDigit(c)) {
            return false;
        }
    }
    
    return true;
}

bool ThemeValidator::isValidGradientValue(const juce::String& gradientValue) const
{
    auto colors = juce::StringArray::fromTokens(gradientValue, ",", "");
    
    if (colors.size() < 2) {
        return false;
    }
    
    for (const auto& color : colors) {
        if (!isValidColorValue(color.trim())) {
            return false;
        }
    }
    
    return true;
}

bool ThemeValidator::isValidFontValue(const juce::String& fontValue) const
{
    auto parts = juce::StringArray::fromTokens(fontValue, ",", "");
    
    if (parts.size() < 2) {
        return false;
    }
    
    // Check if size is a valid number
    auto sizeStr = parts[1].trim();
    return sizeStr.containsOnly("0123456789.");
}

juce::Colour ThemeValidator::parseColor(const juce::String& colorValue) const
{
    if (isValidColorValue(colorValue)) {
        return juce::Colour::fromString(colorValue);
    }
    
    return juce::Colour();
}

juce::Array<juce::Colour> ThemeValidator::parseGradient(const juce::String& gradientValue) const
{
    juce::Array<juce::Colour> colors;
    
    auto colorStrings = juce::StringArray::fromTokens(gradientValue, ",", "");
    
    for (const auto& colorString : colorStrings) {
        auto color = parseColor(colorString.trim());
        if (color != juce::Colour()) {
            colors.add(color);
        }
    }
    
    return colors;
}

void ThemeValidator::initializeMigrationRules()
{
    // Add migration rules for different version transitions
    // Example: migrating from version 1.0 to 2.0
    
    MigrationRule rule;
    rule.fromVersion = "1.0";
    rule.toVersion = "2.0";
    rule.property = "colors";
    rule.oldName = "BackgroundColor";
    rule.newName = "WindowBackground";
    rule.defaultValue = "FF2D2D2D";
    rule.required = true;
    
    migrationRules.add(rule);
}

void ThemeValidator::initializeValidationRules()
{
    // Custom validation rules can be added here
    // This is where you would add domain-specific validation logic
}

ThemePresets::ThemePreset ThemeValidator::applyMigrationRules(const ThemePresets::ThemePreset& preset,
                                                             const juce::Array<MigrationRule>& rules) const
{
    auto migratedPreset = preset;
    
    for (const auto& rule : rules) {
        if (rule.property == "colors") {
            // Migrate color property
            if (migratedPreset.colorValues.containsKey(rule.oldName)) {
                auto value = migratedPreset.colorValues[rule.oldName];
                migratedPreset.colorValues.remove(rule.oldName);
                migratedPreset.colorValues.set(rule.newName, value);
            } else if (rule.required && !migratedPreset.colorValues.containsKey(rule.newName)) {
                migratedPreset.colorValues.set(rule.newName, rule.defaultValue);
            }
        }
        // Add more property types as needed
    }
    
    // Update version
    migratedPreset.version = rules.isEmpty() ? preset.version : rules.getLast().toVersion;
    migratedPreset.modifiedTime = juce::Time::getCurrentTime();
    
    return migratedPreset;
}

} // namespace Themes
} // namespace UI
} // namespace OTTO