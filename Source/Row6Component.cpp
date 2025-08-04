#include "Row6Component.h"

Row6Component::Row6Component(ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : RowComponentBase(6, layoutManager, fontManager, colorScheme) {
    
    setupLoopComponents();
}

void Row6Component::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::transparentBlack);
}

void Row6Component::resized() {
    updateLoopLayout();
}

void Row6Component::saveStates(ComponentState& state) {
}

void Row6Component::loadStates(const ComponentState& state) {
}

void Row6Component::updateFromState(const ComponentState& state) {
}

juce::Rectangle<int> Row6Component::getRowBounds() const {
    using namespace INIConfig::LayoutConstants;
    return getScaledBounds(0, Row6::yPosition, 
                          INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH, 
                          Row6::height);
}

void Row6Component::setupLoopComponents() {
}

void Row6Component::updateLoopLayout() {
}
