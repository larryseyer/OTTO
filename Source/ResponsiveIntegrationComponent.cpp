#include "ResponsiveIntegrationComponent.h"

ResponsiveIntegrationComponent::ResponsiveIntegrationComponent(
    PlatformResponsiveManager& responsiveManager,
    std::unique_ptr<PlatformAwareLookAndFeel> lookAndFeel,
    FontManager& fontManager,
    ColorScheme& colorScheme)
    : responsiveManager(responsiveManager)
    , platformLookAndFeel(std::move(lookAndFeel))
    , fontManager(fontManager)
    , colorScheme(colorScheme)
{
    // Set the platform-aware look and feel
    setLookAndFeel(platformLookAndFeel.get());
    
    // Create UI components
    setupControls();
    
    // Register for responsive updates
    responsiveManager.onScaleChanged = [this](const PlatformResponsiveManager::ResponsiveScale& scale) {
        onResponsiveScaleChanged(scale);
    };
    
    responsiveManager.onPlatformChanged = [this](PlatformResponsiveManager::TargetPlatform platform) {
        onPlatformChanged(platform);
    };
    
    // Initial update
    updatePlatformInfo();
    updateLayout();
}

ResponsiveIntegrationComponent::~ResponsiveIntegrationComponent()
{
    // Clear callbacks to avoid dangling pointers
    responsiveManager.onScaleChanged = nullptr;
    responsiveManager.onPlatformChanged = nullptr;
    
    // Clear look and feel
    setLookAndFeel(nullptr);
}

void ResponsiveIntegrationComponent::setupControls()
{
    // Title label
    titleLabel = std::make_unique<juce::Label>("Title", "OTTO Responsive Demo");
    titleLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*titleLabel);
    
    // Demo button with touch-optimized sizing
    demoButton = std::make_unique<juce::TextButton>("Demo Button");
    demoButton->setButtonText("Responsive Button");
    demoButton->onClick = [this]() {
        // Demonstrate responsive feedback
        auto platform = responsiveManager.getPlatformConfig().platform;
        juce::String platformName;
        
        switch (platform) {
            case PlatformResponsiveManager::TargetPlatform::macOS_Standalone:
                platformName = "macOS Standalone";
                break;
            case PlatformResponsiveManager::TargetPlatform::macOS_VST3:
                platformName = "macOS VST3";
                break;
            case PlatformResponsiveManager::TargetPlatform::iOS_Standalone:
                platformName = "iOS Standalone";
                break;
            case PlatformResponsiveManager::TargetPlatform::Android_Standalone:
                platformName = "Android Standalone";
                break;
            default:
                platformName = "Unknown Platform";
                break;
        }
        
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon,
            "Platform Info",
            "Currently running on: " + platformName
        );
    };
    addAndMakeVisible(*demoButton);
    
    // Demo slider with touch-friendly thumb
    demoSlider = std::make_unique<juce::Slider>("Demo Slider");
    demoSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    demoSlider->setRange(0.0, 100.0, 1.0);
    demoSlider->setValue(50.0);
    demoSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 
                               responsiveManager.scaled(80), 
                               responsiveManager.scaled(20));
    addAndMakeVisible(*demoSlider);
    
    // Platform selector combo box
    platformSelector = std::make_unique<juce::ComboBox>("Platform Info");
    platformSelector->setEditableText(false);
    platformSelector->setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(*platformSelector);
    
    // Info label showing current responsive state
    infoLabel = std::make_unique<juce::Label>("Info", "");
    infoLabel->setJustificationType(juce::Justification::topLeft);
    infoLabel->setColour(juce::Label::textColourId, colorScheme.getTextColor());
    addAndMakeVisible(*infoLabel);
}

void ResponsiveIntegrationComponent::paint(juce::Graphics& g)
{
    // Background with platform-adapted color
    auto backgroundColor = responsiveManager.adaptColorForPlatform(colorScheme.getBackgroundColor());
    g.fillAll(backgroundColor);
    
    // Draw platform-appropriate separators
    auto separatorColor = colorScheme.getBorderColor();
    g.setColour(separatorColor);
    
    // Responsive line thickness
    float lineThickness = responsiveManager.scaled(1.0f);
    
    // Draw separators between sections
    if (!compactLayout)
    {
        // Horizontal separator between title and controls
        g.fillRect(titleArea.getBottom(), 0, getWidth(), static_cast<int>(lineThickness));
        
        // Horizontal separator between controls and info
        g.fillRect(controlsArea.getBottom(), 0, getWidth(), static_cast<int>(lineThickness));
    }
    
    // Draw platform-specific styling indicators
    if (responsiveManager.getPlatformConfig().hasTouchInput)
    {
        // Show touch indicators
        g.setColour(colorScheme.getAccentColor().withAlpha(0.3f));
        auto cornerRadius = responsiveManager.scaled(8.0f);
        
        for (auto* component : getChildren())
        {
            if (auto* button = dynamic_cast<juce::Button*>(component))
            {
                auto bounds = button->getBounds().toFloat().expanded(responsiveManager.scaled(4.0f));
                g.drawRoundedRectangle(bounds, cornerRadius, responsiveManager.scaled(2.0f));
            }
        }
    }
}

void ResponsiveIntegrationComponent::resized()
{
    updateLayout();
}

void ResponsiveIntegrationComponent::updateLayout()
{
    auto bounds = getLocalBounds();
    
    // Determine layout type based on size and platform
    compactLayout = responsiveManager.shouldUseCompactLayout();
    touchOptimized = responsiveManager.getPlatformConfig().hasTouchInput;
    
    if (responsiveManager.getPlatformConfig().formFactor == PlatformResponsiveManager::DeviceFormFactor::Phone)
    {
        calculatePhoneLayout();
    }
    else if (responsiveManager.getPlatformConfig().formFactor == PlatformResponsiveManager::DeviceFormFactor::Tablet)
    {
        calculateTabletLayout();
    }
    else if (responsiveManager.getPlatformConfig().formFactor == PlatformResponsiveManager::DeviceFormFactor::PluginHost)
    {
        calculatePluginLayout();
    }
    else
    {
        calculateDesktopLayout();
    }
    
    // Apply the calculated layouts
    if (titleLabel)
        titleLabel->setBounds(titleArea);
    
    if (demoButton && demoSlider && platformSelector && infoLabel)
    {
        auto controlsBounds = controlsArea;
        auto margin = responsiveManager.scaled(8);
        auto spacing = responsiveManager.scaled(12);
        
        if (compactLayout)
        {
            // Vertical layout for compact screens
            auto itemHeight = touchOptimized ? responsiveManager.touchTarget(32) : responsiveManager.scaled(28);
            
            demoButton->setBounds(controlsBounds.removeFromTop(itemHeight).reduced(margin, 0));
            controlsBounds.removeFromTop(spacing);
            
            demoSlider->setBounds(controlsBounds.removeFromTop(itemHeight + responsiveManager.scaled(20)).reduced(margin, 0));
            controlsBounds.removeFromTop(spacing);
            
            platformSelector->setBounds(controlsBounds.removeFromTop(itemHeight).reduced(margin, 0));
        }
        else
        {
            // Horizontal layout for larger screens
            auto buttonWidth = responsiveManager.scaled(150);
            auto sliderWidth = responsiveManager.scaled(200);
            auto comboWidth = responsiveManager.scaled(180);
            
            auto top = controlsBounds.getY() + margin;
            auto height = touchOptimized ? responsiveManager.touchTarget(32) : responsiveManager.scaled(28);
            
            auto x = controlsBounds.getX() + margin;
            demoButton->setBounds(x, top, buttonWidth, height);
            
            x += buttonWidth + spacing;
            demoSlider->setBounds(x, top, sliderWidth, height + responsiveManager.scaled(20));
            
            x += sliderWidth + spacing;
            platformSelector->setBounds(x, top, comboWidth, height);
        }
        
        infoLabel->setBounds(infoArea.reduced(margin));
    }
    
    applyResponsiveStyles();
}

void ResponsiveIntegrationComponent::calculateDesktopLayout()
{
    auto bounds = getLocalBounds();
    auto titleHeight = responsiveManager.scaled(40);
    auto controlsHeight = responsiveManager.scaled(80);
    
    titleArea = bounds.removeFromTop(titleHeight);
    controlsArea = bounds.removeFromTop(controlsHeight);
    infoArea = bounds; // Remaining space
}

void ResponsiveIntegrationComponent::calculateTabletLayout()
{
    auto bounds = getLocalBounds();
    auto titleHeight = responsiveManager.scaled(50);
    auto controlsHeight = compactLayout ? responsiveManager.scaled(140) : responsiveManager.scaled(90);
    
    titleArea = bounds.removeFromTop(titleHeight);
    controlsArea = bounds.removeFromTop(controlsHeight);
    infoArea = bounds;
}

void ResponsiveIntegrationComponent::calculatePhoneLayout()
{
    auto bounds = getLocalBounds();
    auto titleHeight = responsiveManager.scaled(45);
    auto controlsHeight = responsiveManager.scaled(160); // More space for vertical layout
    
    titleArea = bounds.removeFromTop(titleHeight);
    controlsArea = bounds.removeFromTop(controlsHeight);
    infoArea = bounds;
    
    compactLayout = true; // Force compact layout on phones
}

void ResponsiveIntegrationComponent::calculatePluginLayout()
{
    // Plugin layout is often constrained, so use compact design
    auto bounds = getLocalBounds();
    auto titleHeight = responsiveManager.scaled(30);
    auto controlsHeight = responsiveManager.scaled(60);
    
    titleArea = bounds.removeFromTop(titleHeight);
    controlsArea = bounds.removeFromTop(controlsHeight);
    infoArea = bounds;
    
    compactLayout = true;
}

void ResponsiveIntegrationComponent::applyResponsiveStyles()
{
    if (!titleLabel || !demoButton || !demoSlider || !platformSelector || !infoLabel)
        return;
    
    // Apply responsive fonts
    auto titleFont = fontManager.getFont(FontManager::FontType::Header);
    titleLabel->setFont(responsiveManager.scaled(titleFont));
    
    auto bodyFont = fontManager.getFont(FontManager::FontType::Body);
    infoLabel->setFont(responsiveManager.scaled(bodyFont));
    
    // Update component colors for platform
    titleLabel->setColour(juce::Label::textColourId, 
                         responsiveManager.adaptColorForPlatform(colorScheme.getTextColor()));
    
    // Touch-optimized controls get different styling
    if (touchOptimized)
    {
        // Larger touch targets, different visual feedback
        demoButton->setColour(juce::TextButton::buttonColourId,
                             responsiveManager.adaptColorForPlatform(colorScheme.getButtonColor()));
        
        demoSlider->setColour(juce::Slider::trackColourId,
                             responsiveManager.adaptColorForPlatform(colorScheme.getSliderTrackColor()));
        demoSlider->setColour(juce::Slider::thumbColourId,
                             responsiveManager.adaptColorForPlatform(colorScheme.getSliderThumbColor()));
    }
}

void ResponsiveIntegrationComponent::onResponsiveScaleChanged(
    const PlatformResponsiveManager::ResponsiveScale& newScale)
{
    // Respond to scale changes
    updateLayout();
    updatePlatformInfo();
    repaint();
}

void ResponsiveIntegrationComponent::onPlatformChanged(
    PlatformResponsiveManager::TargetPlatform newPlatform)
{
    // Respond to platform changes (e.g., when running in different environments)
    platformLookAndFeel->platformChanged();
    updatePlatformInfo();
    updateLayout();
    repaint();
}

void ResponsiveIntegrationComponent::updatePlatformInfo()
{
    if (!platformSelector || !infoLabel)
        return;
    
    auto& config = responsiveManager.getPlatformConfig();
    
    // Update platform selector
    platformSelector->clear();
    platformSelector->addItem("Platform Info", 1);
    
    juce::String platformName;
    switch (config.platform)
    {
        case PlatformResponsiveManager::TargetPlatform::macOS_Standalone:
            platformName = "macOS Standalone";
            break;
        case PlatformResponsiveManager::TargetPlatform::macOS_VST3:
            platformName = "macOS VST3";
            break;
        case PlatformResponsiveManager::TargetPlatform::macOS_AU:
            platformName = "macOS AU";
            break;
        case PlatformResponsiveManager::TargetPlatform::Windows_Standalone:
            platformName = "Windows Standalone";
            break;
        case PlatformResponsiveManager::TargetPlatform::iOS_Standalone:
            platformName = "iOS Standalone";
            break;
        case PlatformResponsiveManager::TargetPlatform::Android_Standalone:
            platformName = "Android Standalone";
            break;
        default:
            platformName = "Unknown Platform";
            break;
    }
    
    platformSelector->setText(platformName, juce::dontSendNotification);
    
    // Update info label with detailed responsive state
    juce::String infoText;
    infoText << "Platform: " << platformName << "\n";
    infoText << "Form Factor: ";
    
    switch (config.formFactor)
    {
        case PlatformResponsiveManager::DeviceFormFactor::Desktop:
            infoText << "Desktop";
            break;
        case PlatformResponsiveManager::DeviceFormFactor::Tablet:
            infoText << "Tablet";
            break;
        case PlatformResponsiveManager::DeviceFormFactor::Phone:
            infoText << "Phone";
            break;
        case PlatformResponsiveManager::DeviceFormFactor::PluginHost:
            infoText << "Plugin Host";
            break;
    }
    
    infoText << "\n";
    infoText << "Touch Input: " << (config.hasTouchInput ? "Yes" : "No") << "\n";
    infoText << "Hover Support: " << (config.hasHoverCapability ? "Yes" : "No") << "\n";
    infoText << "Compact Layout: " << (compactLayout ? "Yes" : "No") << "\n";
    
    auto currentScale = responsiveManager.calculateOptimalScaling(getWidth(), getHeight());
    infoText << "UI Scale: " << juce::String(currentScale.baseUIScale, 2) << "\n";
    infoText << "Text Scale: " << juce::String(currentScale.textScale, 2) << "\n";
    infoText << "Touch Scale: " << juce::String(currentScale.touchTargetScale, 2) << "\n";
    
    auto screenBounds = responsiveManager.getScreenBounds();
    infoText << "Screen: " << screenBounds.getWidth() << "x" << screenBounds.getHeight() << "\n";
    infoText << "DPI Scale: " << juce::String(responsiveManager.getSystemDPIScale(), 2);
    
    infoLabel->setText(infoText, juce::dontSendNotification);
}

void ResponsiveIntegrationComponent::lookAndFeelChanged()
{
    // Reapply styles when look and feel changes
    applyResponsiveStyles();
    repaint();
}

// Factory implementation
std::unique_ptr<ResponsiveIntegrationComponent> ResponsiveComponentFactory::createResponsiveDemo(
    PlatformResponsiveManager& responsiveManager,
    FontManager& fontManager,
    ColorScheme& colorScheme)
{
    // Create platform-aware look and feel
    auto lookAndFeel = PlatformLookAndFeelFactory::createForCurrentPlatform(
        fontManager, colorScheme, responsiveManager);
    
    return std::make_unique<ResponsiveIntegrationComponent>(
        responsiveManager, std::move(lookAndFeel), fontManager, colorScheme);
}