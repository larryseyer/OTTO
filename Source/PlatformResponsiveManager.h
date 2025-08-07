#pragma once

#include <JuceHeader.h>
#include "INIConfig.h"

/**
 * Enhanced platform-aware responsive layout manager for OTTO
 * Provides consistent look and feel across all target platforms:
 * macOS Standalone, VST3, AUv3, Clap AU, Windows, Linux, iOS, and Android
 */
class PlatformResponsiveManager 
{
public:
    // Platform detection for platform-specific optimizations
    enum class TargetPlatform {
        macOS_Standalone,
        macOS_VST3,
        macOS_AU,
        macOS_CLAP,
        Windows_Standalone,
        Windows_VST3,
        Windows_CLAP,
        Linux_Standalone,
        Linux_VST3,
        Linux_CLAP,
        iOS_Standalone,
        iOS_AUv3,
        Android_Standalone
    };

    // Device form factor affects layout decisions
    enum class DeviceFormFactor {
        Desktop,        // macOS/Windows/Linux desktop
        Tablet,         // iPad, large Android tablets
        Phone,          // iPhone, Android phones
        PluginHost      // When running as plugin
    };

    // Screen density categories for consistent sizing
    enum class ScreenDensity {
        LDPI,           // ~120dpi (0.75x)
        MDPI,           // ~160dpi (1.0x) - baseline
        HDPI,           // ~240dpi (1.5x)
        XHDPI,          // ~320dpi (2.0x)
        XXHDPI,         // ~480dpi (3.0x)
        XXXHDPI         // ~640dpi (4.0x)
    };

    struct PlatformConfig {
        TargetPlatform platform = TargetPlatform::macOS_Standalone;
        DeviceFormFactor formFactor = DeviceFormFactor::Desktop;
        ScreenDensity density = ScreenDensity::MDPI;
        
        // Base dimensions adapted for platform
        int baseWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
        int baseHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
        
        // Platform-specific scaling constraints
        float minScale = 0.5f;
        float maxScale = 4.0f;
        float preferredScale = 1.0f;
        
        // Touch-specific adaptations
        bool hasTouchInput = false;
        bool hasPhysicalKeyboard = true;
        bool hasHoverCapability = true;
        
        // Platform-specific UI adjustments
        bool useNativeScrollbars = true;
        bool useSystemMenuBar = false;
        bool supportsDarkMode = true;
        bool supportsWindowResize = true;
        
        // Safe area insets (for mobile platforms)
        juce::BorderSize<int> safeAreaInsets{0, 0, 0, 0};
    };

    struct ResponsiveBreakpoints {
        int extraSmall = 320;   // Phone portrait
        int small = 480;        // Phone landscape / small tablet portrait
        int medium = 768;       // Tablet portrait
        int large = 1024;       // Tablet landscape / small desktop
        int extraLarge = 1440;  // Desktop
    };

    struct ResponsiveScale {
        float baseUIScale = 1.0f;
        float textScale = 1.0f;
        float touchTargetScale = 1.0f;
        float iconScale = 1.0f;
        float spacingScale = 1.0f;
    };

    PlatformResponsiveManager();
    explicit PlatformResponsiveManager(const PlatformConfig& config);
    ~PlatformResponsiveManager();

    // Platform detection and configuration
    void autoDetectPlatform();
    void setPlatformConfig(const PlatformConfig& config);
    const PlatformConfig& getPlatformConfig() const { return platformConfig; }
    
    // Screen information
    void updateScreenInfo();
    juce::Rectangle<int> getScreenBounds() const;
    juce::Rectangle<int> getWorkAreaBounds() const;
    juce::Rectangle<int> getSafeAreaBounds() const;
    float getSystemDPIScale() const;
    
    // Responsive scaling
    void updateLayout(int windowWidth, int windowHeight);
    ResponsiveScale calculateOptimalScaling(int windowWidth, int windowHeight) const;
    
    // Dimension scaling methods
    int scaled(int dimension) const;
    float scaled(float dimension) const;
    juce::Point<int> scaled(const juce::Point<int>& point) const;
    juce::Rectangle<int> scaled(const juce::Rectangle<int>& rect) const;
    juce::Font scaled(const juce::Font& font) const;
    
    // Touch-optimized scaling
    int touchTarget(int baseSize) const;
    int minTouchTarget() const;
    
    // Breakpoint-based layout decisions
    bool isExtraSmall() const { return currentBreakpoint <= breakpoints.extraSmall; }
    bool isSmall() const { return currentBreakpoint <= breakpoints.small; }
    bool isMedium() const { return currentBreakpoint <= breakpoints.medium; }
    bool isLarge() const { return currentBreakpoint <= breakpoints.large; }
    bool isExtraLarge() const { return currentBreakpoint > breakpoints.large; }
    
    // Platform-specific layout helpers
    bool shouldUseCompactLayout() const;
    bool shouldShowScrollbars() const;
    bool shouldUseNativeControls() const;
    bool supportsMultiTouch() const;
    bool supportsHover() const;
    
    // Component bounds constrainer for resizable windows
    void setupConstrainer(juce::ComponentBoundsConstrainer& constrainer) const;
    juce::Rectangle<int> getOptimalBounds(int containerWidth, int containerHeight) const;
    juce::Rectangle<int> getCenteredBounds(int containerWidth, int containerHeight) const;
    
    // State management
    juce::ValueTree saveState() const;
    void loadState(const juce::ValueTree& state);
    
    // Color scheme adaptations for platform
    juce::Colour adaptColorForPlatform(const juce::Colour& baseColor) const;
    float getOptimalOpacity(float baseOpacity) const;
    
    // Animation preferences
    bool shouldUseAnimations() const;
    int getAnimationDuration(int baseDuration) const;
    
    // Accessibility
    bool isHighContrastMode() const;
    bool isReducedMotionEnabled() const;
    bool isLargeTextEnabled() const;
    
    // Notification callbacks
    std::function<void(const ResponsiveScale&)> onScaleChanged;
    std::function<void(TargetPlatform)> onPlatformChanged;
    std::function<void(DeviceFormFactor)> onFormFactorChanged;

private:
    PlatformConfig platformConfig;
    ResponsiveBreakpoints breakpoints;
    ResponsiveScale currentScale;
    
    int currentBreakpoint = 0;
    juce::Rectangle<int> screenBounds;
    juce::Rectangle<int> workAreaBounds;
    float systemDPIScale = 1.0f;
    
    mutable juce::String lastError;
    
    // Platform detection helpers
    TargetPlatform detectCurrentPlatform() const;
    DeviceFormFactor detectFormFactor() const;
    ScreenDensity detectScreenDensity() const;
    
    // Scaling calculations
    float calculateBaseScale(int width, int height) const;
    float calculateTextScale() const;
    float calculateTouchTargetScale() const;
    float calculateIconScale() const;
    float calculateSpacingScale() const;
    
    // Platform-specific adaptations
    void applyMacOSAdaptations();
    void applyWindowsAdaptations();
    void applyLinuxAdaptations();
    void applyiOSAdaptations();
    void applyAndroidAdaptations();
    
    // Safe area handling for mobile
    void updateSafeAreaInsets();
    
    // Accessibility detection
    void updateAccessibilitySettings();
    
    // Error handling
    void setError(const juce::String& error) const;
    void clearError() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlatformResponsiveManager)
};

// Global singleton for easy access
class PlatformResponsive {
public:
    static PlatformResponsiveManager& getInstance() {
        static PlatformResponsiveManager instance;
        return instance;
    }
    
    // Convenience static methods
    static int s(int dimension) { return getInstance().scaled(dimension); }
    static float s(float dimension) { return getInstance().scaled(dimension); }
    static juce::Rectangle<int> s(const juce::Rectangle<int>& rect) { return getInstance().scaled(rect); }
    static juce::Font s(const juce::Font& font) { return getInstance().scaled(font); }
    static int touch(int baseSize) { return getInstance().touchTarget(baseSize); }
};

// Convenience macro for scaled values
#define SCALED(value) PlatformResponsive::s(value)
#define TOUCH_TARGET(value) PlatformResponsive::touch(value)