#pragma once
#include "RowComponentBase.h"

class MidiEngine;
class Mixer;

class Row5Component : public RowComponentBase {
public:
    Row5Component(MidiEngine& midiEngine,
                 Mixer& mixer,
                 juce::AudioProcessorValueTreeState& valueTreeState,
                 ResponsiveLayoutManager& layoutManager,
                 FontManager& fontManager,
                 ColorScheme& colorScheme);
    
    ~Row5Component() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void saveStates(ComponentState& state) override;
    void loadStates(const ComponentState& state) override;
    void updateFromState(const ComponentState& state) override;
    juce::String getRowName() const override { return "InteractiveControls"; }
    juce::Rectangle<int> getRowBounds() const override;
    
private:
    MidiEngine& midiEngine;
    Mixer& mixer;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    void setupInteractiveComponents();
    void updateInteractiveLayout();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Row5Component)
};
