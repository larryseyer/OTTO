#pragma once
#include "RowComponentBase.h"
#include "UtilityComponents.h"

class MidiEngine;
class Mixer;

class Row1Component : public RowComponentBase {
public:
    Row1Component(MidiEngine& midiEngine,
                 Mixer& mixer,
                 ResponsiveLayoutManager& layoutManager,
                 FontManager& fontManager,
                 ColorScheme& colorScheme);
    
    ~Row1Component() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void saveStates(ComponentState& state) override;
    void loadStates(const ComponentState& state) override;
    void updateFromState(const ComponentState& state) override;
    juce::String getRowName() const override { return "TopBar"; }
    juce::Rectangle<int> getRowBounds() const override;
    
private:
    MidiEngine& midiEngine;
    Mixer& mixer;
    
    void setupTopBarComponents();
    void updateTopBarLayout();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Row1Component)
};
