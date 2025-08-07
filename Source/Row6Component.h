#pragma once
#include "JUCE8_CODING_STANDARDS.h"
#include "UI/Layout/BreakpointManager.h"
#include "UtilityComponents.h"

class Row6Component : public OTTO::UI::Layout::ResponsiveComponent {
public:
    Row6Component(ResponsiveLayoutManager& layoutManager,
                 FontManager& fontManager,
                 ColorScheme& colorScheme);
    
    ~Row6Component() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);
    void updateFromState(const ComponentState& state);
    void lookAndFeelChanged() override;
    juce::String getRowName() const { return "LoopControls"; }
    juce::Rectangle<int> getRowBounds() const;
    
    // ResponsiveComponent overrides
    void updateResponsiveLayout() override;
    
    // Responsive calculations
    int getResponsiveButtonSize() const;
    int getResponsiveSpacing() const;
    float getResponsiveFontSize(float baseSize) const;
    
    // Loop control interface methods
    float getLoopPosition() const { return static_cast<float>(loopSlider.getValue()); }
    void setLoopPosition(float position) { loopSlider.setValue(position, juce::dontSendNotification); }
    
    std::function<void(float)> onLoopPositionChanged;
    
private:
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;
    
    // Issue 6.4: Use existing CustomLookAndFeel which already handles rotated slider images
    
    void setupLoopComponents();
    void updateLoopLayout();
    
    // Loop control components
    juce::Label startLabel;
    juce::Label endLabel;
    ScaledSlider loopSlider;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Row6Component)
};
