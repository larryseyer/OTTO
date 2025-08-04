#include "Row5Component.h"
#include "MidiEngine.h"
#include "Mixer.h"

Row5Component::Row5Component(MidiEngine& midiEngine,
                           Mixer& mixer,
                           juce::AudioProcessorValueTreeState& valueTreeState,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : RowComponentBase(5, layoutManager, fontManager, colorScheme)
    , midiEngine(midiEngine)
    , mixer(mixer)
    , valueTreeState(valueTreeState) {
    
    setupInteractiveComponents();
}

void Row5Component::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::transparentBlack);
}

void Row5Component::resized() {
    updateInteractiveLayout();
}

void Row5Component::saveStates(ComponentState& state) {
}

void Row5Component::loadStates(const ComponentState& state) {
}

void Row5Component::updateFromState(const ComponentState& state) {
}

juce::Rectangle<int> Row5Component::getRowBounds() const {
    using namespace INIConfig::LayoutConstants;
    return getScaledBounds(0, Row5::yPosition, 
                          INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH, 
                          Row5::height);
}

void Row5Component::setupInteractiveComponents() {
}

void Row5Component::updateInteractiveLayout() {
}
