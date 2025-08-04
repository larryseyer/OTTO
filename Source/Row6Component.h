#pragma once
#include "RowComponentBase.h"

class Row6Component : public RowComponentBase {
public:
    Row6Component(ResponsiveLayoutManager& layoutManager,
                 FontManager& fontManager,
                 ColorScheme& colorScheme);
    
    ~Row6Component() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void saveStates(ComponentState& state) override;
    void loadStates(const ComponentState& state) override;
    void updateFromState(const ComponentState& state) override;
    juce::String getRowName() const override { return "LoopControls"; }
    juce::Rectangle<int> getRowBounds() const override;
    
private:
    void setupLoopComponents();
    void updateLoopLayout();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Row6Component)
};
