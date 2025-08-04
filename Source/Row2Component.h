#pragma once
#include "RowComponentBase.h"

class MidiEngine;

class Row2Component : public RowComponentBase {
public:
    Row2Component(MidiEngine& midiEngine,
                 ResponsiveLayoutManager& layoutManager,
                 FontManager& fontManager,
                 ColorScheme& colorScheme);
    
    ~Row2Component() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void saveStates(ComponentState& state) override;
    void loadStates(const ComponentState& state) override;
    void updateFromState(const ComponentState& state) override;
    juce::String getRowName() const override { return "PlayerTabs"; }
    juce::Rectangle<int> getRowBounds() const override;
    
private:
    MidiEngine& midiEngine;
    
    void setupPlayerTabsComponents();
    void updatePlayerTabsLayout();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Row2Component)
};
