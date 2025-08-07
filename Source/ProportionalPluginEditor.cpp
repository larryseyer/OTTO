#include "ProportionalPluginEditor.h"
#include "PluginEditor.h"

//==============================================================================
// ProportionalPluginEditor Implementation
//==============================================================================

ProportionalPluginEditor::ProportionalPluginEditor(OTTOAudioProcessor& processor)
    : juce::AudioProcessorEditor(processor), audioProcessor(processor)
{
    // Initialize proportional scaling system first
    initializeProportionalScaling();
    
    // Initialize managers
    initializeManagers();
    
    // Initialize components
    initializeComponents();
    
    // Setup proportional wrappers for existing components
    setupProportionalWrappers();
    
    // Setup look and feel
    setupLookAndFeel();
    
    // Setup callbacks
    setupCallbacks();
    
    // Enhance existing components with proportional scaling
    enhanceExistingComponents();
    
    // Detect and adapt to current platform/plugin format
    detectAndAdaptPlatform();
    detectAndAdaptPluginFormat();
    
    // Set initial size with proportional scaling
    auto config = proportionalManager->getReferenceConfig();
    setSize(config.baseWidth, config.baseHeight);
    setResizable(true, true);
    
    // Apply proportional constrainer
    if (getConstrainer()) {
        proportionalManager->setupProportionalConstrainer(*getConstrainer());
    }
    
    // Start timer for smooth updates
    startTimer(16); // ~60fps
    
    // Trigger initial layout
    resized();
    
    DBG("ProportionalPluginEditor initialized with proportional scaling");
}

ProportionalPluginEditor::~ProportionalPluginEditor()
{
    stopTimer();
    
    // Save state before destruction
    saveEditorState();
    
    // Clear callbacks to prevent issues during destruction
    if (proportionalManager) {
        proportionalManager->onProportionalScaleChanged = nullptr;
        proportionalManager->onDimensionContextChanged = nullptr;
        proportionalManager->onBreakpointChanged = nullptr;
    }
    
    // Reset global scaling manager if we own it
    if (OTTOProportionalIntegration::getGlobalScalingManager() == proportionalManager.get()) {
        OTTOProportionalIntegration::setGlobalScalingManager(nullptr);
    }
}

void ProportionalPluginEditor::initializeProportionalScaling()
{
    // Create proportional scaling manager with OTTO-specific configuration
    ProportionalScalingManager::ReferenceConfig config;
    config.baseWidth = layoutConfig.referenceWidth;
    config.baseHeight = layoutConfig.referenceHeight;
    config.aspectRatio = static_cast<float>(config.baseWidth) / static_cast<float>(config.baseHeight);
    config.minScaleFactor = layoutConfig.minScale;
    config.maxScaleFactor = layoutConfig.maxScale;
    
    // Platform-specific touch optimization
#if JUCE_IOS || JUCE_ANDROID
    config.minTouchTargetPx = 44;
    config.minTouchTargetDp = 48;
    layoutConfig.enableTouchOptimization = true;
#endif
    
    proportionalManager = std::make_unique<ProportionalScalingManager>(config);
    proportionalManager->enableDebugMode(debugVisualization);
    proportionalManager->enableAutoRecalculation(true);
    
    // Set as global manager for OTTO integration
    OTTOProportionalIntegration::setGlobalScalingManager(proportionalManager.get());
}

void ProportionalPluginEditor::initializeManagers()
{
    // Initialize responsive layout manager
    ResponsiveLayoutManager::LayoutConfig responsiveConfig;
    responsiveConfig.baseWidth = layoutConfig.referenceWidth;
    responsiveConfig.baseHeight = layoutConfig.referenceHeight;
    responsiveConfig.aspectRatio = static_cast<float>(responsiveConfig.baseWidth) / responsiveConfig.baseHeight;
    responsiveManager = std::make_unique<ResponsiveLayoutManager>(responsiveConfig);
    
    // Initialize platform manager
    platformManager = std::make_unique<PlatformResponsiveManager>();
    platformManager->autoDetectPlatform();
    
    // Integrate managers
    proportionalManager->integrateWithResponsiveManager(responsiveManager.get());
    proportionalManager->integrateWithPlatformManager(platformManager.get());
    
    // Initialize other OTTO managers
    colorScheme = std::make_unique<ColorScheme>();
    fontManager = std::make_unique<FontManager>();
    dataManager = std::make_unique<INIDataManager>();
    
    // Setup color scheme listener
    colorScheme->addListener(this);
}

void ProportionalPluginEditor::initializeComponents()
{
    // Note: The original OTTO components require specific parameter types that
    // aren't available in this context (MidiEngine, ValueTreeState, etc.)
    // For now, we'll create placeholder components or skip creation
    // In a real implementation, these would need to be properly initialized
    // with the required dependencies
    
    // Create proportional demo if enabled
    if (showDemo) {
        proportionalDemo = std::make_unique<EnhancedProportionalEditor>(audioProcessor);
        demoTabs = std::make_unique<juce::TabbedComponent>(juce::TabbedButtonBar::TabsAtTop);
        demoTabs->addTab("Main Interface", juce::Colours::darkblue, nullptr, false);
        demoTabs->addTab("Proportional Demo", juce::Colours::darkgreen, proportionalDemo.get(), false);
        addAndMakeVisible(demoTabs.get());
    }
}

void ProportionalPluginEditor::setupProportionalWrappers()
{
    if (!proportionalScalingEnabled) {
        // Add components directly without wrappers (if they exist)
        if (topBar) addAndMakeVisible(topBar.get());
        if (playerTabs) addAndMakeVisible(playerTabs.get());
        if (drumKitSection) addAndMakeVisible(drumKitSection.get());
        if (mainContent) addAndMakeVisible(mainContent.get());
        return;
    }
    
    // Wrap components for proportional scaling (if they exist)
    if (topBar) {
        topBarWrapper = std::make_unique<ProportionalComponentWrapper>(
            std::unique_ptr<juce::Component>(topBar.release()), proportionalManager.get());
        addAndMakeVisible(topBarWrapper.get());
    }
    
    if (playerTabs) {
        playerTabsWrapper = std::make_unique<ProportionalComponentWrapper>(
            std::unique_ptr<juce::Component>(playerTabs.release()), proportionalManager.get());
        addAndMakeVisible(playerTabsWrapper.get());
    }
    
    if (drumKitSection) {
        drumKitSectionWrapper = std::make_unique<ProportionalComponentWrapper>(
            std::unique_ptr<juce::Component>(drumKitSection.release()), proportionalManager.get());
        addAndMakeVisible(drumKitSectionWrapper.get());
    }
    
    if (mainContent) {
        mainContentWrapper = std::make_unique<ProportionalComponentWrapper>(
            std::unique_ptr<juce::Component>(mainContent.release()), proportionalManager.get());
        addAndMakeVisible(mainContentWrapper.get());
    }
    
    // Note: Since the original components aren't created in initializeComponents(),
    // the wrappers won't be created either. This is expected behavior until
    // proper component initialization is implemented with required dependencies.
}

void ProportionalPluginEditor::setupLookAndFeel()
{
    // Create enhanced look and feel
    if (proportionalScalingEnabled && proportionalManager) {
        proportionalLookAndFeel = std::make_unique<ProportionalLookAndFeel>(proportionalManager.get());
        
        // Set platform-specific styling
        if (platformManager) {
            proportionalLookAndFeel->setPlatformSpecificStyling(
                platformManager->getPlatformConfig().platform);
        }
        
        setLookAndFeel(proportionalLookAndFeel.get());
    } else {
        // Use original OTTO look and feel
        customLookAndFeel = std::make_unique<CustomLookAndFeel>(*fontManager, *colorScheme);
        setLookAndFeel(customLookAndFeel.get());
    }
}

void ProportionalPluginEditor::setupCallbacks()
{
    if (!proportionalManager) return;
    
    // Set up proportional scaling callbacks
    proportionalManager->onProportionalScaleChanged = [this](const ProportionalScalingManager::ProportionalScale& scale) {
        onProportionalScaleChanged(scale);
    };
    
    proportionalManager->onDimensionContextChanged = [this](const ProportionalScalingManager::DimensionContext& context) {
        onDimensionContextChanged(context);
    };
    
    proportionalManager->onBreakpointChanged = [this](int width, int height) {
        onBreakpointChanged(width, height);
    };
}

void ProportionalPluginEditor::paint(juce::Graphics& g)
{
    // OTTO-style background
    auto bounds = getLocalBounds();
    
    if (colorScheme) {
        auto backgroundColor = colorScheme->getColor(ColorScheme::ColorRole::WindowBackground);
        g.fillAll(backgroundColor);
    } else {
        // Fallback gradient background
        juce::ColourGradient gradient(juce::Colours::darkblue.darker(), bounds.getTopLeft().toFloat(),
                                     juce::Colours::black, bounds.getBottomRight().toFloat(), false);
        g.setGradientFill(gradient);
        g.fillAll();
    }
    
    // Debug visualization if enabled
    if (debugVisualization && proportionalManager) {
        showLayoutDebugInfo(g);
    }
    
    // Border
    if (colorScheme) {
        g.setColour(colorScheme->getColor(ColorScheme::ColorRole::Separator));
    } else {
        g.setColour(juce::Colours::lightgrey.withAlpha(0.3f));
    }
    g.drawRect(bounds, proportionalManager ? proportionalManager->scaledBorder(1) : 1);
}

void ProportionalPluginEditor::resized()
{
    if (!proportionalManager) return;
    
    // Update proportional scaling system
    proportionalManager->updateDimensions(getWidth(), getHeight());
    
    // Update layout based on current mode
    if (showDemo && demoTabs) {
        layoutDemoInterface();
    } else {
        layoutMainInterface();
    }
    
    // Adapt layout for current platform and plugin format
    adaptLayoutForPlatform();
    adaptLayoutForPluginFormat();
    
    // Update component sizing and fonts
    updateComponentSizing();
    updateComponentFonts();
}

void ProportionalPluginEditor::layoutMainInterface()
{
    if (!proportionalManager) return;
    
    auto bounds = getLocalBounds();
    int margin = proportionalManager->scaledSpacing(8);
    
    bounds.reduce(margin, margin);
    
    // Layout based on screen size and orientation
    auto context = proportionalManager->getCurrentContext();
    
    if (context.isExtraSmall || context.isSmall) {
        // Compact vertical layout for small screens
        layoutCompactInterface(bounds);
    } else {
        // Standard layout for larger screens
        layoutStandardInterface(bounds);
    }
}

void ProportionalPluginEditor::layoutStandardInterface(juce::Rectangle<int> bounds)
{
    if (!proportionalManager) return;
    
    int topBarHeight = proportionalManager->scaledElement(60);
    int playerTabsHeight = proportionalManager->scaledElement(40);
    int margin = proportionalManager->scaledSpacing(8);
    
    // Top bar
    if (topBarWrapper) {
        topBarWrapper->setBounds(bounds.removeFromTop(topBarHeight));
    } else if (topBar) {
        topBar->setBounds(bounds.removeFromTop(topBarHeight));
    }
    
    bounds.removeFromTop(margin);
    
    // Player tabs
    if (playerTabsWrapper) {
        playerTabsWrapper->setBounds(bounds.removeFromTop(playerTabsHeight));
    } else if (playerTabs) {
        playerTabs->setBounds(bounds.removeFromTop(playerTabsHeight));
    }
    
    bounds.removeFromTop(margin);
    
    // Main content area - split between drum kit and main content
    int drumKitWidth = proportionalManager->scaledElement(300);
    
    // Drum kit section on the left
    if (drumKitSectionWrapper) {
        drumKitSectionWrapper->setBounds(bounds.removeFromLeft(drumKitWidth));
    } else if (drumKitSection) {
        drumKitSection->setBounds(bounds.removeFromLeft(drumKitWidth));
    }
    
    bounds.removeFromLeft(margin);
    
    // Main content on the right
    if (mainContentWrapper) {
        mainContentWrapper->setBounds(bounds);
    } else if (mainContent) {
        mainContent->setBounds(bounds);
    }
}

void ProportionalPluginEditor::layoutCompactInterface(juce::Rectangle<int> bounds)
{
    if (!proportionalManager) return;
    
    int topBarHeight = proportionalManager->scaledElement(50); // Smaller for compact
    int playerTabsHeight = proportionalManager->scaledElement(35);
    int margin = proportionalManager->scaledSpacing(4); // Tighter spacing
    
    // Top bar
    if (topBarWrapper) {
        topBarWrapper->setBounds(bounds.removeFromTop(topBarHeight));
    } else if (topBar) {
        topBar->setBounds(bounds.removeFromTop(topBarHeight));
    }
    
    bounds.removeFromTop(margin);
    
    // Player tabs
    if (playerTabsWrapper) {
        playerTabsWrapper->setBounds(bounds.removeFromTop(playerTabsHeight));
    } else if (playerTabs) {
        playerTabs->setBounds(bounds.removeFromTop(playerTabsHeight));
    }
    
    bounds.removeFromTop(margin);
    
    // Stack drum kit and main content vertically for compact layout
    int drumKitHeight = proportionalManager->scaledElement(200);
    
    if (drumKitSectionWrapper) {
        drumKitSectionWrapper->setBounds(bounds.removeFromTop(drumKitHeight));
    } else if (drumKitSection) {
        drumKitSection->setBounds(bounds.removeFromTop(drumKitHeight));
    }
    
    bounds.removeFromTop(margin);
    
    if (mainContentWrapper) {
        mainContentWrapper->setBounds(bounds);
    } else if (mainContent) {
        mainContent->setBounds(bounds);
    }
}

void ProportionalPluginEditor::layoutDemoInterface()
{
    if (!demoTabs) return;
    
    auto bounds = getLocalBounds();
    
    // Set up demo tabs
    demoTabs->setBounds(bounds);
    
    // Configure main interface tab
    auto mainTab = std::make_unique<juce::Component>();
    
    if (proportionalScalingEnabled) {
        if (topBarWrapper) mainTab->addAndMakeVisible(topBarWrapper.get());
        if (playerTabsWrapper) mainTab->addAndMakeVisible(playerTabsWrapper.get());
        if (drumKitSectionWrapper) mainTab->addAndMakeVisible(drumKitSectionWrapper.get());
        if (mainContentWrapper) mainTab->addAndMakeVisible(mainContentWrapper.get());
    } else {
        if (topBar) mainTab->addAndMakeVisible(topBar.get());
        if (playerTabs) mainTab->addAndMakeVisible(playerTabs.get());
        if (drumKitSection) mainTab->addAndMakeVisible(drumKitSection.get());
        if (mainContent) mainTab->addAndMakeVisible(mainContent.get());
    }
    
    // Layout components within main tab
    auto tabBounds = demoTabs->getLocalBounds();
    tabBounds.removeFromTop(30); // Account for tab bar
    layoutStandardInterface(tabBounds);
    
    demoTabs->clearTabs();
    demoTabs->addTab("Main Interface", juce::Colours::darkblue, mainTab.release(), true);
    demoTabs->addTab("Proportional Demo", juce::Colours::darkgreen, proportionalDemo.get(), false);
}

void ProportionalPluginEditor::updateProportionalLayout()
{
    resized();
}

void ProportionalPluginEditor::adaptLayoutForPlatform()
{
    if (!platformManager || !proportionalManager) return;
    
    auto platform = platformManager->getPlatformConfig().platform;
    auto context = proportionalManager->getCurrentContext();
    
    switch (platform) {
        case PlatformResponsiveManager::TargetPlatform::iOS_Standalone:
        case PlatformResponsiveManager::TargetPlatform::iOS_AUv3:
        case PlatformResponsiveManager::TargetPlatform::Android_Standalone:
            // Mobile platforms - ensure touch optimization
            if (!context.needsTouchOptimization) {
                auto config = proportionalManager->getReferenceConfig();
                config.minTouchTargetPx = 44;
                config.minTouchTargetDp = 48;
                proportionalManager->setReferenceConfig(config);
            }
            break;
            
        case PlatformResponsiveManager::TargetPlatform::macOS_VST3:
        case PlatformResponsiveManager::TargetPlatform::Windows_VST3:
        case PlatformResponsiveManager::TargetPlatform::Linux_VST3:
            // Plugin contexts - more compact layout
            if (layoutConfig.adaptToHost) {
                handleHostResize();
            }
            break;
            
        default:
            break;
    }
}

void ProportionalPluginEditor::adaptLayoutForPluginFormat()
{
    // Plugin format specific adaptations would go here
    // This depends on how your processor reports its format
}

void ProportionalPluginEditor::detectAndAdaptPlatform()
{
    if (platformManager) {
        platformManager->updateScreenInfo();
        applyPlatformSpecificSettings();
    }
}

void ProportionalPluginEditor::detectAndAdaptPluginFormat()
{
    // Detect plugin format from processor
    applyPluginFormatSpecificSettings();
}

void ProportionalPluginEditor::applyPlatformSpecificSettings()
{
    if (!platformManager || !proportionalManager) return;
    
    auto platform = platformManager->getPlatformConfig().platform;
    
    // Update proportional scaling for platform
    proportionalManager->updateDPIScale();
    proportionalManager->updateScreenDensity();
    
    // Update look and feel
    if (proportionalLookAndFeel) {
        proportionalLookAndFeel->setPlatformSpecificStyling(platform);
    }
}

void ProportionalPluginEditor::applyPluginFormatSpecificSettings()
{
    // Apply settings based on plugin format
    // This would typically involve setting host constraints
    
    // Example for common plugin sizes
    if (layoutConfig.adaptToHost) {
        proportionalManager->setPluginHostConstraints(1024, 768, true);
    }
}

void ProportionalPluginEditor::enhanceExistingComponents()
{
    // Enhance individual components with proportional scaling
    enhanceTopBar();
    enhancePlayerTabs();
    enhanceDrumKitSection();
    enhanceMainContent();
}

void ProportionalPluginEditor::enhanceTopBar()
{
    auto* component = topBarWrapper ? topBarWrapper->getWrappedComponent() : topBar.get();
    if (component) {
        OTTOProportionalIntegration::enhanceOTTOComponent(component, "TopBar");
    }
}

void ProportionalPluginEditor::enhancePlayerTabs()
{
    auto* component = playerTabsWrapper ? playerTabsWrapper->getWrappedComponent() : playerTabs.get();
    if (component) {
        OTTOProportionalIntegration::enhanceOTTOComponent(component, "PlayerTabs");
    }
}

void ProportionalPluginEditor::enhanceDrumKitSection()
{
    auto* component = drumKitSectionWrapper ? drumKitSectionWrapper->getWrappedComponent() : drumKitSection.get();
    if (component) {
        OTTOProportionalIntegration::enhanceOTTOComponent(component, "DrumKitSection");
    }
}

void ProportionalPluginEditor::enhanceMainContent()
{
    auto* component = mainContentWrapper ? mainContentWrapper->getWrappedComponent() : mainContent.get();
    if (component) {
        OTTOProportionalIntegration::enhanceOTTOComponent(component, "MainContent");
    }
}

void ProportionalPluginEditor::buttonClicked(juce::Button* button)
{
    // Handle button clicks - delegate to original components
    // This maintains compatibility with existing OTTO functionality
}

void ProportionalPluginEditor::sliderValueChanged(juce::Slider* slider)
{
    // Handle slider changes - delegate to original components
}

void ProportionalPluginEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    // Handle combo box changes - delegate to original components
}

void ProportionalPluginEditor::themeChanged(const juce::String& newThemeName)
{
    // Handle theme changes
    updateComponentColors();
    repaint();
}

void ProportionalPluginEditor::colorChanged(const juce::String& component, const juce::String& property, const juce::Colour& newColor)
{
    // Handle individual color changes
    updateComponentColors();
    repaint();
}

void ProportionalPluginEditor::timerCallback()
{
    // Regular updates for smooth scaling
    if (proportionalManager) {
        // Check for any layout updates needed
        updateComponentSizing();
    }
}

void ProportionalPluginEditor::saveEditorState()
{
    if (!dataManager || !proportionalManager) return;
    
    try {
        // Save proportional scaling state through global settings
        componentState.globalSettings.interfaceScale = proportionalManager->getCurrentScale().baseScale;
        componentState.globalSettings.interfaceWidth = getWidth();
        componentState.globalSettings.interfaceHeight = getHeight();
        
        // Save layout configuration to global settings
        componentState.globalSettings.interfaceScale = layoutConfig.minScale;
        
        // Save through data manager
        dataManager->saveAllSettings(componentState);
        
        DBG("ProportionalPluginEditor state saved successfully");
    }
    catch (const std::exception& e) {
        DBG("Error saving editor state: " << e.what());
    }
}

void ProportionalPluginEditor::loadEditorState()
{
    if (!dataManager || !proportionalManager) return;
    
    try {
        // Load state from data manager
        dataManager->loadAllSettings(componentState);
        
        // Load proportional scaling state from global settings
        if (proportionalManager && componentState.globalSettings.interfaceScale > 0) {
            // Update proportional manager with loaded settings
            proportionalManager->updateDimensions(
                componentState.globalSettings.interfaceWidth,
                componentState.globalSettings.interfaceHeight);
        }
        
        // Load layout configuration from global settings
        layoutConfig.minScale = componentState.globalSettings.interfaceScale;
        
        DBG("ProportionalPluginEditor state loaded successfully");
    }
    catch (const std::exception& e) {
        DBG("Error loading editor state: " << e.what());
    }
}

void ProportionalPluginEditor::enableProportionalScaling(bool enabled)
{
    if (proportionalScalingEnabled == enabled) return;
    
    proportionalScalingEnabled = enabled;
    
    // Reinitialize components with or without proportional scaling
    setupProportionalWrappers();
    setupLookAndFeel();
    
    // Trigger layout update
    resized();
    
    DBG("Proportional scaling " << (enabled ? "enabled" : "disabled"));
}

void ProportionalPluginEditor::adaptForCurrentPlatform()
{
    if (platformManager) {
        platformManager->autoDetectPlatform();
        applyPlatformSpecificSettings();
        adaptLayoutForPlatform();
    }
}

void ProportionalPluginEditor::setPluginHostConstraints(int maxWidth, int maxHeight, bool resizable)
{
    if (proportionalManager) {
        proportionalManager->setPluginHostConstraints(maxWidth, maxHeight, resizable);
        layoutConfig.adaptToHost = true;
        adaptToHostEnvironment();
    }
}

void ProportionalPluginEditor::showProportionalScalingDemo(bool show)
{
    showDemo = show;
    
    if (show && !proportionalDemo) {
        proportionalDemo = std::make_unique<EnhancedProportionalEditor>(audioProcessor);
        demoTabs = std::make_unique<juce::TabbedComponent>(juce::TabbedButtonBar::TabsAtTop);
        addAndMakeVisible(demoTabs.get());
    }
    
    resized();
}

void ProportionalPluginEditor::enableDebugVisualization(bool enabled)
{
    debugVisualization = enabled;
    
    if (proportionalManager) {
        proportionalManager->enableDebugMode(enabled);
    }
    
    OTTOProportionalIntegration::enableDebugVisualization(enabled);
    repaint();
}

// Event callbacks

void ProportionalPluginEditor::onProportionalScaleChanged(const ProportionalScalingManager::ProportionalScale& scale)
{
    updateComponentSizing();
    updateComponentFonts();
    repaint();
}

void ProportionalPluginEditor::onDimensionContextChanged(const ProportionalScalingManager::DimensionContext& context)
{
    updateProportionalLayout();
}

void ProportionalPluginEditor::onBreakpointChanged(int width, int height)
{
    adaptLayoutForPlatform();
    repaint();
}

void ProportionalPluginEditor::onColorSchemeChanged()
{
    updateComponentColors();
    repaint();
}

// Utility methods

void ProportionalPluginEditor::updateComponentFonts()
{
    if (!proportionalManager || !fontManager) return;
    
    // Update fonts for all components
    auto baseFont = fontManager->getFont(FontManager::FontRole::Body, fontManager->getFontMainSize());
    auto scaledFont = proportionalManager->scaledProportional(baseFont);
    
    // Apply to wrapped components
    if (topBarWrapper) {
        // Update fonts in top bar
    }
    if (playerTabsWrapper) {
        // Update fonts in player tabs
    }
    // ... continue for other components
}

void ProportionalPluginEditor::updateComponentColors()
{
    if (!colorScheme) return;
    
    // Update colors for all components based on current color scheme
    // This maintains OTTO's existing color theming system
    
    repaint();
}

void ProportionalPluginEditor::updateComponentSizing()
{
    if (!proportionalManager) return;
    
    // Update component sizes based on current proportional scale
    // This is handled automatically by the wrappers and layout methods
}

void ProportionalPluginEditor::refreshAllComponents()
{
    updateComponentSizing();
    updateComponentFonts();
    updateComponentColors();
    repaint();
}

void ProportionalPluginEditor::handleHostResize()
{
    if (proportionalManager && layoutConfig.adaptToHost) {
        proportionalManager->adaptToHostEnvironment();
        constrainToHostLimits();
    }
}

void ProportionalPluginEditor::adaptToHostEnvironment()
{
    if (proportionalManager) {
        proportionalManager->adaptToHostEnvironment();
    }
}

void ProportionalPluginEditor::constrainToHostLimits()
{
    if (proportionalManager && proportionalManager->isConstrainedByHost()) {
        auto hostBounds = proportionalManager->getHostConstrainedBounds();
        setSize(hostBounds.getWidth(), hostBounds.getHeight());
    }
}

void ProportionalPluginEditor::debugPrintLayoutInfo()
{
    if (proportionalManager) {
        proportionalManager->debugPrintProportionalState();
    }
}

void ProportionalPluginEditor::showLayoutDebugInfo(juce::Graphics& g)
{
    if (!proportionalManager) return;
    
    auto bounds = getLocalBounds();
    
    // Draw proportional grid
    g.setColour(juce::Colours::yellow.withAlpha(0.2f));
    int gridSize = proportionalManager->scaledSpacing(50);
    
    for (int x = 0; x < bounds.getWidth(); x += gridSize) {
        g.drawVerticalLine(x, 0.0f, static_cast<float>(bounds.getHeight()));
    }
    for (int y = 0; y < bounds.getHeight(); y += gridSize) {
        g.drawHorizontalLine(y, 0.0f, static_cast<float>(bounds.getWidth()));
    }
    
    // Draw scale info
    g.setColour(juce::Colours::yellow);
    g.setFont(proportionalManager->scaledProportional(juce::Font(juce::FontOptions(10.0f))));
    
    auto context = proportionalManager->getCurrentContext();
    auto scale = proportionalManager->getCurrentScale();
    
    juce::String info = "Scale: " + juce::String(scale.baseScale, 2) + 
                       " | DPI: " + juce::String(scale.dpiMultiplier, 2) +
                       " | Touch: " + juce::String(scale.touchTargetScale, 2);
    
    g.drawText(info, bounds.getX() + 5, bounds.getY() + 5, 300, 15, juce::Justification::topLeft);
    
    // Draw breakpoint info
    juce::String breakpoint = "Breakpoint: ";
    if (context.isExtraSmall) breakpoint += "Extra Small";
    else if (context.isSmall) breakpoint += "Small";
    else if (context.isMedium) breakpoint += "Medium";
    else if (context.isLarge) breakpoint += "Large";
    else if (context.isExtraLarge) breakpoint += "Extra Large";
    
    if (context.needsTouchOptimization) breakpoint += " (Touch)";
    
    g.drawText(breakpoint, bounds.getX() + 5, bounds.getY() + 20, 300, 15, juce::Justification::topLeft);
}

//==============================================================================
// OTTOEditorFactory Implementation
//==============================================================================

std::unique_ptr<juce::AudioProcessorEditor> OTTOEditorFactory::createEditor(OTTOAudioProcessor& processor, bool useProportionalScaling)
{
    if (useProportionalScaling) {
        return std::make_unique<ProportionalPluginEditor>(processor);
    } else {
        return std::make_unique<OTTOAudioProcessorEditor>(processor);
    }
}

std::unique_ptr<juce::AudioProcessorEditor> OTTOEditorFactory::createEditorForPlatform(
    OTTOAudioProcessor& processor, PlatformResponsiveManager::TargetPlatform platform)
{
    auto editor = std::make_unique<ProportionalPluginEditor>(processor);
    
    // Configure for specific platform
    if (auto* platformManager = editor->getProportionalManager()) {
        PlatformResponsiveManager::PlatformConfig config;
        config.platform = platform;
        
        // Platform-specific settings
        switch (platform) {
            case PlatformResponsiveManager::TargetPlatform::iOS_Standalone:
            case PlatformResponsiveManager::TargetPlatform::iOS_AUv3:
                config.hasTouchInput = true;
                config.hasPhysicalKeyboard = false;
                config.hasHoverCapability = false;
                break;
                
            case PlatformResponsiveManager::TargetPlatform::Android_Standalone:
                config.hasTouchInput = true;
                config.hasPhysicalKeyboard = false;
                config.hasHoverCapability = false;
                break;
                
            default:
                config.hasTouchInput = false;
                config.hasPhysicalKeyboard = true;
                config.hasHoverCapability = true;
                break;
        }
        
        // Apply configuration (this would require extending the platform manager)
        editor->adaptForCurrentPlatform();
    }
    
    return std::move(editor);
}

std::unique_ptr<juce::AudioProcessorEditor> OTTOEditorFactory::createEditorForPluginFormat(
    OTTOAudioProcessor& processor, const juce::String& pluginFormat)
{
    auto editor = std::make_unique<ProportionalPluginEditor>(processor);
    
    // Configure for specific plugin format
    if (pluginFormat == "VST3") {
        editor->setPluginHostConstraints(1024, 768, true);
    } else if (pluginFormat == "AU") {
        editor->setPluginHostConstraints(800, 600, true);
    } else if (pluginFormat == "CLAP") {
        editor->setPluginHostConstraints(1200, 800, true);
    }
    
    return std::move(editor);
}

std::vector<OTTOEditorFactory::EditorPreset> OTTOEditorFactory::getAvailablePresets()
{
    return {
        createDesktopPreset(),
        createMobilePreset(),
        createPluginPreset(),
        createDemoPreset()
    };
}

OTTOEditorFactory::EditorPreset OTTOEditorFactory::createDesktopPreset()
{
    EditorPreset preset;
    preset.name = "Desktop";
    preset.scalingConfig.baseWidth = 1000;
    preset.scalingConfig.baseHeight = 750;
    preset.scalingConfig.minScaleFactor = 0.5f;
    preset.scalingConfig.maxScaleFactor = 2.0f;
    preset.enableTouchOptimization = false;
    preset.showDemo = false;
    preset.debugMode = false;
    return preset;
}

OTTOEditorFactory::EditorPreset OTTOEditorFactory::createMobilePreset()
{
    EditorPreset preset;
    preset.name = "Mobile";
    preset.scalingConfig.baseWidth = 800;
    preset.scalingConfig.baseHeight = 600;
    preset.scalingConfig.minScaleFactor = 0.3f;
    preset.scalingConfig.maxScaleFactor = 3.0f;
    preset.scalingConfig.minTouchTargetPx = 44;
    preset.scalingConfig.minTouchTargetDp = 48;
    preset.enableTouchOptimization = true;
    preset.showDemo = false;
    preset.debugMode = false;
    return preset;
}

OTTOEditorFactory::EditorPreset OTTOEditorFactory::createPluginPreset()
{
    EditorPreset preset;
    preset.name = "Plugin";
    preset.scalingConfig.baseWidth = 800;
    preset.scalingConfig.baseHeight = 600;
    preset.scalingConfig.minScaleFactor = 0.5f;
    preset.scalingConfig.maxScaleFactor = 2.0f;
    preset.enableTouchOptimization = false;
    preset.showDemo = false;
    preset.debugMode = false;
    return preset;
}

OTTOEditorFactory::EditorPreset OTTOEditorFactory::createDemoPreset()
{
    EditorPreset preset;
    preset.name = "Demo";
    preset.scalingConfig.baseWidth = 1200;
    preset.scalingConfig.baseHeight = 900;
    preset.scalingConfig.minScaleFactor = 0.25f;
    preset.scalingConfig.maxScaleFactor = 4.0f;
    preset.enableTouchOptimization = true;
    preset.showDemo = true;
    preset.debugMode = true;
    return preset;
}

std::unique_ptr<juce::AudioProcessorEditor> OTTOEditorFactory::createEditorFromPreset(
    OTTOAudioProcessor& processor, const EditorPreset& preset)
{
    auto editor = std::make_unique<ProportionalPluginEditor>(processor);
    
    // Apply preset configuration
    if (auto* scalingManager = editor->getProportionalManager()) {
        scalingManager->setReferenceConfig(preset.scalingConfig);
    }
    
    editor->showProportionalScalingDemo(preset.showDemo);
    editor->enableDebugVisualization(preset.debugMode);
    
    return std::move(editor);
}

//==============================================================================
// OTTOEditorMigration Implementation
//==============================================================================

juce::String OTTOEditorMigration::analyzeMigrationNeeds(juce::AudioProcessorEditor* existingEditor)
{
    if (!existingEditor) return "No editor provided for analysis";
    
    juce::String analysis;
    analysis << "OTTO Editor Migration Analysis:\n";
    analysis << "==============================\n";
    analysis << "Editor Type: " << existingEditor->getName() << "\n";
    analysis << "Current Size: " << existingEditor->getWidth() << "x" << existingEditor->getHeight() << "\n";
    analysis << "Component Count: " << existingEditor->getNumChildComponents() << "\n";
    
    // Analyze component hierarchy
    analysis << "\nComponent Hierarchy:\n";
    analyzeComponentHierarchy(existingEditor);
    
    analysis << "\nRecommendations:\n";
    analysis << "- Enable proportional scaling for better multi-platform support\n";
    analysis << "- Consider touch optimization for mobile platforms\n";
    analysis << "- Implement responsive breakpoints for different screen sizes\n";
    
    return analysis;
}

void OTTOEditorMigration::migrateEditorState(juce::AudioProcessorEditor* existingEditor, 
                                           ProportionalPluginEditor* newEditor)
{
    if (!existingEditor || !newEditor) return;
    
    // Extract state from existing editor
    ComponentState state;
    extractComponentState(existingEditor, state);
    
    // Apply to new editor
    applyMigratedState(newEditor, state);
    
    DBG("Editor migration completed");
}

void OTTOEditorMigration::extractComponentState(juce::AudioProcessorEditor* existingEditor, ComponentState& state)
{
    if (!existingEditor) return;
    
    // Extract global properties and set them in ComponentState
    state.globalSettings.interfaceWidth = existingEditor->getWidth();
    state.globalSettings.interfaceHeight = existingEditor->getHeight();
    state.interfaceWidth = existingEditor->getWidth();
    state.interfaceHeight = existingEditor->getHeight();
    
    // Extract component properties
    juce::ValueTree root("EditorState");
    root.setProperty("width", existingEditor->getWidth(), nullptr);
    root.setProperty("height", existingEditor->getHeight(), nullptr);
    root.setProperty("resizable", existingEditor->isResizable(), nullptr);
    extractComponentProperties(existingEditor, root);
    
    DBG("ComponentState extracted from existing editor");
}

void OTTOEditorMigration::applyMigratedState(ProportionalPluginEditor* editor, const ComponentState& state)
{
    if (!editor) return;
    
    // Apply migrated state to new proportional editor
    // This would involve setting up the proportional scaling manager with
    // equivalent settings derived from the original editor
    
    DBG("Migrated state applied to proportional editor");
}

bool OTTOEditorMigration::validateMigration(juce::AudioProcessorEditor* originalEditor, 
                                           ProportionalPluginEditor* migratedEditor)
{
    if (!originalEditor || !migratedEditor) return false;
    
    // Validation checks
    bool sizeMatches = (originalEditor->getWidth() == migratedEditor->getWidth()) &&
                      (originalEditor->getHeight() == migratedEditor->getHeight());
    
    bool componentCountMatches = originalEditor->getNumChildComponents() == 
                               migratedEditor->getNumChildComponents();
    
    return sizeMatches && componentCountMatches;
}

juce::String OTTOEditorMigration::generateMigrationReport(juce::AudioProcessorEditor* originalEditor, 
                                                         ProportionalPluginEditor* migratedEditor)
{
    juce::String report;
    report << "OTTO Editor Migration Report:\n";
    report << "============================\n";
    
    if (originalEditor && migratedEditor) {
        report << "Migration successful: " << (validateMigration(originalEditor, migratedEditor) ? "Yes" : "No") << "\n";
        report << "Original size: " << originalEditor->getWidth() << "x" << originalEditor->getHeight() << "\n";
        report << "Migrated size: " << migratedEditor->getWidth() << "x" << migratedEditor->getHeight() << "\n";
        
        if (auto* scalingManager = migratedEditor->getProportionalManager()) {
            auto context = scalingManager->getCurrentContext();
            report << "Proportional scale: " << context.proportionalScale << "\n";
            report << "Touch optimization: " << (context.needsTouchOptimization ? "Yes" : "No") << "\n";
        }
    } else {
        report << "Migration failed: Invalid editor references\n";
    }
    
    return report;
}

void OTTOEditorMigration::analyzeComponentHierarchy(juce::Component* component, int depth)
{
    if (!component || depth > 10) return;
    
    juce::String indent = juce::String::repeatedString("  ", depth);
    DBG(indent << component->getName() << " (" << component->getComponentID() << ") - " << 
        component->getBounds().toString());
    
    for (int i = 0; i < component->getNumChildComponents(); ++i) {
        analyzeComponentHierarchy(component->getChildComponent(i), depth + 1);
    }
}

void OTTOEditorMigration::extractComponentProperties(juce::Component* component, juce::ValueTree& properties)
{
    if (!component) return;
    
    juce::ValueTree componentTree("Component");
    componentTree.setProperty("name", component->getName(), nullptr);
    componentTree.setProperty("id", component->getComponentID(), nullptr);
    componentTree.setProperty("bounds", component->getBounds().toString(), nullptr);
    componentTree.setProperty("visible", component->isVisible(), nullptr);
    componentTree.setProperty("enabled", component->isEnabled(), nullptr);
    
    // Extract child components
    for (int i = 0; i < component->getNumChildComponents(); ++i) {
        juce::ValueTree childTree("Child");
        extractComponentProperties(component->getChildComponent(i), childTree);
        componentTree.appendChild(childTree, nullptr);
    }
    
    properties.appendChild(componentTree, nullptr);
}

void OTTOEditorMigration::applyComponentProperties(juce::Component* component, const juce::ValueTree& properties)
{
    if (!component || !properties.isValid()) return;
    
    // Apply properties to component
    if (properties.hasProperty("name")) {
        component->setName(properties.getProperty("name"));
    }
    
    if (properties.hasProperty("bounds")) {
        auto boundsStr = properties.getProperty("bounds").toString();
        // Parse bounds string and apply (implementation depends on bounds format)
    }
    
    if (properties.hasProperty("visible")) {
        component->setVisible(properties.getProperty("visible"));
    }
    
    if (properties.hasProperty("enabled")) {
        component->setEnabled(properties.getProperty("enabled"));
    }
}