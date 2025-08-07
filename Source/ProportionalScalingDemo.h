#pragma once

#include <JuceHeader.h>
#include "ProportionalScalingManager.h"
#include "CustomLookAndFeel.h"

/**
 * Demonstration component showcasing proportional scaling features
 * for JUCE 8 across all platforms including touch optimization
 */
class ProportionalScalingDemo : public juce::Component,
                                public juce::Slider::Listener,
                                public juce::Button::Listener
{
public:
    ProportionalScalingDemo();
    ~ProportionalScalingDemo() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

private:
    // Proportional scaling manager
    std::unique_ptr<ProportionalScalingManager> scalingManager;
    
    // Demo UI components with proportional scaling
    juce::Label titleLabel;
    juce::Label infoLabel;
    juce::Label dimensionsLabel;
    juce::Label scaleLabel;
    juce::Label breakpointLabel;
    
    // Interactive controls
    juce::Slider scalingSlider;
    juce::Label scalingSliderLabel;
    
    juce::Slider widthSlider;
    juce::Label widthSliderLabel;
    
    juce::Slider heightSlider;
    juce::Label heightSliderLabel;
    
    // Touch-optimized buttons
    juce::TextButton smallButton;
    juce::TextButton mediumButton;
    juce::TextButton largeButton;
    juce::TextButton touchOptimizedButton;
    
    // Element demonstration
    std::vector<juce::Rectangle<int>> demoElements;
    std::vector<juce::Colour> elementColors;
    
    // Font demonstration (will be initialized in constructor)
    juce::Font baseFont{juce::FontOptions()};
    juce::Font scaledFont{juce::FontOptions()};
    
    // State tracking
    bool isInitialized = false;
    
    void initializeComponents();
    void setupProportionalScaling();
    void updateProportionalLayout();
    void updateInfoLabels();
    void createDemoElements();
    void paintDemoElements(juce::Graphics& g);
    void paintTouchTargetVisualization(juce::Graphics& g);
    void paintScaleVisualization(juce::Graphics& g);
    
    // Callback handlers
    void onProportionalScaleChanged(const ProportionalScalingManager::ProportionalScale& scale);
    void onDimensionContextChanged(const ProportionalScalingManager::DimensionContext& context);
    void onBreakpointChanged(int width, int height);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProportionalScalingDemo)
};

/**
 * Touch-optimized control component demonstrating proportional scaling
 */
class ProportionalTouchControl : public juce::Component
{
public:
    ProportionalTouchControl(const juce::String& name, ProportionalScalingManager* manager);
    ~ProportionalTouchControl() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    // Value handling
    void setValue(float newValue);
    float getValue() const { return currentValue; }
    
    // Touch target optimization
    void setTouchOptimized(bool enabled) { touchOptimized = enabled; repaint(); }
    bool isTouchOptimized() const { return touchOptimized; }
    
    // Callbacks
    std::function<void(float)> onValueChanged;

private:
    ProportionalScalingManager* scalingManager;
    juce::String controlName;
    
    float currentValue = 0.5f;
    bool isPressed = false;
    bool isHovered = false;
    bool touchOptimized = false;
    
    void updateValueFromMouse(const juce::MouseEvent& event);
    juce::Rectangle<int> getTouchTarget() const;
    juce::Rectangle<int> getVisualBounds() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProportionalTouchControl)
};

/**
 * Responsive grid layout demonstrating proportional element arrangement
 */
class ProportionalGridLayout : public juce::Component
{
public:
    ProportionalGridLayout(ProportionalScalingManager* manager);
    ~ProportionalGridLayout() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Grid configuration
    void setGridSize(int columns, int rows);
    void addGridItem(std::unique_ptr<juce::Component> component);
    void clearGridItems();
    
    // Responsive behavior
    void setAutoResize(bool enabled) { autoResize = enabled; }
    bool isAutoResize() const { return autoResize; }

private:
    ProportionalScalingManager* scalingManager;
    
    int gridColumns = 3;
    int gridRows = 2;
    bool autoResize = true;
    
    juce::OwnedArray<juce::Component> gridItems;
    
    void updateGridLayout();
    juce::Rectangle<int> getGridCellBounds(int column, int row) const;
    void determineOptimalGridSize();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProportionalGridLayout)
};

/**
 * Text scaling demonstration showing proportional font adaptation
 */
class ProportionalTextDemo : public juce::Component
{
public:
    ProportionalTextDemo(ProportionalScalingManager* manager);
    ~ProportionalTextDemo() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    ProportionalScalingManager* scalingManager;
    
    struct TextSample {
        juce::String text;
        int baseFontSize;
        juce::Font::FontStyleFlags style;
        juce::Justification justification;
        juce::Colour color;
    };
    
    std::vector<TextSample> textSamples;
    
    void initializeTextSamples();
    void paintTextSample(juce::Graphics& g, const TextSample& sample, juce::Rectangle<int> bounds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProportionalTextDemo)
};

/**
 * Plugin integration example showing host constraint handling
 */
class ProportionalPluginDemo : public juce::Component
{
public:
    ProportionalPluginDemo(ProportionalScalingManager* manager);
    ~ProportionalPluginDemo() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Plugin host simulation
    void simulateHostConstraints(int maxWidth, int maxHeight, bool resizable);
    void removeHostConstraints();
    
    // Plugin interface
    void updateForHostEnvironment();

private:
    ProportionalScalingManager* scalingManager;
    
    bool hasHostConstraints = false;
    int maxHostWidth = 0;
    int maxHostHeight = 0;
    bool hostAllowsResize = true;
    
    // Plugin UI elements
    juce::Label pluginTitleLabel;
    juce::Slider mainVolumeSlider;
    juce::Slider filterSlider;
    juce::Slider reverbSlider;
    
    juce::TextButton presetButton;
    juce::TextButton settingsButton;
    
    void initializePluginUI();
    void layoutPluginComponents();
    void adaptToHostSize();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProportionalPluginDemo)
};

/**
 * Platform-specific demonstration showing multi-platform adaptation
 */
class ProportionalPlatformDemo : public juce::Component,
                                 public juce::ComboBox::Listener
{
public:
    ProportionalPlatformDemo(ProportionalScalingManager* manager);
    ~ProportionalPlatformDemo() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;

private:
    ProportionalScalingManager* scalingManager;
    
    // Platform simulation
    juce::ComboBox platformSelector;
    juce::Label platformLabel;
    
    // Platform-specific UI adjustments
    juce::Component desktopUI;
    juce::Component mobileUI;
    juce::Component pluginUI;
    
    void initializePlatformSelector();
    void simulatePlatform(const juce::String& platformName);
    void layoutForDesktop();
    void layoutForMobile();
    void layoutForPlugin();
    void updatePlatformSpecificElements();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProportionalPlatformDemo)
};