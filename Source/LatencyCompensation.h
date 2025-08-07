#pragma once
#include <JuceHeader.h>

class LatencyCompensation {
public:
    static int getPlatformOptimalBufferSize() {
        #if JUCE_MAC
            return 256;
        #elif JUCE_IOS
            return 256;
        #elif JUCE_WINDOWS
            return 512;
        #elif JUCE_LINUX
            return 1024;
        #elif JUCE_ANDROID
            return 1024;
        #else
            return 512;
        #endif
    }

    static double getPlatformOptimalSampleRate() {
        #if JUCE_MAC || JUCE_IOS
            return 48000.0;
        #elif JUCE_WINDOWS
            return 44100.0;
        #elif JUCE_LINUX
            return 48000.0;
        #elif JUCE_ANDROID
            return 44100.0;
        #else
            return 44100.0;
        #endif
    }

    static int getPlatformLatencyCompensationSamples(double sampleRate) {
        #if JUCE_MAC
            return static_cast<int>(sampleRate * 0.005);
        #elif JUCE_WINDOWS
            return static_cast<int>(sampleRate * 0.010);
        #elif JUCE_LINUX
            return static_cast<int>(sampleRate * 0.015);
        #elif JUCE_ANDROID
            return static_cast<int>(sampleRate * 0.020);
        #else
            return static_cast<int>(sampleRate * 0.010);
        #endif
    }
};
