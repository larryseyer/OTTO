#pragma once
#include <JuceHeader.h>
#include "../UtilityComponents.h"

class AccessibilityEnhancer {
public:
    AccessibilityEnhancer();
    
    void enhanceComponent(juce::Component& component, const juce::String& role);
    void setAccessibleName(juce::Component& component, const juce::String& name);
    void setAccessibleDescription(juce::Component& component, const juce::String& description);
    void setupTabOrder(const juce::Array<juce::Component*>& components);
    
    bool isScreenReaderActive() const;
    bool isHighContrastMode() const;
    bool isReducedMotionEnabled() const;
    
private:
    void announceChange(const juce::String& message);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AccessibilityEnhancer)
};
