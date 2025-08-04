#pragma once
#include "RowComponentBase.h"
#include "UtilityComponents.h"

class MidiEngine;
class INIDataManager;

class Row4Component : public RowComponentBase {
public:
    Row4Component(MidiEngine& midiEngine,
                 ResponsiveLayoutManager& layoutManager,
                 FontManager& fontManager,
                 ColorScheme& colorScheme);
    
    ~Row4Component() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void saveStates(ComponentState& state) override;
    void loadStates(const ComponentState& state) override;
    void updateFromState(const ComponentState& state) override;
    juce::String getRowName() const override { return "PatternGroupControls"; }
    juce::Rectangle<int> getRowBounds() const override;
    
    void setINIDataManager(INIDataManager* manager) { iniDataManager = manager; }
    
private:
    MidiEngine& midiEngine;
    INIDataManager* iniDataManager = nullptr;
    
    void setupPatternGroupComponents();
    void updatePatternGroupLayout();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Row4Component)
};
