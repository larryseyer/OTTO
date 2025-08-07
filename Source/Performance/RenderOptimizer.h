#pragma once
#include <JuceHeader.h>
#include "../INIConfig.h"

class RenderOptimizer {
public:
    RenderOptimizer();
    
    void startFrame();
    void endFrame();
    bool isTargetFrameRateAchieved() const { return averageFrameTime <= targetFrameTime; }
    
    void optimizePaintCalls(juce::Component& component);
    void enableDirtyRegionTracking(juce::Component& component);
    
    double getAverageFrameTime() const { return averageFrameTime; }
    int getCurrentFPS() const;
    
private:
    static constexpr double targetFrameTime = 1000.0 / 60.0;
    double averageFrameTime = 0.0;
    juce::int64 frameStartTime = 0;
    juce::Array<double> frameTimes;
    
    void updateFrameTimeAverage(double frameTime);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RenderOptimizer)
};
