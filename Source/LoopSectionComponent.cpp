#include "LoopSectionComponent.h"
#include "INIConfig.h"

LoopSectionComponent::LoopSectionComponent(ResponsiveLayoutManager& layoutManager,
                                          FontManager& fontManager,
                                          ColorScheme& colorScheme)
    : layoutManager(layoutManager), fontManager(fontManager), colorScheme(colorScheme),
      loopSlider(layoutManager) {
    setupComponents();
}

void LoopSectionComponent::setupComponents() {
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

void LoopSectionComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::transparentBlack);
}

void LoopSectionComponent::resized() {
    auto bounds = getLocalBounds();

    int labelWidth = layoutManager.scaled(INIConfig::LayoutConstants::loopSectionLabelWidth);
    int labelHeight = layoutManager.scaled(INIConfig::LayoutConstants::loopSectionLabelHeight);

    startLabel.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::loopSectionLabelMargin), 0, labelWidth, labelHeight);
    endLabel.setBounds(bounds.getWidth() - labelWidth - layoutManager.scaled(INIConfig::LayoutConstants::loopSectionLabelMargin), 0, labelWidth, labelHeight);

    int loopWidth = bounds.getWidth() - layoutManager.scaled(INIConfig::LayoutConstants::loopSectionSliderWidthReduction);
    int loopHeight = layoutManager.scaled(INIConfig::LayoutConstants::loopSectionSliderHeight);
    int loopX = layoutManager.scaled(INIConfig::LayoutConstants::loopSectionSliderMargin);
    int loopY = layoutManager.scaled(INIConfig::LayoutConstants::loopSectionSliderY);

    loopSlider.setBounds(loopX, loopY, loopWidth, loopHeight);
    loopSlider.repaint();
}

void LoopSectionComponent::saveStates(ComponentState& state) {
    float loopPosition = getLoopPosition();
    if (!INIConfig::isValidLoopPosition(loopPosition)) {
        loopPosition = INIConfig::clampLoopPosition(loopPosition);
    }
    state.sliderValues["loopPosition"] = loopPosition;
}

void LoopSectionComponent::loadStates(const ComponentState& state) {
    auto it = state.sliderValues.find("loopPosition");
    if (it != state.sliderValues.end()) {
        float loopPosition = it->second;
        if (!INIConfig::isValidLoopPosition(loopPosition)) {
            loopPosition = INIConfig::Defaults::DEFAULT_LOOP_POSITION;
        }
        setLoopPosition(loopPosition);
    } else {
        setLoopPosition(INIConfig::Defaults::DEFAULT_LOOP_POSITION);
    }
}
