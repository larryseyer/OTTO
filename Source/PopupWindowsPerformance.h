#pragma once
#include <JuceHeader.h>
#include "ColorScheme.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ComponentState.h"
#include "INIConfig.h"

class PerformanceTab : public juce::Component,
                      public juce::Timer,
                      public juce::Slider::Listener,
                      public juce::Button::Listener {
public:
    PerformanceTab(ColorScheme& colorScheme,
                   ResponsiveLayoutManager& layoutManager,
                   FontManager& fontManager);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

    void savePerformanceSettings(ComponentState& state);
    void loadPerformanceSettings(const ComponentState& state);

private:
    ColorScheme& colorScheme;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;

    juce::Label cpuLabel, memoryLabel, voicesLabel, latencyLabel;
    juce::Label cpuValueLabel, memoryValueLabel, voicesValueLabel, latencyValueLabel;
    juce::ProgressBar cpuMeter, memoryMeter;

    juce::Slider threadCountSlider, maxVoicesSlider, qualitySlider;
    juce::Label threadCountLabel, maxVoicesLabel, qualityLabel;
    juce::ToggleButton multiThreadToggle, gpuAccelToggle;

    juce::TextButton resetMetersButton, optimizeButton;

    double cpuUsage = INIConfig::Validation::MIN_VOLUME;
    double memoryUsage = INIConfig::Validation::MIN_VOLUME;
    int activeVoices = INIConfig::Defaults::ZERO_VALUE;
    double latency = INIConfig::Validation::MIN_VOLUME;

    void setupComponents();
    void updateMeters();
    void optimizeSettings();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PerformanceTab)
};
