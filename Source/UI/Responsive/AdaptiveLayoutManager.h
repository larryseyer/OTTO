#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "ResponsiveLayoutManager.h"
#include "ComponentState.h"
#include "INIConfig.h"

/**
 * @class AdaptiveLayoutManager
 * @brief Enhanced responsive layout system for OTTO Phase 9D
 * 
 * Extends the existing ResponsiveLayoutManager with advanced features:
 * - Breakpoint-based responsive design (phone, tablet, desktop)
 * - Orientation-aware layout adaptation
 * - Component priority system for space-constrained layouts
 * - Intelligent content scaling and reflow
 * - Animation-driven layout transitions
 * - Platform-specific optimizations
 * 
 * Provides seamless user experience across all device types and orientations.
 */
class AdaptiveLayoutManager : public ResponsiveLayoutManager {
public:
    /**
     * @enum DeviceType
     * @brief Device categories for responsive design
     */
    enum class DeviceType {
        Phone,      // Small screens (< 600px width)
        Tablet,     // Medium screens (600-1200px width)
        Desktop,    // Large screens (> 1200px width)
        TV,         // Very large screens (> 1920px width)
        Unknown
    };

    /**
     * @enum Orientation
     * @brief Screen orientation
     */
    enum class Orientation {
        Portrait,   // Height > Width
        Landscape,  // Width > Height
        Square      // Width ≈ Height
    };

    /**
     * @enum LayoutMode
     * @brief Different layout modes for various contexts
     */
    enum class LayoutMode {
        Full,       // Full interface with all components
        Compact,    // Compact interface for smaller screens
        Minimal,    // Minimal interface with essential components only
        Touch,      // Touch-optimized interface
        Performance // Performance-optimized interface
    };

    /**
     * @struct Breakpoint
     * @brief Responsive design breakpoint definition
     */
    struct Breakpoint {
        juce::String name;
        int minWidth = 0;
        int maxWidth = INT_MAX;
        int minHeight = 0;
        int maxHeight = INT_MAX;
        DeviceType deviceType = DeviceType::Desktop;
        LayoutMode layoutMode = LayoutMode::Full;
        float scaleFactor = 1.0f;
        bool touchOptimized = false;
        
        Breakpoint() = default;
        Breakpoint(const juce::String& breakpointName, int minW, int maxW, DeviceType device)
            : name(breakpointName), minWidth(minW), maxWidth(maxW), deviceType(device) {}
        
        bool matches(int width, int height) const {
            return width >= minWidth && width <= maxWidth && 
                   height >= minHeight && height <= maxHeight;
        }
    };

    /**
     * @struct ComponentPriority
     * @brief Priority system for component visibility
     */
    struct ComponentPriority {
        juce::String componentId;
        int priority = 5;           // 1 = highest, 10 = lowest
        bool essential = false;     // Always visible regardless of space
        bool collapsible = true;    // Can be collapsed to save space
        bool hideable = true;       // Can be hidden when space is limited
        int minWidth = 50;          // Minimum width when collapsed
        int minHeight = 30;         // Minimum height when collapsed
        
        ComponentPriority() = default;
        ComponentPriority(const juce::String& id, int prio, bool ess = false)
            : componentId(id), priority(prio), essential(ess) {}
    };

    /**
     * @struct AdaptiveSettings
     * @brief Configuration for adaptive layout behavior
     */
    struct AdaptiveSettings {
        bool enableBreakpoints = true;
        bool enableOrientationAdaptation = true;
        bool enableComponentPriority = true;
        bool enableAnimatedTransitions = true;
        bool enableTouchOptimization = true;
        bool enablePerformanceMode = false;
        
        double transitionDuration = 0.3;       // Layout transition duration
        float touchTargetMinSize = 44.0f;      // Minimum touch target size
        float componentSpacing = 8.0f;         // Default component spacing
        float marginSize = 16.0f;              // Default margin size
        
        // Performance thresholds
        double maxLayoutTime = 16.0;           // Maximum layout time (ms)
        int maxVisibleComponents = 50;         // Maximum visible components
        
        AdaptiveSettings() = default;
    };

    /**
     * @struct LayoutConstraints
     * @brief Constraints for adaptive layout calculation
     */
    struct LayoutConstraints {
        juce::Rectangle<int> availableArea;
        DeviceType deviceType = DeviceType::Desktop;
        Orientation orientation = Orientation::Landscape;
        LayoutMode layoutMode = LayoutMode::Full;
        bool touchMode = false;
        float scaleFactor = 1.0f;
        
        // Resource constraints
        int maxComponents = INT_MAX;
        double maxLayoutTime = 16.0;
        int availableMemory = INT_MAX;
        
        LayoutConstraints() = default;
        LayoutConstraints(const juce::Rectangle<int>& area) : availableArea(area) {}
    };

    AdaptiveLayoutManager(const LayoutConfig& config);
    ~AdaptiveLayoutManager() override;

    // Breakpoint management
    void addBreakpoint(const Breakpoint& breakpoint);
    void removeBreakpoint(const juce::String& name);
    void clearBreakpoints();
    juce::Array<Breakpoint> getBreakpoints() const;
    Breakpoint getCurrentBreakpoint() const;
    Breakpoint findBreakpoint(int width, int height) const;

    // Device and orientation detection
    DeviceType detectDeviceType(int width, int height) const;
    Orientation detectOrientation(int width, int height) const;
    DeviceType getCurrentDeviceType() const { return currentDeviceType; }
    Orientation getCurrentOrientation() const { return currentOrientation; }

    // Layout mode management
    void setLayoutMode(LayoutMode mode);
    LayoutMode getLayoutMode() const { return currentLayoutMode; }
    void setAutoLayoutMode(bool enabled);
    bool isAutoLayoutModeEnabled() const { return autoLayoutMode; }

    // Component priority system
    void setComponentPriority(const juce::String& componentId, const ComponentPriority& priority);
    ComponentPriority getComponentPriority(const juce::String& componentId) const;
    void removeComponentPriority(const juce::String& componentId);
    juce::Array<ComponentPriority> getComponentPriorities() const;

    // Adaptive layout calculation
    void updateLayout(int width, int height) override;
    juce::Array<juce::String> calculateVisibleComponents(const LayoutConstraints& constraints) const;
    juce::Rectangle<int> calculateComponentBounds(const juce::String& componentId, 
                                                 const LayoutConstraints& constraints) const;

    // Layout optimization
    void optimizeLayout(const LayoutConstraints& constraints);
    void enablePerformanceMode(bool enabled);
    bool isPerformanceModeEnabled() const { return performanceMode; }

    // Touch optimization
    void enableTouchOptimization(bool enabled);
    bool isTouchOptimizationEnabled() const { return touchOptimization; }
    float getTouchTargetSize() const;
    void adjustForTouchTargets();

    // Animation and transitions
    void setTransitionDuration(double duration);
    double getTransitionDuration() const { return settings.transitionDuration; }
    void enableAnimatedTransitions(bool enabled);
    bool areAnimatedTransitionsEnabled() const { return settings.enableAnimatedTransitions; }

    // Settings management
    void setAdaptiveSettings(const AdaptiveSettings& newSettings);
    AdaptiveSettings getAdaptiveSettings() const { return settings; }

    // Layout analysis
    struct LayoutAnalysis {
        int totalComponents = 0;
        int visibleComponents = 0;
        int hiddenComponents = 0;
        int collapsedComponents = 0;
        double layoutTime = 0.0;
        double memoryUsage = 0.0;
        float spaceUtilization = 0.0f;
        
        void reset() {
            totalComponents = visibleComponents = hiddenComponents = collapsedComponents = 0;
            layoutTime = memoryUsage = 0.0;
            spaceUtilization = 0.0f;
        }
    };
    
    LayoutAnalysis analyzeCurrentLayout() const;
    LayoutAnalysis predictLayout(const LayoutConstraints& constraints) const;

    // State management
    void saveState(ComponentState& state) const override;
    void loadState(const ComponentState& state) override;

    // Listener interface for layout changes
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void breakpointChanged(const Breakpoint& newBreakpoint) {}
        virtual void deviceTypeChanged(DeviceType newDeviceType) {}
        virtual void orientationChanged(Orientation newOrientation) {}
        virtual void layoutModeChanged(LayoutMode newLayoutMode) {}
        virtual void componentVisibilityChanged(const juce::String& componentId, bool visible) {}
        virtual void layoutOptimized(const LayoutAnalysis& analysis) {}
    };
    
    void addListener(Listener* listener);
    void removeListener(Listener* listener);

    // Debug and diagnostics
    void enableDebugMode(bool enabled);
    bool isDebugModeEnabled() const { return debugMode; }
    juce::String getLayoutDebugInfo() const;
    void logLayoutMetrics() const;

private:
    AdaptiveSettings settings;
    
    // Current state
    DeviceType currentDeviceType = DeviceType::Desktop;
    Orientation currentOrientation = Orientation::Landscape;
    LayoutMode currentLayoutMode = LayoutMode::Full;
    Breakpoint currentBreakpoint;
    
    // Configuration
    std::vector<Breakpoint> breakpoints;
    std::unordered_map<juce::String, ComponentPriority> componentPriorities;
    
    // Optimization flags
    bool autoLayoutMode = true;
    bool performanceMode = false;
    bool touchOptimization = false;
    bool debugMode = false;
    
    // Layout cache
    struct LayoutCache {
        LayoutConstraints constraints;
        juce::Array<juce::String> visibleComponents;
        std::unordered_map<juce::String, juce::Rectangle<int>> componentBounds;
        LayoutAnalysis analysis;
        juce::Time cacheTime;
        bool valid = false;
        
        void invalidate() { valid = false; }
    };
    
    mutable LayoutCache layoutCache;
    
    // Animation state
    struct AnimationState {
        bool animating = false;
        juce::Time startTime;
        LayoutConstraints fromConstraints;
        LayoutConstraints toConstraints;
        std::unordered_map<juce::String, juce::Rectangle<int>> fromBounds;
        std::unordered_map<juce::String, juce::Rectangle<int>> toBounds;
        
        void reset() {
            animating = false;
            fromBounds.clear();
            toBounds.clear();
        }
    };
    
    AnimationState animationState;
    
    // Listeners
    juce::ListenerList<Listener> listeners;
    
    // Performance monitoring
    mutable LayoutAnalysis currentAnalysis;
    mutable juce::Time lastLayoutTime;
    
    // Initialization
    void initializeDefaultBreakpoints();
    void initializeDefaultPriorities();
    
    // Layout calculation
    void calculateAdaptiveLayout(const LayoutConstraints& constraints);
    juce::Array<juce::String> prioritizeComponents(const LayoutConstraints& constraints) const;
    void distributeSpace(const juce::Array<juce::String>& components, 
                        const LayoutConstraints& constraints);
    
    // Component management
    bool shouldShowComponent(const juce::String& componentId, const LayoutConstraints& constraints) const;
    bool shouldCollapseComponent(const juce::String& componentId, const LayoutConstraints& constraints) const;
    juce::Rectangle<int> calculateOptimalBounds(const juce::String& componentId, 
                                               const LayoutConstraints& constraints) const;
    
    // Touch optimization helpers
    void adjustBoundsForTouch(juce::Rectangle<int>& bounds) const;
    float calculateTouchTargetScale(const juce::Rectangle<int>& bounds) const;
    void ensureMinimumTouchTarget(juce::Rectangle<int>& bounds) const;
    
    // Animation helpers
    void startLayoutAnimation(const LayoutConstraints& fromConstraints, 
                             const LayoutConstraints& toConstraints);
    void updateLayoutAnimation();
    juce::Rectangle<int> interpolateBounds(const juce::Rectangle<int>& from, 
                                          const juce::Rectangle<int>& to, float progress) const;
    float easeInOutCubic(float t) const;
    
    // Cache management
    bool isLayoutCacheValid(const LayoutConstraints& constraints) const;
    void updateLayoutCache(const LayoutConstraints& constraints) const;
    void invalidateLayoutCache() const;
    
    // Performance optimization
    void optimizeForPerformance();
    void reduceComponentCount(const LayoutConstraints& constraints);
    void simplifyLayout(const LayoutConstraints& constraints);
    
    // Analysis helpers
    void updateLayoutAnalysis() const;
    double measureLayoutTime() const;
    double estimateMemoryUsage() const;
    float calculateSpaceUtilization(const LayoutConstraints& constraints) const;
    
    // Validation helpers
    bool isValidBreakpoint(const Breakpoint& breakpoint) const;
    bool isValidComponentPriority(const ComponentPriority& priority) const;
    bool isValidLayoutConstraints(const LayoutConstraints& constraints) const;
    
    // Notification helpers
    void notifyBreakpointChanged();
    void notifyDeviceTypeChanged();
    void notifyOrientationChanged();
    void notifyLayoutModeChanged();
    void notifyComponentVisibilityChanged(const juce::String& componentId, bool visible);
    void notifyLayoutOptimized();
    
    // Debug helpers
    void debugPrintLayout() const;
    void debugPrintBreakpoints() const;
    void debugPrintComponentPriorities() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdaptiveLayoutManager)
};