#include "Row2Component.h"
#include "MidiEngine.h"

Row2Component::Row2Component(MidiEngine& midiEngine,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : RowComponentBase(2, layoutManager, fontManager, colorScheme)
    , midiEngine(midiEngine) {
    
    setupPlayerTabsComponents();
}

void Row2Component::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::transparentBlack);
}

void Row2Component::resized() {
    updatePlayerTabsLayout();
}

void Row2Component::saveStates(ComponentState& state) {
}

void Row2Component::loadStates(const ComponentState& state) {
}

void Row2Component::updateFromState(const ComponentState& state) {
}

juce::Rectangle<int> Row2Component::getRowBounds() const {
    using namespace INIConfig::LayoutConstants;
    return getScaledBounds(0, Row2::yPosition, 
                          INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH, 
                          Row2::height);
}

void Row2Component::setupPlayerTabsComponents() {
}

void Row2Component::updatePlayerTabsLayout() {
}
