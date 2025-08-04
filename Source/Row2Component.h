#pragma once
#include "RowComponentBase.h"
#include "UtilityComponents.h"
#include "INIConfig.h"

class MidiEngine;
struct PlayerSettings;

class Row2Component : public RowComponentBase {
public:
    Row2Component(MidiEngine& midiEngine,
                 juce::AudioProcessorValueTreeState& valueTreeState,
                 ResponsiveLayoutManager& layoutManager,
                 FontManager& fontManager,
                 ColorScheme& colorScheme);
    
    ~Row2Component() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void saveStates(ComponentState& state) override;
    void loadStates(const ComponentState& state) override;
    void updateFromState(const ComponentState& state) override;
    juce::String getRowName() const override { return "PlayerTabs"; }
    juce::Rectangle<int> getRowBounds() const override;
    void lookAndFeelChanged() override;
    void parentSizeChanged() override;
    
    // Player tab functionality
    int getSelectedTab() const { return selectedTab; }
    void setSelectedTab(int tab);
    void setClipLaunchMode(bool enabled);
    bool isClipLaunchMode() const { return clipLaunchMode; }
    void highlightQueuedTab(int tabIndex, bool highlight);
    void showClipState(int tabIndex, bool hasClip, bool isPlaying);
    
    // Callback functions
    std::function<void(int)> onTabChanged;
    std::function<void(int)> onClipTriggered;
    
private:
    MidiEngine& midiEngine;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    // Player tab components
    juce::TextButton tabs[INIConfig::Defaults::MAX_PLAYERS];
    SeparatorComponent bottomSeparator;
    juce::Label playerNumber;
    
    // State variables
    int selectedTab = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;
    bool clipLaunchMode = false;
    bool tabQueuedStates[INIConfig::Defaults::MAX_PLAYERS] = {false};
    bool tabHasClip[INIConfig::Defaults::MAX_PLAYERS] = {false};
    bool tabIsPlaying[INIConfig::Defaults::MAX_PLAYERS] = {false};
    
    void setupPlayerTabsComponents();
    void updatePlayerTabsLayout();
    void setupTabs();
    void updateTabVisuals();
    void updateTabText(int tabIndex, const PlayerSettings& player);
    void handleTabClick(int tabIndex);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Row2Component)
};
