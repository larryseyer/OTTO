#pragma once

#include <JuceHeader.h>
#include "ProportionalScalingManager.h"
#include "ProportionalScalingDemo.h"
#include "PluginEditor.h"
#include "ResponsiveLayoutManager.h"
#include "PlatformResponsiveManager.h"

/**
 * Enhanced Plugin Editor with integrated proportional scaling
 * for JUCE 8 supporting all platforms and plugin formats
 */
class EnhancedProportionalEditor : public juce::AudioProcessorEditor,
                                  public juce::Timer
{
public:
    explicit EnhancedProportionalEditor(juce::AudioProcessor& processor);
    ~EnhancedProportionalEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Timer callback for smooth updates
    void timerCallback() override;

private:
    // Reference to the audio processor
    juce::AudioProcessor& audioProcessor;
    
    // Proportional scaling system
    std::unique_ptr<ProportionalScalingManager> proportionalManager;
    std::unique_ptr<ResponsiveLayoutManager> responsiveManager;
    std::unique_ptr<PlatformResponsiveManager> platformManager;
    
    // Demo components
    std::unique_ptr<ProportionalScalingDemo> scalingDemo;
    std::unique_ptr<ProportionalTouchControl> touchControl1;
    std::unique_ptr<ProportionalTouchControl> touchControl2;
    std::unique_ptr<ProportionalGridLayout> gridLayout;
    std::unique_ptr<ProportionalTextDemo> textDemo;
    std::unique_ptr<ProportionalPluginDemo> pluginDemo;
    std::unique_ptr<ProportionalPlatformDemo> platformDemo;
    
    // Tab system for demos
    juce::TabbedComponent tabbedDemo;
    
    // Control panel for real-time adjustments
    juce::GroupComponent controlPanel;
    juce::Slider scaleSlider;
    juce::Label scaleLabel;
    juce::Slider widthSlider;
    juce::Label widthLabel;
    juce::Slider heightSlider;
    juce::Label heightLabel;
    juce::ToggleButton touchOptimizationToggle;
    juce::ToggleButton autoScalingToggle;
    juce::ComboBox platformCombo;
    juce::Label platformLabel;
    
    // Info display
    juce::Label infoLabel;
    juce::TextEditor statusDisplay;
    
    // State tracking
    bool isInitialized = false;
    juce::int64 lastUpdateTime = 0;
    
    void initializeProportionalScaling();
    void initializeManagers();
    void initializeDemoComponents();
    void initializeControls();
    void setupIntegration();
    
    void updateProportionalLayout();
    void updateInfoDisplay();
    void updateStatusDisplay();
    
    // Event handlers
    void onProportionalScaleChanged(const ProportionalScalingManager::ProportionalScale& scale);
    void onDimensionContextChanged(const ProportionalScalingManager::DimensionContext& context);
    void onBreakpointChanged(int width, int height);
    
    // Control callbacks
    void scaleSliderChanged();
    void dimensionSliderChanged();
    void touchToggleChanged();
    void autoScalingToggleChanged();
    void platformComboChanged();
    
    // Platform-specific adaptations
    void adaptForMacOS();
    void adaptForWindows();
    void adaptForLinux();
    void adaptForIOS();
    void adaptForAndroid();
    void adaptForPlugin();
    
    // Plugin format detection and adaptation
    void detectPluginFormat();
    void adaptForVST3();
    void adaptForAU();
    void adaptForCLAP();
    void adaptForStandalone();
    
    // Host constraint handling
    void handleHostConstraints();
    void simulateHostEnvironment();
    
    // Utility methods
    juce::String getCurrentPlatformName() const;
    juce::String getCurrentPluginFormatName() const;
    juce::Rectangle<int> getOptimalBounds() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnhancedProportionalEditor)
};

/**
 * Proportional scaling wrapper for existing OTTO components
 * Allows seamless integration with current codebase
 */
class ProportionalComponentWrapper : public juce::Component
{
public:
    ProportionalComponentWrapper(std::unique_ptr<juce::Component> wrappedComponent,
                                ProportionalScalingManager* manager);
    ~ProportionalComponentWrapper() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Access to wrapped component
    juce::Component* getWrappedComponent() const { return wrappedComponent.get(); }
    
    // Proportional scaling control
    void enableProportionalScaling(bool enabled) { proportionalScalingEnabled = enabled; }
    bool isProportionalScalingEnabled() const { return proportionalScalingEnabled; }
    
    // Override methods for proportional behavior
    void setProportionalBounds(const juce::Rectangle<int>& bounds);
    juce::Rectangle<int> getProportionalBounds() const;

private:
    std::unique_ptr<juce::Component> wrappedComponent;
    ProportionalScalingManager* scalingManager;
    bool proportionalScalingEnabled = true;
    
    juce::Rectangle<int> originalBounds;
    juce::Rectangle<int> proportionalBounds;
    
    void updateProportionalBounds();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProportionalComponentWrapper)
};

/**
 * Enhanced Look and Feel with proportional scaling support
 */
class ProportionalLookAndFeel : public juce::LookAndFeel_V4
{
public:
    explicit ProportionalLookAndFeel(ProportionalScalingManager* manager);
    ~ProportionalLookAndFeel() override = default;

    // Override drawing methods to use proportional scaling
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                             const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown) override;
    
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                       bool shouldDrawButtonAsHighlighted,
                       bool shouldDrawButtonAsDown) override;
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override;
    
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float minSliderPos, float maxSliderPos,
                         juce::Slider::SliderStyle style, juce::Slider& slider) override;
    
    void drawLabel(juce::Graphics& g, juce::Label& label) override;
    
    void drawComboBox(juce::Graphics& g, int width, int height,
                     bool isButtonDown, int buttonX, int buttonY,
                     int buttonW, int buttonH, juce::ComboBox& box) override;
    
    void drawTextEditor(juce::Graphics& g, int width, int height,
                       juce::TextEditor& textEditor);
    
    // Font scaling (virtual methods in LookAndFeel_V4)
    juce::Font getTextButtonFont(juce::TextButton& button, int buttonHeight) override;
    juce::Font getLabelFont(juce::Label& label) override;
    juce::Font getComboBoxFont(juce::ComboBox& box) override;
    
    // Touch optimization (virtual methods in LookAndFeel_V4)
    int getSliderThumbRadius(juce::Slider& slider) override;
    int getDefaultScrollbarWidth() override;
    
    // Platform adaptations
    void setPlatformSpecificStyling(PlatformResponsiveManager::TargetPlatform platform);

private:
    ProportionalScalingManager* scalingManager;
    PlatformResponsiveManager::TargetPlatform currentPlatform;
    
    // Cached proportional values
    mutable float cachedScale = 1.0f;
    mutable juce::int64 cacheTime = 0;
    
    float getProportionalScale() const;
    void updateCache() const;
    
    // Platform-specific drawing helpers
    void drawMacOSButton(juce::Graphics& g, juce::Button& button, const juce::Colour& color);
    void drawWindowsButton(juce::Graphics& g, juce::Button& button, const juce::Colour& color);
    void drawLinuxButton(juce::Graphics& g, juce::Button& button, const juce::Colour& color);
    void drawMobileButton(juce::Graphics& g, juce::Button& button, const juce::Colour& color);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProportionalLookAndFeel)
};

/**
 * Integration utility for existing OTTO codebase
 * Provides migration helpers and compatibility functions
 */
class OTTOProportionalIntegration
{
public:
    // Initialize proportional scaling for OTTO
    static void initializeForOTTO(juce::AudioProcessorEditor* editor);
    
    // Enhance existing OTTO components
    static void enhanceOTTOComponent(juce::Component* component, const juce::String& componentType);
    
    // Convert existing layouts to proportional
    static void convertOTTOLayoutToProportional(juce::Component* parent);
    
    // Plugin format specific adaptations
    static void adaptForOTTOPluginFormat(juce::AudioProcessor* processor, juce::AudioProcessorEditor* editor);
    
    // Platform specific optimizations
    static void applyOTTOPlatformOptimizations(juce::Component* component);
    
    // State persistence
    static void saveOTTOProportionalState(juce::ValueTree& state);
    static void loadOTTOProportionalState(const juce::ValueTree& state);
    
    // Utility functions
    static ProportionalScalingManager* getGlobalScalingManager();
    static void setGlobalScalingManager(ProportionalScalingManager* manager);
    static void cleanup(); // Clean up global resources
    
    // Migration helpers
    static juce::Rectangle<int> convertFixedBoundsToProportional(const juce::Rectangle<int>& bounds);
    static juce::Font convertFixedFontToProportional(const juce::Font& font);
    static int convertFixedDimensionToProportional(int dimension);
    
    // Debug and development
    static void enableDebugVisualization(bool enabled);
    static bool isDebugVisualizationEnabled() { return debugVisualizationEnabled; }
    static void printScalingReport();
    static juce::String getIntegrationStatus();

private:
    static ProportionalScalingManager* globalScalingManager;
    static bool debugVisualizationEnabled;
    
    // Helper functions
    static void enhanceSlider(juce::Slider* slider);
    static void enhanceButton(juce::Button* button);
    static void enhanceLabel(juce::Label* label);
    static void enhanceTextEditor(juce::TextEditor* editor);
    static void enhanceComboBox(juce::ComboBox* combo);
    
    // Component type detection
    static juce::String detectComponentType(juce::Component* component);
    
    // Layout analysis
    static void analyzeExistingLayout(juce::Component* parent, int depth = 0);
};

// Convenience macros for OTTO integration
#define OTTO_PROPORTIONAL_INIT(editor) OTTOProportionalIntegration::initializeForOTTO(editor)
#define OTTO_ENHANCE_COMPONENT(comp, type) OTTOProportionalIntegration::enhanceOTTOComponent(comp, type)
#define OTTO_CONVERT_LAYOUT(parent) OTTOProportionalIntegration::convertOTTOLayoutToProportional(parent)
#define OTTO_PROPORTIONAL_BOUNDS(bounds) OTTOProportionalIntegration::convertFixedBoundsToProportional(bounds)
#define OTTO_PROPORTIONAL_FONT(font) OTTOProportionalIntegration::convertFixedFontToProportional(font)
#define OTTO_PROPORTIONAL_SIZE(size) OTTOProportionalIntegration::convertFixedDimensionToProportional(size)