#pragma once
#include <JuceHeader.h>
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ColorScheme.h"
#include "ComponentState.h"
#include "INIConfig.h"

class RowComponentBase : public juce::Component {
public:
    RowComponentBase(int rowNumber,
                    ResponsiveLayoutManager& layoutManager,
                    FontManager& fontManager,
                    ColorScheme& colorScheme);
    
    virtual ~RowComponentBase() = default;
    
    virtual void saveStates(ComponentState& state) = 0;
    virtual void loadStates(const ComponentState& state) = 0;
    virtual void updateFromState(const ComponentState& state) = 0;
    
    int getRowNumber() const { return rowNumber; }
    virtual juce::String getRowName() const = 0;
    virtual juce::Rectangle<int> getRowBounds() const = 0;
    
    void lookAndFeelChanged() override;
    
protected:
    const int rowNumber;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;
    
    void setupCommonComponents();
    void updateCommonColors();
    juce::Rectangle<int> getScaledBounds(int x, int y, int width, int height) const;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RowComponentBase)
};
