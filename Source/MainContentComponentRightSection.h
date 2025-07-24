#pragma once
#include <JuceHeader.h>
#include "UtilityComponents.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ColorScheme.h"
#include "ComponentState.h"
#include "INIConfig.h"

class MidiEngine;
class Mixer;

class MainContentComponentRightSection : public juce::Component {
public:
    MainContentComponentRightSection(MidiEngine& midiEngine,
                                   Mixer& mixer,
                                   juce::AudioProcessorValueTreeState& valueTreeState,
                                   ResponsiveLayoutManager& layoutManager,
                                   FontManager& fontManager,
                                   ColorScheme& colorScheme);

    ~MainContentComponentRightSection() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);
    void updatePlayerState(int playerIndex, const ComponentState& state);
    void updateCurrentPlayerFromState(const ComponentState& state);
    void saveCurrentPlayerToState(ComponentState& state);

    float getSwingValue() const { return static_cast<float>(swingSlider.getValue()); }
    float getEnergyValue() const { return static_cast<float>(energySlider.getValue()); }
    float getVolumeValue() const { return static_cast<float>(volumeSlider.getValue()); }

    void setSwingValue(float value) { swingSlider.setValue(value); }
    void setEnergyValue(float value) { energySlider.setValue(value); }
    void setVolumeValue(float value) { volumeSlider.setValue(value); }

    bool getToggleState(int index) const;
    void setToggleState(int index, bool state);
    bool getFillState(int index) const;
    void setFillState(int index, bool state);

    void setCurrentPlayerIndex(int index) { currentPlayerIndex = index; }

    std::function<void(const juce::String&, float)> onSliderValueChanged;
    std::function<void(int, const juce::String&, float)> onPlayerSliderValueChanged;
    std::function<void(int, int, bool)> onPlayerToggleChanged;
    std::function<void(int, int, bool)> onPlayerFillChanged;

private:
    MidiEngine& midiEngine;
    Mixer& mixer;
    juce::AudioProcessorValueTreeState& valueTreeState;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;

    juce::Label togglesLabel;
    juce::Label fillsLabel;
    juce::Label swingLabel;
    juce::Label energyLabel;
    juce::Label volumeLabel;

    juce::TextButton toggleButtons[INIConfig::UI::MAX_TOGGLE_STATES];
    juce::TextButton fillButtons[INIConfig::UI::MAX_FILL_STATES];

    juce::Slider swingSlider;
    juce::Slider energySlider;
    juce::Slider volumeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;

    SeparatorComponent middleSeparator;

    int currentPlayerIndex = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;
    bool toggleStates[INIConfig::UI::MAX_TOGGLE_STATES] = {};
    bool fillStates[INIConfig::UI::MAX_FILL_STATES] = {};

    void setupComponents();
    void setupSliders();
    void updateSliderCallbacks();
    void updateButtonCallbacks();
    void validateSliderValues();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponentRightSection)
};
