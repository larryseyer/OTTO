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
class LoopSectionComponent;

class MainContentComponent : public juce::Component,
                              public juce::Button::Listener,
                              public juce::ComboBox::Listener {
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
    void lookAndFeelChanged() override;
    
    // juce::Button::Listener
    void buttonClicked(juce::Button* button) override;
    
    // juce::ComboBox::Listener
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

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
    
    void updateDrumKitList(const juce::StringArray& drumkitNames);
    void setSelectedDrumKit(const juce::String& drumkitName);
    void updateDrumKitDisplayToggle();
    void showDrumKitLabel();
    void showDrumKitMenu();
    
    void mouseDown(const juce::MouseEvent& event) override;

    std::function<void(bool)> onEditModeChanged;
    std::function<void(int, const juce::String&)> onMidiFileChanged;
    std::function<void(const juce::String&, float)> onSliderValueChanged;
    std::function<void()> onGrooveAndFillsRequested;
    std::function<void(ComponentState&)> onStateChanged;
    std::function<void()> onDrumKitPopupRequested;
    std::function<void()> onMixerPopupRequested;
    std::function<void(const juce::String&)> onDrumKitChanged;

private:
    MidiEngine& midiEngine;
    Mixer& mixer;
    juce::AudioProcessorValueTreeState& valueTreeState;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;

    juce::Label rhythmLabel;

    
    // TEMPORARY: Row identification labels for debugging layout
    juce::Label rowLabel1, rowLabel2, rowLabel3, rowLabel4, rowLabel5, rowLabel6;
    
    // Row 3: Player & DrumKit Controls
    PhosphorIconButton drumKitEditButton;
    PhosphorIconButton drumKitLeftChevron;
    HierarchicalComboBox drumKitDropdown;
    PhosphorIconButton drumKitRightChevron;
    PhosphorIconButton drumKitMuteButton;
    PhosphorIconButton drumKitMixerButton;
    juce::Label drumKitSelectedLabel;  // Shows currently selected drumkit name
    
    bool showingDrumKitLabel = true;  // Toggle state: true = show label, false = show menu
    
    // Row 4: Pattern Group Controls
    juce::Label patternGroupLabel;
    HierarchicalComboBox patternGroupDropdown;
    juce::Label patternStatusLabel;
    PhosphorIconButton patternAddButton;
    PhosphorIconButton patternDeleteButton;
    
    std::unique_ptr<MainContentComponentLeftSection> leftSection;
    std::unique_ptr<MainContentComponentRightSection> rightSection;
    std::unique_ptr<SceneLauncherComponent> sceneLauncher;
    std::unique_ptr<LoopSectionComponent> loopSection;
    // Row separators - one between each row for visual debugging
    SeparatorComponent row1Separator;  // Between Row 1 (TopBar) and Row 2 (Player Tabs)
    SeparatorComponent row2Separator;  // Between Row 2 (Player Tabs) and Row 3 (DrumKit Menu)
    SeparatorComponent row3Separator;  // Between Row 3 (DrumKit Menu) and Row 4 (Pattern Menu)
    SeparatorComponent row4Separator;  // Between Row 4 (Pattern Menu) and Row 5 (4x4 Matrix)
    SeparatorComponent row5Separator;  // Between Row 5 (4x4 Matrix) and Row 6 (Loop Controls)

    int currentPlayerIndex = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;
    INIDataManager* iniDataManager = nullptr;
    bool livePerformanceMode = false;

    void validatePlayerIndex(int playerIndex) const;

    void notifyStateChanged(ComponentState& state);
    void updateLayoutForPerformanceMode();
    void setupRow3Components();
    void setupRow4Components();
    void updateRow2Layout();
    void updateRow3Layout();
    void updateRow4Layout();
    void updateRow5Layout();
    void updateRow6Layout();
    void handleDrumKitChevrons(bool isRight);
    
#ifdef JUCE_DEBUG
    void performIntegrationValidation(const juce::Rectangle<int>& bounds);
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
