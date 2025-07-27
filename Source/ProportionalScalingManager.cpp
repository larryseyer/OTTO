#include "ProportionalScalingManager.h"

ProportionalScalingManager::ProportionalScalingManager()
    : ProportionalScalingManager(ReferenceConfig{})
{
}

ProportionalScalingManager::ProportionalScalingManager(const ReferenceConfig& config)
    : referenceConfig(config)
{
    clearError();
    
    // Initialize with default context
    currentContext.currentWidth = referenceConfig.baseWidth;
    currentContext.currentHeight = referenceConfig.baseHeight;
    currentContext.containerWidth = referenceConfig.baseWidth;
    currentContext.containerHeight = referenceConfig.baseHeight;
    
    // Calculate initial proportional scale
    recalculateProportions();
    
    if (debugMode) {
        debugPrintProportionalState();
    }
}

ProportionalScalingManager::~ProportionalScalingManager()
{
    // Clear callbacks to prevent dangling pointers
    onProportionalScaleChanged = nullptr;
    onDimensionContextChanged = nullptr;
    onBreakpointChanged = nullptr;
}

void ProportionalScalingManager::setReferenceConfig(const ReferenceConfig& config)
{
    try {
        if (config.baseWidth <= 0 || config.baseHeight <= 0) {
            setError("Invalid reference dimensions");
            return;
        }
        
        if (config.minScaleFactor <= 0.0f || config.maxScaleFactor <= 0.0f || 
            config.minScaleFactor >= config.maxScaleFactor) {
            setError("Invalid scale factor range");
            return;
        }
        
        referenceConfig = config;
        recalculateProportions();
        clearError();
        
        if (debugMode) {
            DBG("Reference config updated: " << config.baseWidth << "x" << config.baseHeight);
        }
    }
    catch (const std::exception& e) {
        setError("Error setting reference config: " + juce::String(e.what()));
    }
}

void ProportionalScalingManager::updateDimensions(int windowWidth, int windowHeight, 
                                                 int containerWidth, int containerHeight)
{
    try {
        // Throttle updates for performance
        juce::uint32 currentTime = juce::Time::getMillisecondCounter();
        if (autoRecalculation && (currentTime - lastUpdateTime) < UPDATE_THROTTLE_MS) {
            return;
        }
        lastUpdateTime = currentTime;
        
        // Validate inputs
        if (!isValidDimension(windowWidth) || !isValidDimension(windowHeight)) {
            setError("Invalid window dimensions");
            return;
        }
        
        // Use window dimensions for container if not specified
        if (containerWidth <= 0) containerWidth = windowWidth;
        if (containerHeight <= 0) containerHeight = windowHeight;
        
        // Apply host constraints if present
        if (hasHostConstraints) {
            windowWidth = juce::jmin(windowWidth, maxHostWidth);
            windowHeight = juce::jmin(windowHeight, maxHostHeight);
            containerWidth = juce::jmin(containerWidth, maxHostWidth);
            containerHeight = juce::jmin(containerHeight, maxHostHeight);
        }
        
        // Calculate new dimension context
        DimensionContext newContext = calculateDimensionContext(
            windowWidth, windowHeight, containerWidth, containerHeight);
        
        // Check if significant change occurred
        bool significantChange = 
            std::abs(newContext.proportionalScale - currentContext.proportionalScale) > 0.01f ||
            newContext.isExtraSmall != currentContext.isExtraSmall ||
            newContext.isSmall != currentContext.isSmall ||
            newContext.isMedium != currentContext.isMedium ||
            newContext.isLarge != currentContext.isLarge ||
            newContext.isExtraLarge != currentContext.isExtraLarge;
        
        if (significantChange) {
            // Store previous breakpoint for comparison
            int previousBreakpoint = 0;
            if (currentContext.isExtraSmall) previousBreakpoint = 1;
            else if (currentContext.isSmall) previousBreakpoint = 2;
            else if (currentContext.isMedium) previousBreakpoint = 3;
            else if (currentContext.isLarge) previousBreakpoint = 4;
            else if (currentContext.isExtraLarge) previousBreakpoint = 5;
            
            currentContext = newContext;
            
            // Recalculate scaling
            ProportionalScale newScale = calculateScaleFactors(currentContext);
            applyProportionalScale(newScale);
            
            // Trigger callbacks
            if (onDimensionContextChanged) {
                onDimensionContextChanged(currentContext);
            }
            
            // Check for breakpoint change
            int newBreakpoint = 0;
            if (currentContext.isExtraSmall) newBreakpoint = 1;
            else if (currentContext.isSmall) newBreakpoint = 2;
            else if (currentContext.isMedium) newBreakpoint = 3;
            else if (currentContext.isLarge) newBreakpoint = 4;
            else if (currentContext.isExtraLarge) newBreakpoint = 5;
            
            if (newBreakpoint != previousBreakpoint && onBreakpointChanged) {
                onBreakpointChanged(windowWidth, windowHeight);
            }
            
            if (debugMode) {
                DBG("Dimensions updated: " << windowWidth << "x" << windowHeight << 
                    " Scale: " << currentContext.proportionalScale);
            }
        }
        
        clearError();
    }
    catch (const std::exception& e) {
        setError("Error updating dimensions: " + juce::String(e.what()));
    }
}

void ProportionalScalingManager::updateFromComponent(juce::Component* component)
{
    if (component == nullptr) {
        setError("Null component passed to updateFromComponent");
        return;
    }
    
    auto bounds = component->getBounds();
    auto parent = component->getParentComponent();
    
    if (parent) {
        auto parentBounds = parent->getBounds();
        updateDimensions(bounds.getWidth(), bounds.getHeight(), 
                        parentBounds.getWidth(), parentBounds.getHeight());
    } else {
        updateDimensions(bounds.getWidth(), bounds.getHeight());
    }
}

ProportionalScalingManager::ProportionalScale ProportionalScalingManager::calculateProportionalScale(
    int width, int height) const
{
    DimensionContext context = calculateDimensionContext(width, height, width, height);
    return calculateScaleFactors(context);
}

void ProportionalScalingManager::applyProportionalScale(const ProportionalScale& scale)
{
    if (!isValidScale(scale.baseScale)) {
        setError("Invalid base scale factor");
        return;
    }
    
    ProportionalScale previousScale = currentScale;
    currentScale = scale;
    
    // Trigger callback if scale changed significantly
    bool scaleChanged = std::abs(scale.baseScale - previousScale.baseScale) > 0.01f;
    if (scaleChanged && onProportionalScaleChanged) {
        onProportionalScaleChanged(currentScale);
    }
    
    if (debugMode && scaleChanged) {
        DBG("Proportional scale applied - Base: " << scale.baseScale << 
            " Text: " << scale.textScale << " Touch: " << scale.touchTargetScale);
    }
}

int ProportionalScalingManager::scaledProportional(int dimension) const
{
    if (!isValidDimension(dimension)) {
        setError("Invalid dimension for proportional scaling");
        return dimension;
    }
    
    return applyDimensionConstraints(
        static_cast<int>(dimension * currentScale.baseScale * currentScale.dpiMultiplier));
}

float ProportionalScalingManager::scaledProportional(float dimension) const
{
    if (!std::isfinite(dimension)) {
        setError("Invalid float dimension for proportional scaling");
        return dimension;
    }
    
    return dimension * currentScale.baseScale * currentScale.dpiMultiplier;
}

juce::Point<int> ProportionalScalingManager::scaledProportional(const juce::Point<int>& point) const
{
    return juce::Point<int>(scaledProportional(point.getX()), scaledProportional(point.getY()));
}

juce::Rectangle<int> ProportionalScalingManager::scaledProportional(const juce::Rectangle<int>& rect) const
{
    try {
        return juce::Rectangle<int>(
            scaledProportional(rect.getX()),
            scaledProportional(rect.getY()),
            scaledProportional(rect.getWidth()),
            scaledProportional(rect.getHeight())
        );
    }
    catch (const std::exception& e) {
        setError("Error scaling rectangle proportionally: " + juce::String(e.what()));
        return rect;
    }
}

juce::Font ProportionalScalingManager::scaledProportional(const juce::Font& font) const
{
    try {
        float newHeight = font.getHeight() * currentScale.textScale * currentScale.dpiMultiplier;
        return font.withHeight(juce::jmax(8.0f, newHeight)); // Minimum readable size
    }
    catch (const std::exception& e) {
        setError("Error scaling font proportionally: " + juce::String(e.what()));
        return font;
    }
}

int ProportionalScalingManager::scaledElement(int baseDimension) const
{
    return applyDimensionConstraints(
        static_cast<int>(baseDimension * currentScale.elementScale * currentScale.dpiMultiplier));
}

int ProportionalScalingManager::scaledText(int baseFontSize) const
{
    int scaledSize = static_cast<int>(baseFontSize * currentScale.textScale * currentScale.dpiMultiplier);
    return juce::jmax(8, scaledSize); // Ensure minimum readable size
}

int ProportionalScalingManager::scaledIcon(int baseIconSize) const
{
    return applyDimensionConstraints(
        static_cast<int>(baseIconSize * currentScale.iconScale * currentScale.dpiMultiplier));
}

int ProportionalScalingManager::scaledSpacing(int baseSpacing) const
{
    return applyDimensionConstraints(
        static_cast<int>(baseSpacing * currentScale.spacingScale * currentScale.dpiMultiplier));
}

int ProportionalScalingManager::scaledBorder(int baseBorderWidth) const
{
    int scaledBorder = static_cast<int>(baseBorderWidth * currentScale.borderScale * currentScale.dpiMultiplier);
    return juce::jmax(1, scaledBorder); // Ensure minimum visible border
}

int ProportionalScalingManager::scaledTouchTarget(int baseSize) const
{
    int scaledSize = static_cast<int>(baseSize * currentScale.touchTargetScale * currentScale.dpiMultiplier);
    return juce::jmax(minTouchTarget(), scaledSize);
}

int ProportionalScalingManager::minTouchTarget() const
{
    if (currentContext.needsTouchOptimization) {
        // Use platform-appropriate minimum
        if (platformManager) {
            auto platform = platformManager->getPlatformConfig().platform;
            if (platform == PlatformResponsiveManager::TargetPlatform::iOS_Standalone ||
                platform == PlatformResponsiveManager::TargetPlatform::iOS_AUv3) {
                return static_cast<int>(referenceConfig.minTouchTargetPx * currentScale.dpiMultiplier);
            } else if (platform == PlatformResponsiveManager::TargetPlatform::Android_Standalone) {
                return static_cast<int>(referenceConfig.minTouchTargetDp * currentScale.dpiMultiplier);
            }
        }
        
        // Default touch minimum
        return juce::jmax(
            static_cast<int>(referenceConfig.minTouchTargetPx * currentScale.dpiMultiplier),
            static_cast<int>(referenceConfig.minTouchTargetDp * currentScale.dpiMultiplier)
        );
    }
    
    return 0; // No minimum for non-touch interfaces
}

bool ProportionalScalingManager::isTouchOptimal(int size) const
{
    return size >= minTouchTarget();
}

juce::Rectangle<int> ProportionalScalingManager::ensureTouchTarget(const juce::Rectangle<int>& bounds) const
{
    if (!currentContext.needsTouchOptimization) {
        return bounds;
    }
    
    int minSize = minTouchTarget();
    int width = juce::jmax(bounds.getWidth(), minSize);
    int height = juce::jmax(bounds.getHeight(), minSize);
    
    // Center the expanded bounds
    int x = bounds.getX() - (width - bounds.getWidth()) / 2;
    int y = bounds.getY() - (height - bounds.getHeight()) / 2;
    
    return juce::Rectangle<int>(x, y, width, height);
}

juce::Rectangle<int> ProportionalScalingManager::getProportionalBounds(
    int containerWidth, int containerHeight) const
{
    try {
        int scaledWidth = scaledProportional(referenceConfig.baseWidth);
        int scaledHeight = scaledProportional(referenceConfig.baseHeight);
        
        // Maintain aspect ratio if needed
        if (containerWidth > 0 && containerHeight > 0) {
            float containerAspect = static_cast<float>(containerWidth) / containerHeight;
            float targetAspect = referenceConfig.aspectRatio;
            
            if (containerAspect > targetAspect) {
                // Container is wider - fit to height
                scaledHeight = juce::jmin(scaledHeight, containerHeight);
                scaledWidth = static_cast<int>(scaledHeight * targetAspect);
            } else {
                // Container is taller - fit to width
                scaledWidth = juce::jmin(scaledWidth, containerWidth);
                scaledHeight = static_cast<int>(scaledWidth / targetAspect);
            }
        }
        
        return juce::Rectangle<int>(0, 0, scaledWidth, scaledHeight);
    }
    catch (const std::exception& e) {
        setError("Error calculating proportional bounds: " + juce::String(e.what()));
        return juce::Rectangle<int>(0, 0, referenceConfig.baseWidth, referenceConfig.baseHeight);
    }
}

juce::Rectangle<int> ProportionalScalingManager::getCenteredProportionalBounds(
    int containerWidth, int containerHeight) const
{
    auto bounds = getProportionalBounds(containerWidth, containerHeight);
    
    int offsetX = (containerWidth - bounds.getWidth()) / 2;
    int offsetY = (containerHeight - bounds.getHeight()) / 2;
    
    return bounds.withPosition(juce::jmax(0, offsetX), juce::jmax(0, offsetY));
}

void ProportionalScalingManager::setupProportionalConstrainer(juce::ComponentBoundsConstrainer& constrainer)
{
    try {
        constrainer.setFixedAspectRatio(referenceConfig.aspectRatio);
        
        int minWidth = static_cast<int>(referenceConfig.baseWidth * referenceConfig.minScaleFactor);
        int minHeight = static_cast<int>(referenceConfig.baseHeight * referenceConfig.minScaleFactor);
        int maxWidth = static_cast<int>(referenceConfig.baseWidth * referenceConfig.maxScaleFactor);
        int maxHeight = static_cast<int>(referenceConfig.baseHeight * referenceConfig.maxScaleFactor);
        
        // Apply host constraints if present
        if (hasHostConstraints) {
            maxWidth = juce::jmin(maxWidth, maxHostWidth);
            maxHeight = juce::jmin(maxHeight, maxHostHeight);
        }
        
        constrainer.setSizeLimits(minWidth, minHeight, maxWidth, maxHeight);
        clearError();
    }
    catch (const std::exception& e) {
        setError("Error setting up proportional constrainer: " + juce::String(e.what()));
    }
}

int ProportionalScalingManager::getRecommendedColumns() const
{
    if (currentContext.isExtraSmall) return 1;
    if (currentContext.isSmall) return 2;
    if (currentContext.isMedium) return 3;
    if (currentContext.isLarge) return 4;
    return 5; // Extra large
}

int ProportionalScalingManager::getRecommendedRows() const
{
    // Recommend fewer rows for smaller screens
    if (currentContext.isExtraSmall) return 2;
    if (currentContext.isSmall) return 3;
    if (currentContext.isMedium) return 4;
    return 5; // Large and extra large
}

bool ProportionalScalingManager::shouldUseCompactLayout() const
{
    return currentContext.isExtraSmall || currentContext.isSmall || 
           currentContext.needsTouchOptimization;
}

bool ProportionalScalingManager::shouldStackVertically() const
{
    return currentContext.isExtraSmall || 
           (currentContext.isSmall && currentContext.aspectRatio < 1.0f);
}

bool ProportionalScalingManager::shouldShowScrollbars() const
{
    // Show scrollbars on touch devices or small screens
    return currentContext.needsTouchOptimization || currentContext.isExtraSmall;
}

void ProportionalScalingManager::setPluginHostConstraints(int maxWidth, int maxHeight, bool isResizable)
{
    hasHostConstraints = true;
    maxHostWidth = maxWidth;
    maxHostHeight = maxHeight;
    hostAllowsResize = isResizable;
    
    // Recalculate with new constraints
    if (autoRecalculation) {
        recalculateProportions();
    }
    
    if (debugMode) {
        DBG("Plugin host constraints set: " << maxWidth << "x" << maxHeight << 
            " Resizable: " << (isResizable ? "Yes" : "No"));
    }
}

void ProportionalScalingManager::adaptToHostEnvironment()
{
    if (!hasHostConstraints) return;
    
    // Adapt reference config to fit within host constraints
    float hostAspectRatio = static_cast<float>(maxHostWidth) / maxHostHeight;
    
    if (std::abs(hostAspectRatio - referenceConfig.aspectRatio) > 0.1f) {
        // Adjust base dimensions to better fit host
        if (hostAspectRatio > referenceConfig.aspectRatio) {
            // Host is wider - adjust width
            int adjustedWidth = static_cast<int>(referenceConfig.baseHeight * hostAspectRatio);
            if (adjustedWidth <= maxHostWidth) {
                ReferenceConfig adjustedConfig = referenceConfig;
                adjustedConfig.baseWidth = adjustedWidth;
                adjustedConfig.aspectRatio = hostAspectRatio;
                setReferenceConfig(adjustedConfig);
            }
        } else {
            // Host is taller - adjust height
            int adjustedHeight = static_cast<int>(referenceConfig.baseWidth / hostAspectRatio);
            if (adjustedHeight <= maxHostHeight) {
                ReferenceConfig adjustedConfig = referenceConfig;
                adjustedConfig.baseHeight = adjustedHeight;
                adjustedConfig.aspectRatio = hostAspectRatio;
                setReferenceConfig(adjustedConfig);
            }
        }
    }
}

void ProportionalScalingManager::enableAutoRecalculation(bool enabled)
{
    autoRecalculation = enabled;
    if (enabled) {
        recalculateProportions();
    }
}

void ProportionalScalingManager::recalculateProportions()
{
    try {
        DimensionContext newContext = calculateDimensionContext(
            currentContext.currentWidth, 
            currentContext.currentHeight,
            currentContext.containerWidth, 
            currentContext.containerHeight
        );
        
        ProportionalScale newScale = calculateScaleFactors(newContext);
        
        currentContext = newContext;
        applyProportionalScale(newScale);
        
        clearError();
    }
    catch (const std::exception& e) {
        setError("Error recalculating proportions: " + juce::String(e.what()));
    }
}

void ProportionalScalingManager::forceUpdate()
{
    lastUpdateTime = 0; // Reset throttle
    recalculateProportions();
}

void ProportionalScalingManager::updateDPIScale()
{
    if (platformManager) {
        float systemDPI = platformManager->getSystemDPIScale();
        if (systemDPI > 0.0f) {
            // Apply DPI scale to current scale
            ProportionalScale updatedScale = currentScale;
            updatedScale.dpiMultiplier = systemDPI;
            applyProportionalScale(updatedScale);
        }
    }
}

void ProportionalScalingManager::updateScreenDensity()
{
    if (platformManager) {
        auto density = platformManager->getPlatformConfig().density;
        float densityMultiplier = 1.0f;
        
        switch (density) {
            case PlatformResponsiveManager::ScreenDensity::LDPI:   densityMultiplier = 0.75f; break;
            case PlatformResponsiveManager::ScreenDensity::MDPI:   densityMultiplier = 1.0f;  break;
            case PlatformResponsiveManager::ScreenDensity::HDPI:   densityMultiplier = 1.5f;  break;
            case PlatformResponsiveManager::ScreenDensity::XHDPI:  densityMultiplier = 2.0f;  break;
            case PlatformResponsiveManager::ScreenDensity::XXHDPI: densityMultiplier = 3.0f;  break;
            case PlatformResponsiveManager::ScreenDensity::XXXHDPI:densityMultiplier = 4.0f;  break;
        }
        
        ProportionalScale updatedScale = currentScale;
        updatedScale.densityMultiplier = densityMultiplier;
        applyProportionalScale(updatedScale);
    }
}

float ProportionalScalingManager::getEffectiveDPIScale() const
{
    return currentScale.dpiMultiplier * currentScale.densityMultiplier;
}

PlatformResponsiveManager::ScreenDensity ProportionalScalingManager::getCurrentDensity() const
{
    if (platformManager) {
        return platformManager->getPlatformConfig().density;
    }
    return PlatformResponsiveManager::ScreenDensity::MDPI;
}

juce::ValueTree ProportionalScalingManager::saveProportionalState() const
{
    juce::ValueTree state("ProportionalScalingState");
    
    // Save reference config
    juce::ValueTree refConfig("ReferenceConfig");
    refConfig.setProperty("baseWidth", referenceConfig.baseWidth, nullptr);
    refConfig.setProperty("baseHeight", referenceConfig.baseHeight, nullptr);
    refConfig.setProperty("aspectRatio", referenceConfig.aspectRatio, nullptr);
    refConfig.setProperty("minScaleFactor", referenceConfig.minScaleFactor, nullptr);
    refConfig.setProperty("maxScaleFactor", referenceConfig.maxScaleFactor, nullptr);
    state.appendChild(refConfig, nullptr);
    
    // Save current context
    juce::ValueTree context("DimensionContext");
    context.setProperty("currentWidth", currentContext.currentWidth, nullptr);
    context.setProperty("currentHeight", currentContext.currentHeight, nullptr);
    context.setProperty("proportionalScale", currentContext.proportionalScale, nullptr);
    state.appendChild(context, nullptr);
    
    // Save current scale
    juce::ValueTree scale("ProportionalScale");
    scale.setProperty("baseScale", currentScale.baseScale, nullptr);
    scale.setProperty("textScale", currentScale.textScale, nullptr);
    scale.setProperty("touchTargetScale", currentScale.touchTargetScale, nullptr);
    scale.setProperty("dpiMultiplier", currentScale.dpiMultiplier, nullptr);
    state.appendChild(scale, nullptr);
    
    return state;
}

void ProportionalScalingManager::loadProportionalState(const juce::ValueTree& state)
{
    try {
        if (!state.isValid() || state.getType() != juce::Identifier("ProportionalScalingState")) {
            setError("Invalid proportional scaling state");
            return;
        }
        
        // Load reference config
        auto refConfig = state.getChildWithName("ReferenceConfig");
        if (refConfig.isValid()) {
            ReferenceConfig config;
            config.baseWidth = refConfig.getProperty("baseWidth", config.baseWidth);
            config.baseHeight = refConfig.getProperty("baseHeight", config.baseHeight);
            config.aspectRatio = refConfig.getProperty("aspectRatio", config.aspectRatio);
            config.minScaleFactor = refConfig.getProperty("minScaleFactor", config.minScaleFactor);
            config.maxScaleFactor = refConfig.getProperty("maxScaleFactor", config.maxScaleFactor);
            setReferenceConfig(config);
        }
        
        // Load and apply context
        auto contextTree = state.getChildWithName("DimensionContext");
        if (contextTree.isValid()) {
            int width = contextTree.getProperty("currentWidth", currentContext.currentWidth);
            int height = contextTree.getProperty("currentHeight", currentContext.currentHeight);
            updateDimensions(width, height);
        }
        
        clearError();
    }
    catch (const std::exception& e) {
        setError("Error loading proportional state: " + juce::String(e.what()));
        resetToReferenceDefaults();
    }
}

void ProportionalScalingManager::resetToReferenceDefaults()
{
    // Reset to default reference config
    ReferenceConfig defaultConfig;
    referenceConfig = defaultConfig;
    
    // Reset context
    currentContext = DimensionContext{};
    currentContext.currentWidth = referenceConfig.baseWidth;
    currentContext.currentHeight = referenceConfig.baseHeight;
    currentContext.containerWidth = referenceConfig.baseWidth;
    currentContext.containerHeight = referenceConfig.baseHeight;
    
    // Reset scale
    currentScale = ProportionalScale{};
    
    // Recalculate
    recalculateProportions();
    clearError();
}

bool ProportionalScalingManager::validateConfiguration() const
{
    try {
        // Validate reference config
        if (referenceConfig.baseWidth <= 0 || referenceConfig.baseHeight <= 0) {
            return false;
        }
        
        if (referenceConfig.aspectRatio <= 0.0f) {
            return false;
        }
        
        if (referenceConfig.minScaleFactor <= 0.0f || 
            referenceConfig.maxScaleFactor <= 0.0f ||
            referenceConfig.minScaleFactor >= referenceConfig.maxScaleFactor) {
            return false;
        }
        
        // Validate current scale
        if (!isValidScale(currentScale.baseScale) ||
            !isValidScale(currentScale.textScale) ||
            !isValidScale(currentScale.touchTargetScale)) {
            return false;
        }
        
        return true;
    }
    catch (const std::exception& e) {
        setError("Error validating configuration: " + juce::String(e.what()));
        return false;
    }
}

void ProportionalScalingManager::clearError()
{
    lastError.clear();
}

void ProportionalScalingManager::integrateWithResponsiveManager(ResponsiveLayoutManager* manager)
{
    responsiveManager = manager;
    
    if (manager && autoRecalculation) {
        // Sync with responsive manager's scale
        float responsiveScale = manager->getCurrentScale();
        if (responsiveScale > 0.0f) {
            ProportionalScale updatedScale = currentScale;
            updatedScale.baseScale *= responsiveScale;
            applyProportionalScale(updatedScale);
        }
    }
}

void ProportionalScalingManager::integrateWithPlatformManager(PlatformResponsiveManager* manager)
{
    platformManager = manager;
    
    if (manager && autoRecalculation) {
        updateDPIScale();
        updateScreenDensity();
    }
}

void ProportionalScalingManager::debugPrintProportionalState() const
{
    if (!debugMode) return;
    
    DBG("=== Proportional Scaling State ===");
    DBG("Reference: " << referenceConfig.baseWidth << "x" << referenceConfig.baseHeight);
    DBG("Current: " << currentContext.currentWidth << "x" << currentContext.currentHeight);
    DBG("Proportional Scale: " << currentContext.proportionalScale);
    DBG("Base Scale: " << currentScale.baseScale);
    DBG("Text Scale: " << currentScale.textScale);
    DBG("Touch Scale: " << currentScale.touchTargetScale);
    DBG("DPI Multiplier: " << currentScale.dpiMultiplier);
    
    juce::String breakpoint = "Unknown";
    if (currentContext.isExtraSmall) breakpoint = "Extra Small";
    else if (currentContext.isSmall) breakpoint = "Small";
    else if (currentContext.isMedium) breakpoint = "Medium";
    else if (currentContext.isLarge) breakpoint = "Large";
    else if (currentContext.isExtraLarge) breakpoint = "Extra Large";
    DBG("Breakpoint: " << breakpoint);
    
    DBG("Touch Optimization: " << (currentContext.needsTouchOptimization ? "Yes" : "No"));
    DBG("Host Constraints: " << (hasHostConstraints ? "Yes" : "No"));
    if (hasHostConstraints) {
        DBG("Max Host Size: " << maxHostWidth << "x" << maxHostHeight);
    }
    
    if (hasError()) {
        DBG("Last Error: " << lastError);
    }
    
    DBG("==================================");
}

juce::String ProportionalScalingManager::getProportionalStateString() const
{
    juce::String state;
    state << "Proportional Scaling - ";
    state << "Scale: " << juce::String(currentContext.proportionalScale, 2) << ", ";
    state << "Size: " << currentContext.currentWidth << "x" << currentContext.currentHeight << ", ";
    
    if (currentContext.isExtraSmall) state << "Extra Small";
    else if (currentContext.isSmall) state << "Small";
    else if (currentContext.isMedium) state << "Medium";
    else if (currentContext.isLarge) state << "Large";
    else if (currentContext.isExtraLarge) state << "Extra Large";
    
    if (currentContext.needsTouchOptimization) {
        state << " (Touch)";
    }
    
    return state;
}

// Private implementation methods

ProportionalScalingManager::DimensionContext ProportionalScalingManager::calculateDimensionContext(
    int width, int height, int containerWidth, int containerHeight) const
{
    DimensionContext context;
    
    context.currentWidth = width;
    context.currentHeight = height;
    context.containerWidth = containerWidth;
    context.containerHeight = containerHeight;
    context.aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    // Calculate scaling factors
    context.widthScale = static_cast<float>(width) / static_cast<float>(referenceConfig.baseWidth);
    context.heightScale = static_cast<float>(height) / static_cast<float>(referenceConfig.baseHeight);
    context.uniformScale = juce::jmin(context.widthScale, context.heightScale);
    
    // Apply constraints to get final proportional scale
    context.proportionalScale = applyScaleConstraints(context.uniformScale);
    
    // Update breakpoint flags
    updateBreakpointFlags(context);
    
    // Determine touch optimization needs
    context.needsTouchOptimization = shouldOptimizeForTouch(context);
    
    // Platform capabilities (from platform manager if available)
    if (platformManager) {
        auto platformConfig = platformManager->getPlatformConfig();
        context.hasPhysicalKeyboard = platformConfig.hasPhysicalKeyboard;
        context.supportsHover = platformConfig.hasHoverCapability;
    }
    
    return context;
}

ProportionalScalingManager::ProportionalScale ProportionalScalingManager::calculateScaleFactors(
    const DimensionContext& context) const
{
    ProportionalScale scale;
    
    // Base scale from proportional calculation
    scale.baseScale = context.proportionalScale;
    
    // Element scaling (slightly reduced for smaller screens)
    scale.elementScale = context.proportionalScale;
    if (context.isExtraSmall || context.isSmall) {
        scale.elementScale *= 0.9f; // Slightly smaller elements for compact layouts
    }
    
    // Text scaling (maintain readability)
    scale.textScale = context.proportionalScale;
    if (context.isExtraSmall) {
        scale.textScale = juce::jmax(0.8f, scale.textScale); // Don't go too small
    }
    
    // Icon scaling (can be slightly larger than text)
    scale.iconScale = context.proportionalScale;
    if (context.needsTouchOptimization) {
        scale.iconScale *= 1.1f; // Slightly larger icons for touch
    }
    
    // Spacing scaling (reduce for compact layouts)
    scale.spacingScale = context.proportionalScale;
    if (context.isExtraSmall || context.isSmall) {
        scale.spacingScale *= 0.8f; // Tighter spacing for small screens
    }
    
    // Touch target scaling
    scale.touchTargetScale = calculateTouchTargetScale(context);
    
    // Border scaling (don't scale borders as much)
    scale.borderScale = juce::jmin(1.5f, context.proportionalScale);
    
    // Apply DPI and platform adjustments
    applyDPIAdjustments(scale);
    applyPlatformAdjustments(scale);
    
    return scale;
}

void ProportionalScalingManager::updateBreakpointFlags(DimensionContext& context) const
{
    int minDimension = juce::jmin(context.currentWidth, context.currentHeight);
    
    context.isExtraSmall = minDimension <= referenceConfig.extraSmallThreshold;
    context.isSmall = minDimension <= referenceConfig.smallThreshold && !context.isExtraSmall;
    context.isMedium = minDimension <= referenceConfig.mediumThreshold && !context.isSmall && !context.isExtraSmall;
    context.isLarge = minDimension <= referenceConfig.largeThreshold && !context.isMedium && !context.isSmall && !context.isExtraSmall;
    context.isExtraLarge = minDimension > referenceConfig.largeThreshold;
}

void ProportionalScalingManager::applyDPIAdjustments(ProportionalScale& scale) const
{
    if (platformManager) {
        scale.dpiMultiplier = platformManager->getSystemDPIScale();
    } else {
        // Fallback DPI detection
        scale.dpiMultiplier = juce::Desktop::getInstance().getGlobalScaleFactor();
    }
    
    // Ensure reasonable DPI range
    scale.dpiMultiplier = juce::jlimit(0.5f, 4.0f, scale.dpiMultiplier);
}

void ProportionalScalingManager::applyPlatformAdjustments(ProportionalScale& scale) const
{
    if (!platformManager) return;
    
    auto platform = platformManager->getPlatformConfig().platform;
    
    switch (platform) {
        case PlatformResponsiveManager::TargetPlatform::iOS_Standalone:
        case PlatformResponsiveManager::TargetPlatform::iOS_AUv3:
            // iOS prefers slightly larger touch targets
            scale.touchTargetScale *= 1.1f;
            break;
            
        case PlatformResponsiveManager::TargetPlatform::Android_Standalone:
            // Android Material Design guidelines
            scale.touchTargetScale *= 1.0f; // Standard
            scale.spacingScale *= 1.1f; // Slightly more spacing
            break;
            
        case PlatformResponsiveManager::TargetPlatform::macOS_VST3:
        case PlatformResponsiveManager::TargetPlatform::Windows_VST3:
            // Plugin contexts might need more compact layouts
            scale.spacingScale *= 0.9f;
            break;
            
        default:
            break;
    }
}

float ProportionalScalingManager::applyScaleConstraints(float scale) const
{
    return juce::jlimit(referenceConfig.minScaleFactor, referenceConfig.maxScaleFactor, scale);
}

int ProportionalScalingManager::applyDimensionConstraints(int dimension) const
{
    return juce::jmax(1, dimension); // Ensure minimum 1px
}

bool ProportionalScalingManager::shouldOptimizeForTouch(const DimensionContext& context) const
{
    if (platformManager) {
        return platformManager->getPlatformConfig().hasTouchInput;
    }
    
    // Fallback: assume touch optimization needed for small screens
    return context.isExtraSmall || context.isSmall;
}

float ProportionalScalingManager::calculateTouchTargetScale(const DimensionContext& context) const
{
    if (!context.needsTouchOptimization) {
        return context.proportionalScale;
    }
    
    // Calculate scale needed to achieve minimum touch targets
    float minScale = static_cast<float>(referenceConfig.minTouchTargetPx) / 
                    static_cast<float>(referenceConfig.baseHeight / 10); // Rough estimate
    
    return juce::jmax(context.proportionalScale, minScale);
}

bool ProportionalScalingManager::isValidDimension(int dimension) const
{
    return dimension > 0 && dimension <= 32768; // Reasonable maximum
}

bool ProportionalScalingManager::isValidScale(float scale) const
{
    return std::isfinite(scale) && scale > 0.0f && scale <= 10.0f;
}

void ProportionalScalingManager::setError(const juce::String& error) const
{
    lastError = error;
    if (debugMode) {
        DBG("ProportionalScalingManager Error: " << error);
    }
}

// ComponentListener implementation for JUCE 8

void ProportionalScalingManager::componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
{
    if (wasResized && autoRecalculation) {
        updateFromComponent(&component);
    }
}

void ProportionalScalingManager::componentParentHierarchyChanged(juce::Component& component)
{
    // Update when component hierarchy changes (e.g., added to different parent)
    if (autoRecalculation) {
        updateFromComponent(&component);
    }
}

// ResponsiveMigrationHelper implementation

void ResponsiveMigrationHelper::enhanceExistingComponentProportional(
    juce::Component* component, ProportionalScalingManager* manager)
{
    if (!component || !manager) return;
    
    // Convert existing bounds to proportional
    auto bounds = component->getBounds();
    auto proportionalBounds = convertToProportional(bounds, manager);
    component->setBounds(proportionalBounds);
    
    // Set up automatic updates
    manager->updateFromComponent(component);
}

juce::Rectangle<int> ResponsiveMigrationHelper::convertToProportional(
    const juce::Rectangle<int>& fixedBounds, ProportionalScalingManager* manager)
{
    if (!manager) return fixedBounds;
    
    return manager->scaledProportional(fixedBounds);
}

juce::Font ResponsiveMigrationHelper::convertToProportional(
    const juce::Font& fixedFont, ProportionalScalingManager* manager)
{
    if (!manager) return fixedFont;
    
    return manager->scaledProportional(fixedFont);
}

juce::Array<juce::Rectangle<int>> ResponsiveMigrationHelper::convertBoundsToProportional(
    const juce::Array<juce::Rectangle<int>>& fixedBounds, ProportionalScalingManager* manager)
{
    juce::Array<juce::Rectangle<int>> proportionalBounds;
    
    for (auto& bounds : fixedBounds) {
        proportionalBounds.add(convertToProportional(bounds, manager));
    }
    
    return proportionalBounds;
}

// Global singleton implementation

ProportionalScalingManager& ProportionalScaling::getInstance()
{
    static ProportionalScalingManager instance;
    return instance;
}