#pragma once

#include <JuceHeader.h>
#include "PopupWindows.h"
#include "Mixer.h"
#include "ColorScheme.h"
#include "FontManager.h"
#include "ResponsiveLayoutManager.h"
#include "INIDataManager.h"

class MixerWindow : public PopupWindow {
public:
    MixerWindow(Mixer& mixer,
               INIDataManager& iniManager, 
               ColorScheme& colorScheme, 
               FontManager& fontManager);
    ~MixerWindow() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void showMixer();
    void hideMixer();
    bool isMixerVisible() const;

private:
    Mixer& mixer;
    INIDataManager& iniDataManager;
    ColorScheme& colorScheme;
    FontManager& fontManager;
    
    juce::Array<std::unique_ptr<juce::Slider>> channelSliders;
    juce::Array<std::unique_ptr<juce::Label>> channelLabels;
    std::unique_ptr<juce::Slider> masterSlider;
    std::unique_ptr<juce::Label> masterLabel;
    
    void setupWindow();
    void setupMixerControls();
    void updateLayout();
    
    static constexpr int NUM_CHANNELS = 8;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerWindow)
};
