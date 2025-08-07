#pragma once

#include <JuceHeader.h>
#include "PlatformResponsiveManager.h"
#include "PlatformAwareLookAndFeel.h"
#include "ColorScheme.h"
#include "FontManager.h"

/**
 * Demonstration component showing how to integrate the new responsive system
 * with existing OTTO components. This shows best practices for creating
 * components that adapt across all platforms.
 */
class ResponsiveIntegrationComponent : public juce::Component
{
public:
    ResponsiveIntegrationComponent(PlatformResponsiveManager& responsiveManager,
                                  std::unique_ptr<PlatformAwareLookAndFeel> lookAndFeel,
                                  FontManager& fontManager,
                                  ColorScheme& colorScheme);
    
    ~ResponsiveIntegrationComponent() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void lookAndFeelChanged() override;

    // Responsive callbacks
    void onResponsiveScaleChanged(const PlatformResponsiveManager::ResponsiveScale& newScale);
    void onPlatformChanged(PlatformResponsiveManager::TargetPlatform newPlatform);

private:
    PlatformResponsiveManager& responsiveManager;
    std::unique_ptr<PlatformAwareLookAndFeel> platformLookAndFeel;
    FontManager& fontManager;
    ColorScheme& colorScheme;

    // Example UI components demonstrating responsive behavior
    std::unique_ptr<juce::TextButton> demoButton;
    std::unique_ptr<juce::Slider> demoSlider;
    std::unique_ptr<juce::Label> titleLabel;
    std::unique_ptr<juce::Label> infoLabel;
    std::unique_ptr<juce::ComboBox> platformSelector;
    
    // Layout areas that adapt to different screen sizes
    juce::Rectangle<int> titleArea;
    juce::Rectangle<int> controlsArea;
    juce::Rectangle<int> infoArea;

    // Current responsive state
    bool compactLayout = false;
    bool touchOptimized = false;
    
    void updateLayout();
    void updatePlatformInfo();
    void setupControls();
    void applyResponsiveStyles();
    
    // Layout calculations for different breakpoints
    void calculateDesktopLayout();
    void calculateTabletLayout();
    void calculatePhoneLayout();
    void calculatePluginLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponsiveIntegrationComponent)
};

/**
 * Factory for creating responsive components
 * Shows how to integrate the responsive system with existing component hierarchies
 */
class ResponsiveComponentFactory
{
public:
    static std::unique_ptr<ResponsiveIntegrationComponent> createResponsiveDemo(
        PlatformResponsiveManager& responsiveManager,
        FontManager& fontManager,
        ColorScheme& colorScheme);

    // Templates for common responsive component patterns
    template<typename ComponentType>
    static std::unique_ptr<ComponentType> createResponsiveComponent(
        PlatformResponsiveManager& responsiveManager,
        FontManager& fontManager,
        ColorScheme& colorScheme);

private:
    ResponsiveComponentFactory() = delete;
};