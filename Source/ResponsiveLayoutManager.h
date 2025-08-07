#pragma once
#include <JuceHeader.h>
#include "ComponentState.h"
#include "INIConfig.h"

class ResponsiveLayoutManager {
public:
    struct LayoutConfig {
        int baseWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
        int baseHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
        float aspectRatio = static_cast<float>(INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH) / static_cast<float>(INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT);
        float minScale = INIConfig::Validation::MIN_INTERFACE_SCALE;
        float maxScale = INIConfig::Validation::MAX_INTERFACE_SCALE;
    };

    struct LayoutState {
        float currentScale;
        int windowX;
        int windowY;
        int windowWidth;
        int windowHeight;
        LayoutConfig config;
    };

    explicit ResponsiveLayoutManager(const LayoutConfig& config);

    ~ResponsiveLayoutManager() {
        onScaleChanged = nullptr;
    }

    float calculateScale(int currentWidth, int currentHeight) const;
    void updateScale(int currentWidth, int currentHeight);
    void setScale(float newScale);

    int scaled(int dimension) const;
    float scaled(float dimension) const;
    juce::Rectangle<int> scaled(const juce::Rectangle<int>& rect) const;

    void setupConstrainer(juce::ComponentBoundsConstrainer& constrainer) const;
    juce::Rectangle<int> getCenteredBounds(int containerWidth, int containerHeight) const;

    void saveState(ComponentState& state) const;
    void loadState(const ComponentState& state);

    LayoutState getCurrentState() const;
    void applyState(const LayoutState& state);

    bool validateLayoutSettings() const;
    void resetToDefaults();

    float getCurrentScale() const { return currentScale; }
    int getWindowX() const { return windowX; }
    int getWindowY() const { return windowY; }
    int getWindowWidth() const { return windowWidth; }
    int getWindowHeight() const { return windowHeight; }
    const LayoutConfig& getConfig() const { return config; }

    juce::String getLastError() const { return lastError; }
    bool hasError() const { return !lastError.isEmpty(); }
    void clearError() const { lastError.clear(); }

    std::function<void(float)> onScaleChanged;

    void rememberWindowPosition(int x, int y, int displayIndex);
    void restoreWindowPosition(juce::Component* window);
    int getPreferredDisplayIndex() const { return preferredDisplayIndex; }
    void setPreferredDisplayIndex(int index) { preferredDisplayIndex = index; }

    float getSystemDPIScale() const;
    void applyDPIScale(juce::Component* component);

    void debugPrintState() const {
        if (hasError()) {
        }
    }

    bool verifyConfiguration() const {
        bool valid = validateLayoutSettings();
        if (!valid) {
            debugPrintState();
        }
        return valid;
    }

private:
    LayoutConfig config;
    float currentScale;
    int windowX;
    int windowY;
    int windowWidth;
    int windowHeight;
    int preferredDisplayIndex = INIConfig::Defaults::ZERO_VALUE;
    mutable juce::String lastError;

    void setError(const juce::String& error) const;
    bool isValidDimension(int dimension) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponsiveLayoutManager)
};
