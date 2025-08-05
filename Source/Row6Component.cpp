#include "JUCE8_CODING_STANDARDS.h"
#include "Row6Component.h"
#include "INIConfig.h"

Row6Component::Row6Component(ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : ResponsiveComponent(),
      layoutManager(layoutManager),
      fontManager(fontManager),
      colorScheme(colorScheme),
      loopSlider(layoutManager) {
    
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
    return getScaledBounds(0, Row6::yPosition, 
                          INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH, 
                          Row6::height);
}

void Row6Component::setupLoopComponents() {
    addAndMakeVisible(startLabel);
    addAndMakeVisible(endLabel);
    addAndMakeVisible(loopSlider);

    startLabel.setText("LOOP START", juce::dontSendNotification);
    startLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    startLabel.setJustificationType(juce::Justification::centredLeft);

    endLabel.setText("LOOP END", juce::dontSendNotification);
    endLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    endLabel.setJustificationType(juce::Justification::centredRight);

    loopSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    loopSlider.setRange(INIConfig::Validation::MIN_LOOP_POSITION, INIConfig::Validation::MAX_LOOP_POSITION, 1.0);
    loopSlider.setValue(INIConfig::Defaults::DEFAULT_LOOP_POSITION);
    loopSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    loopSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::transparentBlack);
    loopSlider.setColour(juce::Slider::trackColourId, colorScheme.getColor(ColorScheme::ColorRole::Separator));
    loopSlider.setColour(juce::Slider::thumbColourId, colorScheme.getColor(ColorScheme::ColorRole::Accent));

    loopSlider.onValueChange = [this] {
        if (onLoopPositionChanged) {
            onLoopPositionChanged(static_cast<float>(loopSlider.getValue()));
        }
    };
}

void Row6Component::updateLoopLayout() {
    auto bounds = getLocalBounds();
    
    // Use responsive calculations instead of hardcoded values
    int spacing = getResponsiveSpacing();
    int margin = getResponsiveMargin(10);
    
    // Calculate responsive dimensions
    int labelWidth = static_cast<int>(bounds.getWidth() * 0.15f); // 15% of width for labels
    int labelHeight = static_cast<int>(bounds.getHeight() * 0.4f); // 40% of row height
    int sliderHeight = static_cast<int>(bounds.getHeight() * 0.6f); // 60% of row height
    
    // Center everything vertically
    int labelY = (bounds.getHeight() - labelHeight) / 2;
    int sliderY = (bounds.getHeight() - sliderHeight) / 2;
    
    // Update label font size responsively
    float labelFontSize = getResponsiveFontSize(10.0f);
    startLabel.setFont(JUCE8_FONT(labelFontSize));
    endLabel.setFont(JUCE8_FONT(labelFontSize));
    
    // Position start label (left side)
    int startLabelX = margin;
    startLabel.setBounds(startLabelX, labelY, labelWidth, labelHeight);
    
    // Position end label (right side)
    int endLabelX = bounds.getWidth() - labelWidth - margin;
    endLabel.setBounds(endLabelX, labelY, labelWidth, labelHeight);
    
    // Position slider (center, between labels)
    int sliderX = startLabelX + labelWidth + spacing;
    int sliderWidth = endLabelX - sliderX - spacing;
    
    // Ensure minimum slider width for usability
    int minSliderWidth = static_cast<int>(bounds.getWidth() * 0.3f); // At least 30% of width
    if (sliderWidth < minSliderWidth) {
        sliderWidth = minSliderWidth;
        sliderX = (bounds.getWidth() - sliderWidth) / 2;
    }
    
    loopSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);
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
    
    resized();
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
