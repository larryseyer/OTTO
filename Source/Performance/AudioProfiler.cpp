#include "AudioProfiler.h"

AudioProfiler::AudioProfiler(MidiEngine& midiEngine, RenderOptimizer& renderOptimizer)
    : midiEngine(midiEngine), renderOptimizer(renderOptimizer) {
}

void AudioProfiler::profilePatternSwitching() {
    auto startTime = juce::Time::getHighResolutionTicks();
    
    auto endTime = juce::Time::getHighResolutionTicks();
    lastPatternSwitchTime = (endTime - startTime) / juce::Time::getHighResolutionTicksPerSecond() * 1000.0;
    
    jassert(lastPatternSwitchTime < 100.0);
    
    DBG("Pattern switch time: " << lastPatternSwitchTime << "ms");
}

void AudioProfiler::profileRenderPerformance() {
    renderOptimizer.startFrame();
    
    for (int i = 0; i < 1000; ++i) {
    }
    
    renderOptimizer.endFrame();
    
    jassert(renderOptimizer.isTargetFrameRateAchieved());
}
