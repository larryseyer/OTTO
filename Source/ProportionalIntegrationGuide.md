# OTTO Proportional Scaling Integration Guide

## Overview

This guide explains how to integrate the new proportional scaling system into your existing OTTO JUCE 8 project. The system provides:

- **Proportional scaling based on app window dimensions**
- **Automatic DPI scaling and screen density detection**
- **Touch input optimization with proportionally scaled touch targets**
- **Responsive breakpoint system with dimension-based scaling**
- **Plugin-specific proportional scaling within host constraints**
- **Cross-platform compatibility** (macOS, Windows, Linux, iOS, Android)
- **Multi-format support** (Standalone, VST3, AUv3, CLAP AU)

## Quick Start

### 1. Basic Integration (Minimal Changes)

Replace your existing editor creation in `PluginProcessor.cpp`:

```cpp
// OLD:
juce::AudioProcessorEditor* OTTOAudioProcessor::createEditor()
{
    return new OTTOAudioProcessorEditor(*this);
}

// NEW:
juce::AudioProcessorEditor* OTTOAudioProcessor::createEditor()
{
    return OTTO_INIT_PROPORTIONAL(*this).release();
}
```

### 2. Enhanced Integration (Full Features)

For complete control over proportional scaling:

```cpp
juce::AudioProcessorEditor* OTTOAudioProcessor::createEditor()
{
    // Create proportional editor with specific configuration
    auto editor = std::make_unique<ProportionalPluginEditor>(*this);
    
    // Enable debug visualization in development builds
    #if JUCE_DEBUG
    editor->enableDebugVisualization(true);
    editor->showProportionalScalingDemo(true);
    #endif
    
    // Adapt for current platform
    editor->adaptForCurrentPlatform();
    
    return editor.release();
}
```

### 3. Platform-Specific Editors

Create editors optimized for specific platforms:

```cpp
juce::AudioProcessorEditor* OTTOAudioProcessor::createEditor()
{
    #if JUCE_IOS
    return OTTO_CREATE_FOR_PLATFORM(*this, PlatformResponsiveManager::TargetPlatform::iOS_Standalone).release();
    #elif JUCE_ANDROID
    return OTTO_CREATE_FOR_PLATFORM(*this, PlatformResponsiveManager::TargetPlatform::Android_Standalone).release();
    #elif JUCE_MAC
    return OTTO_CREATE_FOR_PLATFORM(*this, detectedPlatform()).release();
    #else
    return OTTO_INIT_PROPORTIONAL(*this).release();
    #endif
}

private:
    PlatformResponsiveManager::TargetPlatform detectedPlatform() const
    {
        // Detect if running as plugin or standalone
        if (wrapperType == wrapperType_VST3)
            return PlatformResponsiveManager::TargetPlatform::macOS_VST3;
        else if (wrapperType == wrapperType_AudioUnit)
            return PlatformResponsiveManager::TargetPlatform::macOS_AU;
        else if (wrapperType == wrapperType_CLAP)
            return PlatformResponsiveManager::TargetPlatform::macOS_CLAP;
        else
            return PlatformResponsiveManager::TargetPlatform::macOS_Standalone;
    }
```

## Component Enhancement

### Enhancing Existing Components

The system can enhance your existing OTTO components with proportional scaling:

```cpp
// In your component initialization:
void MyOTTOComponent::initialise()
{
    // Original component setup...
    
    // Enhance with proportional scaling
    OTTO_ENHANCE_COMPONENT(this, "MyComponentType");
    
    // Convert existing layouts
    OTTO_CONVERT_LAYOUT(this);
}
```

### Using Proportional Scaling in Custom Components

```cpp
void MyCustomComponent::paint(juce::Graphics& g)
{
    // Use proportional scaling macros
    auto bounds = getLocalBounds();
    int margin = PROP_SPACING(10);
    int buttonHeight = PROP_ELEMENT(30);
    
    bounds.reduce(margin, margin);
    
    // Draw with proportional dimensions
    g.setFont(OTTO_PROPORTIONAL_FONT(juce::Font(14.0f)));
    g.fillRoundedRectangle(bounds.toFloat(), PROP_SPACING(5));
}

void MyCustomComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Layout with proportional scaling
    myButton.setBounds(OTTO_PROPORTIONAL_BOUNDS(juce::Rectangle<int>(10, 10, 100, 30)));
    
    // Or use the scaling manager directly
    auto scalingManager = ProportionalScaling::getInstance();
    mySlider.setBounds(scalingManager.scaledProportional(juce::Rectangle<int>(10, 50, 200, 25)));
}
```

## Configuration Options

### Reference Configuration

Customize the base reference dimensions for your interface:

```cpp
void setupProportionalScaling()
{
    ProportionalScalingManager::ReferenceConfig config;
    config.baseWidth = 1000;                    // Your interface design width
    config.baseHeight = 750;                    // Your interface design height
    config.aspectRatio = 1000.0f / 750.0f;     // Maintain aspect ratio
    config.minScaleFactor = 0.3f;               // Minimum scale (30%)
    config.maxScaleFactor = 3.0f;               // Maximum scale (300%)
    
    // Touch optimization
    config.minTouchTargetPx = 44;               // iOS minimum touch target
    config.minTouchTargetDp = 48;               // Android minimum touch target
    
    // Breakpoints based on your design
    config.extraSmallThreshold = 320;          // Phone portrait
    config.smallThreshold = 480;               // Phone landscape
    config.mediumThreshold = 768;              // Tablet portrait
    config.largeThreshold = 1024;              // Desktop/tablet landscape
    
    auto& scalingManager = ProportionalScaling::getInstance();
    scalingManager.setReferenceConfig(config);
}
```

### Plugin Host Integration

Handle plugin host constraints automatically:

```cpp
void MyPluginEditor::hostResized(int newWidth, int newHeight)
{
    // The proportional scaling system handles this automatically
    // but you can also set explicit constraints:
    
    if (auto* scalingManager = getProportionalManager()) {
        scalingManager->setPluginHostConstraints(newWidth, newHeight, true);
        scalingManager->adaptToHostEnvironment();
    }
}
```

## Responsive Breakpoints

Use responsive breakpoints to adapt your interface:

```cpp
void MyComponent::layoutForCurrentScreen()
{
    if (IS_SMALL_SCREEN()) {
        // Compact vertical layout
        layoutCompactVertical();
    } else if (IS_MEDIUM_SCREEN()) {
        // Standard layout
        layoutStandard();
    } else if (IS_LARGE_SCREEN()) {
        // Expanded layout with more features
        layoutExpanded();
    }
    
    // Touch optimization
    if (NEEDS_COMPACT_LAYOUT()) {
        enableTouchFriendlyControls();
    }
}
```

## Migration from Existing OTTO Editor

### Gradual Migration

You can gradually migrate your existing editor:

```cpp
// In your existing OTTOAudioProcessorEditor constructor:
OTTOAudioProcessorEditor::OTTOAudioProcessorEditor(OTTOAudioProcessor& p)
    : AudioProcessorEditor(p), processor(p)
{
    // Initialize OTTO proportional scaling
    OTTO_PROPORTIONAL_INIT(this);
    
    // Your existing initialization code...
    setupComponents();
    
    // Enhance existing components
    for (int i = 0; i < getNumChildComponents(); ++i) {
        auto* child = getChildComponent(i);
        OTTO_ENHANCE_COMPONENT(child, "AutoDetect");
    }
}
```

### Full Migration

For complete migration, replace your editor class:

```cpp
// Replace OTTOAudioProcessorEditor with ProportionalPluginEditor
class MyProportionalOTTOEditor : public ProportionalPluginEditor
{
public:
    MyProportionalOTTOEditor(OTTOAudioProcessor& processor)
        : ProportionalPluginEditor(processor)
    {
        // Your custom initialization
        setupCustomFeatures();
    }
    
private:
    void setupCustomFeatures()
    {
        // Add any OTTO-specific customizations
    }
};
```

## Platform-Specific Adaptations

### iOS/Mobile Adaptations

```cpp
void MyComponent::adaptForMobile()
{
    if (auto* scalingManager = ProportionalScaling::getGlobalScalingManager()) {
        // Enable touch optimization
        auto config = scalingManager->getReferenceConfig();
        config.minTouchTargetPx = 44;
        config.minTouchTargetDp = 48;
        scalingManager->setReferenceConfig(config);
        
        // Use larger fonts for mobile
        if (scalingManager->getCurrentContext().needsTouchOptimization) {
            setFont(scalingManager->scaledText(16)); // Larger base font
        }
    }
}
```

### Plugin Format Adaptations

```cpp
void MyEditor::adaptForPluginFormat()
{
    // VST3 typically has more constrained sizing
    if (processor.wrapperType == juce::AudioProcessor::wrapperType_VST3) {
        setPluginHostConstraints(800, 600, true);
    }
    // AU plugins might have different constraints
    else if (processor.wrapperType == juce::AudioProcessor::wrapperType_AudioUnit) {
        setPluginHostConstraints(1024, 768, true);
    }
    // Standalone has full freedom
    else {
        // No constraints for standalone
    }
}
```

## Debug and Development

### Enable Debug Visualization

```cpp
#if JUCE_DEBUG
void MyEditor::enableDebugMode()
{
    // Enable debug visualization
    OTTO_ENABLE_DEBUG(this);
    
    // Show proportional scaling demo
    OTTO_SHOW_DEMO(this);
    
    // Print scaling report
    OTTOProportionalIntegration::printScalingReport();
}
#endif
```

### Performance Monitoring

```cpp
void MyEditor::monitorPerformance()
{
    // Get integration status
    auto status = OTTOProportionalIntegration::getIntegrationStatus();
    DBG("Proportional Scaling Status: " << status);
    
    // Check for errors
    if (auto* manager = OTTOProportionalIntegration::getGlobalScalingManager()) {
        if (manager->hasError()) {
            DBG("Scaling Error: " << manager->getLastError());
        }
    }
}
```

## Best Practices

### 1. Use Proportional Macros

Instead of hard-coded pixel values:

```cpp
// BAD:
button.setBounds(10, 10, 100, 30);
label.setFont(juce::Font(14.0f));

// GOOD:
button.setBounds(PROP_SPACING(10), PROP_SPACING(10), PROP_ELEMENT(100), PROP_ELEMENT(30));
label.setFont(OTTO_PROPORTIONAL_FONT(juce::Font(14.0f)));

// BEST:
auto bounds = OTTO_PROPORTIONAL_BOUNDS(juce::Rectangle<int>(10, 10, 100, 30));
button.setBounds(bounds);
```

### 2. Design for Multiple Screen Sizes

```cpp
void MyComponent::designResponsively()
{
    // Design for your base reference size (e.g., 1000x750)
    // The system will scale proportionally for other sizes
    
    // Use relative positioning when possible
    auto bounds = getLocalBounds();
    auto centerArea = bounds.reduced(PROP_SPACING(20));
    
    // Use proportional divisions
    auto leftPanel = centerArea.removeFromLeft(centerArea.getWidth() / 3);
    auto rightPanel = centerArea; // Remaining area
}
```

### 3. Test Across Platforms

```cpp
void MyComponent::testScaling()
{
    #if JUCE_DEBUG
    // Test different scale factors
    std::vector<float> testScales = {0.5f, 0.75f, 1.0f, 1.5f, 2.0f, 3.0f};
    
    for (auto scale : testScales) {
        // Simulate different scales
        if (auto* manager = ProportionalScaling::getGlobalScalingManager()) {
            auto currentScale = manager->getCurrentScale();
            currentScale.baseScale = scale;
            manager->applyProportionalScale(currentScale);
            
            // Verify layout still works
            resized();
        }
    }
    #endif
}
```

## Troubleshooting

### Common Issues

1. **Components not scaling**: Make sure you're using the proportional macros or scaling manager
2. **Text too small**: Check if minimum font size constraints are being applied
3. **Touch targets too small**: Enable touch optimization for mobile platforms
4. **Layout breaks on resize**: Use proportional positioning instead of fixed coordinates
5. **Performance issues**: Reduce update frequency or disable debug visualization

### Debug Information

```cpp
void debugScalingIssues()
{
    auto* manager = OTTOProportionalIntegration::getGlobalScalingManager();
    if (manager) {
        // Print current state
        manager->debugPrintProportionalState();
        
        // Check configuration
        if (!manager->validateConfiguration()) {
            DBG("Invalid proportional scaling configuration!");
        }
        
        // Check for errors
        if (manager->hasError()) {
            DBG("Scaling error: " << manager->getLastError());
        }
    }
}
```

## Advanced Usage

### Custom Scaling Behaviors

```cpp
class MyCustomScalingBehavior
{
public:
    static int scaleForAudioParameter(int baseSize, float parameterValue)
    {
        auto* manager = ProportionalScaling::getGlobalScalingManager();
        if (manager) {
            int scaledSize = manager->scaledProportional(baseSize);
            // Apply parameter-based scaling
            return static_cast<int>(scaledSize * (0.8f + parameterValue * 0.4f));
        }
        return baseSize;
    }
};
```

### Integration with JUCE Animation

```cpp
void MyComponent::animateWithProportionalScaling()
{
    auto targetBounds = OTTO_PROPORTIONAL_BOUNDS(juce::Rectangle<int>(50, 50, 200, 100));
    
    // Animate to proportionally scaled bounds
    juce::Desktop::getInstance().getAnimator().animateComponent(
        this, targetBounds, 1.0f, 300, false, 0.0, 0.0);
}
```

This integration guide provides comprehensive coverage of how to implement proportional scaling in your OTTO project while maintaining compatibility with your existing codebase.