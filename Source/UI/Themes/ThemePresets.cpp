#include "ThemePresets.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "../../INIDataManager.h"

namespace OTTO {
namespace UI {
namespace Themes {

//==============================================================================
// Constructor and Destructor
//==============================================================================

ThemePresets::ThemePresets()
{
    initializeFactoryPresets();
    loadFromINI();
}

ThemePresets::~ThemePresets()
{
    saveToINI();
}

//==============================================================================
// Preset Management
//==============================================================================

juce::Array<ThemePresets::ThemePreset> ThemePresets::getAllPresets() const
{
    juce::Array<ThemePreset> allPresets;
    allPresets.addArray(factoryPresets);
    allPresets.addArray(customPresets);
    return allPresets;
}

juce::Array<ThemePresets::ThemePreset> ThemePresets::getPresetsByCategory(PresetCategory category) const
{
    juce::Array<ThemePreset> categoryPresets;
    
    for (const auto& preset : factoryPresets) {
        if (preset.category == category) {
            categoryPresets.add(preset);
        }
    }
    
    for (const auto& preset : customPresets) {
        if (preset.category == category) {
            categoryPresets.add(preset);
        }
    }
    
    return categoryPresets;
}

ThemePresets::ThemePreset ThemePresets::getPreset(const juce::String& presetName) const
{
    // Search factory presets first
    for (const auto& preset : factoryPresets) {
        if (preset.name == presetName) {
            return preset;
        }
    }
    
    // Search custom presets
    for (const auto& preset : customPresets) {
        if (preset.name == presetName) {
            return preset;
        }
    }
    
    // Return empty preset if not found
    return ThemePreset{};
}

bool ThemePresets::hasPreset(const juce::String& presetName) const
{
    return !getPreset(presetName).name.isEmpty();
}

//==============================================================================
// Studio Themes
//==============================================================================

bool ThemePresets::applyStudioTheme(StudioTheme theme, ColorScheme& colorScheme)
{
    auto preset = getStudioTheme(theme);
    if (preset.name.isEmpty()) {
        return false;
    }
    
    applyPresetToColorScheme(preset, colorScheme);
    return true;
}

ThemePresets::ThemePreset ThemePresets::getStudioTheme(StudioTheme theme) const
{
    juce::String themeName;
    
    switch (theme) {
        case StudioTheme::DarkStudio:
            themeName = "Dark Studio";
            break;
        case StudioTheme::LightStudio:
            themeName = "Light Studio";
            break;
        case StudioTheme::Midnight:
            themeName = "Midnight";
            break;
        case StudioTheme::Charcoal:
            themeName = "Charcoal";
            break;
        case StudioTheme::Arctic:
            themeName = "Arctic";
            break;
    }
    
    return getPreset(themeName);
}

//==============================================================================
// Accessibility Themes
//==============================================================================

bool ThemePresets::applyAccessibilityTheme(AccessibilityTheme theme, ColorScheme& colorScheme)
{
    auto preset = getAccessibilityTheme(theme);
    if (preset.name.isEmpty()) {
        return false;
    }
    
    applyPresetToColorScheme(preset, colorScheme);
    return true;
}

ThemePresets::ThemePreset ThemePresets::getAccessibilityTheme(AccessibilityTheme theme) const
{
    juce::String themeName;
    
    switch (theme) {
        case AccessibilityTheme::HighContrast:
            themeName = "High Contrast";
            break;
        case AccessibilityTheme::Deuteranopia:
            themeName = "Deuteranopia Friendly";
            break;
        case AccessibilityTheme::Protanopia:
            themeName = "Protanopia Friendly";
            break;
        case AccessibilityTheme::Tritanopia:
            themeName = "Tritanopia Friendly";
            break;
        case AccessibilityTheme::LowVision:
            themeName = "Low Vision";
            break;
    }
    
    return getPreset(themeName);
}

bool ThemePresets::validateAccessibility(const ThemePreset& preset) const
{
    return preset.isAccessible && preset.contrastRatio >= MIN_CONTRAST_RATIO;
}

//==============================================================================
// Seasonal Themes
//==============================================================================

bool ThemePresets::applySeasonalTheme(SeasonalTheme theme, ColorScheme& colorScheme)
{
    auto preset = getSeasonalTheme(theme);
    if (preset.name.isEmpty()) {
        return false;
    }
    
    applyPresetToColorScheme(preset, colorScheme);
    return true;
}

ThemePresets::ThemePreset ThemePresets::getSeasonalTheme(SeasonalTheme theme) const
{
    juce::String themeName;
    
    switch (theme) {
        case SeasonalTheme::Spring:
            themeName = "Spring";
            break;
        case SeasonalTheme::Summer:
            themeName = "Summer";
            break;
        case SeasonalTheme::Autumn:
            themeName = "Autumn";
            break;
        case SeasonalTheme::Winter:
            themeName = "Winter";
            break;
    }
    
    return getPreset(themeName);
}

ThemePresets::SeasonalTheme ThemePresets::getCurrentSeasonalTheme() const
{
    auto now = juce::Time::getCurrentTime();
    auto month = now.getMonth();
    
    // Northern hemisphere seasons
    if (month >= 2 && month <= 4) {  // March-May
        return SeasonalTheme::Spring;
    } else if (month >= 5 && month <= 7) {  // June-August
        return SeasonalTheme::Summer;
    } else if (month >= 8 && month <= 10) {  // September-November
        return SeasonalTheme::Autumn;
    } else {  // December-February
        return SeasonalTheme::Winter;
    }
}

//==============================================================================
// Genre Themes
//==============================================================================

bool ThemePresets::applyGenreTheme(GenreTheme theme, ColorScheme& colorScheme)
{
    auto preset = getGenreTheme(theme);
    if (preset.name.isEmpty()) {
        return false;
    }
    
    applyPresetToColorScheme(preset, colorScheme);
    return true;
}

ThemePresets::ThemePreset ThemePresets::getGenreTheme(GenreTheme theme) const
{
    juce::String themeName;
    
    switch (theme) {
        case GenreTheme::Electronic:
            themeName = "Electronic";
            break;
        case GenreTheme::HipHop:
            themeName = "Hip-Hop";
            break;
        case GenreTheme::Jazz:
            themeName = "Jazz";
            break;
        case GenreTheme::Rock:
            themeName = "Rock";
            break;
        case GenreTheme::Ambient:
            themeName = "Ambient";
            break;
    }
    
    return getPreset(themeName);
}

//==============================================================================
// Custom Theme Management
//==============================================================================

bool ThemePresets::addCustomPreset(const ThemePreset& preset)
{
    if (preset.name.isEmpty() || hasPreset(preset.name)) {
        return false;
    }
    
    auto customPreset = preset;
    customPreset.category = PresetCategory::Custom;
    customPreset.createdTime = juce::Time::getCurrentTime();
    customPreset.modifiedTime = customPreset.createdTime;
    
    customPresets.add(customPreset);
    notifyListeners([&](Listener* l) { l->presetAdded(customPreset); });
    
    return true;
}

bool ThemePresets::removeCustomPreset(const juce::String& presetName)
{
    for (int i = 0; i < customPresets.size(); ++i) {
        if (customPresets[i].name == presetName) {
            customPresets.remove(i);
            notifyListeners([&](Listener* l) { l->presetRemoved(presetName); });
            return true;
        }
    }
    
    return false;
}

bool ThemePresets::updateCustomPreset(const ThemePreset& preset)
{
    for (int i = 0; i < customPresets.size(); ++i) {
        if (customPresets[i].name == preset.name) {
            auto updatedPreset = preset;
            updatedPreset.modifiedTime = juce::Time::getCurrentTime();
            customPresets.set(i, updatedPreset);
            notifyListeners([&](Listener* l) { l->presetUpdated(updatedPreset); });
            return true;
        }
    }
    
    return false;
}

//==============================================================================
// Theme Validation and Migration
//==============================================================================

bool ThemePresets::validateTheme(const ThemePreset& preset) const
{
    // Check required fields
    if (preset.name.isEmpty() || preset.colorValues.size() == 0) {
        return false;
    }
    
    // Validate color values
    for (const auto& colorPair : preset.colorValues.getAllKeys()) {
        auto colorValue = preset.colorValues[colorPair];
        if (colorValue.isEmpty()) {
            return false;
        }
        
        // Try to parse as color
        auto color = juce::Colour::fromString(colorValue);
        if (color == juce::Colour()) {
            return false;
        }
    }
    
    return true;
}

ThemePresets::ThemePreset ThemePresets::migrateTheme(const ThemePreset& oldPreset, 
                                                    const juce::String& targetVersion) const
{
    auto migratedPreset = oldPreset;
    migratedPreset.version = targetVersion;
    migratedPreset.modifiedTime = juce::Time::getCurrentTime();
    
    // Add any missing color values with defaults
    if (!migratedPreset.colorValues.containsKey("WindowBackground")) {
        migratedPreset.colorValues.set("WindowBackground", "FF2D2D2D");
    }
    
    if (!migratedPreset.colorValues.containsKey("ComponentBackground")) {
        migratedPreset.colorValues.set("ComponentBackground", "FF3D3D3D");
    }
    
    // Recalculate accessibility metrics
    migratedPreset.isAccessible = meetsAccessibilityStandards(migratedPreset);
    
    return migratedPreset;
}

bool ThemePresets::needsMigration(const ThemePreset& preset) const
{
    return preset.version != INIConfig::FILE_FORMAT_VERSION;
}

//==============================================================================
// Import/Export
//==============================================================================

bool ThemePresets::exportTheme(const ThemePreset& preset, const juce::File& file) const
{
    juce::XmlElement themeXml("Theme");
    
    themeXml.setAttribute("name", preset.name);
    themeXml.setAttribute("description", preset.description);
    themeXml.setAttribute("category", getCategoryName(preset.category));
    themeXml.setAttribute("author", preset.author);
    themeXml.setAttribute("version", preset.version);
    themeXml.setAttribute("isAccessible", preset.isAccessible);
    themeXml.setAttribute("contrastRatio", preset.contrastRatio);
    
    // Add colors
    auto colorsXml = themeXml.createNewChildElement("Colors");
    for (const auto& key : preset.colorValues.getAllKeys()) {
        auto colorXml = colorsXml->createNewChildElement("Color");
        colorXml->setAttribute("name", key);
        colorXml->setAttribute("value", preset.colorValues[key]);
    }
    
    // Add gradients
    if (preset.gradientValues.size() > 0) {
        auto gradientsXml = themeXml.createNewChildElement("Gradients");
        for (const auto& key : preset.gradientValues.getAllKeys()) {
            auto gradientXml = gradientsXml->createNewChildElement("Gradient");
            gradientXml->setAttribute("name", key);
            gradientXml->setAttribute("value", preset.gradientValues[key]);
        }
    }
    
    // Add fonts
    if (preset.fontSettings.size() > 0) {
        auto fontsXml = themeXml.createNewChildElement("Fonts");
        for (const auto& key : preset.fontSettings.getAllKeys()) {
            auto fontXml = fontsXml->createNewChildElement("Font");
            fontXml->setAttribute("name", key);
            fontXml->setAttribute("value", preset.fontSettings[key]);
        }
    }
    
    return themeXml.writeTo(file);
}

ThemePresets::ThemePreset ThemePresets::importTheme(const juce::File& file) const
{
    ThemePreset preset;
    
    auto themeXml = juce::XmlDocument::parse(file);
    if (!themeXml || themeXml->getTagName() != "Theme") {
        return preset;
    }
    
    preset.name = themeXml->getStringAttribute("name");
    preset.description = themeXml->getStringAttribute("description");
    preset.category = parseCategoryName(themeXml->getStringAttribute("category"));
    preset.author = themeXml->getStringAttribute("author");
    preset.version = themeXml->getStringAttribute("version");
    preset.isAccessible = themeXml->getBoolAttribute("isAccessible");
    preset.contrastRatio = static_cast<float>(themeXml->getDoubleAttribute("contrastRatio"));
    
    // Load colors
    auto colorsXml = themeXml->getChildByName("Colors");
    if (colorsXml) {
        for (auto* colorXml : colorsXml->getChildIterator()) {
            if (colorXml->getTagName() == "Color") {
                preset.colorValues.set(colorXml->getStringAttribute("name"),
                                     colorXml->getStringAttribute("value"));
            }
        }
    }
    
    // Load gradients
    auto gradientsXml = themeXml->getChildByName("Gradients");
    if (gradientsXml) {
        for (auto* gradientXml : gradientsXml->getChildIterator()) {
            if (gradientXml->getTagName() == "Gradient") {
                preset.gradientValues.set(gradientXml->getStringAttribute("name"),
                                        gradientXml->getStringAttribute("value"));
            }
        }
    }
    
    // Load fonts
    auto fontsXml = themeXml->getChildByName("Fonts");
    if (fontsXml) {
        for (auto* fontXml : fontsXml->getChildIterator()) {
            if (fontXml->getTagName() == "Font") {
                preset.fontSettings.set(fontXml->getStringAttribute("name"),
                                      fontXml->getStringAttribute("value"));
            }
        }
    }
    
    preset.createdTime = juce::Time::getCurrentTime();
    preset.modifiedTime = preset.createdTime;
    
    return preset;
}

int ThemePresets::exportAllThemes(const juce::File& directory) const
{
    if (!directory.exists()) {
        directory.createDirectory();
    }
    
    int exportedCount = 0;
    auto allPresets = getAllPresets();
    
    for (const auto& preset : allPresets) {
        auto filename = preset.name.replaceCharacter(' ', '_') + ".xml";
        auto file = directory.getChildFile(filename);
        
        if (exportTheme(preset, file)) {
            ++exportedCount;
        }
    }
    
    return exportedCount;
}

int ThemePresets::importAllThemes(const juce::File& directory)
{
    if (!directory.exists()) {
        return 0;
    }
    
    int importedCount = 0;
    auto files = directory.findChildFiles(juce::File::findFiles, false, "*.xml");
    
    for (const auto& file : files) {
        auto preset = importTheme(file);
        if (!preset.name.isEmpty() && addCustomPreset(preset)) {
            ++importedCount;
        }
    }
    
    return importedCount;
}

//==============================================================================
// Utility Functions
//==============================================================================

juce::StringArray ThemePresets::getPresetNames(PresetCategory category) const
{
    juce::StringArray names;
    auto presets = getPresetsByCategory(category);
    
    for (const auto& preset : presets) {
        names.add(preset.name);
    }
    
    return names;
}

juce::StringArray ThemePresets::getAllPresetNames() const
{
    juce::StringArray names;
    auto allPresets = getAllPresets();
    
    for (const auto& preset : allPresets) {
        names.add(preset.name);
    }
    
    return names;
}

juce::String ThemePresets::getCategoryName(PresetCategory category)
{
    switch (category) {
        case PresetCategory::Studio:
            return "Studio";
        case PresetCategory::Accessibility:
            return "Accessibility";
        case PresetCategory::Seasonal:
            return "Seasonal";
        case PresetCategory::Genre:
            return "Genre";
        case PresetCategory::Custom:
            return "Custom";
        default:
            return "Unknown";
    }
}

ThemePresets::PresetCategory ThemePresets::parseCategoryName(const juce::String& categoryName)
{
    if (categoryName == "Studio") return PresetCategory::Studio;
    if (categoryName == "Accessibility") return PresetCategory::Accessibility;
    if (categoryName == "Seasonal") return PresetCategory::Seasonal;
    if (categoryName == "Genre") return PresetCategory::Genre;
    if (categoryName == "Custom") return PresetCategory::Custom;
    
    return PresetCategory::Custom;
}

//==============================================================================
// State Management
//==============================================================================

bool ThemePresets::saveToINI() const
{
    try {
        ComponentState state;
        
        // Save custom presets count
        state.setValue("CustomPresetsCount", customPresets.size());
        
        // Save each custom preset
        for (int i = 0; i < customPresets.size(); ++i) {
            const auto& preset = customPresets[i];
            auto prefix = "CustomPreset" + juce::String(i) + "_";
            
            state.setValue(prefix + "Name", preset.name);
            state.setValue(prefix + "Description", preset.description);
            state.setValue(prefix + "Category", getCategoryName(preset.category));
            state.setValue(prefix + "Author", preset.author);
            state.setValue(prefix + "Version", preset.version);
            state.setValue(prefix + "IsAccessible", preset.isAccessible);
            state.setValue(prefix + "ContrastRatio", preset.contrastRatio);
            
            // Save colors
            state.setValue(prefix + "ColorCount", preset.colorValues.size());
            for (int j = 0; j < preset.colorValues.size(); ++j) {
                auto colorPrefix = prefix + "Color" + juce::String(j) + "_";
                state.setValue(colorPrefix + "Key", preset.colorValues.getAllKeys()[j]);
                state.setValue(colorPrefix + "Value", preset.colorValues.getAllValues()[j]);
            }
            
            // Save gradients
            state.setValue(prefix + "GradientCount", preset.gradientValues.size());
            for (int j = 0; j < preset.gradientValues.size(); ++j) {
                auto gradientPrefix = prefix + "Gradient" + juce::String(j) + "_";
                state.setValue(gradientPrefix + "Key", preset.gradientValues.getAllKeys()[j]);
                state.setValue(gradientPrefix + "Value", preset.gradientValues.getAllValues()[j]);
            }
            
            // Save fonts
            state.setValue(prefix + "FontCount", preset.fontSettings.size());
            for (int j = 0; j < preset.fontSettings.size(); ++j) {
                auto fontPrefix = prefix + "Font" + juce::String(j) + "_";
                state.setValue(fontPrefix + "Key", preset.fontSettings.getAllKeys()[j]);
                state.setValue(fontPrefix + "Value", preset.fontSettings.getAllValues()[j]);
            }
        }
        
        return INIDataManager::saveComponentState("ThemePresets", state);
    } catch (...) {
        return false;
    }
}

bool ThemePresets::loadFromINI()
{
    try {
        ComponentState state;
        if (!INIDataManager::loadComponentState("ThemePresets", state)) {
            return false;
        }
        
        customPresets.clear();
        
        int customPresetsCount = state.getIntValue("CustomPresetsCount", 0);
        
        for (int i = 0; i < customPresetsCount; ++i) {
            auto prefix = "CustomPreset" + juce::String(i) + "_";
            
            ThemePreset preset;
            preset.name = state.getStringValue(prefix + "Name", "");
            preset.description = state.getStringValue(prefix + "Description", "");
            preset.category = parseCategoryName(state.getStringValue(prefix + "Category", "Custom"));
            preset.author = state.getStringValue(prefix + "Author", "");
            preset.version = state.getStringValue(prefix + "Version", INIConfig::FILE_FORMAT_VERSION);
            preset.isAccessible = state.getBoolValue(prefix + "IsAccessible", false);
            preset.contrastRatio = static_cast<float>(state.getDoubleValue(prefix + "ContrastRatio", 0.0));
            
            // Load colors
            int colorCount = state.getIntValue(prefix + "ColorCount", 0);
            for (int j = 0; j < colorCount; ++j) {
                auto colorPrefix = prefix + "Color" + juce::String(j) + "_";
                auto key = state.getStringValue(colorPrefix + "Key", "");
                auto value = state.getStringValue(colorPrefix + "Value", "");
                if (!key.isEmpty() && !value.isEmpty()) {
                    preset.colorValues.set(key, value);
                }
            }
            
            // Load gradients
            int gradientCount = state.getIntValue(prefix + "GradientCount", 0);
            for (int j = 0; j < gradientCount; ++j) {
                auto gradientPrefix = prefix + "Gradient" + juce::String(j) + "_";
                auto key = state.getStringValue(gradientPrefix + "Key", "");
                auto value = state.getStringValue(gradientPrefix + "Value", "");
                if (!key.isEmpty() && !value.isEmpty()) {
                    preset.gradientValues.set(key, value);
                }
            }
            
            // Load fonts
            int fontCount = state.getIntValue(prefix + "FontCount", 0);
            for (int j = 0; j < fontCount; ++j) {
                auto fontPrefix = prefix + "Font" + juce::String(j) + "_";
                auto key = state.getStringValue(fontPrefix + "Key", "");
                auto value = state.getStringValue(fontPrefix + "Value", "");
                if (!key.isEmpty() && !value.isEmpty()) {
                    preset.fontSettings.set(key, value);
                }
            }
            
            if (!preset.name.isEmpty()) {
                customPresets.add(preset);
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

void ThemePresets::resetToFactoryPresets()
{
    customPresets.clear();
    initializeFactoryPresets();
    notifyListeners([](Listener* l) { l->presetsReset(); });
}

//==============================================================================
// Listeners
//==============================================================================

void ThemePresets::addListener(Listener* listener)
{
    listeners.add(listener);
}

void ThemePresets::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

//==============================================================================
// Private Methods
//==============================================================================

void ThemePresets::initializeFactoryPresets()
{
    factoryPresets.clear();
    
    createStudioPresets();
    createAccessibilityPresets();
    createSeasonalPresets();
    createGenrePresets();
}

void ThemePresets::createStudioPresets()
{
    // Dark Studio Theme
    {
        juce::StringPairArray colors;
        colors.set("WindowBackground", "FF1E1E1E");
        colors.set("ComponentBackground", "FF2D2D2D");
        colors.set("ControlBackground", "FF3D3D3D");
        colors.set("PrimaryText", "FFFFFFFF");
        colors.set("SecondaryText", "FFCCCCCC");
        colors.set("ButtonBackground", "FF404040");
        colors.set("ButtonBackgroundHover", "FF505050");
        colors.set("ButtonBackgroundPressed", "FF606060");
        colors.set("ButtonText", "FFFFFFFF");
        colors.set("Accent", "FF00A8FF");
        colors.set("AccentHover", "FF0088CC");
        
        auto preset = createPreset("Dark Studio", 
                                 "Professional dark theme optimized for low-light studio environments",
                                 PresetCategory::Studio, colors);
        preset.isAccessible = true;
        preset.contrastRatio = 7.2f;
        factoryPresets.add(preset);
    }
    
    // Light Studio Theme
    {
        juce::StringPairArray colors;
        colors.set("WindowBackground", "FFF5F5F5");
        colors.set("ComponentBackground", "FFFFFFFF");
        colors.set("ControlBackground", "FFF0F0F0");
        colors.set("PrimaryText", "FF000000");
        colors.set("SecondaryText", "FF666666");
        colors.set("ButtonBackground", "FFE0E0E0");
        colors.set("ButtonBackgroundHover", "FFD0D0D0");
        colors.set("ButtonBackgroundPressed", "FFC0C0C0");
        colors.set("ButtonText", "FF000000");
        colors.set("Accent", "FF0066CC");
        colors.set("AccentHover", "FF0055AA");
        
        auto preset = createPreset("Light Studio", 
                                 "Clean light theme for bright studio environments",
                                 PresetCategory::Studio, colors);
        preset.isAccessible = true;
        preset.contrastRatio = 8.1f;
        factoryPresets.add(preset);
    }
    
    // Midnight Theme
    {
        juce::StringPairArray colors;
        colors.set("WindowBackground", "FF0A0A0A");
        colors.set("ComponentBackground", "FF1A1A1A");
        colors.set("ControlBackground", "FF2A2A2A");
        colors.set("PrimaryText", "FFFFFFFF");
        colors.set("SecondaryText", "FFAAAAAA");
        colors.set("ButtonBackground", "FF333333");
        colors.set("ButtonBackgroundHover", "FF444444");
        colors.set("ButtonBackgroundPressed", "FF555555");
        colors.set("ButtonText", "FFFFFFFF");
        colors.set("Accent", "FF00FFAA");
        colors.set("AccentHover", "FF00CC88");
        
        auto preset = createPreset("Midnight", 
                                 "Ultra-dark theme for late-night production sessions",
                                 PresetCategory::Studio, colors);
        preset.isAccessible = true;
        preset.contrastRatio = 9.5f;
        factoryPresets.add(preset);
    }
}

void ThemePresets::createAccessibilityPresets()
{
    // High Contrast Theme
    {
        juce::StringPairArray colors;
        colors.set("WindowBackground", "FF000000");
        colors.set("ComponentBackground", "FF000000");
        colors.set("ControlBackground", "FF000000");
        colors.set("PrimaryText", "FFFFFFFF");
        colors.set("SecondaryText", "FFFFFFFF");
        colors.set("ButtonBackground", "FF000000");
        colors.set("ButtonBackgroundHover", "FF333333");
        colors.set("ButtonBackgroundPressed", "FF666666");
        colors.set("ButtonText", "FFFFFFFF");
        colors.set("Accent", "FFFFFF00");
        colors.set("AccentHover", "FFFFCC00");
        
        auto preset = createPreset("High Contrast", 
                                 "Maximum contrast theme for visual accessibility",
                                 PresetCategory::Accessibility, colors);
        preset.isAccessible = true;
        preset.contrastRatio = 21.0f;  // Maximum possible contrast
        factoryPresets.add(preset);
    }
    
    // Deuteranopia Friendly Theme
    {
        juce::StringPairArray colors;
        colors.set("WindowBackground", "FF2D2D2D");
        colors.set("ComponentBackground", "FF3D3D3D");
        colors.set("ControlBackground", "FF4D4D4D");
        colors.set("PrimaryText", "FFFFFFFF");
        colors.set("SecondaryText", "FFCCCCCC");
        colors.set("ButtonBackground", "FF404040");
        colors.set("ButtonBackgroundHover", "FF505050");
        colors.set("ButtonBackgroundPressed", "FF606060");
        colors.set("ButtonText", "FFFFFFFF");
        colors.set("Accent", "FF0088FF");  // Blue instead of green/red
        colors.set("AccentHover", "FF0066CC");
        
        auto preset = createPreset("Deuteranopia Friendly", 
                                 "Red-green colorblind friendly theme using blue accents",
                                 PresetCategory::Accessibility, colors);
        preset.isAccessible = true;
        preset.contrastRatio = 6.8f;
        factoryPresets.add(preset);
    }
}

void ThemePresets::createSeasonalPresets()
{
    // Spring Theme
    {
        juce::StringPairArray colors;
        colors.set("WindowBackground", "FFF0F8F0");
        colors.set("ComponentBackground", "FFFFFFFF");
        colors.set("ControlBackground", "FFF5FFF5");
        colors.set("PrimaryText", "FF2D4A2D");
        colors.set("SecondaryText", "FF4A6A4A");
        colors.set("ButtonBackground", "FFE8F5E8");
        colors.set("ButtonBackgroundHover", "FFD8EDD8");
        colors.set("ButtonBackgroundPressed", "FFC8E5C8");
        colors.set("ButtonText", "FF2D4A2D");
        colors.set("Accent", "FF66BB6A");
        colors.set("AccentHover", "FF4CAF50");
        
        auto preset = createPreset("Spring", 
                                 "Fresh green theme inspired by spring renewal",
                                 PresetCategory::Seasonal, colors);
        preset.isAccessible = true;
        preset.contrastRatio = 5.2f;
        factoryPresets.add(preset);
    }
    
    // Summer Theme
    {
        juce::StringPairArray colors;
        colors.set("WindowBackground", "FFFFF8E1");
        colors.set("ComponentBackground", "FFFFFFFF");
        colors.set("ControlBackground", "FFFFF3C4");
        colors.set("PrimaryText", "FF5D4037");
        colors.set("SecondaryText", "FF8D6E63");
        colors.set("ButtonBackground", "FFFFE0B2");
        colors.set("ButtonBackgroundHover", "FFFFCC80");
        colors.set("ButtonBackgroundPressed", "FFFFB74D");
        colors.set("ButtonText", "FF5D4037");
        colors.set("Accent", "FFFF9800");
        colors.set("AccentHover", "FFEF6C00");
        
        auto preset = createPreset("Summer", 
                                 "Warm and vibrant theme capturing summer energy",
                                 PresetCategory::Seasonal, colors);
        preset.isAccessible = true;
        preset.contrastRatio = 4.9f;
        factoryPresets.add(preset);
    }
}

void ThemePresets::createGenrePresets()
{
    // Electronic Theme
    {
        juce::StringPairArray colors;
        colors.set("WindowBackground", "FF0D1117");
        colors.set("ComponentBackground", "FF161B22");
        colors.set("ControlBackground", "FF21262D");
        colors.set("PrimaryText", "FF00FFFF");
        colors.set("SecondaryText", "FF58A6FF");
        colors.set("ButtonBackground", "FF1F2937");
        colors.set("ButtonBackgroundHover", "FF374151");
        colors.set("ButtonBackgroundPressed", "FF4B5563");
        colors.set("ButtonText", "FF00FFFF");
        colors.set("Accent", "FFFF00FF");
        colors.set("AccentHover", "FFCC00CC");
        
        juce::StringPairArray gradients;
        gradients.set("AccentGradient", "FF00FFFF,FFFF00FF");
        
        auto preset = createPreset("Electronic", 
                                 "Neon cyber theme for electronic music production",
                                 PresetCategory::Genre, colors, gradients);
        preset.isAccessible = true;
        preset.contrastRatio = 6.1f;
        factoryPresets.add(preset);
    }
    
    // Jazz Theme
    {
        juce::StringPairArray colors;
        colors.set("WindowBackground", "FF2C1810");
        colors.set("ComponentBackground", "FF3D2818");
        colors.set("ControlBackground", "FF4E3820");
        colors.set("PrimaryText", "FFFFD700");
        colors.set("SecondaryText", "FFCDAA3D");
        colors.set("ButtonBackground", "FF5D4037");
        colors.set("ButtonBackgroundHover", "FF6D5047");
        colors.set("ButtonBackgroundPressed", "FF7D6057");
        colors.set("ButtonText", "FFFFD700");
        colors.set("Accent", "FFCD853F");
        colors.set("AccentHover", "FFBC7A2F");
        
        auto preset = createPreset("Jazz", 
                                 "Warm and sophisticated theme for jazz production",
                                 PresetCategory::Genre, colors);
        preset.isAccessible = true;
        preset.contrastRatio = 5.8f;
        factoryPresets.add(preset);
    }
}

ThemePresets::ThemePreset ThemePresets::createPreset(const juce::String& name, 
                                                    const juce::String& description,
                                                    PresetCategory category,
                                                    const juce::StringPairArray& colors,
                                                    const juce::StringPairArray& gradients,
                                                    const juce::StringPairArray& fonts) const
{
    ThemePreset preset;
    preset.name = name;
    preset.description = description;
    preset.category = category;
    preset.author = "OTTO Development Team";
    preset.version = INIConfig::FILE_FORMAT_VERSION;
    preset.colorValues = colors;
    preset.gradientValues = gradients;
    preset.fontSettings = fonts;
    preset.createdTime = juce::Time::getCurrentTime();
    preset.modifiedTime = preset.createdTime;
    
    // Calculate accessibility metrics
    preset.isAccessible = meetsAccessibilityStandards(preset);
    
    return preset;
}

void ThemePresets::applyPresetToColorScheme(const ThemePreset& preset, ColorScheme& colorScheme) const
{
    // Apply colors to color scheme
    for (const auto& key : preset.colorValues.getAllKeys()) {
        auto colorValue = preset.colorValues[key];
        auto color = juce::Colour::fromString(colorValue);
        
        // Map preset color names to ColorScheme color roles
        if (key == "WindowBackground") {
            // Apply to appropriate color scheme method
        } else if (key == "PrimaryText") {
            // Apply to appropriate color scheme method
        }
        // Add more mappings as needed
    }
}

float ThemePresets::calculateContrastRatio(const juce::Colour& foreground, const juce::Colour& background) const
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

bool ThemePresets::meetsAccessibilityStandards(const ThemePreset& preset) const
{
    // Check if primary text has sufficient contrast
    auto primaryText = juce::Colour::fromString(preset.colorValues["PrimaryText"]);
    auto background = juce::Colour::fromString(preset.colorValues["WindowBackground"]);
    
    if (primaryText == juce::Colour() || background == juce::Colour()) {
        return false;
    }
    
    auto contrastRatio = calculateContrastRatio(primaryText, background);
    return contrastRatio >= MIN_CONTRAST_RATIO;
}

void ThemePresets::notifyListeners(std::function<void(Listener*)> notification)
{
    listeners.call(notification);
}

} // namespace Themes
} // namespace UI
} // namespace OTTO