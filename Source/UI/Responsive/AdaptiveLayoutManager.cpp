#include "AdaptiveLayoutManager.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "INIDataManager.h"
#include <algorithm>
#include <cmath>

//==============================================================================
// AdaptiveLayoutManager Implementation
//==============================================================================

AdaptiveLayoutManager::AdaptiveLayoutManager(const LayoutConfig& config)
    : ResponsiveLayoutManager(config)
{
    // Initialize default breakpoints and priorities
    initializeDefaultBreakpoints();
    initializeDefaultPriorities();
    
    // Load settings from INI
    ComponentState state;
    if (INIDataManager::loadComponentState("AdaptiveLayoutManager", state)) {
        loadState(state);
    }
    
    // Set initial device type and orientation
    auto bounds = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea;
    currentDeviceType = detectDeviceType(bounds.getWidth(), bounds.getHeight());
    currentOrientation = detectOrientation(bounds.getWidth(), bounds.getHeight());
    currentBreakpoint = findBreakpoint(bounds.getWidth(), bounds.getHeight());
    
    // Enable touch optimization on mobile platforms
    #if JUCE_IOS || JUCE_ANDROID
    touchOptimization = true;
    settings.enableTouchOptimization = true;
    #endif
}

AdaptiveLayoutManager::~AdaptiveLayoutManager() {
    // Save current state
    ComponentState state;
    saveState(state);
    INIDataManager::saveComponentState("AdaptiveLayoutManager", state);
}

//==============================================================================
// Breakpoint Management
//==============================================================================

void AdaptiveLayoutManager::addBreakpoint(const Breakpoint& breakpoint) {
    if (!isValidBreakpoint(breakpoint)) {
        return;
    }
    
    // Remove existing breakpoint with same name
    removeBreakpoint(breakpoint.name);
    
    // Add new breakpoint
    breakpoints.push_back(breakpoint);
    
    // Sort breakpoints by min width
    std::sort(breakpoints.begin(), breakpoints.end(),
              [](const Breakpoint& a, const Breakpoint& b) {
                  return a.minWidth < b.minWidth;
              });
    
    // Invalidate cache
    invalidateLayoutCache();
}

void AdaptiveLayoutManager::removeBreakpoint(const juce::String& name) {
    breakpoints.erase(
        std::remove_if(breakpoints.begin(), breakpoints.end(),
                      [&name](const Breakpoint& bp) {
                          return bp.name == name;
                      }),
        breakpoints.end()
    );
    
    invalidateLayoutCache();
}

void AdaptiveLayoutManager::clearBreakpoints() {
    breakpoints.clear();
    invalidateLayoutCache();
}

juce::Array<AdaptiveLayoutManager::Breakpoint> AdaptiveLayoutManager::getBreakpoints() const {
    juce::Array<Breakpoint> result;
    for (const auto& bp : breakpoints) {
        result.add(bp);
    }
    return result;
}

AdaptiveLayoutManager::Breakpoint AdaptiveLayoutManager::getCurrentBreakpoint() const {
    return currentBreakpoint;
}

AdaptiveLayoutManager::Breakpoint AdaptiveLayoutManager::findBreakpoint(int width, int height) const {
    // Find the best matching breakpoint
    for (const auto& bp : breakpoints) {
        if (bp.matches(width, height)) {
            return bp;
        }
    }
    
    // Return default breakpoint if no match found
    Breakpoint defaultBp;
    defaultBp.name = "default";
    defaultBp.deviceType = detectDeviceType(width, height);
    defaultBp.layoutMode = LayoutMode::Full;
    return defaultBp;
}

//==============================================================================
// Device and Orientation Detection
//==============================================================================

AdaptiveLayoutManager::DeviceType AdaptiveLayoutManager::detectDeviceType(int width, int height) const {
    int minDimension = juce::jmin(width, height);
    int maxDimension = juce::jmax(width, height);
    
    if (maxDimension < 600) {
        return DeviceType::Phone;
    } else if (maxDimension < 1200) {
        return DeviceType::Tablet;
    } else if (maxDimension < 1920) {
        return DeviceType::Desktop;
    } else {
        return DeviceType::TV;
    }
}

AdaptiveLayoutManager::Orientation AdaptiveLayoutManager::detectOrientation(int width, int height) const {
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    if (aspectRatio > 1.1f) {
        return Orientation::Landscape;
    } else if (aspectRatio < 0.9f) {
        return Orientation::Portrait;
    } else {
        return Orientation::Square;
    }
}

//==============================================================================
// Layout Mode Management
//==============================================================================

void AdaptiveLayoutManager::setLayoutMode(LayoutMode mode) {
    if (mode != currentLayoutMode) {
        currentLayoutMode = mode;
        invalidateLayoutCache();
        notifyLayoutModeChanged();
    }
}

void AdaptiveLayoutManager::setAutoLayoutMode(bool enabled) {
    autoLayoutMode = enabled;
    
    if (enabled) {
        // Automatically determine layout mode based on current constraints
        auto bounds = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea;
        auto deviceType = detectDeviceType(bounds.getWidth(), bounds.getHeight());
        
        switch (deviceType) {
            case DeviceType::Phone:
                setLayoutMode(LayoutMode::Compact);
                break;
            case DeviceType::Tablet:
                setLayoutMode(touchOptimization ? LayoutMode::Touch : LayoutMode::Full);
                break;
            case DeviceType::Desktop:
            case DeviceType::TV:
                setLayoutMode(performanceMode ? LayoutMode::Performance : LayoutMode::Full);
                break;
            default:
                setLayoutMode(LayoutMode::Full);
                break;
        }
    }
}

//==============================================================================
// Component Priority System
//==============================================================================

void AdaptiveLayoutManager::setComponentPriority(const juce::String& componentId, const ComponentPriority& priority) {
    if (!isValidComponentPriority(priority)) {
        return;
    }
    
    componentPriorities[componentId] = priority;
    invalidateLayoutCache();
}

AdaptiveLayoutManager::ComponentPriority AdaptiveLayoutManager::getComponentPriority(const juce::String& componentId) const {
    auto it = componentPriorities.find(componentId);
    if (it != componentPriorities.end()) {
        return it->second;
    }
    
    // Return default priority
    ComponentPriority defaultPriority;
    defaultPriority.componentId = componentId;
    defaultPriority.priority = 5; // Medium priority
    return defaultPriority;
}

void AdaptiveLayoutManager::removeComponentPriority(const juce::String& componentId) {
    componentPriorities.erase(componentId);
    invalidateLayoutCache();
}

juce::Array<AdaptiveLayoutManager::ComponentPriority> AdaptiveLayoutManager::getComponentPriorities() const {
    juce::Array<ComponentPriority> result;
    for (const auto& pair : componentPriorities) {
        result.add(pair.second);
    }
    return result;
}

//==============================================================================
// Adaptive Layout Calculation
//==============================================================================

void AdaptiveLayoutManager::updateLayout(int width, int height) {
    auto startTime = juce::Time::getCurrentTime();
    
    // Detect device type and orientation changes
    auto newDeviceType = detectDeviceType(width, height);
    auto newOrientation = detectOrientation(width, height);
    auto newBreakpoint = findBreakpoint(width, height);
    
    bool deviceChanged = (newDeviceType != currentDeviceType);
    bool orientationChanged = (newOrientation != currentOrientation);
    bool breakpointChanged = (newBreakpoint.name != currentBreakpoint.name);
    
    // Update current state
    currentDeviceType = newDeviceType;
    currentOrientation = newOrientation;
    currentBreakpoint = newBreakpoint;
    
    // Auto-adjust layout mode if enabled
    if (autoLayoutMode) {
        LayoutMode newMode = currentLayoutMode;
        
        switch (currentDeviceType) {
            case DeviceType::Phone:
                newMode = LayoutMode::Compact;
                break;
            case DeviceType::Tablet:
                newMode = touchOptimization ? LayoutMode::Touch : LayoutMode::Full;
                break;
            case DeviceType::Desktop:
            case DeviceType::TV:
                newMode = performanceMode ? LayoutMode::Performance : LayoutMode::Full;
                break;
            default:
                break;
        }
        
        if (newMode != currentLayoutMode) {
            currentLayoutMode = newMode;
            notifyLayoutModeChanged();
        }
    }
    
    // Create layout constraints
    LayoutConstraints constraints;
    constraints.availableArea = juce::Rectangle<int>(0, 0, width, height);
    constraints.deviceType = currentDeviceType;
    constraints.orientation = currentOrientation;
    constraints.layoutMode = currentLayoutMode;
    constraints.touchMode = touchOptimization;
    constraints.scaleFactor = currentBreakpoint.scaleFactor;
    constraints.maxLayoutTime = settings.maxLayoutTime;
    
    // Check if we can use cached layout
    if (isLayoutCacheValid(constraints)) {
        // Use cached layout
        ResponsiveLayoutManager::updateLayout(width, height);
    } else {
        // Calculate new adaptive layout
        calculateAdaptiveLayout(constraints);
        
        // Update base layout manager
        ResponsiveLayoutManager::updateLayout(width, height);
        
        // Update cache
        updateLayoutCache(constraints);
    }
    
    // Update performance analysis
    auto endTime = juce::Time::getCurrentTime();
    currentAnalysis.layoutTime = (endTime - startTime).inMilliseconds();
    updateLayoutAnalysis();
    
    // Notify listeners of changes
    if (deviceChanged) {
        notifyDeviceTypeChanged();
    }
    if (orientationChanged) {
        notifyOrientationChanged();
    }
    if (breakpointChanged) {
        notifyBreakpointChanged();
    }
    
    // Debug output if enabled
    if (debugMode) {
        debugPrintLayout();
    }
}

juce::Array<juce::String> AdaptiveLayoutManager::calculateVisibleComponents(const LayoutConstraints& constraints) const {
    // Get all available components from base class
    auto allComponents = getComponentIds();
    
    // Prioritize components based on constraints
    auto prioritizedComponents = prioritizeComponents(constraints);
    
    // Filter based on available space and priorities
    juce::Array<juce::String> visibleComponents;
    int availableArea = constraints.availableArea.getWidth() * constraints.availableArea.getHeight();
    int usedArea = 0;
    
    for (const auto& componentId : prioritizedComponents) {
        auto priority = getComponentPriority(componentId);
        
        // Always show essential components
        if (priority.essential) {
            visibleComponents.add(componentId);
            continue;
        }
        
        // Check if component should be shown based on constraints
        if (shouldShowComponent(componentId, constraints)) {
            // Estimate component area
            auto bounds = calculateOptimalBounds(componentId, constraints);
            int componentArea = bounds.getWidth() * bounds.getHeight();
            
            // Check if we have enough space
            if (usedArea + componentArea <= availableArea * 0.9f) { // Leave 10% margin
                visibleComponents.add(componentId);
                usedArea += componentArea;
            } else if (priority.collapsible) {
                // Try collapsed version
                int collapsedArea = priority.minWidth * priority.minHeight;
                if (usedArea + collapsedArea <= availableArea * 0.9f) {
                    visibleComponents.add(componentId);
                    usedArea += collapsedArea;
                }
            }
        }
        
        // Stop if we've reached the maximum component limit
        if (visibleComponents.size() >= constraints.maxComponents) {
            break;
        }
    }
    
    return visibleComponents;
}

juce::Rectangle<int> AdaptiveLayoutManager::calculateComponentBounds(const juce::String& componentId, 
                                                                   const LayoutConstraints& constraints) const {
    // Get base bounds from parent class
    auto baseBounds = ResponsiveLayoutManager::getComponentBounds(componentId);
    
    // Apply adaptive adjustments
    auto bounds = calculateOptimalBounds(componentId, constraints);
    
    // Apply touch optimization if enabled
    if (constraints.touchMode) {
        adjustBoundsForTouch(bounds);
    }
    
    // Apply scale factor
    if (constraints.scaleFactor != 1.0f) {
        bounds = bounds.transformedBy(juce::AffineTransform::scale(constraints.scaleFactor));
    }
    
    // Ensure bounds are within available area
    bounds = bounds.constrainedWithin(constraints.availableArea);
    
    return bounds;
}

//==============================================================================
// Layout Optimization
//==============================================================================

void AdaptiveLayoutManager::optimizeLayout(const LayoutConstraints& constraints) {
    auto startTime = juce::Time::getCurrentTime();
    
    if (performanceMode) {
        optimizeForPerformance();
    }
    
    // Reduce component count if necessary
    if (constraints.maxComponents < INT_MAX) {
        reduceComponentCount(constraints);
    }
    
    // Simplify layout if performance is poor
    if (currentAnalysis.layoutTime > settings.maxLayoutTime) {
        simplifyLayout(constraints);
    }
    
    // Update analysis
    auto endTime = juce::Time::getCurrentTime();
    currentAnalysis.layoutTime = (endTime - startTime).inMilliseconds();
    
    notifyLayoutOptimized();
}

void AdaptiveLayoutManager::enablePerformanceMode(bool enabled) {
    if (enabled != performanceMode) {
        performanceMode = enabled;
        
        if (enabled) {
            // Adjust settings for better performance
            settings.enableAnimatedTransitions = false;
            settings.maxVisibleComponents = 30;
            settings.maxLayoutTime = 8.0; // Stricter timing
        } else {
            // Restore normal settings
            settings.enableAnimatedTransitions = true;
            settings.maxVisibleComponents = 50;
            settings.maxLayoutTime = 16.0;
        }
        
        invalidateLayoutCache();
    }
}

//==============================================================================
// Touch Optimization
//==============================================================================

void AdaptiveLayoutManager::enableTouchOptimization(bool enabled) {
    if (enabled != touchOptimization) {
        touchOptimization = enabled;
        settings.enableTouchOptimization = enabled;
        
        if (enabled) {
            // Ensure minimum touch target sizes
            adjustForTouchTargets();
        }
        
        invalidateLayoutCache();
    }
}

float AdaptiveLayoutManager::getTouchTargetSize() const {
    return settings.touchTargetMinSize * currentBreakpoint.scaleFactor;
}

void AdaptiveLayoutManager::adjustForTouchTargets() {
    if (!touchOptimization) {
        return;
    }
    
    float minTouchSize = getTouchTargetSize();
    
    // Adjust component priorities to ensure touch targets meet minimum size
    for (auto& pair : componentPriorities) {
        auto& priority = pair.second;
        priority.minWidth = juce::jmax(priority.minWidth, static_cast<int>(minTouchSize));
        priority.minHeight = juce::jmax(priority.minHeight, static_cast<int>(minTouchSize));
    }
    
    invalidateLayoutCache();
}

//==============================================================================
// Animation and Transitions
//==============================================================================

void AdaptiveLayoutManager::setTransitionDuration(double duration) {
    settings.transitionDuration = juce::jlimit(0.0, 2.0, duration);
}

void AdaptiveLayoutManager::enableAnimatedTransitions(bool enabled) {
    settings.enableAnimatedTransitions = enabled && !performanceMode;
}

//==============================================================================
// Settings Management
//==============================================================================

void AdaptiveLayoutManager::setAdaptiveSettings(const AdaptiveSettings& newSettings) {
    settings = newSettings;
    
    // Apply settings that affect current state
    if (settings.enableTouchOptimization != touchOptimization) {
        enableTouchOptimization(settings.enableTouchOptimization);
    }
    
    if (settings.enablePerformanceMode != performanceMode) {
        enablePerformanceMode(settings.enablePerformanceMode);
    }
    
    invalidateLayoutCache();
}

//==============================================================================
// Layout Analysis
//==============================================================================

AdaptiveLayoutManager::LayoutAnalysis AdaptiveLayoutManager::analyzeCurrentLayout() const {
    updateLayoutAnalysis();
    return currentAnalysis;
}

AdaptiveLayoutManager::LayoutAnalysis AdaptiveLayoutManager::predictLayout(const LayoutConstraints& constraints) const {
    LayoutAnalysis analysis;
    
    auto visibleComponents = calculateVisibleComponents(constraints);
    auto allComponents = getComponentIds();
    
    analysis.totalComponents = allComponents.size();
    analysis.visibleComponents = visibleComponents.size();
    analysis.hiddenComponents = analysis.totalComponents - analysis.visibleComponents;
    analysis.spaceUtilization = calculateSpaceUtilization(constraints);
    analysis.memoryUsage = estimateMemoryUsage();
    
    // Estimate layout time based on component count
    analysis.layoutTime = analysis.visibleComponents * 0.5; // 0.5ms per component estimate
    
    return analysis;
}

//==============================================================================
// State Management
//==============================================================================

void AdaptiveLayoutManager::saveState(ComponentState& state) const {
    // Save base class state
    ResponsiveLayoutManager::saveState(state);
    
    // Save adaptive settings
    state.setValue("enableBreakpoints", settings.enableBreakpoints);
    state.setValue("enableOrientationAdaptation", settings.enableOrientationAdaptation);
    state.setValue("enableComponentPriority", settings.enableComponentPriority);
    state.setValue("enableAnimatedTransitions", settings.enableAnimatedTransitions);
    state.setValue("enableTouchOptimization", settings.enableTouchOptimization);
    state.setValue("enablePerformanceMode", settings.enablePerformanceMode);
    state.setValue("transitionDuration", settings.transitionDuration);
    state.setValue("touchTargetMinSize", settings.touchTargetMinSize);
    state.setValue("componentSpacing", settings.componentSpacing);
    state.setValue("marginSize", settings.marginSize);
    
    // Save current state
    state.setValue("currentLayoutMode", static_cast<int>(currentLayoutMode));
    state.setValue("autoLayoutMode", autoLayoutMode);
    state.setValue("performanceMode", performanceMode);
    state.setValue("touchOptimization", touchOptimization);
    
    // Save breakpoints
    state.setValue("breakpointCount", static_cast<int>(breakpoints.size()));
    for (size_t i = 0; i < breakpoints.size(); ++i) {
        const auto& bp = breakpoints[i];
        juce::String prefix = "breakpoint" + juce::String(static_cast<int>(i)) + "_";
        state.setValue(prefix + "name", bp.name);
        state.setValue(prefix + "minWidth", bp.minWidth);
        state.setValue(prefix + "maxWidth", bp.maxWidth);
        state.setValue(prefix + "deviceType", static_cast<int>(bp.deviceType));
        state.setValue(prefix + "layoutMode", static_cast<int>(bp.layoutMode));
        state.setValue(prefix + "scaleFactor", bp.scaleFactor);
        state.setValue(prefix + "touchOptimized", bp.touchOptimized);
    }
}

void AdaptiveLayoutManager::loadState(const ComponentState& state) {
    // Load base class state
    ResponsiveLayoutManager::loadState(state);
    
    // Load adaptive settings
    settings.enableBreakpoints = state.getValue("enableBreakpoints", true);
    settings.enableOrientationAdaptation = state.getValue("enableOrientationAdaptation", true);
    settings.enableComponentPriority = state.getValue("enableComponentPriority", true);
    settings.enableAnimatedTransitions = state.getValue("enableAnimatedTransitions", true);
    settings.enableTouchOptimization = state.getValue("enableTouchOptimization", true);
    settings.enablePerformanceMode = state.getValue("enablePerformanceMode", false);
    settings.transitionDuration = state.getValue("transitionDuration", 0.3);
    settings.touchTargetMinSize = state.getValue("touchTargetMinSize", 44.0f);
    settings.componentSpacing = state.getValue("componentSpacing", 8.0f);
    settings.marginSize = state.getValue("marginSize", 16.0f);
    
    // Load current state
    currentLayoutMode = static_cast<LayoutMode>(state.getValue("currentLayoutMode", static_cast<int>(LayoutMode::Full)));
    autoLayoutMode = state.getValue("autoLayoutMode", true);
    performanceMode = state.getValue("performanceMode", false);
    touchOptimization = state.getValue("touchOptimization", false);
    
    // Load breakpoints
    breakpoints.clear();
    int breakpointCount = state.getValue("breakpointCount", 0);
    for (int i = 0; i < breakpointCount; ++i) {
        juce::String prefix = "breakpoint" + juce::String(i) + "_";
        Breakpoint bp;
        bp.name = state.getValue(prefix + "name", "");
        bp.minWidth = state.getValue(prefix + "minWidth", 0);
        bp.maxWidth = state.getValue(prefix + "maxWidth", INT_MAX);
        bp.deviceType = static_cast<DeviceType>(state.getValue(prefix + "deviceType", static_cast<int>(DeviceType::Desktop)));
        bp.layoutMode = static_cast<LayoutMode>(state.getValue(prefix + "layoutMode", static_cast<int>(LayoutMode::Full)));
        bp.scaleFactor = state.getValue(prefix + "scaleFactor", 1.0f);
        bp.touchOptimized = state.getValue(prefix + "touchOptimized", false);
        
        if (isValidBreakpoint(bp)) {
            breakpoints.push_back(bp);
        }
    }
    
    // Apply loaded settings
    enablePerformanceMode(settings.enablePerformanceMode);
    enableTouchOptimization(settings.enableTouchOptimization);
}

//==============================================================================
// Listener Management
//==============================================================================

void AdaptiveLayoutManager::addListener(Listener* listener) {
    listeners.add(listener);
}

void AdaptiveLayoutManager::removeListener(Listener* listener) {
    listeners.remove(listener);
}

//==============================================================================
// Debug and Diagnostics
//==============================================================================

void AdaptiveLayoutManager::enableDebugMode(bool enabled) {
    debugMode = enabled;
}

juce::String AdaptiveLayoutManager::getLayoutDebugInfo() const {
    juce::String info;
    
    info += "=== Adaptive Layout Debug Info ===\n";
    info += "Device Type: " + juce::String(static_cast<int>(currentDeviceType)) + "\n";
    info += "Orientation: " + juce::String(static_cast<int>(currentOrientation)) + "\n";
    info += "Layout Mode: " + juce::String(static_cast<int>(currentLayoutMode)) + "\n";
    info += "Current Breakpoint: " + currentBreakpoint.name + "\n";
    info += "Performance Mode: " + juce::String(performanceMode ? "ON" : "OFF") + "\n";
    info += "Touch Optimization: " + juce::String(touchOptimization ? "ON" : "OFF") + "\n";
    
    info += "\n=== Layout Analysis ===\n";
    info += "Total Components: " + juce::String(currentAnalysis.totalComponents) + "\n";
    info += "Visible Components: " + juce::String(currentAnalysis.visibleComponents) + "\n";
    info += "Hidden Components: " + juce::String(currentAnalysis.hiddenComponents) + "\n";
    info += "Layout Time: " + juce::String(currentAnalysis.layoutTime, 2) + "ms\n";
    info += "Space Utilization: " + juce::String(currentAnalysis.spaceUtilization * 100.0f, 1) + "%\n";
    info += "Memory Usage: " + juce::String(currentAnalysis.memoryUsage, 2) + "MB\n";
    
    return info;
}

void AdaptiveLayoutManager::logLayoutMetrics() const {
    if (debugMode) {
        DBG("AdaptiveLayoutManager Metrics:");
        DBG("  Layout Time: " + juce::String(currentAnalysis.layoutTime, 2) + "ms");
        DBG("  Visible Components: " + juce::String(currentAnalysis.visibleComponents));
        DBG("  Space Utilization: " + juce::String(currentAnalysis.spaceUtilization * 100.0f, 1) + "%");
        DBG("  Memory Usage: " + juce::String(currentAnalysis.memoryUsage, 2) + "MB");
    }
}

//==============================================================================
// Private Implementation
//==============================================================================

void AdaptiveLayoutManager::initializeDefaultBreakpoints() {
    // Phone breakpoint
    Breakpoint phone("phone", 0, 599, DeviceType::Phone);
    phone.layoutMode = LayoutMode::Compact;
    phone.scaleFactor = 1.0f;
    phone.touchOptimized = true;
    breakpoints.push_back(phone);
    
    // Tablet breakpoint
    Breakpoint tablet("tablet", 600, 1199, DeviceType::Tablet);
    tablet.layoutMode = LayoutMode::Full;
    tablet.scaleFactor = 1.0f;
    tablet.touchOptimized = true;
    breakpoints.push_back(tablet);
    
    // Desktop breakpoint
    Breakpoint desktop("desktop", 1200, 1919, DeviceType::Desktop);
    desktop.layoutMode = LayoutMode::Full;
    desktop.scaleFactor = 1.0f;
    desktop.touchOptimized = false;
    breakpoints.push_back(desktop);
    
    // TV breakpoint
    Breakpoint tv("tv", 1920, INT_MAX, DeviceType::TV);
    tv.layoutMode = LayoutMode::Full;
    tv.scaleFactor = 1.2f;
    tv.touchOptimized = false;
    breakpoints.push_back(tv);
}

void AdaptiveLayoutManager::initializeDefaultPriorities() {
    // Set default priorities for common components
    ComponentPriority topBar("TopBar", 1, true); // Essential, highest priority
    componentPriorities["TopBar"] = topBar;
    
    ComponentPriority playerTabs("PlayerTabs", 2, true); // Essential
    componentPriorities["PlayerTabs"] = playerTabs;
    
    ComponentPriority drumKit("DrumKit", 3, false); // High priority
    componentPriorities["DrumKit"] = drumKit;
    
    ComponentPriority sceneLauncher("SceneLauncher", 4, false); // Medium-high priority
    componentPriorities["SceneLauncher"] = sceneLauncher;
    
    ComponentPriority patternMatrix("PatternMatrix", 5, false); // Medium priority
    componentPriorities["PatternMatrix"] = patternMatrix;
    
    ComponentPriority loopSection("LoopSection", 6, false); // Lower priority
    componentPriorities["LoopSection"] = loopSection;
}

void AdaptiveLayoutManager::calculateAdaptiveLayout(const LayoutConstraints& constraints) {
    // Get visible components based on constraints
    auto visibleComponents = calculateVisibleComponents(constraints);
    
    // Distribute space among visible components
    distributeSpace(visibleComponents, constraints);
    
    // Apply touch optimizations if needed
    if (constraints.touchMode) {
        for (const auto& componentId : visibleComponents) {
            auto bounds = calculateComponentBounds(componentId, constraints);
            adjustBoundsForTouch(bounds);
            // Update bounds in base class
            setComponentBounds(componentId, bounds);
        }
    }
}

juce::Array<juce::String> AdaptiveLayoutManager::prioritizeComponents(const LayoutConstraints& constraints) const {
    auto allComponents = getComponentIds();
    
    // Sort components by priority
    allComponents.sort([this](const juce::String& a, const juce::String& b) {
        auto priorityA = getComponentPriority(a);
        auto priorityB = getComponentPriority(b);
        
        // Essential components first
        if (priorityA.essential != priorityB.essential) {
            return priorityA.essential;
        }
        
        // Then by priority number (lower = higher priority)
        return priorityA.priority < priorityB.priority;
    });
    
    return allComponents;
}

void AdaptiveLayoutManager::distributeSpace(const juce::Array<juce::String>& components, 
                                          const LayoutConstraints& constraints) {
    if (components.isEmpty()) {
        return;
    }
    
    // Calculate available space
    auto availableArea = constraints.availableArea;
    int totalWidth = availableArea.getWidth();
    int totalHeight = availableArea.getHeight();
    
    // Apply margins
    int margin = static_cast<int>(settings.marginSize * constraints.scaleFactor);
    totalWidth -= margin * 2;
    totalHeight -= margin * 2;
    
    // Calculate space per component (simplified equal distribution)
    int componentsPerRow = juce::jmax(1, static_cast<int>(std::sqrt(components.size())));
    int rows = (components.size() + componentsPerRow - 1) / componentsPerRow;
    
    int componentWidth = totalWidth / componentsPerRow;
    int componentHeight = totalHeight / rows;
    
    // Apply spacing
    int spacing = static_cast<int>(settings.componentSpacing * constraints.scaleFactor);
    componentWidth -= spacing;
    componentHeight -= spacing;
    
    // Position components
    for (int i = 0; i < components.size(); ++i) {
        int row = i / componentsPerRow;
        int col = i % componentsPerRow;
        
        int x = margin + col * (componentWidth + spacing);
        int y = margin + row * (componentHeight + spacing);
        
        juce::Rectangle<int> bounds(x, y, componentWidth, componentHeight);
        
        // Apply component-specific adjustments
        auto priority = getComponentPriority(components[i]);
        if (shouldCollapseComponent(components[i], constraints)) {
            bounds = bounds.withSize(priority.minWidth, priority.minHeight);
        }
        
        // Update bounds in base class
        setComponentBounds(components[i], bounds);
    }
}

bool AdaptiveLayoutManager::shouldShowComponent(const juce::String& componentId, const LayoutConstraints& constraints) const {
    auto priority = getComponentPriority(componentId);
    
    // Essential components are always shown
    if (priority.essential) {
        return true;
    }
    
    // Check layout mode restrictions
    switch (constraints.layoutMode) {
        case LayoutMode::Minimal:
            return priority.priority <= 3; // Only high priority components
            
        case LayoutMode::Compact:
            return priority.priority <= 6; // Medium and high priority components
            
        case LayoutMode::Performance:
            return priority.priority <= 4; // Reduce components for performance
            
        case LayoutMode::Touch:
        case LayoutMode::Full:
        default:
            return true; // Show all components
    }
}

bool AdaptiveLayoutManager::shouldCollapseComponent(const juce::String& componentId, const LayoutConstraints& constraints) const {
    auto priority = getComponentPriority(componentId);
    
    if (!priority.collapsible) {
        return false;
    }
    
    // Collapse components in compact mode or when space is limited
    if (constraints.layoutMode == LayoutMode::Compact || 
        constraints.layoutMode == LayoutMode::Minimal) {
        return priority.priority > 3;
    }
    
    return false;
}

juce::Rectangle<int> AdaptiveLayoutManager::calculateOptimalBounds(const juce::String& componentId, 
                                                                 const LayoutConstraints& constraints) const {
    // Get base bounds from parent class
    auto baseBounds = ResponsiveLayoutManager::getComponentBounds(componentId);
    
    // Apply scale factor
    if (constraints.scaleFactor != 1.0f) {
        baseBounds = baseBounds.transformedBy(juce::AffineTransform::scale(constraints.scaleFactor));
    }
    
    // Apply component-specific adjustments
    auto priority = getComponentPriority(componentId);
    
    if (shouldCollapseComponent(componentId, constraints)) {
        // Use minimum size for collapsed components
        baseBounds = baseBounds.withSize(priority.minWidth, priority.minHeight);
    }
    
    return baseBounds;
}

//==============================================================================
// Touch Optimization Helpers
//==============================================================================

void AdaptiveLayoutManager::adjustBoundsForTouch(juce::Rectangle<int>& bounds) const {
    if (!touchOptimization) {
        return;
    }
    
    ensureMinimumTouchTarget(bounds);
}

float AdaptiveLayoutManager::calculateTouchTargetScale(const juce::Rectangle<int>& bounds) const {
    float minTouchSize = getTouchTargetSize();
    float currentSize = juce::jmin(bounds.getWidth(), bounds.getHeight());
    
    if (currentSize < minTouchSize) {
        return minTouchSize / currentSize;
    }
    
    return 1.0f;
}

void AdaptiveLayoutManager::ensureMinimumTouchTarget(juce::Rectangle<int>& bounds) const {
    float minTouchSize = getTouchTargetSize();
    
    if (bounds.getWidth() < minTouchSize) {
        bounds = bounds.withWidth(static_cast<int>(minTouchSize));
    }
    
    if (bounds.getHeight() < minTouchSize) {
        bounds = bounds.withHeight(static_cast<int>(minTouchSize));
    }
}

//==============================================================================
// Animation Helpers
//==============================================================================

void AdaptiveLayoutManager::startLayoutAnimation(const LayoutConstraints& fromConstraints, 
                                                const LayoutConstraints& toConstraints) {
    if (!settings.enableAnimatedTransitions) {
        return;
    }
    
    animationState.animating = true;
    animationState.startTime = juce::Time::getCurrentTime();
    animationState.fromConstraints = fromConstraints;
    animationState.toConstraints = toConstraints;
    
    // Store current bounds as starting points
    auto components = getComponentIds();
    for (const auto& componentId : components) {
        animationState.fromBounds[componentId] = getComponentBounds(componentId);
        animationState.toBounds[componentId] = calculateComponentBounds(componentId, toConstraints);
    }
}

void AdaptiveLayoutManager::updateLayoutAnimation() {
    if (!animationState.animating) {
        return;
    }
    
    auto currentTime = juce::Time::getCurrentTime();
    double elapsed = (currentTime - animationState.startTime).inSeconds();
    double progress = elapsed / settings.transitionDuration;
    
    if (progress >= 1.0) {
        // Animation complete
        animationState.reset();
        return;
    }
    
    // Apply easing
    float easedProgress = easeInOutCubic(static_cast<float>(progress));
    
    // Interpolate component bounds
    for (const auto& pair : animationState.fromBounds) {
        const auto& componentId = pair.first;
        const auto& fromBounds = pair.second;
        
        auto it = animationState.toBounds.find(componentId);
        if (it != animationState.toBounds.end()) {
            const auto& toBounds = it->second;
            auto interpolatedBounds = interpolateBounds(fromBounds, toBounds, easedProgress);
            setComponentBounds(componentId, interpolatedBounds);
        }
    }
}

juce::Rectangle<int> AdaptiveLayoutManager::interpolateBounds(const juce::Rectangle<int>& from, 
                                                            const juce::Rectangle<int>& to, 
                                                            float progress) const {
    int x = static_cast<int>(from.getX() + progress * (to.getX() - from.getX()));
    int y = static_cast<int>(from.getY() + progress * (to.getY() - from.getY()));
    int width = static_cast<int>(from.getWidth() + progress * (to.getWidth() - from.getWidth()));
    int height = static_cast<int>(from.getHeight() + progress * (to.getHeight() - from.getHeight()));
    
    return juce::Rectangle<int>(x, y, width, height);
}

float AdaptiveLayoutManager::easeInOutCubic(float t) const {
    return t < 0.5f ? 4 * t * t * t : 1 - std::pow(-2 * t + 2, 3) / 2;
}

//==============================================================================
// Cache Management
//==============================================================================

bool AdaptiveLayoutManager::isLayoutCacheValid(const LayoutConstraints& constraints) const {
    if (!layoutCache.valid) {
        return false;
    }
    
    // Check if constraints match
    const auto& cached = layoutCache.constraints;
    return cached.availableArea == constraints.availableArea &&
           cached.deviceType == constraints.deviceType &&
           cached.orientation == constraints.orientation &&
           cached.layoutMode == constraints.layoutMode &&
           cached.touchMode == constraints.touchMode &&
           std::abs(cached.scaleFactor - constraints.scaleFactor) < 0.01f;
}

void AdaptiveLayoutManager::updateLayoutCache(const LayoutConstraints& constraints) const {
    layoutCache.constraints = constraints;
    layoutCache.visibleComponents = calculateVisibleComponents(constraints);
    layoutCache.analysis = predictLayout(constraints);
    layoutCache.cacheTime = juce::Time::getCurrentTime();
    layoutCache.valid = true;
    
    // Cache component bounds
    layoutCache.componentBounds.clear();
    for (const auto& componentId : layoutCache.visibleComponents) {
        layoutCache.componentBounds[componentId] = calculateComponentBounds(componentId, constraints);
    }
}

void AdaptiveLayoutManager::invalidateLayoutCache() const {
    layoutCache.invalidate();
}

//==============================================================================
// Performance Optimization
//==============================================================================

void AdaptiveLayoutManager::optimizeForPerformance() {
    // Reduce animation complexity
    settings.enableAnimatedTransitions = false;
    
    // Limit component count
    settings.maxVisibleComponents = 25;
    
    // Simplify layout calculations
    settings.enableComponentPriority = false;
    
    invalidateLayoutCache();
}

void AdaptiveLayoutManager::reduceComponentCount(const LayoutConstraints& constraints) {
    auto visibleComponents = calculateVisibleComponents(constraints);
    
    if (visibleComponents.size() > constraints.maxComponents) {
        // Hide lowest priority components
        auto prioritizedComponents = prioritizeComponents(constraints);
        
        for (int i = constraints.maxComponents; i < prioritizedComponents.size(); ++i) {
            auto priority = getComponentPriority(prioritizedComponents[i]);
            if (!priority.essential) {
                // Hide this component
                setComponentVisible(prioritizedComponents[i], false);
            }
        }
    }
}

void AdaptiveLayoutManager::simplifyLayout(const LayoutConstraints& constraints) {
    // Switch to a simpler layout mode
    if (currentLayoutMode == LayoutMode::Full) {
        setLayoutMode(LayoutMode::Compact);
    } else if (currentLayoutMode == LayoutMode::Compact) {
        setLayoutMode(LayoutMode::Minimal);
    }
}

//==============================================================================
// Analysis Helpers
//==============================================================================

void AdaptiveLayoutManager::updateLayoutAnalysis() const {
    auto allComponents = getComponentIds();
    currentAnalysis.totalComponents = allComponents.size();
    
    int visibleCount = 0;
    for (const auto& componentId : allComponents) {
        if (isComponentVisible(componentId)) {
            visibleCount++;
        }
    }
    
    currentAnalysis.visibleComponents = visibleCount;
    currentAnalysis.hiddenComponents = currentAnalysis.totalComponents - visibleCount;
    currentAnalysis.memoryUsage = estimateMemoryUsage();
}

double AdaptiveLayoutManager::measureLayoutTime() const {
    return (juce::Time::getCurrentTime() - lastLayoutTime).inMilliseconds();
}

double AdaptiveLayoutManager::estimateMemoryUsage() const {
    // Rough estimate based on component count and cache size
    double baseUsage = currentAnalysis.visibleComponents * 0.1; // 0.1MB per visible component
    double cacheUsage = layoutCache.componentBounds.size() * 0.01; // 0.01MB per cached bound
    return baseUsage + cacheUsage;
}

float AdaptiveLayoutManager::calculateSpaceUtilization(const LayoutConstraints& constraints) const {
    int totalArea = constraints.availableArea.getWidth() * constraints.availableArea.getHeight();
    if (totalArea <= 0) {
        return 0.0f;
    }
    
    int usedArea = 0;
    auto visibleComponents = calculateVisibleComponents(constraints);
    
    for (const auto& componentId : visibleComponents) {
        auto bounds = calculateComponentBounds(componentId, constraints);
        usedArea += bounds.getWidth() * bounds.getHeight();
    }
    
    return static_cast<float>(usedArea) / static_cast<float>(totalArea);
}

//==============================================================================
// Validation Helpers
//==============================================================================

bool AdaptiveLayoutManager::isValidBreakpoint(const Breakpoint& breakpoint) const {
    return !breakpoint.name.isEmpty() &&
           breakpoint.minWidth >= 0 &&
           breakpoint.maxWidth > breakpoint.minWidth &&
           breakpoint.scaleFactor > 0.0f;
}

bool AdaptiveLayoutManager::isValidComponentPriority(const ComponentPriority& priority) const {
    return !priority.componentId.isEmpty() &&
           priority.priority >= 1 && priority.priority <= 10 &&
           priority.minWidth > 0 && priority.minHeight > 0;
}

bool AdaptiveLayoutManager::isValidLayoutConstraints(const LayoutConstraints& constraints) const {
    return constraints.availableArea.getWidth() > 0 &&
           constraints.availableArea.getHeight() > 0 &&
           constraints.scaleFactor > 0.0f &&
           constraints.maxLayoutTime > 0.0;
}

//==============================================================================
// Notification Helpers
//==============================================================================

void AdaptiveLayoutManager::notifyBreakpointChanged() {
    listeners.call([this](Listener& l) { l.breakpointChanged(currentBreakpoint); });
}

void AdaptiveLayoutManager::notifyDeviceTypeChanged() {
    listeners.call([this](Listener& l) { l.deviceTypeChanged(currentDeviceType); });
}

void AdaptiveLayoutManager::notifyOrientationChanged() {
    listeners.call([this](Listener& l) { l.orientationChanged(currentOrientation); });
}

void AdaptiveLayoutManager::notifyLayoutModeChanged() {
    listeners.call([this](Listener& l) { l.layoutModeChanged(currentLayoutMode); });
}

void AdaptiveLayoutManager::notifyComponentVisibilityChanged(const juce::String& componentId, bool visible) {
    listeners.call([&](Listener& l) { l.componentVisibilityChanged(componentId, visible); });
}

void AdaptiveLayoutManager::notifyLayoutOptimized() {
    listeners.call([this](Listener& l) { l.layoutOptimized(currentAnalysis); });
}

//==============================================================================
// Debug Helpers
//==============================================================================

void AdaptiveLayoutManager::debugPrintLayout() const {
    if (!debugMode) {
        return;
    }
    
    DBG("=== Adaptive Layout Debug ===");
    DBG("Device: " + juce::String(static_cast<int>(currentDeviceType)) + 
        ", Orientation: " + juce::String(static_cast<int>(currentOrientation)));
    DBG("Layout Mode: " + juce::String(static_cast<int>(currentLayoutMode)));
    DBG("Breakpoint: " + currentBreakpoint.name);
    DBG("Visible Components: " + juce::String(currentAnalysis.visibleComponents) + 
        "/" + juce::String(currentAnalysis.totalComponents));
    DBG("Layout Time: " + juce::String(currentAnalysis.layoutTime, 2) + "ms");
}

void AdaptiveLayoutManager::debugPrintBreakpoints() const {
    if (!debugMode) {
        return;
    }
    
    DBG("=== Breakpoints ===");
    for (const auto& bp : breakpoints) {
        DBG(bp.name + ": " + juce::String(bp.minWidth) + "-" + juce::String(bp.maxWidth) + 
            "px, Device: " + juce::String(static_cast<int>(bp.deviceType)));
    }
}

void AdaptiveLayoutManager::debugPrintComponentPriorities() const {
    if (!debugMode) {
        return;
    }
    
    DBG("=== Component Priorities ===");
    for (const auto& pair : componentPriorities) {
        const auto& priority = pair.second;
        DBG(priority.componentId + ": Priority " + juce::String(priority.priority) + 
            (priority.essential ? " (Essential)" : "") +
            (priority.collapsible ? " (Collapsible)" : ""));
    }
}