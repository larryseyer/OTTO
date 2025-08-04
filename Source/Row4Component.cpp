#include "Row4Component.h"
#include "MidiEngine.h"

Row4Component::Row4Component(MidiEngine& midiEngine,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : RowComponentBase(4, layoutManager, fontManager, colorScheme)
    , midiEngine(midiEngine) {
    
    setupPatternGroupComponents();
}

void Row4Component::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::transparentBlack);
}

void Row4Component::resized() {
    updatePatternGroupLayout();
}

void Row4Component::saveStates(ComponentState& state) {
}

void Row4Component::loadStates(const ComponentState& state) {
}

void Row4Component::updateFromState(const ComponentState& state) {
}

juce::Rectangle<int> Row4Component::getRowBounds() const {
    using namespace INIConfig::LayoutConstants;
    return getScaledBounds(0, Row4::yPosition, 
                          INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH, 
                          Row4::height);
}

void Row4Component::setupPatternGroupComponents() {
}

void Row4Component::updatePatternGroupLayout() {
}
