#include "ProportionalScalingDemo.h"
#include "JUCE8Compatibility.h"

//==============================================================================
// ProportionalScalingDemo Implementation
//==============================================================================

ProportionalScalingDemo::ProportionalScalingDemo()
{
    // Initialize proportional scaling manager with custom reference config
    ProportionalScalingManager::ReferenceConfig config;
    config.baseWidth = 800;
    config.baseHeight = 600;
    config.aspectRatio = 800.0f / 600.0f;
    config.minScaleFactor = 0.25f;
    config.maxScaleFactor = 4.0f;
    
    scalingManager = std::make_unique<ProportionalScalingManager>(config);
    scalingManager->enableDebugMode(true);
    
    initializeComponents();
    setupProportionalScaling();
    createDemoElements();
    
    isInitialized = true;
    
    // Set initial size to trigger proportional calculation
    setSize(800, 600);
}

ProportionalScalingDemo::~ProportionalScalingDemo()
{
    // Clear callbacks to prevent issues during destruction
    if (scalingManager) {
        scalingManager->onProportionalScaleChanged = nullptr;
        scalingManager->onDimensionContextChanged = nullptr;
        scalingManager->onBreakpointChanged = nullptr;
    }
}

void ProportionalScalingDemo::initializeComponents()
{
    // Title label
    titleLabel.setText("Proportional Scaling Demo - JUCE 8", juce::dontSendNotification);
    titleLabel.setFont(JUCE8_CREATE_FONT(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centredTop);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);
    
    // Info labels
    infoLabel.setText("Demonstrates proportional scaling across all platforms", juce::dontSendNotification);
    infoLabel.setFont(JUCE8_CREATE_FONT(14.0f));
    infoLabel.setJustificationType(juce::Justification::centredTop);
    infoLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(infoLabel);
    
    dimensionsLabel.setText("Dimensions: 800x600", juce::dontSendNotification);
    dimensionsLabel.setFont(JUCE8_CREATE_FONT(12.0f));
    dimensionsLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
    addAndMakeVisible(dimensionsLabel);
    
    scaleLabel.setText("Scale: 1.00", juce::dontSendNotification);
    scaleLabel.setFont(JUCE8_CREATE_FONT(12.0f));
    scaleLabel.setColour(juce::Label::textColourId, juce::Colours::cyan);
    addAndMakeVisible(scaleLabel);
    
    breakpointLabel.setText("Breakpoint: Large", juce::dontSendNotification);
    breakpointLabel.setFont(JUCE8_CREATE_FONT(12.0f));
    breakpointLabel.setColour(juce::Label::textColourId, juce::Colours::lime);
    addAndMakeVisible(breakpointLabel);
    
    // Interactive controls
    scalingSlider.setRange(0.25, 4.0, 0.01);
    scalingSlider.setValue(1.0);
    scalingSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    scalingSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    scalingSlider.addListener(this);
    addAndMakeVisible(scalingSlider);
    
    scalingSliderLabel.setText("Manual Scale Override:", juce::dontSendNotification);
    scalingSliderLabel.setFont(JUCE8_CREATE_FONT(12.0f));
    scalingSliderLabel.attachToComponent(&scalingSlider, true);
    addAndMakeVisible(scalingSliderLabel);
    
    widthSlider.setRange(200, 1600, 1);
    widthSlider.setValue(800);
    widthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    widthSlider.addListener(this);
    addAndMakeVisible(widthSlider);
    
    widthSliderLabel.setText("Width:", juce::dontSendNotification);
    widthSliderLabel.setFont(JUCE8_CREATE_FONT(12.0f));
    widthSliderLabel.attachToComponent(&widthSlider, true);
    addAndMakeVisible(widthSliderLabel);
    
    heightSlider.setRange(150, 1200, 1);
    heightSlider.setValue(600);
    heightSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    heightSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    heightSlider.addListener(this);
    addAndMakeVisible(heightSlider);
    
    heightSliderLabel.setText("Height:", juce::dontSendNotification);
    heightSliderLabel.setFont(JUCE8_CREATE_FONT(12.0f));
    heightSliderLabel.attachToComponent(&heightSlider, true);
    addAndMakeVisible(heightSliderLabel);
    
    // Touch-optimized buttons
    smallButton.setButtonText("Small");
    smallButton.addListener(this);
    addAndMakeVisible(smallButton);
    
    mediumButton.setButtonText("Medium");
    mediumButton.addListener(this);
    addAndMakeVisible(mediumButton);
    
    largeButton.setButtonText("Large");
    largeButton.addListener(this);
    addAndMakeVisible(largeButton);
    
    touchOptimizedButton.setButtonText("Touch Optimized");
    touchOptimizedButton.addListener(this);
    touchOptimizedButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
    addAndMakeVisible(touchOptimizedButton);
}

void ProportionalScalingDemo::setupProportionalScaling()
{
    if (!scalingManager) return;
    
    // Set up callbacks for real-time updates
    scalingManager->onProportionalScaleChanged = [this](const ProportionalScalingManager::ProportionalScale& scale) {
        onProportionalScaleChanged(scale);
    };
    
    scalingManager->onDimensionContextChanged = [this](const ProportionalScalingManager::DimensionContext& context) {
        onDimensionContextChanged(context);
    };
    
    scalingManager->onBreakpointChanged = [this](int width, int height) {
        onBreakpointChanged(width, height);
    };
    
    // Enable auto-recalculation
    scalingManager->enableAutoRecalculation(true);
}

void ProportionalScalingDemo::paint(juce::Graphics& g)
{
    // Background with proportional gradient
    auto bounds = getLocalBounds();
    juce::ColourGradient gradient(juce::Colours::darkblue, bounds.getTopLeft().toFloat(),
                                 juce::Colours::black, bounds.getBottomRight().toFloat(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Paint demo elements
    paintDemoElements(g);
    paintTouchTargetVisualization(g);
    paintScaleVisualization(g);
    
    // Border
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(bounds, scalingManager ? scalingManager->scaledBorder(2) : 2);
}

void ProportionalScalingDemo::resized()
{
    if (!isInitialized || !scalingManager) return;
    
    // Update scaling manager with new dimensions
    scalingManager->updateDimensions(getWidth(), getHeight());
    
    updateProportionalLayout();
    updateInfoLabels();
}

void ProportionalScalingDemo::updateProportionalLayout()
{
    if (!scalingManager) return;
    
    auto bounds = getLocalBounds();
    int margin = scalingManager->scaledSpacing(10);
    int lineHeight = scalingManager->scaledText(20);
    int controlHeight = scalingManager->scaledElement(25);
    int buttonWidth = scalingManager->scaledElement(100);
    int buttonHeight = scalingManager->scaledElement(30);
    
    // Title and info area
    auto titleArea = bounds.removeFromTop(lineHeight * 2 + margin);
    titleLabel.setBounds(titleArea.removeFromTop(lineHeight));
    titleLabel.setFont(scalingManager->scaledProportional(JUCE8_CREATE_FONT(24.0f, juce::Font::bold)));
    
    infoLabel.setBounds(titleArea);
    infoLabel.setFont(scalingManager->scaledProportional(JUCE8_CREATE_FONT(14.0f)));
    
    bounds.removeFromTop(margin);
    
    // Info labels area
    auto infoArea = bounds.removeFromTop(lineHeight * 3 + margin);
    dimensionsLabel.setBounds(infoArea.removeFromTop(lineHeight));
    dimensionsLabel.setFont(scalingManager->scaledProportional(JUCE8_CREATE_FONT(12.0f)));
    
    scaleLabel.setBounds(infoArea.removeFromTop(lineHeight));
    scaleLabel.setFont(scalingManager->scaledProportional(JUCE8_CREATE_FONT(12.0f)));
    
    breakpointLabel.setBounds(infoArea.removeFromTop(lineHeight));
    breakpointLabel.setFont(scalingManager->scaledProportional(JUCE8_CREATE_FONT(12.0f)));
    
    bounds.removeFromTop(margin);
    
    // Controls area
    auto controlsArea = bounds.removeFromTop(controlHeight * 3 + margin * 3);
    
    // Scaling slider
    auto scalingArea = controlsArea.removeFromTop(controlHeight);
    scalingArea.removeFromLeft(120); // Space for label
    scalingSlider.setBounds(scalingArea);
    
    controlsArea.removeFromTop(margin);
    
    // Width slider
    auto widthArea = controlsArea.removeFromTop(controlHeight);
    widthArea.removeFromLeft(120); // Space for label
    widthSlider.setBounds(widthArea);
    
    controlsArea.removeFromTop(margin);
    
    // Height slider
    auto heightArea = controlsArea.removeFromTop(controlHeight);
    heightArea.removeFromLeft(120); // Space for label
    heightSlider.setBounds(heightArea);
    
    bounds.removeFromTop(margin * 2);
    
    // Buttons area
    auto buttonsArea = bounds.removeFromTop(buttonHeight + margin);
    int buttonSpacing = scalingManager->scaledSpacing(10);
    
    // Layout buttons based on available space
    if (scalingManager->shouldUseCompactLayout()) {
        // Stack buttons vertically for compact layouts
        auto buttonArea = buttonsArea.removeFromLeft(buttonWidth);
        smallButton.setBounds(buttonArea.removeFromTop(buttonHeight / 2));
        mediumButton.setBounds(buttonArea.removeFromTop(buttonHeight / 2));
        
        buttonArea = buttonsArea.removeFromLeft(buttonWidth + buttonSpacing);
        buttonArea.removeFromLeft(buttonSpacing);
        largeButton.setBounds(buttonArea.removeFromTop(buttonHeight / 2));
        
        // Touch optimized button gets special treatment
        auto touchButtonBounds = juce::Rectangle<int>(
            buttonsArea.getX(), 
            buttonsArea.getY(), 
            scalingManager->scaledTouchTarget(buttonWidth), 
            scalingManager->scaledTouchTarget(buttonHeight / 2)
        );
        touchOptimizedButton.setBounds(scalingManager->ensureTouchTarget(touchButtonBounds));
    } else {
        // Horizontal layout for larger screens
        smallButton.setBounds(buttonsArea.removeFromLeft(buttonWidth));
        buttonsArea.removeFromLeft(buttonSpacing);
        
        mediumButton.setBounds(buttonsArea.removeFromLeft(buttonWidth));
        buttonsArea.removeFromLeft(buttonSpacing);
        
        largeButton.setBounds(buttonsArea.removeFromLeft(buttonWidth));
        buttonsArea.removeFromLeft(buttonSpacing);
        
        // Touch optimized button
        auto touchButtonBounds = buttonsArea.removeFromLeft(scalingManager->scaledTouchTarget(buttonWidth));
        touchOptimizedButton.setBounds(scalingManager->ensureTouchTarget(touchButtonBounds));
    }
}

void ProportionalScalingDemo::updateInfoLabels()
{
    if (!scalingManager) return;
    
    auto context = scalingManager->getCurrentContext();
    auto scale = scalingManager->getCurrentScale();
    
    // Update dimension info
    dimensionsLabel.setText(
        juce::String::formatted("Dimensions: %dx%d (Container: %dx%d)", 
                               context.currentWidth, context.currentHeight,
                               context.containerWidth, context.containerHeight),
        juce::dontSendNotification
    );
    
    // Update scale info
    scaleLabel.setText(
        juce::String::formatted("Scale: %.2f (Base: %.2f, Text: %.2f, Touch: %.2f)", 
                               context.proportionalScale, scale.baseScale, 
                               scale.textScale, scale.touchTargetScale),
        juce::dontSendNotification
    );
    
    // Update breakpoint info
    juce::String breakpoint = "Unknown";
    if (context.isExtraSmall) breakpoint = "Extra Small";
    else if (context.isSmall) breakpoint = "Small";
    else if (context.isMedium) breakpoint = "Medium";
    else if (context.isLarge) breakpoint = "Large";
    else if (context.isExtraLarge) breakpoint = "Extra Large";
    
    if (context.needsTouchOptimization) {
        breakpoint += " (Touch)";
    }
    
    breakpointLabel.setText("Breakpoint: " + breakpoint, juce::dontSendNotification);
}

void ProportionalScalingDemo::createDemoElements()
{
    demoElements.clear();
    elementColors.clear();
    
    // Create various sized elements to demonstrate proportional scaling
    demoElements.push_back(juce::Rectangle<int>(50, 50, 100, 60));   // Medium rectangle
    demoElements.push_back(juce::Rectangle<int>(200, 80, 80, 80));   // Square
    demoElements.push_back(juce::Rectangle<int>(350, 60, 120, 40));  // Wide rectangle
    demoElements.push_back(juce::Rectangle<int>(500, 50, 60, 100));  // Tall rectangle
    demoElements.push_back(juce::Rectangle<int>(600, 70, 90, 70));   // Another rectangle
    
    // Assign colors
    elementColors.push_back(juce::Colours::red.withAlpha(0.7f));
    elementColors.push_back(juce::Colours::green.withAlpha(0.7f));
    elementColors.push_back(juce::Colours::blue.withAlpha(0.7f));
    elementColors.push_back(juce::Colours::orange.withAlpha(0.7f));
    elementColors.push_back(juce::Colours::purple.withAlpha(0.7f));
}

void ProportionalScalingDemo::paintDemoElements(juce::Graphics& g)
{
    if (!scalingManager) return;
    
    // Paint proportionally scaled demo elements
    for (size_t i = 0; i < demoElements.size() && i < elementColors.size(); ++i) {
        auto scaledBounds = scalingManager->scaledProportional(demoElements[i]);
        
        // Offset to avoid overlapping with controls
        scaledBounds = scaledBounds.translated(0, getHeight() - 200);
        
        g.setColour(elementColors[i]);
        g.fillRoundedRectangle(scaledBounds.toFloat(), 
                              scalingManager->scaledProportional(5.0f));
        
        g.setColour(elementColors[i].darker());
        g.drawRoundedRectangle(scaledBounds.toFloat(), 
                              scalingManager->scaledProportional(5.0f),
                              scalingManager->scaledBorder(2.0f));
        
        // Add size label
        g.setColour(juce::Colours::white);
        g.setFont(scalingManager->scaledProportional(JUCE8_CREATE_FONT(10.0f)));
        g.drawText(juce::String::formatted("%dx%d", scaledBounds.getWidth(), scaledBounds.getHeight()),
                  scaledBounds, juce::Justification::centred);
    }
}

void ProportionalScalingDemo::paintTouchTargetVisualization(juce::Graphics& g)
{
    if (!scalingManager) return;
    
    auto context = scalingManager->getCurrentContext();
    if (!context.needsTouchOptimization) return;
    
    // Visualize minimum touch targets
    int minTouchSize = scalingManager->minTouchTarget();
    
    g.setColour(juce::Colours::yellow.withAlpha(0.3f));
    
    // Draw touch target overlays on buttons
    auto touchButtonBounds = touchOptimizedButton.getBounds();
    auto touchTarget = scalingManager->ensureTouchTarget(touchButtonBounds);
    
    g.drawRect(touchTarget, 2);
    g.setColour(juce::Colours::yellow.withAlpha(0.1f));
    g.fillRect(touchTarget);
    
    // Add label
    g.setColour(juce::Colours::yellow);
    g.setFont(scalingManager->scaledProportional(JUCE8_CREATE_FONT(8.0f)));
    g.drawText(juce::String::formatted("Min Touch: %dpx", minTouchSize),
              touchTarget.getX(), touchTarget.getBottom() + 2, 
              touchTarget.getWidth(), 15, juce::Justification::centred);
}

void ProportionalScalingDemo::paintScaleVisualization(juce::Graphics& g)
{
    if (!scalingManager) return;
    
    // Draw a reference grid to show scaling
    auto bounds = getLocalBounds();
    int gridSize = scalingManager->scaledSpacing(20);
    
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    
    // Vertical lines
    for (int x = 0; x < bounds.getWidth(); x += gridSize) {
        g.drawVerticalLine(x, 0.0f, static_cast<float>(bounds.getHeight()));
    }
    
    // Horizontal lines
    for (int y = 0; y < bounds.getHeight(); y += gridSize) {
        g.drawHorizontalLine(y, 0.0f, static_cast<float>(bounds.getWidth()));
    }
    
    // Draw scale indicator in corner
    auto scaleIndicator = bounds.removeFromBottom(50).removeFromRight(100);
    g.setColour(juce::Colours::black.withAlpha(0.7f));
    g.fillRect(scaleIndicator);
    
    g.setColour(juce::Colours::white);
    g.setFont(scalingManager->scaledProportional(JUCE8_CREATE_FONT(10.0f)));
    g.drawText(juce::String::formatted("Grid: %dpx", gridSize),
              scaleIndicator, juce::Justification::centred);
}

void ProportionalScalingDemo::sliderValueChanged(juce::Slider* slider)
{
    if (!scalingManager) return;
    
    if (slider == &scalingSlider) {
        // Manual scale override
        ProportionalScalingManager::ProportionalScale scale = scalingManager->getCurrentScale();
        scale.baseScale = static_cast<float>(slider->getValue());
        scalingManager->applyProportionalScale(scale);
        resized(); // Trigger layout update
    }
    else if (slider == &widthSlider || slider == &heightSlider) {
        // Simulate window resize
        int newWidth = static_cast<int>(widthSlider.getValue());
        int newHeight = static_cast<int>(heightSlider.getValue());
        setSize(newWidth, newHeight);
    }
}

void ProportionalScalingDemo::buttonClicked(juce::Button* button)
{
    if (!scalingManager) return;
    
    if (button == &smallButton) {
        setSize(400, 300);
    }
    else if (button == &mediumButton) {
        setSize(800, 600);
    }
    else if (button == &largeButton) {
        setSize(1200, 900);
    }
    else if (button == &touchOptimizedButton) {
        // Toggle touch optimization demo
        auto config = scalingManager->getReferenceConfig();
        config.minTouchTargetPx = (config.minTouchTargetPx == 44) ? 60 : 44;
        scalingManager->setReferenceConfig(config);
        resized();
    }
}

void ProportionalScalingDemo::onProportionalScaleChanged(const ProportionalScalingManager::ProportionalScale& scale)
{
    // Update UI in response to scale changes
    updateInfoLabels();
    repaint();
    
    // Update slider to reflect current scale
    scalingSlider.setValue(scale.baseScale, juce::dontSendNotification);
}

void ProportionalScalingDemo::onDimensionContextChanged(const ProportionalScalingManager::DimensionContext& context)
{
    // Update sliders to reflect current dimensions
    widthSlider.setValue(context.currentWidth, juce::dontSendNotification);
    heightSlider.setValue(context.currentHeight, juce::dontSendNotification);
    
    updateInfoLabels();
}

void ProportionalScalingDemo::onBreakpointChanged(int width, int height)
{
    // Trigger layout recalculation when breakpoints change
    updateProportionalLayout();
    repaint();
    
    DBG("Breakpoint changed for dimensions: " << width << "x" << height);
}

//==============================================================================
// ProportionalTouchControl Implementation
//==============================================================================

ProportionalTouchControl::ProportionalTouchControl(const juce::String& name, ProportionalScalingManager* manager)
    : scalingManager(manager), controlName(name)
{
    setInterceptsMouseClicks(true, false);
}

void ProportionalTouchControl::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto visualBounds = getVisualBounds();
    auto touchBounds = getTouchTarget();
    
    // Draw touch target area if touch optimized
    if (touchOptimized && scalingManager) {
        g.setColour(juce::Colours::yellow.withAlpha(0.2f));
        g.fillRect(touchBounds);
        g.setColour(juce::Colours::yellow.withAlpha(0.5f));
        g.drawRect(touchBounds, 1);
    }
    
    // Draw visual control
    juce::Colour baseColor = isPressed ? juce::Colours::lightblue : 
                            (isHovered ? juce::Colours::lightgrey : juce::Colours::grey);
    
    g.setColour(baseColor);
    g.fillRoundedRectangle(visualBounds.toFloat(), 4.0f);
    
    g.setColour(baseColor.darker());
    g.drawRoundedRectangle(visualBounds.toFloat(), 4.0f, 1.0f);
    
    // Draw value indicator
    int indicatorWidth = static_cast<int>(visualBounds.getWidth() * currentValue);
    auto indicatorBounds = visualBounds.removeFromLeft(indicatorWidth);
    
    g.setColour(juce::Colours::blue);
    g.fillRoundedRectangle(indicatorBounds.toFloat(), 4.0f);
    
    // Draw label
    g.setColour(juce::Colours::white);
    juce::Font font = scalingManager ? scalingManager->scaledProportional(JUCE8_CREATE_FONT(12.0f)) : JUCE8_CREATE_FONT(12.0f);
    g.setFont(font);
    g.drawText(controlName, visualBounds, juce::Justification::centred);
}

void ProportionalTouchControl::resized()
{
    // Nothing special needed - bounds are handled by parent
}

void ProportionalTouchControl::mouseDown(const juce::MouseEvent& event)
{
    isPressed = true;
    updateValueFromMouse(event);
    repaint();
}

void ProportionalTouchControl::mouseUp(const juce::MouseEvent& event)
{
    isPressed = false;
    repaint();
}

void ProportionalTouchControl::mouseEnter(const juce::MouseEvent& event)
{
    isHovered = true;
    repaint();
}

void ProportionalTouchControl::mouseExit(const juce::MouseEvent& event)
{
    isHovered = false;
    repaint();
}

void ProportionalTouchControl::setValue(float newValue)
{
    currentValue = juce::jlimit(0.0f, 1.0f, newValue);
    repaint();
}

void ProportionalTouchControl::updateValueFromMouse(const juce::MouseEvent& event)
{
    auto visualBounds = getVisualBounds();
    float newValue = static_cast<float>(event.x - visualBounds.getX()) / visualBounds.getWidth();
    setValue(newValue);
    
    if (onValueChanged) {
        onValueChanged(currentValue);
    }
}

juce::Rectangle<int> ProportionalTouchControl::getTouchTarget() const
{
    if (!scalingManager || !touchOptimized) {
        return getLocalBounds();
    }
    
    return scalingManager->ensureTouchTarget(getLocalBounds());
}

juce::Rectangle<int> ProportionalTouchControl::getVisualBounds() const
{
    return getLocalBounds();
}

//==============================================================================
// ProportionalGridLayout Implementation
//==============================================================================

ProportionalGridLayout::ProportionalGridLayout(ProportionalScalingManager* manager)
    : scalingManager(manager)
{
}

void ProportionalGridLayout::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Draw grid background
    g.setColour(juce::Colours::darkgrey.withAlpha(0.3f));
    g.fillAll();
    
    // Draw grid lines
    g.setColour(juce::Colours::white.withAlpha(0.2f));
    
    int cellWidth = bounds.getWidth() / gridColumns;
    int cellHeight = bounds.getHeight() / gridRows;
    
    // Vertical lines
    for (int col = 1; col < gridColumns; ++col) {
        int x = col * cellWidth;
        g.drawVerticalLine(x, 0.0f, static_cast<float>(bounds.getHeight()));
    }
    
    // Horizontal lines
    for (int row = 1; row < gridRows; ++row) {
        int y = row * cellHeight;
        g.drawHorizontalLine(y, 0.0f, static_cast<float>(bounds.getWidth()));
    }
    
    // Draw border
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.drawRect(bounds, scalingManager ? scalingManager->scaledBorder(1) : 1);
}

void ProportionalGridLayout::resized()
{
    if (autoResize) {
        determineOptimalGridSize();
    }
    
    updateGridLayout();
}

void ProportionalGridLayout::setGridSize(int columns, int rows)
{
    gridColumns = columns;
    gridRows = rows;
    updateGridLayout();
}

void ProportionalGridLayout::addGridItem(std::unique_ptr<juce::Component> component)
{
    addAndMakeVisible(component.get());
    gridItems.add(component.release());
    updateGridLayout();
}

void ProportionalGridLayout::clearGridItems()
{
    gridItems.clear();
    updateGridLayout();
}

void ProportionalGridLayout::updateGridLayout()
{
    int itemIndex = 0;
    
    for (int row = 0; row < gridRows && itemIndex < gridItems.size(); ++row) {
        for (int col = 0; col < gridColumns && itemIndex < gridItems.size(); ++col) {
            auto cellBounds = getGridCellBounds(col, row);
            
            // Add margin
            int margin = scalingManager ? scalingManager->scaledSpacing(5) : 5;
            cellBounds = cellBounds.reduced(margin);
            
            gridItems[itemIndex]->setBounds(cellBounds);
            ++itemIndex;
        }
    }
}

juce::Rectangle<int> ProportionalGridLayout::getGridCellBounds(int column, int row) const
{
    auto bounds = getLocalBounds();
    
    int cellWidth = bounds.getWidth() / gridColumns;
    int cellHeight = bounds.getHeight() / gridRows;
    
    return juce::Rectangle<int>(column * cellWidth, row * cellHeight, cellWidth, cellHeight);
}

void ProportionalGridLayout::determineOptimalGridSize()
{
    if (!scalingManager) return;
    
    auto context = scalingManager->getCurrentContext();
    
    if (context.isExtraSmall) {
        setGridSize(1, 3);
    } else if (context.isSmall) {
        setGridSize(2, 2);
    } else if (context.isMedium) {
        setGridSize(3, 2);
    } else {
        setGridSize(4, 2);
    }
}

//==============================================================================
// ProportionalTextDemo Implementation
//==============================================================================

ProportionalTextDemo::ProportionalTextDemo(ProportionalScalingManager* manager)
    : scalingManager(manager)
{
    initializeTextSamples();
}

void ProportionalTextDemo::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Background
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.fillAll();
    
    // Paint text samples
    int sampleHeight = bounds.getHeight() / static_cast<int>(textSamples.size());
    
    for (size_t i = 0; i < textSamples.size(); ++i) {
        auto sampleBounds = bounds.removeFromTop(sampleHeight);
        paintTextSample(g, textSamples[i], sampleBounds);
    }
}

void ProportionalTextDemo::resized()
{
    repaint();
}

void ProportionalTextDemo::initializeTextSamples()
{
    textSamples.clear();
    
    textSamples.push_back({
        "Large Heading Text (32pt base)",
        32,
        juce::Font::bold,
        juce::Justification::centredLeft,
        juce::Colours::white
    });
    
    textSamples.push_back({
        "Medium Subtitle Text (18pt base)",
        18,
        juce::Font::plain,
        juce::Justification::centredLeft,
        juce::Colours::lightgrey
    });
    
    textSamples.push_back({
        "Regular Body Text (14pt base) - Proportionally scaled for readability",
        14,
        juce::Font::plain,
        juce::Justification::centredLeft,
        juce::Colours::white
    });
    
    textSamples.push_back({
        "Small Caption Text (10pt base)",
        10,
        juce::Font::italic,
        juce::Justification::centredLeft,
        juce::Colours::yellow
    });
    
    textSamples.push_back({
        "Tiny Label Text (8pt base) - Minimum readable size enforced",
        8,
        juce::Font::plain,
        juce::Justification::centredLeft,
        juce::Colours::cyan
    });
}

void ProportionalTextDemo::paintTextSample(juce::Graphics& g, const TextSample& sample, juce::Rectangle<int> bounds)
{
    if (!scalingManager) return;
    
    // Create proportionally scaled font
    juce::Font font = JUCE8_CREATE_FONT(static_cast<float>(sample.baseFontSize), sample.style);
    font = scalingManager->scaledProportional(font);
    
    g.setFont(font);
    g.setColour(sample.color);
    
    // Add size information to text
    juce::String displayText = sample.text + juce::String::formatted(" [Scaled: %.1fpt]", font.getHeight());
    
    g.drawText(displayText, bounds, sample.justification);
}

//==============================================================================
// ProportionalPluginDemo Implementation
//==============================================================================

ProportionalPluginDemo::ProportionalPluginDemo(ProportionalScalingManager* manager)
    : scalingManager(manager)
{
    initializePluginUI();
}

void ProportionalPluginDemo::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Plugin-style background
    g.setColour(juce::Colours::darkslategrey);
    g.fillAll();
    
    // Draw host constraint indicator if present
    if (hasHostConstraints) {
        g.setColour(juce::Colours::red.withAlpha(0.3f));
        g.drawRect(bounds, 3);
        
        g.setColour(juce::Colours::red);
        g.setFont(scalingManager ? scalingManager->scaledProportional(JUCE8_CREATE_FONT(10.0f)) : JUCE8_CREATE_FONT(10.0f));
        g.drawText(juce::String::formatted("Host Constraint: %dx%d", maxHostWidth, maxHostHeight),
                  bounds.getX() + 5, bounds.getY() + 5, 200, 15, juce::Justification::topLeft);
    }
}

void ProportionalPluginDemo::resized()
{
    if (scalingManager) {
        scalingManager->updateDimensions(getWidth(), getHeight());
    }
    
    layoutPluginComponents();
    adaptToHostSize();
}

void ProportionalPluginDemo::simulateHostConstraints(int maxWidth, int maxHeight, bool resizable)
{
    hasHostConstraints = true;
    maxHostWidth = maxWidth;
    maxHostHeight = maxHeight;
    hostAllowsResize = resizable;
    
    if (scalingManager) {
        scalingManager->setPluginHostConstraints(maxWidth, maxHeight, resizable);
    }
    
    updateForHostEnvironment();
}

void ProportionalPluginDemo::removeHostConstraints()
{
    hasHostConstraints = false;
    maxHostWidth = 0;
    maxHostHeight = 0;
    hostAllowsResize = true;
    
    resized();
}

void ProportionalPluginDemo::updateForHostEnvironment()
{
    if (scalingManager) {
        scalingManager->adaptToHostEnvironment();
    }
    
    resized();
    repaint();
}

void ProportionalPluginDemo::initializePluginUI()
{
    // Plugin title
    pluginTitleLabel.setText("OTTO Plugin (Proportional)", juce::dontSendNotification);
    pluginTitleLabel.setFont(JUCE8_CREATE_FONT(16.0f, juce::Font::bold));
    pluginTitleLabel.setJustificationType(juce::Justification::centred);
    pluginTitleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(pluginTitleLabel);
    
    // Main controls
    mainVolumeSlider.setRange(0.0, 1.0);
    mainVolumeSlider.setValue(0.7);
    mainVolumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mainVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(mainVolumeSlider);
    
    filterSlider.setRange(20.0, 20000.0);
    filterSlider.setValue(1000.0);
    filterSlider.setSkewFactorFromMidPoint(500.0);
    filterSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(filterSlider);
    
    reverbSlider.setRange(0.0, 1.0);
    reverbSlider.setValue(0.3);
    reverbSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(reverbSlider);
    
    // Buttons
    presetButton.setButtonText("Presets");
    addAndMakeVisible(presetButton);
    
    settingsButton.setButtonText("Settings");
    addAndMakeVisible(settingsButton);
}

void ProportionalPluginDemo::layoutPluginComponents()
{
    if (!scalingManager) return;
    
    auto bounds = getLocalBounds();
    int margin = scalingManager->scaledSpacing(10);
    int titleHeight = scalingManager->scaledText(24);
    int sliderSize = scalingManager->scaledElement(80);
    int buttonHeight = scalingManager->scaledElement(25);
    
    bounds.reduce(margin, margin);
    
    // Title
    pluginTitleLabel.setBounds(bounds.removeFromTop(titleHeight));
    pluginTitleLabel.setFont(scalingManager->scaledProportional(JUCE8_CREATE_FONT(16.0f, juce::Font::bold)));
    
    bounds.removeFromTop(margin);
    
    // Sliders area
    auto slidersArea = bounds.removeFromTop(sliderSize + 40);
    
    if (scalingManager->shouldUseCompactLayout()) {
        // Vertical layout for compact view
        int sliderWidth = slidersArea.getWidth();
        int sliderHeight = (slidersArea.getHeight() - margin * 2) / 3;
        
        mainVolumeSlider.setBounds(slidersArea.removeFromTop(sliderHeight));
        slidersArea.removeFromTop(margin / 2);
        filterSlider.setBounds(slidersArea.removeFromTop(sliderHeight));
        slidersArea.removeFromTop(margin / 2);
        reverbSlider.setBounds(slidersArea.removeFromTop(sliderHeight));
    } else {
        // Horizontal layout
        int sliderWidth = (slidersArea.getWidth() - margin * 2) / 3;
        
        mainVolumeSlider.setBounds(slidersArea.removeFromLeft(sliderWidth));
        slidersArea.removeFromLeft(margin);
        filterSlider.setBounds(slidersArea.removeFromLeft(sliderWidth));
        slidersArea.removeFromLeft(margin);
        reverbSlider.setBounds(slidersArea);
    }
    
    bounds.removeFromTop(margin);
    
    // Buttons
    auto buttonsArea = bounds.removeFromTop(buttonHeight);
    int buttonWidth = (buttonsArea.getWidth() - margin) / 2;
    
    presetButton.setBounds(buttonsArea.removeFromLeft(buttonWidth));
    buttonsArea.removeFromLeft(margin);
    settingsButton.setBounds(buttonsArea);
}

void ProportionalPluginDemo::adaptToHostSize()
{
    if (!hasHostConstraints) return;
    
    // Ensure we don't exceed host constraints
    auto currentBounds = getBounds();
    if (currentBounds.getWidth() > maxHostWidth || currentBounds.getHeight() > maxHostHeight) {
        setSize(juce::jmin(currentBounds.getWidth(), maxHostWidth),
               juce::jmin(currentBounds.getHeight(), maxHostHeight));
    }
}

//==============================================================================
// ProportionalPlatformDemo Implementation
//==============================================================================

ProportionalPlatformDemo::ProportionalPlatformDemo(ProportionalScalingManager* manager)
    : scalingManager(manager)
{
    initializePlatformSelector();
}

void ProportionalPlatformDemo::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Platform-specific background
    g.setColour(juce::Colours::midnightblue);
    g.fillAll();
    
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawRect(bounds, 1);
}

void ProportionalPlatformDemo::resized()
{
    if (!scalingManager) return;
    
    auto bounds = getLocalBounds();
    int margin = scalingManager->scaledSpacing(10);
    int controlHeight = scalingManager->scaledElement(25);
    
    bounds.reduce(margin, margin);
    
    // Platform selector
    auto selectorArea = bounds.removeFromTop(controlHeight * 2);
    platformLabel.setBounds(selectorArea.removeFromTop(controlHeight));
    platformLabel.setFont(scalingManager->scaledProportional(JUCE8_CREATE_FONT(12.0f)));
    
    platformSelector.setBounds(selectorArea);
    
    bounds.removeFromTop(margin);
    
    // Update layout based on current platform simulation
    auto selectedPlatform = platformSelector.getText();
    if (selectedPlatform.contains("Desktop")) {
        layoutForDesktop();
    } else if (selectedPlatform.contains("Mobile")) {
        layoutForMobile();
    } else if (selectedPlatform.contains("Plugin")) {
        layoutForPlugin();
    }
}

void ProportionalPlatformDemo::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &platformSelector) {
        simulatePlatform(comboBox->getText());
        resized();
    }
}

void ProportionalPlatformDemo::initializePlatformSelector()
{
    platformLabel.setText("Platform Simulation:", juce::dontSendNotification);
    platformLabel.setFont(JUCE8_CREATE_FONT(12.0f));
    platformLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(platformLabel);
    
    platformSelector.addItem("macOS Desktop", 1);
    platformSelector.addItem("Windows Desktop", 2);
    platformSelector.addItem("Linux Desktop", 3);
    platformSelector.addItem("iOS Mobile", 4);
    platformSelector.addItem("Android Mobile", 5);
    platformSelector.addItem("VST3 Plugin", 6);
    platformSelector.addItem("AUv3 Plugin", 7);
    
    platformSelector.setSelectedId(1);
    platformSelector.addListener(this);
    addAndMakeVisible(platformSelector);
}

void ProportionalPlatformDemo::simulatePlatform(const juce::String& platformName)
{
    // Simulate different platform characteristics
    if (platformName.contains("iOS") || platformName.contains("Android")) {
        // Mobile platforms - enable touch optimization
        if (scalingManager) {
            auto config = scalingManager->getReferenceConfig();
            config.minTouchTargetPx = 44;
            config.minTouchTargetDp = 48;
            scalingManager->setReferenceConfig(config);
        }
    } else {
        // Desktop platforms - disable touch optimization
        if (scalingManager) {
            auto config = scalingManager->getReferenceConfig();
            config.minTouchTargetPx = 0;
            config.minTouchTargetDp = 0;
            scalingManager->setReferenceConfig(config);
        }
    }
    
    repaint();
}

void ProportionalPlatformDemo::layoutForDesktop()
{
    // Desktop-optimized layout with smaller controls
}

void ProportionalPlatformDemo::layoutForMobile()
{
    // Mobile-optimized layout with larger touch targets
}

void ProportionalPlatformDemo::layoutForPlugin()
{
    // Plugin-optimized layout with compact design
}

void ProportionalPlatformDemo::updatePlatformSpecificElements()
{
    // Update UI elements based on platform
}