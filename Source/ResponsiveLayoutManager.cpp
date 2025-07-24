#include "ResponsiveLayoutManager.h"
#include "INIConfig.h"

ResponsiveLayoutManager::ResponsiveLayoutManager(const LayoutConfig& config)
    : config(config) {
    currentScale = INIConfig::Defaults::DEFAULT_INTERFACE_SCALE;
    windowX = INIConfig::Defaults::DEFAULT_INTERFACE_X;
    windowY = INIConfig::Defaults::DEFAULT_INTERFACE_Y;
    windowWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
    windowHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
    clearError();
}

float ResponsiveLayoutManager::calculateScale(int currentWidth, int currentHeight) const {
    if (currentWidth <= 0 || currentHeight <= 0) {
        setError("Invalid window dimensions for scale calculation");
        return INIConfig::Defaults::DEFAULT_INTERFACE_SCALE;
    }

    float widthScale = static_cast<float>(currentWidth) / static_cast<float>(config.baseWidth);
    float heightScale = static_cast<float>(currentHeight) / static_cast<float>(config.baseHeight);

    float scale = juce::jmin(widthScale, heightScale);
    return INIConfig::clampScale(scale);
}

void ResponsiveLayoutManager::updateScale(int currentWidth, int currentHeight) {
    try {
        float newScale = calculateScale(currentWidth, currentHeight);

        if (std::abs(newScale - currentScale) > INIConfig::LayoutConstants::responsiveLayoutScaleThreshold) {
            float previousScale = currentScale;
            currentScale = newScale;
            windowWidth = INIConfig::clampWindowSize(currentWidth);
            windowHeight = INIConfig::clampWindowSize(currentHeight);

            if (onScaleChanged) {
                onScaleChanged(currentScale);
            }

            clearError();
        }
    }
    catch (const std::exception& e) {
        setError("Error updating scale: " + juce::String(e.what()));
    }
}

int ResponsiveLayoutManager::scaled(int dimension) const {
    if (!isValidDimension(dimension)) {
        setError("Invalid dimension for scaling: " + juce::String(dimension));
        return dimension;
    }

    return static_cast<int>(dimension * currentScale);
}

float ResponsiveLayoutManager::scaled(float dimension) const {
    if (!std::isfinite(dimension)) {
        setError("Invalid float dimension for scaling");
        return dimension;
    }

    return dimension * currentScale;
}

juce::Rectangle<int> ResponsiveLayoutManager::scaled(const juce::Rectangle<int>& rect) const {
    try {
        return juce::Rectangle<int>(
            scaled(rect.getX()),
            scaled(rect.getY()),
            scaled(rect.getWidth()),
            scaled(rect.getHeight())
        );
    }
    catch (const std::exception& e) {
        setError("Error scaling rectangle: " + juce::String(e.what()));
        return rect;
    }
}

void ResponsiveLayoutManager::setupConstrainer(juce::ComponentBoundsConstrainer& constrainer) const {
    try {
        constrainer.setFixedAspectRatio(config.aspectRatio);

        int minWidth = static_cast<int>(config.baseWidth * config.minScale);
        int minHeight = static_cast<int>(config.baseHeight * config.minScale);
        int maxWidth = static_cast<int>(config.baseWidth * config.maxScale);
        int maxHeight = static_cast<int>(config.baseHeight * config.maxScale);

        minWidth = INIConfig::clampWindowSize(minWidth);
        minHeight = INIConfig::clampWindowSize(minHeight);
        maxWidth = INIConfig::clampWindowSize(maxWidth);
        maxHeight = INIConfig::clampWindowSize(maxHeight);

        constrainer.setSizeLimits(minWidth, minHeight, maxWidth, maxHeight);
        clearError();
    }
    catch (const std::exception& e) {
        setError("Error setting up constrainer: " + juce::String(e.what()));
    }
}

juce::Rectangle<int> ResponsiveLayoutManager::getCenteredBounds(int containerWidth, int containerHeight) const {
    try {
        int scaledWidth = scaled(config.baseWidth);
        int scaledHeight = scaled(config.baseHeight);

        int offsetX = (containerWidth - scaledWidth) / INIConfig::LayoutConstants::responsiveLayoutCenterDivisor;
        int offsetY = (containerHeight - scaledHeight) / INIConfig::LayoutConstants::responsiveLayoutCenterDivisor;

        offsetX = juce::jmax(0, offsetX);
        offsetY = juce::jmax(0, offsetY);

        return juce::Rectangle<int>(offsetX, offsetY, scaledWidth, scaledHeight);
    }
    catch (const std::exception& e) {
        setError("Error calculating centered bounds: " + juce::String(e.what()));
        return juce::Rectangle<int>(0, 0, config.baseWidth, config.baseHeight);
    }
}

void ResponsiveLayoutManager::saveState(ComponentState& state) const {
    try {
        state.globalSettings.interfaceScale = currentScale;
        state.globalSettings.interfaceStartingX = windowX;
        state.globalSettings.interfaceStartingY = windowY;
        state.globalSettings.interfaceWidth = windowWidth;
        state.globalSettings.interfaceHeight = windowHeight;

        clearError();
    }
    catch (const std::exception& e) {
        setError("Error saving layout state: " + juce::String(e.what()));
    }
}

void ResponsiveLayoutManager::loadState(const ComponentState& state) {
    try {
        clearError();

        currentScale = INIConfig::clampScale(state.globalSettings.interfaceScale);
        windowX = state.globalSettings.interfaceStartingX;
        windowY = state.globalSettings.interfaceStartingY;
        windowWidth = INIConfig::clampWindowSize(state.globalSettings.interfaceWidth);
        windowHeight = INIConfig::clampWindowSize(state.globalSettings.interfaceHeight);

        if (!validateLayoutSettings()) {
            setError("Loaded layout settings failed validation - using defaults");
            resetToDefaults();
        }
    }
    catch (const std::exception& e) {
        setError("Error loading layout state: " + juce::String(e.what()));
        resetToDefaults();
    }
}

bool ResponsiveLayoutManager::validateLayoutSettings() const {
    try {
        if (!INIConfig::isValidInterfaceScale(currentScale)) {
            return false;
        }

        if (windowWidth < INIConfig::Validation::MIN_WINDOW_SIZE ||
            windowWidth > INIConfig::Validation::MAX_WINDOW_SIZE ||
            windowHeight < INIConfig::Validation::MIN_WINDOW_SIZE ||
            windowHeight > INIConfig::Validation::MAX_WINDOW_SIZE) {
            return false;
        }

        if (config.baseWidth <= 0 || config.baseHeight <= 0 ||
            config.aspectRatio <= 0.0f ||
            config.minScale <= 0.0f || config.maxScale <= 0.0f ||
            config.minScale >= config.maxScale) {
            return false;
        }

        return true;
    }
    catch (const std::exception& e) {
        setError("Error validating layout settings: " + juce::String(e.what()));
        return false;
    }
}

void ResponsiveLayoutManager::resetToDefaults() {
    try {
        currentScale = INIConfig::Defaults::DEFAULT_INTERFACE_SCALE;
        windowX = INIConfig::Defaults::DEFAULT_INTERFACE_X;
        windowY = INIConfig::Defaults::DEFAULT_INTERFACE_Y;
        windowWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
        windowHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;

        config.baseWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
        config.baseHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
        config.aspectRatio = INIConfig::LayoutConstants::responsiveLayoutDefaultAspectRatio;
        config.minScale = INIConfig::Validation::MIN_INTERFACE_SCALE;
        config.maxScale = INIConfig::Validation::MAX_INTERFACE_SCALE;

        clearError();
    }
    catch (const std::exception& e) {
        setError("Error resetting to defaults: " + juce::String(e.what()));
    }
}

ResponsiveLayoutManager::LayoutState ResponsiveLayoutManager::getCurrentState() const {
    LayoutState state;
    state.currentScale = currentScale;
    state.windowX = windowX;
    state.windowY = windowY;
    state.windowWidth = windowWidth;
    state.windowHeight = windowHeight;
    state.config = config;
    return state;
}

void ResponsiveLayoutManager::applyState(const LayoutState& state) {
    try {
        currentScale = INIConfig::clampScale(state.currentScale);
        windowX = state.windowX;
        windowY = state.windowY;
        windowWidth = INIConfig::clampWindowSize(state.windowWidth);
        windowHeight = INIConfig::clampWindowSize(state.windowHeight);
        config = state.config;

        if (!validateLayoutSettings()) {
            setError("Applied state failed validation - using defaults");
            resetToDefaults();
        } else {
            clearError();
        }
    }
    catch (const std::exception& e) {
        setError("Error applying layout state: " + juce::String(e.what()));
        resetToDefaults();
    }
}

void ResponsiveLayoutManager::setError(const juce::String& error) const {
    lastError = error;
}

bool ResponsiveLayoutManager::isValidDimension(int dimension) const {
    return dimension >= 0 && dimension <= INIConfig::Validation::MAX_WINDOW_SIZE;
}

void ResponsiveLayoutManager::setScale(float newScale) {
    try {
        float clampedScale = INIConfig::clampScale(newScale);

        if (std::abs(clampedScale - currentScale) > INIConfig::LayoutConstants::responsiveLayoutScaleThreshold) {
            currentScale = clampedScale;

            if (onScaleChanged) {
                onScaleChanged(currentScale);
            }

            clearError();
        }
    }
    catch (const std::exception& e) {
        setError("Error setting scale: " + juce::String(e.what()));
    }
}

void ResponsiveLayoutManager::rememberWindowPosition(int x, int y, int displayIndex) {
    windowX = x;
    windowY = y;
    preferredDisplayIndex = displayIndex;
}

void ResponsiveLayoutManager::restoreWindowPosition(juce::Component* window) {
    if (window == nullptr) return;

    auto& displays = juce::Desktop::getInstance().getDisplays();

    if (preferredDisplayIndex >= 0 && preferredDisplayIndex < displays.displays.size()) {
        auto preferredDisplayArea = displays.displays[preferredDisplayIndex].userArea;

        if (preferredDisplayArea.contains(windowX, windowY)) {
            window->setTopLeftPosition(windowX, windowY);
            return;
        }
    }

    window->centreWithSize(windowWidth, windowHeight);
}

float ResponsiveLayoutManager::getSystemDPIScale() const {
    auto& displays = juce::Desktop::getInstance().getDisplays();
    if (preferredDisplayIndex >= 0 && preferredDisplayIndex < displays.displays.size()) {
        return displays.displays[preferredDisplayIndex].dpi / INIConfig::LayoutConstants::responsiveLayoutBaseDPI;
    }
    return 1.0f;
}

void ResponsiveLayoutManager::applyDPIScale(juce::Component* component) {
    if (component == nullptr) return;

    float dpiScale = getSystemDPIScale();
    if (std::abs(dpiScale - 1.0f) > INIConfig::LayoutConstants::responsiveLayoutDPIThreshold) {
        float adjustedScale = currentScale * dpiScale;
        setScale(adjustedScale);
    }
}
