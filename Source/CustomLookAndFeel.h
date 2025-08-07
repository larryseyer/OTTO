#pragma once
#include <JuceHeader.h>
#include "ColorScheme.h"
#include "FontManager.h"
#include "INIConfig.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    enum ComponentType {
        NormalComponent,
        LargeComponent,
        SmallComponent,
        HeaderComponent,
        IconComponent,
        Icon,
        Player,
        Beat,
        DrumButton,  // Issue 5.2: New type for Row 5 drum buttons with 2x font size
        Label,
        Other
    };

    const juce::Image& getSplashImage() const { return splashImage; }
    const juce::Image& getPanKnobImage() const { return panKnobImage; }
    const juce::Image& getPan100Image() const { return pan100Image; }

    CustomLookAndFeel(FontManager& fontManager, ColorScheme& colorScheme);
    ~CustomLookAndFeel() override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                            const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted,
                            bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                       bool shouldDrawButtonAsHighlighted,
                       bool shouldDrawButtonAsDown) override;

    void drawComboBox(juce::Graphics& g, int width, int height,
                     bool isButtonDown, int buttonX, int buttonY,
                     int buttonW, int buttonH, juce::ComboBox& box) override;

    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float minSliderPos, float maxSliderPos,
                         const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    void drawLinearSliderThumb(juce::Graphics& g, int x, int y, int width, int height,
                              float sliderPos, float minSliderPos, float maxSliderPos,
                              const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, const float rotaryStartAngle,
                         const float rotaryEndAngle, juce::Slider& slider) override;

    void drawTextEditorOutline(juce::Graphics& g, int width, int height,
                              juce::TextEditor& textEditor) override;

    void fillTextEditorBackground(juce::Graphics& g, int width, int height,
                                 juce::TextEditor& textEditor) override;

    void drawTooltip(juce::Graphics& g, const juce::String& text,
                    int width, int height) override;

    void drawGroupComponentOutline(juce::Graphics& g, int w, int h,
                                  const juce::String& text,
                                  const juce::Justification& position,
                                  juce::GroupComponent& group) override;

    void drawProgressBar(juce::Graphics& g, juce::ProgressBar& progressBar,
                        int width, int height, double progress,
                        const juce::String& textToShow) override;

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                          bool isSeparator, bool isActive, bool isHighlighted,
                          bool isTicked, bool hasSubMenu, const juce::String& text,
                          const juce::String& shortcutKeyText, const juce::Drawable* icon,
                          const juce::Colour* textColour) override;

    void drawPopupMenuSectionHeader(juce::Graphics& g, const juce::Rectangle<int>& area,
                                   const juce::String& sectionName) override;

    void drawPopupMenuUpDownArrow(juce::Graphics& g, int width, int height,
                                 bool isScrollUpArrow) override;

    void drawHorizontalSeparator(juce::Graphics& g, int x, int y, int width, float thickness);
    void drawVerticalSeparator(juce::Graphics& g, int x, int y, int height, float thickness);

    juce::Rectangle<int> getTooltipBounds(const juce::String& tipText,
                                         juce::Point<int> screenPos,
                                         juce::Rectangle<int> parentArea) override;

    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override;

    juce::Font getLabelFont(juce::Label& label) override;
    juce::Font getTextButtonFont(juce::TextButton& button, int buttonHeight) override;
    juce::Font getComboBoxFont(juce::ComboBox& box) override;
    juce::Font getPopupMenuFont() override;
    juce::Font getAlertWindowMessageFont() override;
    juce::Font getAlertWindowTitleFont() override;

    void forceRefreshFromColorScheme();
    void repaintAll();
    ComponentType getComponentType(const juce::Component* component) const;

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    bool largeTextMode = false;
    bool highContrastMode = false;

protected:
    FontManager& fontManager;
    ColorScheme& colorScheme;

private:

    juce::Image buttonImage;
    juce::Image sliderImage;
    juce::Image splashImage;
    juce::Image panKnobImage;
    juce::Image pan100Image;

    void refreshColours();
    void syncWithColorScheme();
    void reloadImages();

    void drawButtonWithImage(juce::Graphics& g, juce::Button& button,
                            const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted,
                            bool shouldDrawButtonAsDown);

    void drawButtonFallback(juce::Graphics& g, juce::Button& button,
                           const juce::Colour& backgroundColour,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown);

    void drawIconButton(juce::Graphics& g, juce::TextButton& button, const juce::String& iconName,
                       bool isHighlighted, bool isDown, bool isToggled);

    void drawSliderWithImage(juce::Graphics& g, int x, int y, int width, int height,
                            float sliderPos, float minSliderPos, float maxSliderPos,
                            const juce::Slider::SliderStyle style, juce::Slider& slider);

    void drawSliderFallback(juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};
