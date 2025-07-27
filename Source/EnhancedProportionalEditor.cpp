#include "EnhancedProportionalEditor.h"
#include "JUCE8Compatibility.h"

//==============================================================================
// Static members for global integration
//==============================================================================

ProportionalScalingManager* OTTOProportionalIntegration::globalScalingManager = nullptr;
bool OTTOProportionalIntegration::debugVisualizationEnabled = false;

//==============================================================================
// EnhancedProportionalEditor Implementation
//==============================================================================

EnhancedProportionalEditor::EnhancedProportionalEditor(juce::AudioProcessor& processor)
    : juce::AudioProcessorEditor(processor), audioProcessor(processor), tabbedDemo(juce::TabbedButtonBar::TabsAtTop)
{
    // Initialize managers first
    initializeManagers();
    initializeProportionalScaling();
    
    // Initialize UI components
    initializeDemoComponents();
    initializeControls();
    
    // Setup integration with existing systems
    setupIntegration();
    
    // Set initial size
    setSize(1000, 750);
    setResizable(true, true);
    
    // Start timer for smooth updates
    startTimer(16); // ~60fps
    
    isInitialized = true;
    
    // Trigger initial layout
    resized();
}

EnhancedProportionalEditor::~EnhancedProportionalEditor()
{
    stopTimer();
    
    // Clear callbacks to prevent issues during destruction
    if (proportionalManager) {
        proportionalManager->onProportionalScaleChanged = nullptr;
        proportionalManager->onDimensionContextChanged = nullptr;
        proportionalManager->onBreakpointChanged = nullptr;
    }
}

void EnhancedProportionalEditor::initializeManagers()
{
    // Initialize proportional scaling manager with OTTO-specific config
    ProportionalScalingManager::ReferenceConfig config;
    config.baseWidth = 1000;
    config.baseHeight = 750;
    config.aspectRatio = 1000.0f / 750.0f;
    config.minScaleFactor = 0.3f;
    config.maxScaleFactor = 3.0f;
    
    // Touch optimization for mobile platforms
#if JUCE_IOS || JUCE_ANDROID
    config.minTouchTargetPx = 44;
    config.minTouchTargetDp = 48;
#endif
    
    proportionalManager = std::make_unique<ProportionalScalingManager>(config);
    proportionalManager->enableDebugMode(true);
    
    // Initialize responsive layout manager
    ResponsiveLayoutManager::LayoutConfig responsiveConfig;
    responsiveConfig.baseWidth = 1000;
    responsiveConfig.baseHeight = 750;
    responsiveConfig.aspectRatio = 1000.0f / 750.0f;
    responsiveManager = std::make_unique<ResponsiveLayoutManager>(responsiveConfig);
    
    // Initialize platform manager
    platformManager = std::make_unique<PlatformResponsiveManager>();
    platformManager->autoDetectPlatform();
    
    // Integrate managers
    proportionalManager->integrateWithResponsiveManager(responsiveManager.get());
    proportionalManager->integrateWithPlatformManager(platformManager.get());
}

void EnhancedProportionalEditor::initializeProportionalScaling()
{
    if (!proportionalManager) return;
    
    // Set up callbacks for real-time updates
    proportionalManager->onProportionalScaleChanged = [this](const ProportionalScalingManager::ProportionalScale& scale) {
        onProportionalScaleChanged(scale);
    };
    
    proportionalManager->onDimensionContextChanged = [this](const ProportionalScalingManager::DimensionContext& context) {
        onDimensionContextChanged(context);
    };
    
    proportionalManager->onBreakpointChanged = [this](int width, int height) {
        onBreakpointChanged(width, height);
    };
    
    // Detect and adapt to plugin format
    detectPluginFormat();
}

void EnhancedProportionalEditor::initializeDemoComponents()
{
    // Main scaling demo
    scalingDemo = std::make_unique<ProportionalScalingDemo>();
    tabbedDemo.addTab("Scaling Demo", juce::Colours::darkblue, scalingDemo.get(), false);
    
    // Touch controls demo
    touchControl1 = std::make_unique<ProportionalTouchControl>("Volume", proportionalManager.get());
    touchControl2 = std::make_unique<ProportionalTouchControl>("Filter", proportionalManager.get());
    
    auto touchPanel = std::make_unique<juce::Component>();
    touchPanel->addAndMakeVisible(touchControl1.get());
    touchPanel->addAndMakeVisible(touchControl2.get());
    tabbedDemo.addTab("Touch Controls", juce::Colours::darkgreen, touchPanel.release(), true);
    
    // Grid layout demo
    gridLayout = std::make_unique<ProportionalGridLayout>(proportionalManager.get());
    
    // Add some demo components to the grid
    for (int i = 0; i < 6; ++i) {
        auto button = std::make_unique<juce::TextButton>("Button " + juce::String(i + 1));
        button->setColour(juce::TextButton::buttonColourId, 
                         juce::Colour::fromHSV(i * 0.15f, 0.7f, 0.8f, 1.0f));
        gridLayout->addGridItem(std::move(button));
    }
    
    tabbedDemo.addTab("Grid Layout", juce::Colours::darkorange, gridLayout.get(), false);
    
    // Text demo
    textDemo = std::make_unique<ProportionalTextDemo>(proportionalManager.get());
    tabbedDemo.addTab("Text Scaling", juce::Colours::darkmagenta, textDemo.get(), false);
    
    // Plugin demo
    pluginDemo = std::make_unique<ProportionalPluginDemo>(proportionalManager.get());
    tabbedDemo.addTab("Plugin Integration", juce::Colours::darkcyan, pluginDemo.get(), false);
    
    // Platform demo
    platformDemo = std::make_unique<ProportionalPlatformDemo>(proportionalManager.get());
    tabbedDemo.addTab("Platform Adaptation", juce::Colours::darkred, platformDemo.get(), false);
    
    addAndMakeVisible(tabbedDemo);
}

void EnhancedProportionalEditor::initializeControls()
{
    // Control panel
    controlPanel.setText("Proportional Scaling Controls");
    controlPanel.setColour(juce::GroupComponent::outlineColourId, juce::Colours::white);
    controlPanel.setColour(juce::GroupComponent::textColourId, juce::Colours::white);
    addAndMakeVisible(controlPanel);
    
    // Scale control
    scaleLabel.setText("Manual Scale:", juce::dontSendNotification);
    scaleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    scaleLabel.attachToComponent(&scaleSlider, true);
    addAndMakeVisible(scaleLabel);
    
    scaleSlider.setRange(0.3, 3.0, 0.01);
    scaleSlider.setValue(1.0);
    scaleSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    scaleSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    scaleSlider.onValueChange = [this] { scaleSliderChanged(); };
    addAndMakeVisible(scaleSlider);
    
    // Width control
    widthLabel.setText("Width:", juce::dontSendNotification);
    widthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    widthLabel.attachToComponent(&widthSlider, true);
    addAndMakeVisible(widthLabel);
    
    widthSlider.setRange(300, 2000, 1);
    widthSlider.setValue(1000);
    widthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    widthSlider.onValueChange = [this] { dimensionSliderChanged(); };
    addAndMakeVisible(widthSlider);
    
    // Height control
    heightLabel.setText("Height:", juce::dontSendNotification);
    heightLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    heightLabel.attachToComponent(&heightSlider, true);
    addAndMakeVisible(heightLabel);
    
    heightSlider.setRange(200, 1500, 1);
    heightSlider.setValue(750);
    heightSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    heightSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    heightSlider.onValueChange = [this] { dimensionSliderChanged(); };
    addAndMakeVisible(heightSlider);
    
    // Touch optimization toggle
    touchOptimizationToggle.setButtonText("Touch Optimization");
    touchOptimizationToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    touchOptimizationToggle.onClick = [this] { touchToggleChanged(); };
    addAndMakeVisible(touchOptimizationToggle);
    
    // Auto scaling toggle
    autoScalingToggle.setButtonText("Auto Scaling");
    autoScalingToggle.setToggleState(true, juce::dontSendNotification);
    autoScalingToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    autoScalingToggle.onClick = [this] { autoScalingToggleChanged(); };
    addAndMakeVisible(autoScalingToggle);
    
    // Platform selector
    platformLabel.setText("Platform:", juce::dontSendNotification);
    platformLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    platformLabel.attachToComponent(&platformCombo, true);
    addAndMakeVisible(platformLabel);
    
    platformCombo.addItem("Auto Detect", 1);
    platformCombo.addItem("macOS Desktop", 2);
    platformCombo.addItem("Windows Desktop", 3);
    platformCombo.addItem("Linux Desktop", 4);
    platformCombo.addItem("iOS Mobile", 5);
    platformCombo.addItem("Android Mobile", 6);
    platformCombo.addItem("VST3 Plugin", 7);
    platformCombo.addItem("AU Plugin", 8);
    
    platformCombo.setSelectedId(1);
    platformCombo.onChange = [this] { platformComboChanged(); };
    addAndMakeVisible(platformCombo);
    
    // Info display
    infoLabel.setText("Proportional Scaling Status", juce::dontSendNotification);
    infoLabel.setFont(JUCE8_CREATE_FONT(14.0f, juce::Font::bold));
    infoLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
    infoLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(infoLabel);
    
    // Status display
    statusDisplay.setMultiLine(true);
    statusDisplay.setReadOnly(true);
    statusDisplay.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black.withAlpha(0.8f));
    statusDisplay.setColour(juce::TextEditor::textColourId, juce::Colours::lightgreen);
    statusDisplay.setFont(JUCE8_CREATE_NAMED_FONT(juce::Font::getDefaultMonospacedFontName(), 10.0f, juce::Font::plain));
    addAndMakeVisible(statusDisplay);
}

void EnhancedProportionalEditor::setupIntegration()
{
    // Set up global integration for OTTO (pass reference instead of copying)
    OTTOProportionalIntegration::setGlobalScalingManager(proportionalManager.get());
    
    // Enable debug visualization in development builds
#if JUCE_DEBUG
    OTTOProportionalIntegration::enableDebugVisualization(true);
#endif
    
    // Apply plugin format specific adaptations
    adaptForPlugin();
}

void EnhancedProportionalEditor::paint(juce::Graphics& g)
{
    // Gradient background
    auto bounds = getLocalBounds();
    juce::ColourGradient gradient(juce::Colours::darkblue.darker(), bounds.getTopLeft().toFloat(),
                                 juce::Colours::black, bounds.getBottomRight().toFloat(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Debug visualization if enabled
    if (OTTOProportionalIntegration::isDebugVisualizationEnabled() && proportionalManager) {
        g.setColour(juce::Colours::yellow.withAlpha(0.2f));
        
        // Draw proportional grid
        int gridSize = proportionalManager->scaledSpacing(50);
        for (int x = 0; x < bounds.getWidth(); x += gridSize) {
            g.drawVerticalLine(x, 0.0f, static_cast<float>(bounds.getHeight()));
        }
        for (int y = 0; y < bounds.getHeight(); y += gridSize) {
            g.drawHorizontalLine(y, 0.0f, static_cast<float>(bounds.getWidth()));
        }
        
        // Draw scaling info
        g.setColour(juce::Colours::yellow);
        g.setFont(JUCE8_CREATE_FONT(10.0f, juce::Font::plain));
        auto context = proportionalManager->getCurrentContext();
        g.drawText(juce::String::formatted("Scale: %.2f", context.proportionalScale),
                  bounds.getX() + 5, bounds.getY() + 5, 150, 15, juce::Justification::topLeft);
    }
    
    // Border
    g.setColour(juce::Colours::lightgrey.withAlpha(0.5f));
    g.drawRect(bounds, 1);
}

void EnhancedProportionalEditor::resized()
{
    if (!isInitialized || !proportionalManager) return;
    
    // Update proportional scaling with new dimensions
    proportionalManager->updateDimensions(getWidth(), getHeight());
    
    // Update layout
    updateProportionalLayout();
    
    // Update info displays
    updateInfoDisplay();
    updateStatusDisplay();
}

void EnhancedProportionalEditor::updateProportionalLayout()
{
    if (!proportionalManager) return;
    
    auto bounds = getLocalBounds();
    int margin = proportionalManager->scaledSpacing(10);
    int controlHeight = proportionalManager->scaledElement(25);
    int infoHeight = proportionalManager->scaledText(20);
    
    bounds.reduce(margin, margin);
    
    // Info label at top
    infoLabel.setBounds(bounds.removeFromTop(infoHeight));
    infoLabel.setFont(proportionalManager->scaledProportional(
        JUCE8_CREATE_FONT(14.0f, juce::Font::bold)));
    bounds.removeFromTop(margin / 2);
    
    // Control panel on the left
    int controlPanelWidth = proportionalManager->scaledElement(250);
    auto controlArea = bounds.removeFromLeft(controlPanelWidth);
    controlPanel.setBounds(controlArea);
    
    // Layout controls within panel
    controlArea.reduce(margin, margin * 2); // Account for group box
    
    // Scale control
    auto scaleArea = controlArea.removeFromTop(controlHeight);
    scaleArea.removeFromLeft(60); // Space for label
    scaleSlider.setBounds(scaleArea);
    controlArea.removeFromTop(margin / 2);
    
    // Dimension controls
    auto widthArea = controlArea.removeFromTop(controlHeight);
    widthArea.removeFromLeft(60);
    widthSlider.setBounds(widthArea);
    controlArea.removeFromTop(margin / 2);
    
    auto heightArea = controlArea.removeFromTop(controlHeight);
    heightArea.removeFromLeft(60);
    heightSlider.setBounds(heightArea);
    controlArea.removeFromTop(margin);
    
    // Toggles
    touchOptimizationToggle.setBounds(controlArea.removeFromTop(controlHeight));
    controlArea.removeFromTop(margin / 2);
    autoScalingToggle.setBounds(controlArea.removeFromTop(controlHeight));
    controlArea.removeFromTop(margin);
    
    // Platform selector
    auto platformArea = controlArea.removeFromTop(controlHeight);
    platformArea.removeFromLeft(60);
    platformCombo.setBounds(platformArea);
    controlArea.removeFromTop(margin);
    
    // Status display at bottom of control panel
    if (controlArea.getHeight() > 50) {
        statusDisplay.setBounds(controlArea.removeFromTop(100));
        statusDisplay.setFont(proportionalManager->scaledProportional(
            JUCE8_CREATE_NAMED_FONT(juce::Font::getDefaultMonospacedFontName(), 9.0f, juce::Font::plain)));
    }
    
    bounds.removeFromLeft(margin);
    
    // Main demo area
    tabbedDemo.setBounds(bounds);
    
    // Set font for tab bar
    auto& tabBar = tabbedDemo.getTabbedButtonBar();
    // Note: TabBarButton doesn't have setFont method in JUCE 8
    // Tab font is controlled by the TabbedComponent's LookAndFeel
}

void EnhancedProportionalEditor::updateInfoDisplay()
{
    if (!proportionalManager) return;
    
    auto context = proportionalManager->getCurrentContext();
    juce::String info = "Proportional Scaling Active - ";
    info << "Scale: " << juce::String(context.proportionalScale, 2) << " - ";
    info << "Size: " << context.currentWidth << "x" << context.currentHeight;
    
    if (context.isExtraSmall) info << " (Extra Small)";
    else if (context.isSmall) info << " (Small)";
    else if (context.isMedium) info << " (Medium)";
    else if (context.isLarge) info << " (Large)";
    else if (context.isExtraLarge) info << " (Extra Large)";
    
    if (context.needsTouchOptimization) {
        info << " - Touch Optimized";
    }
    
    infoLabel.setText(info, juce::dontSendNotification);
}

void EnhancedProportionalEditor::updateStatusDisplay()
{
    if (!proportionalManager) return;
    
    juce::String status;
    status << "=== OTTO Proportional Scaling Status ===\n";
    status << "Platform: " << getCurrentPlatformName() << "\n";
    status << "Plugin Format: " << getCurrentPluginFormatName() << "\n";
    
    auto context = proportionalManager->getCurrentContext();
    auto scale = proportionalManager->getCurrentScale();
    
    status << "Dimensions: " << context.currentWidth << "x" << context.currentHeight << "\n";
    status << "Aspect Ratio: " << juce::String(context.aspectRatio, 2) << "\n";
    status << "Base Scale: " << juce::String(scale.baseScale, 3) << "\n";
    status << "Text Scale: " << juce::String(scale.textScale, 3) << "\n";
    status << "Touch Scale: " << juce::String(scale.touchTargetScale, 3) << "\n";
    status << "DPI Multiplier: " << juce::String(scale.dpiMultiplier, 3) << "\n";
    
    if (context.needsTouchOptimization) {
        status << "Min Touch Target: " << proportionalManager->minTouchTarget() << "px\n";
    }
    
    status << "Breakpoint: ";
    if (context.isExtraSmall) status << "Extra Small";
    else if (context.isSmall) status << "Small";
    else if (context.isMedium) status << "Medium";
    else if (context.isLarge) status << "Large";
    else if (context.isExtraLarge) status << "Extra Large";
    status << "\n";
    
    status << "Compact Layout: " << (proportionalManager->shouldUseCompactLayout() ? "Yes" : "No") << "\n";
    status << "Auto Scaling: " << (autoScalingToggle.getToggleState() ? "Enabled" : "Disabled") << "\n";
    
    if (proportionalManager->hasError()) {
        status << "ERROR: " << proportionalManager->getLastError() << "\n";
    }
    
    statusDisplay.setText(status);
}

void EnhancedProportionalEditor::timerCallback()
{
    // Update at 60fps for smooth scaling
    juce::int64 currentTime = juce::Time::getMillisecondCounter();
    if (currentTime - lastUpdateTime > 16) { // ~60fps
        updateInfoDisplay();
        lastUpdateTime = currentTime;
    }
}

void EnhancedProportionalEditor::onProportionalScaleChanged(const ProportionalScalingManager::ProportionalScale& scale)
{
    // Update slider to reflect current scale if auto-scaling is enabled
    if (autoScalingToggle.getToggleState()) {
        scaleSlider.setValue(scale.baseScale, juce::dontSendNotification);
    }
    
    // Trigger layout update
    updateProportionalLayout();
    updateStatusDisplay();
    repaint();
}

void EnhancedProportionalEditor::onDimensionContextChanged(const ProportionalScalingManager::DimensionContext& context)
{
    // Update dimension sliders
    widthSlider.setValue(context.currentWidth, juce::dontSendNotification);
    heightSlider.setValue(context.currentHeight, juce::dontSendNotification);
    
    updateInfoDisplay();
    updateStatusDisplay();
}

void EnhancedProportionalEditor::onBreakpointChanged(int width, int height)
{
    // Handle breakpoint changes - might need to reconfigure layout
    updateProportionalLayout();
    repaint();
    
    DBG("Breakpoint changed: " << width << "x" << height);
}

void EnhancedProportionalEditor::scaleSliderChanged()
{
    if (proportionalManager && !autoScalingToggle.getToggleState()) {
        // Manual scale override
        auto scale = proportionalManager->getCurrentScale();
        scale.baseScale = static_cast<float>(scaleSlider.getValue());
        proportionalManager->applyProportionalScale(scale);
    }
}

void EnhancedProportionalEditor::dimensionSliderChanged()
{
    if (proportionalManager) {
        int newWidth = static_cast<int>(widthSlider.getValue());
        int newHeight = static_cast<int>(heightSlider.getValue());
        
        // Update component size
        setSize(newWidth, newHeight);
    }
}

void EnhancedProportionalEditor::touchToggleChanged()
{
    if (proportionalManager) {
        auto config = proportionalManager->getReferenceConfig();
        if (touchOptimizationToggle.getToggleState()) {
            config.minTouchTargetPx = 44;
            config.minTouchTargetDp = 48;
        } else {
            config.minTouchTargetPx = 0;
            config.minTouchTargetDp = 0;
        }
        proportionalManager->setReferenceConfig(config);
    }
}

void EnhancedProportionalEditor::autoScalingToggleChanged()
{
    if (proportionalManager) {
        proportionalManager->enableAutoRecalculation(autoScalingToggle.getToggleState());
    }
}

void EnhancedProportionalEditor::platformComboChanged()
{
    auto selectedText = platformCombo.getText();
    
    if (selectedText == "Auto Detect") {
        if (platformManager) {
            platformManager->autoDetectPlatform();
        }
    } else if (selectedText.contains("macOS")) {
        adaptForMacOS();
    } else if (selectedText.contains("Windows")) {
        adaptForWindows();
    } else if (selectedText.contains("Linux")) {
        adaptForLinux();
    } else if (selectedText.contains("iOS")) {
        adaptForIOS();
    } else if (selectedText.contains("Android")) {
        adaptForAndroid();
    }
    
    updateStatusDisplay();
}

void EnhancedProportionalEditor::detectPluginFormat()
{
    // This would be implemented based on your processor's format detection
    adaptForStandalone(); // Default to standalone for demo
}

void EnhancedProportionalEditor::adaptForMacOS()
{
    // macOS-specific adaptations
    if (proportionalManager) {
        proportionalManager->updateDPIScale();
    }
}

void EnhancedProportionalEditor::adaptForWindows()
{
    // Windows-specific adaptations
    if (proportionalManager) {
        proportionalManager->updateDPIScale();
    }
}

void EnhancedProportionalEditor::adaptForLinux()
{
    // Linux-specific adaptations
}

void EnhancedProportionalEditor::adaptForIOS()
{
    // iOS-specific adaptations
    if (proportionalManager) {
        auto config = proportionalManager->getReferenceConfig();
        config.minTouchTargetPx = 44;
        config.minTouchTargetDp = 44;
        proportionalManager->setReferenceConfig(config);
        
        touchOptimizationToggle.setToggleState(true, juce::dontSendNotification);
    }
}

void EnhancedProportionalEditor::adaptForAndroid()
{
    // Android-specific adaptations
    if (proportionalManager) {
        auto config = proportionalManager->getReferenceConfig();
        config.minTouchTargetPx = 48;
        config.minTouchTargetDp = 48;
        proportionalManager->setReferenceConfig(config);
        
        touchOptimizationToggle.setToggleState(true, juce::dontSendNotification);
    }
}

void EnhancedProportionalEditor::adaptForPlugin()
{
    // General plugin adaptations
    if (proportionalManager) {
        // Plugins typically have more constrained sizing
        auto config = proportionalManager->getReferenceConfig();
        config.minScaleFactor = 0.5f;
        config.maxScaleFactor = 2.0f;
        proportionalManager->setReferenceConfig(config);
    }
}

void EnhancedProportionalEditor::adaptForVST3()
{
    adaptForPlugin();
    
    // VST3-specific constraints might be applied by the host
    if (proportionalManager) {
        proportionalManager->setPluginHostConstraints(800, 600, true);
    }
}

void EnhancedProportionalEditor::adaptForAU()
{
    adaptForPlugin();
    
    // AU-specific adaptations
    if (proportionalManager) {
        proportionalManager->setPluginHostConstraints(1024, 768, true);
    }
}

void EnhancedProportionalEditor::adaptForCLAP()
{
    adaptForPlugin();
    
    // CLAP-specific adaptations
}

void EnhancedProportionalEditor::adaptForStandalone()
{
    // Standalone applications have more freedom
    if (proportionalManager) {
        auto config = proportionalManager->getReferenceConfig();
        config.minScaleFactor = 0.3f;
        config.maxScaleFactor = 3.0f;
        proportionalManager->setReferenceConfig(config);
    }
}

juce::String EnhancedProportionalEditor::getCurrentPlatformName() const
{
#if JUCE_MAC
    return "macOS";
#elif JUCE_WINDOWS
    return "Windows";
#elif JUCE_LINUX
    return "Linux";
#elif JUCE_IOS
    return "iOS";
#elif JUCE_ANDROID
    return "Android";
#else
    return "Unknown";
#endif
}

juce::String EnhancedProportionalEditor::getCurrentPluginFormatName() const
{
    // This would typically be determined from the processor
    return "Standalone"; // Default for demo
}

juce::Rectangle<int> EnhancedProportionalEditor::getOptimalBounds() const
{
    if (proportionalManager) {
        return proportionalManager->getProportionalBounds(getWidth(), getHeight());
    }
    return getBounds();
}

//==============================================================================
// ProportionalComponentWrapper Implementation
//==============================================================================

ProportionalComponentWrapper::ProportionalComponentWrapper(std::unique_ptr<juce::Component> wrapped,
                                                          ProportionalScalingManager* manager)
    : wrappedComponent(std::move(wrapped)), scalingManager(manager)
{
    if (wrappedComponent) {
        addAndMakeVisible(wrappedComponent.get());
        originalBounds = wrappedComponent->getBounds();
    }
}

ProportionalComponentWrapper::~ProportionalComponentWrapper() = default;

void ProportionalComponentWrapper::paint(juce::Graphics& g)
{
    // Optional: draw wrapper-specific elements
    if (proportionalScalingEnabled && scalingManager) {
        // Debug visualization
        g.setColour(juce::Colours::yellow.withAlpha(0.1f));
        g.fillAll();
        g.setColour(juce::Colours::yellow.withAlpha(0.3f));
        g.drawRect(getLocalBounds(), 1);
    }
}

void ProportionalComponentWrapper::resized()
{
    if (wrappedComponent) {
        if (proportionalScalingEnabled && scalingManager) {
            updateProportionalBounds();
            wrappedComponent->setBounds(proportionalBounds);
        } else {
            wrappedComponent->setBounds(getLocalBounds());
        }
    }
}

void ProportionalComponentWrapper::setProportionalBounds(const juce::Rectangle<int>& bounds)
{
    originalBounds = bounds;
    updateProportionalBounds();
    setBounds(proportionalBounds);
}

juce::Rectangle<int> ProportionalComponentWrapper::getProportionalBounds() const
{
    return proportionalBounds;
}

void ProportionalComponentWrapper::updateProportionalBounds()
{
    if (scalingManager && proportionalScalingEnabled) {
        proportionalBounds = scalingManager->scaledProportional(originalBounds);
    } else {
        proportionalBounds = originalBounds;
    }
}

//==============================================================================
// ProportionalLookAndFeel Implementation
//==============================================================================

ProportionalLookAndFeel::ProportionalLookAndFeel(ProportionalScalingManager* manager)
    : scalingManager(manager), currentPlatform(PlatformResponsiveManager::TargetPlatform::macOS_Standalone)
{
}

void ProportionalLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                  const juce::Colour& backgroundColour,
                                                  bool shouldDrawButtonAsHighlighted,
                                                  bool shouldDrawButtonAsDown)
{
    updateCache();
    
    auto bounds = button.getLocalBounds().toFloat();
    auto cornerRadius = getProportionalScale() * 4.0f;
    
    juce::Colour baseColour = backgroundColour;
    if (shouldDrawButtonAsDown) {
        baseColour = baseColour.darker(0.3f);
    } else if (shouldDrawButtonAsHighlighted) {
        baseColour = baseColour.brighter(0.2f);
    }
    
    // Platform-specific drawing
    switch (currentPlatform) {
        case PlatformResponsiveManager::TargetPlatform::macOS_Standalone:
        case PlatformResponsiveManager::TargetPlatform::macOS_VST3:
        case PlatformResponsiveManager::TargetPlatform::macOS_AU:
            drawMacOSButton(g, button, baseColour);
            break;
        case PlatformResponsiveManager::TargetPlatform::Windows_Standalone:
        case PlatformResponsiveManager::TargetPlatform::Windows_VST3:
            drawWindowsButton(g, button, baseColour);
            break;
        case PlatformResponsiveManager::TargetPlatform::Linux_Standalone:
        case PlatformResponsiveManager::TargetPlatform::Linux_VST3:
            drawLinuxButton(g, button, baseColour);
            break;
        case PlatformResponsiveManager::TargetPlatform::iOS_Standalone:
        case PlatformResponsiveManager::TargetPlatform::iOS_AUv3:
        case PlatformResponsiveManager::TargetPlatform::Android_Standalone:
            drawMobileButton(g, button, baseColour);
            break;
        default:
            // Fallback to default JUCE drawing
            LookAndFeel_V4::drawButtonBackground(g, button, backgroundColour,
                                               shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
            break;
    }
}

void ProportionalLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto font = getTextButtonFont(button, button.getHeight());
    g.setFont(font);
    g.setColour(button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                                                          : juce::TextButton::textColourOffId)
                    .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));
    
    const int yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
    const int cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;
    
    const int fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
    const int leftIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    const int rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
    const int textWidth = button.getWidth() - leftIndent - rightIndent;
    
    if (textWidth > 0) {
        g.drawFittedText(button.getButtonText(),
                        leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                        juce::Justification::centred, 2);
    }
}

juce::Font ProportionalLookAndFeel::getTextButtonFont(juce::TextButton& button, int buttonHeight)
{
    updateCache();
    auto baseFont = LookAndFeel_V4::getTextButtonFont(button, buttonHeight);
    
    if (scalingManager) {
        return scalingManager->scaledProportional(baseFont);
    }
    
    return baseFont.withHeight(baseFont.getHeight() * getProportionalScale());
}

void ProportionalLookAndFeel::setPlatformSpecificStyling(PlatformResponsiveManager::TargetPlatform platform)
{
    currentPlatform = platform;
}

float ProportionalLookAndFeel::getProportionalScale() const
{
    updateCache();
    return cachedScale;
}

void ProportionalLookAndFeel::updateCache() const
{
    juce::int64 currentTime = juce::Time::getMillisecondCounter();
    if (currentTime - cacheTime > 16) { // Update cache every ~16ms
        cacheTime = currentTime;
        
        if (scalingManager) {
            cachedScale = scalingManager->getCurrentScale().baseScale;
        } else {
            cachedScale = 1.0f;
        }
    }
}

void ProportionalLookAndFeel::drawMacOSButton(juce::Graphics& g, juce::Button& button, const juce::Colour& color)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto cornerRadius = getProportionalScale() * 6.0f;
    
    // macOS style with subtle gradient
    juce::ColourGradient gradient(color.brighter(0.1f), bounds.getTopLeft(),
                                 color.darker(0.1f), bounds.getBottomLeft(), false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    g.setColour(color.darker(0.3f));
    g.drawRoundedRectangle(bounds, cornerRadius, getProportionalScale());
}

void ProportionalLookAndFeel::drawWindowsButton(juce::Graphics& g, juce::Button& button, const juce::Colour& color)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto cornerRadius = getProportionalScale() * 2.0f;
    
    // Windows style - more angular
    g.setColour(color);
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    g.setColour(color.contrasting(0.3f));
    g.drawRoundedRectangle(bounds, cornerRadius, getProportionalScale());
}

void ProportionalLookAndFeel::drawLinuxButton(juce::Graphics& g, juce::Button& button, const juce::Colour& color)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto cornerRadius = getProportionalScale() * 3.0f;
    
    // Linux/GTK style
    g.setColour(color);
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    g.setColour(color.contrasting(0.4f));
    g.drawRoundedRectangle(bounds, cornerRadius, getProportionalScale() * 1.5f);
}

void ProportionalLookAndFeel::drawMobileButton(juce::Graphics& g, juce::Button& button, const juce::Colour& color)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto cornerRadius = getProportionalScale() * 8.0f; // More rounded for touch
    
    // Mobile style - larger touch targets, more rounded
    g.setColour(color);
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    // Softer outline
    g.setColour(color.contrasting(0.2f));
    g.drawRoundedRectangle(bounds, cornerRadius, getProportionalScale() * 0.5f);
}

// Additional LookAndFeel methods for JUCE 8 compatibility

void ProportionalLookAndFeel::drawTextEditor(juce::Graphics& g, int width, int height,
                                            juce::TextEditor& textEditor)
{
    // Draw TextEditor background with proportional scaling
    auto cornerRadius = getProportionalScale() * 3.0f;
    
    g.setColour(textEditor.findColour(juce::TextEditor::backgroundColourId));
    g.fillRoundedRectangle(0, 0, (float)width, (float)height, cornerRadius);
    
    g.setColour(textEditor.findColour(juce::TextEditor::outlineColourId));
    g.drawRoundedRectangle(1, 1, width - 2, height - 2, cornerRadius, getProportionalScale());
}

juce::Font ProportionalLookAndFeel::getLabelFont(juce::Label& label)
{
    if (scalingManager) {
        return scalingManager->scaledProportional(juce::LookAndFeel_V4::getLabelFont(label));
    }
    return juce::LookAndFeel_V4::getLabelFont(label);
}

juce::Font ProportionalLookAndFeel::getComboBoxFont(juce::ComboBox& box)
{
    if (scalingManager) {
        return scalingManager->scaledProportional(juce::LookAndFeel_V4::getComboBoxFont(box));
    }
    return juce::LookAndFeel_V4::getComboBoxFont(box);
}

int ProportionalLookAndFeel::getSliderThumbRadius(juce::Slider& slider)
{
    int baseRadius = juce::LookAndFeel_V4::getSliderThumbRadius(slider);
    if (scalingManager) {
        return scalingManager->scaledElement(baseRadius);
    }
    return baseRadius;
}

int ProportionalLookAndFeel::getDefaultScrollbarWidth()
{
    int baseWidth = juce::LookAndFeel_V4::getDefaultScrollbarWidth();
    if (scalingManager) {
        return scalingManager->scaledElement(baseWidth);
    }
    return baseWidth;
}

//==============================================================================
// OTTOProportionalIntegration Implementation
//==============================================================================

void OTTOProportionalIntegration::initializeForOTTO(juce::AudioProcessorEditor* editor)
{
    if (!editor) return;
    
    // Create global scaling manager if not exists
    if (!globalScalingManager) {
        ProportionalScalingManager::ReferenceConfig config;
        config.baseWidth = 1000;
        config.baseHeight = 750;
        globalScalingManager = new ProportionalScalingManager(config);
    }
    
    // Update with editor dimensions
    globalScalingManager->updateFromComponent(editor);
    
    // Enable auto-recalculation
    globalScalingManager->enableAutoRecalculation(true);
    
    DBG("OTTO Proportional Scaling initialized for editor: " << editor->getName());
}

void OTTOProportionalIntegration::enhanceOTTOComponent(juce::Component* component, const juce::String& componentType)
{
    if (!component || !globalScalingManager) return;
    
    if (componentType == "Slider") {
        enhanceSlider(dynamic_cast<juce::Slider*>(component));
    } else if (componentType == "Button") {
        enhanceButton(dynamic_cast<juce::Button*>(component));
    } else if (componentType == "Label") {
        enhanceLabel(dynamic_cast<juce::Label*>(component));
    }
    // Add more component types as needed
}

void OTTOProportionalIntegration::convertOTTOLayoutToProportional(juce::Component* parent)
{
    if (!parent || !globalScalingManager) return;
    
    analyzeExistingLayout(parent);
    
    // Convert child components
    for (int i = 0; i < parent->getNumChildComponents(); ++i) {
        auto child = parent->getChildComponent(i);
        if (child) {
            auto componentType = detectComponentType(child);
            enhanceOTTOComponent(child, componentType);
        }
    }
}

ProportionalScalingManager* OTTOProportionalIntegration::getGlobalScalingManager()
{
    return globalScalingManager;
}

void OTTOProportionalIntegration::setGlobalScalingManager(ProportionalScalingManager* manager)
{
    // Note: We don't take ownership, just hold a reference
    // The caller is responsible for lifetime management
    globalScalingManager = manager;
}

void OTTOProportionalIntegration::cleanup()
{
    delete globalScalingManager;
    globalScalingManager = nullptr;
}

juce::Rectangle<int> OTTOProportionalIntegration::convertFixedBoundsToProportional(const juce::Rectangle<int>& bounds)
{
    if (globalScalingManager) {
        return globalScalingManager->scaledProportional(bounds);
    }
    return bounds;
}

juce::Font OTTOProportionalIntegration::convertFixedFontToProportional(const juce::Font& font)
{
    if (globalScalingManager) {
        return globalScalingManager->scaledProportional(font);
    }
    return font;
}

int OTTOProportionalIntegration::convertFixedDimensionToProportional(int dimension)
{
    if (globalScalingManager) {
        return globalScalingManager->scaledProportional(dimension);
    }
    return dimension;
}

void OTTOProportionalIntegration::enableDebugVisualization(bool enabled)
{
    debugVisualizationEnabled = enabled;
}

void OTTOProportionalIntegration::printScalingReport()
{
    if (globalScalingManager) {
        globalScalingManager->debugPrintProportionalState();
    }
}

juce::String OTTOProportionalIntegration::getIntegrationStatus()
{
    if (globalScalingManager) {
        return globalScalingManager->getProportionalStateString();
    }
    return "Proportional scaling not initialized";
}

void OTTOProportionalIntegration::enhanceSlider(juce::Slider* slider)
{
    if (!slider || !globalScalingManager) return;
    
    // Apply proportional scaling to slider bounds
    auto bounds = slider->getBounds();
    auto scaledBounds = globalScalingManager->scaledProportional(bounds);
    slider->setBounds(scaledBounds);
    
    // Enhance for touch if needed
    auto context = globalScalingManager->getCurrentContext();
    if (context.needsTouchOptimization) {
        auto touchBounds = globalScalingManager->ensureTouchTarget(scaledBounds);
        slider->setBounds(touchBounds);
    }
}

void OTTOProportionalIntegration::enhanceButton(juce::Button* button)
{
    if (!button || !globalScalingManager) return;
    
    // Apply proportional scaling to button bounds
    auto bounds = button->getBounds();
    auto scaledBounds = globalScalingManager->scaledProportional(bounds);
    button->setBounds(scaledBounds);
    
    // Enhance for touch if needed
    auto context = globalScalingManager->getCurrentContext();
    if (context.needsTouchOptimization) {
        auto touchBounds = globalScalingManager->ensureTouchTarget(scaledBounds);
        button->setBounds(touchBounds);
    }
}

void OTTOProportionalIntegration::enhanceLabel(juce::Label* label)
{
    if (!label || !globalScalingManager) return;
    
    // Apply proportional scaling to label bounds and font
    auto bounds = label->getBounds();
    auto scaledBounds = globalScalingManager->scaledProportional(bounds);
    label->setBounds(scaledBounds);
    
    auto font = label->getFont();
    auto scaledFont = globalScalingManager->scaledProportional(font);
    label->setFont(scaledFont);
}

juce::String OTTOProportionalIntegration::detectComponentType(juce::Component* component)
{
    if (dynamic_cast<juce::Slider*>(component)) return "Slider";
    if (dynamic_cast<juce::Button*>(component)) return "Button";
    if (dynamic_cast<juce::Label*>(component)) return "Label";
    if (dynamic_cast<juce::TextEditor*>(component)) return "TextEditor";
    if (dynamic_cast<juce::ComboBox*>(component)) return "ComboBox";
    return "Unknown";
}

void OTTOProportionalIntegration::analyzeExistingLayout(juce::Component* parent, int depth)
{
    if (!parent || depth > 10) return; // Prevent infinite recursion
    
    juce::String indent = juce::String::repeatedString("  ", depth);
    DBG(indent << "Analyzing component: " << parent->getName() << 
        " (" << detectComponentType(parent) << ") - " << 
        parent->getBounds().toString());
    
    for (int i = 0; i < parent->getNumChildComponents(); ++i) {
        analyzeExistingLayout(parent->getChildComponent(i), depth + 1);
    }
}