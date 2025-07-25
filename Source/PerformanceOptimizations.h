#pragma once
#include <JuceHeader.h>
#include <unordered_map>
#include <string>

class StringCache {
public:
    static StringCache& getInstance() {
        static StringCache instance;
        return instance;
    }
    
    const juce::String& getPercentageString(int percentage) {
        auto it = percentageCache.find(percentage);
        if (it != percentageCache.end()) {
            return it->second;
        }
        
        auto result = percentageCache.emplace(percentage, juce::String(percentage) + "%");
        return result.first->second;
    }
    
    const juce::String& getPlayerString(int playerNumber) {
        auto it = playerCache.find(playerNumber);
        if (it != playerCache.end()) {
            return it->second;
        }
        
        auto result = playerCache.emplace(playerNumber, juce::String("PLAYER ") + juce::String(playerNumber));
        return result.first->second;
    }
    
    const juce::String& getGroupString(int groupNumber) {
        auto it = groupCache.find(groupNumber);
        if (it != groupCache.end()) {
            return it->second;
        }
        
        auto result = groupCache.emplace(groupNumber, juce::String("Group ") + juce::String(groupNumber));
        return result.first->second;
    }
    
    const juce::String& getPatternString(int patternNumber) {
        auto it = patternCache.find(patternNumber);
        if (it != patternCache.end()) {
            return it->second;
        }
        
        auto result = patternCache.emplace(patternNumber, juce::String("Pattern ") + juce::String(patternNumber));
        return result.first->second;
    }
    
    juce::String getLatencyString(double latency, int decimalPlaces = 1) {
        int key = static_cast<int>(latency * 10);
        auto it = latencyCache.find(key);
        if (it != latencyCache.end()) {
            return it->second;
        }
        
        juce::String result = juce::String(latency, decimalPlaces) + " ms";
        if (latencyCache.size() < 1000) {
            latencyCache[key] = result;
        }
        return result;
    }
    
    void clearCache() {
        percentageCache.clear();
        playerCache.clear();
        groupCache.clear();
        patternCache.clear();
        latencyCache.clear();
    }

private:
    StringCache() {
        for (int i = 0; i <= 100; ++i) {
            percentageCache[i] = juce::String(i) + "%";
        }
        
        for (int i = 1; i <= 8; ++i) {
            playerCache[i] = juce::String("PLAYER ") + juce::String(i);
            groupCache[i] = juce::String("Group ") + juce::String(i);
        }
        
        for (int i = 1; i <= 50; ++i) {
            patternCache[i] = juce::String("Pattern ") + juce::String(i);
        }
    }
    
    std::unordered_map<int, juce::String> percentageCache;
    std::unordered_map<int, juce::String> playerCache;
    std::unordered_map<int, juce::String> groupCache;
    std::unordered_map<int, juce::String> patternCache;
    std::unordered_map<int, juce::String> latencyCache;
};

class FastStringBuilder {
public:
    FastStringBuilder() {
        buffer.reserve(256);
    }
    
    FastStringBuilder& append(const juce::String& str) {
        buffer += str.toStdString();
        return *this;
    }
    
    FastStringBuilder& append(const char* str) {
        buffer += str;
        return *this;
    }
    
    FastStringBuilder& append(int value) {
        buffer += std::to_string(value);
        return *this;
    }
    
    FastStringBuilder& append(float value, int decimalPlaces = 2) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(decimalPlaces) << value;
        buffer += oss.str();
        return *this;
    }
    
    FastStringBuilder& append(double value, int decimalPlaces = 2) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(decimalPlaces) << value;
        buffer += oss.str();
        return *this;
    }
    
    juce::String toString() const {
        return juce::String(buffer);
    }
    
    void clear() {
        buffer.clear();
    }
    
    void reserve(size_t capacity) {
        buffer.reserve(capacity);
    }

private:
    std::string buffer;
};

template<typename KeyType>
class GenericCache {
public:
    GenericCache(size_t maxSize = 1000) : maxSize(maxSize) {}
    
    bool get(const KeyType& key, juce::String& value) {
        auto it = cache.find(key);
        if (it != cache.end()) {
            value = it->second;
            return true;
        }
        return false;
    }
    
    void put(const KeyType& key, const juce::String& value) {
        if (cache.size() >= maxSize) {
            auto it = cache.begin();
            cache.erase(it);
        }
        cache[key] = value;
    }
    
    void clear() {
        cache.clear();
    }
    
    size_t size() const {
        return cache.size();
    }

private:
    std::unordered_map<KeyType, juce::String> cache;
    size_t maxSize;
};

class DrumkitSearchOptimizer {
public:
    static DrumkitSearchOptimizer& getInstance() {
        static DrumkitSearchOptimizer instance;
        return instance;
    }
    
    void indexDrumkits(const juce::Array<juce::String>& drumkitNames) {
        drumkitIndex.clear();
        for (int i = 0; i < drumkitNames.size(); ++i) {
            drumkitIndex[drumkitNames[i].toLowerCase()] = i;
        }
    }
    
    int findDrumkitIndex(const juce::String& drumkitName) {
        auto it = drumkitIndex.find(drumkitName.toLowerCase());
        return (it != drumkitIndex.end()) ? it->second : -1;
    }
    
    void clearIndex() {
        drumkitIndex.clear();
    }
    
    size_t getIndexSize() const {
        return drumkitIndex.size();
    }

private:
    std::unordered_map<juce::String, int> drumkitIndex;
};
