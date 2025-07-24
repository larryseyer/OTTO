#pragma once
#include <JuceHeader.h>
#include "INIDataManager.h"
#include "ColorScheme.h"

class GrooveAndFillsWindow : public juce::DocumentWindow {
public:
    GrooveAndFillsWindow(INIDataManager& INIManager, ColorScheme& colorScheme);
    ~GrooveAndFillsWindow();

    void closeButtonPressed() override;
    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

private:
    class GrooveContent;

    INIDataManager& INIManager;
    ColorScheme& colorScheme;
    std::unique_ptr<Component> content;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrooveAndFillsWindow)
};
