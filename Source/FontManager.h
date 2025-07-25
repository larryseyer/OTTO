#pragma once
#include <JuceHeader.h>
#include "ComponentState.h"
#include "INIConfig.h"

class FontManager {
public:
    enum class PhosphorWeight {
        Thin = 0, Light = 1, Regular = 2, Bold = 3, Fill = 4, Duotone = 5
    };

    enum class FontRole {
        Body,
        Header,
        Numeric,
        Brand,
        Icon,
        Button,
        Version,
        Popup
    };

    FontManager();
    ~FontManager() = default;

    juce::Font getFont(FontRole role, float size = INIConfig::Validation::MIN_VOLUME) const;
    juce::Font getPhosphorFont(PhosphorWeight weight = PhosphorWeight::Regular, float size = INIConfig::Fonts::ICON_DEFAULT_SIZE) const;

    void loadCustomFonts(const juce::File& assetsPath);
    bool hasCustomFonts() const { return robotoTypeface != nullptr; }

    static juce::File getAssetsPath();

    juce::uint32 getIconUnicode(const juce::String& iconName) const;
    juce::String getIconString(const juce::String& iconName) const;

    void setScaleFactor(float scale) { scaleFactor = INIConfig::clampScale(scale); }
    float getScaleFactor() const { return scaleFactor; }

    void setPhosphorWeight(PhosphorWeight weight) { currentPhosphorWeight = weight; }
    PhosphorWeight getPhosphorWeight() const { return currentPhosphorWeight; }
    int getPhosphorWeightAsInt() const { return static_cast<int>(currentPhosphorWeight); }
    void setPhosphorWeightFromInt(int weight) {
        currentPhosphorWeight = static_cast<PhosphorWeight>(INIConfig::clampPhosphorWeight(weight));
    }

    void saveStates(ComponentState& state) const;
    void loadStates(const ComponentState& state);
    void initializeFontDefaults();

    float getDefaultSize(FontRole role) const;
    juce::String getDefaultFontName(FontRole role) const;

    struct FontSettings {
        PhosphorWeight phosphorWeight = PhosphorWeight::Regular;
        float scaleFactor = INIConfig::Defaults::DEFAULT_INTERFACE_SCALE;
        bool hasCustomFonts = false;
        juce::String customFontPath = "";
        bool enableAdvancedTypography = false;
    };

    FontSettings getCurrentSettings() const {
        FontSettings settings;
        settings.phosphorWeight = currentPhosphorWeight;
        settings.scaleFactor = scaleFactor;
        settings.hasCustomFonts = hasCustomFonts();
        settings.customFontPath = customFontPath;
        settings.enableAdvancedTypography = enableAdvancedTypography;
        return settings;
    }

    void applySettings(const FontSettings& settings) {
        setPhosphorWeight(settings.phosphorWeight);
        setScaleFactor(settings.scaleFactor);
        customFontPath = settings.customFontPath;
        enableAdvancedTypography = settings.enableAdvancedTypography;
    }

    juce::String getLastError() const { return lastError; }
    bool hasError() const { return !lastError.isEmpty(); }
    void clearError() const { lastError.clear(); }

    bool arePhosphorFontsLoaded() const;
    juce::StringArray getLoadedFontNames() const;

    juce::String getFontMainType() const { return fontMainType; }
    void setFontMainType(const juce::String& type) { fontMainType = type; }

    float getFontMainSize() const { return fontMainSize; }
    void setFontMainSize(float size) { fontMainSize = size; }

    juce::String getFontSecondaryType() const { return fontSecondaryType; }
    void setFontSecondaryType(const juce::String& type) { fontSecondaryType = type; }

    float getFontSecondarySize() const { return fontSecondarySize; }
    void setFontSecondarySize(float size) { fontSecondarySize = size; }

    juce::String getPhosphorWeightString(PhosphorWeight weight) const;
    PhosphorWeight getPhosphorWeightFromString(const juce::String& weightStr) const;

    static FontManager* getInstance() {
        static FontManager instance;
        return &instance;
    }

private:
    juce::Typeface::Ptr robotoTypeface;
    juce::Typeface::Ptr playfairTypeface;
    juce::Typeface::Ptr orbitronTypeface;
    juce::Typeface::Ptr robotoCondensedTypeface;
    juce::Typeface::Ptr montserratTypeface;
    juce::Typeface::Ptr openSansTypeface;
    juce::Typeface::Ptr phosphorTypefaces[INIConfig::UI::MAX_PHOSPHOR_TYPEFACES];

    float scaleFactor = INIConfig::Defaults::DEFAULT_INTERFACE_SCALE;
    PhosphorWeight currentPhosphorWeight = static_cast<PhosphorWeight>(INIConfig::Defaults::DEFAULT_PHOSPHOR_WEIGHT);
    juce::String customFontPath;
    bool enableAdvancedTypography = false;
    mutable juce::String lastError;

    juce::String fontMainType = "Roboto";
    float fontMainSize = INIConfig::Fonts::BODY_DEFAULT_SIZE;
    juce::String fontSecondaryType = "Playfair Display";
    float fontSecondarySize = INIConfig::Fonts::HEADER_DEFAULT_SIZE;

    static const std::map<juce::String, juce::uint32> iconMappings;

    juce::Font createFontWithOptions(juce::Typeface::Ptr typeface,
                                    const juce::String& fallbackName,
                                    float size) const;

    void loadPhosphorFontsFromBinary();
    float scaled(float size) const { return size * scaleFactor; }
    void setError(const juce::String& error) const { lastError = error; }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FontManager)
};

inline float FontManager::getDefaultSize(FontRole role) const {
    switch (role) {
        case FontRole::Body: return fontMainSize;
        case FontRole::Header: return fontSecondarySize;
        case FontRole::Numeric: return INIConfig::Fonts::NUMERIC_DEFAULT_SIZE;
        case FontRole::Brand: return INIConfig::Fonts::BRAND_DEFAULT_SIZE;
        case FontRole::Button: return INIConfig::Fonts::BUTTON_DEFAULT_SIZE;
        case FontRole::Icon: return INIConfig::Fonts::ICON_DEFAULT_SIZE;
        case FontRole::Version: return INIConfig::Fonts::VERSION_DEFAULT_SIZE;
        case FontRole::Popup: return INIConfig::Fonts::POPUP_DEFAULT_SIZE;
        default: return INIConfig::Fonts::BUTTON_DEFAULT_SIZE;
    }
}

inline juce::String FontManager::getDefaultFontName(FontRole role) const {
    switch (role) {
        case FontRole::Body:
        case FontRole::Numeric:
            return fontMainType;
        case FontRole::Header:
        case FontRole::Button:
            return fontSecondaryType;
        case FontRole::Brand:
            return orbitronTypeface != nullptr ? "Orbitron" : "Arial Black";
        case FontRole::Icon:
            return "Phosphor";
        case FontRole::Version:
            return robotoCondensedTypeface != nullptr ? "Roboto Condensed" : "Arial Narrow";
        case FontRole::Popup:
            return montserratTypeface != nullptr ? "Montserrat" : "Helvetica";
        default:
            return "Arial";
    }
}
