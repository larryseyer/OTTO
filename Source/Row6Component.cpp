#include "Row6Component.h"
#include "INIConfig.h"

Row6Component::Row6Component(ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : RowComponentBase(6, layoutManager, fontManager, colorScheme),
      loopSlider(layoutManager) {
    
    setupLoopComponents();
}

void Row6Component::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::transparentBlack);
}

void Row6Component::resized() {
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
    using namespace INIConfig::LayoutConstants;
    
    auto bounds = getLocalBounds();
    
    // Calculate scaled dimensions using Row6 constants
    int labelWidth = layoutManager.scaled(Row6::labelWidth);
    int labelMargin = layoutManager.scaled(Row6::labelMargin);
    int sliderMargin = layoutManager.scaled(Row6::sliderMargin);
    int labelHeight = layoutManager.scaled(Row6::labelHeight);
    int sliderHeight = layoutManager.scaled(Row6::sliderHeight);
    
    // Position start label (left side)
    int startLabelX = layoutManager.scaled(Row6::startLabelX);
    int labelY = layoutManager.scaled(Row6::labelY);
    startLabel.setBounds(startLabelX, labelY, labelWidth, labelHeight);
    
    // Position end label (right side)
    int endLabelX = bounds.getWidth() - labelWidth - labelMargin;
    endLabel.setBounds(endLabelX, labelY, labelWidth, labelHeight);
    
    // Position slider (center, between labels)
    int sliderX = startLabelX + labelWidth + sliderMargin;
    int sliderWidth = endLabelX - sliderX - sliderMargin;
    int sliderY = layoutManager.scaled(Row6::sliderY);
    
    // Ensure minimum slider width for usability
    if (sliderWidth < layoutManager.scaled(200)) {
        sliderWidth = layoutManager.scaled(200);
        sliderX = (bounds.getWidth() - sliderWidth) / 2;
    }
    
    loopSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);
}
