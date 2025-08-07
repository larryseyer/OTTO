#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ProportionalScalingManager.h"
#include "EnhancedProportionalEditor.h"
#include "JUCE8Compatibility.h"
#include "Row1Component.h"
#include "PlayerTabsComponent.h"
#include "DrumKitSectionComponent.h"
#include "ColorScheme.h"
#include "ResponsiveLayoutManager.h"
#include "PlatformResponsiveManager.h"
#include "FontManager.h"
#include "ComponentState.h"
#include "INIDataManager.h"
#include "CustomLookAndFeel.h"
#include "MainContentComponent.h"

/**
 * Enhanced OTTO Plugin Editor with integrated proportional scaling
 * Maintains compatibility with existing OTTO codebase while adding
 * comprehensive proportional scaling support for all platforms
 */
class ProportionalPluginEditor : public juce::AudioProcessorEditor,
                                public juce::Button::Listener,
                                public juce::Slider::Listener,
                                public juce::ComboBox::Listener,
                                public ColorScheme::Listener,
                                private juce::Timer
{
public:
    ProportionalPluginEditor(OTTOAudioProcessor& processor);
    ~ProportionalPluginEditor() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void themeChanged(const juce::String& newThemeName) override;
    void colorChanged(const juce::String& component, const juce::String& property, const juce::Colour& newColor) override;
    void timerCallback() override;
    
    // State management
    void saveEditorState();
    void loadEditorState();
    
    // Proportional scaling control
    void enableProportionalScaling(bool enabled = true);
    bool isProportionalScalingEnabled() const { return proportionalScalingEnabled; }
    ProportionalScalingManager* getProportionalManager() const { return proportionalManager.get(); }
    
    // Platform integration
    void adaptForCurrentPlatform();
    void setPluginHostConstraints(int maxWidth, int maxHeight, bool resizable = true);
    
    // Debug and development
    void showProportionalScalingDemo(bool show = true);
    void enableDebugVisualization(bool enabled = true);

private:
    // Core components
    OTTOAudioProcessor& audioProcessor;
    
    // Enhanced managers with proportional scaling
    std::unique_ptr<ProportionalScalingManager> proportionalManager;
    std::unique_ptr<ResponsiveLayoutManager> responsiveManager;
    std::unique_ptr<PlatformResponsiveManager> platformManager;
    
    // Existing OTTO components (enhanced with proportional scaling)
    std::unique_ptr<ColorScheme> colorScheme;
    std::unique_ptr<FontManager> fontManager;
    std::unique_ptr<INIDataManager> dataManager;
    std::unique_ptr<CustomLookAndFeel> customLookAndFeel;
    std::unique_ptr<ProportionalLookAndFeel> proportionalLookAndFeel;
    
    // Main UI components wrapped for proportional scaling
    std::unique_ptr<ProportionalComponentWrapper> topBarWrapper;
    std::unique_ptr<ProportionalComponentWrapper> playerTabsWrapper;
    std::unique_ptr<ProportionalComponentWrapper> drumKitSectionWrapper;
    std::unique_ptr<ProportionalComponentWrapper> mainContentWrapper;
    
    // Original components (for compatibility)
    std::unique_ptr<Row1Component> topBar;
    std::unique_ptr<PlayerTabsComponent> playerTabs;
    std::unique_ptr<DrumKitSectionComponent> drumKitSection;
    std::unique_ptr<MainContentComponent> mainContent;
    
    // Proportional scaling demo (optional)
    std::unique_ptr<EnhancedProportionalEditor> proportionalDemo;
    std::unique_ptr<juce::TabbedComponent> demoTabs;
    
    // State and configuration
    ComponentState componentState;
    bool proportionalScalingEnabled = true;
    bool showDemo = false;
    bool debugVisualization = false;
    
    // Layout configuration
    struct LayoutConfig {
        bool maintainAspectRatio = true;
        bool enableTouchOptimization = false;
        bool adaptToHost = true;
        float minScale = 0.3f;
        float maxScale = 3.0f;
        int referenceWidth = 1000;
        int referenceHeight = 750;
    } layoutConfig;
    
    // Initialization methods
    void initializeProportionalScaling();
    void initializeManagers();
    void initializeComponents();
    void setupProportionalWrappers();
    void setupLookAndFeel();
    void setupCallbacks();
    
    // Layout methods
    void updateProportionalLayout();
    void layoutMainInterface();
    void layoutStandardInterface(juce::Rectangle<int> bounds);
    void layoutCompactInterface(juce::Rectangle<int> bounds);
    void layoutDemoInterface();
    void adaptLayoutForPlatform();
    void adaptLayoutForPluginFormat();
    
    // Platform detection and adaptation
    void detectAndAdaptPlatform();
    void detectAndAdaptPluginFormat();
    void applyPlatformSpecificSettings();
    void applyPluginFormatSpecificSettings();
    
    // Component enhancement methods
    void enhanceExistingComponents();
    void enhanceTopBar();
    void enhancePlayerTabs();
    void enhanceDrumKitSection();
    void enhanceMainContent();
    
    // Event handling
    void onProportionalScaleChanged(const ProportionalScalingManager::ProportionalScale& scale);
    void onDimensionContextChanged(const ProportionalScalingManager::DimensionContext& context);
    void onBreakpointChanged(int width, int height);
    void onColorSchemeChanged();
    
    // Utility methods
    void updateComponentFonts();
    void updateComponentColors();
    void updateComponentSizing();
    void refreshAllComponents();
    
    // Debug methods
    void debugPrintLayoutInfo();
    void showLayoutDebugInfo(juce::Graphics& g);
    
    // Host integration
    void handleHostResize();
    void adaptToHostEnvironment();
    void constrainToHostLimits();
    
    // Compatibility layer
    void migrateFromOriginalEditor();
    void preserveOriginalFunctionality();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProportionalPluginEditor)
};

/**
 * Factory class for creating proportional-enhanced OTTO editors
 */
class OTTOEditorFactory
{
public:
    // Create editor based on configuration
    static std::unique_ptr<juce::AudioProcessorEditor> createEditor(OTTOAudioProcessor& processor, 
                                                                    bool useProportionalScaling = true);
    
    // Create editor for specific platform
    static std::unique_ptr<juce::AudioProcessorEditor> createEditorForPlatform(
        OTTOAudioProcessor& processor, 
        PlatformResponsiveManager::TargetPlatform platform);
    
    // Create editor for specific plugin format
    static std::unique_ptr<juce::AudioProcessorEditor> createEditorForPluginFormat(
        OTTOAudioProcessor& processor, 
        const juce::String& pluginFormat);
    
    // Configuration presets
    struct EditorPreset {
        juce::String name;
        ProportionalScalingManager::ReferenceConfig scalingConfig;
        bool enableTouchOptimization;
        bool showDemo;
        bool debugMode;
    };
    
    static std::vector<EditorPreset> getAvailablePresets();
    static std::unique_ptr<juce::AudioProcessorEditor> createEditorFromPreset(
        OTTOAudioProcessor& processor, 
        const EditorPreset& preset);

private:
    static EditorPreset createDesktopPreset();
    static EditorPreset createMobilePreset();
    static EditorPreset createPluginPreset();
    static EditorPreset createDemoPreset();
};

/**
 * Migration utility to help transition existing OTTO editors
 */
class OTTOEditorMigration
{
public:
    // Analyze existing editor and recommend migration strategy
    static juce::String analyzeMigrationNeeds(juce::AudioProcessorEditor* existingEditor);
    
    // Migrate existing editor state to proportional system
    static void migrateEditorState(juce::AudioProcessorEditor* existingEditor, 
                                  ProportionalPluginEditor* newEditor);
    
    // Extract component configuration from existing editor
    static void extractComponentState(juce::AudioProcessorEditor* existingEditor, ComponentState& state);
    
    // Apply extracted state to new proportional editor
    static void applyMigratedState(ProportionalPluginEditor* editor, 
                                  const ComponentState& state);
    
    // Validate migration success
    static bool validateMigration(juce::AudioProcessorEditor* originalEditor, 
                                 ProportionalPluginEditor* migratedEditor);
    
    // Generate migration report
    static juce::String generateMigrationReport(juce::AudioProcessorEditor* originalEditor, 
                                               ProportionalPluginEditor* migratedEditor);

private:
    static void analyzeComponentHierarchy(juce::Component* component, int depth = 0);
    static void extractComponentProperties(juce::Component* component, juce::ValueTree& properties);
    static void applyComponentProperties(juce::Component* component, const juce::ValueTree& properties);
};

/**
 * Convenience macros for OTTO proportional scaling integration
 */

// Initialize proportional scaling for OTTO
#define OTTO_INIT_PROPORTIONAL(processor) \
    OTTOEditorFactory::createEditor(processor, true)

// Create platform-specific OTTO editor
#define OTTO_CREATE_FOR_PLATFORM(processor, platform) \
    OTTOEditorFactory::createEditorForPlatform(processor, platform)

// Create plugin format-specific OTTO editor
#define OTTO_CREATE_FOR_FORMAT(processor, format) \
    OTTOEditorFactory::createEditorForPluginFormat(processor, format)

// Enable debug visualization
#define OTTO_ENABLE_DEBUG(editor) \
    if (auto* propEditor = dynamic_cast<ProportionalPluginEditor*>(editor)) \
        propEditor->enableDebugVisualization(true)

// Show proportional scaling demo
#define OTTO_SHOW_DEMO(editor) \
    if (auto* propEditor = dynamic_cast<ProportionalPluginEditor*>(editor)) \
        propEditor->showProportionalScalingDemo(true)
