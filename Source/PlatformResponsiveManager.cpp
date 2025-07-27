#include "PlatformResponsiveManager.h"

PlatformResponsiveManager::PlatformResponsiveManager()
{
    clearError();
    autoDetectPlatform();
    updateScreenInfo();
    updateAccessibilitySettings();
}

PlatformResponsiveManager::PlatformResponsiveManager(const PlatformConfig& config)
    : platformConfig(config)
{
    clearError();
    autoDetectPlatform();
    updateScreenInfo();
    updateAccessibilitySettings();
}

PlatformResponsiveManager::~PlatformResponsiveManager()
{
    onScaleChanged = nullptr;
    onPlatformChanged = nullptr;
    onFormFactorChanged = nullptr;
}

void PlatformResponsiveManager::autoDetectPlatform()
{
    auto detectedPlatform = detectCurrentPlatform();
    auto detectedFormFactor = detectFormFactor();
    auto detectedDensity = detectScreenDensity();
    
    if (detectedPlatform != platformConfig.platform || 
        detectedFormFactor != platformConfig.formFactor ||
        detectedDensity != platformConfig.density)
    {
        platformConfig.platform = detectedPlatform;
        platformConfig.formFactor = detectedFormFactor;
        platformConfig.density = detectedDensity;
        
        // Apply platform-specific configurations
        switch (platformConfig.platform) {
            case TargetPlatform::macOS_Standalone:
            case TargetPlatform::macOS_VST3:
            case TargetPlatform::macOS_AU:
            case TargetPlatform::macOS_CLAP:
                applyMacOSAdaptations();
                break;
                
            case TargetPlatform::Windows_Standalone:
            case TargetPlatform::Windows_VST3:
            case TargetPlatform::Windows_CLAP:
                applyWindowsAdaptations();
                break;
                
            case TargetPlatform::Linux_Standalone:
            case TargetPlatform::Linux_VST3:
            case TargetPlatform::Linux_CLAP:
                applyLinuxAdaptations();
                break;
                
            case TargetPlatform::iOS_Standalone:
            case TargetPlatform::iOS_AUv3:
                applyiOSAdaptations();
                break;
                
            case TargetPlatform::Android_Standalone:
                applyAndroidAdaptations();
                break;
        }
        
        if (onPlatformChanged)
            onPlatformChanged(platformConfig.platform);
        if (onFormFactorChanged)
            onFormFactorChanged(platformConfig.formFactor);
    }
}

PlatformResponsiveManager::TargetPlatform PlatformResponsiveManager::detectCurrentPlatform() const
{
#if JUCE_MAC
    #if JUCE_STANDALONE_APPLICATION
        return TargetPlatform::macOS_Standalone;
    #elif JucePlugin_Build_VST3
        return TargetPlatform::macOS_VST3;
    #elif JucePlugin_Build_AU
        return TargetPlatform::macOS_AU;
    #elif JucePlugin_Build_CLAP
        return TargetPlatform::macOS_CLAP;
    #else
        return TargetPlatform::macOS_Standalone;
    #endif
#elif JUCE_WINDOWS
    #if JUCE_STANDALONE_APPLICATION
        return TargetPlatform::Windows_Standalone;
    #elif JucePlugin_Build_VST3
        return TargetPlatform::Windows_VST3;
    #elif JucePlugin_Build_CLAP
        return TargetPlatform::Windows_CLAP;
    #else
        return TargetPlatform::Windows_Standalone;
    #endif
#elif JUCE_LINUX
    #if JUCE_STANDALONE_APPLICATION
        return TargetPlatform::Linux_Standalone;
    #elif JucePlugin_Build_VST3
        return TargetPlatform::Linux_VST3;
    #elif JucePlugin_Build_CLAP
        return TargetPlatform::Linux_CLAP;
    #else
        return TargetPlatform::Linux_Standalone;
    #endif
#elif JUCE_IOS
    #if JUCE_STANDALONE_APPLICATION
        return TargetPlatform::iOS_Standalone;
    #elif JucePlugin_Build_AUv3
        return TargetPlatform::iOS_AUv3;
    #else
        return TargetPlatform::iOS_Standalone;
    #endif
#elif JUCE_ANDROID
    return TargetPlatform::Android_Standalone;
#else
    return TargetPlatform::macOS_Standalone; // fallback
#endif
}

PlatformResponsiveManager::DeviceFormFactor PlatformResponsiveManager::detectFormFactor() const
{
    auto& desktop = juce::Desktop::getInstance();
    auto& displays = desktop.getDisplays();
    
    if (displays.displays.isEmpty())
        return DeviceFormFactor::Desktop;
    
    auto mainDisplay = displays.displays[0];
    auto bounds = mainDisplay.userArea;
    
    // Check if running as plugin
    auto* topLevelWindow = juce::TopLevelWindow::getActiveTopLevelWindow();
    if (topLevelWindow && topLevelWindow->getParentComponent())
    {
        return DeviceFormFactor::PluginHost;
    }
    
#if JUCE_IOS || JUCE_ANDROID
    // Mobile platform detection
    auto minDimension = std::min(bounds.getWidth(), bounds.getHeight());
    auto maxDimension = std::max(bounds.getWidth(), bounds.getHeight());
    
    // Typical phone dimensions
    if (minDimension < 500 || maxDimension < 900)
        return DeviceFormFactor::Phone;
    else
        return DeviceFormFactor::Tablet;
#else
    // Desktop platforms
    return DeviceFormFactor::Desktop;
#endif
}

PlatformResponsiveManager::ScreenDensity PlatformResponsiveManager::detectScreenDensity() const
{
    float dpi = getSystemDPIScale() * 96.0f; // Base DPI
    
    if (dpi <= 130.0f)
        return ScreenDensity::LDPI;
    else if (dpi <= 180.0f)
        return ScreenDensity::MDPI;
    else if (dpi <= 270.0f)
        return ScreenDensity::HDPI;
    else if (dpi <= 400.0f)
        return ScreenDensity::XHDPI;
    else if (dpi <= 560.0f)
        return ScreenDensity::XXHDPI;
    else
        return ScreenDensity::XXXHDPI;
}

void PlatformResponsiveManager::updateScreenInfo()
{
    auto& desktop = juce::Desktop::getInstance();
    auto& displays = desktop.getDisplays();
    
    if (!displays.displays.isEmpty())
    {
        auto mainDisplay = displays.displays[0];
        screenBounds = mainDisplay.totalArea;
        workAreaBounds = mainDisplay.userArea;
        systemDPIScale = mainDisplay.dpi / 96.0f;
    }
    else
    {
        screenBounds = juce::Rectangle<int>(0, 0, 1920, 1080);
        workAreaBounds = screenBounds;
        systemDPIScale = 1.0f;
    }
    
    updateSafeAreaInsets();
}

void PlatformResponsiveManager::updateSafeAreaInsets()
{
#if JUCE_IOS
    // Get safe area insets from iOS
    if (auto* peer = juce::TopLevelWindow::getActiveTopLevelWindow()->getPeer())
    {
        auto safeArea = peer->getFrameSize();
        platformConfig.safeAreaInsets = juce::BorderSize<int>(
            safeArea.getTop(),
            safeArea.getLeft(), 
            safeArea.getBottom(),
            safeArea.getRight()
        );
    }
#elif JUCE_ANDROID
    // Android safe area handling
    platformConfig.safeAreaInsets = juce::BorderSize<int>(24, 0, 0, 0); // Status bar
#else
    // Desktop platforms don't need safe area insets
    platformConfig.safeAreaInsets = juce::BorderSize<int>(0, 0, 0, 0);
#endif
}

void PlatformResponsiveManager::updateLayout(int windowWidth, int windowHeight)
{
    // Determine current breakpoint
    int minDimension = std::min(windowWidth, windowHeight);
    currentBreakpoint = minDimension;
    
    // Calculate optimal scaling
    currentScale = calculateOptimalScaling(windowWidth, windowHeight);
    
    if (onScaleChanged)
        onScaleChanged(currentScale);
    
    clearError();
}

PlatformResponsiveManager::ResponsiveScale PlatformResponsiveManager::calculateOptimalScaling(int windowWidth, int windowHeight) const
{
    ResponsiveScale scale;
    
    // Base UI scaling
    scale.baseUIScale = calculateBaseScale(windowWidth, windowHeight);
    
    // Text scaling with platform considerations
    scale.textScale = calculateTextScale();
    
    // Touch target scaling for mobile platforms
    scale.touchTargetScale = calculateTouchTargetScale();
    
    // Icon scaling
    scale.iconScale = calculateIconScale();
    
    // Spacing scaling
    scale.spacingScale = calculateSpacingScale();
    
    return scale;
}

float PlatformResponsiveManager::calculateBaseScale(int width, int height) const
{
    float widthScale = static_cast<float>(width) / static_cast<float>(platformConfig.baseWidth);
    float heightScale = static_cast<float>(height) / static_cast<float>(platformConfig.baseHeight);
    
    float baseScale = std::min(widthScale, heightScale);
    
    // Apply DPI scaling
    baseScale *= systemDPIScale;
    
    // Clamp to platform limits
    baseScale = juce::jlimit(platformConfig.minScale, platformConfig.maxScale, baseScale);
    
    return baseScale;
}

float PlatformResponsiveManager::calculateTextScale() const
{
    float textScale = currentScale.baseUIScale;
    
    // Platform-specific text scaling
    switch (platformConfig.platform)
    {
        case TargetPlatform::iOS_Standalone:
        case TargetPlatform::iOS_AUv3:
        case TargetPlatform::Android_Standalone:
            // Mobile text should be slightly larger for readability
            textScale *= 1.1f;
            break;
            
        case TargetPlatform::Windows_Standalone:
        case TargetPlatform::Windows_VST3:
        case TargetPlatform::Windows_CLAP:
            // Windows often needs slight adjustment
            textScale *= 1.05f;
            break;
            
        default:
            break;
    }
    
    // Accessibility adjustments
    if (isLargeTextEnabled())
        textScale *= 1.3f;
    
    return textScale;
}

float PlatformResponsiveManager::calculateTouchTargetScale() const
{
    if (!platformConfig.hasTouchInput)
        return currentScale.baseUIScale;
    
    // Touch targets need to be at least 44pt (iOS) / 48dp (Android)
    float minTouchSize = 44.0f; // iOS standard
#if JUCE_ANDROID
    minTouchSize = 48.0f; // Android standard
#endif
    
    float requiredScale = minTouchSize / 32.0f; // Assuming 32px base touch target
    return std::max(currentScale.baseUIScale, requiredScale);
}

float PlatformResponsiveManager::calculateIconScale() const
{
    float iconScale = currentScale.baseUIScale;
    
    // High DPI displays need crisp icons
    if (platformConfig.density >= ScreenDensity::XHDPI)
        iconScale = std::max(iconScale, 2.0f);
    
    return iconScale;
}

float PlatformResponsiveManager::calculateSpacingScale() const
{
    float spacingScale = currentScale.baseUIScale;
    
    // Compact layouts need tighter spacing
    if (shouldUseCompactLayout())
        spacingScale *= 0.8f;
    
    return spacingScale;
}

void PlatformResponsiveManager::applyMacOSAdaptations()
{
    platformConfig.hasTouchInput = false;
    platformConfig.hasPhysicalKeyboard = true;
    platformConfig.hasHoverCapability = true;
    platformConfig.useNativeScrollbars = true;
    platformConfig.useSystemMenuBar = true;
    platformConfig.supportsDarkMode = true;
    platformConfig.supportsWindowResize = true;
    
    // macOS prefers slightly smaller base scale
    platformConfig.preferredScale = 0.95f;
}

void PlatformResponsiveManager::applyWindowsAdaptations()
{
    platformConfig.hasTouchInput = false; // Assume mouse/keyboard unless detected
    platformConfig.hasPhysicalKeyboard = true;
    platformConfig.hasHoverCapability = true;
    platformConfig.useNativeScrollbars = true;
    platformConfig.useSystemMenuBar = false;
    platformConfig.supportsDarkMode = true;
    platformConfig.supportsWindowResize = true;
    
    // Windows often needs slightly larger text
    platformConfig.preferredScale = 1.05f;
}

void PlatformResponsiveManager::applyLinuxAdaptations()
{
    platformConfig.hasTouchInput = false;
    platformConfig.hasPhysicalKeyboard = true;
    platformConfig.hasHoverCapability = true;
    platformConfig.useNativeScrollbars = true;
    platformConfig.useSystemMenuBar = false;
    platformConfig.supportsDarkMode = true;
    platformConfig.supportsWindowResize = true;
    
    platformConfig.preferredScale = 1.0f;
}

void PlatformResponsiveManager::applyiOSAdaptations()
{
    platformConfig.hasTouchInput = true;
    platformConfig.hasPhysicalKeyboard = false;
    platformConfig.hasHoverCapability = false;
    platformConfig.useNativeScrollbars = false; // iOS uses custom scroll indicators
    platformConfig.useSystemMenuBar = false;
    platformConfig.supportsDarkMode = true;
    platformConfig.supportsWindowResize = false;
    
    // iOS needs larger touch targets
    platformConfig.minScale = 0.8f;
    platformConfig.maxScale = 3.0f;
    platformConfig.preferredScale = 1.2f;
}

void PlatformResponsiveManager::applyAndroidAdaptations()
{
    platformConfig.hasTouchInput = true;
    platformConfig.hasPhysicalKeyboard = false;
    platformConfig.hasHoverCapability = false;
    platformConfig.useNativeScrollbars = false;
    platformConfig.useSystemMenuBar = false;
    platformConfig.supportsDarkMode = true;
    platformConfig.supportsWindowResize = false;
    
    // Android Material Design guidelines
    platformConfig.minScale = 0.75f;
    platformConfig.maxScale = 4.0f;
    platformConfig.preferredScale = 1.1f;
}

// Dimension scaling methods
int PlatformResponsiveManager::scaled(int dimension) const
{
    if (dimension <= 0) return dimension;
    return static_cast<int>(dimension * currentScale.baseUIScale + 0.5f);
}

float PlatformResponsiveManager::scaled(float dimension) const
{
    return dimension * currentScale.baseUIScale;
}

juce::Point<int> PlatformResponsiveManager::scaled(const juce::Point<int>& point) const
{
    return juce::Point<int>(scaled(point.x), scaled(point.y));
}

juce::Rectangle<int> PlatformResponsiveManager::scaled(const juce::Rectangle<int>& rect) const
{
    return juce::Rectangle<int>(
        scaled(rect.getX()),
        scaled(rect.getY()),
        scaled(rect.getWidth()),
        scaled(rect.getHeight())
    );
}

juce::Font PlatformResponsiveManager::scaled(const juce::Font& font) const
{
    return font.withHeight(font.getHeight() * currentScale.textScale);
}

int PlatformResponsiveManager::touchTarget(int baseSize) const
{
    if (!platformConfig.hasTouchInput)
        return scaled(baseSize);
    
    int scaledSize = static_cast<int>(baseSize * currentScale.touchTargetScale + 0.5f);
    return std::max(scaledSize, minTouchTarget());
}

int PlatformResponsiveManager::minTouchTarget() const
{
#if JUCE_IOS
    return 44; // iOS Human Interface Guidelines
#elif JUCE_ANDROID
    return 48; // Android Material Design
#else
    return 32; // Desktop fallback
#endif
}

// Layout decision helpers
bool PlatformResponsiveManager::shouldUseCompactLayout() const
{
    return isSmall() || platformConfig.formFactor == DeviceFormFactor::Phone;
}

bool PlatformResponsiveManager::shouldShowScrollbars() const
{
    return platformConfig.useNativeScrollbars && platformConfig.formFactor == DeviceFormFactor::Desktop;
}

bool PlatformResponsiveManager::shouldUseNativeControls() const
{
    return platformConfig.useNativeScrollbars;
}

bool PlatformResponsiveManager::supportsMultiTouch() const
{
    return platformConfig.hasTouchInput;
}

bool PlatformResponsiveManager::supportsHover() const
{
    return platformConfig.hasHoverCapability;
}

// Bounds and constrainer setup
void PlatformResponsiveManager::setupConstrainer(juce::ComponentBoundsConstrainer& constrainer) const
{
    if (!platformConfig.supportsWindowResize)
    {
        constrainer.setSizeLimits(platformConfig.baseWidth, platformConfig.baseHeight,
                                 platformConfig.baseWidth, platformConfig.baseHeight);
        return;
    }
    
    int minWidth = static_cast<int>(platformConfig.baseWidth * platformConfig.minScale);
    int minHeight = static_cast<int>(platformConfig.baseHeight * platformConfig.minScale);
    int maxWidth = static_cast<int>(platformConfig.baseWidth * platformConfig.maxScale);
    int maxHeight = static_cast<int>(platformConfig.baseHeight * platformConfig.maxScale);
    
    constrainer.setSizeLimits(minWidth, minHeight, maxWidth, maxHeight);
    
    // Maintain aspect ratio for better scaling
    float aspectRatio = static_cast<float>(platformConfig.baseWidth) / static_cast<float>(platformConfig.baseHeight);
    constrainer.setFixedAspectRatio(aspectRatio);
}

juce::Rectangle<int> PlatformResponsiveManager::getOptimalBounds(int containerWidth, int containerHeight) const
{
    auto safeBounds = getSafeAreaBounds();
    
    int optimalWidth = scaled(platformConfig.baseWidth);
    int optimalHeight = scaled(platformConfig.baseHeight);
    
    // Ensure we fit within safe area
    optimalWidth = std::min(optimalWidth, safeBounds.getWidth());
    optimalHeight = std::min(optimalHeight, safeBounds.getHeight());
    
    return juce::Rectangle<int>(0, 0, optimalWidth, optimalHeight);
}

juce::Rectangle<int> PlatformResponsiveManager::getCenteredBounds(int containerWidth, int containerHeight) const
{
    auto optimalBounds = getOptimalBounds(containerWidth, containerHeight);
    
    int x = (containerWidth - optimalBounds.getWidth()) / 2;
    int y = (containerHeight - optimalBounds.getHeight()) / 2;
    
    return optimalBounds.withPosition(x, y);
}

juce::Rectangle<int> PlatformResponsiveManager::getScreenBounds() const
{
    return screenBounds;
}

juce::Rectangle<int> PlatformResponsiveManager::getWorkAreaBounds() const
{
    return workAreaBounds;
}

juce::Rectangle<int> PlatformResponsiveManager::getSafeAreaBounds() const
{
    return juce::Rectangle<int>(
        workAreaBounds.getX() + platformConfig.safeAreaInsets.getLeft(),
        workAreaBounds.getY() + platformConfig.safeAreaInsets.getTop(),
        workAreaBounds.getWidth() - platformConfig.safeAreaInsets.getLeftAndRight(),
        workAreaBounds.getHeight() - platformConfig.safeAreaInsets.getTopAndBottom()
    );
}

float PlatformResponsiveManager::getSystemDPIScale() const
{
    return systemDPIScale;
}

// Color and visual adaptations
juce::Colour PlatformResponsiveManager::adaptColorForPlatform(const juce::Colour& baseColor) const
{
    juce::Colour adaptedColor = baseColor;
    
    // High contrast adjustments
    if (isHighContrastMode())
    {
        // Increase contrast for accessibility
        float brightness = adaptedColor.getBrightness();
        if (brightness < 0.5f)
            adaptedColor = adaptedColor.darker(0.2f);
        else
            adaptedColor = adaptedColor.brighter(0.2f);
    }
    
    return adaptedColor;
}

float PlatformResponsiveManager::getOptimalOpacity(float baseOpacity) const
{
    if (isHighContrastMode())
        return std::min(1.0f, baseOpacity + 0.2f);
    
    return baseOpacity;
}

// Animation and accessibility
bool PlatformResponsiveManager::shouldUseAnimations() const
{
    return !isReducedMotionEnabled();
}

int PlatformResponsiveManager::getAnimationDuration(int baseDuration) const
{
    if (isReducedMotionEnabled())
        return 0;
    
    // Mobile platforms prefer slightly faster animations
    if (platformConfig.formFactor == DeviceFormFactor::Phone)
        return static_cast<int>(baseDuration * 0.8f);
    
    return baseDuration;
}

void PlatformResponsiveManager::updateAccessibilitySettings()
{
    // Platform-specific accessibility detection would go here
    // For now, we'll use defaults
}

bool PlatformResponsiveManager::isHighContrastMode() const
{
    // Platform-specific high contrast detection
#if JUCE_MAC
    return false; // Would check NSAccessibilityDisplayShouldIncreaseContrast
#elif JUCE_WINDOWS
    return false; // Would check Windows high contrast setting
#else
    return false;
#endif
}

bool PlatformResponsiveManager::isReducedMotionEnabled() const
{
    // Platform-specific reduced motion detection
#if JUCE_MAC
    return false; // Would check NSAccessibilityDisplayShouldReduceMotion
#elif JUCE_IOS
    return false; // Would check UIAccessibilityIsReduceMotionEnabled
#else
    return false;
#endif
}

bool PlatformResponsiveManager::isLargeTextEnabled() const
{
    // Platform-specific large text detection
    return false; // Would implement platform-specific checks
}

// State management
juce::ValueTree PlatformResponsiveManager::saveState() const
{
    juce::ValueTree state("PlatformResponsive");
    
    // Save current responsive state
    state.setProperty("BaseUIScale", currentScale.baseUIScale, nullptr);
    state.setProperty("TextScale", currentScale.textScale, nullptr);
    state.setProperty("TouchTargetScale", currentScale.touchTargetScale, nullptr);
    state.setProperty("IconScale", currentScale.iconScale, nullptr);
    state.setProperty("SpacingScale", currentScale.spacingScale, nullptr);
    state.setProperty("Platform", static_cast<int>(platformConfig.platform), nullptr);
    state.setProperty("FormFactor", static_cast<int>(platformConfig.formFactor), nullptr);
    state.setProperty("Density", static_cast<int>(platformConfig.density), nullptr);
    
    return state;
}

void PlatformResponsiveManager::loadState(const juce::ValueTree& state)
{
    if (!state.hasType("PlatformResponsive"))
        return;
    
    // Load responsive state
    currentScale.baseUIScale = state.getProperty("BaseUIScale", 1.0f);
    currentScale.textScale = state.getProperty("TextScale", 1.0f);
    currentScale.touchTargetScale = state.getProperty("TouchTargetScale", 1.0f);
    currentScale.iconScale = state.getProperty("IconScale", 1.0f);
    currentScale.spacingScale = state.getProperty("SpacingScale", 1.0f);
    
    auto savedPlatform = static_cast<TargetPlatform>(static_cast<int>(state.getProperty("Platform", 0)));
    auto savedFormFactor = static_cast<DeviceFormFactor>(static_cast<int>(state.getProperty("FormFactor", 0)));
    auto savedDensity = static_cast<ScreenDensity>(static_cast<int>(state.getProperty("Density", 1)));
    
    // Only restore if it matches current platform (avoid issues when moving between platforms)
    if (savedPlatform == platformConfig.platform)
    {
        platformConfig.formFactor = savedFormFactor;
        platformConfig.density = savedDensity;
    }
}

void PlatformResponsiveManager::setPlatformConfig(const PlatformConfig& config)
{
    platformConfig = config;
    updateScreenInfo();
    updateAccessibilitySettings();
}

void PlatformResponsiveManager::setError(const juce::String& error) const
{
    lastError = error;
}

void PlatformResponsiveManager::clearError() const
{
    lastError.clear();
}