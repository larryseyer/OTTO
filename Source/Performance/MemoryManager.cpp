#include "MemoryManager.h"

MemoryManager::MemoryManager() {
}

void MemoryManager::trackAllocation(size_t size) {
    currentMemoryUsage += size;
    if (currentMemoryUsage > peakMemoryUsage) {
        peakMemoryUsage = currentMemoryUsage;
    }
}

void MemoryManager::trackDeallocation(size_t size) {
    if (currentMemoryUsage >= size) {
        currentMemoryUsage -= size;
    }
}

bool MemoryManager::isMemoryUsageWithinTarget() const {
    return currentMemoryUsage <= targetMemoryLimit;
}

void MemoryManager::logMemoryStats() const {
    DBG("Current memory usage: " << (currentMemoryUsage / 1024 / 1024) << " MB");
    DBG("Peak memory usage: " << (peakMemoryUsage / 1024 / 1024) << " MB");
    DBG("Target limit: " << (targetMemoryLimit / 1024 / 1024) << " MB");
}
