#include "FontManager.h"
#include "INIConfig.h"
#include "BinaryData.h"

const std::map<juce::String, juce::uint32> FontManager::iconMappings = {
    {"gear", 0xE270}, {"settings", 0xE270},
    {"link", 0xE2E2}, {"link-simple", 0xE2E2},
    {"cloud", 0xE1AA}, {"cloud-check", 0xE1AA},

    {"play", 0xE3D0}, {"play-circle", 0xE3D0},
    {"pause", 0xE39E}, {"pause-circle", 0xE39E},
    {"stop", 0xE46C}, {"stop-circle", 0xE46C},
    {"record", 0xE3EE}, {"stack-plus", 0xEDF6},

    {"caret-left", 0xE138}, {"left", 0xE138},
    {"caret-right", 0xE13A}, {"right", 0xE13A},
    {"chevron-left", 0xE138},
    {"chevron-right", 0xE13A},

    {"speaker-high", 0xEA77}, {"volume", 0xEA77},
    {"speaker-x", 0xEA7C}, {"mute", 0xE456}, {"unmute", 0xE44A},
    {"speaker-low", 0xEA78},
    {"microphone", 0xE9BD},

    {"heart", 0xE2A8}, {"favorite", 0xE2A8},
    {"star", 0xE46A},
    {"power", 0xE3DA},
    {"x", 0xE4F6}, {"close", 0xE4F6},
    {"pencil", 0xE3AE}, {"edit", 0xE3AE},
    {"faders", 0xE228}, {"mixer", 0xE228},

    {"metronome", 0xEC8E},
    {"music-note", 0xEC8E}, {"tempo", 0xEC8E},

    {"loop", 0xE9AC}, {"repeat", 0xE9AC},
    {"shuffle", 0xEA0C},
    {"rewind", 0xE9F7}, {"skip-back", 0xEA18},
    {"forward", 0xE91C}, {"skip-forward", 0xEA1B}
};

FontManager::FontManager() {
    for (int i = 0; i < 6; ++i) {
        phosphorTypefaces[i] = nullptr;
    }

    initializeFontDefaults();
}

void FontManager::initializeFontDefaults() {
    clearError();

    try {
        scaleFactor = INIConfig::Defaults::DEFAULT_SCALE_FACTOR;
        currentPhosphorWeight = static_cast<PhosphorWeight>(INIConfig::Defaults::DEFAULT_PHOSPHOR_WEIGHT);
        customFontPath.clear();
        enableAdvancedTypography = false;

        fontMainType = "Roboto";
        fontMainSize = 16.0f;
        fontSecondaryType = "Playfair Display";
        fontSecondarySize = 14.0f;
    }
    catch (const std::exception& e) {
        setError("Error initializing font defaults: " + juce::String(e.what()));
    }
}

juce::File FontManager::getAssetsPath() {
    juce::File exePath = juce::File::getSpecialLocation(juce::File::currentExecutableFile);

    juce::File bundleAssets = exePath.getParentDirectory().getParentDirectory().getChildFile("Resources/Assets");
    if (bundleAssets.exists()) {

        return bundleAssets;
    }

    juce::File currentDir = exePath.getParentDirectory();
    for (int i = 0; i < 5; ++i) {
        juce::File assetsDir = currentDir.getChildFile("Assets");
        if (assetsDir.exists()) {

            return assetsDir;
        }
        currentDir = currentDir.getParentDirectory();
    }


    return juce::File();
}

void FontManager::loadCustomFonts(const juce::File& assetsPath) {
    clearError();

    try {
        // Load fonts from embedded BinaryData (ignore assetsPath - kept for compatibility)
        auto loadTypefaceFromBinary = [&](const char* data, int size, juce::Typeface::Ptr& target, const juce::String& name) {
            target = juce::Typeface::createSystemTypefaceFor(data, size);
            if (target != nullptr) {
                return true;
            } else {
                setError("Failed to load " + name + " font from BinaryData");
                return false;
            }
        };

        loadTypefaceFromBinary(BinaryData::RobotoVariableFont_wdthwght_ttf, BinaryData::RobotoVariableFont_wdthwght_ttfSize, robotoTypeface, "Roboto");
        loadTypefaceFromBinary(BinaryData::PlayfairDisplayVariableFont_wght_ttf, BinaryData::PlayfairDisplayVariableFont_wght_ttfSize, playfairTypeface, "Playfair Display");
        loadTypefaceFromBinary(BinaryData::OrbitronVariableFont_wght_ttf, BinaryData::OrbitronVariableFont_wght_ttfSize, orbitronTypeface, "Orbitron");
        loadTypefaceFromBinary(BinaryData::RobotoCondensedVariableFont_wght_ttf, BinaryData::RobotoCondensedVariableFont_wght_ttfSize, robotoCondensedTypeface, "Roboto Condensed");
        loadTypefaceFromBinary(BinaryData::MontserratVariableFont_wght_ttf, BinaryData::MontserratVariableFont_wght_ttfSize, montserratTypeface, "Montserrat");
        loadTypefaceFromBinary(BinaryData::OpenSansVariableFont_wdthwght_ttf, BinaryData::OpenSansVariableFont_wdthwght_ttfSize, openSansTypeface, "Open Sans");

        loadPhosphorFontsFromBinary();
    }
    catch (const std::exception& e) {
        setError("Error loading custom fonts: " + juce::String(e.what()));
    }
}

void FontManager::loadPhosphorFontsFromBinary() {
    const juce::String phosphorNames[] = {
        "Phosphor Thin", "Phosphor Light", "Phosphor Regular",
        "Phosphor Bold", "Phosphor Fill", "Phosphor Duotone"
    };

    int loadedCount = 0;

    try {
        // Load Phosphor fonts from BinaryData
        phosphorTypefaces[0] = juce::Typeface::createSystemTypefaceFor(BinaryData::PhosphorThin_ttf, BinaryData::PhosphorThin_ttfSize);
        phosphorTypefaces[1] = juce::Typeface::createSystemTypefaceFor(BinaryData::PhosphorLight_ttf, BinaryData::PhosphorLight_ttfSize);
        phosphorTypefaces[2] = juce::Typeface::createSystemTypefaceFor(BinaryData::Phosphor_ttf, BinaryData::Phosphor_ttfSize);
        phosphorTypefaces[3] = juce::Typeface::createSystemTypefaceFor(BinaryData::PhosphorBold_ttf, BinaryData::PhosphorBold_ttfSize);
        phosphorTypefaces[4] = juce::Typeface::createSystemTypefaceFor(BinaryData::PhosphorFill_ttf, BinaryData::PhosphorFill_ttfSize);
        phosphorTypefaces[5] = juce::Typeface::createSystemTypefaceFor(BinaryData::PhosphorDuotone_ttf, BinaryData::PhosphorDuotone_ttfSize);

        for (int i = 0; i < 6; ++i) {
            if (phosphorTypefaces[i] != nullptr) {
                loadedCount++;
            } else {
                setError("Failed to load " + phosphorNames[i] + " from BinaryData");
            }
        }

        if (loadedCount == 0) {
            setError("No Phosphor fonts loaded successfully");
        }

    }
    catch (const std::exception& e) {
        setError("Error loading Phosphor fonts: " + juce::String(e.what()));
    }
}

juce::Font FontManager::createFontWithOptions(juce::Typeface::Ptr typeface,
                                             const juce::String& fallbackName,
                                             float size) const {
    if (typeface != nullptr) {
#if JUCE_MAJOR_VERSION >= 8
        return juce::Font(juce::FontOptions(typeface).withHeight(scaled(size)));
#else
        return juce::Font(typeface).withHeight(scaled(size));
#endif
    }
#if JUCE_MAJOR_VERSION >= 8
    return juce::Font(juce::FontOptions().withName(fallbackName).withHeight(scaled(size)));
#else
    return juce::Font(fallbackName, scaled(size), juce::Font::plain);
#endif
}

juce::Font FontManager::getFont(FontRole role, float size) const {
    switch (role) {
        case FontRole::Body:
        case FontRole::Numeric:
            return createFontWithOptions(robotoTypeface, "Arial",
                size > 0 ? size : getDefaultSize(role));

        case FontRole::Header:
        case FontRole::Button:
            return createFontWithOptions(playfairTypeface, "Georgia",
                size > 0 ? size : getDefaultSize(role));

        case FontRole::Brand:
            return createFontWithOptions(orbitronTypeface, "Arial Black",
                size > 0 ? size : getDefaultSize(role));

        case FontRole::Icon:
            return getPhosphorFont(currentPhosphorWeight,
                size > 0 ? size : getDefaultSize(role));

        case FontRole::Version:
            return createFontWithOptions(robotoCondensedTypeface, "Arial Narrow",
                size > 0 ? size : getDefaultSize(role));

        case FontRole::Popup:
            return createFontWithOptions(montserratTypeface, "Helvetica",
                size > 0 ? size : getDefaultSize(role));

        default:
            return createFontWithOptions(robotoTypeface, "Arial",
                size > 0 ? size : getDefaultSize(FontRole::Body));
    }
}

juce::Font FontManager::getPhosphorFont(PhosphorWeight weight, float size) const {
    int index = static_cast<int>(weight);
    if (index >= 0 && index < 6 && phosphorTypefaces[index] != nullptr) {
        return createFontWithOptions(phosphorTypefaces[index], "Arial", size);
    }

    if (phosphorTypefaces[2] != nullptr) {
        return createFontWithOptions(phosphorTypefaces[2], "Arial", size);
    }

    return createFontWithOptions(nullptr, "Arial", size);
}

juce::uint32 FontManager::getIconUnicode(const juce::String& iconName) const {
    auto it = iconMappings.find(iconName);
    return (it != iconMappings.end()) ? it->second : 0x0000;
}

juce::String FontManager::getIconString(const juce::String& iconName) const {
    auto unicode = getIconUnicode(iconName);
    return juce::String::charToString(static_cast<juce::juce_wchar>(unicode));
}

void FontManager::saveStates(ComponentState& state) const {
    clearError();

    try {
        state.globalSettings.fontMainType = fontMainType;
        state.globalSettings.fontMainSize = fontMainSize;
        state.globalSettings.fontSecondaryType = fontSecondaryType;
        state.globalSettings.fontSecondarySize = fontSecondarySize;
        state.globalSettings.phosphorWeight = static_cast<int>(currentPhosphorWeight);
        state.globalSettings.enableAdvancedTypography = enableAdvancedTypography;
        state.globalSettings.interfaceScale = scaleFactor;
        state.globalSettings.customFontPath = customFontPath;

        state.themeSettings.fontMain = fontMainType;
        state.themeSettings.fontMainSize = fontMainSize;
        state.themeSettings.fontMainWeight = getPhosphorWeightString(currentPhosphorWeight);
        state.themeSettings.fontSecondary = fontSecondaryType;
        state.themeSettings.fontSecondarySize = fontSecondarySize;
        state.themeSettings.fontSecondaryWeight = enableAdvancedTypography ? "Advanced" : "Normal";
    }
    catch (const std::exception& e) {
        setError("Exception saving font settings: " + juce::String(e.what()));
    }
}

void FontManager::loadStates(const ComponentState& state) {
    clearError();

    try {
        if (state.globalSettings.interfaceScale > 0) {
            scaleFactor = INIConfig::clampScale(state.globalSettings.interfaceScale);
        }

        if (state.globalSettings.fontMainType.isNotEmpty()) {
            fontMainType = state.globalSettings.fontMainType;
        }
        if (state.globalSettings.fontMainSize > 0) {
            fontMainSize = state.globalSettings.fontMainSize;
        }
        if (state.globalSettings.fontSecondaryType.isNotEmpty()) {
            fontSecondaryType = state.globalSettings.fontSecondaryType;
        }
        if (state.globalSettings.fontSecondarySize > 0) {
            fontSecondarySize = state.globalSettings.fontSecondarySize;
        }

        if (state.globalSettings.phosphorWeight >= 0 && state.globalSettings.phosphorWeight <= 5) {
            setPhosphorWeightFromInt(state.globalSettings.phosphorWeight);
        }

        enableAdvancedTypography = state.globalSettings.enableAdvancedTypography;

        if (state.globalSettings.customFontPath.isNotEmpty()) {
            customFontPath = state.globalSettings.customFontPath;
        }
    }
    catch (const std::exception& e) {
        setError("Exception loading font settings: " + juce::String(e.what()));
        initializeFontDefaults();
    }
}

bool FontManager::arePhosphorFontsLoaded() const {
    for (int i = 0; i < 6; ++i) {
        if (phosphorTypefaces[i] != nullptr) {
            return true;
        }
    }
    return false;
}

juce::StringArray FontManager::getLoadedFontNames() const {
    juce::StringArray loadedFonts;

    if (robotoTypeface != nullptr) {
        loadedFonts.add("Roboto Variable");
    }

    if (playfairTypeface != nullptr) {
        loadedFonts.add("Playfair Display Variable");
    }

    if (orbitronTypeface != nullptr) {
        loadedFonts.add("Orbitron Variable");
    }

    if (robotoCondensedTypeface != nullptr) {
        loadedFonts.add("Roboto Condensed Variable");
    }

    if (montserratTypeface != nullptr) {
        loadedFonts.add("Montserrat Variable");
    }

    if (openSansTypeface != nullptr) {
        loadedFonts.add("Open Sans Variable");
    }

    const juce::StringArray phosphorNames = {
        "Phosphor Thin", "Phosphor Light", "Phosphor Regular",
        "Phosphor Bold", "Phosphor Fill", "Phosphor Duotone"
    };

    for (int i = 0; i < 6; ++i) {
        if (phosphorTypefaces[i] != nullptr) {
            loadedFonts.add(phosphorNames[i]);
        }
    }

    return loadedFonts;
}

juce::String FontManager::getPhosphorWeightString(PhosphorWeight weight) const {
    switch (weight) {
        case PhosphorWeight::Thin: return "Thin";
        case PhosphorWeight::Light: return "Light";
        case PhosphorWeight::Regular: return "Regular";
        case PhosphorWeight::Bold: return "Bold";
        case PhosphorWeight::Fill: return "Fill";
        case PhosphorWeight::Duotone: return "Duotone";
        default: return "Regular";
    }
}

FontManager::PhosphorWeight FontManager::getPhosphorWeightFromString(const juce::String& weightStr) const {
    if (weightStr == "Thin") return PhosphorWeight::Thin;
    if (weightStr == "Light") return PhosphorWeight::Light;
    if (weightStr == "Bold") return PhosphorWeight::Bold;
    if (weightStr == "Fill") return PhosphorWeight::Fill;
    if (weightStr == "Duotone") return PhosphorWeight::Duotone;
    return PhosphorWeight::Regular;
}
