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

void PlayerTabsComponent::parentSizeChanged() {
    // JUCE 8 compatible responsive layout update
    resized();
    repaint();
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
    
    // Calculate button dimensions and total width needed
    auto [buttonWidth, totalWidth, startX] = calculatePlayerButtonLayout(bounds.getWidth());
    
    // Draw highlight for selected tab
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    
    // Calculate selected button position with CORRECTED layout
    int selectedButtonX = startX;
    for (int i = 0; i < selectedTab; ++i) {
        // Move past current button + 1/4w margin to next button
        selectedButtonX += static_cast<int>(buttonWidth + (buttonWidth * 0.25f));
    }
    
    g.fillRect(selectedButtonX + layoutManager.scaled(INIConfig::LayoutConstants::playerTabHighlightMargin),
               bounds.getBottom() - layoutManager.scaled(INIConfig::LayoutConstants::playerTabHighlightHeight),
               static_cast<int>(buttonWidth) - layoutManager.scaled(INIConfig::LayoutConstants::playerTabHighlightWidthReduction),
               layoutManager.scaled(INIConfig::LayoutConstants::playerTabHighlightHeight));
}

void PlayerTabsComponent::resized() {
    auto bounds = getLocalBounds();
    
    // Calculate button dimensions and positioning
    auto [buttonWidth, totalWidth, startX] = calculatePlayerButtonLayout(bounds.getWidth());
    int tabHeight = bounds.getHeight() - layoutManager.scaled(INIConfig::LayoutConstants::playerTabHeight);

    // Position each player tab with CORRECTED layout:
    // Button 1: [1/2w margin][Button1] (starts at 0.5w from left)
    // Buttons 2-7: [1/4w margin][Button] (1/4w margin before each)
    // Button 8: [1/4w margin][Button8][1/2w margin] (1/4w margin before, 1/2w margin after)
    int currentX = startX;
    for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
        tabs[i].setBounds(currentX, 
                         layoutManager.scaled(INIConfig::LayoutConstants::playerTabTopOffset), 
                         static_cast<int>(buttonWidth), 
                         tabHeight);
        
        // Move to next button position:
        // - Add current button width
        // - Add 1/4w margin before next button (except for the last button)
        currentX += static_cast<int>(buttonWidth);
        if (i < INIConfig::LayoutConstants::playerTabsCount - 1) {
            currentX += static_cast<int>(buttonWidth * 0.25f); // 1/4w margin before next button
        }
    }

    bottomSeparator.setBounds(0, bounds.getBottom() - layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness),
                             bounds.getWidth(), layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness));
}

std::tuple<float, float, int> PlayerTabsComponent::calculatePlayerButtonLayout(int availableWidth) {
    const int numButtons = INIConfig::LayoutConstants::playerTabsCount;
    
    // Ensure minimum width to prevent division by zero or negative values
    if (availableWidth <= 0 || numButtons <= 0) {
        return std::make_tuple(0.0f, 0.0f, 0);
    }
    
    // CORRECTED Layout specification:
    // Button 1: [1/2w margin][Button1] (double margin on left only)
    // Buttons 2-7: [1/4w margin][Button] (single margin on left only) 
    // Button 8: [1/4w margin][Button8][1/2w margin] (single margin on left, double margin on right)
    // 
    // Layout: [1/2w][B1][1/4w][B2][1/4w][B3][1/4w][B4][1/4w][B5][1/4w][B6][1/4w][B7][1/4w][B8][1/2w]
    // Total width = 1/2w + 8×B + 7×(1/4w) + 1/2w = 1w + 8B + 1.75w = 2.75w + 8B
    // If B = w, then: availableWidth = 10.75w
    // So: w = availableWidth / 10.75
    
    float buttonWidth = static_cast<float>(availableWidth) / 10.75f;
    
    // Ensure minimum button width for usability
    buttonWidth = juce::jmax(buttonWidth, 16.0f);
    
    // Calculate total layout width
    float totalWidth = 10.75f * buttonWidth;
    
    // Button 1 starts after 1/2w margin from left edge
    int startX = static_cast<int>(buttonWidth * 0.5f);
    

    
    return std::make_tuple(buttonWidth, totalWidth, startX);
}
