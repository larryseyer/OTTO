#pragma once
#include <JuceHeader.h>
#include "../MidiEngine.h"
#include "RenderOptimizer.h"

class AudioProfiler {
public:
    AudioProfiler(MidiEngine& midiEngine, RenderOptimizer& renderOptimizer);
    
    void profilePatternSwitching();
    void profileRenderPerformance();
    double getLastPatternSwitchTime() const { return lastPatternSwitchTime; }
    
private:
    MidiEngine& midiEngine;
    RenderOptimizer& renderOptimizer;
    double lastPatternSwitchTime = 0.0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProfiler)
};
