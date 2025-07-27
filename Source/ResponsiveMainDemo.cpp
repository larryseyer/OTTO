#include "ResponsiveMainDemo.h"

ResponsiveMainDemo::ResponsiveMainDemo(FontManager& fontManager, ColorScheme& colorScheme)
    : fontManager(fontManager)
    , colorScheme(colorScheme)
{
    // Initialize the responsive system
    responsiveManager = std::make_unique<PlatformResponsiveManager>();
    
    // Create the responsive demo component
    responsiveDemo = ResponsiveComponentFactory::createResponsiveDemo(
        *responsiveManager, fontManager, colorScheme);
    addAndMakeVisible(*responsiveDemo);
    
    // Create the original main content for comparison
    originalMainContent = std::make_unique<MainContentComponent>(fontManager, colorScheme);
    addAndMakeVisible(*originalMainContent);
    
    // Setup responsive callbacks
    setupResponsiveCallbacks();
    
    // Set initial size based on platform
    auto optimalBounds = responsiveManager->getOptimalBounds(800, 600);
    setSize(optimalBounds.getWidth(), optimalBounds.getHeight());
    
    updateLayoutMode();
}

ResponsiveMainDemo::~ResponsiveMainDemo()
{
    // Cleanup is handled by unique_ptr destructors
}

void ResponsiveMainDemo::setupResponsiveCallbacks()
{
    responsiveManager->onScaleChanged = [this](const PlatformResponsiveManager::ResponsiveScale& scale) {
        // Update layout when scale changes
        updateLayoutMode();
        resized();
        repaint();
    };
    
    responsiveManager->onPlatformChanged = [this](PlatformResponsiveManager::TargetPlatform platform) {
        // Adapt to platform changes
        updateLayoutMode();
        resized();
        repaint();
    };
}

void ResponsiveMainDemo::updateLayoutMode()
{
    auto& config = responsiveManager->getPlatformConfig();
    
    if (config.formFactor == PlatformResponsiveManager::DeviceFormFactor::Phone)
    {
        currentLayoutMode = LayoutMode::Phone;
    }
    else if (config.formFactor == PlatformResponsiveManager::DeviceFormFactor::Tablet)
    {
        currentLayoutMode = LayoutMode::Tablet;
    }
    else if (config.formFactor == PlatformResponsiveManager::DeviceFormFactor::PluginHost)
    {
        currentLayoutMode = LayoutMode::PluginCompact;
    }
    else
    {
        currentLayoutMode = LayoutMode::Desktop;
    }
    
    applyLayoutMode();
}

void ResponsiveMainDemo::applyLayoutMode()
{
    switch (currentLayoutMode)
    {
        case LayoutMode::Phone:
            prepareForMobile();
            break;
            
        case LayoutMode::Tablet:
            // Similar to desktop but with touch optimizations
            prepareForStandalone();
            break;
            
        case LayoutMode::PluginCompact:
            prepareForPluginHost();
            break;
            
        case LayoutMode::Desktop:
        default:
            prepareForStandalone();
            break;
    }
}

void ResponsiveMainDemo::prepareForPluginHost()
{
    // Plugin hosts often have limited space
    // Hide demo component in plugin mode for more space
    responsiveDemo->setVisible(false);
    originalMainContent->setVisible(true);
    
    // Use more compact spacing
    if (auto* constrainer = getConstrainer())
    {
        responsiveManager->setupConstrainer(*constrainer);
    }
}

void ResponsiveMainDemo::prepareForStandalone()
{
    // Show both components side by side for comparison
    responsiveDemo->setVisible(true);
    originalMainContent->setVisible(true);
}

void ResponsiveMainDemo::prepareForMobile()
{
    // On mobile, prioritize the responsive demo
    responsiveDemo->setVisible(true);
    originalMainContent->setVisible(false);
    
    // Ensure touch-friendly minimum sizes
    auto minSize = responsiveManager->touchTarget(320);
    setSize(juce::jmax(getWidth(), minSize), juce::jmax(getHeight(), minSize));
}

void ResponsiveMainDemo::paint(juce::Graphics& g)
{
    // Background
    auto backgroundColor = responsiveManager->adaptColorForPlatform(colorScheme.getBackgroundColor());
    g.fillAll(backgroundColor);
    
    // Draw platform indicator in top-right corner
    auto bounds = getLocalBounds();
    auto indicatorArea = bounds.removeFromTop(responsiveManager->scaled(20))
                              .removeFromRight(responsiveManager->scaled(150));
    
    g.setColour(colorScheme.getTextColor().withAlpha(0.7f));
    g.setFont(responsiveManager->scaled(fontManager.getFont(FontManager::FontType::Body).withHeight(12.0f)));
    
    juce::String platformText;
    switch (responsiveManager->getPlatformConfig().platform)
    {
        case PlatformResponsiveManager::TargetPlatform::macOS_Standalone:
            platformText = "macOS";
            break;
        case PlatformResponsiveManager::TargetPlatform::macOS_VST3:
            platformText = "macOS VST3";
            break;
        case PlatformResponsiveManager::TargetPlatform::iOS_Standalone:
            platformText = "iOS";
            break;
        case PlatformResponsiveManager::TargetPlatform::Android_Standalone:
            platformText = "Android";
            break;
        default:
            platformText = "Desktop";
            break;
    }
    
    g.drawText(platformText, indicatorArea, juce::Justification::centredRight);
    
    // Draw separator between demo and main content
    if (responsiveDemo->isVisible() && originalMainContent->isVisible())
    {
        g.setColour(colorScheme.getBorderColor());
        auto separatorX = demoArea.getRight();
        g.fillRect(separatorX, 0, responsiveManager->scaled(1), getHeight());
    }
}

void ResponsiveMainDemo::resized()
{
    auto bounds = getLocalBounds();
    
    // Reserve space for platform indicator
    bounds.removeFromTop(responsiveManager->scaled(20));
    
    switch (currentLayoutMode)
    {
        case LayoutMode::Phone:
            // Full screen for demo on mobile
            demoArea = bounds;
            mainContentArea = juce::Rectangle<int>();
            break;
            
        case LayoutMode::PluginCompact:
            // Full screen for main content in plugin
            demoArea = juce::Rectangle<int>();
            mainContentArea = bounds;
            break;
            
        case LayoutMode::Tablet:
        case LayoutMode::Desktop:
        default:
            // Split screen for comparison
            if (responsiveDemo->isVisible() && originalMainContent->isVisible())
            {
                auto demoWidth = bounds.getWidth() / 2;
                demoArea = bounds.removeFromLeft(demoWidth);
                mainContentArea = bounds;
            }
            else if (responsiveDemo->isVisible())
            {
                demoArea = bounds;
                mainContentArea = juce::Rectangle<int>();
            }
            else
            {
                demoArea = juce::Rectangle<int>();
                mainContentArea = bounds;
            }
            break;
    }
    
    // Apply bounds to components
    if (responsiveDemo->isVisible())
        responsiveDemo->setBounds(demoArea);
    
    if (originalMainContent->isVisible())
        originalMainContent->setBounds(mainContentArea);
    
    // Update responsive manager with new size
    responsiveManager->updateLayout(getWidth(), getHeight());
}

void ResponsiveMainDemo::visibilityChanged()
{
    if (isVisible())
    {
        // Update screen info when becoming visible
        responsiveManager->updateScreenInfo();
        updateLayoutMode();
    }
}

// ResponsiveApplicationSetup implementation
std::unique_ptr<PlatformResponsiveManager> ResponsiveApplicationSetup::globalManager;

void ResponsiveApplicationSetup::initializeResponsiveSystem(const ResponsiveConfig& config)
{
    if (!globalManager)
    {
        PlatformResponsiveManager::PlatformConfig platformConfig;
        
        // Apply configuration
        platformConfig.minScale = config.minimumUIScale;
        platformConfig.maxScale = config.maximumUIScale;
        
        globalManager = std::make_unique<PlatformResponsiveManager>(platformConfig);
        
        // Setup global responsive behaviors
        if (config.enableAnimations)
        {
            // Enable animations if platform supports them
            juce::Desktop::getInstance().setDefaultLookAndFeel(nullptr);
        }
        
        if (config.enableAccessibilityFeatures)
        {
            // Configure accessibility settings
            globalManager->updateScreenInfo();
        }
    }
}

PlatformResponsiveManager& ResponsiveApplicationSetup::getGlobalResponsiveManager()
{
    if (!globalManager)
    {
        initializeResponsiveSystem();
    }
    return *globalManager;
}

void ResponsiveApplicationSetup::setupWindowForPlatform(juce::DocumentWindow& window)
{
    auto& responsiveManager = getGlobalResponsiveManager();
    
    // Setup window constraints
    if (auto* constrainer = window.getConstrainer())
    {
        responsiveManager.setupConstrainer(*constrainer);
    }
    else
    {
        auto newConstrainer = std::make_unique<juce::ComponentBoundsConstrainer>();
        responsiveManager.setupConstrainer(*newConstrainer);
        window.setConstrainer(newConstrainer.release());
    }
    
    // Set optimal initial size
    auto optimalBounds = responsiveManager.getOptimalBounds(800, 600);
    window.setSize(optimalBounds.getWidth(), optimalBounds.getHeight());
    
    // Center window if on desktop
    if (responsiveManager.getPlatformConfig().formFactor == PlatformResponsiveManager::DeviceFormFactor::Desktop)
    {
        window.centreWithSize(optimalBounds.getWidth(), optimalBounds.getHeight());
    }
}

void ResponsiveApplicationSetup::setupPluginEditorForPlatform(juce::AudioProcessorEditor& editor)
{
    auto& responsiveManager = getGlobalResponsiveManager();
    
    // Plugin editors have different constraints
    auto& config = responsiveManager.getPlatformConfig();
    config.formFactor = PlatformResponsiveManager::DeviceFormFactor::PluginHost;
    config.supportsWindowResize = true; // Most modern hosts support resizable editors
    
    // Set appropriate size for plugin
    auto optimalBounds = responsiveManager.getOptimalBounds(400, 300);
    editor.setSize(optimalBounds.getWidth(), optimalBounds.getHeight());
    
    // Make plugin editor resizable if supported
    editor.setResizable(true, true);
}