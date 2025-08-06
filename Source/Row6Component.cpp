#include "JUCE8_CODING_STANDARDS.h"
#include "Row6Component.h"
#include "INIConfig.h"
#include "ResponsiveLayoutManager.h"

Row6Component::Row6Component(ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : ResponsiveComponent(),
      layoutManager(layoutManager),
      fontManager(fontManager),
      colorScheme(colorScheme),
      loopSlider(layoutManager) {
    
    // Issue 6.4: Apply custom look and feel to slider for rotated thumb
    rotatedSliderLookAndFeel = std::make_unique<RotatedSliderLookAndFeel>();
    loopSlider.setLookAndFeel(rotatedSliderLookAndFeel.get());
    
    setupLoopComponents();
}

void Row6Component::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::transparentBlack);
}

void Row6Component::resized() {
    ResponsiveComponent::resized(); // Call parent first
    updateLoopLayout();
}

void Row6Component::saveStates(ComponentState& state) {
    // Save loop position to state
    state.loopPosition = static_cast<float>(loopSlider.getValue());
}

void Row6Component::loadStates(const ComponentState& state) {
    // Load loop position from state
    loopSlider.setValue(state.loopPosition, juce::dontSendNotification);
}

void Row6Component::updateFromState(const ComponentState& state) {
    // Update loop position from state (with notification for UI updates)
    loopSlider.setValue(state.loopPosition, juce::sendNotification);
}

void Row6Component::lookAndFeelChanged() {
    // PHASE 7: Row6Component ColorScheme integration - Loop controls only
    startLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    endLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    
    loopSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::transparentBlack);
    loopSlider.setColour(juce::Slider::trackColourId, colorScheme.getColor(ColorScheme::ColorRole::Separator));
    loopSlider.setColour(juce::Slider::thumbColourId, colorScheme.getColor(ColorScheme::ColorRole::Accent));
}

juce::Rectangle<int> Row6Component::getRowBounds() const {
    using namespace INIConfig::LayoutConstants;
    return juce::Rectangle<int>(
        layoutManager.scaled(0),
        layoutManager.scaled(Row6::yPosition),
        layoutManager.scaled(INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH),
        layoutManager.scaled(Row6::height)
    );
}

void Row6Component::setupLoopComponents() {
    // Issues 6.1 & 6.2: Ensure only one instance of each component exists
    // Set unique component IDs to prevent duplicates
    
    // LOOP START label - single instance only
    startLabel.setComponentID("loop_start_label");
    startLabel.setText("LOOP START", juce::dontSendNotification);
    startLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    startLabel.setJustificationType(juce::Justification::centredLeft);
    startLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, 
                      layoutManager.scaled(INIConfig::LayoutConstants::Row6::labelFontSize)));
    addAndMakeVisible(startLabel);
    
    // LOOP END label - single instance only
    endLabel.setComponentID("loop_end_label");
    endLabel.setText("LOOP END", juce::dontSendNotification);
    endLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    endLabel.setJustificationType(juce::Justification::centredRight);
    endLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, 
                    layoutManager.scaled(INIConfig::LayoutConstants::Row6::labelFontSize)));
    addAndMakeVisible(endLabel);
    
    // Loop slider - single instance only
    loopSlider.setComponentID("loop_position_slider");
    loopSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    loopSlider.setRange(INIConfig::Validation::MIN_LOOP_POSITION, INIConfig::Validation::MAX_LOOP_POSITION, 1.0);
    loopSlider.setValue(INIConfig::Defaults::DEFAULT_LOOP_POSITION);
    loopSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    loopSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::transparentBlack);
    loopSlider.setColour(juce::Slider::trackColourId, colorScheme.getColor(ColorScheme::ColorRole::Separator));
    loopSlider.setColour(juce::Slider::thumbColourId, colorScheme.getColor(ColorScheme::ColorRole::Accent));
    addAndMakeVisible(loopSlider);

    loopSlider.onValueChange = [this] {
        if (onLoopPositionChanged) {
            onLoopPositionChanged(static_cast<float>(loopSlider.getValue()));
        }
    };
}

void Row6Component::updateLoopLayout() {
    auto bounds = getLocalBounds();
    
    // Issue 6.3: Slider should travel full width of app window
    int fullWindowWidth = layoutManager.getWindowWidth(); // Get full app width
    
    // Use responsive calculations instead of hardcoded values
    int spacing = getResponsiveSpacing();
    int margin = layoutManager.scaled(INIConfig::LayoutConstants::Row6::windowMargin);
    
    // Calculate responsive dimensions using INIConfig constants
    int labelWidth = layoutManager.scaled(INIConfig::LayoutConstants::Row6::labelWidth);
    int labelHeight = static_cast<int>(bounds.getHeight() * 0.4f); // 40% of row height
    int sliderHeight = static_cast<int>(bounds.getHeight() * 0.6f); // 60% of row height
    
    // Position labels at window edges with margin
    int labelY = bounds.getY() + layoutManager.scaled(INIConfig::LayoutConstants::Row6::labelTopMargin);
    
    // Update label font size responsively using INIConfig constant
    float labelFontSize = getResponsiveFontSize(INIConfig::LayoutConstants::Row6::labelFontSize);
    startLabel.setFont(JUCE8_FONT(labelFontSize));
    endLabel.setFont(JUCE8_FONT(labelFontSize));
    
    // Position start label (left edge of window)
    startLabel.setBounds(margin, labelY, labelWidth, labelHeight);
    
    // Position end label (right edge of window)
    endLabel.setBounds(fullWindowWidth - labelWidth - margin, labelY, labelWidth, labelHeight);
    
    // Issue 6.3: Slider spans full width with margins
    int sliderY = labelY + labelHeight + layoutManager.scaled(INIConfig::LayoutConstants::Row6::sliderTopOffset);
    int sliderWidth = fullWindowWidth - (2 * margin); // Full width minus margins
    int finalSliderHeight = bounds.getHeight() - labelHeight - layoutManager.scaled(INIConfig::LayoutConstants::Row6::remainingHeightOffset);
    
    loopSlider.setBounds(margin, sliderY, sliderWidth, finalSliderHeight);
}

//==============================================================================
// ResponsiveComponent Implementation
//==============================================================================

void Row6Component::updateResponsiveLayout() {
    auto category = getCurrentDeviceCategory();
    
    // Device-specific adjustments for loop controls
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            // Mobile: Larger touch targets, simplified layout
            break;
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            // Tablet: Medium touch targets, balanced layout
            break;
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            // Desktop: Standard layout with mouse precision
            break;
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            // Large Desktop: Expanded layout, more information density
            break;
        default:
            break;
    }
    
    // Layout update will happen naturally through component hierarchy
    // Do not call resized() here as it creates infinite recursion
}

int Row6Component::getResponsiveButtonSize() const {
    auto category = getCurrentDeviceCategory();
    auto rules = getCurrentLayoutRules();
    
    // Base size from row height - this is a smaller row
    int baseSize = static_cast<int>(getHeight() * 0.8f); // 80% of row height
    
    // Apply device-specific adjustments
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            return juce::jmax(static_cast<int>(rules.sizing.minTouchTarget), baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            return juce::jmax(static_cast<int>(rules.sizing.minTouchTarget * 0.9f), baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            return juce::jmax(24, baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            return juce::jmax(28, static_cast<int>(baseSize * 1.1f));
        default:
            return juce::jmax(24, baseSize);
    }
}

int Row6Component::getResponsiveSpacing() const {
    auto category = getCurrentDeviceCategory();
    auto rules = getCurrentLayoutRules();
    
    // Base spacing
    int baseSpacing = rules.spacing.defaultSpacing;
    
    // Apply device-specific adjustments
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            return juce::jmax(6, baseSpacing);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            return juce::jmax(4, baseSpacing);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            return juce::jmax(3, baseSpacing);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            return juce::jmax(4, baseSpacing);
        default:
            return baseSpacing;
    }
}

float Row6Component::getResponsiveFontSize(float baseSize) const {
    return ResponsiveComponent::getResponsiveFontSize(baseSize);
}

//==============================================================================
// Issue 6.4: Custom LookAndFeel Implementation for Rotated Slider Thumb
//==============================================================================

void Row6Component::RotatedSliderLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                                              float sliderPos, float minSliderPos, float maxSliderPos,
                                                              const juce::Slider::SliderStyle style, juce::Slider& slider) {
    juce::ignoreUnused(minSliderPos, maxSliderPos, style);
    
    // Draw the track normally (horizontal)
    juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, 
                                          minSliderPos, maxSliderPos, style, slider);
    
    // Draw custom rotated thumb
    float thumbX = x + sliderPos * (width - 20); // 20 = thumb width
    float thumbY = y + (height - 30) / 2; // 30 = thumb height, center vertically
    
    // Save graphics state
    juce::Graphics::ScopedSaveState saveState(g);
    
    // Rotate thumb image 90 degrees
    g.addTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::halfPi, 
                                                  thumbX + 10, thumbY + 15));
    
    // Draw rotated thumb
    juce::Rectangle<float> thumbBounds(thumbX, thumbY, 20, 30);
    g.setColour(juce::Colours::lightgrey);
    g.fillRoundedRectangle(thumbBounds, 3.0f);
    g.setColour(juce::Colours::darkgrey);
    g.drawRoundedRectangle(thumbBounds, 3.0f, 1.0f);
}
