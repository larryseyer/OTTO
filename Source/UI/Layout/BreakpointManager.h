#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "../../ColorScheme.h"
#include "../../INIConfig.h"
#include "../../ComponentState.h"

/**
 * @file BreakpointManager.h
 * @brief Responsive design breakpoint management system for OTTO JUCE 8 application
 * 
 * This component provides comprehensive responsive design capabilities with
 * intelligent breakpoint detection, adaptive layout management, and
 * cross-platform screen size optimization for professional audio interfaces.
 * 
 * FEATURES:
 * - Intelligent breakpoint detection based on screen dimensions
 * - Adaptive layout scaling for different device categories
 * - Cross-platform screen density adaptation
 * - Dynamic component sizing and positioning
 * - Orientation change handling (portrait/landscape)
 * - Custom breakpoint definitions and rules
 * - Performance-optimized layout calculations
 * - Integration with INI-driven layout system
 * 
 * TECHNICAL SPECIFICATIONS:
 * - Breakpoint categories: Mobile, Tablet, Desktop, Large Desktop
 * - Screen density support: 1x, 1.5x, 2x, 3x, 4x scaling
 * - Orientation detection: Portrait, Landscape, Square
 * - Layout update frequency: 60fps maximum for smooth transitions
 * - Memory-efficient breakpoint caching system
 * - Thread-safe breakpoint calculations
 * 
 * INTEGRATION:
 * - Uses ColorScheme for consistent theming across breakpoints
 * - INI-driven configuration for all breakpoint parameters
 * - Integrates with ComponentScaler for intelligent scaling
 * - Supports ComponentState for persistence
 * 
 * @author OTTO Development Team
 * @version 1.0
 * @date 2025
 */

namespace OTTO {
namespace UI {
namespace Layout {

/**
 * @class BreakpointManager
 * @brief Responsive design breakpoint management with adaptive scaling
 * 
 * This class provides comprehensive breakpoint management for responsive
 * design with intelligent scaling and cross-platform optimization.
 */
class BreakpointManager
{
public:
    /**
     * @enum DeviceCategory
     * @brief Device categories for responsive design
     */
    enum class DeviceCategory {
        Mobile,         ///< Mobile phones (< 768px width)
        Tablet,         ///< Tablets (768px - 1024px width)
        Desktop,        ///< Desktop computers (1024px - 1920px width)
        LargeDesktop,   ///< Large desktop displays (> 1920px width)
        Unknown         ///< Unknown or unsupported device
    };

    /**
     * @enum Orientation
     * @brief Screen orientation types
     */
    enum class Orientation {
        Portrait,       ///< Height > Width
        Landscape,      ///< Width > Height
        Square          ///< Width ≈ Height
    };

    /**
     * @enum BreakpointType
     * @brief Types of responsive breakpoints
     */
    enum class BreakpointType {
        Width,          ///< Width-based breakpoint
        Height,         ///< Height-based breakpoint
        AspectRatio,    ///< Aspect ratio-based breakpoint
        Density,        ///< Screen density-based breakpoint
        Custom          ///< Custom rule-based breakpoint
    };

    /**
     * @struct ScreenInfo
     * @brief Comprehensive screen information
     */
    struct ScreenInfo {
        int width = 0;                          ///< Screen width in pixels
        int height = 0;                         ///< Screen height in pixels
        float density = 1.0f;                   ///< Screen density multiplier
        float aspectRatio = 1.0f;               ///< Width/Height ratio
        DeviceCategory category = DeviceCategory::Unknown;
        Orientation orientation = Orientation::Landscape;
        
        // Physical dimensions (if available)
        float physicalWidth = 0.0f;             ///< Physical width in inches
        float physicalHeight = 0.0f;            ///< Physical height in inches
        float dpi = 96.0f;                      ///< Dots per inch
        
        // Platform information
        juce::String platformName;              ///< Platform identifier
        juce::String deviceModel;               ///< Device model (if available)
        bool isTouch = false;                   ///< Touch capability
        bool isHighDPI = false;                 ///< High DPI display
        
        // Calculated properties
        int logicalWidth = 0;                   ///< Logical width (density-independent)
        int logicalHeight = 0;                  ///< Logical height (density-independent)
        float scaleFactor = 1.0f;               ///< Overall scale factor
    };

    /**
     * @struct Breakpoint
     * @brief Breakpoint definition with conditions and actions
     */
    struct Breakpoint {
        juce::String name;                      ///< Breakpoint identifier
        BreakpointType type = BreakpointType::Width;
        float minValue = 0.0f;                  ///< Minimum value for activation
        float maxValue = std::numeric_limits<float>::max(); ///< Maximum value
        DeviceCategory targetCategory = DeviceCategory::Unknown;
        Orientation targetOrientation = Orientation::Landscape;
        
        // Layout adjustments
        float scaleFactor = 1.0f;               ///< Scale factor for this breakpoint
        int marginAdjustment = 0;               ///< Margin adjustment in pixels
        int spacingAdjustment = 0;              ///< Spacing adjustment in pixels
        float fontSizeMultiplier = 1.0f;        ///< Font size multiplier
        
        // Custom properties
        std::map<juce::String, juce::var> customProperties;
        
        // Activation function
        std::function<bool(const ScreenInfo&)> customCondition;
        
        // Priority (higher values take precedence)
        int priority = 0;
        
        bool isActive = false;                  ///< Current activation state
    };

    /**
     * @struct LayoutRules
     * @brief Layout rules for different breakpoints
     */
    struct LayoutRules {
        // Component sizing rules
        struct {
            float minButtonWidth = 44.0f;       ///< Minimum button width
            float minButtonHeight = 44.0f;      ///< Minimum button height
            float minTouchTarget = 44.0f;       ///< Minimum touch target size
            float maxComponentWidth = 0.0f;     ///< Maximum component width (0 = no limit)
            float maxComponentHeight = 0.0f;    ///< Maximum component height (0 = no limit)
        } sizing;
        
        // Spacing rules
        struct {
            int defaultMargin = 10;             ///< Default margin size
            int defaultSpacing = 5;             ///< Default spacing between components
            int sectionSpacing = 15;            ///< Spacing between sections
            int groupSpacing = 20;              ///< Spacing between groups
        } spacing;
        
        // Typography rules
        struct {
            float baseFont = 14.0f;             ///< Base font size
            float titleFont = 18.0f;            ///< Title font size
            float smallFont = 12.0f;            ///< Small font size
            float largeFont = 20.0f;            ///< Large font size
            float lineHeight = 1.2f;            ///< Line height multiplier
        } typography;
        
        // Grid system
        struct {
            int columns = 12;                   ///< Number of grid columns
            int gutterWidth = 20;               ///< Gutter width between columns
            int maxWidth = 1200;                ///< Maximum container width
        } grid;
    };

    /**
     * @struct ResponsiveSettings
     * @brief Configuration settings for responsive behavior
     */
    struct ResponsiveSettings {
        bool enabled = true;                    ///< Enable responsive behavior
        bool autoDetectBreakpoints = true;     ///< Automatically detect breakpoints
        bool smoothTransitions = true;         ///< Enable smooth layout transitions
        int transitionDuration = 300;          ///< Transition duration in ms
        bool respectSystemSettings = true;     ///< Respect system accessibility settings
        bool enableDebugMode = false;          ///< Enable debug visualization
        
        // Performance settings
        int updateThrottleMs = 16;              ///< Minimum time between updates (60fps)
        bool enableCaching = true;              ///< Enable layout calculation caching
        int cacheSize = 100;                    ///< Maximum cached layouts
        
        // Platform-specific settings
        struct {
            bool adaptToKeyboard = true;        ///< Adapt layout when keyboard appears
            bool handleOrientationChange = true; ///< Handle orientation changes
            bool useSystemDensity = true;       ///< Use system screen density
            bool enableHapticFeedback = true;   ///< Enable haptic feedback for transitions
        } platformSettings;
    };

    // Singleton access
    static BreakpointManager& getInstance();

    // Core Functionality
    /**
     * @brief Update screen information and recalculate breakpoints
     * @param component Component to get screen info from
     * @return Whether breakpoints changed
     */
    bool updateScreenInfo(juce::Component* component = nullptr);

    /**
     * @brief Get current screen information
     * @return Current screen info
     */
    const ScreenInfo& getCurrentScreenInfo() const { return currentScreenInfo; }

    /**
     * @brief Get current device category
     * @return Current device category
     */
    DeviceCategory getCurrentDeviceCategory() const { return currentScreenInfo.category; }

    /**
     * @brief Get current orientation
     * @return Current screen orientation
     */
    Orientation getCurrentOrientation() const { return currentScreenInfo.orientation; }

    /**
     * @brief Check if specific breakpoint is active
     * @param breakpointName Name of breakpoint to check
     * @return Whether breakpoint is active
     */
    bool isBreakpointActive(const juce::String& breakpointName) const;

    /**
     * @brief Get all active breakpoints
     * @return List of active breakpoint names
     */
    juce::StringArray getActiveBreakpoints() const;

    // Breakpoint Management
    /**
     * @brief Register custom breakpoint
     * @param breakpoint Breakpoint definition to register
     * @return Success status
     */
    bool registerBreakpoint(const Breakpoint& breakpoint);

    /**
     * @brief Unregister breakpoint
     * @param breakpointName Name of breakpoint to remove
     * @return Success status
     */
    bool unregisterBreakpoint(const juce::String& breakpointName);

    /**
     * @brief Get breakpoint definition
     * @param breakpointName Name of breakpoint
     * @return Breakpoint definition (empty if not found)
     */
    Breakpoint getBreakpoint(const juce::String& breakpointName) const;

    /**
     * @brief Get all registered breakpoints
     * @return List of all breakpoints
     */
    std::vector<Breakpoint> getAllBreakpoints() const;

    /**
     * @brief Clear all custom breakpoints
     */
    void clearCustomBreakpoints();

    // Layout Rules
    /**
     * @brief Get layout rules for current breakpoint
     * @return Current layout rules
     */
    const LayoutRules& getCurrentLayoutRules() const { return currentLayoutRules; }

    /**
     * @brief Set layout rules for specific breakpoint
     * @param breakpointName Breakpoint name
     * @param rules Layout rules to apply
     */
    void setLayoutRules(const juce::String& breakpointName, const LayoutRules& rules);

    /**
     * @brief Get layout rules for specific breakpoint
     * @param breakpointName Breakpoint name
     * @return Layout rules for breakpoint
     */
    LayoutRules getLayoutRules(const juce::String& breakpointName) const;

    // Responsive Calculations
    /**
     * @brief Calculate responsive size for component
     * @param baseSize Base size in pixels
     * @param component Component to calculate for
     * @return Responsive size
     */
    int calculateResponsiveSize(int baseSize, juce::Component* component = nullptr) const;

    /**
     * @brief Calculate responsive font size
     * @param baseSize Base font size
     * @return Responsive font size
     */
    float calculateResponsiveFontSize(float baseSize) const;

    /**
     * @brief Calculate responsive margin
     * @param baseMargin Base margin size
     * @return Responsive margin
     */
    int calculateResponsiveMargin(int baseMargin) const;

    /**
     * @brief Calculate responsive spacing
     * @param baseSpacing Base spacing size
     * @return Responsive spacing
     */
    int calculateResponsiveSpacing(int baseSpacing) const;

    /**
     * @brief Get optimal grid columns for current breakpoint
     * @return Number of grid columns
     */
    int getOptimalGridColumns() const;

    /**
     * @brief Calculate component bounds for grid system
     * @param gridColumn Grid column (0-based)
     * @param columnSpan Number of columns to span
     * @param containerBounds Container bounds
     * @return Component bounds
     */
    juce::Rectangle<int> calculateGridBounds(int gridColumn, int columnSpan, 
                                           const juce::Rectangle<int>& containerBounds) const;

    // Configuration
    /**
     * @brief Set responsive settings
     * @param settings New settings to apply
     */
    void setSettings(const ResponsiveSettings& settings);

    /**
     * @brief Get current responsive settings
     * @return Current settings
     */
    const ResponsiveSettings& getSettings() const { return settings; }

    /**
     * @brief Enable/disable responsive behavior
     * @param enabled Whether to enable responsive behavior
     */
    void setEnabled(bool enabled);

    /**
     * @brief Check if responsive behavior is enabled
     * @return Whether responsive behavior is enabled
     */
    bool isEnabled() const { return settings.enabled; }

    // Utility Methods
    /**
     * @brief Convert device category to string
     * @param category Device category
     * @return Human-readable category name
     */
    static juce::String deviceCategoryToString(DeviceCategory category);

    /**
     * @brief Convert orientation to string
     * @param orientation Screen orientation
     * @return Human-readable orientation name
     */
    static juce::String orientationToString(Orientation orientation);

    /**
     * @brief Detect device category from dimensions
     * @param width Screen width
     * @param height Screen height
     * @param density Screen density
     * @return Detected device category
     */
    static DeviceCategory detectDeviceCategory(int width, int height, float density = 1.0f);

    /**
     * @brief Detect orientation from dimensions
     * @param width Screen width
     * @param height Screen height
     * @return Detected orientation
     */
    static Orientation detectOrientation(int width, int height);

    /**
     * @brief Calculate aspect ratio
     * @param width Screen width
     * @param height Screen height
     * @return Aspect ratio (width/height)
     */
    static float calculateAspectRatio(int width, int height);

    // State Management
    /**
     * @brief Save breakpoint settings to component state
     * @param state Component state to save to
     */
    void saveToState(ComponentState& state) const;

    /**
     * @brief Load breakpoint settings from component state
     * @param state Component state to load from
     */
    void loadFromState(const ComponentState& state);

    // Listeners
    /**
     * @class Listener
     * @brief Interface for breakpoint change notifications
     */
    class Listener {
    public:
        virtual ~Listener() = default;
        
        /**
         * @brief Called when breakpoints change
         * @param activeBreakpoints List of currently active breakpoints
         */
        virtual void breakpointsChanged(const juce::StringArray& activeBreakpoints) {}
        
        /**
         * @brief Called when device category changes
         * @param newCategory New device category
         * @param oldCategory Previous device category
         */
        virtual void deviceCategoryChanged(DeviceCategory newCategory, DeviceCategory oldCategory) {}
        
        /**
         * @brief Called when orientation changes
         * @param newOrientation New orientation
         * @param oldOrientation Previous orientation
         */
        virtual void orientationChanged(Orientation newOrientation, Orientation oldOrientation) {}
        
        /**
         * @brief Called when screen info is updated
         * @param screenInfo New screen information
         */
        virtual void screenInfoUpdated(const ScreenInfo& screenInfo) {}
    };

    /**
     * @brief Add breakpoint change listener
     * @param listener Listener to add
     */
    void addListener(Listener* listener);

    /**
     * @brief Remove breakpoint change listener
     * @param listener Listener to remove
     */
    void removeListener(Listener* listener);

private:
    BreakpointManager();
    ~BreakpointManager();

    // Core methods
    void initializeDefaultBreakpoints();
    void initializeDefaultLayoutRules();
    void evaluateBreakpoints();
    void updateLayoutRules();
    void notifyListeners();
    
    // Screen detection
    ScreenInfo detectScreenInfo(juce::Component* component) const;
    float detectScreenDensity() const;
    juce::String detectPlatformName() const;
    juce::String detectDeviceModel() const;
    bool detectTouchCapability() const;
    
    // Breakpoint evaluation
    bool evaluateBreakpoint(const Breakpoint& breakpoint, const ScreenInfo& screenInfo) const;
    void sortBreakpointsByPriority();
    
    // Layout calculations
    LayoutRules calculateLayoutRules() const;
    float calculateScaleFactor() const;
    
    // Caching
    struct CacheEntry {
        ScreenInfo screenInfo;
        juce::StringArray activeBreakpoints;
        LayoutRules layoutRules;
        juce::uint32 timestamp;
    };
    
    mutable std::vector<CacheEntry> layoutCache;
    mutable juce::CriticalSection cacheLock;
    
    void addToCache(const ScreenInfo& screenInfo, const juce::StringArray& breakpoints, 
                   const LayoutRules& rules) const;
    bool getFromCache(const ScreenInfo& screenInfo, juce::StringArray& breakpoints, 
                     LayoutRules& rules) const;
    void clearCache() const;

    // Member variables
    ResponsiveSettings settings;
    ScreenInfo currentScreenInfo;
    ScreenInfo previousScreenInfo;
    LayoutRules currentLayoutRules;
    
    // Breakpoint storage
    std::vector<Breakpoint> registeredBreakpoints;
    std::map<juce::String, LayoutRules> breakpointLayoutRules;
    juce::StringArray activeBreakpoints;
    juce::StringArray previousActiveBreakpoints;
    
    // Update throttling
    juce::uint32 lastUpdateTime = 0;
    bool updatePending = false;
    
    // Listeners
    juce::ListenerList<Listener> listeners;
    
    // Constants
    static constexpr int MOBILE_MAX_WIDTH = 768;
    static constexpr int TABLET_MAX_WIDTH = 1024;
    static constexpr int DESKTOP_MAX_WIDTH = 1920;
    static constexpr float SQUARE_ASPECT_TOLERANCE = 0.1f;
    static constexpr int DEFAULT_CACHE_SIZE = 100;
    static constexpr int CACHE_EXPIRY_MS = 5000;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BreakpointManager)
};

/**
 * @class ResponsiveComponent
 * @brief Base class for components with responsive behavior
 */
class ResponsiveComponent : public juce::Component,
                           public BreakpointManager::Listener
{
public:
    ResponsiveComponent();
    ~ResponsiveComponent() override;

    // Component overrides
    void resized() override;
    void parentSizeChanged() override;

    // BreakpointManager::Listener overrides
    void breakpointsChanged(const juce::StringArray& activeBreakpoints) override;
    void deviceCategoryChanged(BreakpointManager::DeviceCategory newCategory, 
                              BreakpointManager::DeviceCategory oldCategory) override;
    void orientationChanged(BreakpointManager::Orientation newOrientation, 
                           BreakpointManager::Orientation oldOrientation) override;

    // Responsive behavior
    /**
     * @brief Update layout for current breakpoints
     * Called automatically when breakpoints change
     */
    virtual void updateResponsiveLayout();

    /**
     * @brief Get responsive bounds for child component
     * @param childIndex Index of child component
     * @param availableBounds Available bounds for layout
     * @return Calculated bounds for child
     */
    virtual juce::Rectangle<int> getResponsiveChildBounds(int childIndex, 
                                                         const juce::Rectangle<int>& availableBounds);

    /**
     * @brief Calculate responsive font size for component
     * @param baseSize Base font size
     * @return Responsive font size
     */
    float getResponsiveFontSize(float baseSize) const;

    /**
     * @brief Calculate responsive margin for component
     * @param baseMargin Base margin size
     * @return Responsive margin
     */
    int getResponsiveMargin(int baseMargin) const;

    /**
     * @brief Calculate responsive spacing for component
     * @param baseSpacing Base spacing size
     * @return Responsive spacing
     */
    int getResponsiveSpacing(int baseSpacing) const;

protected:
    // Helper methods for subclasses
    BreakpointManager::DeviceCategory getCurrentDeviceCategory() const;
    BreakpointManager::Orientation getCurrentOrientation() const;
    bool isBreakpointActive(const juce::String& breakpointName) const;
    const BreakpointManager::LayoutRules& getCurrentLayoutRules() const;

private:
    bool responsiveLayoutUpdatePending = false;
    juce::uint32 lastLayoutUpdate = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponsiveComponent)
};

} // namespace Layout
} // namespace UI
} // namespace OTTO