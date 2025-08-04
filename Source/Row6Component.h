#pragma once
#include "RowComponentBase.h"
#include "UtilityComponents.h"

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
    void lookAndFeelChanged() override;
    juce::String getRowName() const override { return "LoopControls"; }
    juce::Rectangle<int> getRowBounds() const override;
    
    // Loop control interface methods
    float getLoopPosition() const { return static_cast<float>(loopSlider.getValue()); }
    void setLoopPosition(float position) { loopSlider.setValue(position, juce::dontSendNotification); }
    
    std::function<void(float)> onLoopPositionChanged;
    
private:
    void setupLoopComponents();
    void updateLoopLayout();
    
    // Loop control components
    juce::Label startLabel;
    juce::Label endLabel;
    ScaledSlider loopSlider;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Row6Component)
};
