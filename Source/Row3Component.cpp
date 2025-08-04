#include "Row3Component.h"
#include "MidiEngine.h"
#include "Mixer.h"

Row3Component::Row3Component(MidiEngine& midiEngine,
                           Mixer& mixer,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : RowComponentBase(3, layoutManager, fontManager, colorScheme)
    , midiEngine(midiEngine)
    , mixer(mixer) {
    
    setupDrumKitComponents();
}

void Row3Component::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::transparentBlack);
}

void Row3Component::resized() {
    updateDrumKitLayout();
}

void Row3Component::saveStates(ComponentState& state) {
}

void Row3Component::loadStates(const ComponentState& state) {
}

void Row3Component::updateFromState(const ComponentState& state) {
}

juce::Rectangle<int> Row3Component::getRowBounds() const {
    using namespace INIConfig::LayoutConstants;
    return getScaledBounds(0, Row3::yPosition, 
                          INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH, 
                          Row3::height);
}

void Row3Component::setupDrumKitComponents() {
}

void Row3Component::updateDrumKitLayout() {
}
