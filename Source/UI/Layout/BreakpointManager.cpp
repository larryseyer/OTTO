#include "BreakpointManager.h"
#include "../../JUCE8_CODING_STANDARDS.h"

namespace OTTO {
namespace UI {
namespace Layout {

//==============================================================================
// BreakpointManager Implementation
//==============================================================================

BreakpointManager::BreakpointManager()
{
    // Initialize default settings
    settings.enabled = true;
    settings.autoDetectBreakpoints = true;
    settings.smoothTransitions = true;
    settings.transitionDuration = 300;
    settings.respectSystemSettings = true;
    settings.enableDebugMode = false;
    settings.updateThrottleMs = 16; // 60fps
    settings.enableCaching = true;
    settings.cacheSize = DEFAULT_CACHE_SIZE;
    
    // Platform-specific settings
    settings.platformSettings.adaptToKeyboard = true;
    settings.platformSettings.handleOrientationChange = true;
    settings.platformSettings.useSystemDensity = true;
    settings.platformSettings.enableHapticFeedback = true;
    
    // Initialize default breakpoints and layout rules
    initializeDefaultBreakpoints();
    initializeDefaultLayoutRules();
    
    // Initialize screen info
    updateScreenInfo();
}

BreakpointManager::~BreakpointManager()
{
    clearCache();
}

BreakpointManager& BreakpointManager::getInstance()
{
    static BreakpointManager instance;
    return instance;
}

//==============================================================================
// Core Functionality
//==============================================================================

bool BreakpointManager::updateScreenInfo(juce::Component* component)
{
    if (!settings.enabled)
        return false;
        
    // Throttle updates to maintain 60fps
    auto currentTime = juce::Time::getMillisecondCounter();
    if (currentTime - lastUpdateTime < static_cast<juce::uint32>(settings.updateThrottleMs))
    {
        updatePending = true;
        return false;
    }
    
    lastUpdateTime = currentTime;
    updatePending = false;
    
    // Store previous screen info for comparison
    previousScreenInfo = currentScreenInfo;
    
    // Detect new screen information
    currentScreenInfo = detectScreenInfo(component);
    
    // Check if screen info changed significantly
    bool screenInfoChanged = (currentScreenInfo.width != previousScreenInfo.width ||
                             currentScreenInfo.height != previousScreenInfo.height ||
                             currentScreenInfo.density != previousScreenInfo.density ||
                             currentScreenInfo.category != previousScreenInfo.category ||
                             currentScreenInfo.orientation != previousScreenInfo.orientation);
    
    if (screenInfoChanged)
    {
        // Store previous active breakpoints
        previousActiveBreakpoints = activeBreakpoints;
        
        // Re-evaluate breakpoints
        evaluateBreakpoints();
        
        // Update layout rules
        updateLayoutRules();
        
        // Notify listeners
        notifyListeners();
        
        return true;
    }
    
    return false;
}

bool BreakpointManager::isBreakpointActive(const juce::String& breakpointName) const
{
    return activeBreakpoints.contains(breakpointName);
}

juce::StringArray BreakpointManager::getActiveBreakpoints() const
{
    return activeBreakpoints;
}

//==============================================================================
// Breakpoint Management
//==============================================================================

bool BreakpointManager::registerBreakpoint(const Breakpoint& breakpoint)
{
    // Check if breakpoint already exists
    for (auto& existing : registeredBreakpoints)
    {
        if (existing.name == breakpoint.name)
        {
            // Update existing breakpoint
            existing = breakpoint;
            evaluateBreakpoints();
            updateLayoutRules();
            notifyListeners();
            return true;
        }
    }
    
    // Add new breakpoint
    registeredBreakpoints.push_back(breakpoint);
    sortBreakpointsByPriority();
    
    // Re-evaluate breakpoints
    evaluateBreakpoints();
    updateLayoutRules();
    notifyListeners();
    
    return true;
}

bool BreakpointManager::unregisterBreakpoint(const juce::String& breakpointName)
{
    auto it = std::find_if(registeredBreakpoints.begin(), registeredBreakpoints.end(),
                          [&breakpointName](const Breakpoint& bp) { return bp.name == breakpointName; });
    
    if (it != registeredBreakpoints.end())
    {
        registeredBreakpoints.erase(it);
        breakpointLayoutRules.erase(breakpointName);
        
        // Re-evaluate breakpoints
        evaluateBreakpoints();
        updateLayoutRules();
        notifyListeners();
        
        return true;
    }
    
    return false;
}

BreakpointManager::Breakpoint BreakpointManager::getBreakpoint(const juce::String& breakpointName) const
{
    for (const auto& breakpoint : registeredBreakpoints)
    {
        if (breakpoint.name == breakpointName)
            return breakpoint;
    }
    
    return Breakpoint{}; // Return empty breakpoint if not found
}

std::vector<BreakpointManager::Breakpoint> BreakpointManager::getAllBreakpoints() const
{
    return registeredBreakpoints;
}

void BreakpointManager::clearCustomBreakpoints()
{
    registeredBreakpoints.clear();
    breakpointLayoutRules.clear();
    activeBreakpoints.clear();
    
    // Reinitialize default breakpoints
    initializeDefaultBreakpoints();
    initializeDefaultLayoutRules();
    
    evaluateBreakpoints();
    updateLayoutRules();
    notifyListeners();
}

//==============================================================================
// Layout Rules
//==============================================================================

void BreakpointManager::setLayoutRules(const juce::String& breakpointName, const LayoutRules& rules)
{
    breakpointLayoutRules[breakpointName] = rules;
    
    // Update current layout rules if this breakpoint is active
    if (isBreakpointActive(breakpointName))
    {
        updateLayoutRules();
        notifyListeners();
    }
}

BreakpointManager::LayoutRules BreakpointManager::getLayoutRules(const juce::String& breakpointName) const
{
    auto it = breakpointLayoutRules.find(breakpointName);
    if (it != breakpointLayoutRules.end())
        return it->second;
    
    // Return default layout rules if not found
    LayoutRules defaultRules;
    return defaultRules;
}

//==============================================================================
// Responsive Calculations
//==============================================================================

int BreakpointManager::calculateResponsiveSize(int baseSize, juce::Component* component) const
{
    if (!settings.enabled)
        return baseSize;
    
    float scaleFactor = calculateScaleFactor();
    int responsiveSize = static_cast<int>(baseSize * scaleFactor);
    
    // Ensure minimum touch target size on touch devices
    if (currentScreenInfo.isTouch && responsiveSize < static_cast<int>(currentLayoutRules.sizing.minTouchTarget))
    {
        responsiveSize = static_cast<int>(currentLayoutRules.sizing.minTouchTarget);
    }
    
    return responsiveSize;
}

float BreakpointManager::calculateResponsiveFontSize(float baseSize) const
{
    if (!settings.enabled)
        return baseSize;
    
    float scaleFactor = calculateScaleFactor();
    
    // Apply typography rules
    float responsiveSize = baseSize * scaleFactor * currentLayoutRules.typography.lineHeight;
    
    // Ensure minimum readable font size
    const float minFontSize = 10.0f;
    if (responsiveSize < minFontSize)
        responsiveSize = minFontSize;
    
    return responsiveSize;
}

int BreakpointManager::calculateResponsiveMargin(int baseMargin) const
{
    if (!settings.enabled)
        return baseMargin;
    
    float scaleFactor = calculateScaleFactor();
    int responsiveMargin = static_cast<int>(baseMargin * scaleFactor);
    
    // Apply spacing adjustments from active breakpoints
    for (const auto& breakpoint : registeredBreakpoints)
    {
        if (breakpoint.isActive)
        {
            responsiveMargin += breakpoint.marginAdjustment;
        }
    }
    
    return juce::jmax(0, responsiveMargin);
}

int BreakpointManager::calculateResponsiveSpacing(int baseSpacing) const
{
    if (!settings.enabled)
        return baseSpacing;
    
    float scaleFactor = calculateScaleFactor();
    int responsiveSpacing = static_cast<int>(baseSpacing * scaleFactor);
    
    // Apply spacing adjustments from active breakpoints
    for (const auto& breakpoint : registeredBreakpoints)
    {
        if (breakpoint.isActive)
        {
            responsiveSpacing += breakpoint.spacingAdjustment;
        }
    }
    
    return juce::jmax(0, responsiveSpacing);
}

int BreakpointManager::getOptimalGridColumns() const
{
    // Adjust grid columns based on device category
    switch (currentScreenInfo.category)
    {
        case DeviceCategory::Mobile:
            return juce::jmin(6, currentLayoutRules.grid.columns);
        case DeviceCategory::Tablet:
            return juce::jmin(8, currentLayoutRules.grid.columns);
        case DeviceCategory::Desktop:
            return currentLayoutRules.grid.columns;
        case DeviceCategory::LargeDesktop:
            return juce::jmax(currentLayoutRules.grid.columns, 16);
        default:
            return currentLayoutRules.grid.columns;
    }
}

juce::Rectangle<int> BreakpointManager::calculateGridBounds(int gridColumn, int columnSpan, 
                                                           const juce::Rectangle<int>& containerBounds) const
{
    int totalColumns = getOptimalGridColumns();
    int gutterWidth = calculateResponsiveSpacing(currentLayoutRules.grid.gutterWidth);
    
    // Calculate column width
    int availableWidth = containerBounds.getWidth() - (gutterWidth * (totalColumns - 1));
    int columnWidth = availableWidth / totalColumns;
    
    // Calculate position
    int x = containerBounds.getX() + (gridColumn * (columnWidth + gutterWidth));
    int width = (columnWidth * columnSpan) + (gutterWidth * (columnSpan - 1));
    
    return juce::Rectangle<int>(x, containerBounds.getY(), width, containerBounds.getHeight());
}

//==============================================================================
// Configuration
//==============================================================================

void BreakpointManager::setSettings(const ResponsiveSettings& newSettings)
{
    settings = newSettings;
    
    // Update cache size if changed
    if (layoutCache.size() > static_cast<size_t>(settings.cacheSize))
    {
        layoutCache.resize(settings.cacheSize);
    }
    
    // Re-evaluate if settings changed
    if (settings.enabled)
    {
        updateScreenInfo();
    }
}

void BreakpointManager::setEnabled(bool enabled)
{
    if (settings.enabled != enabled)
    {
        settings.enabled = enabled;
        
        if (enabled)
        {
            updateScreenInfo();
        }
        else
        {
            // Clear active breakpoints when disabled
            activeBreakpoints.clear();
            notifyListeners();
        }
    }
}

//==============================================================================
// Utility Methods
//==============================================================================

juce::String BreakpointManager::deviceCategoryToString(DeviceCategory category)
{
    switch (category)
    {
        case DeviceCategory::Mobile:        return "Mobile";
        case DeviceCategory::Tablet:        return "Tablet";
        case DeviceCategory::Desktop:       return "Desktop";
        case DeviceCategory::LargeDesktop:  return "LargeDesktop";
        case DeviceCategory::Unknown:       return "Unknown";
        default:                           return "Unknown";
    }
}

juce::String BreakpointManager::orientationToString(Orientation orientation)
{
    switch (orientation)
    {
        case Orientation::Portrait:   return "Portrait";
        case Orientation::Landscape:  return "Landscape";
        case Orientation::Square:     return "Square";
        default:                     return "Unknown";
    }
}

BreakpointManager::DeviceCategory BreakpointManager::detectDeviceCategory(int width, int height, float density)
{
    // Use logical pixels (density-independent)
    int logicalWidth = static_cast<int>(width / density);
    int logicalHeight = static_cast<int>(height / density);
    int maxDimension = juce::jmax(logicalWidth, logicalHeight);
    
    if (maxDimension < MOBILE_MAX_WIDTH)
        return DeviceCategory::Mobile;
    else if (maxDimension < TABLET_MAX_WIDTH)
        return DeviceCategory::Tablet;
    else if (maxDimension < DESKTOP_MAX_WIDTH)
        return DeviceCategory::Desktop;
    else
        return DeviceCategory::LargeDesktop;
}

BreakpointManager::Orientation BreakpointManager::detectOrientation(int width, int height)
{
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    if (std::abs(aspectRatio - 1.0f) < SQUARE_ASPECT_TOLERANCE)
        return Orientation::Square;
    else if (aspectRatio > 1.0f)
        return Orientation::Landscape;
    else
        return Orientation::Portrait;
}

float BreakpointManager::calculateAspectRatio(int width, int height)
{
    if (height == 0)
        return 1.0f;
    
    return static_cast<float>(width) / static_cast<float>(height);
}

//==============================================================================
// State Management
//==============================================================================

void BreakpointManager::saveToState(ComponentState& state) const
{
    // Save current settings
    state.setValue("responsive_enabled", settings.enabled);
    state.setValue("auto_detect_breakpoints", settings.autoDetectBreakpoints);
    state.setValue("smooth_transitions", settings.smoothTransitions);
    state.setValue("transition_duration", settings.transitionDuration);
    state.setValue("enable_caching", settings.enableCaching);
    state.setValue("cache_size", settings.cacheSize);
    
    // Save current screen info
    state.setValue("screen_width", currentScreenInfo.width);
    state.setValue("screen_height", currentScreenInfo.height);
    state.setValue("screen_density", currentScreenInfo.density);
    state.setValue("device_category", static_cast<int>(currentScreenInfo.category));
    state.setValue("orientation", static_cast<int>(currentScreenInfo.orientation));
    
    // Save active breakpoints
    state.setValue("active_breakpoints", activeBreakpoints.joinIntoString(","));
}

void BreakpointManager::loadFromState(const ComponentState& state)
{
    // Load settings
    settings.enabled = state.getValue("responsive_enabled", settings.enabled);
    settings.autoDetectBreakpoints = state.getValue("auto_detect_breakpoints", settings.autoDetectBreakpoints);
    settings.smoothTransitions = state.getValue("smooth_transitions", settings.smoothTransitions);
    settings.transitionDuration = state.getValue("transition_duration", settings.transitionDuration);
    settings.enableCaching = state.getValue("enable_caching", settings.enableCaching);
    settings.cacheSize = state.getValue("cache_size", settings.cacheSize);
    
    // Update screen info if enabled
    if (settings.enabled)
    {
        updateScreenInfo();
    }
}

//==============================================================================
// Listeners
//==============================================================================

void BreakpointManager::addListener(Listener* listener)
{
    listeners.add(listener);
}

void BreakpointManager::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

//==============================================================================
// Private Methods
//==============================================================================

void BreakpointManager::initializeDefaultBreakpoints()
{
    registeredBreakpoints.clear();
    
    // Mobile breakpoint
    Breakpoint mobile;
    mobile.name = "mobile";
    mobile.type = BreakpointType::Width;
    mobile.minValue = 0.0f;
    mobile.maxValue = static_cast<float>(MOBILE_MAX_WIDTH);
    mobile.targetCategory = DeviceCategory::Mobile;
    mobile.scaleFactor = 0.8f;
    mobile.marginAdjustment = -2;
    mobile.spacingAdjustment = -1;
    mobile.fontSizeMultiplier = 0.9f;
    mobile.priority = 100;
    registeredBreakpoints.push_back(mobile);
    
    // Tablet breakpoint
    Breakpoint tablet;
    tablet.name = "tablet";
    tablet.type = BreakpointType::Width;
    tablet.minValue = static_cast<float>(MOBILE_MAX_WIDTH);
    tablet.maxValue = static_cast<float>(TABLET_MAX_WIDTH);
    tablet.targetCategory = DeviceCategory::Tablet;
    tablet.scaleFactor = 0.9f;
    tablet.marginAdjustment = 0;
    tablet.spacingAdjustment = 0;
    tablet.fontSizeMultiplier = 1.0f;
    tablet.priority = 90;
    registeredBreakpoints.push_back(tablet);
    
    // Desktop breakpoint
    Breakpoint desktop;
    desktop.name = "desktop";
    desktop.type = BreakpointType::Width;
    desktop.minValue = static_cast<float>(TABLET_MAX_WIDTH);
    desktop.maxValue = static_cast<float>(DESKTOP_MAX_WIDTH);
    desktop.targetCategory = DeviceCategory::Desktop;
    desktop.scaleFactor = 1.0f;
    desktop.marginAdjustment = 0;
    desktop.spacingAdjustment = 0;
    desktop.fontSizeMultiplier = 1.0f;
    desktop.priority = 80;
    registeredBreakpoints.push_back(desktop);
    
    // Large desktop breakpoint
    Breakpoint largeDesktop;
    largeDesktop.name = "large-desktop";
    largeDesktop.type = BreakpointType::Width;
    largeDesktop.minValue = static_cast<float>(DESKTOP_MAX_WIDTH);
    largeDesktop.maxValue = std::numeric_limits<float>::max();
    largeDesktop.targetCategory = DeviceCategory::LargeDesktop;
    largeDesktop.scaleFactor = 1.2f;
    largeDesktop.marginAdjustment = 4;
    largeDesktop.spacingAdjustment = 2;
    largeDesktop.fontSizeMultiplier = 1.1f;
    largeDesktop.priority = 70;
    registeredBreakpoints.push_back(largeDesktop);
    
    // Portrait orientation breakpoint
    Breakpoint portrait;
    portrait.name = "portrait";
    portrait.type = BreakpointType::AspectRatio;
    portrait.minValue = 0.0f;
    portrait.maxValue = 1.0f;
    portrait.targetOrientation = Orientation::Portrait;
    portrait.scaleFactor = 1.0f;
    portrait.marginAdjustment = -2;
    portrait.spacingAdjustment = -1;
    portrait.fontSizeMultiplier = 0.95f;
    portrait.priority = 50;
    registeredBreakpoints.push_back(portrait);
    
    // High DPI breakpoint
    Breakpoint highDPI;
    highDPI.name = "high-dpi";
    highDPI.type = BreakpointType::Density;
    highDPI.minValue = 2.0f;
    highDPI.maxValue = std::numeric_limits<float>::max();
    highDPI.scaleFactor = 1.0f;
    highDPI.marginAdjustment = 0;
    highDPI.spacingAdjustment = 0;
    highDPI.fontSizeMultiplier = 1.0f;
    highDPI.priority = 60;
    registeredBreakpoints.push_back(highDPI);
    
    sortBreakpointsByPriority();
}

void BreakpointManager::initializeDefaultLayoutRules()
{
    // Default layout rules
    LayoutRules defaultRules;
    
    // Sizing rules
    defaultRules.sizing.minButtonWidth = 44.0f;
    defaultRules.sizing.minButtonHeight = 44.0f;
    defaultRules.sizing.minTouchTarget = 44.0f;
    defaultRules.sizing.maxComponentWidth = 0.0f; // No limit
    defaultRules.sizing.maxComponentHeight = 0.0f; // No limit
    
    // Spacing rules - use INI constants
    defaultRules.spacing.defaultMargin = INIConfig::LayoutConstants::defaultMargin;
    defaultRules.spacing.defaultSpacing = INIConfig::LayoutConstants::defaultSpacing;
    defaultRules.spacing.sectionSpacing = INIConfig::LayoutConstants::defaultSpacing * 2;
    defaultRules.spacing.groupSpacing = INIConfig::LayoutConstants::defaultSpacing * 3;
    
    // Typography rules
    defaultRules.typography.baseFont = 14.0f;
    defaultRules.typography.titleFont = 18.0f;
    defaultRules.typography.smallFont = 12.0f;
    defaultRules.typography.largeFont = 20.0f;
    defaultRules.typography.lineHeight = 1.2f;
    
    // Grid system
    defaultRules.grid.columns = 12;
    defaultRules.grid.gutterWidth = 20;
    defaultRules.grid.maxWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
    
    currentLayoutRules = defaultRules;
    
    // Set layout rules for each breakpoint
    breakpointLayoutRules["mobile"] = defaultRules;
    breakpointLayoutRules["tablet"] = defaultRules;
    breakpointLayoutRules["desktop"] = defaultRules;
    breakpointLayoutRules["large-desktop"] = defaultRules;
    breakpointLayoutRules["portrait"] = defaultRules;
    breakpointLayoutRules["high-dpi"] = defaultRules;
}

void BreakpointManager::evaluateBreakpoints()
{
    activeBreakpoints.clear();
    
    // Evaluate each registered breakpoint
    for (auto& breakpoint : registeredBreakpoints)
    {
        bool wasActive = breakpoint.isActive;
        breakpoint.isActive = evaluateBreakpoint(breakpoint, currentScreenInfo);
        
        if (breakpoint.isActive)
        {
            activeBreakpoints.add(breakpoint.name);
        }
    }
    
    // Sort active breakpoints by priority
    activeBreakpoints.sort(false); // Sort alphabetically for consistency
}

void BreakpointManager::updateLayoutRules()
{
    currentLayoutRules = calculateLayoutRules();
}

void BreakpointManager::notifyListeners()
{
    // Check for cache hit first
    juce::StringArray cachedBreakpoints;
    LayoutRules cachedRules;
    if (settings.enableCaching && getFromCache(currentScreenInfo, cachedBreakpoints, cachedRules))
    {
        return; // Already notified from cache
    }
    
    // Notify breakpoint changes
    if (activeBreakpoints != previousActiveBreakpoints)
    {
        listeners.call([this](Listener& l) { l.breakpointsChanged(activeBreakpoints); });
    }
    
    // Notify device category changes
    if (currentScreenInfo.category != previousScreenInfo.category)
    {
        listeners.call([this](Listener& l) { 
            l.deviceCategoryChanged(currentScreenInfo.category, previousScreenInfo.category); 
        });
    }
    
    // Notify orientation changes
    if (currentScreenInfo.orientation != previousScreenInfo.orientation)
    {
        listeners.call([this](Listener& l) { 
            l.orientationChanged(currentScreenInfo.orientation, previousScreenInfo.orientation); 
        });
    }
    
    // Notify screen info updates
    listeners.call([this](Listener& l) { l.screenInfoUpdated(currentScreenInfo); });
    
    // Add to cache
    if (settings.enableCaching)
    {
        addToCache(currentScreenInfo, activeBreakpoints, currentLayoutRules);
    }
}

BreakpointManager::ScreenInfo BreakpointManager::detectScreenInfo(juce::Component* component) const
{
    ScreenInfo info;
    
    // Get display information using JUCE 8 API
    auto displays = getDisplays();
    auto primaryDisplay = getPrimaryDisplay();
    
    if (component != nullptr)
    {
        // Get screen info from component's display
        auto componentDisplay = displays.getDisplayForPoint(component->getScreenPosition());
        if (componentDisplay != nullptr)
        {
            auto area = componentDisplay->userArea;
            info.width = area.getWidth();
            info.height = area.getHeight();
            info.dpi = componentDisplay->dpi;
        }
        else
        {
            // Fallback to component size
            info.width = component->getWidth();
            info.height = component->getHeight();
            info.dpi = 96.0f; // Default DPI
        }
    }
    else
    {
        // Use primary display
        if (primaryDisplay != nullptr)
        {
            auto area = primaryDisplay->userArea;
            info.width = area.getWidth();
            info.height = area.getHeight();
            info.dpi = primaryDisplay->dpi;
        }
        else
        {
            // Fallback to INI defaults
            info.width = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
            info.height = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
            info.dpi = 96.0f;
        }
    }
    
    // Calculate derived properties
    info.density = detectScreenDensity();
    info.aspectRatio = calculateAspectRatio(info.width, info.height);
    info.category = detectDeviceCategory(info.width, info.height, info.density);
    info.orientation = detectOrientation(info.width, info.height);
    
    // Physical dimensions
    if (info.dpi > 0.0f)
    {
        info.physicalWidth = static_cast<float>(info.width) / info.dpi;
        info.physicalHeight = static_cast<float>(info.height) / info.dpi;
    }
    
    // Platform information
    info.platformName = detectPlatformName();
    info.deviceModel = detectDeviceModel();
    info.isTouch = detectTouchCapability();
    info.isHighDPI = (info.density >= 2.0f);
    
    // Logical dimensions
    info.logicalWidth = static_cast<int>(info.width / info.density);
    info.logicalHeight = static_cast<int>(info.height / info.density);
    info.scaleFactor = calculateScaleFactor();
    
    return info;
}

float BreakpointManager::detectScreenDensity() const
{
    // Use JUCE 8 display API to get screen density
    auto displays = getDisplays();
    auto primaryDisplay = getPrimaryDisplay();
    
    if (primaryDisplay != nullptr)
    {
        float dpi = primaryDisplay->dpi;
        return dpi / 96.0f; // Standard DPI baseline
    }
    
    return 1.0f; // Default density
}

juce::String BreakpointManager::detectPlatformName() const
{
    #if JUCE_MAC
        return "macOS";
    #elif JUCE_WINDOWS
        return "Windows";
    #elif JUCE_LINUX
        return "Linux";
    #elif JUCE_IOS
        return "iOS";
    #elif JUCE_ANDROID
        return "Android";
    #else
        return "Unknown";
    #endif
}

juce::String BreakpointManager::detectDeviceModel() const
{
    // Platform-specific device model detection
    #if JUCE_IOS
        return juce::SystemStats::getDeviceDescription();
    #elif JUCE_ANDROID
        return juce::SystemStats::getDeviceDescription();
    #else
        return juce::SystemStats::getComputerName();
    #endif
}

bool BreakpointManager::detectTouchCapability() const
{
    #if JUCE_IOS || JUCE_ANDROID
        return true;
    #else
        // Check for touch capability on desktop platforms
        return juce::Desktop::getInstance().getMainMouseSource().isTouch();
    #endif
}

bool BreakpointManager::evaluateBreakpoint(const Breakpoint& breakpoint, const ScreenInfo& screenInfo) const
{
    // Check custom condition first
    if (breakpoint.customCondition)
    {
        return breakpoint.customCondition(screenInfo);
    }
    
    // Evaluate based on breakpoint type
    switch (breakpoint.type)
    {
        case BreakpointType::Width:
        {
            float width = static_cast<float>(screenInfo.logicalWidth);
            return (width >= breakpoint.minValue && width <= breakpoint.maxValue);
        }
        
        case BreakpointType::Height:
        {
            float height = static_cast<float>(screenInfo.logicalHeight);
            return (height >= breakpoint.minValue && height <= breakpoint.maxValue);
        }
        
        case BreakpointType::AspectRatio:
        {
            return (screenInfo.aspectRatio >= breakpoint.minValue && 
                   screenInfo.aspectRatio <= breakpoint.maxValue);
        }
        
        case BreakpointType::Density:
        {
            return (screenInfo.density >= breakpoint.minValue && 
                   screenInfo.density <= breakpoint.maxValue);
        }
        
        case BreakpointType::Custom:
        {
            // Custom breakpoints require custom condition function
            return false;
        }
        
        default:
            return false;
    }
}

void BreakpointManager::sortBreakpointsByPriority()
{
    std::sort(registeredBreakpoints.begin(), registeredBreakpoints.end(),
              [](const Breakpoint& a, const Breakpoint& b) {
                  return a.priority > b.priority; // Higher priority first
              });
}

BreakpointManager::LayoutRules BreakpointManager::calculateLayoutRules() const
{
    LayoutRules rules;
    
    // Start with default rules
    rules.sizing.minButtonWidth = 44.0f;
    rules.sizing.minButtonHeight = 44.0f;
    rules.sizing.minTouchTarget = 44.0f;
    rules.sizing.maxComponentWidth = 0.0f;
    rules.sizing.maxComponentHeight = 0.0f;
    
    rules.spacing.defaultMargin = INIConfig::LayoutConstants::defaultMargin;
    rules.spacing.defaultSpacing = INIConfig::LayoutConstants::defaultSpacing;
    rules.spacing.sectionSpacing = INIConfig::LayoutConstants::defaultSpacing * 2;
    rules.spacing.groupSpacing = INIConfig::LayoutConstants::defaultSpacing * 3;
    
    rules.typography.baseFont = 14.0f;
    rules.typography.titleFont = 18.0f;
    rules.typography.smallFont = 12.0f;
    rules.typography.largeFont = 20.0f;
    rules.typography.lineHeight = 1.2f;
    
    rules.grid.columns = 12;
    rules.grid.gutterWidth = 20;
    rules.grid.maxWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
    
    // Apply rules from active breakpoints (in priority order)
    for (const auto& breakpoint : registeredBreakpoints)
    {
        if (breakpoint.isActive)
        {
            // Apply breakpoint-specific layout rules if they exist
            auto it = breakpointLayoutRules.find(breakpoint.name);
            if (it != breakpointLayoutRules.end())
            {
                const auto& breakpointRules = it->second;
                
                // Merge rules (active breakpoint rules take precedence)
                rules.sizing = breakpointRules.sizing;
                rules.spacing = breakpointRules.spacing;
                rules.typography = breakpointRules.typography;
                rules.grid = breakpointRules.grid;
            }
        }
    }
    
    return rules;
}

float BreakpointManager::calculateScaleFactor() const
{
    float scaleFactor = 1.0f;
    
    // Apply scale factors from active breakpoints
    for (const auto& breakpoint : registeredBreakpoints)
    {
        if (breakpoint.isActive)
        {
            scaleFactor *= breakpoint.scaleFactor;
        }
    }
    
    // Ensure reasonable bounds
    scaleFactor = juce::jlimit(0.5f, 3.0f, scaleFactor);
    
    return scaleFactor;
}

void BreakpointManager::addToCache(const ScreenInfo& screenInfo, const juce::StringArray& breakpoints, 
                                  const LayoutRules& rules) const
{
    if (!settings.enableCaching)
        return;
    
    juce::ScopedLock lock(cacheLock);
    
    // Remove expired entries
    auto currentTime = juce::Time::getMillisecondCounter();
    layoutCache.erase(
        std::remove_if(layoutCache.begin(), layoutCache.end(),
                      [currentTime](const CacheEntry& entry) {
                          return (currentTime - entry.timestamp) > CACHE_EXPIRY_MS;
                      }),
        layoutCache.end());
    
    // Add new entry
    CacheEntry entry;
    entry.screenInfo = screenInfo;
    entry.activeBreakpoints = breakpoints;
    entry.layoutRules = rules;
    entry.timestamp = currentTime;
    
    layoutCache.push_back(entry);
    
    // Limit cache size
    if (layoutCache.size() > static_cast<size_t>(settings.cacheSize))
    {
        layoutCache.erase(layoutCache.begin());
    }
}

bool BreakpointManager::getFromCache(const ScreenInfo& screenInfo, juce::StringArray& breakpoints, 
                                    LayoutRules& rules) const
{
    if (!settings.enableCaching)
        return false;
    
    juce::ScopedLock lock(cacheLock);
    
    auto currentTime = juce::Time::getMillisecondCounter();
    
    // Look for matching cache entry
    for (const auto& entry : layoutCache)
    {
        // Check if entry is still valid
        if ((currentTime - entry.timestamp) > CACHE_EXPIRY_MS)
            continue;
        
        // Check if screen info matches
        if (entry.screenInfo.width == screenInfo.width &&
            entry.screenInfo.height == screenInfo.height &&
            entry.screenInfo.density == screenInfo.density &&
            entry.screenInfo.category == screenInfo.category &&
            entry.screenInfo.orientation == screenInfo.orientation)
        {
            breakpoints = entry.activeBreakpoints;
            rules = entry.layoutRules;
            return true;
        }
    }
    
    return false;
}

void BreakpointManager::clearCache() const
{
    juce::ScopedLock lock(cacheLock);
    layoutCache.clear();
}

//==============================================================================
// ResponsiveComponent Implementation
//==============================================================================

ResponsiveComponent::ResponsiveComponent()
{
    // Register as listener for breakpoint changes
    BreakpointManager::getInstance().addListener(this);
}

ResponsiveComponent::~ResponsiveComponent()
{
    // Unregister listener
    BreakpointManager::getInstance().removeListener(this);
}

void ResponsiveComponent::resized()
{
    // Update responsive layout when component is resized
    updateResponsiveLayout();
}

void ResponsiveComponent::parentSizeChanged()
{
    // Update screen info when parent size changes
    BreakpointManager::getInstance().updateScreenInfo(this);
}

void ResponsiveComponent::breakpointsChanged(const juce::StringArray& activeBreakpoints)
{
    juce::ignoreUnused(activeBreakpoints);
    
    // Schedule responsive layout update
    responsiveLayoutUpdatePending = true;
    
    // Trigger layout update on next message loop iteration
    juce::MessageManager::callAsync([this]() {
        if (responsiveLayoutUpdatePending)
        {
            updateResponsiveLayout();
            responsiveLayoutUpdatePending = false;
        }
    });
}

void ResponsiveComponent::deviceCategoryChanged(BreakpointManager::DeviceCategory newCategory, 
                                               BreakpointManager::DeviceCategory oldCategory)
{
    juce::ignoreUnused(newCategory, oldCategory);
    
    // Update layout for new device category
    updateResponsiveLayout();
}

void ResponsiveComponent::orientationChanged(BreakpointManager::Orientation newOrientation, 
                                            BreakpointManager::Orientation oldOrientation)
{
    juce::ignoreUnused(newOrientation, oldOrientation);
    
    // Update layout for new orientation
    updateResponsiveLayout();
}

void ResponsiveComponent::updateResponsiveLayout()
{
    // Throttle layout updates
    auto currentTime = juce::Time::getMillisecondCounter();
    if (currentTime - lastLayoutUpdate < 16) // 60fps limit
    {
        return;
    }
    
    lastLayoutUpdate = currentTime;
    
    // Trigger resized() to update layout
    resized();
}

juce::Rectangle<int> ResponsiveComponent::getResponsiveChildBounds(int childIndex, 
                                                                  const juce::Rectangle<int>& availableBounds)
{
    juce::ignoreUnused(childIndex);
    
    // Default implementation returns the full available bounds
    // Subclasses should override this method for custom child layout
    return availableBounds;
}

float ResponsiveComponent::getResponsiveFontSize(float baseSize) const
{
    return BreakpointManager::getInstance().calculateResponsiveFontSize(baseSize);
}

int ResponsiveComponent::getResponsiveMargin(int baseMargin) const
{
    return BreakpointManager::getInstance().calculateResponsiveMargin(baseMargin);
}

int ResponsiveComponent::getResponsiveSpacing(int baseSpacing) const
{
    return BreakpointManager::getInstance().calculateResponsiveSpacing(baseSpacing);
}

BreakpointManager::DeviceCategory ResponsiveComponent::getCurrentDeviceCategory() const
{
    return BreakpointManager::getInstance().getCurrentDeviceCategory();
}

BreakpointManager::Orientation ResponsiveComponent::getCurrentOrientation() const
{
    return BreakpointManager::getInstance().getCurrentOrientation();
}

bool ResponsiveComponent::isBreakpointActive(const juce::String& breakpointName) const
{
    return BreakpointManager::getInstance().isBreakpointActive(breakpointName);
}

const BreakpointManager::LayoutRules& ResponsiveComponent::getCurrentLayoutRules() const
{
    return BreakpointManager::getInstance().getCurrentLayoutRules();
}

} // namespace Layout
} // namespace UI
} // namespace OTTO