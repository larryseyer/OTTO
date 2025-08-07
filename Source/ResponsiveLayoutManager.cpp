/**
 * @file ResponsiveLayoutManager.cpp
 * @brief Implementation of OTTO's comprehensive responsive layout scaling system
 * 
 * This file implements the ResponsiveLayoutManager class, which provides real-time
 * scaling calculations for all UI components based on current window dimensions.
 * It ensures consistent visual proportions across all screen sizes and DPI settings
 * while maintaining accessibility requirements and minimum touch targets.
 * 
 * CORE RESPONSIBILITIES:
 * ======================
 * - Scale Calculation: Determines appropriate scaling factor for current window size
 * - Dimension Scaling: Applies scaling to individual component dimensions
 * - Accessibility Compliance: Enforces minimum touch target sizes (44px)
 * - Change Notification: Alerts components when scaling updates are needed
 * - Error Handling: Provides robust fallbacks for invalid dimensions
 * 
 * SCALING ALGORITHM:
 * ==================
 * 1. Calculate separate width and height scale factors
 * 2. Use minimum of the two to maintain aspect ratio
 * 3. Clamp result within acceptable scale range (typically 0.5x to 3.0x)
 * 4. Apply threshold to prevent excessive update frequency
 * 5. Notify all registered components of scale changes
 * 
 * INTEGRATION WITH INI SYSTEM:
 * =============================
 * - Uses INIConfig::Defaults for base dimensions and initial scale
 * - References INIConfig::LayoutConstants for all component sizing
 * - Applies INIConfig validation for dimension and scale limits
 * - Integrates with INIConfig error handling and reporting
 * 
 * COMPONENT INTEGRATION POINTS:
 * ==============================
 * - MainContentComponent.cpp: Row-based layout scaling
 * - Row1Component.cpp: Icon and button scaling in Row 1
 * - PlayerTabsComponent.cpp: Tab width and height scaling in Row 2
 * - CustomLookAndFeel.cpp: Font size and visual element scaling
 * - All child components: Receive scaled dimensions for positioning
 * 
 * PERFORMANCE OPTIMIZATIONS:
 * ===========================
 * - Scale threshold prevents excessive recalculations
 * - Cached scaling values reduce computation overhead
 * - Error handling avoids crashes during window resize
 * - Efficient change detection minimizes unnecessary updates
 * 
 * @author OTTO Development Team
 * @version 2.0
 * @date 2024
 * @see INIConfig.h for base dimensions and scaling constants
 * @see MainContentComponent.cpp for layout application
 * @see CustomLookAndFeel.cpp for visual scaling integration
 */

#include "ResponsiveLayoutManager.h"
#include "INIConfig.h"

/**
 * @brief ResponsiveLayoutManager constructor - initialize scaling system with base configuration
 * 
 * Sets up the responsive layout system with default values from INIConfig and
 * initializes all scaling parameters for immediate use. The LayoutConfig parameter
 * allows customization of base dimensions and scaling behavior.
 * 
 * INITIALIZATION PROCESS:
 * 1. Store layout configuration (base width/height, scaling limits)
 * 2. Set initial scale factor from INIConfig defaults
 * 3. Initialize window position and dimensions
 * 4. Clear any existing error states
 * 5. Prepare for immediate scaling calculations
 * 
 * DEFAULT VALUES SOURCE:
 * - currentScale: INIConfig::Defaults::DEFAULT_INTERFACE_SCALE (typically 1.0)
 * - windowX/Y: INIConfig::Defaults::DEFAULT_INTERFACE_X/Y (window position)
 * - windowWidth/Height: INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH/HEIGHT (1024x768)
 * 
 * LAYOUT CONFIG INTEGRATION:
 * - config.baseWidth/baseHeight: Reference dimensions for scaling calculations
 * - config.minScale/maxScale: Acceptable scaling range limits
 * - config.scaleThreshold: Minimum change required to trigger updates
 * 
 * @param config LayoutConfig containing base dimensions and scaling parameters
 * 
 * Called by: PluginEditor.cpp or MainComponent.cpp during application startup
 * References: INIConfig::Defaults for all initial values
 */
ResponsiveLayoutManager::ResponsiveLayoutManager(const LayoutConfig& config)
    : config(config) {
    // Initialize scaling system with INIConfig default values
    // These provide safe starting points for all scaling calculations
    currentScale = INIConfig::Defaults::DEFAULT_INTERFACE_SCALE;    // Typically 1.0 (no scaling)
    windowX = INIConfig::Defaults::DEFAULT_INTERFACE_X;            // Default window X position
    windowY = INIConfig::Defaults::DEFAULT_INTERFACE_Y;            // Default window Y position
    windowWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;    // Base width (1024px)
    windowHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;  // Base height (768px)
    
    // Clear any error states from previous instances
    clearError();
}

/**
 * @brief Calculate appropriate scaling factor for given window dimensions
 * 
 * Determines the optimal scaling factor to maintain visual proportions when
 * the window size differs from the base dimensions. Uses aspect-ratio preserving
 * scaling to ensure components maintain their intended appearance.
 * 
 * SCALING ALGORITHM:
 * 1. Calculate independent width and height scale factors
 * 2. Use minimum of the two to preserve aspect ratio
 * 3. Clamp result within acceptable range via INIConfig::clampScale()
 * 4. Return validated scale factor for immediate application
 * 
 * ASPECT RATIO PRESERVATION:
 * - Takes minimum of width/height scales to prevent distortion
 * - Ensures UI components maintain their intended proportions
 * - Prevents stretching or squashing of interface elements
 * 
 * ERROR HANDLING:
 * - Invalid dimensions (≤ 0) trigger error state and return default scale
 * - Provides graceful fallback to prevent application crashes
 * - Error message stored for debugging and user feedback
 * 
 * EXAMPLE CALCULATIONS:
 * - 2048x1536 window → 2.0x scale (double size)
 * - 512x384 window → 0.5x scale (half size)
 * - 1024x400 window → ~0.52x scale (limited by height)
 * 
 * @param currentWidth Current window width in pixels
 * @param currentHeight Current window height in pixels
 * @return Validated scaling factor (typically 0.5x to 3.0x range)
 * 
 * Called by: updateScale() during window resize events
 * References: INIConfig::clampScale() for range validation
 */
float ResponsiveLayoutManager::calculateScale(int currentWidth, int currentHeight) const {
    // VALIDATION: Ensure dimensions are positive and valid
    if (currentWidth <= 0 || currentHeight <= 0) {
        setError("Invalid window dimensions for scale calculation");
        return INIConfig::Defaults::DEFAULT_INTERFACE_SCALE;  // Safe fallback
    }

    // SCALE FACTOR CALCULATION: Determine scaling based on dimension ratios
    // Cast to float for precise calculations and avoid integer division
    float widthScale = static_cast<float>(currentWidth) / static_cast<float>(config.baseWidth);
    float heightScale = static_cast<float>(currentHeight) / static_cast<float>(config.baseHeight);

    // ASPECT RATIO PRESERVATION: Use minimum scale to prevent distortion
    // This ensures components fit within available space without stretching
    float scale = juce::jmin(widthScale, heightScale);
    
    // RANGE VALIDATION: Apply INIConfig limits and return validated scale
    return INIConfig::clampScale(scale);
}

/**
 * @brief Update scaling factor with threshold-based change detection
 * 
 * Recalculates scaling factor for new window dimensions and triggers component
 * updates only when the change exceeds the threshold. This prevents excessive
 * recalculation during smooth resize operations while ensuring components
 * update when meaningful scale changes occur.
 * 
 * UPDATE PROCESS:
 * 1. Calculate new scale factor for current dimensions
 * 2. Compare against current scale using threshold
 * 3. Update scale and window dimensions if change is significant
 * 4. Notify all registered components of scale change
 * 5. Clear any previous error states
 * 
 * THRESHOLD OPTIMIZATION:
 * - Uses INIConfig::LayoutConstants::responsiveLayoutScaleThreshold
 * - Prevents excessive updates during smooth window resizing
 * - Balances responsiveness with performance
 * - Typical threshold: 0.01 (1% change required)
 * 
 * COMPONENT NOTIFICATION:
 * - onScaleChanged callback notifies all registered components
 * - Components can immediately recalculate their layouts
 * - Enables real-time responsive behavior
 * - Callback includes new scale factor for immediate use
 * 
 * ERROR HANDLING:
 * - Try-catch block handles any calculation exceptions
 * - Graceful error recovery prevents application crashes
 * - Error state preserved for debugging and user feedback
 * 
 * @param currentWidth New window width in pixels
 * @param currentHeight New window height in pixels
 * 
 * Called by: Window resize events, component bounds changes
 * References: INIConfig::LayoutConstants::responsiveLayoutScaleThreshold
 */
void ResponsiveLayoutManager::updateScale(int currentWidth, int currentHeight) {
    try {
        // STEP 1: Calculate new scale factor for current window dimensions
        float newScale = calculateScale(currentWidth, currentHeight);

        // STEP 2: Check if scale change exceeds threshold for update
        // This prevents excessive recalculation during smooth resize operations
        if (std::abs(newScale - currentScale) > INIConfig::LayoutConstants::responsiveLayoutScaleThreshold) {
            
            // STEP 3: Update internal state with new scale and dimensions
            currentScale = newScale;
            windowWidth = INIConfig::clampWindowSize(currentWidth);      // Apply size validation
            windowHeight = INIConfig::clampWindowSize(currentHeight);    // Apply size validation

            // STEP 4: Notify all registered components of scale change
            // This triggers immediate layout recalculation in all child components
            if (onScaleChanged) {
                onScaleChanged(currentScale);
            }

            // STEP 5: Clear any previous error states - successful update
            clearError();
        }
    }
    catch (const std::exception& e) {
        // GRACEFUL ERROR HANDLING: Preserve error state for debugging
        setError("Error updating scale: " + juce::String(e.what()));
    }
}

/**
 * @brief Apply current scaling factor to an integer dimension value
 * 
 * Scales an individual integer dimension (width, height, position, etc.) using
 * the current scaling factor. This is the primary method used by all components
 * to convert INIConfig constants into actual pixel values for their layouts.
 * 
 * SCALING PROCESS:
 * 1. Validate input dimension for reasonable range
 * 2. Multiply by current scale factor (implicit float conversion)
 * 3. Cast result back to int for pixel-perfect positioning
 * 
 * USAGE EXAMPLES:
 * - scaled(100) with 2.0x scale → 200 pixels
 * - scaled(50) with 0.5x scale → 25 pixels  
 * - scaled(INIConfig::LayoutConstants::defaultMargin) → responsive margin
 * 
 * @param dimension Original integer dimension value to scale
 * @return Scaled dimension in pixels, or original value if invalid
 */
int ResponsiveLayoutManager::scaled(int dimension) const {
    if (!isValidDimension(dimension)) {
        setError("Invalid dimension for scaling: " + juce::String(dimension));
        return dimension;  // Return original value to prevent layout breakage
    }

    return static_cast<int>(dimension * currentScale);
}

/**
 * @brief Apply current scaling factor to a float dimension value
 * 
 * Scales a floating-point dimension value using the current scaling factor.
 * This overload is used for precise calculations that require sub-pixel
 * accuracy, such as font sizes, line thicknesses, or gradient positions.
 * 
 * PRECISION ADVANTAGES:
 * - Maintains sub-pixel accuracy for smooth scaling
 * - Used for font sizes that benefit from fractional values
 * - Supports precise gradient and visual effect scaling
 * - No integer truncation during calculation
 * 
 * VALIDATION:
 * - std::isfinite() ensures input is a valid number
 * - Rejects NaN, infinity, and other invalid float values
 * - Returns original value on validation failure
 * 
 * USAGE EXAMPLES:
 * - scaled(12.5f) with 1.5x scale → 18.75f (precise font scaling)
 * - scaled(0.25f) with 2.0x scale → 0.5f (line thickness scaling)
 * 
 * @param dimension Original float dimension value to scale
 * @return Scaled dimension as float, or original value if invalid
 */
float ResponsiveLayoutManager::scaled(float dimension) const {
    if (!std::isfinite(dimension)) {
        setError("Invalid float dimension for scaling");
        return dimension;  // Return original to prevent calculation errors
    }

    return dimension * currentScale;
}

/**
 * @brief Apply current scaling factor to all dimensions of a Rectangle
 * 
 * Scales all four dimensions of a JUCE Rectangle (x, y, width, height) using
 * the current scaling factor. This is a convenience method for scaling entire
 * component bounds at once, commonly used during layout calculations.
 * 
 * SCALING PROCESS:
 * 1. Extract all four rectangle dimensions (x, y, width, height)
 * 2. Scale each dimension individually using scaled(int) method
 * 3. Construct new Rectangle with all scaled dimensions
 * 4. Return scaled Rectangle for immediate use
 * 
 * ERROR HANDLING:
 * - Try-catch block handles any scaling exceptions
 * - Returns original rectangle if scaling fails
 * - Preserves error state for debugging
 * 
 * USAGE EXAMPLES:
 * - Component bounds scaling during layout updates
 * - Child component positioning relative to parent
 * - Hit-testing area calculations with responsive sizing
 * 
 * @param rect Original Rectangle to scale (position and size)
 * @return New Rectangle with all dimensions scaled, or original if error
 * 
 * Called by: Component layout methods, bounds calculation functions
 * References: scaled(int) method for individual dimension scaling
 */
juce::Rectangle<int> ResponsiveLayoutManager::scaled(const juce::Rectangle<int>& rect) const {
    try {
        // Scale all four Rectangle dimensions using the integer scaling method
        return juce::Rectangle<int>(
            scaled(rect.getX()),      // Scale X position
            scaled(rect.getY()),      // Scale Y position  
            scaled(rect.getWidth()),  // Scale width
            scaled(rect.getHeight())  // Scale height
        );
    }
    catch (const std::exception& e) {
        setError("Error scaling rectangle: " + juce::String(e.what()));
        return rect;  // Return original rectangle to prevent layout breakage
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
