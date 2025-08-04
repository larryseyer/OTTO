#pragma once
#include <JuceHeader.h>
#include "../INIConfig.h"

class MemoryManager {
public:
    MemoryManager();
    
    void trackAllocation(size_t size);
    void trackDeallocation(size_t size);
    
    size_t getCurrentMemoryUsage() const { return currentMemoryUsage; }
    size_t getPeakMemoryUsage() const { return peakMemoryUsage; }
    
    bool isMemoryUsageWithinTarget() const;
    void logMemoryStats() const;
    
private:
    static constexpr size_t targetMemoryLimit = 50 * 1024 * 1024;
    
    size_t currentMemoryUsage = 0;
    size_t peakMemoryUsage = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MemoryManager)
};
