#pragma once
#include <JuceHeader.h>
#include "../UtilityComponents.h"
#include "../INIConfig.h"
#include "../PlatformResponsiveManager.h"

class AnimationManager {
public:
    AnimationManager(PlatformResponsiveManager& platformManager);
    
    static float easeInOut(float t);
    static float easeSpring(float t);
    static float easeElastic(float t);
    
    void animateSliderValue(juce::Slider& slider, float targetValue, int durationMs = 300);
    void animateButtonPress(juce::Button& button, int durationMs = 150);
    void animatePatternSwitch(juce::Component& component, int durationMs = 200);
    
    bool shouldUseAnimations() const;
    int getOptimalDuration(int baseDuration) const;
    
private:
    class SliderAnimationTimer : public juce::Timer {
    public:
        SliderAnimationTimer(juce::Slider& slider, float startValue, float targetValue, 
                           juce::int64 startTime, int duration, AnimationManager& manager);
        void timerCallback() override;
        
    private:
        juce::Slider& slider;
        float startValue, targetValue;
        juce::int64 startTime;
        int duration;
        AnimationManager& animationManager;
    };
    
    juce::OwnedArray<SliderAnimationTimer> activeTimers;
    PlatformResponsiveManager& platformManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnimationManager)
};
