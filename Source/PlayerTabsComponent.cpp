/**
 * @file PlayerTabsComponent.cpp
 * @brief Implementation of OTTO's 8-player tab selection interface
 * 
 * This file implements the PlayerTabsComponent class, which provides the player
 * selection interface for OTTO's 8-track system. It occupies Row 2 of the main
 * layout (8% of interface height) and presents 8 selectable tabs for switching
 * between different drum pattern players/tracks.
 * 
 * LAYOUT ARCHITECTURE (Row 2 - 8% of interface height):
 * ======================================================
 * - 8 Player tabs arranged horizontally across the full width
 * - Each tab displays "PLAYER X" where X is the player number (1-8)
 * - Active tab highlighted with different color scheme
 * - Radio button behavior ensures only one tab active at a time
 * - Bottom separator provides visual division from Row 3
 * 
 * PLAYER SYSTEM INTEGRATION:
 * ===========================
 * Each player tab represents an independent drum pattern track with:
 * - Individual pattern sequences and timing
 * - Separate volume, swing, and energy controls
 * - Independent drum kit assignments
 * - Individual mute/solo capabilities
 * - Unique MIDI channel routing
 * 
 * RESPONSIVE DESIGN FEATURES:
 * ===========================
 * - Uses INIConfig::LayoutConstants::Row2 for all positioning
 * - Tab width calculated as percentage of interface width
 * - FontManager integration for consistent tab typography
 * - ColorScheme integration for theme-consistent styling
 * - ResponsiveLayoutManager for real-time scaling
 * 
 * VISUAL DESIGN ELEMENTS:
 * =======================
 * - Radio group behavior (only one tab selected)
 * - ColorScheme roles for consistent theming:
 *   * ButtonBackground: Default tab appearance
 *   * ButtonBackgroundToggled: Selected tab appearance
 *   * ButtonText: Tab text color in all states
 * - Bottom separator using theme colors
 * - Highlight indicator showing active selection
 * 
 * INTEGRATION POINTS:
 * ===================
 * - MidiEngine.cpp: Player switching and pattern management
 * - INIConfig.h: Row2 layout constants and tab specifications
 * - FontManager.cpp: Tab typography and text sizing
 * - ColorScheme.cpp: Theme-consistent visual styling
 * - ResponsiveLayoutManager.cpp: Dynamic scaling and positioning
 * - MainContentComponent.cpp: Receives player selection notifications
 * 
 * @author OTTO Development Team
 * @version 2.0
 * @date 2024
 * @see INIConfig.h for Row2 layout constants
 * @see MidiEngine.cpp for player management
 * @see ColorScheme.cpp for theming integration
 */

#include "PlayerTabsComponent.h"
#include "INIConfig.h"

/**
 * @brief PlayerTabsComponent constructor - initialize 8-player tab interface
 * 
 * Sets up the complete player selection system using Row 2 layout specifications
 * from INIConfig. Creates 8 player tabs with radio button behavior, configures
 * theme integration, and establishes callbacks for player switching.
 * 
 * INITIALIZATION SEQUENCE:
 * 1. Store references to all required subsystems
 * 2. Set up bottom separator for visual row division
 * 3. Call setupTabs() to create and configure all 8 player tabs
 * 4. Add temporary debug label for development identification
 * 5. Configure initial selection state (Player 1 active by default)
 * 
 * TAB SYSTEM DESIGN:
 * - 8 tabs total (matching OTTO's 8-player architecture)
 * - Radio group behavior ensures single selection
 * - ColorScheme integration for theme consistency
 * - Click callbacks trigger player switching in MidiEngine
 * 
 * DEBUG FEATURES:
 * - Temporary debug label for Row 2 identification during development
 * - Bright colors (red text, yellow background) for easy visual confirmation
 * - Large header font for clear visibility during layout testing
 * 
 * @param midiEngine Reference to MIDI processing and player management system
 * @param layoutManager Responsive layout calculation and scaling system
 * @param fontManager Typography management for tab text rendering
 * @param colorScheme Theme and color management for consistent styling
 * 
 * Called by: MainComponent.cpp during interface construction
 * References: INIConfig.h for Row2 constants, ColorScheme for theme colors
 */
PlayerTabsComponent::PlayerTabsComponent(MidiEngine& midiEngine,
                                        ResponsiveLayoutManager& layoutManager,
                                        FontManager& fontManager,
                                        ColorScheme& colorScheme)
    : midiEngine(midiEngine), layoutManager(layoutManager),
      fontManager(fontManager), colorScheme(colorScheme),
      bottomSeparator(colorScheme) {

    setupTabs();
    addAndMakeVisible(bottomSeparator);
    
    // Setup player number display with ColorScheme integration
    addAndMakeVisible(playerNumber);
    playerNumber.setText("1", juce::dontSendNotification);
    playerNumber.setJustificationType(juce::Justification::centred);
    playerNumber.setComponentID("player_number_display");  // Set ID for CustomLookAndFeel
    playerNumber.setColour(juce::Label::backgroundColourId, 
                          colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    playerNumber.setColour(juce::Label::textColourId, 
                          colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    
}

/**
 * @brief Set up all 8 player tabs with consistent styling and behavior
 * 
 * Creates and configures each of the 8 player tabs with proper ColorScheme
 * integration, radio button behavior, and click callbacks for player switching.
 * This method establishes the core tab functionality and visual appearance.
 * 
 * TAB CONFIGURATION PROCESS:
 * 1. Create each tab and add to component visibility
 * 2. Set descriptive button text ("PLAYER 1", "PLAYER 2", etc.)
 * 3. Apply ColorScheme colors for theme consistency
 * 4. Configure radio group behavior for single selection
 * 5. Set up click callbacks for player switching
 * 6. Set Player 1 as initially selected tab
 * 
 * COLOR SCHEME INTEGRATION:
 * - ButtonBackground: Default tab appearance (unselected state)
 * - ButtonBackgroundToggled: Active tab appearance (selected state)
 * - ButtonText: Text color for both selected and unselected states
 * - Colors automatically update when theme changes via ColorScheme
 * 
 * RADIO GROUP BEHAVIOR:
 * - All tabs share same radio group ID (playerTabsRadioGroupId)
 * - Clicking one tab automatically deselects others
 * - Ensures only one player can be active at a time
 * - Visual feedback shows current selection clearly
 * 
 * CALLBACK SYSTEM:
 * - Each tab's onClick lambda captures tab index
 * - Calls setSelectedTab(i) to handle player switching
 * - Enables integration with MidiEngine for pattern switching
 * 
 * Called by: Constructor during component initialization
 * References: INIConfig::LayoutConstants::Row2::tabsCount for tab count
 *            ColorScheme color roles for theme integration
 */
void PlayerTabsComponent::setupTabs() {
    using namespace INIConfig::LayoutConstants;
    
    // CREATE AND CONFIGURE ALL 8 PLAYER TABS
    for (int i = 0; i < Row2::tabsCount; ++i) {
        // Add tab to component hierarchy for rendering and event handling
        addAndMakeVisible(tabs[i]);
        
        // Set descriptive text for each player tab (1-indexed for user clarity)
        tabs[i].setButtonText("PLAYER " + juce::String(i + 1));

        // COLORSCHEME INTEGRATION: Apply theme colors for consistent appearance
        // These colors automatically update when user changes themes
        tabs[i].setColour(juce::TextButton::buttonColourId, 
                         colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        tabs[i].setColour(juce::TextButton::buttonOnColourId, 
                         colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundToggled));
        tabs[i].setColour(juce::TextButton::textColourOnId, 
                         colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        tabs[i].setColour(juce::TextButton::textColourOffId, 
                         colorScheme.getColor(ColorScheme::ColorRole::ButtonText));

        // RADIO GROUP CONFIGURATION: Ensure single selection behavior
        tabs[i].setClickingTogglesState(true);                    // Enable toggle mode
        tabs[i].setRadioGroupId(playerTabsRadioGroupId);         // Group all tabs together

        // CLICK CALLBACK: Handle player switching when tab is clicked
        tabs[i].onClick = [this, i] {
            setSelectedTab(i);  // Switch to selected player and update UI
        };
    }

    // SET INITIAL SELECTION: Player 1 active by default
    // dontSendNotification prevents triggering callbacks during initialization
    tabs[0].setToggleState(true, juce::dontSendNotification);
}

void PlayerTabsComponent::setSelectedTab(int tab) {
    using namespace INIConfig::LayoutConstants;
    
    if (tab >= 0 && tab < Row2::tabsCount && tab != selectedTab) {
        selectedTab = INIConfig::clampPlayerIndex(tab);
        midiEngine.selectPattern(selectedTab, 0);

        updateTabVisuals();
        
        // Update player number display
        playerNumber.setText(juce::String(selectedTab + 1), juce::dontSendNotification);

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
        tabs[i].setColour(juce::TextButton::textColourOnId, colorScheme.getColor(ColorScheme::ColorRole::IconButtonText));
        tabs[i].setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::IconButtonText));
        
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
        tabs[i].setColour(juce::TextButton::textColourOnId, colorScheme.getColor(ColorScheme::ColorRole::IconButtonText));
        tabs[i].setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::IconButtonText));
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
    // UPDATED: Calculate dynamic horizontal centering based on actual component width
    int totalTabsWidth = (Row2::tabsCount * layoutManager.scaled(Row2::tabWidth)) + ((Row2::tabsCount - 1) * layoutManager.scaled(Row2::tabSpacing));
    int leftMargin = (bounds.getWidth() - totalTabsWidth) / 2;
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
    // UPDATED: Calculate dynamic horizontal centering based on actual component width
    int totalTabsWidth = (Row2::tabsCount * layoutManager.scaled(Row2::tabWidth)) + ((Row2::tabsCount - 1) * layoutManager.scaled(Row2::tabSpacing));
    int leftMargin = (bounds.getWidth() - totalTabsWidth) / 2;
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
    
    // Position player number display in the center of the area to the left of the first tab
    // Use the available space from left edge to where the first tab starts (leftMargin)
    int availableLeftSpace = leftMargin;  // Space from 0 to first tab
    int playerNumWidth = static_cast<int>(availableLeftSpace * 0.8f);  // Use 80% of available space
    int playerNumHeight = layoutManager.scaled(Row2::height);  // Use FULL Row2 height, not tiny tabContentHeight
    int playerNumX = (availableLeftSpace - playerNumWidth) / 2;  // Center in available space
    int playerNumY = 0;  // Start at top of Row2
    
    // Position the player number - font will be handled by CustomLookAndFeel
    playerNumber.setBounds(playerNumX, playerNumY, playerNumWidth, playerNumHeight);
    
    // TEMPORARY: Position Row 2 debug label on right side for visibility
    // ROW 2 DEBUG REMOVED.setBounds(bounds.getWidth() - 120 - 20, layoutManager.scaled(defaultPadding), 
    // ROW 2 DEBUG REMOVED: 120, 50);
}

// PHASE 4: Removed calculatePlayerButtonLayout method
// Now using percentage-based constants from INIConfig::LayoutConstants::Row2
