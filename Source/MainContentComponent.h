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
class INIDataManager;
class MainContentComponentLeftSection;
class MainContentComponentRightSection;
class SceneLauncherComponent;

class MainContentComponent : public juce::Component {
public:
    MainContentComponent(MidiEngine& midiEngine,
                        Mixer& mixer,
                        juce::AudioProcessorValueTreeState& valueTreeState,
                        ResponsiveLayoutManager& layoutManager,
                        FontManager& fontManager,
                        ColorScheme& colorScheme);

    ~MainContentComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void updatePlayerDisplay(int playerIndex);

    float getSwingValue() const;
    void setSwingValue(float value);
    float getEnergyValue() const;
    void setEnergyValue(float value);
    float getVolumeValue() const;
    void setVolumeValue(float value);

    bool getEditMode() const;
    void setEditModeVisuals(bool isEditing);
    juce::String getMidiFileAssignment(int buttonIndex) const;
    void setMidiFileAssignment(int buttonIndex, const juce::String& midiFile);

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    int getFavoritesSelection() const;
    void setFavoritesSelection(int selection);

    void setINIDataManager(INIDataManager* manager) { iniDataManager = manager; }

    void updateFromState(const ComponentState& state);
    void switchToPlayer(int playerIndex, const ComponentState& state);

    MainContentComponentLeftSection* getLeftSection() const { return leftSection.get(); }
    MainContentComponentRightSection* getRightSection() const { return rightSection.get(); }

    void setLivePerformanceMode(bool enabled);
    bool isLivePerformanceMode() const { return livePerformanceMode; }
    void showSceneLauncher(bool show);

    std::function<void(bool)> onEditModeChanged;
    std::function<void(int, const juce::String&)> onMidiFileChanged;
    std::function<void(const juce::String&, float)> onSliderValueChanged;
    std::function<void()> onGrooveAndFillsRequested;
    std::function<void(ComponentState&)> onStateChanged;

private:
    MidiEngine& midiEngine;
    Mixer& mixer;
    juce::AudioProcessorValueTreeState& valueTreeState;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;

    juce::Label rhythmLabel;
    juce::Label playerNumber;
    std::unique_ptr<MainContentComponentLeftSection> leftSection;
    std::unique_ptr<MainContentComponentRightSection> rightSection;
    std::unique_ptr<SceneLauncherComponent> sceneLauncher;
    SeparatorComponent topSeparator;
    SeparatorComponent bottomSeparator;

    int currentPlayerIndex = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;
    INIDataManager* iniDataManager = nullptr;
    bool livePerformanceMode = false;

    void validatePlayerIndex(int playerIndex) const;
    void updatePlayerNumberDisplay();
    void notifyStateChanged(ComponentState& state);
    void updateLayoutForPerformanceMode();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
