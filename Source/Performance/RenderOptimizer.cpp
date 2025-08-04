#include "RenderOptimizer.h"

RenderOptimizer::RenderOptimizer() {
    frameTimes.ensureStorageAllocated(60);
}

void RenderOptimizer::startFrame() {
    frameStartTime = juce::Time::getHighResolutionTicks();
}

void RenderOptimizer::endFrame() {
    auto frameEndTime = juce::Time::getHighResolutionTicks();
    auto frameTime = (frameEndTime - frameStartTime) / juce::Time::getHighResolutionTicksPerSecond() * 1000.0;
    
    updateFrameTimeAverage(frameTime);
}

void RenderOptimizer::optimizePaintCalls(juce::Component& component) {
    component.setBufferedToImage(true);
}

void RenderOptimizer::enableDirtyRegionTracking(juce::Component& component) {
    juce::ignoreUnused(component);
}

int RenderOptimizer::getCurrentFPS() const {
    return averageFrameTime > 0.0 ? static_cast<int>(1000.0 / averageFrameTime) : 0;
}

void RenderOptimizer::updateFrameTimeAverage(double frameTime) {
    frameTimes.add(frameTime);
    
    if (frameTimes.size() > 60) {
        frameTimes.removeRange(0, frameTimes.size() - 60);
    }
    
    double total = 0.0;
    for (auto time : frameTimes) {
        total += time;
    }
    
    averageFrameTime = total / frameTimes.size();
}
