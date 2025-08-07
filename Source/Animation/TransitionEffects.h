#pragma once
#include <JuceHeader.h>
#include "EasingCurves.h"

class TransitionEffects {
public:
    static void fadeTransition(juce::Component& component, float targetAlpha, int durationMs);
    static void slideTransition(juce::Component& component, juce::Point<int> targetPosition, int durationMs);
    static void scaleTransition(juce::Component& component, float targetScale, int durationMs);
    static void colorTransition(juce::Component& component, juce::Colour targetColor, int durationMs);
    
    class TransitionTimer : public juce::Timer {
    public:
        TransitionTimer(std::function<void(float)> updateCallback, 
                       std::function<void()> completionCallback,
                       int durationMs);
        
        void timerCallback() override;
        
    private:
        std::function<void(float)> updateCallback;
        std::function<void()> completionCallback;
        int duration;
        juce::int64 startTime;
    };
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransitionEffects)
};
