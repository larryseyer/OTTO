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
    
    // TEMPORARY: Add debug label for Row 2 identification
    addAndMakeVisible(row2DebugLabel);
    row2DebugLabel.setText("ROW 2", juce::dontSendNotification);
    row2DebugLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    row2DebugLabel.setColour(juce::Label::backgroundColourId, juce::Colours::yellow);
    row2DebugLabel.setJustificationType(juce::Justification::centred);
    row2DebugLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, 32.0f));
}

void PlayerTabsComponent::setupTabs() {
    using namespace INIConfig::LayoutConstants;
    
    for (int i = 0; i < Row2::tabsCount; ++i) {
        addAndMakeVisible(tabs[i]);
        tabs[i].setButtonText("PLAYER " + juce::String(i + 1));

        // Configure with ColorScheme integration (fonts handled by LookAndFeel)
        tabs[i].setColour(juce::TextButton::buttonColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        tabs[i].setColour(juce::TextButton::buttonOnColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundToggled));
        tabs[i].setColour(juce::TextButton::textColourOnId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        tabs[i].setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));

        tabs[i].setClickingTogglesState(true);
        tabs[i].setRadioGroupId(playerTabsRadioGroupId);

        tabs[i].onClick = [this, i] {
            setSelectedTab(i);
        };
    }

    tabs[0].setToggleState(true, juce::dontSendNotification);
}

void PlayerTabsComponent::setSelectedTab(int tab) {
    using namespace INIConfig::LayoutConstants;
    
    if (tab >= 0 && tab < Row2::tabsCount && tab != selectedTab) {
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
    using namespace INIConfig::LayoutConstants;
    
    // Update colors when look and feel changes (fonts handled by LookAndFeel)
    for (int i = 0; i < Row2::tabsCount; ++i) {
        tabs[i].setColour(juce::TextButton::buttonColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        tabs[i].setColour(juce::TextButton::buttonOnColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundToggled));
        tabs[i].setColour(juce::TextButton::textColourOnId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        tabs[i].setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        
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
    using namespace INIConfig::LayoutConstants;
    
    for (int i = 0; i < Row2::tabsCount; ++i) {
        tabs[i].setToggleState(i == selectedTab, juce::dontSendNotification);

        // Update colors using ColorScheme (fonts handled by LookAndFeel)
        tabs[i].setColour(juce::TextButton::buttonColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        tabs[i].setColour(juce::TextButton::buttonOnColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundToggled));
        tabs[i].setColour(juce::TextButton::textColourOnId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        tabs[i].setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
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
    using namespace INIConfig::LayoutConstants;
    
    for (int i = 0; i < Row2::tabsCount; ++i) {
        if (INIConfig::isValidPlayerIndex(i)) {
            updateTabText(i, state.playerSettings[i]);
        }
    }

    updateTabVisuals();
    repaint();
}

void PlayerTabsComponent::setClipLaunchMode(bool enabled) {
    using namespace INIConfig::LayoutConstants;
    
    clipLaunchMode = enabled;

    for (int i = 0; i < Row2::tabsCount; ++i) {
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
    using namespace INIConfig::LayoutConstants;
    
    if (tabIndex >= 0 && tabIndex < Row2::tabsCount) {
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
    using namespace INIConfig::LayoutConstants;
    
    if (tabIndex >= 0 && tabIndex < Row2::tabsCount) {
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
    // Use ColorScheme for background
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));

    auto bounds = getLocalBounds();
    
    // Use Row2 namespace constants for highlight positioning
    using namespace INIConfig::LayoutConstants;
    int tabWidth = layoutManager.scaled(Row2::tabWidth);
    int leftMargin = layoutManager.scaled(Row2::leftMargin);
    int tabSpacing = layoutManager.scaled(Row2::tabSpacing);
    int highlightHeight = layoutManager.scaled(Row2::highlightHeight);
    int highlightMargin = layoutManager.scaled(Row2::highlightMargin);
    int highlightWidthReduction = layoutManager.scaled(Row2::highlightWidthReduction);
    
    // Draw highlight for selected tab using ColorScheme
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Accent));
    
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
    
    // Use Row2 namespace constants for positioning  
    using namespace INIConfig::LayoutConstants;
    int tabWidth = layoutManager.scaled(Row2::tabWidth);
    int leftMargin = layoutManager.scaled(Row2::leftMargin);
    int tabSpacing = layoutManager.scaled(Row2::tabSpacing);
    int tabTopOffset = layoutManager.scaled(Row2::tabTopOffset);
    int tabContentHeight = layoutManager.scaled(Row2::tabContentHeight);

    // Position each player tab using percentage-based layout:
    // Button 1: [4.5% margin][Button1] (left margin)
    // Buttons 2-7: [2.25% margin][Button] (spacing between tabs)
    // Button 8: [2.25% margin][Button8][4.5% margin] (right margin)
    int currentX = leftMargin;
    for (int i = 0; i < Row2::tabsCount; ++i) {
        tabs[i].setBounds(currentX, 
                         tabTopOffset, 
                         tabWidth, 
                         tabContentHeight);
        
        // Move to next button position using percentage-based spacing
        currentX += tabWidth;
        if (i < Row2::tabsCount - 1) {
            currentX += tabSpacing; // Add spacing before next tab
        }
    }

    bottomSeparator.setBounds(0, bounds.getBottom() - layoutManager.scaled(separatorThickness),
                             bounds.getWidth(), layoutManager.scaled(separatorThickness));
    
    // TEMPORARY: Position Row 2 debug label on right side for visibility
    row2DebugLabel.setBounds(bounds.getWidth() - 120 - 20, layoutManager.scaled(defaultPadding), 
                            120, 50);
}

// PHASE 4: Removed calculatePlayerButtonLayout method
// Now using percentage-based constants from INIConfig::LayoutConstants::Row2
