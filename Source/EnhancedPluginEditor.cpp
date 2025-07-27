#include "EnhancedPluginEditor.h"
#include "JUCE8Compatibility.h"

EnhancedPluginEditor::EnhancedPluginEditor(OTTOAudioProcessor& processor)
    : OTTOAudioProcessorEditor(processor)
{
    setupResponsiveSystem();
}

EnhancedPluginEditor::~EnhancedPluginEditor()
{
    // Clean up callbacks
    if (platformResponsiveManager)
    {
        platformResponsiveManager->onScaleChanged = nullptr;
        platformResponsiveManager->onPlatformChanged = nullptr;
    }
    
    // Restore original look and feel
    setLookAndFeel(nullptr);
}

void EnhancedPluginEditor::setupResponsiveSystem()
{
    // Initialize platform responsive manager
    platformResponsiveManager = std::make_unique<PlatformResponsiveManager>();
    
    // Set up for plugin environment
    auto config = platformResponsiveManager->getPlatformConfig();
    config.formFactor = PlatformResponsiveManager::DeviceFormFactor::PluginHost;
    config.supportsWindowResize = true;
    platformResponsiveManager->setPlatformConfig(config);
    
    // Create platform-aware look and feel
    // Note: We need access to fontManager and colorScheme from the base class
    // This would require making them protected in the base class
    // For now, we'll create a basic platform-aware look and feel
    
    // Register for responsive updates
    platformResponsiveManager->onScaleChanged = [this](const PlatformResponsiveManager::ResponsiveScale& scale) {
        handleResponsiveScaleChange(scale);
    };
    
    platformResponsiveManager->onPlatformChanged = [this](PlatformResponsiveManager::TargetPlatform platform) {
        handlePlatformChange(platform);
    };
    
    // Detect touch input
    touchOptimized = platformResponsiveManager->getPlatformConfig().hasTouchInput;
    
    // Set initial responsive size
    auto bounds = platformResponsiveManager->getOptimalBounds(getWidth(), getHeight());
    setSize(bounds.getWidth(), bounds.getHeight());
    
    updateResponsiveLayout();
}

void EnhancedPluginEditor::paint(juce::Graphics& g)
{
    if (responsiveMode)
    {
        // Enhanced responsive painting
        auto backgroundColor = platformResponsiveManager->adaptColorForPlatform(
            juce::Colour(0xFF2D2D2D)); // OTTO's typical background
        g.fillAll(backgroundColor);
        
        // Call parent paint for the main content
        OTTOAudioProcessorEditor::paint(g);
        
        // Add platform-specific visual indicators in debug mode
        #if JUCE_DEBUG
        // Show responsive info in corner
        auto bounds = getLocalBounds();
        auto infoArea = bounds.removeFromTop(platformResponsiveManager->scaled(15))
                             .removeFromRight(platformResponsiveManager->scaled(100));
        
        g.setColour(juce::Colours::yellow.withAlpha(0.7f));
        g.setFont(platformResponsiveManager->scaled(JUCE8_CREATE_FONT(10.0f, juce::Font::plain)));
        
        juce::String info = "Scale: " + juce::String(
            platformResponsiveManager->calculateOptimalScaling(getWidth(), getHeight()).baseUIScale, 2);
        g.drawText(info, infoArea, juce::Justification::centred);
        #endif
    }
    else
    {
        // Fall back to original painting
        OTTOAudioProcessorEditor::paint(g);
    }
}

void EnhancedPluginEditor::resized()
{
    if (responsiveMode)
    {
        updateResponsiveLayout();
    }
    
    // Always call parent resized to maintain existing functionality
    OTTOAudioProcessorEditor::resized();
    
    if (responsiveMode)
    {
        applyPlatformSpecificAdjustments();
    }
}

void EnhancedPluginEditor::visibilityChanged()
{
    OTTOAudioProcessorEditor::visibilityChanged();
    
    if (responsiveMode && isVisible())
    {
        // Update platform info when becoming visible
        platformResponsiveManager->updateScreenInfo();
        updateResponsiveLayout();
    }
}

void EnhancedPluginEditor::updateResponsiveLayout()
{
    if (!platformResponsiveManager)
        return;
    
    // Update responsive manager with current size
    platformResponsiveManager->updateLayout(getWidth(), getHeight());
    
    calculateResponsiveLayout();
}

void EnhancedPluginEditor::calculateResponsiveLayout()
{
    // Enhance existing layout with responsive adjustments
    auto& config = platformResponsiveManager->getPlatformConfig();
    
    // Adjust for different form factors
    if (config.formFactor == PlatformResponsiveManager::DeviceFormFactor::PluginHost)
    {
        // Plugin-specific layout adjustments
        auto minSize = platformResponsiveManager->scaled(300);
        if (getWidth() < minSize || getHeight() < minSize)
        {
            // Switch to ultra-compact mode
            // This would need integration with existing layout logic
        }
    }
    
    // Apply touch optimizations if needed
    if (touchOptimized)
    {
        // Enhance touch targets for all child components
        for (int i = 0; i < getNumChildComponents(); ++i)
        {
            if (auto* child = getChildComponent(i))
            {
                ResponsiveMigrationHelper::enhanceExistingComponent(*child, *platformResponsiveManager);
            }
        }
    }
}

void EnhancedPluginEditor::applyPlatformSpecificAdjustments()
{
    // Apply platform-specific visual adjustments
    auto& config = platformResponsiveManager->getPlatformConfig();
    
    switch (config.platform)
    {
        case PlatformResponsiveManager::TargetPlatform::macOS_VST3:
        case PlatformResponsiveManager::TargetPlatform::macOS_AU:
            // macOS plugin adjustments
            break;
            
        case PlatformResponsiveManager::TargetPlatform::Windows_VST3:
            // Windows plugin adjustments
            break;
            
        case PlatformResponsiveManager::TargetPlatform::iOS_AUv3:
            // iOS AUv3 adjustments
            break;
            
        default:
            break;
    }
}

void EnhancedPluginEditor::handleResponsiveScaleChange(const PlatformResponsiveManager::ResponsiveScale& scale)
{
    // Respond to scale changes
    updateResponsiveLayout();
    repaint();
}

void EnhancedPluginEditor::handlePlatformChange(PlatformResponsiveManager::TargetPlatform platform)
{
    // Respond to platform changes
    touchOptimized = platformResponsiveManager->getPlatformConfig().hasTouchInput;
    updateResponsiveLayout();
    repaint();
}

// ResponsiveMigrationHelper implementation
void ResponsiveMigrationHelper::enhanceExistingComponent(juce::Component& component, 
                                                       PlatformResponsiveManager& responsiveManager)
{
    // Apply responsive enhancements to existing components
    
    // Check if it's a button
    if (auto* button = dynamic_cast<juce::Button*>(&component))
    {
        makeButtonTouchFriendly(*button, responsiveManager);
    }
    
    // Check if it's a slider
    if (auto* slider = dynamic_cast<juce::Slider*>(&component))
    {
        makeSliderTouchFriendly(*slider, responsiveManager);
    }
    
    // Apply general responsive adaptations
    adaptLayoutForPlatform(component, responsiveManager);
}

void ResponsiveMigrationHelper::makeButtonTouchFriendly(juce::Button& button,
                                                      PlatformResponsiveManager& responsiveManager)
{
    if (!responsiveManager.getPlatformConfig().hasTouchInput)
        return;
    
    // Ensure minimum touch target size
    auto currentBounds = button.getBounds();
    auto minTouchSize = responsiveManager.minTouchTarget();
    
    if (currentBounds.getWidth() < minTouchSize || currentBounds.getHeight() < minTouchSize)
    {
        auto newWidth = juce::jmax(currentBounds.getWidth(), minTouchSize);
        auto newHeight = juce::jmax(currentBounds.getHeight(), minTouchSize);
        
        // Center the enlarged button
        auto x = currentBounds.getCentreX() - newWidth / 2;
        auto y = currentBounds.getCentreY() - newHeight / 2;
        
        button.setBounds(x, y, newWidth, newHeight);
    }
}

void ResponsiveMigrationHelper::makeSliderTouchFriendly(juce::Slider& slider,
                                                      PlatformResponsiveManager& responsiveManager)
{
    if (!responsiveManager.getPlatformConfig().hasTouchInput)
        return;
    
    // Adjust slider thumb size for touch
    auto minThumbSize = responsiveManager.minTouchTarget();
    
    // For linear sliders, ensure track is thick enough
    if (slider.getSliderStyle() == juce::Slider::LinearHorizontal ||
        slider.getSliderStyle() == juce::Slider::LinearVertical)
    {
        // This would require custom slider implementation or look and feel
        // to make the track thicker and thumb larger
    }
}

void ResponsiveMigrationHelper::adaptLayoutForPlatform(juce::Component& component,
                                                     PlatformResponsiveManager& responsiveManager)
{
    // Apply general platform adaptations
    auto& config = responsiveManager.getPlatformConfig();
    
    // Adjust component bounds for DPI scaling
    auto currentBounds = component.getBounds();
    auto scale = responsiveManager.calculateOptimalScaling(currentBounds.getWidth(), currentBounds.getHeight());
    
    // Apply responsive margin adjustments
    if (config.hasTouchInput)
    {
        auto extraMargin = responsiveManager.scaled(2);
        component.setBounds(currentBounds.expanded(extraMargin));
    }
}

juce::Font ResponsiveMigrationHelper::scaleFont(const juce::Font& font,
                                              PlatformResponsiveManager& responsiveManager)
{
    return responsiveManager.scaled(font);
}

juce::Colour ResponsiveMigrationHelper::adaptColor(const juce::Colour& color,
                                                 PlatformResponsiveManager& responsiveManager)
{
    return responsiveManager.adaptColorForPlatform(color);
}