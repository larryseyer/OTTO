#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "PlatformResponsiveManager.h"
#include "ColorScheme.h"
#include "FontManager.h"

/**
 * Platform-aware LookAndFeel that adapts to all target platforms
 * Extends the existing CustomLookAndFeel with responsive, platform-specific adaptations
 */
class PlatformAwareLookAndFeel : public CustomLookAndFeel
{
public:
    PlatformAwareLookAndFeel(FontManager& fontManager, 
                            ColorScheme& colorScheme,
                            PlatformResponsiveManager& responsiveManager);
    
    ~PlatformAwareLookAndFeel() override;

    // Enhanced button drawing with platform-specific adaptations
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                            const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted,
                            bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                       bool shouldDrawButtonAsHighlighted,
                       bool shouldDrawButtonAsDown) override;

    // Enhanced slider drawing with touch-optimized sizing
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float minSliderPos, float maxSliderPos,
                         const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    void drawLinearSliderThumb(juce::Graphics& g, int x, int y, int width, int height,
                              float sliderPos, float minSliderPos, float maxSliderPos,
                              const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, const float rotaryStartAngle,
                         const float rotaryEndAngle, juce::Slider& slider) override;

    // Platform-specific label drawing
    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    // Enhanced ComboBox with platform-appropriate sizing
    void drawComboBox(juce::Graphics& g, int width, int height,
                     bool isButtonDown, int buttonX, int buttonY,
                     int buttonW, int buttonH, juce::ComboBox& box) override;

    // Text editor with platform-specific appearance
    void drawTextEditorOutline(juce::Graphics& g, int width, int height,
                              juce::TextEditor& textEditor) override;

    void fillTextEditorBackground(juce::Graphics& g, int width, int height,
                                 juce::TextEditor& textEditor) override;

    // Platform-appropriate progress bars
    void drawProgressBar(juce::Graphics& g, juce::ProgressBar& progressBar,
                        int width, int height, double progress,
                        const juce::String& textToShow) override;

    // Scrollbars that adapt to platform conventions
    void drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar,
                      int x, int y, int width, int height,
                      bool isScrollbarVertical, int thumbStartPosition,
                      int thumbSize, bool isMouseOver, bool isMouseDown) override;

    // Tooltips with platform-specific styling
    void drawTooltip(juce::Graphics& g, const juce::String& text,
                    int width, int height) override;

    juce::Rectangle<int> getTooltipBounds(const juce::String& tipText,
                                         juce::Point<int> screenPos,
                                         juce::Rectangle<int> parentArea) override;

    // Popup menus with platform styling
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                          bool isSeparator, bool isActive, bool isHighlighted,
                          bool isTicked, bool hasSubMenu, const juce::String& text,
                          const juce::String& shortcutKeyText, const juce::Drawable* icon,
                          const juce::Colour* textColour) override;

    // Font methods with responsive scaling
    juce::Font getLabelFont(juce::Label& label) override;
    juce::Font getTextButtonFont(juce::TextButton& button, int buttonHeight) override;
    juce::Font getComboBoxFont(juce::ComboBox& box) override;
    juce::Font getPopupMenuFont() override;
    juce::Font getAlertWindowMessageFont() override;
    juce::Font getAlertWindowTitleFont() override;

    // Slider layout with touch-optimized dimensions
    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override;

    // Platform-specific dimensions
    int getDefaultScrollbarWidth() override;
    int getMinimumScrollbarThumbSize(juce::ScrollBar& scrollbar) override;
    int getDefaultMenuBarHeight() override;
    int getTabButtonBestWidth(juce::TabBarButton& button, int tabDepth) override;

    // Animation settings based on platform preferences
    int getAlertWindowButtonHeight() override;
    juce::Font getAlertWindowFont() override;

    // Platform-specific drawing helpers
    void drawRoundedRectangle(juce::Graphics& g, const juce::Rectangle<float>& area,
                             float cornerRadius, const juce::Colour& fillColour,
                             const juce::Colour& outlineColour = juce::Colour(),
                             float outlineThickness = 0.0f) const;

    void drawGlossyButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                         const juce::Colour& baseColour, bool isPressed) const;

    void drawFlatButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                       const juce::Colour& baseColour, bool isPressed, bool isHighlighted) const;

    void drawMaterialButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                           const juce::Colour& baseColour, bool isPressed, bool isHighlighted) const;

    void drawTouchOptimizedSlider(juce::Graphics& g, const juce::Rectangle<int>& trackArea,
                                 const juce::Rectangle<int>& thumbArea,
                                 const juce::Colour& trackColour, const juce::Colour& thumbColour,
                                 bool isHorizontal) const;

    // Accessibility helpers
    void drawFocusOutline(juce::Graphics& g, const juce::Rectangle<int>& area) const;
    void drawHighContrastOutline(juce::Graphics& g, const juce::Rectangle<int>& area,
                                const juce::Colour& colour) const;

    // Platform detection for styling decisions
    bool shouldUseFlatDesign() const;
    bool shouldUseGradients() const;
    bool shouldUseShadows() const;
    bool shouldUseRoundedCorners() const;
    float getCornerRadius(const juce::Rectangle<int>& area) const;

    // Touch-specific adaptations
    int getTouchTargetSize(int baseSize) const;
    float getTouchTargetPadding() const;
    bool shouldShowTouchFeedback() const;

    // Update methods
    void platformChanged();
    void responsiveScaleChanged(const PlatformResponsiveManager::ResponsiveScale& newScale);
    void accessibilitySettingsChanged();

private:
    PlatformResponsiveManager& responsiveManager;
    
    // Cached platform info for quick access
    PlatformResponsiveManager::TargetPlatform currentPlatform;
    PlatformResponsiveManager::DeviceFormFactor currentFormFactor;
    bool hasTouchInput = false;
    bool hasHoverCapability = true;
    bool isHighDPI = false;
    
    // Platform-specific styling constants
    struct PlatformStyling {
        float buttonCornerRadius = 4.0f;
        float sliderCornerRadius = 2.0f;
        float shadowOpacity = 0.3f;
        bool useGradients = true;
        bool useShadows = true;
        bool flatDesign = false;
        int touchTargetMinSize = 44;
        float touchPadding = 8.0f;
    } styling;

    // Drawing method variants for different platforms
    void drawMacOSButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                        const juce::Colour& colour, bool isPressed, bool isHighlighted) const;
    
    void drawWindowsButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                          const juce::Colour& colour, bool isPressed, bool isHighlighted) const;
    
    void drawLinuxButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                        const juce::Colour& colour, bool isPressed, bool isHighlighted) const;
    
    void drawiOSButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                      const juce::Colour& colour, bool isPressed, bool isHighlighted) const;
    
    void drawAndroidButton(juce::Graphics& g, const juce::Rectangle<int>& area,
                          const juce::Colour& colour, bool isPressed, bool isHighlighted) const;

    // Slider drawing variants
    void drawMacOSSlider(juce::Graphics& g, const juce::Rectangle<int>& trackArea,
                        const juce::Rectangle<int>& thumbArea, bool isHorizontal) const;
    
    void drawWindowsSlider(juce::Graphics& g, const juce::Rectangle<int>& trackArea,
                          const juce::Rectangle<int>& thumbArea, bool isHorizontal) const;
    
    void drawMobileSlider(juce::Graphics& g, const juce::Rectangle<int>& trackArea,
                         const juce::Rectangle<int>& thumbArea, bool isHorizontal) const;

    // Utility methods
    void updatePlatformStyling();
    juce::Colour getAdaptedColour(const juce::Colour& baseColour) const;
    juce::Font getAdaptedFont(const juce::Font& baseFont) const;
    void applyPlatformSpecificSettings();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlatformAwareLookAndFeel)
};

/**
 * Factory for creating platform-appropriate LookAndFeel instances
 */
class PlatformLookAndFeelFactory
{
public:
    static std::unique_ptr<PlatformAwareLookAndFeel> createForCurrentPlatform(
        FontManager& fontManager,
        ColorScheme& colorScheme,
        PlatformResponsiveManager& responsiveManager);
    
    static std::unique_ptr<PlatformAwareLookAndFeel> createForPlatform(
        PlatformResponsiveManager::TargetPlatform platform,
        FontManager& fontManager,
        ColorScheme& colorScheme,
        PlatformResponsiveManager& responsiveManager);

private:
    PlatformLookAndFeelFactory() = delete;
};