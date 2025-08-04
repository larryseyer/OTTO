#include "Row1Component.h"
#include "MidiEngine.h"
#include "Mixer.h"

Row1Component::Row1Component(MidiEngine& midiEngine,
                           Mixer& mixer,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : RowComponentBase(1, layoutManager, fontManager, colorScheme)
    , midiEngine(midiEngine)
    , mixer(mixer) {
    
    setupTopBarComponents();
}

void Row1Component::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::transparentBlack);
}

void Row1Component::resized() {
    updateTopBarLayout();
}

void Row1Component::saveStates(ComponentState& state) {
}

void Row1Component::loadStates(const ComponentState& state) {
}

void Row1Component::updateFromState(const ComponentState& state) {
}

juce::Rectangle<int> Row1Component::getRowBounds() const {
    using namespace INIConfig::LayoutConstants;
    return getScaledBounds(0, Row1::yPosition, 
                          INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH, 
                          Row1::height);
}

void Row1Component::setupTopBarComponents() {
}

void Row1Component::updateTopBarLayout() {
}
