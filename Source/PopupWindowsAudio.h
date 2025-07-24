#pragma once
#include <JuceHeader.h>
#include "ColorScheme.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ComponentState.h"
#include "INIConfig.h"

class AudioSettingsTab : public juce::Component,
                        public juce::ComboBox::Listener,
                        public juce::Button::Listener {
public:
    AudioSettingsTab(ColorScheme& colorScheme,
                    ResponsiveLayoutManager& layoutManager,
                    FontManager& fontManager,
                    juce::AudioDeviceManager* deviceManager);

    void resized() override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void buttonClicked(juce::Button* button) override;

    void saveToAudioSettings(AudioSettings& settings);
    void loadFromAudioSettings(const AudioSettings& settings);

private:
    ColorScheme& colorScheme;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    juce::AudioDeviceManager* deviceManager;

    juce::Label deviceLabel, sampleRateLabel, bufferSizeLabel;
    juce::ComboBox deviceCombo, sampleRateCombo, bufferSizeCombo;
    juce::ToggleButton asioToggle, multiCoreToggle;
    juce::TextButton testAudioButton;
    juce::Label statusLabel;

    void setupComponents();
    void updateDeviceList();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSettingsTab)
};
