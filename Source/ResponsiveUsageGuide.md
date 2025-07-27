# OTTO Responsive Design System - Usage Guide

## Overview

The OTTO Responsive Design System provides a comprehensive, cross-platform solution for creating adaptive user interfaces that work consistently across all OTTO's target platforms:

- **macOS**: Standalone, VST3, AU, and CLAP formats
- **Windows**: Standalone, VST3, and CLAP formats  
- **Linux**: Standalone, VST3, and CLAP formats
- **iOS**: Standalone and AUv3 formats
- **Android**: Standalone format

## Key Components

### 1. PlatformResponsiveManager
The core system that handles platform detection, DPI scaling, and responsive layout calculations.

### 2. PlatformAwareLookAndFeel
An enhanced LookAndFeel that adapts visual styling to each platform's design conventions.

### 3. ResponsiveIntegrationComponent
A demonstration component showing best practices for responsive design.

## Quick Start

### Basic Integration

```cpp
// In your component constructor
#include "PlatformResponsiveManager.h"
#include "PlatformAwareLookAndFeel.h"

class MyComponent : public juce::Component
{
private:
    std::unique_ptr<PlatformResponsiveManager> responsiveManager;
    std::unique_ptr<PlatformAwareLookAndFeel> lookAndFeel;

public:
    MyComponent(FontManager& fontManager, ColorScheme& colorScheme)
    {
        // Initialize responsive system
        responsiveManager = std::make_unique<PlatformResponsiveManager>();
        lookAndFeel = PlatformLookAndFeelFactory::createForCurrentPlatform(
            fontManager, colorScheme, *responsiveManager);
        
        setLookAndFeel(lookAndFeel.get());
        
        // Register for updates
        responsiveManager->onScaleChanged = [this](const auto& scale) {
            resized();
            repaint();
        };
    }
    
    void resized() override
    {
        // Update responsive manager with new size
        responsiveManager->updateLayout(getWidth(), getHeight());
        
        // Use scaled dimensions
        auto margin = responsiveManager->scaled(10);
        auto buttonHeight = responsiveManager->touchTarget(32);
        
        // Layout components...
    }
};
```

### Application-Level Setup

```cpp
// In your main application
#include "ResponsiveMainDemo.h"

class OTTOApplication : public juce::JUCEApplication
{
public:
    void initialise(const juce::String& commandLine) override
    {
        // Initialize global responsive system
        ResponsiveApplicationSetup::ResponsiveConfig config;
        config.enableTouchOptimization = true;
        config.enableAnimations = true;
        ResponsiveApplicationSetup::initializeResponsiveSystem(config);
        
        // Create main window
        mainWindow = std::make_unique<MainWindow>("OTTO");
    }
    
private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(const juce::String& name) : DocumentWindow(name, juce::Colours::darkgrey, 
                                                             DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            
            // Setup responsive window
            ResponsiveApplicationSetup::setupWindowForPlatform(*this);
            
            // Set content component
            auto content = std::make_unique<ResponsiveMainDemo>(fontManager, colorScheme);
            setContentOwned(content.release(), true);
            
            setVisible(true);
        }
    };
};
```

## Platform-Specific Features

### Touch Optimization

The system automatically detects touch input and adapts accordingly:

```cpp
// Check if touch input is available
if (responsiveManager->getPlatformConfig().hasTouchInput)
{
    // Use larger touch targets
    auto touchSize = responsiveManager->touchTarget(32); // Minimum 44pt on iOS, 48dp on Android
    button->setSize(touchSize, touchSize);
}

// Apply touch-optimized padding
auto padding = responsiveManager->getTouchTargetPadding();
component->setBounds(area.reduced(padding));
```

### Responsive Breakpoints

Layout adapts to different screen sizes:

```cpp
void MyComponent::resized()
{
    responsiveManager->updateLayout(getWidth(), getHeight());
    
    if (responsiveManager->isExtraSmall())
    {
        // Phone layout - vertical stack
        layoutVertically();
    }
    else if (responsiveManager->isSmall())
    {
        // Small tablet layout
        layoutCompact();
    }
    else
    {
        // Desktop layout - full feature set
        layoutFull();
    }
}
```

### Platform-Specific Styling

```cpp
// Colors adapt to platform conventions
auto backgroundColor = responsiveManager->adaptColorForPlatform(baseColor);
g.fillAll(backgroundColor);

// Visual effects based on platform
if (responsiveManager->shouldUseShadows())
{
    // Draw drop shadows (iOS, Android)
    drawShadow(g, bounds);
}

if (responsiveManager->shouldUseGradients())
{
    // Use gradients (macOS, iOS)
    drawGradient(g, bounds);
}
else
{
    // Use flat design (Windows, Linux, Android)
    drawFlat(g, bounds);
}
```

## Responsive Font Scaling

```cpp
void MyComponent::paint(juce::Graphics& g)
{
    // Fonts automatically scale with DPI and user preferences
    auto titleFont = fontManager.getFont(FontManager::FontType::Header);
    auto scaledFont = responsiveManager->scaled(titleFont);
    
    g.setFont(scaledFont);
    g.drawText("Title", titleArea, juce::Justification::centred);
}
```

## Layout Patterns

### Adaptive Grid Layout

```cpp
void MyComponent::calculateGridLayout()
{
    auto area = getLocalBounds();
    int columns = 4; // Default for desktop
    
    if (responsiveManager->isSmall())
        columns = 2; // Tablet
    else if (responsiveManager->isExtraSmall())
        columns = 1; // Phone
    
    auto itemWidth = area.getWidth() / columns;
    auto itemHeight = responsiveManager->touchTarget(60);
    
    // Layout grid items...
}
```

### Collapsible Panels

```cpp
class ResponsivePanel : public juce::Component
{
public:
    void resized() override
    {
        bool shouldCollapse = responsiveManager->shouldUseCompactLayout();
        
        if (shouldCollapse && !collapsed)
        {
            collapsePanel();
        }
        else if (!shouldCollapse && collapsed)
        {
            expandPanel();
        }
    }
    
private:
    bool collapsed = false;
    void collapsePanel() { /* Hide non-essential controls */ }
    void expandPanel() { /* Show all controls */ }
};
```

## Best Practices

### 1. Always Use Responsive Scaling

```cpp
// ❌ Don't use fixed dimensions
button->setSize(100, 30);

// ✅ Use responsive scaling  
button->setSize(responsiveManager->scaled(100), responsiveManager->touchTarget(30));
```

### 2. Check Platform Capabilities

```cpp
// ❌ Assume desktop behavior
component->setMouseCursor(juce::MouseCursor::PointingHandCursor);

// ✅ Adapt to platform
if (responsiveManager->supportsHover())
{
    component->setMouseCursor(juce::MouseCursor::PointingHandCursor);
}
```

### 3. Use Appropriate Layout Modes

```cpp
void MyComponent::resized()
{
    auto formFactor = responsiveManager->getPlatformConfig().formFactor;
    
    switch (formFactor)
    {
        case PlatformResponsiveManager::DeviceFormFactor::Phone:
            setupPhoneLayout();
            break;
        case PlatformResponsiveManager::DeviceFormFactor::Tablet:
            setupTabletLayout();
            break;
        case PlatformResponsiveManager::DeviceFormFactor::Desktop:
            setupDesktopLayout();
            break;
        case PlatformResponsiveManager::DeviceFormFactor::PluginHost:
            setupPluginLayout();
            break;
    }
}
```

### 4. Handle State Persistence

```cpp
class MyComponent : public juce::Component
{
public:
    void saveState()
    {
        auto state = responsiveManager->saveState();
        // Store state in your settings
        settings->addChild(state, nullptr);
    }
    
    void loadState()
    {
        auto state = settings->getChildWithName("PlatformResponsive");
        if (state.isValid())
        {
            responsiveManager->loadState(state);
        }
    }
};
```

## Accessibility Integration

The system automatically handles accessibility features:

```cpp
// High contrast mode detection
if (responsiveManager->isHighContrastMode())
{
    // Increase contrast, use stronger borders
    g.setColour(textColor.contrasting(0.8f));
}

// Reduced motion support
if (responsiveManager->shouldUseAnimations())
{
    // Animate transitions
    Desktop::getInstance().getAnimator().animateComponent(
        component, newBounds, 1.0f, 300, false, 1.0f, 0.0f);
}
else
{
    // Instant positioning
    component->setBounds(newBounds);
}

// Large text support
if (responsiveManager->isLargeTextEnabled())
{
    // Text is automatically scaled larger
    // Additional spacing may be needed
    auto extraSpacing = responsiveManager->scaled(4);
    component->setBounds(area.expanded(extraSpacing));
}
```

## Testing Across Platforms

### Desktop Testing

```cpp
// Test different DPI settings
responsiveManager->setPlatformConfig(config);
```

### Mobile Simulation

```cpp
// Simulate mobile platform
PlatformResponsiveManager::PlatformConfig mobileConfig;
mobileConfig.platform = PlatformResponsiveManager::TargetPlatform::iOS_Standalone;
mobileConfig.formFactor = PlatformResponsiveManager::DeviceFormFactor::Phone;
mobileConfig.hasTouchInput = true;
mobileConfig.hasHoverCapability = false;

responsiveManager->setPlatformConfig(mobileConfig);
```

## Performance Considerations

- Responsive calculations are cached and only updated when needed
- Use the global responsive manager when possible to avoid redundant calculations
- Update layout only on actual size changes, not every paint cycle
- Cache scaled dimensions for frequently used values

## Migration from Existing Components

1. Add PlatformResponsiveManager to your component
2. Replace fixed dimensions with scaled equivalents
3. Add platform-specific behavior checks
4. Update LookAndFeel to PlatformAwareLookAndFeel
5. Test on all target platforms

The responsive system is designed to be gradually adoptable - you can migrate components one at a time while maintaining compatibility with existing code.