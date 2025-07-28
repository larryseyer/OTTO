#pragma once

#include <JuceHeader.h>
#include <tuple>
#include "UtilityComponents.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ColorScheme.h"
#include "MidiEngine.h"
#include "ComponentState.h"
#include "INIConfig.h"

class PlayerTabsComponent : public juce::Component {
public:
    PlayerTabsComponent(MidiEngine& midiEngine,
                       ResponsiveLayoutManager& layoutManager,
                       FontManager& fontManager,
                       ColorScheme& colorScheme);

    void paint(juce::Graphics&) override;
    void resized() override;

    int getSelectedTab() const { return selectedTab; }
    void setSelectedTab(int tab);

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    void updateTabsFromState(const ComponentState& state);

    void setClipLaunchMode(bool enabled);
    bool isClipLaunchMode() const { return clipLaunchMode; }
    void highlightQueuedTab(int tabIndex, bool highlight);
    void showClipState(int tabIndex, bool hasClip, bool isPlaying);

    void lookAndFeelChanged() override;
    void parentSizeChanged() override;

    std::function<void(int)> onTabChanged;
    std::function<void(int)> onClipTriggered;

private:
    MidiEngine& midiEngine;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;

    juce::TextButton tabs[INIConfig::Defaults::MAX_PLAYERS];
    SeparatorComponent bottomSeparator;

    int selectedTab = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;
    bool clipLaunchMode = false;
    bool tabQueuedStates[INIConfig::Defaults::MAX_PLAYERS] = {false};
    bool tabHasClip[INIConfig::Defaults::MAX_PLAYERS] = {false};
    bool tabIsPlaying[INIConfig::Defaults::MAX_PLAYERS] = {false};

    void setupTabs();
    void updateTabVisuals();
    void updateTabText(int tabIndex, const PlayerSettings& player);
    void handleTabClick(int tabIndex);
    
    // JUCE 8 compatible layout calculation for player buttons
    std::tuple<float, float, int> calculatePlayerButtonLayout(int availableWidth);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerTabsComponent)
};
