#include "ThemeManager.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "INIDataManager.h"
#include <algorithm>
#include <cmath>

//==============================================================================
// ThemeColors Implementation
//==============================================================================

void ThemeManager::ThemeColors::setDefaults() {
    // Default dark theme colors
    primary = juce::Colour(0xff2196f3);
    secondary = juce::Colour(0xff03dac6);
    accent = juce::Colour(0xffff5722);
    background = juce::Colour(0xff121212);
    surface = juce::Colour(0xff1e1e1e);
    error = juce::Colour(0xffcf6679);
    warning = juce::Colour(0xffffc107);
    success = juce::Colour(0xff4caf50);
    
    onPrimary = juce::Colours::white;
    onSecondary = juce::Colours::black;
    onBackground = juce::Colours::white;
    onSurface = juce::Colours::white;
    onError = juce::Colours::black;
    
    hover = primary.brighter(0.2f);
    pressed = primary.darker(0.2f);
    disabled = juce::Colour(0xff666666);
    selected = accent.withAlpha(0.3f);
    
    // Audio-specific colors
    meterLow = juce::Colour(0xff4caf50);
    meterMid = juce::Colour(0xffffc107);
    meterHigh = juce::Colour(0xffff5722);
    meterPeak = juce::Colour(0xfff44336);
    waveform = accent;
    spectrum = primary;
    
    // Gradients
    backgroundGradient = juce::ColourGradient(background.darker(0.1f), 0, 0,
                                             background.brighter(0.1f), 0, 100, false);
    accentGradient = juce::ColourGradient(accent.darker(0.2f), 0, 0,
                                         accent.brighter(0.2f), 0, 100, false);
    useGradients = false;
}

void ThemeManager::ThemeColors::applyAccessibilityMode(AccessibilityMode mode) {
    switch (mode) {
        case AccessibilityMode::HighContrast:
            // Increase contrast for all colors
            background = juce::Colours::black;
            surface = juce::Colour(0xff000000);
            onBackground = juce::Colours::white;
            onSurface = juce::Colours::white;
            primary = juce::Colours::white;
            secondary = juce::Colour(0xffffff00); // Yellow
            accent = juce::Colour(0xff00ffff); // Cyan
            break;
            
        case AccessibilityMode::Protanopia:
            // Adjust for red-blindness
            primary = juce::Colour(0xff0066cc); // Blue
            accent = juce::Colour(0xffffaa00); // Orange/Yellow
            error = juce::Colour(0xff666666); // Gray instead of red
            meterHigh = juce::Colour(0xffffaa00);
            meterPeak = juce::Colour(0xff666666);
            break;
            
        case AccessibilityMode::Deuteranopia:
            // Adjust for green-blindness
            success = juce::Colour(0xff0066cc); // Blue instead of green
            meterLow = juce::Colour(0xff0066cc);
            primary = juce::Colour(0xff6600cc); // Purple
            break;
            
        case AccessibilityMode::Tritanopia:
            // Adjust for blue-blindness
            primary = juce::Colour(0xffcc0066); // Magenta
            secondary = juce::Colour(0xff00cc66); // Green
            spectrum = juce::Colour(0xffcc0066);
            break;
            
        case AccessibilityMode::Monochrome:
            // Convert all colors to grayscale
            primary = juce::Colour(0xffcccccc);
            secondary = juce::Colour(0xff999999);
            accent = juce::Colour(0xffffffff);
            error = juce::Colour(0xff666666);
            warning = juce::Colour(0xffaaaaaa);
            success = juce::Colour(0xff888888);
            meterLow = juce::Colour(0xff888888);
            meterMid = juce::Colour(0xffaaaaaa);
            meterHigh = juce::Colour(0xffcccccc);
            meterPeak = juce::Colour(0xffffffff);
            waveform = juce::Colour(0xffcccccc);
            spectrum = juce::Colour(0xff999999);
            break;
            
        default:
            break;
    }
}

//==============================================================================
// Seasonal Timer Class
//==============================================================================

class SeasonalUpdateTimer : public juce::Timer {
public:
    SeasonalUpdateTimer(ThemeManager& manager) : themeManager(manager) {}
    
    void timerCallback() override {
        themeManager.updateSeasonalTheme();
    }
    
private:
    ThemeManager& themeManager;
};

//==============================================================================
// ThemeManager Implementation
//==============================================================================

ThemeManager::ThemeManager(ColorScheme& colorScheme)
    : colorScheme(colorScheme)
{
    // Initialize built-in themes
    initializeBuiltInThemes();
    
    // Initialize seasonal timer BEFORE loading state
    seasonalTimer = std::make_unique<SeasonalUpdateTimer>(*this);
    
    // Load state from INI
    ComponentState state;
    if (INIDataManager::loadComponentState("ThemeManager", state)) {
        loadState(state);
    } else {
        // Set default theme
        setCurrentTheme("Dark");
    }
    
    // Sync with existing color scheme
    syncWithColorScheme();
}

ThemeManager::~ThemeManager() {
    // Save current state
    ComponentState state;
    saveState(state);
    INIDataManager::saveComponentState("ThemeManager", state);
    
    // Stop seasonal timer
    if (seasonalTimer) {
        seasonalTimer->stopTimer();
    }
}

//==============================================================================
// Theme Management
//==============================================================================

bool ThemeManager::createTheme(const juce::String& name, const ThemeColors& colors, const ThemeMetadata& metadata) {
    if (!isValidThemeName(name) || !validateThemeColors(colors)) {
        return false;
    }
    
    // Create metadata
    ThemeMetadata meta = metadata;
    meta.name = name;
    meta.createdDate = juce::Time::getCurrentTime();
    meta.modifiedDate = meta.createdDate;
    
    // Store theme
    themeMetadata[name] = meta;
    themeColors[name] = colors;
    
    // Save to file
    if (!saveThemeToFile(name, colors, meta)) {
        // Remove from memory if file save failed
        themeMetadata.erase(name);
        themeColors.erase(name);
        return false;
    }
    
    return true;
}

bool ThemeManager::deleteTheme(const juce::String& name) {
    // Don't allow deletion of built-in themes
    auto it = themeMetadata.find(name);
    if (it != themeMetadata.end() && it->second.type == ThemeType::System) {
        return false;
    }
    
    // Remove from memory
    themeMetadata.erase(name);
    themeColors.erase(name);
    themeLoadTimes.erase(name);
    
    // Remove file
    auto themeFile = getThemeFile(name);
    if (themeFile.exists()) {
        themeFile.deleteFile();
    }
    
    // Switch to default theme if current theme was deleted
    if (currentThemeName == name) {
        setCurrentTheme("Dark");
    }
    
    return true;
}

bool ThemeManager::duplicateTheme(const juce::String& sourceName, const juce::String& newName) {
    if (!isValidThemeName(newName)) {
        return false;
    }
    
    // Check if source theme exists
    auto sourceIt = themeColors.find(sourceName);
    if (sourceIt == themeColors.end()) {
        return false;
    }
    
    // Get source metadata
    auto metaIt = themeMetadata.find(sourceName);
    ThemeMetadata newMetadata;
    if (metaIt != themeMetadata.end()) {
        newMetadata = metaIt->second;
    }
    
    // Update metadata for new theme
    newMetadata.name = newName;
    newMetadata.type = ThemeType::User;
    newMetadata.author = "User";
    newMetadata.createdDate = juce::Time::getCurrentTime();
    newMetadata.modifiedDate = newMetadata.createdDate;
    
    return createTheme(newName, sourceIt->second, newMetadata);
}

bool ThemeManager::renameTheme(const juce::String& oldName, const juce::String& newName) {
    if (!isValidThemeName(newName) || oldName == newName) {
        return false;
    }
    
    // Don't allow renaming built-in themes
    auto it = themeMetadata.find(oldName);
    if (it != themeMetadata.end() && it->second.type == ThemeType::System) {
        return false;
    }
    
    // Check if old theme exists
    auto colorsIt = themeColors.find(oldName);
    if (colorsIt == themeColors.end()) {
        return false;
    }
    
    // Duplicate theme with new name
    if (!duplicateTheme(oldName, newName)) {
        return false;
    }
    
    // Update current theme name if necessary
    if (currentThemeName == oldName) {
        currentThemeName = newName;
    }
    
    // Delete old theme
    return deleteTheme(oldName);
}

//==============================================================================
// Theme Application
//==============================================================================

void ThemeManager::setCurrentTheme(const juce::String& name) {
    // Load theme if not in memory
    if (themeColors.find(name) == themeColors.end()) {
        if (!loadThemeFromFile(name)) {
            return;
        }
    }
    
    auto it = themeColors.find(name);
    if (it == themeColors.end()) {
        return;
    }
    
    // Stop any active preview
    if (previewActive) {
        stopPreview();
    }
    
    // Apply theme
    currentThemeName = name;
    currentThemeColors = it->second;
    
    // Apply accessibility mode if set
    if (currentAccessibilityMode != AccessibilityMode::None) {
        currentThemeColors.applyAccessibilityMode(currentAccessibilityMode);
    }
    
    // Update color scheme
    updateColorScheme(currentThemeColors);
    
    // Notify listeners
    notifyThemeChanged();
    
    // Broadcast change to all components
    sendChangeMessage();
}

ThemeManager::ThemeMetadata ThemeManager::getCurrentThemeMetadata() const {
    auto it = themeMetadata.find(currentThemeName);
    if (it != themeMetadata.end()) {
        return it->second;
    }
    
    return ThemeMetadata();
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
    if (it != themeMetadata.end()) {
        return it->second;
    }
    
    return ThemeMetadata();
}

//==============================================================================
// Theme Import/Export
//==============================================================================

bool ThemeManager::exportTheme(const juce::String& name, const juce::File& destination) {
    auto colorsIt = themeColors.find(name);
    auto metaIt = themeMetadata.find(name);
    
    if (colorsIt == themeColors.end() || metaIt == themeMetadata.end()) {
        return false;
    }
    
    // Create JSON representation
    juce::DynamicObject::Ptr themeObject = new juce::DynamicObject();
    
    // Add metadata
    themeObject->setProperty("name", name);
    themeObject->setProperty("author", metaIt->second.author);
    themeObject->setProperty("description", metaIt->second.description);
    themeObject->setProperty("version", metaIt->second.version);
    themeObject->setProperty("type", static_cast<int>(metaIt->second.type));
    themeObject->setProperty("accessibilityMode", static_cast<int>(metaIt->second.accessibilityMode));
    
    // Add colors
    juce::DynamicObject::Ptr colorsObject = new juce::DynamicObject();
    const auto& colors = colorsIt->second;
    
    colorsObject->setProperty("primary", colors.primary.toString());
    colorsObject->setProperty("secondary", colors.secondary.toString());
    colorsObject->setProperty("accent", colors.accent.toString());
    colorsObject->setProperty("background", colors.background.toString());
    colorsObject->setProperty("surface", colors.surface.toString());
    colorsObject->setProperty("error", colors.error.toString());
    colorsObject->setProperty("warning", colors.warning.toString());
    colorsObject->setProperty("success", colors.success.toString());
    colorsObject->setProperty("onPrimary", colors.onPrimary.toString());
    colorsObject->setProperty("onSecondary", colors.onSecondary.toString());
    colorsObject->setProperty("onBackground", colors.onBackground.toString());
    colorsObject->setProperty("onSurface", colors.onSurface.toString());
    colorsObject->setProperty("onError", colors.onError.toString());
    colorsObject->setProperty("hover", colors.hover.toString());
    colorsObject->setProperty("pressed", colors.pressed.toString());
    colorsObject->setProperty("disabled", colors.disabled.toString());
    colorsObject->setProperty("selected", colors.selected.toString());
    colorsObject->setProperty("meterLow", colors.meterLow.toString());
    colorsObject->setProperty("meterMid", colors.meterMid.toString());
    colorsObject->setProperty("meterHigh", colors.meterHigh.toString());
    colorsObject->setProperty("meterPeak", colors.meterPeak.toString());
    colorsObject->setProperty("waveform", colors.waveform.toString());
    colorsObject->setProperty("spectrum", colors.spectrum.toString());
    colorsObject->setProperty("useGradients", colors.useGradients);
    
    themeObject->setProperty("colors", colorsObject.get());
    
    // Convert to JSON string
    juce::var themeVar(themeObject.get());
    juce::String jsonString = juce::JSON::toString(themeVar);
    
    // Write to file
    return destination.replaceWithText(jsonString);
}

bool ThemeManager::importTheme(const juce::File& themeFile) {
    if (!themeFile.exists()) {
        return false;
    }
    
    juce::String themeData = themeFile.loadFileAsString();
    juce::String themeName = themeFile.getFileNameWithoutExtension();
    
    return importThemeFromString(themeData, themeName);
}

bool ThemeManager::importThemeFromString(const juce::String& themeData, const juce::String& name) {
    // Parse JSON
    juce::var parsedJson = juce::JSON::parse(themeData);
    if (!parsedJson.isObject()) {
        return false;
    }
    
    juce::DynamicObject* themeObject = parsedJson.getDynamicObject();
    if (!themeObject) {
        return false;
    }
    
    // Extract metadata
    ThemeMetadata metadata;
    metadata.name = name;
    metadata.author = themeObject->getProperty("author").toString();
    metadata.description = themeObject->getProperty("description").toString();
    metadata.version = themeObject->getProperty("version").toString();
    metadata.type = static_cast<ThemeType>(static_cast<int>(themeObject->getProperty("type")));
    metadata.accessibilityMode = static_cast<AccessibilityMode>(static_cast<int>(themeObject->getProperty("accessibilityMode")));
    metadata.createdDate = juce::Time::getCurrentTime();
    metadata.modifiedDate = metadata.createdDate;
    
    // Extract colors
    juce::var colorsVar = themeObject->getProperty("colors");
    if (!colorsVar.isObject()) {
        return false;
    }
    
    juce::DynamicObject* colorsObject = colorsVar.getDynamicObject();
    if (!colorsObject) {
        return false;
    }
    
    ThemeColors colors;
    colors.primary = juce::Colour::fromString(colorsObject->getProperty("primary").toString());
    colors.secondary = juce::Colour::fromString(colorsObject->getProperty("secondary").toString());
    colors.accent = juce::Colour::fromString(colorsObject->getProperty("accent").toString());
    colors.background = juce::Colour::fromString(colorsObject->getProperty("background").toString());
    colors.surface = juce::Colour::fromString(colorsObject->getProperty("surface").toString());
    colors.error = juce::Colour::fromString(colorsObject->getProperty("error").toString());
    colors.warning = juce::Colour::fromString(colorsObject->getProperty("warning").toString());
    colors.success = juce::Colour::fromString(colorsObject->getProperty("success").toString());
    colors.onPrimary = juce::Colour::fromString(colorsObject->getProperty("onPrimary").toString());
    colors.onSecondary = juce::Colour::fromString(colorsObject->getProperty("onSecondary").toString());
    colors.onBackground = juce::Colour::fromString(colorsObject->getProperty("onBackground").toString());
    colors.onSurface = juce::Colour::fromString(colorsObject->getProperty("onSurface").toString());
    colors.onError = juce::Colour::fromString(colorsObject->getProperty("onError").toString());
    colors.hover = juce::Colour::fromString(colorsObject->getProperty("hover").toString());
    colors.pressed = juce::Colour::fromString(colorsObject->getProperty("pressed").toString());
    colors.disabled = juce::Colour::fromString(colorsObject->getProperty("disabled").toString());
    colors.selected = juce::Colour::fromString(colorsObject->getProperty("selected").toString());
    colors.meterLow = juce::Colour::fromString(colorsObject->getProperty("meterLow").toString());
    colors.meterMid = juce::Colour::fromString(colorsObject->getProperty("meterMid").toString());
    colors.meterHigh = juce::Colour::fromString(colorsObject->getProperty("meterHigh").toString());
    colors.meterPeak = juce::Colour::fromString(colorsObject->getProperty("meterPeak").toString());
    colors.waveform = juce::Colour::fromString(colorsObject->getProperty("waveform").toString());
    colors.spectrum = juce::Colour::fromString(colorsObject->getProperty("spectrum").toString());
    colors.useGradients = colorsObject->getProperty("useGradients");
    
    return createTheme(name, colors, metadata);
}

juce::String ThemeManager::exportThemeToString(const juce::String& name) {
    // Create temporary file for export
    juce::TemporaryFile tempFile;
    
    if (exportTheme(name, tempFile.getFile())) {
        return tempFile.getFile().loadFileAsString();
    }
    
    return juce::String();
}

//==============================================================================
// Real-time Preview
//==============================================================================

void ThemeManager::startPreview(const juce::String& themeName) {
    // Load theme if not in memory
    if (themeColors.find(themeName) == themeColors.end()) {
        if (!loadThemeFromFile(themeName)) {
            return;
        }
    }
    
    auto it = themeColors.find(themeName);
    if (it == themeColors.end()) {
        return;
    }
    
    // Store original colors if not already previewing
    if (!previewActive) {
        originalThemeColors = currentThemeColors;
    }
    
    // Apply preview theme
    previewActive = true;
    previewThemeName = themeName;
    currentThemeColors = it->second;
    
    // Apply accessibility mode if set
    if (currentAccessibilityMode != AccessibilityMode::None) {
        currentThemeColors.applyAccessibilityMode(currentAccessibilityMode);
    }
    
    // Update color scheme
    updateColorScheme(currentThemeColors);
    
    // Notify listeners
    notifyPreviewStarted();
    
    // Broadcast change
    sendChangeMessage();
}

void ThemeManager::stopPreview() {
    if (!previewActive) {
        return;
    }
    
    // Restore original colors
    previewActive = false;
    previewThemeName = juce::String();
    currentThemeColors = originalThemeColors;
    
    // Update color scheme
    updateColorScheme(currentThemeColors);
    
    // Notify listeners
    notifyPreviewStopped();
    
    // Broadcast change
    sendChangeMessage();
}

//==============================================================================
// Accessibility
//==============================================================================

void ThemeManager::setAccessibilityMode(AccessibilityMode mode) {
    if (mode != currentAccessibilityMode) {
        currentAccessibilityMode = mode;
        
        // Reapply current theme with accessibility adjustments
        auto originalColors = currentThemeColors;
        if (mode != AccessibilityMode::None) {
            currentThemeColors.applyAccessibilityMode(mode);
        } else {
            // Restore original colors
            auto it = themeColors.find(currentThemeName);
            if (it != themeColors.end()) {
                currentThemeColors = it->second;
            }
        }
        
        // Update color scheme
        updateColorScheme(currentThemeColors);
        
        // Notify listeners
        notifyAccessibilityModeChanged();
        
        // Broadcast change
        sendChangeMessage();
    }
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
    
    // Ensure timer is initialized before using it
    if (!seasonalTimer) {
        return;
    }
    
    if (settings.enabled) {
        if (!seasonalTimer->isTimerRunning()) {
            seasonalTimer->startTimer(3600000); // Check every hour
        }
        updateSeasonalTheme();
    } else {
        seasonalTimer->stopTimer();
    }
}

void ThemeManager::updateSeasonalTheme() {
    if (!seasonalSettings.enabled) {
        return;
    }
    
    if (!shouldUpdateSeasonalTheme()) {
        return;
    }
    
    juce::String seasonalTheme = getCurrentSeasonalTheme();
    
    if (!seasonalTheme.isEmpty() && seasonalTheme != currentThemeName) {
        setCurrentTheme(seasonalTheme);
        notifySeasonalThemeUpdated();
    }
}

juce::String ThemeManager::getCurrentSeasonalTheme() const {
    int season = getCurrentSeason();
    
    switch (season) {
        case 0: return seasonalSettings.springTheme;  // Spring (Mar-May)
        case 1: return seasonalSettings.summerTheme;  // Summer (Jun-Aug)
        case 2: return seasonalSettings.autumnTheme;  // Autumn (Sep-Nov)
        case 3: return seasonalSettings.winterTheme;  // Winter (Dec-Feb)
        default: return seasonalSettings.springTheme;
    }
}

//==============================================================================
// Theme Validation
//==============================================================================

bool ThemeManager::validateTheme(const juce::String& name) const {
    auto errors = getThemeValidationErrors(name);
    return errors.isEmpty();
}

bool ThemeManager::validateThemeColors(const ThemeColors& colors) const {
    auto errors = validateThemeColorsInternal(colors);
    return errors.isEmpty();
}

juce::StringArray ThemeManager::getThemeValidationErrors(const juce::String& name) const {
    juce::StringArray errors;
    
    if (!isValidThemeName(name)) {
        errors.add("Invalid theme name");
    }
    
    auto it = themeColors.find(name);
    if (it == themeColors.end()) {
        errors.add("Theme not found");
        return errors;
    }
    
    auto colorErrors = validateThemeColorsInternal(it->second);
    errors.addArray(colorErrors);
    
    return errors;
}

//==============================================================================
// State Management
//==============================================================================

void ThemeManager::saveState(ComponentState& state) const {
    state.setValue("currentTheme", currentThemeName);
    state.setValue("accessibilityMode", static_cast<int>(currentAccessibilityMode));
    
    // Save seasonal settings
    state.setValue("seasonalEnabled", seasonalSettings.enabled);
    state.setValue("springTheme", seasonalSettings.springTheme);
    state.setValue("summerTheme", seasonalSettings.summerTheme);
    state.setValue("autumnTheme", seasonalSettings.autumnTheme);
    state.setValue("winterTheme", seasonalSettings.winterTheme);
    state.setValue("useLocationBasedSeasons", seasonalSettings.useLocationBasedSeasons);
    state.setValue("latitude", seasonalSettings.latitude);
    state.setValue("longitude", seasonalSettings.longitude);
}

void ThemeManager::loadState(const ComponentState& state) {
    // Load current theme
    juce::String themeName = state.getValue("currentTheme", "Dark");
    setCurrentTheme(themeName);
    
    // Load accessibility mode
    AccessibilityMode mode = static_cast<AccessibilityMode>(state.getIntValue("accessibilityMode", static_cast<int>(AccessibilityMode::None)));
    setAccessibilityMode(mode);
    
    // Load seasonal settings
    seasonalSettings.enabled = state.getValue("seasonalEnabled", false);
    seasonalSettings.springTheme = state.getValue("springTheme", "Light");
    seasonalSettings.summerTheme = state.getValue("summerTheme", "Light");
    seasonalSettings.autumnTheme = state.getValue("autumnTheme", "Classic");
    seasonalSettings.winterTheme = state.getValue("winterTheme", "Dark");
    seasonalSettings.useLocationBasedSeasons = state.getValue("useLocationBasedSeasons", false);
    seasonalSettings.latitude = state.getValue("latitude", 0.0f);
    seasonalSettings.longitude = state.getValue("longitude", 0.0f);
    
    // Apply seasonal settings
    setSeasonalSettings(seasonalSettings);
}

//==============================================================================
// Integration with ColorScheme
//==============================================================================

void ThemeManager::syncWithColorScheme() {
    // Get current colors from ColorScheme and create a theme
    ThemeColors colors;
    
    // Map ColorScheme colors to ThemeColors
    colors.primary = colorScheme.getColor(ColorScheme::ColorRole::Accent);
    colors.background = colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground);
    colors.surface = colorScheme.getColor(ColorScheme::ColorRole::WindowBackground);
    colors.onBackground = colorScheme.getColor(ColorScheme::ColorRole::PrimaryText);
    colors.onSurface = colorScheme.getColor(ColorScheme::ColorRole::SecondaryText);
    
    // Update current theme colors
    currentThemeColors = colors;
}

void ThemeManager::updateColorScheme(const ThemeColors& colors) {
    // TODO: ColorScheme integration needs to be redesigned
    // ColorScheme::setColor expects component/property strings, not ColorRole enums
    // For now, store colors in currentThemeColors and notify listeners
    currentThemeColors = colors;
    
    // Notify listeners of theme change
    listeners.call([this](Listener& listener) {
        listener.themeChanged(currentThemeName);
    });
}

//==============================================================================
// Theme Editor Support
//==============================================================================

void ThemeManager::beginThemeEdit(const juce::String& name) {
    if (editingTheme) {
        endThemeEdit(false); // Cancel previous edit
    }
    
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
        // Save changes
        themeColors[editingThemeName] = editingThemeColors;
        
        // Update metadata
        auto metaIt = themeMetadata.find(editingThemeName);
        if (metaIt != themeMetadata.end()) {
            metaIt->second.modifiedDate = juce::Time::getCurrentTime();
        }
        
        // Save to file
        auto metadata = getThemeMetadata(editingThemeName);
        saveThemeToFile(editingThemeName, editingThemeColors, metadata);
        
        // Update current theme if it's the one being edited
        if (currentThemeName == editingThemeName) {
            currentThemeColors = editingThemeColors;
            updateColorScheme(currentThemeColors);
            sendChangeMessage();
        }
    }
    
    editingTheme = false;
    editingThemeName = juce::String();
    editingThemeColors = ThemeColors();
    originalEditingColors = ThemeColors();
}

void ThemeManager::setEditingThemeColors(const ThemeColors& colors) {
    if (editingTheme) {
        editingThemeColors = colors;
    }
}

ThemeManager::ThemeColors ThemeManager::getEditingThemeColors() const {
    return editingThemeColors;
}

//==============================================================================
// Performance
//==============================================================================

void ThemeManager::preloadThemes() {
    auto themesDir = getThemesDirectory();
    if (!themesDir.exists()) {
        return;
    }
    
    auto themeFiles = themesDir.findChildFiles(juce::File::findFiles, false, "*.json");
    
    for (const auto& file : themeFiles) {
        juce::String themeName = file.getFileNameWithoutExtension();
        if (themeColors.find(themeName) == themeColors.end()) {
            loadThemeFromFile(themeName);
        }
    }
}

void ThemeManager::clearThemeCache() {
    // Keep built-in themes and current theme
    auto currentColors = currentThemeColors;
    auto currentMeta = getCurrentThemeMetadata();
    
    // Clear all themes
    themeColors.clear();
    themeMetadata.clear();
    themeLoadTimes.clear();
    
    // Restore built-in themes
    initializeBuiltInThemes();
    
    // Restore current theme
    themeColors[currentThemeName] = currentColors;
    themeMetadata[currentThemeName] = currentMeta;
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
    return INIConfig::getOTTODataDirectory().getChildFile(INIConfig::THEMES_FOLDER);
}

juce::File ThemeManager::getThemeFile(const juce::String& name) const {
    return getThemesDirectory().getChildFile(name + ".json");
}

bool ThemeManager::saveThemeToFile(const juce::String& name, const ThemeColors& colors, const ThemeMetadata& metadata) {
    auto themeFile = getThemeFile(name);
    
    // Ensure directory exists
    themeFile.getParentDirectory().createDirectory();
    
    return exportTheme(name, themeFile);
}

bool ThemeManager::loadThemeFromFile(const juce::String& name) {
    auto themeFile = getThemeFile(name);
    
    if (!themeFile.exists()) {
        return false;
    }
    
    bool success = importTheme(themeFile);
    
    if (success) {
        themeLoadTimes[name] = juce::Time::getCurrentTime();
    }
    
    return success;
}

//==============================================================================
// Built-in Themes
//==============================================================================

void ThemeManager::initializeBuiltInThemes() {
    // Dark theme
    ThemeMetadata darkMeta("Dark", ThemeType::System);
    darkMeta.author = "OTTO";
    darkMeta.description = "Default dark theme";
    darkMeta.version = "1.0";
    
    auto darkColors = createDarkThemeColors();
    themeMetadata["Dark"] = darkMeta;
    themeColors["Dark"] = darkColors;
    
    // Light theme
    ThemeMetadata lightMeta("Light", ThemeType::System);
    lightMeta.author = "OTTO";
    lightMeta.description = "Default light theme";
    lightMeta.version = "1.0";
    
    auto lightColors = createLightThemeColors();
    themeMetadata["Light"] = lightMeta;
    themeColors["Light"] = lightColors;
    
    // Classic theme
    ThemeMetadata classicMeta("Classic", ThemeType::System);
    classicMeta.author = "OTTO";
    classicMeta.description = "Classic OTTO theme";
    classicMeta.version = "1.0";
    
    auto classicColors = createClassicThemeColors();
    themeMetadata["Classic"] = classicMeta;
    themeColors["Classic"] = classicColors;
    
    // High contrast theme
    ThemeMetadata hcMeta("High Contrast", ThemeType::Accessibility);
    hcMeta.author = "OTTO";
    hcMeta.description = "High contrast accessibility theme";
    hcMeta.version = "1.0";
    hcMeta.accessibilityMode = AccessibilityMode::HighContrast;
    
    auto hcColors = createHighContrastThemeColors();
    themeMetadata["High Contrast"] = hcMeta;
    themeColors["High Contrast"] = hcColors;
}

ThemeManager::ThemeColors ThemeManager::createDarkThemeColors() {
    ThemeColors colors;
    colors.setDefaults(); // Already set to dark theme defaults
    return colors;
}

ThemeManager::ThemeColors ThemeManager::createLightThemeColors() {
    ThemeColors colors;
    
    colors.primary = juce::Colour(0xff1976d2);
    colors.secondary = juce::Colour(0xff00acc1);
    colors.accent = juce::Colour(0xfff57c00);
    colors.background = juce::Colour(0xfffafafa);
    colors.surface = juce::Colours::white;
    colors.error = juce::Colour(0xffd32f2f);
    colors.warning = juce::Colour(0xfff57c00);
    colors.success = juce::Colour(0xff388e3c);
    
    colors.onPrimary = juce::Colours::white;
    colors.onSecondary = juce::Colours::white;
    colors.onBackground = juce::Colour(0xff212121);
    colors.onSurface = juce::Colour(0xff212121);
    colors.onError = juce::Colours::white;
    
    colors.hover = colors.primary.brighter(0.1f);
    colors.pressed = colors.primary.darker(0.1f);
    colors.disabled = juce::Colour(0xffbdbdbd);
    colors.selected = colors.accent.withAlpha(0.2f);
    
    colors.meterLow = juce::Colour(0xff4caf50);
    colors.meterMid = juce::Colour(0xffffc107);
    colors.meterHigh = juce::Colour(0xffff5722);
    colors.meterPeak = juce::Colour(0xfff44336);
    colors.waveform = colors.accent;
    colors.spectrum = colors.primary;
    
    return colors;
}

ThemeManager::ThemeColors ThemeManager::createClassicThemeColors() {
    ThemeColors colors;
    
    // Classic OTTO colors (based on original design)
    colors.primary = juce::Colour(0xff00ff00);
    colors.secondary = juce::Colour(0xff00ffff);
    colors.accent = juce::Colour(0xffffff00);
    colors.background = juce::Colour(0xff000000);
    colors.surface = juce::Colour(0xff111111);
    colors.error = juce::Colour(0xffff0000);
    colors.warning = juce::Colour(0xffffff00);
    colors.success = juce::Colour(0xff00ff00);
    
    colors.onPrimary = juce::Colours::black;
    colors.onSecondary = juce::Colours::black;
    colors.onBackground = juce::Colour(0xff00ff00);
    colors.onSurface = juce::Colour(0xff00ff00);
    colors.onError = juce::Colours::white;
    
    colors.hover = colors.primary.brighter(0.2f);
    colors.pressed = colors.primary.darker(0.2f);
    colors.disabled = juce::Colour(0xff333333);
    colors.selected = colors.accent.withAlpha(0.3f);
    
    colors.meterLow = juce::Colour(0xff00ff00);
    colors.meterMid = juce::Colour(0xffffff00);
    colors.meterHigh = juce::Colour(0xffff8800);
    colors.meterPeak = juce::Colour(0xffff0000);
    colors.waveform = juce::Colour(0xff00ff00);
    colors.spectrum = juce::Colour(0xff00ffff);
    
    return colors;
}

ThemeManager::ThemeColors ThemeManager::createHighContrastThemeColors() {
    ThemeColors colors;
    colors.setDefaults();
    colors.applyAccessibilityMode(AccessibilityMode::HighContrast);
    return colors;
}

//==============================================================================
// Accessibility Helpers
//==============================================================================

juce::Colour ThemeManager::adjustColorForAccessibility(const juce::Colour& color, AccessibilityMode mode) {
    switch (mode) {
        case AccessibilityMode::HighContrast:
            // Increase contrast
            return color.getPerceivedBrightness() > 0.5f ? juce::Colours::white : juce::Colours::black;
            
        case AccessibilityMode::Protanopia:
            // Simulate red-blindness
            return juce::Colour(0, color.getGreen(), color.getBlue(), color.getAlpha());
            
        case AccessibilityMode::Deuteranopia:
            // Simulate green-blindness
            return juce::Colour(color.getRed(), 0, color.getBlue(), color.getAlpha());
            
        case AccessibilityMode::Tritanopia:
            // Simulate blue-blindness
            return juce::Colour(color.getRed(), color.getGreen(), 0, color.getAlpha());
            
        case AccessibilityMode::Monochrome:
            // Convert to grayscale
            {
                float gray = color.getPerceivedBrightness();
                return juce::Colour::fromFloatRGBA(gray, gray, gray, color.getFloatAlpha());
            }
            
        default:
            return color;
    }
}

float ThemeManager::calculateContrastRatio(const juce::Colour& foreground, const juce::Colour& background) const {
    float fgLuminance = foreground.getPerceivedBrightness();
    float bgLuminance = background.getPerceivedBrightness();
    
    float lighter = juce::jmax(fgLuminance, bgLuminance);
    float darker = juce::jmin(fgLuminance, bgLuminance);
    
    return (lighter + 0.05f) / (darker + 0.05f);
}

bool ThemeManager::meetsAccessibilityStandards(const juce::Colour& foreground, const juce::Colour& background) const {
    float contrastRatio = calculateContrastRatio(foreground, background);
    return contrastRatio >= 4.5f; // WCAG AA standard
}

//==============================================================================
// Seasonal Helpers
//==============================================================================

int ThemeManager::getCurrentSeason() const {
    auto now = juce::Time::getCurrentTime();
    int month = now.getMonth(); // 0-11
    
    // Northern hemisphere seasons (adjust for southern hemisphere if needed)
    if (seasonalSettings.useLocationBasedSeasons && seasonalSettings.latitude < 0) {
        // Southern hemisphere - seasons are reversed
        if (month >= 2 && month <= 4) return 2; // Autumn (Mar-May)
        if (month >= 5 && month <= 7) return 3; // Winter (Jun-Aug)
        if (month >= 8 && month <= 10) return 0; // Spring (Sep-Nov)
        return 1; // Summer (Dec-Feb)
    } else {
        // Northern hemisphere
        if (month >= 2 && month <= 4) return 0; // Spring (Mar-May)
        if (month >= 5 && month <= 7) return 1; // Summer (Jun-Aug)
        if (month >= 8 && month <= 10) return 2; // Autumn (Sep-Nov)
        return 3; // Winter (Dec-Feb)
    }
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
    // Check if enough time has passed since last update
    static juce::Time lastUpdate;
    auto now = juce::Time::getCurrentTime();
    
    if ((now - lastUpdate).inHours() < 1) {
        return false;
    }
    
    lastUpdate = now;
    return true;
}

//==============================================================================
// Validation Helpers
//==============================================================================

bool ThemeManager::isValidThemeName(const juce::String& name) const {
    return !name.isEmpty() && 
           name.length() <= 50 && 
           name.containsOnly("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 -_");
}

bool ThemeManager::isValidColor(const juce::Colour& color) const {
    // Check if color is not completely transparent (unless intended)
    return color.getAlpha() > 0;
}

juce::StringArray ThemeManager::validateThemeColorsInternal(const ThemeColors& colors) const {
    juce::StringArray errors;
    
    // Check contrast ratios for accessibility
    if (!meetsAccessibilityStandards(colors.onBackground, colors.background)) {
        errors.add("Insufficient contrast between text and background");
    }
    
    if (!meetsAccessibilityStandards(colors.onSurface, colors.surface)) {
        errors.add("Insufficient contrast between text and surface");
    }
    
    if (!meetsAccessibilityStandards(colors.onPrimary, colors.primary)) {
        errors.add("Insufficient contrast between primary text and primary color");
    }
    
    // Check for valid colors
    if (!isValidColor(colors.background)) {
        errors.add("Invalid background color");
    }
    
    if (!isValidColor(colors.primary)) {
        errors.add("Invalid primary color");
    }
    
    return errors;
}

//==============================================================================
// Notification Helpers
//==============================================================================

void ThemeManager::notifyThemeChanged() {
    listeners.call([this](Listener& l) { l.themeChanged(currentThemeName); });
}

void ThemeManager::notifyPreviewStarted() {
    listeners.call([this](Listener& l) { l.themePreviewStarted(previewThemeName); });
}

void ThemeManager::notifyPreviewStopped() {
    listeners.call([](Listener& l) { l.themePreviewStopped(); });
}

void ThemeManager::notifyAccessibilityModeChanged() {
    listeners.call([this](Listener& l) { l.accessibilityModeChanged(currentAccessibilityMode); });
}

void ThemeManager::notifySeasonalThemeUpdated() {
    listeners.call([this](Listener& l) { l.seasonalThemeUpdated(getCurrentSeasonalTheme()); });
}

void ThemeManager::notifyValidationFailed(const juce::String& themeName, const juce::StringArray& errors) {
    listeners.call([&](Listener& l) { l.themeValidationFailed(themeName, errors); });
}
