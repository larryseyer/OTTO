#pragma once

#include <JuceHeader.h>
#include "INIConfig.h"
#include "ResponsiveLayoutManager.h"
#include "PlatformResponsiveManager.h"

/**
 * Enhanced Proportional Scaling Manager for JUCE 8
 * Provides dimension-based proportional scaling across all platforms:
 * macOS Standalone, VST3, AUv3, Clap AU, Windows, Linux, iOS, and Android
 * 
 * Features:
 * - Base reference dimensions for consistent scaling ratios
 * - Proportional element scaling maintaining visual relationships
 * - DPI and screen density detection with automatic adaptation
 * - Touch-optimized controls with proportional scaling
 * - Responsive breakpoints based on app dimensions
 * - Plugin host constraint handling
 * - Real-time dimension recalculation
 */
class ProportionalScalingManager : public juce::ChangeBroadcaster, public juce::ComponentListener
{
public:
    // Base reference dimensions for proportional scaling calculations
    struct ReferenceConfig {
        int baseWidth = 800;             // Reference width (adjustable based on design)
        int baseHeight = 600;            // Reference height (adjustable based on design)
        float aspectRatio = 4.0f / 3.0f; // Default 4:3 aspect ratio
        float minScaleFactor = 0.25f;    // Minimum scaling (25%)
        float maxScaleFactor = 4.0f;     // Maximum scaling (400%)
        
        // Touch optimization thresholds
        int minTouchTargetPx = 44;       // iOS 44pt minimum
        int minTouchTargetDp = 48;       // Android 48dp minimum
        
        // Breakpoint thresholds based on dimensions
        int extraSmallThreshold = 320;   // Very small screens/windows
        int smallThreshold = 480;        // Small screens/windows
        int mediumThreshold = 768;       // Medium screens/windows
        int largeThreshold = 1024;       // Large screens/windows
        int extraLargeThreshold = 1440;  // Extra large screens/windows
    };

    // Proportional scaling factors for different element types
    struct ProportionalScale {
        float baseScale = 1.0f;          // Overall base scaling factor
        float elementScale = 1.0f;       // UI elements (buttons, sliders)
        float textScale = 1.0f;          // Font sizes
        float iconScale = 1.0f;          // Icons and graphics
        float spacingScale = 1.0f;       // Margins, padding, gaps
        float touchTargetScale = 1.0f;   // Touch-optimized minimum sizes
        float borderScale = 1.0f;        // Border widths and stroke sizes
        
        // Platform-specific multipliers
        float dpiMultiplier = 1.0f;      // DPI-based adjustment
        float densityMultiplier = 1.0f;  // Screen density adjustment
        float platformMultiplier = 1.0f; // Platform-specific adjustment
    };

    // Dimension-based layout context
    struct DimensionContext {
        int currentWidth = 0;
        int currentHeight = 0;
        int containerWidth = 0;
        int containerHeight = 0;
        float aspectRatio = 1.0f;
        
        // Scaling calculations
        float widthScale = 1.0f;
        float heightScale = 1.0f;
        float uniformScale = 1.0f;       // Min of width/height scale
        float proportionalScale = 1.0f;  // Final proportional scale
        
        // Breakpoint classification
        bool isExtraSmall = false;
        bool isSmall = false;
        bool isMedium = false;
        bool isLarge = false;
        bool isExtraLarge = false;
        
        // Touch optimization flags
        bool needsTouchOptimization = false;
        bool hasPhysicalKeyboard = true;
        bool supportsHover = true;
    };

    // Constructor with reference configuration
    explicit ProportionalScalingManager(const ReferenceConfig& config);
    
    // Default constructor
    ProportionalScalingManager();
    ~ProportionalScalingManager() override;

    // Configuration management
    void setReferenceConfig(const ReferenceConfig& config);
    const ReferenceConfig& getReferenceConfig() const { return referenceConfig; }
    
    // Dimension-based scaling updates
    void updateDimensions(int windowWidth, int windowHeight, int containerWidth = 0, int containerHeight = 0);
    void updateFromComponent(juce::Component* component);
    
    // Core proportional scaling methods
    ProportionalScale calculateProportionalScale(int width, int height) const;
    void applyProportionalScale(const ProportionalScale& scale);
    
    // Element scaling with proportional relationships
    int scaledProportional(int dimension) const;
    float scaledProportional(float dimension) const;
    juce::Point<int> scaledProportional(const juce::Point<int>& point) const;
    juce::Rectangle<int> scaledProportional(const juce::Rectangle<int>& rect) const;
    juce::Font scaledProportional(const juce::Font& font) const;
    
    // Type-specific proportional scaling
    int scaledElement(int baseDimension) const;        // UI elements
    int scaledText(int baseFontSize) const;            // Text/fonts
    int scaledIcon(int baseIconSize) const;            // Icons
    int scaledSpacing(int baseSpacing) const;          // Margins/padding
    int scaledBorder(int baseBorderWidth) const;       // Borders/strokes
    
    // Touch-optimized proportional scaling
    int scaledTouchTarget(int baseSize) const;
    int minTouchTarget() const;
    bool isTouchOptimal(int size) const;
    juce::Rectangle<int> ensureTouchTarget(const juce::Rectangle<int>& bounds) const;
    
    // Dimension-aware layout helpers
    juce::Rectangle<int> getProportionalBounds(int containerWidth, int containerHeight) const;
    juce::Rectangle<int> getCenteredProportionalBounds(int containerWidth, int containerHeight) const;
    void setupProportionalConstrainer(juce::ComponentBoundsConstrainer& constrainer);
    
    // Breakpoint system based on app dimensions
    bool isCurrentlyExtraSmall() const { return currentContext.isExtraSmall; }
    bool isCurrentlySmall() const { return currentContext.isSmall; }
    bool isCurrentlyMedium() const { return currentContext.isMedium; }
    bool isCurrentlyLarge() const { return currentContext.isLarge; }
    bool isCurrentlyExtraLarge() const { return currentContext.isExtraLarge; }
    
    // Responsive layout recommendations
    int getRecommendedColumns() const;
    int getRecommendedRows() const;
    bool shouldUseCompactLayout() const;
    bool shouldStackVertically() const;
    bool shouldShowScrollbars() const;
    
    // Plugin host integration
    void setPluginHostConstraints(int maxWidth, int maxHeight, bool isResizable = true);
    void adaptToHostEnvironment();
    bool isConstrainedByHost() const { return hasHostConstraints; }
    juce::Rectangle<int> getHostConstrainedBounds() const;
    
    // Real-time recalculation and updates
    void enableAutoRecalculation(bool enabled = true);
    void recalculateProportions();
    void forceUpdate();
    
    // DPI and density integration
    void updateDPIScale();
    void updateScreenDensity();
    float getEffectiveDPIScale() const;
    PlatformResponsiveManager::ScreenDensity getCurrentDensity() const;
    
    // State management for consistency
    juce::ValueTree saveProportionalState() const;
    void loadProportionalState(const juce::ValueTree& state);
    void resetToReferenceDefaults();
    
    // Current state access
    const DimensionContext& getCurrentContext() const { return currentContext; }
    const ProportionalScale& getCurrentScale() const { return currentScale; }
    float getCurrentProportionalScale() const { return currentContext.proportionalScale; }
    
    // Error handling and validation
    bool validateConfiguration() const;
    juce::String getLastError() const { return lastError; }
    bool hasError() const { return !lastError.isEmpty(); }
    void clearError();
    
    // Event callbacks for responsive updates
    std::function<void(const ProportionalScale&)> onProportionalScaleChanged;
    std::function<void(const DimensionContext&)> onDimensionContextChanged;
    std::function<void(int, int)> onBreakpointChanged;
    
    // Integration with existing responsive systems
    void integrateWithResponsiveManager(ResponsiveLayoutManager* manager);
    void integrateWithPlatformManager(PlatformResponsiveManager* manager);
    
    // Debug and development utilities
    void debugPrintProportionalState() const;
    juce::String getProportionalStateString() const;
    void enableDebugMode(bool enabled = true) { debugMode = enabled; }

private:
    ReferenceConfig referenceConfig;
    DimensionContext currentContext;
    ProportionalScale currentScale;
    
    // Integration with existing managers
    ResponsiveLayoutManager* responsiveManager = nullptr;
    PlatformResponsiveManager* platformManager = nullptr;
    
    // Plugin host constraints
    bool hasHostConstraints = false;
    int maxHostWidth = 0;
    int maxHostHeight = 0;
    bool hostAllowsResize = true;
    
    // Auto-recalculation settings
    bool autoRecalculation = true;
    juce::uint32 lastUpdateTime = 0;
    static constexpr juce::uint32 UPDATE_THROTTLE_MS = 16; // ~60fps throttling
    
    // Error handling
    mutable juce::String lastError;
    bool debugMode = false;
    
    // Internal calculation methods
    DimensionContext calculateDimensionContext(int width, int height, int containerWidth, int containerHeight) const;
    ProportionalScale calculateScaleFactors(const DimensionContext& context) const;
    void updateBreakpointFlags(DimensionContext& context) const;
    void applyDPIAdjustments(ProportionalScale& scale) const;
    void applyPlatformAdjustments(ProportionalScale& scale) const;
    
    // Constraint application
    float applyScaleConstraints(float scale) const;
    int applyDimensionConstraints(int dimension) const;
    
    // Touch optimization helpers
    bool shouldOptimizeForTouch(const DimensionContext& context) const;
    float calculateTouchTargetScale(const DimensionContext& context) const;
    
    // Validation helpers
    bool isValidDimension(int dimension) const;
    bool isValidScale(float scale) const;
    void setError(const juce::String& error) const;
    
    // ComponentListener methods for automatic updates
    void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;
    void componentParentHierarchyChanged(juce::Component& component) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProportionalScalingManager)
};

/**
 * Migration helper for enhancing existing components with proportional scaling
 */
class ResponsiveMigrationHelper
{
public:
    // Enhance existing component with proportional scaling
    static void enhanceExistingComponentProportional(juce::Component* component, 
                                                    ProportionalScalingManager* manager);
    
    // Convert fixed dimensions to proportional
    static juce::Rectangle<int> convertToProportional(const juce::Rectangle<int>& fixedBounds, 
                                                      ProportionalScalingManager* manager);
    
    // Convert fixed font to proportional
    static juce::Font convertToProportional(const juce::Font& fixedFont, 
                                            ProportionalScalingManager* manager);
    
    // Batch convert multiple bounds
    static juce::Array<juce::Rectangle<int>> convertBoundsToProportional(
        const juce::Array<juce::Rectangle<int>>& fixedBounds, 
        ProportionalScalingManager* manager);
};

/**
 * Global singleton for easy access across the application
 */
class ProportionalScaling
{
public:
    static ProportionalScalingManager& getInstance();
    
    // Convenience static methods for quick access
    static int s(int dimension) { return getInstance().scaledProportional(dimension); }
    static float s(float dimension) { return getInstance().scaledProportional(dimension); }
    static juce::Rectangle<int> s(const juce::Rectangle<int>& rect) { return getInstance().scaledProportional(rect); }
    static juce::Font s(const juce::Font& font) { return getInstance().scaledProportional(font); }
    
    // Type-specific scaling shortcuts
    static int element(int dimension) { return getInstance().scaledElement(dimension); }
    static int text(int fontSize) { return getInstance().scaledText(fontSize); }
    static int icon(int iconSize) { return getInstance().scaledIcon(iconSize); }
    static int spacing(int spacing) { return getInstance().scaledSpacing(spacing); }
    static int touch(int baseSize) { return getInstance().scaledTouchTarget(baseSize); }
    
    // Context queries
    static bool isSmallScreen() { return getInstance().isCurrentlySmall(); }
    static bool isMediumScreen() { return getInstance().isCurrentlyMedium(); }
    static bool isLargeScreen() { return getInstance().isCurrentlyLarge(); }
    static bool needsCompactLayout() { return getInstance().shouldUseCompactLayout(); }
};

// Convenience macros for proportional scaling
#define PROPORTIONAL(value) ProportionalScaling::s(value)
#define PROP_ELEMENT(value) ProportionalScaling::element(value)
#define PROP_TEXT(value) ProportionalScaling::text(value)
#define PROP_ICON(value) ProportionalScaling::icon(value)
#define PROP_SPACING(value) ProportionalScaling::spacing(value)
#define PROP_TOUCH(value) ProportionalScaling::touch(value)

// Breakpoint query macros
#define IS_SMALL_SCREEN() ProportionalScaling::isSmallScreen()
#define IS_MEDIUM_SCREEN() ProportionalScaling::isMediumScreen()
#define IS_LARGE_SCREEN() ProportionalScaling::isLargeScreen()
#define NEEDS_COMPACT_LAYOUT() ProportionalScaling::needsCompactLayout()