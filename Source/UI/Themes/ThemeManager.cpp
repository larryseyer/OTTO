#include "ThemeManager.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "INIDataManager.h"
#include <cmath>

//==============================================================================
// ThemeColors Implementation
//==============================================================================

void ThemeManager::ThemeColors::setDefaults() {
    // Base colors - Professional dark theme defaults
    primary = juce::Colour(0xFF2196F3);      // Material Blue
    secondary = juce::Colour(0xFF424242);    // Dark Grey
    accent = juce::Colour(0xFFFF5722);       // Deep Orange
    background = juce::Colour(0xFF121212);   // Dark Background
    surface = juce::Colour(0xFF1E1E1E);      // Surface Dark
    error = juce::Colour(0xFFF44336);        // Material Red
    warning = juce::Colour(0xFFFF9800);      // Material Orange
    success = juce::Colour(0xFF4CAF50);      // Material Green
    
    // Text colors
    onPrimary = juce::Colours::white;
    onSecondary = juce::Colours::white;
    onBackground = juce::Colour(0xFFE0E0E0);
    onSurface = juce::Colour(0xFFE0E0E0);
    onError = juce::Colours::white;
    
    // State colors
    hover = primary.withAlpha(0.1f);
    pressed = primary.withAlpha(0.2f);
    disabled = juce::Colour(0xFF616161);
    selected = primary.withAlpha(0.3f);
    
    // Audio-specific colors
    meterLow = juce::Colour(0xFF4CAF50);     // Green
    meterMid = juce::Colour(0xFFFF9800);     // Orange
    meterHigh = juce::Colour(0xFFF44336);    // Red
    meterPeak = juce::Colour(0xFFFFFFFF);    // White
    waveform = juce::Colour(0xFF03DAC6);     // Teal
    spectrum = juce::Colour(0xFF6200EA);     // Purple
    
    // Gradient defaults
    backgroundGradient = juce::ColourGradient(
        background, 0.0f, 0.0f,
        background.brighter(0.1f), 0.0f, 1.0f,
        false
    );
    accentGradient = juce::ColourGradient(
        accent, 0.0f, 0.0f,
        accent.brighter(0.2f), 1.0f, 0.0f,
        false
    );
    useGradients = false;
}

void ThemeManager::ThemeColors::applyAccessibilityMode(AccessibilityMode mode) {
    switch (mode) {
        case AccessibilityMode::HighContrast:
            // Increase contrast ratios
            background = juce::Colours::black;
            onBackground = juce::Colours::white;
            surface = juce::Colour(0xFF000000);
            onSurface = juce::Colours::white;
            primary = juce::Colours::white;
            onPrimary = juce::Colours::black;
            break;
            
        case AccessibilityMode::Protanopia:
            // Red-blind friendly colors
            error = juce::Colour(0xFF795548);    // Brown instead of red
            meterHigh = juce::Colour(0xFF795548);
            accent = juce::Colour(0xFF2196F3);   // Blue accent
            break;
            
        case AccessibilityMode::Deuteranopia:
            // Green-blind friendly colors
            success = juce::Colour(0xFF2196F3);  // Blue instead of green
            meterLow = juce::Colour(0xFF2196F3);
            accent = juce::Colour(0xFFFF5722);   // Orange accent
            break;
            
        case AccessibilityMode::Tritanopia:
            // Blue-blind friendly colors
            primary = juce::Colour(0xFF4CAF50);  // Green instead of blue
            accent = juce::Colour(0xFFFF5722);   // Orange accent
            spectrum = juce::Colour(0xFF4CAF50); // Green spectrum
            break;
            
        case AccessibilityMode::Monochrome:
            // Complete grayscale
            primary = juce::Colours::white;
            secondary = juce::Colour(0xFF808080);
            accent = juce::Colours::white;
            error = juce::Colour(0xFFB0B0B0);
            warning = juce::Colour(0xFF909090);
            success = juce::Colour(0xFFC0C0C0);
            meterLow = juce::Colour(0xFF404040);
            meterMid = juce::Colour(0xFF808080);
            meterHigh = juce::Colour(0xFFC0C0C0);
            waveform = juce::Colour(0xFF808080);
            spectrum = juce::Colour(0xFF606060);
            break;
            
        case AccessibilityMode::None:
        default:
            // No changes needed
            break;
    }
}

//==============================================================================
// ThemeManager Implementation
//==============================================================================

ThemeManager::ThemeManager(ColorScheme& colorScheme)
    : colorScheme(colorScheme)
    , currentThemeName("Dark")
    , seasonalTimer(std::make_unique<juce::Timer>())
{
    // Initialize built-in themes
    initializeBuiltInThemes();
    
    // Load saved state
    ComponentState state;
    if (INIDataManager::loadComponentState("ThemeManager", state)) {
        loadState(state);
    }
    
    // Setup seasonal timer (check every hour)
    seasonalTimer->startTimer(3600000); // 1 hour in milliseconds
}

ThemeManager::~ThemeManager() {
    // Save current state
    ComponentState state;
    saveState(state);
    INIDataManager::saveComponentState("ThemeManager", state);
    
    seasonalTimer->stopTimer();
}

//==============================================================================
// Theme Management
//==============================================================================

bool ThemeManager::createTheme(const juce::String& name, const ThemeColors& colors, const ThemeMetadata& metadata) {
    if (!isValidThemeName(name) || themeColors.find(name) != themeColors.end()) {
        return false;
    }
    
    // Validate theme colors
    if (!validateThemeColors(colors)) {
        auto errors = validateThemeColorsInternal(colors);
        notifyValidationFailed(name, errors);
        return false;
    }
    
    // Create metadata
    ThemeMetadata meta = metadata;
    meta.name = name;
    meta.createdDate = juce::Time::getCurrentTime();
    meta.modifiedDate = meta.createdDate;
    
    // Store theme
    themeColors[name] = colors;
    themeMetadata[name] = meta;
    
    // Save to file
    return saveThemeToFile(name, colors, meta);
}

bool ThemeManager::deleteTheme(const juce::String& name) {
    // Don't allow deletion of built-in themes
    auto it = themeMetadata.find(name);
    if (it != themeMetadata.end() && it->second.type == ThemeType::System) {
        return false;
    }
    
    // Remove from memory
    themeColors.erase(name);
    themeMetadata.erase(name);
    themeLoadTimes.erase(name);
    
    // Delete file
    auto themeFile = getThemeFile(name);
    if (themeFile.exists()) {
        return themeFile.deleteFile();
    }
    
    return true;
}

bool ThemeManager::duplicateTheme(const juce::String& sourceName, const juce::String& newName) {
    auto sourceIt = themeColors.find(sourceName);
    if (sourceIt == themeColors.end() || !isValidThemeName(newName)) {
        return false;
    }
    
    // Create new metadata
    ThemeMetadata newMetadata = themeMetadata[sourceName];
    newMetadata.name = newName;
    newMetadata.type = ThemeType::User;
    newMetadata.createdDate = juce::Time::getCurrentTime();
    newMetadata.modifiedDate = newMetadata.createdDate;
    newMetadata.author = "User";
    
    return createTheme(newName, sourceIt->second, newMetadata);
}

bool ThemeManager::renameTheme(const juce::String& oldName, const juce::String& newName) {
    if (!isValidThemeName(newName) || themeColors.find(oldName) == themeColors.end()) {
        return false;
    }
    
    // Don't allow renaming built-in themes
    auto it = themeMetadata.find(oldName);
    if (it != themeMetadata.end() && it->second.type == ThemeType::System) {
        return false;
    }
    
    // Duplicate and delete old
    if (duplicateTheme(oldName, newName)) {
        return deleteTheme(oldName);
    }
    
    return false;
}

//==============================================================================
// Theme Application
//==============================================================================

void ThemeManager::setCurrentTheme(const juce::String& name) {
    auto it = themeColors.find(name);
    if (it == themeColors.end()) {
        // Try to load from file
        if (!loadThemeFromFile(name)) {
            return;
        }
        it = themeColors.find(name);
        if (it == themeColors.end()) {
            return;
        }
    }
    
    currentThemeName = name;
    currentThemeColors = it->second;
    
    // Apply accessibility mode if set
    if (currentAccessibilityMode != AccessibilityMode::None) {
        currentThemeColors.applyAccessibilityMode(currentAccessibilityMode);
    }
    
    // Update ColorScheme integration
    updateColorScheme(currentThemeColors);
    
    // Update load time
    themeLoadTimes[name] = juce::Time::getCurrentTime();
    
    notifyThemeChanged();
}

ThemeManager::ThemeMetadata ThemeManager::getCurrentThemeMetadata() const {
    auto it = themeMetadata.find(currentThemeName);
    return it != themeMetadata.end() ? it->second : ThemeMetadata();
}

ThemeManager::ThemeColors ThemeManager::getCurrentThemeColors() const {
    return currentThemeColors;
}

//==============================================================================
// Theme Discovery
//==============================================================================

juce::StringArray ThemeManager::getAvailableThemes() const {
    juce::StringArray themes;
    for (const auto& pair : themeMetadata) {
        themes.add(pair.first);
    }
    themes.sort(false);
    return themes;
}

juce::StringArray ThemeManager::getThemesByType(ThemeType type) const {
    juce::StringArray themes;
    for (const auto& pair : themeMetadata) {
        if (pair.second.type == type) {
            themes.add(pair.first);
        }
    }
    themes.sort(false);
    return themes;
}

juce::Array<ThemeManager::ThemeMetadata> ThemeManager::getAllThemeMetadata() const {
    juce::Array<ThemeMetadata> metadata;
    for (const auto& pair : themeMetadata) {
        metadata.add(pair.second);
    }
    return metadata;
}

ThemeManager::ThemeMetadata ThemeManager::getThemeMetadata(const juce::String& name) const {
    auto it = themeMetadata.find(name);
    return it != themeMetadata.end() ? it->second : ThemeMetadata();
}

//==============================================================================
// Theme Import/Export
//==============================================================================

bool ThemeManager::exportTheme(const juce::String& name, const juce::File& destination) {
    auto themeData = exportThemeToString(name);
    if (themeData.isEmpty()) {
        return false;
    }
    
    return destination.replaceWithText(themeData);
}

bool ThemeManager::importTheme(const juce::File& themeFile) {
    if (!themeFile.exists()) {
        return false;
    }
    
    auto themeData = themeFile.loadFileAsString();
    auto fileName = themeFile.getFileNameWithoutExtension();
    
    return importThemeFromString(themeData, fileName);
}

bool ThemeManager::importThemeFromString(const juce::String& themeData, const juce::String& name) {
    try {
        auto json = juce::JSON::parse(themeData);
        if (!json.isObject()) {
            return false;
        }
        
        auto jsonObj = json.getDynamicObject();
        if (!jsonObj) {
            return false;
        }
        
        // Parse metadata
        ThemeMetadata metadata;
        metadata.name = name;
        metadata.type = ThemeType::Community;
        metadata.author = jsonObj->getProperty("author", "Unknown").toString();
        metadata.description = jsonObj->getProperty("description", "").toString();
        metadata.version = jsonObj->getProperty("version", "1.0").toString();
        
        // Parse colors
        ThemeColors colors;
        auto colorsObj = jsonObj->getProperty("colors").getDynamicObject();
        if (colorsObj) {
            colors.primary = juce::Colour::fromString(colorsObj->getProperty("primary", "0xFF2196F3").toString());
            colors.secondary = juce::Colour::fromString(colorsObj->getProperty("secondary", "0xFF424242").toString());
            colors.accent = juce::Colour::fromString(colorsObj->getProperty("accent", "0xFFFF5722").toString());
            colors.background = juce::Colour::fromString(colorsObj->getProperty("background", "0xFF121212").toString());
            colors.surface = juce::Colour::fromString(colorsObj->getProperty("surface", "0xFF1E1E1E").toString());
            // ... parse other colors
        }
        
        return createTheme(name, colors, metadata);
    }
    catch (...) {
        return false;
    }
}

juce::String ThemeManager::exportThemeToString(const juce::String& name) {
    auto colorsIt = themeColors.find(name);
    auto metadataIt = themeMetadata.find(name);
    
    if (colorsIt == themeColors.end() || metadataIt == themeMetadata.end()) {
        return {};
    }
    
    auto json = std::make_unique<juce::DynamicObject>();
    const auto& colors = colorsIt->second;
    const auto& metadata = metadataIt->second;
    
    // Export metadata
    json->setProperty("name", metadata.name);
    json->setProperty("author", metadata.author);
    json->setProperty("description", metadata.description);
    json->setProperty("version", metadata.version);
    json->setProperty("created", metadata.createdDate.toISO8601(true));
    
    // Export colors
    auto colorsObj = std::make_unique<juce::DynamicObject>();
    colorsObj->setProperty("primary", colors.primary.toString());
    colorsObj->setProperty("secondary", colors.secondary.toString());
    colorsObj->setProperty("accent", colors.accent.toString());
    colorsObj->setProperty("background", colors.background.toString());
    colorsObj->setProperty("surface", colors.surface.toString());
    colorsObj->setProperty("error", colors.error.toString());
    colorsObj->setProperty("warning", colors.warning.toString());
    colorsObj->setProperty("success", colors.success.toString());
    colorsObj->setProperty("onPrimary", colors.onPrimary.toString());
    colorsObj->setProperty("onSecondary", colors.onSecondary.toString());
    colorsObj->setProperty("onBackground", colors.onBackground.toString());
    colorsObj->setProperty("onSurface", colors.onSurface.toString());
    colorsObj->setProperty("onError", colors.onError.toString());
    colorsObj->setProperty("hover", colors.hover.toString());
    colorsObj->setProperty("pressed", colors.pressed.toString());
    colorsObj->setProperty("disabled", colors.disabled.toString());
    colorsObj->setProperty("selected", colors.selected.toString());
    colorsObj->setProperty("meterLow", colors.meterLow.toString());
    colorsObj->setProperty("meterMid", colors.meterMid.toString());
    colorsObj->setProperty("meterHigh", colors.meterHigh.toString());
    colorsObj->setProperty("meterPeak", colors.meterPeak.toString());
    colorsObj->setProperty("waveform", colors.waveform.toString());
    colorsObj->setProperty("spectrum", colors.spectrum.toString());
    
    json->setProperty("colors", juce::var(colorsObj.release()));
    
    return juce::JSON::toString(juce::var(json.release()), true);
}

//==============================================================================
// Real-time Preview
//==============================================================================

void ThemeManager::startPreview(const juce::String& themeName) {
    auto it = themeColors.find(themeName);
    if (it == themeColors.end()) {
        return;
    }
    
    if (!previewActive) {
        originalThemeColors = currentThemeColors;
    }
    
    previewActive = true;
    previewThemeName = themeName;
    currentThemeColors = it->second;
    
    // Apply accessibility mode if set
    if (currentAccessibilityMode != AccessibilityMode::None) {
        currentThemeColors.applyAccessibilityMode(currentAccessibilityMode);
    }
    
    updateColorScheme(currentThemeColors);
    notifyPreviewStarted();
}

void ThemeManager::stopPreview() {
    if (!previewActive) {
        return;
    }
    
    previewActive = false;
    previewThemeName.clear();
    currentThemeColors = originalThemeColors;
    
    updateColorScheme(currentThemeColors);
    notifyPreviewStopped();
}

//==============================================================================
// Accessibility
//==============================================================================

void ThemeManager::setAccessibilityMode(AccessibilityMode mode) {
    if (currentAccessibilityMode == mode) {
        return;
    }
    
    currentAccessibilityMode = mode;
    
    // Reapply current theme with new accessibility mode
    auto tempColors = themeColors[currentThemeName];
    if (mode != AccessibilityMode::None) {
        tempColors.applyAccessibilityMode(mode);
    }
    
    currentThemeColors = tempColors;
    updateColorScheme(currentThemeColors);
    
    notifyAccessibilityModeChanged();
}

juce::StringArray ThemeManager::getAccessibilityModeNames() const {
    return {
        "None",
        "High Contrast",
        "Protanopia (Red-blind)",
        "Deuteranopia (Green-blind)",
        "Tritanopia (Blue-blind)",
        "Monochrome"
    };
}

//==============================================================================
// Seasonal Themes
//==============================================================================

void ThemeManager::setSeasonalSettings(const SeasonalSettings& settings) {
    seasonalSettings = settings;
    
    if (settings.enabled) {
        updateSeasonalTheme();
    }
}

void ThemeManager::updateSeasonalTheme() {
    if (!seasonalSettings.enabled) {
        return;
    }
    
    auto currentSeason = getCurrentSeason();
    juce::String seasonalTheme;
    
    switch (currentSeason) {
        case 0: seasonalTheme = seasonalSettings.springTheme; break;
        case 1: seasonalTheme = seasonalSettings.summerTheme; break;
        case 2: seasonalTheme = seasonalSettings.autumnTheme; break;
        case 3: seasonalTheme = seasonalSettings.winterTheme; break;
        default: return;
    }
    
    if (seasonalTheme != currentThemeName && themeColors.find(seasonalTheme) != themeColors.end()) {
        setCurrentTheme(seasonalTheme);
        notifySeasonalThemeUpdated();
    }
}

juce::String ThemeManager::getCurrentSeasonalTheme() const {
    if (!seasonalSettings.enabled) {
        return {};
    }
    
    auto currentSeason = getCurrentSeason();
    switch (currentSeason) {
        case 0: return seasonalSettings.springTheme;
        case 1: return seasonalSettings.summerTheme;
        case 2: return seasonalSettings.autumnTheme;
        case 3: return seasonalSettings.winterTheme;
        default: return {};
    }
}

//==============================================================================
// Theme Validation
//==============================================================================

bool ThemeManager::validateTheme(const juce::String& name) const {
    auto it = themeColors.find(name);
    if (it == themeColors.end()) {
        return false;
    }
    
    return validateThemeColors(it->second);
}

bool ThemeManager::validateThemeColors(const ThemeColors& colors) const {
    auto errors = validateThemeColorsInternal(colors);
    return errors.isEmpty();
}

juce::StringArray ThemeManager::getThemeValidationErrors(const juce::String& name) const {
    auto it = themeColors.find(name);
    if (it == themeColors.end()) {
        return {"Theme not found"};
    }
    
    return validateThemeColorsInternal(it->second);
}

//==============================================================================
// State Management
//==============================================================================

void ThemeManager::saveState(ComponentState& state) const {
    state.setValue("currentTheme", currentThemeName);
    state.setValue("accessibilityMode", static_cast<int>(currentAccessibilityMode));
    state.setValue("seasonalEnabled", seasonalSettings.enabled);
    state.setValue("springTheme", seasonalSettings.springTheme);
    state.setValue("summerTheme", seasonalSettings.summerTheme);
    state.setValue("autumnTheme", seasonalSettings.autumnTheme);
    state.setValue("winterTheme", seasonalSettings.winterTheme);
}

void ThemeManager::loadState(const ComponentState& state) {
    currentThemeName = state.getValue("currentTheme", "Dark");
    currentAccessibilityMode = static_cast<AccessibilityMode>(
        state.getValue("accessibilityMode", static_cast<int>(AccessibilityMode::None))
    );
    
    seasonalSettings.enabled = state.getValue("seasonalEnabled", false);
    seasonalSettings.springTheme = state.getValue("springTheme", "Light");
    seasonalSettings.summerTheme = state.getValue("summerTheme", "Light");
    seasonalSettings.autumnTheme = state.getValue("autumnTheme", "Classic");
    seasonalSettings.winterTheme = state.getValue("winterTheme", "Dark");
    
    // Apply loaded theme
    if (themeColors.find(currentThemeName) != themeColors.end()) {
        setCurrentTheme(currentThemeName);
    }
}

//==============================================================================
// Integration with existing ColorScheme
//==============================================================================

void ThemeManager::syncWithColorScheme() {
    // This method would sync with the existing ColorScheme system
    // Implementation depends on ColorScheme's API
}

void ThemeManager::updateColorScheme(const ThemeColors& colors) {
    // Update the existing ColorScheme with new theme colors
    // This would call appropriate ColorScheme methods to update colors
    sendChangeMessage();
}

//==============================================================================
// Theme Editor Support
//==============================================================================

void ThemeManager::beginThemeEdit(const juce::String& name) {
    auto it = themeColors.find(name);
    if (it == themeColors.end()) {
        return;
    }
    
    editingTheme = true;
    editingThemeName = name;
    editingThemeColors = it->second;
    originalEditingColors = it->second;
}

void ThemeManager::endThemeEdit(bool saveChanges) {
    if (!editingTheme) {
        return;
    }
    
    if (saveChanges) {
        themeColors[editingThemeName] = editingThemeColors;
        
        // Update metadata
        auto& metadata = themeMetadata[editingThemeName];
        metadata.modifiedDate = juce::Time::getCurrentTime();
        
        // Save to file
        saveThemeToFile(editingThemeName, editingThemeColors, metadata);
        
        // Update current theme if it's the one being edited
        if (editingThemeName == currentThemeName) {
            setCurrentTheme(editingThemeName);
        }
    } else {
        editingThemeColors = originalEditingColors;
    }
    
    editingTheme = false;
    editingThemeName.clear();
}

void ThemeManager::setEditingThemeColors(const ThemeColors& colors) {
    if (editingTheme) {
        editingThemeColors = colors;
    }
}

ThemeManager::ThemeColors ThemeManager::getEditingThemeColors() const {
    return editingTheme ? editingThemeColors : ThemeColors();
}

//==============================================================================
// Performance
//==============================================================================

void ThemeManager::preloadThemes() {
    auto themesDir = getThemesDirectory();
    if (!themesDir.exists()) {
        return;
    }
    
    auto themeFiles = themesDir.findChildFiles(juce::File::findFiles, false, "*.theme");
    for (const auto& file : themeFiles) {
        auto themeName = file.getFileNameWithoutExtension();
        if (themeColors.find(themeName) == themeColors.end()) {
            loadThemeFromFile(themeName);
        }
    }
}

void ThemeManager::clearThemeCache() {
    // Keep built-in themes, clear user themes
    auto it = themeColors.begin();
    while (it != themeColors.end()) {
        auto metaIt = themeMetadata.find(it->first);
        if (metaIt != themeMetadata.end() && metaIt->second.type != ThemeType::System) {
            it = themeColors.erase(it);
            themeMetadata.erase(metaIt);
        } else {
            ++it;
        }
    }
    
    themeLoadTimes.clear();
}

int ThemeManager::getCachedThemeCount() const {
    return static_cast<int>(themeColors.size());
}

//==============================================================================
// Listener Management
//==============================================================================

void ThemeManager::addListener(Listener* listener) {
    listeners.add(listener);
}

void ThemeManager::removeListener(Listener* listener) {
    listeners.remove(listener);
}

//==============================================================================
// Private Implementation
//==============================================================================

juce::File ThemeManager::getThemesDirectory() const {
    auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    return appDataDir.getChildFile(INIConfig::OTTO_DATA_FOLDER)
                    .getChildFile(INIConfig::SETTINGS_FOLDER)
                    .getChildFile("Themes");
}

juce::File ThemeManager::getThemeFile(const juce::String& name) const {
    return getThemesDirectory().getChildFile(name + ".theme");
}

bool ThemeManager::saveThemeToFile(const juce::String& name, const ThemeColors& colors, const ThemeMetadata& metadata) {
    auto themeFile = getThemeFile(name);
    themeFile.getParentDirectory().createDirectory();
    
    auto themeData = exportThemeToString(name);
    return themeFile.replaceWithText(themeData);
}

bool ThemeManager::loadThemeFromFile(const juce::String& name) {
    auto themeFile = getThemeFile(name);
    if (!themeFile.exists()) {
        return false;
    }
    
    auto themeData = themeFile.loadFileAsString();
    return importThemeFromString(themeData, name);
}

void ThemeManager::initializeBuiltInThemes() {
    // Dark Theme
    {
        ThemeMetadata metadata("Dark", ThemeType::System);
        metadata.author = "OTTO Team";
        metadata.description = "Professional dark theme optimized for studio environments";
        
        auto colors = createDarkThemeColors();
        themeColors["Dark"] = colors;
        themeMetadata["Dark"] = metadata;
    }
    
    // Light Theme
    {
        ThemeMetadata metadata("Light", ThemeType::System);
        metadata.author = "OTTO Team";
        metadata.description = "Clean light theme for bright environments";
        
        auto colors = createLightThemeColors();
        themeColors["Light"] = colors;
        themeMetadata["Light"] = metadata;
    }
    
    // Classic Theme
    {
        ThemeMetadata metadata("Classic", ThemeType::System);
        metadata.author = "OTTO Team";
        metadata.description = "Classic OTTO theme with vintage aesthetics";
        
        auto colors = createClassicThemeColors();
        themeColors["Classic"] = colors;
        themeMetadata["Classic"] = metadata;
    }
    
    // High Contrast Theme
    {
        ThemeMetadata metadata("High Contrast", ThemeType::Accessibility);
        metadata.author = "OTTO Team";
        metadata.description = "High contrast theme for accessibility";
        metadata.accessibilityMode = AccessibilityMode::HighContrast;
        
        auto colors = createHighContrastThemeColors();
        themeColors["High Contrast"] = colors;
        themeMetadata["High Contrast"] = metadata;
    }
}

ThemeManager::ThemeColors ThemeManager::createDarkThemeColors() {
    ThemeColors colors;
    colors.setDefaults(); // Already dark theme defaults
    return colors;
}

ThemeManager::ThemeColors ThemeManager::createLightThemeColors() {
    ThemeColors colors;
    colors.primary = juce::Colour(0xFF1976D2);      // Darker Blue
    colors.secondary = juce::Colour(0xFF757575);    // Medium Grey
    colors.accent = juce::Colour(0xFFD32F2F);       // Red
    colors.background = juce::Colour(0xFFFAFAFA);   // Light Background
    colors.surface = juce::Colours::white;          // White Surface
    colors.error = juce::Colour(0xFFD32F2F);        // Red
    colors.warning = juce::Colour(0xFFF57C00);      // Orange
    colors.success = juce::Colour(0xFF388E3C);      // Green
    
    // Text colors
    colors.onPrimary = juce::Colours::white;
    colors.onSecondary = juce::Colours::white;
    colors.onBackground = juce::Colour(0xFF212121);
    colors.onSurface = juce::Colour(0xFF212121);
    colors.onError = juce::Colours::white;
    
    // State colors
    colors.hover = colors.primary.withAlpha(0.1f);
    colors.pressed = colors.primary.withAlpha(0.2f);
    colors.disabled = juce::Colour(0xFF9E9E9E);
    colors.selected = colors.primary.withAlpha(0.3f);
    
    return colors;
}

ThemeManager::ThemeColors ThemeManager::createClassicThemeColors() {
    ThemeColors colors;
    colors.primary = juce::Colour(0xFF607D8B);      // Blue Grey
    colors.secondary = juce::Colour(0xFF455A64);    // Dark Blue Grey
    colors.accent = juce::Colour(0xFFFF7043);       // Deep Orange
    colors.background = juce::Colour(0xFF263238);   // Dark Blue Grey
    colors.surface = juce::Colour(0xFF37474F);      // Blue Grey
    colors.error = juce::Colour(0xFFE57373);        // Light Red
    colors.warning = juce::Colour(0xFFFFB74D);      // Light Orange
    colors.success = juce::Colour(0xFF81C784);      // Light Green
    
    // Text colors
    colors.onPrimary = juce::Colours::white;
    colors.onSecondary = juce::Colours::white;
    colors.onBackground = juce::Colour(0xFFECEFF1);
    colors.onSurface = juce::Colour(0xFFECEFF1);
    colors.onError = juce::Colours::white;
    
    return colors;
}

ThemeManager::ThemeColors ThemeManager::createHighContrastThemeColors() {
    ThemeColors colors;
    colors.primary = juce::Colours::white;
    colors.secondary = juce::Colour(0xFF808080);
    colors.accent = juce::Colours::yellow;
    colors.background = juce::Colours::black;
    colors.surface = juce::Colour(0xFF000000);
    colors.error = juce::Colours::red;
    colors.warning = juce::Colours::yellow;
    colors.success = juce::Colours::lime;
    
    // Text colors
    colors.onPrimary = juce::Colours::black;
    colors.onSecondary = juce::Colours::white;
    colors.onBackground = juce::Colours::white;
    colors.onSurface = juce::Colours::white;
    colors.onError = juce::Colours::white;
    
    // State colors
    colors.hover = juce::Colours::yellow.withAlpha(0.3f);
    colors.pressed = juce::Colours::yellow.withAlpha(0.5f);
    colors.disabled = juce::Colour(0xFF404040);
    colors.selected = juce::Colours::yellow.withAlpha(0.4f);
    
    return colors;
}

juce::Colour ThemeManager::adjustColorForAccessibility(const juce::Colour& color, AccessibilityMode mode) {
    switch (mode) {
        case AccessibilityMode::HighContrast:
            return color.getPerceivedBrightness() > 0.5f ? juce::Colours::white : juce::Colours::black;
            
        case AccessibilityMode::Protanopia:
            // Simulate protanopia (red-blind)
            return juce::Colour::fromFloatRGBA(
                color.getFloatGreen() * 0.567f + color.getFloatBlue() * 0.433f,
                color.getFloatGreen(),
                color.getFloatBlue(),
                color.getFloatAlpha()
            );
            
        case AccessibilityMode::Deuteranopia:
            // Simulate deuteranopia (green-blind)
            return juce::Colour::fromFloatRGBA(
                color.getFloatRed(),
                color.getFloatRed() * 0.625f + color.getFloatBlue() * 0.375f,
                color.getFloatBlue(),
                color.getFloatAlpha()
            );
            
        case AccessibilityMode::Tritanopia:
            // Simulate tritanopia (blue-blind)
            return juce::Colour::fromFloatRGBA(
                color.getFloatRed(),
                color.getFloatGreen(),
                color.getFloatRed() * 0.95f + color.getFloatGreen() * 0.05f,
                color.getFloatAlpha()
            );
            
        case AccessibilityMode::Monochrome:
            {
                auto brightness = color.getPerceivedBrightness();
                return juce::Colour::fromFloatRGBA(brightness, brightness, brightness, color.getFloatAlpha());
            }
            
        case AccessibilityMode::None:
        default:
            return color;
    }
}

float ThemeManager::calculateContrastRatio(const juce::Colour& foreground, const juce::Colour& background) {
    auto fgLuminance = foreground.getPerceivedBrightness();
    auto bgLuminance = background.getPerceivedBrightness();
    
    auto lighter = juce::jmax(fgLuminance, bgLuminance);
    auto darker = juce::jmin(fgLuminance, bgLuminance);
    
    return (lighter + 0.05f) / (darker + 0.05f);
}

bool ThemeManager::meetsAccessibilityStandards(const juce::Colour& foreground, const juce::Colour& background) {
    auto contrast = calculateContrastRatio(foreground, background);
    return contrast >= 4.5f; // WCAG AA standard
}

int ThemeManager::getCurrentSeason() const {
    auto now = juce::Time::getCurrentTime();
    auto month = now.getMonth();
    
    // Northern hemisphere seasons
    if (month >= 2 && month <= 4) return 0; // Spring (Mar-May)
    if (month >= 5 && month <= 7) return 1; // Summer (Jun-Aug)
    if (month >= 8 && month <= 10) return 2; // Autumn (Sep-Nov)
    return 3; // Winter (Dec-Feb)
}

juce::String ThemeManager::getSeasonName(int season) const {
    switch (season) {
        case 0: return "Spring";
        case 1: return "Summer";
        case 2: return "Autumn";
        case 3: return "Winter";
        default: return "Unknown";
    }
}

bool ThemeManager::shouldUpdateSeasonalTheme() const {
    if (!seasonalSettings.enabled) {
        return false;
    }
    
    auto currentSeason = getCurrentSeason();
    auto expectedTheme = getCurrentSeasonalTheme();
    
    return expectedTheme != currentThemeName;
}

bool ThemeManager::isValidThemeName(const juce::String& name) const {
    return !name.isEmpty() && 
           name.length() <= 50 && 
           name.containsOnly("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 -_()");
}

bool ThemeManager::isValidColor(const juce::Colour& color) const {
    return color.isOpaque() || color.getAlpha() > 0;
}

juce::StringArray ThemeManager::validateThemeColorsInternal(const ThemeColors& colors) const {
    juce::StringArray errors;
    
    // Check color validity
    if (!isValidColor(colors.primary)) errors.add("Invalid primary color");
    if (!isValidColor(colors.background)) errors.add("Invalid background color");
    if (!isValidColor(colors.onBackground)) errors.add("Invalid text color");
    
    // Check contrast ratios
    if (!meetsAccessibilityStandards(colors.onBackground, colors.background)) {
        errors.add("Insufficient contrast between text and background");
    }
    
    if (!meetsAccessibilityStandards(colors.onPrimary, colors.primary)) {
        errors.add("Insufficient contrast between primary text and primary color");
    }
    
    return errors;
}

void ThemeManager::notifyThemeChanged() {
    listeners.call([this](Listener& l) { l.themeChanged(currentThemeName); });
}

void ThemeManager::notifyPreviewStarted() {
    listeners.call([this](Listener& l) { l.themePreviewStarted(previewThemeName); });
}

void ThemeManager::notifyPreviewStopped() {
    listeners.call([&](Listener& l) { l.themePreviewStopped(); });
}

void ThemeManager::notifyAccessibilityModeChanged() {
    listeners.call([this](Listener& l) { l.accessibilityModeChanged(currentAccessibilityMode); });
}

void ThemeManager::notifySeasonalThemeUpdated() {
    auto seasonalTheme = getCurrentSeasonalTheme();
    listeners.call([&seasonalTheme](Listener& l) { l.seasonalThemeUpdated(seasonalTheme); });
}

void ThemeManager::notifyValidationFailed(const juce::String& themeName, const juce::StringArray& errors) {
    listeners.call([&](Listener& l) { l.themeValidationFailed(themeName, errors); });
}