#include "PlayerTabsComponent.h"
#include "INIConfig.h"

PlayerTabsComponent::PlayerTabsComponent(MidiEngine& midiEngine,
                                        ResponsiveLayoutManager& layoutManager,
                                        FontManager& fontManager,
                                        ColorScheme& colorScheme)
    : midiEngine(midiEngine), layoutManager(layoutManager),
      fontManager(fontManager), colorScheme(colorScheme),
      bottomSeparator(colorScheme) {

    setupTabs();
    addAndMakeVisible(bottomSeparator);
}

void PlayerTabsComponent::setupTabs() {
    for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
        addAndMakeVisible(tabs[i]);
        tabs[i].setButtonText("PLAYER " + juce::String(i + 1));

        tabs[i].setClickingTogglesState(true);
        tabs[i].setRadioGroupId(INIConfig::LayoutConstants::playerTabsRadioGroupId);

        tabs[i].onClick = [this, i] {
            setSelectedTab(i);
        };
    }

    tabs[0].setToggleState(true, juce::dontSendNotification);
}

void PlayerTabsComponent::setSelectedTab(int tab) {
    if (tab >= 0 && tab < INIConfig::LayoutConstants::playerTabsCount && tab != selectedTab) {
        selectedTab = INIConfig::clampPlayerIndex(tab);
        midiEngine.selectPattern(selectedTab, 0);

        updateTabVisuals();

        if (onTabChanged) {
            onTabChanged(selectedTab);
        }

        repaint();
    }
}

void PlayerTabsComponent::lookAndFeelChanged() {
    for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
        tabs[i].repaint();
    }
    repaint();

    updateTabVisuals();
}

void PlayerTabsComponent::updateTabVisuals() {
    for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
        tabs[i].setToggleState(i == selectedTab, juce::dontSendNotification);

        tabs[i].removeColour(juce::TextButton::buttonColourId);
        tabs[i].removeColour(juce::TextButton::textColourOnId);
        tabs[i].removeColour(juce::TextButton::textColourOffId);
    }
}

void PlayerTabsComponent::updateTabText(int tabIndex, const PlayerSettings& player) {
    if (!INIConfig::isValidPlayerIndex(tabIndex)) {
        return;
    }

    juce::String tabText = "PLAYER " + juce::String(tabIndex + 1);
    if (!player.enabled) {
        tabText += " (OFF)";
    }

    tabs[tabIndex].setButtonText(tabText);
}

void PlayerTabsComponent::saveStates(ComponentState& state) {
    state.currentPlayer = INIConfig::clampPlayerIndex(selectedTab);
}

void PlayerTabsComponent::loadStates(const ComponentState& state) {
    int tabToSelect = INIConfig::clampPlayerIndex(state.currentPlayer);

    updateTabsFromState(state);

    setSelectedTab(tabToSelect);
}

void PlayerTabsComponent::updateTabsFromState(const ComponentState& state) {
    for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
        if (INIConfig::isValidPlayerIndex(i)) {
            updateTabText(i, state.playerSettings[i]);
        }
    }

    updateTabVisuals();
    repaint();
}

void PlayerTabsComponent::setClipLaunchMode(bool enabled) {
    clipLaunchMode = enabled;

    for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
        tabs[i].onClick = [this, i] {
            handleTabClick(i);
        };
    }
}

void PlayerTabsComponent::handleTabClick(int tabIndex) {
    if (clipLaunchMode) {
        if (onClipTriggered) {
            onClipTriggered(tabIndex);
        }
    } else {
        setSelectedTab(tabIndex);
    }
}

void PlayerTabsComponent::highlightQueuedTab(int tabIndex, bool highlight) {
    if (tabIndex >= 0 && tabIndex < INIConfig::LayoutConstants::playerTabsCount) {
        tabQueuedStates[tabIndex] = highlight;

        if (highlight) {
            tabs[tabIndex].setToggleState(true, juce::dontSendNotification);
        } else {
            tabs[tabIndex].setToggleState(tabIndex == selectedTab, juce::dontSendNotification);
        }
        tabs[tabIndex].repaint();
    }
}

void PlayerTabsComponent::showClipState(int tabIndex, bool hasClip, bool isPlaying) {
    if (tabIndex >= 0 && tabIndex < INIConfig::LayoutConstants::playerTabsCount) {
        tabHasClip[tabIndex] = hasClip;
        tabIsPlaying[tabIndex] = isPlaying;

        if (isPlaying) {
            tabs[tabIndex].setToggleState(true, juce::dontSendNotification);
        } else {
            tabs[tabIndex].setToggleState(tabIndex == selectedTab, juce::dontSendNotification);
        }
        tabs[tabIndex].repaint();
    }
}

void PlayerTabsComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::transparentBlack);

    auto bounds = getLocalBounds();
    int tabWidth = layoutManager.scaled(INIConfig::LayoutConstants::playerTabWidth);
    int startX = (bounds.getWidth() - (INIConfig::LayoutConstants::playerTabsCount * tabWidth)) / 2;

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    g.fillRect(startX + selectedTab * tabWidth + layoutManager.scaled(INIConfig::LayoutConstants::playerTabHighlightMargin),
               bounds.getBottom() - layoutManager.scaled(INIConfig::LayoutConstants::playerTabHighlightHeight),
               tabWidth - layoutManager.scaled(INIConfig::LayoutConstants::playerTabHighlightWidthReduction),
               layoutManager.scaled(INIConfig::LayoutConstants::playerTabHighlightHeight));
}

void PlayerTabsComponent::resized() {
    auto bounds = getLocalBounds();
    int tabWidth = layoutManager.scaled(INIConfig::LayoutConstants::playerTabWidth);
    int startX = (bounds.getWidth() - (INIConfig::LayoutConstants::playerTabsCount * tabWidth)) / 2;
    int tabHeight = bounds.getHeight() - layoutManager.scaled(INIConfig::LayoutConstants::playerTabHeight);

    for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
        tabs[i].setBounds(startX + i * tabWidth, layoutManager.scaled(INIConfig::LayoutConstants::playerTabTopOffset), tabWidth, tabHeight);
    }

    bottomSeparator.setBounds(0, bounds.getBottom() - layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness),
                             bounds.getWidth(), layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness));
}
