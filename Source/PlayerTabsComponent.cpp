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
    for (int i = 0; i < INIConfig::LayoutConstants::Row2::tabsCount; ++i) {
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
    if (tab >= 0 && tab < INIConfig::LayoutConstants::Row2::tabsCount && tab != selectedTab) {
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
    for (int i = 0; i < INIConfig::LayoutConstants::Row2::tabsCount; ++i) {
        tabs[i].repaint();
    }
    repaint();

    updateTabVisuals();
}

void PlayerTabsComponent::parentSizeChanged() {
    // JUCE 8 compatible responsive layout update
    resized();
    repaint();
}

void PlayerTabsComponent::updateTabVisuals() {
    for (int i = 0; i < INIConfig::LayoutConstants::Row2::tabsCount; ++i) {
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
    for (int i = 0; i < INIConfig::LayoutConstants::Row2::tabsCount; ++i) {
        if (INIConfig::isValidPlayerIndex(i)) {
            updateTabText(i, state.playerSettings[i]);
        }
    }

    updateTabVisuals();
    repaint();
}

void PlayerTabsComponent::setClipLaunchMode(bool enabled) {
    clipLaunchMode = enabled;

    for (int i = 0; i < INIConfig::LayoutConstants::Row2::tabsCount; ++i) {
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
    if (tabIndex >= 0 && tabIndex < INIConfig::LayoutConstants::Row2::tabsCount) {
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
    if (tabIndex >= 0 && tabIndex < INIConfig::LayoutConstants::Row2::tabsCount) {
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
    
    // PHASE 4: Use percentage-based Row 2 constants for highlight positioning
    // ======================================================================
    int tabWidth = layoutManager.scaled(INIConfig::LayoutConstants::Row2::tabWidth);
    int leftMargin = layoutManager.scaled(INIConfig::LayoutConstants::Row2::leftMargin);
    int tabSpacing = layoutManager.scaled(INIConfig::LayoutConstants::Row2::tabSpacing);
    int highlightHeight = layoutManager.scaled(INIConfig::LayoutConstants::Row2::highlightHeight);
    int highlightMargin = layoutManager.scaled(INIConfig::LayoutConstants::Row2::highlightMargin);
    int highlightWidthReduction = layoutManager.scaled(INIConfig::LayoutConstants::Row2::highlightWidthReduction);
    
    // Draw highlight for selected tab
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    
    // Calculate selected button position using percentage-based layout
    int selectedButtonX = leftMargin;
    for (int i = 0; i < selectedTab; ++i) {
        selectedButtonX += tabWidth + tabSpacing;
    }
    
    g.fillRect(selectedButtonX + highlightMargin,
               bounds.getBottom() - highlightHeight,
               tabWidth - highlightWidthReduction,
               highlightHeight);
}

void PlayerTabsComponent::resized() {
    auto bounds = getLocalBounds();
    
    // PHASE 4: Use percentage-based Row 2 constants for positioning
    // ============================================================
    int tabWidth = layoutManager.scaled(INIConfig::LayoutConstants::Row2::tabWidth);
    int leftMargin = layoutManager.scaled(INIConfig::LayoutConstants::Row2::leftMargin);
    int tabSpacing = layoutManager.scaled(INIConfig::LayoutConstants::Row2::tabSpacing);
    int tabTopOffset = layoutManager.scaled(INIConfig::LayoutConstants::Row2::tabTopOffset);
    int tabContentHeight = layoutManager.scaled(INIConfig::LayoutConstants::Row2::tabContentHeight);

    // Position each player tab using percentage-based layout:
    // Button 1: [4.5% margin][Button1] (left margin)
    // Buttons 2-7: [2.25% margin][Button] (spacing between tabs)
    // Button 8: [2.25% margin][Button8][4.5% margin] (right margin)
    int currentX = leftMargin;
    for (int i = 0; i < INIConfig::LayoutConstants::Row2::tabsCount; ++i) {
        tabs[i].setBounds(currentX, 
                         tabTopOffset, 
                         tabWidth, 
                         tabContentHeight);
        
        // Move to next button position using percentage-based spacing
        currentX += tabWidth;
        if (i < INIConfig::LayoutConstants::Row2::tabsCount - 1) {
            currentX += tabSpacing; // Add spacing before next tab
        }
    }

    bottomSeparator.setBounds(0, bounds.getBottom() - layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness),
                             bounds.getWidth(), layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness));
}

// PHASE 4: Removed calculatePlayerButtonLayout method
// Now using percentage-based constants from INIConfig::LayoutConstants::Row2
