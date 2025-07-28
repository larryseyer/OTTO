# OTTO Design Rules & GUI Guidelines

This document outlines the user interface design guidelines, component standards, and visual design principles for the OTTO project.

## 🎨 Design Philosophy

OTTO follows a **modern, professional design system** built specifically for audio production environments:

- **Professional Studio Aesthetic**: Clean, functional interface suitable for professional music production
- **Responsive & Adaptive**: Interface scales seamlessly across different devices and screen sizes
- **INI-Driven Layout**: Zero hardcoded values - all positioning and sizing controlled via INI configuration
- **JUCE 8 Native**: Leverages modern JUCE 8 UI components and best practices
- **Cross-Platform Consistency**: Identical appearance and behavior across all 5 supported platforms

## 📐 Layout System

### Row-Based Organization
OTTO uses a **6-row layout system** for optimal organization:

```
Row 1: Top Bar (Transport, Global Controls)
Row 2: Player Tabs (8 Player Selection)  
Row 3: Drum Kit Section (Kit Selection & Controls)
Row 4: Scene Launcher (Pattern Triggers)
Row 5: Pattern Matrix + Controls (Beat Grid + Right Panel)
Row 6: Loop Section (Timeline Controls)
```

### INI-Driven Positioning
**CRITICAL**: All layout values MUST use INIConfig constants:

```cpp
// ✅ CORRECT - INI-driven layout
auto buttonWidth = static_cast<int>(getWidth() * 
    INIConfig::LayoutConstants::buttonWidthPercent / 100.0f);

// ❌ WRONG - Hardcoded values
auto buttonWidth = 120; // Never do this!
```

### Layout Constants Structure
```cpp
namespace INIConfig::LayoutConstants {
    // Interface Dimensions
    constexpr int defaultInterfaceWidth = 1200;
    constexpr int defaultInterfaceHeight = 800;
    
    // Row Heights (percentages of total height)
    constexpr float row1HeightPercent = 8.0f;   // Top Bar
    constexpr float row2HeightPercent = 10.0f;  // Player Tabs
    constexpr float row3HeightPercent = 15.0f;  // Drum Kit Section
    constexpr float row4HeightPercent = 20.0f;  // Scene Launcher
    constexpr float row5HeightPercent = 32.0f;  // Pattern Matrix
    constexpr float row6HeightPercent = 15.0f;  // Loop Section
    
    // Margins and Spacing
    constexpr int defaultMargin = 10;
    constexpr int defaultSpacing = 5;
    constexpr int defaultButtonHeight = 30;
    
    // Font Sizes
    constexpr float fontSizeSmall = 12.0f;
    constexpr float fontSizeNormal = 14.0f;
    constexpr float fontSizeTitle = 16.0f;
    constexpr float fontSizeLarge = 18.0f;
}
```

## 🎛️ Component Standards

### Button Design
```cpp
class StandardButton : public juce::TextButton {
    // Standard button dimensions
    static constexpr int minWidth = 80;
    static constexpr int standardHeight = 30;
    static constexpr int cornerRadius = 4;
    
    // Colors from ColorScheme
    juce::Colour normalColor = colorScheme.getButtonColor();
    juce::Colour hoverColor = colorScheme.getButtonHoverColor();
    juce::Colour pressedColor = colorScheme.getButtonPressedColor();
};
```

### Player Tab Layout
**Special Case**: Player buttons use precise margin calculations:
```cpp
// Player 1: 1/2w margin LEFT only
// Players 2-7: 1/4w margin LEFT only  
// Player 8: 1/4w margin LEFT + 1/2w margin RIGHT
auto buttonWidth = availableWidth / 10.75f;
```

### Slider Controls
```cpp
class StandardSlider : public juce::Slider {
    // Standard slider dimensions
    static constexpr int minWidth = 100;
    static constexpr int standardHeight = 30;
    static constexpr int knobSize = 20;
    
    // Touch-friendly sizing for mobile
    static constexpr int touchHeight = 44;  // iOS guideline
};
```

## 🎨 Visual Design System

### Color Scheme Integration
OTTO uses a **centralized color management system**:

```cpp
// ColorScheme.h defines all interface colors
class ColorScheme {
public:
    // Primary Interface Colors
    juce::Colour getBackgroundColor() const;
    juce::Colour getForegroundColor() const;
    juce::Colour getAccentColor() const;
    
    // Component-Specific Colors
    juce::Colour getButtonColor() const;
    juce::Colour getSliderColor() const;
    juce::Colour getTextColor() const;
    
    // State Colors
    juce::Colour getActiveColor() const;
    juce::Colour getInactiveColor() const;
    juce::Colour getHoverColor() const;
};
```

### Typography System
OTTO includes embedded professional fonts:

#### Primary Fonts
- **Roboto**: Main UI text, excellent readability
- **Roboto Condensed**: Compact UI elements
- **Montserrat**: Headings and titles
- **Orbitron**: Technical/digital displays
- **Open Sans**: Body text and descriptions

#### Font Loading Pattern
```cpp
// FontManager handles all font loading
auto font = fontManager->getRobotoFont(FontManager::Weight::Regular, 14.0f);
auto titleFont = fontManager->getMontserratFont(FontManager::Weight::Bold, 18.0f);

// Icon fonts (Phosphor system)
auto iconFont = fontManager->getPhosphorFont(FontManager::PhosphorWeight::Regular, 16.0f);
auto playIcon = fontManager->getIconString("play");
```

### Phosphor Icon System
OTTO uses the **Phosphor icon font** with 6 weight variations:

```cpp
enum class PhosphorWeight {
    Thin,      // Ultra-light icons
    Light,     // Light icons
    Regular,   // Standard weight
    Bold,      // Bold icons
    Fill,      // Filled icons
    Duotone    // Two-tone icons
};

// Usage examples
auto playIcon = fontManager->getIconString("play");
auto stopIcon = fontManager->getIconString("stop");
auto recordIcon = fontManager->getIconString("record");
```

## 📱 Responsive Design

### Screen Size Adaptation
```cpp
void resized() override {
    // Calculate proportional dimensions
    auto area = getLocalBounds();
    auto scaleFactor = getWidth() / static_cast<float>(INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH);
    
    // Apply scaling to all components
    updateRow1Layout(area.removeFromTop(getRowHeight(1)));
    updateRow2Layout(area.removeFromTop(getRowHeight(2)));
    // ... continue for all rows
}

int getRowHeight(int rowNumber) const {
    auto totalHeight = getHeight();
    switch (rowNumber) {
        case 1: return totalHeight * INIConfig::LayoutConstants::row1HeightPercent / 100.0f;
        case 2: return totalHeight * INIConfig::LayoutConstants::row2HeightPercent / 100.0f;
        // ... etc
    }
}
```

### Platform-Specific Adaptations
```cpp
#if JUCE_IOS || JUCE_ANDROID
    // Touch-friendly sizing for mobile
    static constexpr int minTouchTarget = 44;
    static constexpr int defaultSpacing = 8;
#else
    // Desktop sizing
    static constexpr int minTouchTarget = 30;
    static constexpr int defaultSpacing = 5;
#endif
```

## 🎯 Component Guidelines

### MainContentComponent Structure
```cpp
class MainContentComponent : public juce::Component {
private:
    // Row Components (defined order)
    std::unique_ptr<TopBarComponent> topBar;
    std::unique_ptr<PlayerTabsComponent> playerTabs;
    std::unique_ptr<DrumKitSectionComponent> drumKitSection;
    std::unique_ptr<SceneLauncherComponent> sceneLauncher;
    std::unique_ptr<MainContentComponentRightSection> rightSection;
    std::unique_ptr<LoopSectionComponent> loopSection;
    
    // Layout methods for each row
    void updateRow1Layout(juce::Rectangle<int> area);
    void updateRow2Layout(juce::Rectangle<int> area);
    void updateRow3Layout(juce::Rectangle<int> area);
    void updateRow4Layout(juce::Rectangle<int> area);
    void updateRow5Layout(juce::Rectangle<int> area);
    void updateRow6Layout(juce::Rectangle<int> area);
};
```

### Popup Window Standards
OTTO uses consistent popup windows for contextual editing:

```cpp
class StandardPopup : public juce::Component {
public:
    // Standard popup dimensions
    static constexpr int defaultWidth = 400;
    static constexpr int defaultHeight = 300;
    static constexpr int cornerRadius = 8;
    
    // Standard layout
    void setupStandardLayout() {
        auto area = getLocalBounds().reduced(INIConfig::LayoutConstants::defaultMargin);
        
        // Title area
        auto titleArea = area.removeFromTop(30);
        titleLabel.setBounds(titleArea);
        
        // Content area
        area.removeFromTop(INIConfig::LayoutConstants::defaultSpacing);
        contentArea = area.removeFromTop(area.getHeight() - 50);
        
        // Button area
        area.removeFromTop(INIConfig::LayoutConstants::defaultSpacing);
        setupButtons(area);
    }
};
```

## 🖼️ Asset Management

### Embedded Graphics
All UI graphics are embedded via JUCE BinaryData:

```cpp
// Loading embedded button graphics
auto buttonImage = juce::ImageCache::getFromMemory(
    BinaryData::Button080_png, 
    BinaryData::Button080_pngSize
);

// With fallback strategy
if (!buttonImage.isValid()) {
    // Create default/generated graphics
    buttonImage = createDefaultButtonImage();
}
```

### Custom Look and Feel
```cpp
class OTTOLookAndFeel : public juce::LookAndFeel_V4 {
public:
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, 
                            const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted,
                            bool shouldDrawButtonAsDown) override {
        
        auto bounds = button.getLocalBounds().toFloat();
        auto cornerRadius = INIConfig::LayoutConstants::buttonCornerRadius;
        
        // Use color scheme
        auto color = shouldDrawButtonAsDown ? colorScheme.getButtonPressedColor() :
                    shouldDrawButtonAsHighlighted ? colorScheme.getButtonHoverColor() :
                    colorScheme.getButtonColor();
        
        g.setColour(color);
        g.fillRoundedRectangle(bounds, cornerRadius);
    }
};
```

## 📏 Accessibility Standards

### Touch Targets
- **Minimum Size**: 44×44 pixels (iOS standard)
- **Recommended Size**: 48×48 pixels
- **Spacing**: Minimum 8 pixels between touch targets

### Color Contrast
- **Normal Text**: 4.5:1 contrast ratio minimum
- **Large Text**: 3:1 contrast ratio minimum
- **UI Elements**: 3:1 contrast ratio minimum

### Keyboard Navigation
```cpp
void setupKeyboardNavigation() {
    setWantsKeyboardFocus(true);
    setFocusContainer(true);
    
    // Tab order for components
    addAndMakeVisible(component1);
    addAndMakeVisible(component2); 
    // Components automatically get tab order based on add order
}
```

## 🔧 Development Patterns

### Component Lifecycle
```cpp
class StandardComponent : public juce::Component {
public:
    StandardComponent() {
        // Initialize with INI config
        setupFromConfig();
        
        // Setup look and feel
        setLookAndFeel(&lookAndFeel);
    }
    
    ~StandardComponent() override {
        setLookAndFeel(nullptr);
    }
    
    void resized() override {
        // Always use proportional layout
        calculateProportionalLayout();
    }
    
private:
    void setupFromConfig() {
        // Load all settings from INI
        setSize(INIConfig::getComponentWidth(componentId),
                INIConfig::getComponentHeight(componentId));
    }
    
    void calculateProportionalLayout() {
        // All positioning based on current size, not hardcoded values
        auto area = getLocalBounds();
        // Use percentages and INI constants only
    }
};
```

### State Management
```cpp
class ComponentWithState : public juce::Component {
private:
    ComponentState state;  // Centralized state management
    
public:
    void setState(const ComponentState& newState) {
        state = newState;
        repaint();
    }
    
    void saveToINI() {
        INIDataManager::saveComponentState(componentId, state);
    }
    
    void loadFromINI() {
        state = INIDataManager::loadComponentState(componentId);
        updateFromState();
    }
};
```

## 📱 Platform-Specific Considerations

### macOS
- **Native Controls**: Use system-native appearances where appropriate
- **Menu Bar Integration**: Proper macOS menu structure
- **Retina Support**: High-DPI graphics and scaling

### Windows
- **Windows 10/11 Style**: Modern Windows design language
- **High-DPI Awareness**: Proper scaling on high-DPI displays
- **Window Management**: Proper minimize/maximize behavior

### Linux
- **GTK Integration**: Respect system themes where possible
- **Window Manager Compatibility**: Works with various Linux WMs
- **Consistent Appearance**: Maintain OTTO branding regardless of system theme

### iOS
- **Touch Optimization**: Larger touch targets, gesture support
- **Safe Area Respect**: Proper handling of notches and home indicators
- **iOS Design Guidelines**: Follow Apple's Human Interface Guidelines

### Android
- **Material Design Elements**: Where appropriate for audio apps
- **Gesture Navigation**: Support for Android gesture systems
- **Adaptive Icons**: Proper Android icon formats

## ✅ Quality Checklist

Before submitting any UI changes, verify:

- [ ] **No Hardcoded Values**: All positioning uses INIConfig constants
- [ ] **Responsive Layout**: Components resize properly on different screen sizes
- [ ] **Cross-Platform Testing**: Tested on all 5 supported platforms
- [ ] **Color Scheme Integration**: Uses centralized ColorScheme class
- [ ] **Font Management**: Uses FontManager for all text rendering
- [ ] **Asset Loading**: Proper embedded asset loading with fallbacks
- [ ] **Accessibility**: Meets minimum touch target and contrast requirements
- [ ] **Performance**: No unnecessary repaints or heavy calculations in paint()
- [ ] **Memory Management**: Proper RAII and smart pointer usage
- [ ] **JUCE 8 Compliance**: Uses modern JUCE 8 APIs and patterns

## 🎼 Audio-Specific UI Guidelines

### Real-Time Feedback
- **Visual Response**: UI must respond to audio events within 16ms (60fps)
- **Parameter Changes**: Smooth visual transitions for audio parameter changes
- **Level Meters**: Accurate audio level visualization with proper ballistics

### Professional Workflow
- **DAW Integration**: UI should feel familiar to DAW users
- **Keyboard Shortcuts**: Standard shortcuts for common audio operations
- **MIDI Learn**: Visual feedback for MIDI controller assignment

---

These design rules ensure OTTO maintains a **professional, consistent, and modern interface** across all platforms while leveraging the full power of JUCE 8's cross-platform capabilities.