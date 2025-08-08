/**
 * @file CustomLookAndFeel.cpp
 * @brief Implementation of OTTO's custom JUCE 8 Look and Feel system
 * 
 * This file implements the CustomLookAndFeel class which provides comprehensive
 * visual styling for all JUCE components in the OTTO application. It integrates
 * tightly with the ColorScheme and FontManager systems to ensure consistent
 * visual appearance across all interface elements.
 * 
 * DESIGN PHILOSOPHY:
 * ==================
 * - INI-Driven Consistency: All dimensions use INIConfig::LayoutConstants
 * - ColorScheme Integration: All colors sourced from ColorScheme.cpp
 * - FontManager Integration: All fonts sourced from FontManager.cpp  
 * - Responsive Design: Supports scaling and theme changes at runtime
 * - Modern JUCE 8: Uses latest JUCE rendering techniques and patterns
 * 
 * COMPONENT COVERAGE:
 * ===================
 * - Buttons: Custom styling with embedded images and hover effects
 * - Sliders: Rotary and linear sliders with custom thumbs and tracks
 * - PopupMenus: Themed menus with consistent colors and spacing
 * - Tooltips: Styled tooltips with gradients, shadows, and proper typography
 * - ComboBoxes: Custom dropdown styling matching overall theme
 * - Labels: Consistent text rendering with theme-appropriate colors
 * 
 * INTEGRATION POINTS:
 * ===================
 * - ColorScheme.cpp: Provides all color values via ColorRole enumeration
 * - FontManager.cpp: Provides all fonts via FontRole enumeration
 * - INIConfig.h: Provides all spacing, sizing, and layout constants
 * - BinaryData.h: Embedded images for buttons, sliders, and splash screen
 * - All UI Components: Apply this LookAndFeel for consistent appearance
 * 
 * @author OTTO Development Team
 * @version 2.0
 * @date 2024
 * @see ColorScheme.cpp for color management
 * @see FontManager.cpp for font management
 * @see INIConfig.h for layout constants
 */

#include "CustomLookAndFeel.h"
#include "INIConfig.h"
#include "UtilityComponents.h"
#include "BinaryData.h"

/**
 * @brief CustomLookAndFeel constructor - initialize styling with integrated systems
 * 
 * Sets up the custom look and feel by integrating with ColorScheme and FontManager
 * systems, loading embedded images, and configuring all JUCE component color IDs
 * for consistent theming throughout the application.
 * 
 * INITIALIZATION PROCESS:
 * 1. Store references to FontManager and ColorScheme systems
 * 2. Apply current color scheme to all JUCE component color IDs
 * 3. Configure popup menu styling with theme colors
 * 4. Set up tooltip styling with gradients and shadows
 * 5. Load embedded images from BinaryData for custom components
 * 
 * COLOR ROLE MAPPING:
 * - WindowBackground: Main background areas, popup backgrounds
 * - PrimaryText: Main text, tooltip text, menu text
 * - SecondaryText: Menu items, secondary labels
 * - ButtonBackgroundHover: Menu highlights, button hover states
 * - Separator: Borders, outlines, dividers
 * 
 * @param fontMgr Reference to FontManager for consistent typography
 * @param colorScheme Reference to ColorScheme for consistent colors
 * 
 * Called by: PluginEditor.cpp, MainContentComponent.cpp during component setup
 * References: ColorScheme.cpp color roles, INIConfig.h layout constants
 */
CustomLookAndFeel::CustomLookAndFeel(FontManager& fontMgr, ColorScheme& colorScheme)
    : fontManager(fontMgr), colorScheme(colorScheme) {

    // STEP 1: Apply complete color scheme to all standard JUCE component color IDs
    // This ensures buttons, labels, text editors, etc. all use consistent theming
    colorScheme.applyToLookAndFeel(*this);

    // STEP 2: Configure PopupMenu styling for dropdown menus and context menus
    // Used by: ComboBox dropdowns, right-click context menus, preset selection
    setColour(juce::PopupMenu::backgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
    setColour(juce::PopupMenu::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundHover));
    setColour(juce::PopupMenu::highlightedTextColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    setColour(juce::PopupMenu::headerTextColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));

    // STEP 3: Configure TooltipWindow styling for help text and parameter displays
    // Uses darker background for contrast and subtle shadow effects
    // Applied to: All component tooltips, parameter value displays
    setColour(juce::TooltipWindow::backgroundColourId, 
              colorScheme.getColor(ColorScheme::ColorRole::WindowBackground)
                         .darker(INIConfig::LayoutConstants::customLookFeelDarkerAmount));
    setColour(juce::TooltipWindow::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    setColour(juce::TooltipWindow::outlineColourId, colorScheme.getColor(ColorScheme::ColorRole::Separator));

    // STEP 4: Load embedded images from BinaryData for custom component rendering
    // These images provide professional visual elements without external file dependencies
    // Used by: Custom button rendering, slider thumb graphics, application branding
    buttonImage = juce::ImageCache::getFromMemory(BinaryData::Button100_png, BinaryData::Button100_pngSize);
    sliderImage = juce::ImageCache::getFromMemory(BinaryData::Slider100_png, BinaryData::Slider100_pngSize);
    splashImage = juce::ImageCache::getFromMemory(BinaryData::OTTO_Splash_Screen_png, BinaryData::OTTO_Splash_Screen_pngSize);
    panKnobImage = juce::ImageCache::getFromMemory(BinaryData::PanKnob_png, BinaryData::PanKnob_pngSize);
    pan100Image = juce::ImageCache::getFromMemory(BinaryData::Pan100_png, BinaryData::Pan100_pngSize);
}

/**
 * @brief CustomLookAndFeel destructor - cleanup resources
 * 
 * Default destructor as all resources are managed by JUCE's automatic
 * memory management system (ImageCache, ColorScheme references, etc.).
 */
CustomLookAndFeel::~CustomLookAndFeel() = default;

/**
 * @brief Draw custom styled tooltips with gradients, shadows, and proper typography
 * 
 * Renders tooltips with a professional appearance including gradient backgrounds,
 * drop shadows, rounded corners, and theme-appropriate colors. All styling
 * parameters are sourced from INIConfig constants for consistency.
 * 
 * VISUAL ELEMENTS:
 * - Gradient background: WindowBackground (darker to lighter)
 * - Rounded corners: customLookFeelCornerRadius from INIConfig
 * - Border: Separator color with customLookFeelBorderThickness
 * - Drop shadow: Black with alpha, configurable offset and radius
 * - Typography: FontManager body font, size based on largeTextMode
 * 
 * STYLING CONSTANTS USED:
 * - INIConfig::LayoutConstants::customLookFeelCornerRadius: Corner rounding
 * - INIConfig::LayoutConstants::customLookFeelShadowAlpha: Shadow transparency
 * - INIConfig::LayoutConstants::customLookFeelShadowRadius: Shadow blur radius
 * - INIConfig::LayoutConstants::customLookFeelShadowOffset[X/Y]: Shadow position
 * - INIConfig::LayoutConstants::fontSizeBody[Large]: Text size selection
 * 
 * @param g Graphics context for rendering
 * @param text Tooltip text content to display
 * @param width Tooltip width in pixels
 * @param height Tooltip height in pixels
 * 
 * Called by: JUCE framework when tooltips need rendering
 * References: ColorScheme.cpp for colors, FontManager.cpp for fonts, INIConfig.h for dimensions
 */
void CustomLookAndFeel::drawTooltip(juce::Graphics& g, const juce::String& text, int width, int height) {
    juce::Rectangle<float> bounds(0, 0, static_cast<float>(width), static_cast<float>(height));

    juce::ColourGradient gradient(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground).darker(INIConfig::LayoutConstants::customLookFeelDarkerAmount),
                                  bounds.getTopLeft(),
                                  colorScheme.getColor(ColorScheme::ColorRole::WindowBackground).darker(INIConfig::LayoutConstants::customLookFeelGradientDarkness),
                                  bounds.getBottomRight(),
                                  false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, static_cast<float>(INIConfig::LayoutConstants::customLookFeelCornerRadius));

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
    g.drawRoundedRectangle(bounds.reduced(INIConfig::LayoutConstants::customLookFeelBorderReduction), static_cast<float>(INIConfig::LayoutConstants::customLookFeelCornerRadius), INIConfig::LayoutConstants::customLookFeelBorderThickness);

    juce::DropShadow shadow(juce::Colours::black.withAlpha(INIConfig::LayoutConstants::customLookFeelShadowAlpha),
                           INIConfig::LayoutConstants::customLookFeelShadowRadius,
                           {INIConfig::LayoutConstants::customLookFeelShadowOffsetX, INIConfig::LayoutConstants::customLookFeelShadowOffsetY});
    shadow.drawForRectangle(g, bounds.toNearestInt());

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    g.setFont(fontManager.getFont(FontManager::FontRole::Body, largeTextMode ? INIConfig::LayoutConstants::fontSizeBodyLarge : INIConfig::LayoutConstants::fontSizeBody));

    juce::Rectangle<int> textArea(bounds.reduced(INIConfig::LayoutConstants::customLookFeelTooltipTextMargin).toNearestInt());
    g.drawFittedText(text, textArea, juce::Justification::centredLeft, static_cast<int>(INIConfig::LayoutConstants::customLookFeelTooltipLineSpacing));
}

juce::Rectangle<int> CustomLookAndFeel::getTooltipBounds(const juce::String& tipText,
                                                         juce::Point<int> screenPos,
                                                         juce::Rectangle<int> parentArea) {
    const int maxWidth = INIConfig::LayoutConstants::customLookFeelTooltipMaxWidth;
    const int margin = INIConfig::LayoutConstants::customLookFeelTooltipMargin;

    juce::Font font = fontManager.getFont(FontManager::FontRole::Body, largeTextMode ? INIConfig::LayoutConstants::fontSizeBodyLarge : INIConfig::LayoutConstants::fontSizeBody);

    juce::AttributedString attributedString;
    attributedString.setText(tipText);
    attributedString.setFont(font);

    juce::TextLayout textLayout;
    textLayout.createLayout(attributedString, static_cast<float>(maxWidth - margin * 2));

    int textWidth = static_cast<int>(textLayout.getWidth());
    int textHeight = static_cast<int>(textLayout.getHeight());

    int actualWidth = juce::jmin(textWidth + margin * 2, maxWidth);
    int actualHeight = textHeight + margin * 2;

    int x = screenPos.x + INIConfig::LayoutConstants::customLookFeelTooltipOffsetX;
    int y = screenPos.y + INIConfig::LayoutConstants::customLookFeelTooltipOffsetY;

    if (x + actualWidth > parentArea.getRight())
        x = screenPos.x - actualWidth - INIConfig::LayoutConstants::customLookFeelTooltipOffsetX;

    if (y + actualHeight > parentArea.getBottom())
        y = screenPos.y - actualHeight - INIConfig::LayoutConstants::customLookFeelTooltipOffsetX;

    return juce::Rectangle<int>(x, y, actualWidth, actualHeight);
}

void CustomLookAndFeel::repaintAll() {
    if (auto* topLevel = juce::Desktop::getInstance().getComponent(0)) {
        topLevel->repaint();
    }
}

juce::Slider::SliderLayout CustomLookAndFeel::getSliderLayout(juce::Slider& slider) {
    juce::Slider::SliderLayout layout;

    layout.sliderBounds = slider.getLocalBounds();

    if (slider.getTextBoxPosition() != juce::Slider::NoTextBox) {
        auto baseLayout = LookAndFeel_V4::getSliderLayout(slider);
        layout.textBoxBounds = baseLayout.textBoxBounds;

        if (slider.getTextBoxPosition() == juce::Slider::TextBoxBelow ||
            slider.getTextBoxPosition() == juce::Slider::TextBoxAbove) {
            int textBoxHeight = baseLayout.textBoxBounds.getHeight();
            if (slider.getTextBoxPosition() == juce::Slider::TextBoxBelow) {
                layout.sliderBounds = slider.getLocalBounds().removeFromTop(slider.getHeight() - textBoxHeight);
            } else {
                layout.sliderBounds = slider.getLocalBounds().removeFromBottom(slider.getHeight() - textBoxHeight);
            }
        } else if (slider.getTextBoxPosition() == juce::Slider::TextBoxLeft ||
                   slider.getTextBoxPosition() == juce::Slider::TextBoxRight) {
            int textBoxWidth = baseLayout.textBoxBounds.getWidth();
            if (slider.getTextBoxPosition() == juce::Slider::TextBoxRight) {
                layout.sliderBounds = slider.getLocalBounds().removeFromLeft(slider.getWidth() - textBoxWidth);
            } else {
                layout.sliderBounds = slider.getLocalBounds().removeFromRight(slider.getWidth() - textBoxWidth);
            }
        }
    }

    return layout;
}

CustomLookAndFeel::ComponentType CustomLookAndFeel::getComponentType(const juce::Component* component) const {
    juce::String componentID = component->getComponentID();

    if (auto* textButton = dynamic_cast<const juce::TextButton*>(component)) {
        juce::String buttonText = textButton->getButtonText();

        if (buttonText.isEmpty() && dynamic_cast<const PhosphorIconButton*>(component)) {
            return Icon;
        }

        // Issue 5.2: Identify Row 5 drum buttons for 2x font size
        if (componentID.startsWith("drum_button_")) {
            return DrumButton;
        }

        if (textButton->getButtonText().startsWith("PLAYER")) {
            return Player;
        }
        juce::String text = textButton->getButtonText();
        if (text.length() <= INIConfig::LayoutConstants::editableNumericMaxLength || text.isNotEmpty()) {
            return Beat;
        }
    }

    if (dynamic_cast<const juce::Label*>(component)) {
        return Label;
    }

    return Other;
}

juce::Font CustomLookAndFeel::getLabelFont(juce::Label& label) {
    juce::String componentID = label.getComponentID();
    float sizeMult = largeTextMode ? INIConfig::LayoutConstants::customLookFeelLargeTextMultiplier : 1.0f;

    if (componentID == "otto_label") {
        return fontManager.getFont(FontManager::FontRole::Brand, INIConfig::LayoutConstants::fontSizeBrand * sizeMult);
    }

    if (componentID == "version_label") {
        return fontManager.getFont(FontManager::FontRole::Version, INIConfig::LayoutConstants::fontSizeSmall * sizeMult);
    }

    if (componentID == "preset_display_label") {
        // Reduced preset label font size by 40% for better alignment with chevrons
        float fontSize = INIConfig::LayoutConstants::Row1::ottoHeight * INIConfig::LayoutConstants::fontSizePresetLabelReduced;
        return fontManager.getFont(FontManager::FontRole::Header, fontSize * sizeMult);
    }

    if (componentID == "bpm_label" || componentID == "rhythm_number" || componentID == "kit_number") {
        return fontManager.getFont(FontManager::FontRole::Numeric,
            componentID == "kit_number" ? INIConfig::LayoutConstants::fontSizeNumeric * 1.5f * sizeMult :
            INIConfig::LayoutConstants::fontSizeIconLarge * sizeMult);
    }

    if (componentID.contains("toggles") || componentID.contains("fills") ||
        componentID.contains("swing") || componentID.contains("energy") ||
        componentID.contains("volume")) {
        return fontManager.getFont(FontManager::FontRole::Header, INIConfig::LayoutConstants::fontSizeBody * sizeMult);
    }

    if (componentID == "player_number_display") {
        // Use large player number font - 2.5x the numeric size for visibility, with responsive scaling
        float playerNumberSize = INIConfig::LayoutConstants::fontSizeNumeric * 2.5f;  // 25% larger than double
        return fontManager.getFont(FontManager::FontRole::Numeric, playerNumberSize * sizeMult);
    }

    if (componentID == "drumkit_selected_label") {
        // DrumKit label uses same font pattern as preset label for consistency
        float fontSize = INIConfig::LayoutConstants::Row1::ottoHeight * INIConfig::LayoutConstants::fontSizePresetLabelReduced;
        return fontManager.getFont(FontManager::FontRole::Header, fontSize * sizeMult);
    }

    return fontManager.getFont(FontManager::FontRole::Body, INIConfig::LayoutConstants::fontSizeBody * sizeMult);
}

juce::Font CustomLookAndFeel::getTextButtonFont(juce::TextButton& button, int buttonHeight) {
    ComponentType type = getComponentType(&button);
    float sizeMult = largeTextMode ? INIConfig::LayoutConstants::customLookFeelLargeTextMultiplier : 1.0f;

#if JUCE_MAJOR_VERSION >= 8
    juce::Font font = juce::Font(juce::FontOptions());
#else
    juce::Font font = juce::Font();
#endif
    switch (type) {
        case Icon:
            font = fontManager.getFont(FontManager::FontRole::Icon, INIConfig::LayoutConstants::fontSizeIcon * sizeMult);
            break;
        case Player:
            // Issue 2.2: Font Size Should Be Twice as Large for Row 2 player tabs
            font = fontManager.getFont(FontManager::FontRole::Button, INIConfig::LayoutConstants::fontSizeButton * 2.0f * sizeMult);
            break;
        case DrumButton:
            // Issue 5.2: Font Size Should Be Twice as Large for Row 5 drum buttons
            font = fontManager.getFont(FontManager::FontRole::Button, INIConfig::LayoutConstants::fontSizeButton * INIConfig::LayoutConstants::Row5::drumButtonFontMultiplier * sizeMult);
            break;
        case Beat:
            font = fontManager.getFont(FontManager::FontRole::Button,
                                     juce::jlimit(INIConfig::LayoutConstants::customLookFeelFontScaleMin, INIConfig::LayoutConstants::fontSizeBody, buttonHeight * INIConfig::LayoutConstants::customLookFeelFontScaleFactor) * sizeMult);
            break;
        default:
            font = fontManager.getFont(FontManager::FontRole::Button,
                                     juce::jlimit(INIConfig::LayoutConstants::customLookFeelFontScaleMin, INIConfig::LayoutConstants::customLookFeelFontScaleMax, buttonHeight * INIConfig::LayoutConstants::customLookFeelFontScaleFactor) * sizeMult);
    }

    if (font.getTypefaceName().isEmpty()) {

#if JUCE_MAJOR_VERSION >= 8
        font = juce::Font(juce::FontOptions().withHeight(sizeMult * INIConfig::LayoutConstants::fontSizeButton));
#else
        font = juce::Font(sizeMult * INIConfig::LayoutConstants::fontSizeButton);
#endif
    }

    return font;
}

juce::Font CustomLookAndFeel::getComboBoxFont(juce::ComboBox& comboBox) {
    // Check if this is the preset menu by component ID
    if (comboBox.getComponentID() == "presets_menu") {
        return fontManager.getFont(FontManager::FontRole::Header, INIConfig::LayoutConstants::fontSizePresetMenuReduced);
    }
    
    // Check if this is the drumkit dropdown - use same styling as preset menu
    if (comboBox.getComponentID() == "drumkit_dropdown") {
        return fontManager.getFont(FontManager::FontRole::Header, INIConfig::LayoutConstants::fontSizePresetMenuReduced);
    }
    
    return fontManager.getFont(FontManager::FontRole::Body, INIConfig::LayoutConstants::fontSizeBody);
}

juce::Font CustomLookAndFeel::getPopupMenuFont() {
    // Use Header font (Playfair Display) for popup menus to match preset ComboBox
    return fontManager.getFont(FontManager::FontRole::Header, INIConfig::LayoutConstants::fontSizePresetMenuReduced);
}

juce::Font CustomLookAndFeel::getAlertWindowMessageFont() {
    return fontManager.getFont(FontManager::FontRole::Body, INIConfig::LayoutConstants::fontSizeBody);
}

juce::Font CustomLookAndFeel::getAlertWindowTitleFont() {
    return fontManager.getFont(FontManager::FontRole::Header, INIConfig::LayoutConstants::fontSizeHeader);
}

void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                            const juce::Colour& backgroundColour,
                                            bool shouldDrawButtonAsHighlighted,
                                            bool shouldDrawButtonAsDown) {
    juce::ignoreUnused(backgroundColour);

    auto bounds = button.getLocalBounds().toFloat();
    if (bounds.getWidth() <= 0.0f || bounds.getHeight() <= 0.0f) return;

    ComponentType type = getComponentType(&button);
    bool isToggled = button.getToggleState();

    if (type == Icon) {
        return;
    }

    if (highContrastMode) {
        if (shouldDrawButtonAsDown || isToggled) {
            g.setColour(juce::Colours::white);
            g.fillRoundedRectangle(bounds, fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelSliderCornerRadius);
            g.setColour(juce::Colours::black);
            g.drawRoundedRectangle(bounds, fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelSliderCornerRadius, INIConfig::LayoutConstants::customLookFeelHighContrastBorderWidth);
        } else {
            g.setColour(juce::Colours::black);
            g.fillRoundedRectangle(bounds, fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelSliderCornerRadius);
            g.setColour(juce::Colours::white);
            g.drawRoundedRectangle(bounds, fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelSliderCornerRadius, INIConfig::LayoutConstants::customLookFeelHighContrastBorderWidth);
        }
        return;
    }

    if (type == Beat || type == Player || type == DrumButton) {
        if (!buttonImage.isNull()) {
            drawButtonWithImage(g, button, backgroundColour, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        } else {
            drawButtonFallback(g, button, backgroundColour, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        }
    } else {
        if (shouldDrawButtonAsDown) {
            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundPressed));
        } else if (shouldDrawButtonAsHighlighted) {
            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundHover));
        } else {
            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        }
        g.fillRoundedRectangle(bounds, fontManager.getScaleFactor() * INIConfig::LayoutConstants::cornerRadiusLarge);
    }
}

void CustomLookAndFeel::drawButtonWithImage(juce::Graphics& g, juce::Button& button,
                                           const juce::Colour& backgroundColour,
                                           bool shouldDrawButtonAsHighlighted,
                                           bool shouldDrawButtonAsDown) {
    juce::ignoreUnused(backgroundColour);

    auto bounds = button.getLocalBounds().toFloat();
    bool isToggled = button.getToggleState();

    int sliceIndex = 0;
    if (shouldDrawButtonAsDown) {
        sliceIndex = isToggled ? 3 : 2;
    } else if (shouldDrawButtonAsHighlighted) {
        sliceIndex = isToggled ? 5 : 4;
    } else {
        sliceIndex = isToggled ? 1 : 0;
    }

    int totalHeight = buttonImage.getHeight();
    if (totalHeight >= INIConfig::LayoutConstants::customLookFeelButtonImageSlices) {
        int sliceHeight = totalHeight / INIConfig::LayoutConstants::customLookFeelButtonImageSlices;
        int sourceY = sliceIndex * sliceHeight;

        if (sourceY >= 0 && sourceY + sliceHeight <= totalHeight) {
            juce::Image sliceImage = buttonImage.getClippedImage(
                juce::Rectangle<int>(0, sourceY, buttonImage.getWidth(), sliceHeight));

            if (!sliceImage.isNull()) {
                auto scaledImage = sliceImage.rescaled(
                    static_cast<int>(bounds.getWidth()),
                    static_cast<int>(bounds.getHeight()),
                    juce::Graphics::highResamplingQuality);

                if (!scaledImage.isNull()) {
                    g.drawImageAt(scaledImage, static_cast<int>(bounds.getX()), static_cast<int>(bounds.getY()));
                }
            }
        }
    }
}

void CustomLookAndFeel::drawButtonFallback(juce::Graphics& g, juce::Button& button,
                                          const juce::Colour& backgroundColour,
                                          bool shouldDrawButtonAsHighlighted,
                                          bool shouldDrawButtonAsDown) {
    juce::ignoreUnused(backgroundColour);

    auto bounds = button.getLocalBounds().toFloat();
    bool isToggled = button.getToggleState();

    juce::Colour baseColor = colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground);

    if (shouldDrawButtonAsDown) {
        baseColor = colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundPressed);
    } else if (shouldDrawButtonAsHighlighted) {
        baseColor = colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundHover);
    }

    if (isToggled) {
        baseColor = colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundToggled);
    }

    g.setColour(baseColor);
    g.fillRoundedRectangle(bounds, fontManager.getScaleFactor() * INIConfig::LayoutConstants::cornerRadiusLarge);

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
    g.drawRoundedRectangle(bounds.reduced(INIConfig::LayoutConstants::customLookFeelBorderReduction), fontManager.getScaleFactor() * INIConfig::LayoutConstants::cornerRadiusLarge, INIConfig::LayoutConstants::customLookFeelBorderThickness);
}

void CustomLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                       bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
    ComponentType type = getComponentType(&button);
    juce::String buttonText = button.getButtonText();

    if (type == Icon) {
        if (auto* iconButton = dynamic_cast<PhosphorIconButton*>(&button)) {
            drawIconButton(g, button, iconButton->getIconName(),
                          shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown, button.getToggleState());
        }
        return;
    } else {
        if (highContrastMode) {
            if (shouldDrawButtonAsDown || button.getToggleState()) {
                g.setColour(juce::Colours::black);
            } else {
                g.setColour(juce::Colours::white);
            }
        } else {
            if (type == Player) {
                auto bounds = button.getLocalBounds();
                const int margin = INIConfig::LayoutConstants::customLookFeelButtonTextMargin;
                auto textBounds = bounds.reduced(margin);

                juce::Font font = getTextButtonFont(button, button.getHeight());
                g.setFont(font);

                g.setColour(juce::Colours::black.withAlpha(INIConfig::LayoutConstants::customLookFeelShadowAlpha));
                g.drawFittedText(buttonText, textBounds.translated(1, 1), juce::Justification::centred, 1);

                g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
                g.drawFittedText(buttonText, textBounds, juce::Justification::centred, 1);
                return;
            } else {
                g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
            }
        }

        auto bounds = button.getLocalBounds();
        const int margin = INIConfig::LayoutConstants::customLookFeelButtonTextMargin;
        auto textBounds = bounds.reduced(margin);

        juce::Font font = getTextButtonFont(button, button.getHeight());

        juce::GlyphArrangement glyphs;
        glyphs.addFittedText(font, buttonText, 0, 0, static_cast<float>(textBounds.getWidth()),
                            font.getHeight(), juce::Justification::centred, 1);

        float textWidth = glyphs.getBoundingBox(0, -1, true).getWidth();
        if (textWidth > textBounds.getWidth() && textBounds.getWidth() > 0) {
            float scale = textBounds.getWidth() / textWidth;
#if JUCE_MAJOR_VERSION >= 8
            font = juce::Font(juce::FontOptions().withHeight(font.getHeight() * scale * INIConfig::LayoutConstants::customLookFeelTextScaleFactor));
#else
            font = juce::Font(font.getHeight() * scale * INIConfig::LayoutConstants::customLookFeelTextScaleFactor);
#endif
        }

        g.setFont(font);
        g.drawFittedText(buttonText, textBounds, juce::Justification::centred, 1);
    }
}

void CustomLookAndFeel::drawIconButton(juce::Graphics& g, juce::TextButton& button, const juce::String& iconName,
                                      bool isHighlighted, bool isDown, bool isToggled) {
    juce::Colour iconColor;

    if (isToggled) {
        iconColor = colorScheme.getColor(ColorScheme::ColorRole::PrimaryText);
    } else {
        iconColor = colorScheme.getColor(ColorScheme::ColorRole::SecondaryText);
    }

    if (isDown) {
        iconColor = iconColor.darker(INIConfig::LayoutConstants::customLookFeelDarkerAmount);
    }
    else if (isHighlighted && !isToggled) {
        iconColor = iconColor.brighter(INIConfig::LayoutConstants::customLookFeelBrighterAmount);
    }

    g.setColour(iconColor);

    juce::String iconChar = fontManager.getIconString(iconName);

    // EUREKA IMPLEMENTATION: Icons size to configurable % of their ACTUAL box area
    // No double-scaling - the box handles responsive scaling, icon just fits the box perfectly
    float actualBoxHeight = static_cast<float>(button.getHeight());
    float actualBoxWidth = static_cast<float>(button.getWidth());
    float boxFitRatio = INIConfig::clampPhosphorIconBoxFitRatio(INIConfig::LayoutConstants::phosphorIconButtonBoxFitRatio);
    float desiredIconSize = juce::jmin(actualBoxHeight, actualBoxWidth) * boxFitRatio;
    
    // Compensate for FontManager's scaling - divide by scale factor so final result is correct
    float finalFontSize = desiredIconSize / fontManager.getScaleFactor();
    
    juce::Font iconFont = fontManager.getPhosphorFont(fontManager.getPhosphorWeight(), finalFontSize);
    g.setFont(iconFont);
    g.drawText(iconChar, button.getLocalBounds(), juce::Justification::centred);
}

void CustomLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                    int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& comboBox) {
    juce::ignoreUnused(isButtonDown, buttonX, buttonY, buttonW, buttonH, comboBox);

    g.setColour(juce::Colours::transparentBlack);
    g.fillRoundedRectangle(juce::Rectangle<float>(0, 0, static_cast<float>(width), static_cast<float>(height)), 0.0f);
}

void CustomLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height) {
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
    g.fillRoundedRectangle(juce::Rectangle<float>(0, 0, static_cast<float>(width), static_cast<float>(height)),
                          fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelSliderCornerRadius);

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
    g.drawRoundedRectangle(juce::Rectangle<float>(0, 0, static_cast<float>(width), static_cast<float>(height)),
                          fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelSliderCornerRadius, fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelBorderThickness);
}

void CustomLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                         bool isSeparator, bool isActive, bool isHighlighted,
                                         bool isTicked, bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText,
                                         const juce::Drawable* icon, const juce::Colour* textColour) {
    juce::ignoreUnused(isTicked, hasSubMenu, shortcutKeyText, icon, textColour);

    if (isSeparator) {
        auto r = area.reduced(static_cast<int>(INIConfig::LayoutConstants::customLookFeelSeparatorMargin), 0).removeFromTop(1);
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
        g.fillRect(r);
    } else {
        juce::Colour textColourToUse = colorScheme.getColor(ColorScheme::ColorRole::SecondaryText);
        if (isHighlighted) {
            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundHover));
            g.fillRect(area);
            textColourToUse = colorScheme.getColor(ColorScheme::ColorRole::PrimaryText);
        }
        if (!isActive) textColourToUse = textColourToUse.withAlpha(INIConfig::LayoutConstants::alphaTransparent);

        g.setColour(textColourToUse);
        g.setFont(getPopupMenuFont());

        juce::String displayText = text;
        if (text.contains(".sfz")) {
            displayText = juce::File(text).getFileNameWithoutExtension();
        }

        g.drawFittedText(displayText, area.reduced(INIConfig::LayoutConstants::customLookFeelComboBoxMargin, 0), juce::Justification::centredLeft, 1);
    }
}

void CustomLookAndFeel::drawPopupMenuSectionHeader(juce::Graphics& g, const juce::Rectangle<int>& area,
                                                   const juce::String& sectionName) {
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    g.setFont(fontManager.getFont(FontManager::FontRole::Header, INIConfig::LayoutConstants::fontSizeBody));
    g.drawFittedText(sectionName, area.reduced(INIConfig::LayoutConstants::cornerRadiusLarge, INIConfig::LayoutConstants::customLookFeelTextReduction), juce::Justification::centredLeft, 1);
}

void CustomLookAndFeel::drawPopupMenuUpDownArrow(juce::Graphics& g, int width, int height,
                                                 bool isScrollUpArrow) {
    const float arrowSize = juce::jmin(width, height) * INIConfig::LayoutConstants::alphaTransparent;
    const float centreX = width * INIConfig::LayoutConstants::alphaDisabled;
    const float centreY = height * INIConfig::LayoutConstants::alphaDisabled;

    juce::Path arrow;
    arrow.addTriangle(centreX - arrowSize * INIConfig::LayoutConstants::alphaDisabled, centreY + (isScrollUpArrow ? arrowSize * INIConfig::LayoutConstants::alphaDisabled : -arrowSize * INIConfig::LayoutConstants::alphaDisabled),
                     centreX + arrowSize * INIConfig::LayoutConstants::alphaDisabled, centreY + (isScrollUpArrow ? arrowSize * INIConfig::LayoutConstants::alphaDisabled : -arrowSize * INIConfig::LayoutConstants::alphaDisabled),
                     centreX, centreY + (isScrollUpArrow ? -arrowSize * INIConfig::LayoutConstants::alphaDisabled : arrowSize * INIConfig::LayoutConstants::alphaDisabled));

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    g.fillPath(arrow);
}

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPos, float minSliderPos, float maxSliderPos,
                                        const juce::Slider::SliderStyle style, juce::Slider& slider) {

    if (width <= 0 || height <= 0) {
        return;
    }

    if (style == juce::Slider::LinearVertical) {
        float range = maxSliderPos - minSliderPos;
        if (range <= 0.0f) {
            float normalizedValue = (float)(slider.getValue() - slider.getMinimum()) /
                                  (float)(slider.getMaximum() - slider.getMinimum());
            minSliderPos = static_cast<float>(y);
            maxSliderPos = static_cast<float>(y + height);
            sliderPos = maxSliderPos - normalizedValue * height;
        }
    } else {
        float range = maxSliderPos - minSliderPos;
        if (range <= 0.0f) {
            float normalizedValue = (float)(slider.getValue() - slider.getMinimum()) /
                                  (float)(slider.getMaximum() - slider.getMinimum());
            minSliderPos = static_cast<float>(x);
            maxSliderPos = static_cast<float>(x + width);
            sliderPos = minSliderPos + normalizedValue * width;
        }
    }

    sliderPos = juce::jlimit(minSliderPos, maxSliderPos, sliderPos);

    // Use Slider100 image for both vertical and horizontal faders
    bool hasValidImage = (!sliderImage.isNull() && sliderImage.isValid());

    if (hasValidImage) {
        drawSliderWithImage(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    } else {
        drawSliderFallback(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}

void CustomLookAndFeel::drawLinearSliderThumb(juce::Graphics& g, int x, int y, int width, int height,
                                             float sliderPos, float minSliderPos, float maxSliderPos,
                                             const juce::Slider::SliderStyle style, juce::Slider& slider) {
    juce::ignoreUnused(slider);

    if (width <= 0 || height <= 0 || maxSliderPos <= minSliderPos) return;

    sliderPos = juce::jlimit(minSliderPos, maxSliderPos, sliderPos);

    auto normalizedPos = (sliderPos - minSliderPos) / (maxSliderPos - minSliderPos);
    auto thumbSize = fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelThumbSize;

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SliderThumb));

    if (style == juce::Slider::LinearVertical) {
        auto thumbY = y + height * normalizedPos;
        g.fillRoundedRectangle(x + width * INIConfig::LayoutConstants::alphaDisabled - thumbSize/INIConfig::LayoutConstants::customLookFeelThumbDivisor, thumbY - thumbSize/INIConfig::LayoutConstants::customLookFeelThumbDivisor, thumbSize, thumbSize, INIConfig::LayoutConstants::customLookFeelSliderCornerRadius);
    } else {
        auto thumbX = x + width * normalizedPos;
        g.fillRoundedRectangle(thumbX - thumbSize/INIConfig::LayoutConstants::customLookFeelThumbDivisor, y + height * INIConfig::LayoutConstants::alphaDisabled - thumbSize/INIConfig::LayoutConstants::customLookFeelThumbDivisor, thumbSize, thumbSize, INIConfig::LayoutConstants::customLookFeelSliderCornerRadius);
    }
}

void CustomLookAndFeel::drawSliderWithImage(juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float minSliderPos, float maxSliderPos,
                                           const juce::Slider::SliderStyle style, juce::Slider& slider) {
    juce::ignoreUnused(slider);

    // Use Slider100 image for both vertical and horizontal faders
    if (sliderImage.isNull() || !sliderImage.isValid() || sliderImage.getHeight() <= 0) {
        drawSliderFallback(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        return;
    }

    juce::Image imageToUse = sliderImage;

    float normalizedPos = (sliderPos - minSliderPos) / (maxSliderPos - minSliderPos);
    normalizedPos = juce::jlimit(0.0f, 1.0f, normalizedPos);

    // For vertical sliders, invert the normalized position to match expected behavior
    // (higher values should show higher image slices)
    if (style == juce::Slider::LinearVertical) {
        normalizedPos = 1.0f - normalizedPos;
    }

    int totalSlices = INIConfig::LayoutConstants::customLookFeelSliderImageSlices;
    int sliceHeight = imageToUse.getHeight() / totalSlices;

    if (sliceHeight <= 0) {
        drawSliderFallback(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        return;
    }

    int sliceIndex = static_cast<int>(normalizedPos * (totalSlices - 1));

    sliceIndex = juce::jlimit(0, totalSlices - 1, sliceIndex);

    int sourceY = sliceIndex * sliceHeight;

    if (sourceY + sliceHeight > imageToUse.getHeight()) {
        sourceY = imageToUse.getHeight() - sliceHeight;
    }

    juce::Rectangle<int> sourceRect(0, sourceY, imageToUse.getWidth(), sliceHeight);

    try {
        juce::Image sliceImage = imageToUse.getClippedImage(sourceRect);

        if (sliceImage.isValid() && !sliceImage.isNull()) {
            if (style == juce::Slider::LinearHorizontal) {
                int rotatedWidth = sliceImage.getHeight();
                int rotatedHeight = sliceImage.getWidth();

                juce::Image rotatedImage(juce::Image::ARGB, rotatedWidth, rotatedHeight, true);
                juce::Graphics rotG(rotatedImage);

                rotG.fillAll(juce::Colours::transparentBlack);

                juce::AffineTransform rotation = juce::AffineTransform::rotation(
                    juce::MathConstants<float>::halfPi,
                    static_cast<float>(sliceImage.getWidth()) * INIConfig::LayoutConstants::alphaDisabled,
                    static_cast<float>(sliceImage.getHeight()) * INIConfig::LayoutConstants::alphaDisabled);

                rotation = rotation.translated(
                    static_cast<float>(rotatedWidth - sliceImage.getWidth()) * INIConfig::LayoutConstants::alphaDisabled,
                    static_cast<float>(rotatedHeight - sliceImage.getHeight()) * INIConfig::LayoutConstants::alphaDisabled);

                rotG.drawImageTransformed(sliceImage, rotation);

                // Reduce fader image size by 50%
                int scaledWidth = static_cast<int>(width * 0.5f);
                int scaledHeight = static_cast<int>(height * 0.5f);
                
                auto scaledImage = rotatedImage.rescaled(scaledWidth, scaledHeight, juce::Graphics::highResamplingQuality);

                if (scaledImage.isValid()) {
                    // Center the smaller fader image within the original bounds
                    int centeredX = x + (width - scaledWidth) / 2;
                    int centeredY = y + (height - scaledHeight) / 2;
                    g.drawImageAt(scaledImage, centeredX, centeredY);
                } else {
                    drawSliderFallback(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
                }
            } else {
                // Reduce fader image size by 50%
                int scaledWidth = static_cast<int>(width * 0.5f);
                int scaledHeight = static_cast<int>(height * 0.5f);
                
                auto scaledImage = sliceImage.rescaled(scaledWidth, scaledHeight, juce::Graphics::highResamplingQuality);
                if (scaledImage.isValid()) {
                    // Center the smaller fader image within the original bounds
                    int centeredX = x + (width - scaledWidth) / 2;
                    int centeredY = y + (height - scaledHeight) / 2;
                    g.drawImageAt(scaledImage, centeredX, centeredY);
                } else {
                    drawSliderFallback(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
                }
            }
        } else {
            drawSliderFallback(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        }
    } catch (...) {
        drawSliderFallback(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}

void CustomLookAndFeel::drawSliderFallback(juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float minSliderPos, float maxSliderPos,
                                          const juce::Slider::SliderStyle style, juce::Slider& slider) {
    juce::ignoreUnused(slider);

    auto trackBounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                            static_cast<float>(width), static_cast<float>(height));

    if (style == juce::Slider::LinearVertical) {
        auto trackWidth = fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelTrackThickness;
        auto trackRect = trackBounds.withWidth(trackWidth).withX(trackBounds.getCentreX() - trackWidth / INIConfig::LayoutConstants::customLookFeelThumbDivisor);

        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SliderTrack));
        g.fillRoundedRectangle(trackRect, fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelSliderCornerRadius);

        auto normalizedPos = (sliderPos - minSliderPos) / (maxSliderPos - minSliderPos);
        auto thumbY = trackRect.getY() + trackRect.getHeight() * normalizedPos;
        auto filledRect = trackRect.withTop(thumbY);
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Accent));
        g.fillRoundedRectangle(filledRect, fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelSliderCornerRadius);

        auto thumbSize = fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelThumbSize;
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SliderThumb));
        g.fillRoundedRectangle(trackRect.getCentreX() - thumbSize/INIConfig::LayoutConstants::customLookFeelThumbDivisor, thumbY - thumbSize/INIConfig::LayoutConstants::customLookFeelThumbDivisor, thumbSize, thumbSize, INIConfig::LayoutConstants::customLookFeelSliderCornerRadius);
    } else {
        auto trackHeight = fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelTrackThickness;
        auto trackRect = trackBounds.withHeight(trackHeight).withY(trackBounds.getCentreY() - trackHeight / INIConfig::LayoutConstants::customLookFeelThumbDivisor);

        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SliderTrack));
        g.fillRoundedRectangle(trackRect, fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelSliderCornerRadius);

        auto normalizedPos = (sliderPos - minSliderPos) / (maxSliderPos - minSliderPos);
        auto thumbX = trackRect.getX() + trackRect.getWidth() * normalizedPos;
        auto filledRect = trackRect.withRight(thumbX);
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Accent));
        g.fillRoundedRectangle(filledRect, fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelSliderCornerRadius);

        auto thumbSize = fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelThumbSize;
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SliderThumb));
        g.fillRoundedRectangle(thumbX - thumbSize/INIConfig::LayoutConstants::customLookFeelThumbDivisor, trackRect.getCentreY() - thumbSize/INIConfig::LayoutConstants::customLookFeelThumbDivisor, thumbSize, thumbSize, INIConfig::LayoutConstants::customLookFeelSliderCornerRadius);
    }
}

void CustomLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label) {
    if (!label.isBeingEdited()) {
        auto alpha = label.isEnabled() ? 1.0f : INIConfig::LayoutConstants::alphaDisabled;
        auto font = getLabelFont(label);

        g.setColour(label.findColour(juce::Label::textColourId).withMultipliedAlpha(alpha));
        g.setFont(font);

        auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                        juce::jmax(1, static_cast<int>(textArea.getHeight() / font.getHeight())),
                        label.getMinimumHorizontalScale());
    } else {
        if (auto* editor = label.getCurrentTextEditor())
        {
            editor->setColour(juce::TextEditor::backgroundColourId,
                            colorScheme.getColor(ColorScheme::ColorRole::ControlBackground));
            editor->setColour(juce::TextEditor::textColourId,
                            colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
            editor->setColour(juce::TextEditor::focusedOutlineColourId,
                            colorScheme.getColor(ColorScheme::ColorRole::Accent));
            editor->setFont(getLabelFont(label));
        }
    }
}

void CustomLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height,
                                              juce::TextEditor& textEditor) {
    if (textEditor.hasKeyboardFocus(true)) {
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Accent));
        g.drawRoundedRectangle(0.5f, 0.5f, width - 1.0f, height - 1.0f,
                              INIConfig::LayoutConstants::cornerRadiusSmall, 2.0f);
    } else {
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
        g.drawRoundedRectangle(0.5f, 0.5f, width - 1.0f, height - 1.0f,
                              INIConfig::LayoutConstants::cornerRadiusSmall, 1.0f);
    }
}

void CustomLookAndFeel::fillTextEditorBackground(juce::Graphics& g, int width, int height,
                                                 juce::TextEditor& textEditor) {
    juce::ignoreUnused(textEditor);
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ControlBackground));
    g.fillRoundedRectangle(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height),
                          INIConfig::LayoutConstants::cornerRadiusSmall);
}

void CustomLookAndFeel::drawGroupComponentOutline(juce::Graphics& g, int w, int h,
                                                 const juce::String& text,
                                                 const juce::Justification& position,
                                                 juce::GroupComponent& group) {
    juce::ignoreUnused(position, group);

    const float textH = 20.0f;
    const float indent = 10.0f;
    const float cornerSize = 5.0f;

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));

    juce::Path outline;
    if (text.isNotEmpty()) {
        juce::Font font = fontManager.getFont(FontManager::FontRole::Header, 14.0f);

        juce::AttributedString attrStr;
        attrStr.setText(text);
        attrStr.setFont(font);

        juce::TextLayout textLayout;
        textLayout.createLayout(attrStr, 1000.0f);

        float textWidth = textLayout.getWidth();
        const float textW = juce::jlimit(50.0f, 200.0f, textWidth + 20.0f);

        outline.startNewSubPath(indent + textW, textH * 0.5f);
        outline.lineTo(w - cornerSize, textH * 0.5f);
        outline.addArc(w - cornerSize * 2.0f, textH * 0.5f, cornerSize * 2.0f, cornerSize * 2.0f,
                      0, juce::MathConstants<float>::halfPi);
        outline.lineTo(w - 0.5f, h - cornerSize);
        outline.addArc(w - cornerSize * 2.0f, h - cornerSize * 2.0f, cornerSize * 2.0f, cornerSize * 2.0f,
                      0, juce::MathConstants<float>::halfPi);
        outline.lineTo(cornerSize, h - 0.5f);
        outline.addArc(0.5f, h - cornerSize * 2.0f, cornerSize * 2.0f, cornerSize * 2.0f,
                      juce::MathConstants<float>::halfPi, juce::MathConstants<float>::halfPi);
        outline.lineTo(0.5f, cornerSize + textH * 0.5f);
        outline.addArc(0.5f, textH * 0.5f, cornerSize * 2.0f, cornerSize * 2.0f,
                      juce::MathConstants<float>::pi, juce::MathConstants<float>::halfPi);
        outline.lineTo(indent, textH * 0.5f);

        g.strokePath(outline, juce::PathStrokeType(1.0f));

        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
        g.fillRect(static_cast<int>(indent), 0, static_cast<int>(textW), static_cast<int>(textH));

        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
        g.setFont(font);
        g.drawText(text, static_cast<int>(indent), 0, static_cast<int>(textW), static_cast<int>(textH),
                  juce::Justification::centred);
    } else {
        g.drawRoundedRectangle(0.5f, 0.5f, w - 1.0f, h - 1.0f, cornerSize, 1.0f);
    }
}

void CustomLookAndFeel::drawProgressBar(juce::Graphics& g, juce::ProgressBar& progressBar,
                                       int width, int height, double progress,
                                       const juce::String& textToShow) {
    juce::ignoreUnused(progressBar);

    auto bounds = juce::Rectangle<float>(0, 0, static_cast<float>(width), static_cast<float>(height));
    const float cornerSize = INIConfig::LayoutConstants::cornerRadiusSmall;

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SliderTrack));
    g.fillRoundedRectangle(bounds, cornerSize);

    if (progress > 0.0) {
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Accent));
        g.fillRoundedRectangle(bounds.withWidth(static_cast<float>(progress * width)), cornerSize);
    }

    if (textToShow.isNotEmpty()) {
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
        g.setFont(fontManager.getFont(FontManager::FontRole::Body, 12.0f));
        g.drawText(textToShow, bounds.toNearestInt(), juce::Justification::centred);
    }
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPosProportional, float rotaryStartAngle,
                                        float rotaryEndAngle, juce::Slider& slider) {
    juce::ignoreUnused(slider);

    auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                       static_cast<float>(width), static_cast<float>(height));
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    if (sliderImage.isValid()) {
        auto knobSize = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.8f;
        
        auto rotation = juce::AffineTransform::rotation(angle + juce::MathConstants<float>::halfPi, centreX, centreY);
        
        auto imageScale = knobSize / static_cast<float>(juce::jmax(sliderImage.getWidth(), sliderImage.getHeight()));
        auto transform = juce::AffineTransform::scale(imageScale, imageScale, centreX, centreY)
                        .followedBy(rotation);
        
        g.drawImageTransformed(sliderImage, transform);
    } else {
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / INIConfig::LayoutConstants::customLookFeelThumbDivisor;
   auto rx = centreX - radius;
   auto ry = centreY - radius;
   auto rw = radius * INIConfig::LayoutConstants::customLookFeelThumbDivisor;

   g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground).darker());
   g.fillEllipse(rx, ry, rw, rw);

   g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
   g.drawEllipse(rx, ry, rw, rw, fontManager.getScaleFactor());

   juce::Path valuePath;
   valuePath.addPieSegment(rx, ry, rw, rw, rotaryStartAngle, angle, INIConfig::LayoutConstants::customLookFeelRotaryInnerRadius);
   g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Accent));
   g.fillPath(valuePath);

   juce::Path pointer;
   auto pointerLength = radius * INIConfig::LayoutConstants::customLookFeelRotaryPointerLength;
   auto pointerThickness = fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelRotaryThickness;
   pointer.addRectangle(-pointerThickness * INIConfig::LayoutConstants::alphaDisabled, -radius, pointerThickness, pointerLength);
   pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

   g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
   g.fillPath(pointer);

   auto dotSize = fontManager.getScaleFactor() * INIConfig::LayoutConstants::customLookFeelRotaryDotSize;
   g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
   g.fillEllipse(centreX - dotSize * INIConfig::LayoutConstants::alphaDisabled, centreY - dotSize * INIConfig::LayoutConstants::alphaDisabled, dotSize, dotSize);
    }
}

void CustomLookAndFeel::drawHorizontalSeparator(juce::Graphics& g, int x, int y, int width, float thickness) {
   g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
   g.fillRect(static_cast<float>(x), static_cast<float>(y),
              static_cast<float>(width), thickness * fontManager.getScaleFactor());
}

void CustomLookAndFeel::drawVerticalSeparator(juce::Graphics& g, int x, int y, int height, float thickness) {
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
    g.fillRect(static_cast<float>(x), static_cast<float>(y),
               thickness * fontManager.getScaleFactor(), static_cast<float>(height));
}

void CustomLookAndFeel::saveStates(ComponentState& state) {
   if (colorScheme.getCurrentTheme() == 0) {
       state.themeSettings.themeID = 1;
       state.themeSettings.themeName = "Dark";
   } else {
       state.themeSettings.themeID = 2;
       state.themeSettings.themeName = "Light";
   }

   state.themeSettings.fontMainSize = fontManager.getDefaultSize(FontManager::FontRole::Body);
   state.themeSettings.fontSecondarySize = fontManager.getDefaultSize(FontManager::FontRole::Header);

   state.themeSettings.fontMainWeight = fontManager.getPhosphorWeightString(fontManager.getPhosphorWeight());
   state.themeSettings.fontSecondaryWeight = state.themeSettings.fontMainWeight;

   state.globalSettings.interfaceScale = INIConfig::clampScale(fontManager.getScaleFactor());

   state.themeSettings.backgroundColor = colorScheme.getColor(ColorScheme::ColorRole::WindowBackground).toDisplayString(true);
   state.themeSettings.foregroundColor = colorScheme.getColor(ColorScheme::ColorRole::ControlBackground).toDisplayString(true);
   state.themeSettings.accentColor = colorScheme.getColor(ColorScheme::ColorRole::Accent).toDisplayString(true);
   state.themeSettings.textColor = colorScheme.getColor(ColorScheme::ColorRole::PrimaryText).toDisplayString(true);
   state.themeSettings.borderColor = colorScheme.getColor(ColorScheme::ColorRole::Separator).toDisplayString(true);
   state.themeSettings.buttonColor = colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground).toDisplayString(true);
   state.themeSettings.buttonHoverColor = colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundHover).toDisplayString(true);
   state.themeSettings.buttonActiveColor = colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundPressed).toDisplayString(true);
   state.themeSettings.sliderTrackColor = colorScheme.getColor(ColorScheme::ColorRole::SecondaryText).toDisplayString(true);
   state.themeSettings.sliderThumbColor = colorScheme.getColor(ColorScheme::ColorRole::Accent).toDisplayString(true);
   state.themeSettings.meterColorLow = colorScheme.getColor(ColorScheme::ColorRole::MeterLow).toDisplayString(true);
   state.themeSettings.meterColorMid = colorScheme.getColor(ColorScheme::ColorRole::MeterMid).toDisplayString(true);
   state.themeSettings.meterColorHigh = colorScheme.getColor(ColorScheme::ColorRole::MeterHigh).toDisplayString(true);
   state.themeSettings.gridLineColor = colorScheme.getColor(ColorScheme::ColorRole::GridLine).toDisplayString(true);
   state.themeSettings.patternActiveColor = colorScheme.getColor(ColorScheme::ColorRole::PatternActive).toDisplayString(true);
   state.themeSettings.patternInactiveColor = colorScheme.getColor(ColorScheme::ColorRole::PatternInactive).toDisplayString(true);

   state.themeSettings.fontMain = fontManager.getDefaultFontName(FontManager::FontRole::Body);
   state.themeSettings.fontSecondary = fontManager.getDefaultFontName(FontManager::FontRole::Header);

   state.themeSettings.modifiedDate = juce::Time::getCurrentTime().formatted("%Y-%m-%d-%H-%M-%S");
}

void CustomLookAndFeel::loadStates(const ComponentState& state) {
   if (state.themeSettings.themeID == 1) {
       colorScheme.setDarkTheme();
   } else if (state.themeSettings.themeID == 2) {
       colorScheme.setLightTheme();
   }

   juce::String fontWeight = state.themeSettings.fontMainWeight;
   fontManager.setPhosphorWeight(fontManager.getPhosphorWeightFromString(fontWeight));

   float scale = INIConfig::clampScale(state.globalSettings.interfaceScale);
   fontManager.setScaleFactor(scale);

   syncWithColorScheme();
   reloadImages();
}

void CustomLookAndFeel::refreshColours() {
    syncWithColorScheme();
}

void CustomLookAndFeel::syncWithColorScheme() {
    colorScheme.applyToLookAndFeel(*this);

    setColour(juce::PopupMenu::backgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
    setColour(juce::PopupMenu::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundHover));
    setColour(juce::PopupMenu::highlightedTextColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    setColour(juce::PopupMenu::headerTextColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));

    setColour(juce::TooltipWindow::backgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::WindowBackground).darker(INIConfig::LayoutConstants::customLookFeelDarkerAmount));
    setColour(juce::TooltipWindow::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    setColour(juce::TooltipWindow::outlineColourId, colorScheme.getColor(ColorScheme::ColorRole::Separator));
}

void CustomLookAndFeel::reloadImages() {
   // Load from embedded BinaryData instead of files
   if (!buttonImage.isValid()) {
       buttonImage = juce::ImageCache::getFromMemory(BinaryData::Button100_png, BinaryData::Button100_pngSize);
   }

   if (!sliderImage.isValid()) {
       sliderImage = juce::ImageCache::getFromMemory(BinaryData::Slider100_png, BinaryData::Slider100_pngSize);
   }

   if (!splashImage.isValid()) {
       splashImage = juce::ImageCache::getFromMemory(BinaryData::OTTO_Splash_Screen_png, BinaryData::OTTO_Splash_Screen_pngSize);
   }

   if (!panKnobImage.isValid()) {
       panKnobImage = juce::ImageCache::getFromMemory(BinaryData::PanKnob_png, BinaryData::PanKnob_pngSize);
   }

   if (!pan100Image.isValid()) {
       pan100Image = juce::ImageCache::getFromMemory(BinaryData::Pan100_png, BinaryData::Pan100_pngSize);
   }
}

void CustomLookAndFeel::forceRefreshFromColorScheme() {
   colorScheme.forceUpdateLookAndFeel(*this);
   reloadImages();

   setColour(juce::PopupMenu::backgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
   setColour(juce::PopupMenu::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
   setColour(juce::PopupMenu::highlightedBackgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundHover));
   setColour(juce::PopupMenu::highlightedTextColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
   setColour(juce::PopupMenu::headerTextColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
}
