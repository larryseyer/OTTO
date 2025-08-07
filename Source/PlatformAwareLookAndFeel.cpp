#include "PlatformAwareLookAndFeel.h"

PlatformAwareLookAndFeel::PlatformAwareLookAndFeel(FontManager& fontManager, 
                                                   ColorScheme& colorScheme,
                                                   PlatformResponsiveManager& responsiveManager)
    : CustomLookAndFeel(fontManager, colorScheme)
    , responsiveManager(responsiveManager)
{
    currentPlatform = responsiveManager.getPlatformConfig().platform;
    currentFormFactor = responsiveManager.getPlatformConfig().formFactor;
    hasTouchInput = responsiveManager.getPlatformConfig().hasTouchInput;
    hasHoverCapability = responsiveManager.getPlatformConfig().hasHoverCapability;
    isHighDPI = responsiveManager.getPlatformConfig().density >= PlatformResponsiveManager::ScreenDensity::XHDPI;
    
    updatePlatformStyling();
    applyPlatformSpecificSettings();
}

PlatformAwareLookAndFeel::~PlatformAwareLookAndFeel()
{
}

void PlatformAwareLookAndFeel::updatePlatformStyling()
{
    switch (currentPlatform)
    {
        case PlatformResponsiveManager::TargetPlatform::macOS_Standalone:
        case PlatformResponsiveManager::TargetPlatform::macOS_VST3:
        case PlatformResponsiveManager::TargetPlatform::macOS_AU:
        case PlatformResponsiveManager::TargetPlatform::macOS_CLAP:
            styling.buttonCornerRadius = 6.0f;
            styling.sliderCornerRadius = 3.0f;
            styling.shadowOpacity = 0.2f;
            styling.useGradients = true;
            styling.useShadows = true;
            styling.flatDesign = false;
            styling.touchTargetMinSize = 28; // Mouse-optimized
            styling.touchPadding = 4.0f;
            break;
            
        case PlatformResponsiveManager::TargetPlatform::Windows_Standalone:
        case PlatformResponsiveManager::TargetPlatform::Windows_VST3:
        case PlatformResponsiveManager::TargetPlatform::Windows_CLAP:
            styling.buttonCornerRadius = 2.0f;
            styling.sliderCornerRadius = 1.0f;
            styling.shadowOpacity = 0.15f;
            styling.useGradients = false;
            styling.useShadows = false;
            styling.flatDesign = true;
            styling.touchTargetMinSize = 32;
            styling.touchPadding = 4.0f;
            break;
            
        case PlatformResponsiveManager::TargetPlatform::Linux_Standalone:
        case PlatformResponsiveManager::TargetPlatform::Linux_VST3:
        case PlatformResponsiveManager::TargetPlatform::Linux_CLAP:
            styling.buttonCornerRadius = 3.0f;
            styling.sliderCornerRadius = 2.0f;
            styling.shadowOpacity = 0.1f;
            styling.useGradients = false;
            styling.useShadows = false;
            styling.flatDesign = true;
            styling.touchTargetMinSize = 32;
            styling.touchPadding = 4.0f;
            break;
            
        case PlatformResponsiveManager::TargetPlatform::iOS_Standalone:
        case PlatformResponsiveManager::TargetPlatform::iOS_AUv3:
            styling.buttonCornerRadius = 8.0f;
            styling.sliderCornerRadius = 6.0f;
            styling.shadowOpacity = 0.3f;
            styling.useGradients = true;
            styling.useShadows = true;
            styling.flatDesign = false;
            styling.touchTargetMinSize = 44; // iOS HIG minimum
            styling.touchPadding = 12.0f;
            break;
            
        case PlatformResponsiveManager::TargetPlatform::Android_Standalone:
            styling.buttonCornerRadius = 4.0f;
            styling.sliderCornerRadius = 2.0f;
            styling.shadowOpacity = 0.4f;
            styling.useGradients = false;
            styling.useShadows = true;
            styling.flatDesign = false; // Material Design uses elevation
            styling.touchTargetMinSize = 48; // Material Design minimum
            styling.touchPadding = 16.0f;
            break;
    }
}

void PlatformAwareLookAndFeel::applyPlatformSpecificSettings()
{
    // Apply JUCE LookAndFeel settings based on platform
    switch (currentPlatform)
    {
        case PlatformResponsiveManager::TargetPlatform::macOS_Standalone:
        case PlatformResponsiveManager::TargetPlatform::macOS_VST3:
        case PlatformResponsiveManager::TargetPlatform::macOS_AU:
        case PlatformResponsiveManager::TargetPlatform::macOS_CLAP:
            // macOS-specific colors and settings
            setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff2d2d2d));
            break;
            
        case PlatformResponsiveManager::TargetPlatform::Windows_Standalone:
        case PlatformResponsiveManager::TargetPlatform::Windows_VST3:
        case PlatformResponsiveManager::TargetPlatform::Windows_CLAP:
            // Windows-specific colors and settings
            setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff323232));
            break;
            
        case PlatformResponsiveManager::TargetPlatform::iOS_Standalone:
        case PlatformResponsiveManager::TargetPlatform::iOS_AUv3:
            // iOS-specific colors and settings
            setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff1c1c1e));
            break;
            
        case PlatformResponsiveManager::TargetPlatform::Android_Standalone:
            // Android Material Design colors
            setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff121212));
            break;
            
        default:
            break;
    }
}

// Enhanced button drawing with platform-specific adaptations
void PlatformAwareLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                   const juce::Colour& backgroundColour,
                                                   bool shouldDrawButtonAsHighlighted,
                                                   bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    
    // Apply responsive scaling to corner radius
    float cornerRadius = responsiveManager.scaled(styling.buttonCornerRadius);
    
    // Platform-specific button drawing
    switch (currentPlatform)
    {
        case PlatformResponsiveManager::TargetPlatform::macOS_Standalone:
        case PlatformResponsiveManager::TargetPlatform::macOS_VST3:
        case PlatformResponsiveManager::TargetPlatform::macOS_AU:
        case PlatformResponsiveManager::TargetPlatform::macOS_CLAP:
            drawMacOSButton(g, bounds.toNearestInt(), backgroundColour, 
                           shouldDrawButtonAsDown, shouldDrawButtonAsHighlighted);
            break;
            
        case PlatformResponsiveManager::TargetPlatform::Windows_Standalone:
        case PlatformResponsiveManager::TargetPlatform::Windows_VST3:
        case PlatformResponsiveManager::TargetPlatform::Windows_CLAP:
            drawWindowsButton(g, bounds.toNearestInt(), backgroundColour,
                             shouldDrawButtonAsDown, shouldDrawButtonAsHighlighted);
            break;
            
        case PlatformResponsiveManager::TargetPlatform::Linux_Standalone:
        case PlatformResponsiveManager::TargetPlatform::Linux_VST3:
        case PlatformResponsiveManager::TargetPlatform::Linux_CLAP:
            drawLinuxButton(g, bounds.toNearestInt(), backgroundColour,
                           shouldDrawButtonAsDown, shouldDrawButtonAsHighlighted);
            break;
            
        case PlatformResponsiveManager::TargetPlatform::iOS_Standalone:
        case PlatformResponsiveManager::TargetPlatform::iOS_AUv3:
            drawiOSButton(g, bounds.toNearestInt(), backgroundColour,
                         shouldDrawButtonAsDown, shouldDrawButtonAsHighlighted);
            break;
            
        case PlatformResponsiveManager::TargetPlatform::Android_Standalone:
            drawAndroidButton(g, bounds.toNearestInt(), backgroundColour,
                             shouldDrawButtonAsDown, shouldDrawButtonAsHighlighted);
            break;
    }
}

void PlatformAwareLookAndFeel::drawMacOSButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                                              const juce::Colour& colour, bool isPressed, bool isHighlighted) const
{
    auto cornerRadius = responsiveManager.scaled(styling.buttonCornerRadius);
    
    // macOS-style gradient button
    juce::ColourGradient gradient;
    if (isPressed)
    {
        gradient = juce::ColourGradient(colour.darker(0.3f), area.toFloat().getTopLeft(),
                                       colour.darker(0.1f), area.toFloat().getBottomLeft(), false);
    }
    else if (isHighlighted)
    {
        gradient = juce::ColourGradient(colour.brighter(0.2f), area.toFloat().getTopLeft(),
                                       colour, area.toFloat().getBottomLeft(), false);
    }
    else
    {
        gradient = juce::ColourGradient(colour.brighter(0.1f), area.toFloat().getTopLeft(),
                                       colour.darker(0.1f), area.toFloat().getBottomLeft(), false);
    }
    
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(area.toFloat(), cornerRadius);
    
    // Subtle border
    g.setColour(colour.darker(0.3f));
    g.drawRoundedRectangle(area.toFloat().reduced(0.5f), cornerRadius, 1.0f);
}

void PlatformAwareLookAndFeel::drawWindowsButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                                                const juce::Colour& colour, bool isPressed, bool isHighlighted) const
{
    auto cornerRadius = responsiveManager.scaled(styling.buttonCornerRadius);
    
    // Windows flat design
    juce::Colour fillColour = colour;
    if (isPressed)
        fillColour = colour.darker(0.2f);
    else if (isHighlighted)
        fillColour = colour.brighter(0.1f);
    
    g.setColour(fillColour);
    g.fillRoundedRectangle(area.toFloat(), cornerRadius);
    
    // Minimal border only when focused or pressed
    if (isPressed || isHighlighted)
    {
        g.setColour(colour.contrasting(0.3f));
        g.drawRoundedRectangle(area.toFloat().reduced(0.5f), cornerRadius, 1.0f);
    }
}

void PlatformAwareLookAndFeel::drawLinuxButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                                              const juce::Colour& colour, bool isPressed, bool isHighlighted) const
{
    auto cornerRadius = responsiveManager.scaled(styling.buttonCornerRadius);
    
    // Linux GTK-style flat button
    juce::Colour fillColour = colour;
    if (isPressed)
        fillColour = colour.darker(0.15f);
    else if (isHighlighted)
        fillColour = colour.brighter(0.05f);
    
    g.setColour(fillColour);
    g.fillRoundedRectangle(area.toFloat(), cornerRadius);
    
    // Simple border
    g.setColour(colour.darker(0.2f));
    g.drawRoundedRectangle(area.toFloat().reduced(0.5f), cornerRadius, 1.0f);
}

void PlatformAwareLookAndFeel::drawiOSButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                                            const juce::Colour& colour, bool isPressed, bool isHighlighted) const
{
    auto cornerRadius = responsiveManager.scaled(styling.buttonCornerRadius);
    
    // iOS-style with shadow and gradient
    if (styling.useShadows && !isPressed)
    {
        // Drop shadow
        juce::DropShadow shadow(juce::Colours::black.withAlpha(styling.shadowOpacity), 
                               responsiveManager.scaled(3), juce::Point<int>(0, responsiveManager.scaled(1)));
        shadow.drawForRectangle(g, area);
    }
    
    // Gradient fill
    juce::ColourGradient gradient;
    if (isPressed)
    {
        gradient = juce::ColourGradient(colour.darker(0.2f), area.toFloat().getTopLeft(),
                                       colour.darker(0.05f), area.toFloat().getBottomLeft(), false);
    }
    else
    {
        gradient = juce::ColourGradient(colour.brighter(0.15f), area.toFloat().getTopLeft(),
                                       colour.darker(0.05f), area.toFloat().getBottomLeft(), false);
    }
    
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(area.toFloat(), cornerRadius);
    
    // Subtle highlight on top edge
    if (!isPressed)
    {
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.fillRoundedRectangle(juce::Rectangle<float>(area.getX(), area.getY(), 
                                                     area.getWidth(), responsiveManager.scaled(1)), 
                              cornerRadius);
    }
}

void PlatformAwareLookAndFeel::drawAndroidButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                                                const juce::Colour& colour, bool isPressed, bool isHighlighted) const
{
    auto cornerRadius = responsiveManager.scaled(styling.buttonCornerRadius);
    
    // Material Design elevation
    if (styling.useShadows)
    {
        int elevation = isPressed ? responsiveManager.scaled(8) : responsiveManager.scaled(4);
        juce::DropShadow shadow(juce::Colours::black.withAlpha(styling.shadowOpacity), 
                               elevation, juce::Point<int>(0, elevation / 2));
        shadow.drawForRectangle(g, area);
    }
    
    // Material button fill
    juce::Colour fillColour = colour;
    if (isPressed)
        fillColour = colour.darker(0.1f);
    else if (isHighlighted)
        fillColour = colour.brighter(0.05f);
    
    g.setColour(fillColour);
    g.fillRoundedRectangle(area.toFloat(), cornerRadius);
    
    // Ripple effect overlay (simplified)
    if (isPressed && hasTouchInput)
    {
        g.setColour(juce::Colours::white.withAlpha(0.1f));
        g.fillRoundedRectangle(area.toFloat(), cornerRadius);
    }
}

// Enhanced text drawing with responsive font scaling
void PlatformAwareLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                             bool shouldDrawButtonAsHighlighted,
                                             bool shouldDrawButtonAsDown)
{
    auto font = getTextButtonFont(button, button.getHeight());
    g.setFont(font);
    
    // Platform-specific text color adjustments
    auto baseColour = button.findColour(juce::TextButton::textColourOffId);
    if (shouldDrawButtonAsDown)
        baseColour = button.findColour(juce::TextButton::textColourOnId);
    
    auto textColour = getAdaptedColour(baseColour);
    g.setColour(textColour);
    
    auto textArea = button.getLocalBounds();
    if (hasTouchInput)
    {
        // Add more padding for touch targets
        textArea = textArea.reduced(responsiveManager.scaled(styling.touchPadding));
    }
    
    g.drawFittedText(button.getButtonText(), textArea, juce::Justification::centred, 1,
                     responsiveManager.shouldUseAnimations() ? 1.0f : 0.0f);
}

// Enhanced slider drawing with touch optimization
void PlatformAwareLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                               float sliderPos, float minSliderPos, float maxSliderPos,
                                               const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    auto trackArea = juce::Rectangle<int>(x, y, width, height);
    auto isHorizontal = style == juce::Slider::LinearHorizontal;
    
    // Calculate thumb size based on platform and touch requirements
    int thumbSize = hasTouchInput ? getTouchTargetSize(24) : responsiveManager.scaled(16);
    
    juce::Rectangle<int> thumbArea;
    if (isHorizontal)
    {
        auto thumbX = static_cast<int>(sliderPos - thumbSize / 2);
        thumbArea = juce::Rectangle<int>(thumbX, y + (height - thumbSize) / 2, thumbSize, thumbSize);
    }
    else
    {
        auto thumbY = static_cast<int>(sliderPos - thumbSize / 2);
        thumbArea = juce::Rectangle<int>(x + (width - thumbSize) / 2, thumbY, thumbSize, thumbSize);
    }
    
    // Platform-specific slider drawing
    switch (currentPlatform)
    {
        case PlatformResponsiveManager::TargetPlatform::macOS_Standalone:
        case PlatformResponsiveManager::TargetPlatform::macOS_VST3:
        case PlatformResponsiveManager::TargetPlatform::macOS_AU:
        case PlatformResponsiveManager::TargetPlatform::macOS_CLAP:
            drawMacOSSlider(g, trackArea, thumbArea, isHorizontal);
            break;
            
        case PlatformResponsiveManager::TargetPlatform::Windows_Standalone:
        case PlatformResponsiveManager::TargetPlatform::Windows_VST3:
        case PlatformResponsiveManager::TargetPlatform::Windows_CLAP:
            drawWindowsSlider(g, trackArea, thumbArea, isHorizontal);
            break;
            
        case PlatformResponsiveManager::TargetPlatform::iOS_Standalone:
        case PlatformResponsiveManager::TargetPlatform::iOS_AUv3:
        case PlatformResponsiveManager::TargetPlatform::Android_Standalone:
            drawMobileSlider(g, trackArea, thumbArea, isHorizontal);
            break;
            
        default:
            drawMacOSSlider(g, trackArea, thumbArea, isHorizontal);
            break;
    }
}

void PlatformAwareLookAndFeel::drawMacOSSlider(juce::Graphics& g, const juce::Rectangle<int>& trackArea,
                                              const juce::Rectangle<int>& thumbArea, bool isHorizontal) const
{
    auto trackColour = colorScheme.getSliderTrackColor();
    auto thumbColour = colorScheme.getSliderThumbColor();
    
    // Track
    auto track = trackArea.reduced(trackArea.getWidth() / 4, trackArea.getHeight() / 4);
    g.setColour(trackColour);
    g.fillRoundedRectangle(track.toFloat(), responsiveManager.scaled(styling.sliderCornerRadius));
    
    // Thumb with macOS-style gradient
    juce::ColourGradient gradient(thumbColour.brighter(0.2f), thumbArea.toFloat().getTopLeft(),
                                 thumbColour.darker(0.1f), thumbArea.toFloat().getBottomLeft(), false);
    g.setGradientFill(gradient);
    g.fillEllipse(thumbArea.toFloat());
    
    // Thumb border
    g.setColour(thumbColour.darker(0.3f));
    g.drawEllipse(thumbArea.toFloat().reduced(0.5f), 1.0f);
}

void PlatformAwareLookAndFeel::drawWindowsSlider(juce::Graphics& g, const juce::Rectangle<int>& trackArea,
                                                const juce::Rectangle<int>& thumbArea, bool isHorizontal) const
{
    auto trackColour = colorScheme.getSliderTrackColor();
    auto thumbColour = colorScheme.getSliderThumbColor();
    
    // Flat track
    auto track = trackArea.reduced(trackArea.getWidth() / 4, trackArea.getHeight() / 4);
    g.setColour(trackColour);
    g.fillRoundedRectangle(track.toFloat(), responsiveManager.scaled(2.0f));
    
    // Flat thumb
    g.setColour(thumbColour);
    g.fillRoundedRectangle(thumbArea.toFloat(), responsiveManager.scaled(2.0f));
    
    // Subtle border
    g.setColour(thumbColour.darker(0.2f));
    g.drawRoundedRectangle(thumbArea.toFloat().reduced(0.5f), responsiveManager.scaled(2.0f), 1.0f);
}

void PlatformAwareLookAndFeel::drawMobileSlider(juce::Graphics& g, const juce::Rectangle<int>& trackArea,
                                               const juce::Rectangle<int>& thumbArea, bool isHorizontal) const
{
    auto trackColour = colorScheme.getSliderTrackColor();
    auto thumbColour = colorScheme.getSliderThumbColor();
    
    // Thicker track for touch
    auto trackThickness = responsiveManager.scaled(6);
    auto track = isHorizontal ? 
        juce::Rectangle<int>(trackArea.getX(), trackArea.getCentreY() - trackThickness/2, 
                            trackArea.getWidth(), trackThickness) :
        juce::Rectangle<int>(trackArea.getCentreX() - trackThickness/2, trackArea.getY(),
                            trackThickness, trackArea.getHeight());
    
    g.setColour(trackColour);
    g.fillRoundedRectangle(track.toFloat(), trackThickness / 2.0f);
    
    // Larger thumb for touch
    if (styling.useShadows)
    {
        juce::DropShadow shadow(juce::Colours::black.withAlpha(0.3f), 
                               responsiveManager.scaled(4), juce::Point<int>(0, responsiveManager.scaled(2)));
        shadow.drawForRectangle(g, thumbArea);
    }
    
    g.setColour(thumbColour);
    g.fillEllipse(thumbArea.toFloat());
}

// Font methods with responsive scaling
juce::Font PlatformAwareLookAndFeel::getLabelFont(juce::Label& label)
{
    auto baseFont = CustomLookAndFeel::getLabelFont(label);
    return getAdaptedFont(baseFont);
}

juce::Font PlatformAwareLookAndFeel::getTextButtonFont(juce::TextButton& button, int buttonHeight)
{
    auto baseFont = CustomLookAndFeel::getTextButtonFont(button, buttonHeight);
    return getAdaptedFont(baseFont);
}

juce::Font PlatformAwareLookAndFeel::getComboBoxFont(juce::ComboBox& box)
{
    auto baseFont = CustomLookAndFeel::getComboBoxFont(box);
    return getAdaptedFont(baseFont);
}

juce::Font PlatformAwareLookAndFeel::getPopupMenuFont()
{
    auto baseFont = CustomLookAndFeel::getPopupMenuFont();
    return getAdaptedFont(baseFont);
}

juce::Font PlatformAwareLookAndFeel::getAlertWindowMessageFont()
{
    auto baseFont = CustomLookAndFeel::getAlertWindowMessageFont();
    return getAdaptedFont(baseFont);
}

juce::Font PlatformAwareLookAndFeel::getAlertWindowTitleFont()
{
    auto baseFont = CustomLookAndFeel::getAlertWindowTitleFont();
    return getAdaptedFont(baseFont);
}

juce::Font PlatformAwareLookAndFeel::getAdaptedFont(const juce::Font& baseFont) const
{
    return responsiveManager.scaled(baseFont);
}

// Utility methods
juce::Colour PlatformAwareLookAndFeel::getAdaptedColour(const juce::Colour& baseColour) const
{
    return responsiveManager.adaptColorForPlatform(baseColour);
}

int PlatformAwareLookAndFeel::getTouchTargetSize(int baseSize) const
{
    return responsiveManager.touchTarget(baseSize);
}

float PlatformAwareLookAndFeel::getTouchTargetPadding() const
{
    return responsiveManager.scaled(styling.touchPadding);
}

bool PlatformAwareLookAndFeel::shouldUseFlatDesign() const
{
    return styling.flatDesign;
}

bool PlatformAwareLookAndFeel::shouldUseGradients() const
{
    return styling.useGradients;
}

bool PlatformAwareLookAndFeel::shouldUseShadows() const
{
    return styling.useShadows;
}

bool PlatformAwareLookAndFeel::shouldUseRoundedCorners() const
{
    return styling.buttonCornerRadius > 0.0f;
}

float PlatformAwareLookAndFeel::getCornerRadius(const juce::Rectangle<int>& area) const
{
    auto radius = responsiveManager.scaled(styling.buttonCornerRadius);
    return std::min(radius, static_cast<float>(std::min(area.getWidth(), area.getHeight()) / 4));
}

bool PlatformAwareLookAndFeel::shouldShowTouchFeedback() const
{
    return hasTouchInput;
}

// Update methods
void PlatformAwareLookAndFeel::platformChanged()
{
    currentPlatform = responsiveManager.getPlatformConfig().platform;
    currentFormFactor = responsiveManager.getPlatformConfig().formFactor;
    hasTouchInput = responsiveManager.getPlatformConfig().hasTouchInput;
    hasHoverCapability = responsiveManager.getPlatformConfig().hasHoverCapability;
    
    updatePlatformStyling();
    applyPlatformSpecificSettings();
}

void PlatformAwareLookAndFeel::responsiveScaleChanged(const PlatformResponsiveManager::ResponsiveScale& newScale)
{
    // Update any cached scaled values if needed
    forceRefreshFromColorScheme();
}

void PlatformAwareLookAndFeel::accessibilitySettingsChanged()
{
    // Update accessibility-related styling
    forceRefreshFromColorScheme();
}

// Additional drawing methods for platform-specific UI elements
void PlatformAwareLookAndFeel::drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar,
                                            int x, int y, int width, int height,
                                            bool isScrollbarVertical, int thumbStartPosition,
                                            int thumbSize, bool isMouseOver, bool isMouseDown)
{
    if (!responsiveManager.shouldShowScrollbars())
        return; // Hide scrollbars on mobile platforms
    
    CustomLookAndFeel::drawScrollbar(g, scrollbar, x, y, width, height,
                                    isScrollbarVertical, thumbStartPosition, thumbSize,
                                    isMouseOver, isMouseDown);
}

int PlatformAwareLookAndFeel::getDefaultScrollbarWidth()
{
    if (hasTouchInput)
        return responsiveManager.scaled(20); // Wider for touch
    else
        return responsiveManager.scaled(14); // Standard desktop width
}

// Factory implementation
std::unique_ptr<PlatformAwareLookAndFeel> PlatformLookAndFeelFactory::createForCurrentPlatform(
    FontManager& fontManager,
    ColorScheme& colorScheme,
    PlatformResponsiveManager& responsiveManager)
{
    return std::make_unique<PlatformAwareLookAndFeel>(fontManager, colorScheme, responsiveManager);
}

std::unique_ptr<PlatformAwareLookAndFeel> PlatformLookAndFeelFactory::createForPlatform(
    PlatformResponsiveManager::TargetPlatform platform,
    FontManager& fontManager,
    ColorScheme& colorScheme,
    PlatformResponsiveManager& responsiveManager)
{
    auto lookAndFeel = std::make_unique<PlatformAwareLookAndFeel>(fontManager, colorScheme, responsiveManager);
    
    // Set platform-specific overrides if needed
    return lookAndFeel;
}