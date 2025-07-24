#pragma once
#include <JuceHeader.h>
#include "UtilityComponents.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ColorScheme.h"
#include "ComponentState.h"
#include "INIConfig.h"

class LoopSectionComponent : public juce::Component {
public:
    LoopSectionComponent(ResponsiveLayoutManager& layoutManager,
                        FontManager& fontManager,
                        ColorScheme& colorScheme);

    void paint(juce::Graphics&) override;
    void resized() override;

    float getLoopPosition() const { return static_cast<float>(loopSlider.getValue()); }
    void setLoopPosition(float position) { loopSlider.setValue(position, juce::dontSendNotification); }

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    std::function<void(float)> onLoopPositionChanged;

private:
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;

    juce::Label startLabel;
    juce::Label endLabel;
    ScaledSlider loopSlider;

    void setupComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoopSectionComponent)
};
