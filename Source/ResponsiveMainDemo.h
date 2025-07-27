#pragma once

#include <JuceHeader.h>
#include "MainContentComponent.h"
#include "PlatformResponsiveManager.h"
#include "ResponsiveIntegrationComponent.h"

/**
 * Enhanced MainContentComponent demonstrating responsive design integration
 * This shows how to retrofit existing OTTO components with the new responsive system
 */
class ResponsiveMainDemo : public juce::Component
{
public:
    ResponsiveMainDemo(FontManager& fontManager, ColorScheme& colorScheme);
    ~ResponsiveMainDemo() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void visibilityChanged() override;

    // Window management for different platforms
    void prepareForPluginHost();
    void prepareForStandalone();
    void prepareForMobile();

private:
    FontManager& fontManager;
    ColorScheme& colorScheme;
    
    // Core responsive system
    std::unique_ptr<PlatformResponsiveManager> responsiveManager;
    
    // Demo components
    std::unique_ptr<ResponsiveIntegrationComponent> responsiveDemo;
    std::unique_ptr<MainContentComponent> originalMainContent;
    
    // Responsive layout states
    enum class LayoutMode {
        Desktop,
        Tablet,
        Phone,
        PluginCompact
    };
    
    LayoutMode currentLayoutMode = LayoutMode::Desktop;
    
    // Layout areas
    juce::Rectangle<int> demoArea;
    juce::Rectangle<int> mainContentArea;
    
    void updateLayoutMode();
    void applyLayoutMode();
    void setupResponsiveCallbacks();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponsiveMainDemo)
};

/**
 * Application-level responsive setup utility
 * Shows how to configure the responsive system at startup
 */
class ResponsiveApplicationSetup
{
public:
    struct ResponsiveConfig {
        bool enableAutoDetection = true;
        bool enableTouchOptimization = true;
        bool enableAnimations = true;
        float minimumUIScale = 0.5f;
        float maximumUIScale = 4.0f;
        bool enableAccessibilityFeatures = true;
    };

    static void initializeResponsiveSystem(const ResponsiveConfig& config = {});
    static PlatformResponsiveManager& getGlobalResponsiveManager();
    static void setupWindowForPlatform(juce::DocumentWindow& window);
    static void setupPluginEditorForPlatform(juce::AudioProcessorEditor& editor);

private:
    static std::unique_ptr<PlatformResponsiveManager> globalManager;
    ResponsiveApplicationSetup() = delete;
};