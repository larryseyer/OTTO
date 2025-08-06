#include "JUCE8_CODING_STANDARDS.h"
#include "Row2Component.h"
#include "MidiEngine.h"
#include "INIConfig.h"
#include "ComponentState.h"
#include "ResponsiveLayoutManager.h"

Row2Component::Row2Component(MidiEngine& midiEngine,
                           juce::AudioProcessorValueTreeState& valueTreeState,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : ResponsiveComponent()
    , midiEngine(midiEngine)
    , valueTreeState(valueTreeState)
    , layoutManager(layoutManager)
    , fontManager(fontManager)
    , colorScheme(colorScheme)
    , bottomSeparator(colorScheme) {
    
    setupPlayerTabsComponents();
}

void Row2Component::paint(juce::Graphics& g) {
    // Use ColorScheme for background
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));

    auto bounds = getLocalBounds();
    
    // Use Row2 namespace constants for highlight positioning
    using namespace INIConfig::LayoutConstants;
    int tabWidth = layoutManager.scaled(Row2::tabWidth);
    // Calculate dynamic horizontal centering based on actual component width
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
    
    // Issue 2.1: Player Selection Line Not Centered - Center the highlight indicator
    int indicatorWidth = static_cast<int>(tabWidth * 0.8f); // 80% of tab width for better centering
    int centeredIndicatorX = selectedButtonX + (tabWidth - indicatorWidth) / 2;
    
    g.fillRect(centeredIndicatorX,
               bounds.getBottom() - highlightHeight,
               indicatorWidth,
               highlightHeight);
}

void Row2Component::resized() {
    ResponsiveComponent::resized(); // Call parent first
    updatePlayerTabsLayout();
}

void Row2Component::saveStates(ComponentState& state) {
    state.currentPlayer = INIConfig::clampPlayerIndex(selectedTab);
}

void Row2Component::loadStates(const ComponentState& state) {
    int tabToSelect = INIConfig::clampPlayerIndex(state.currentPlayer);
    updateFromState(state);
    setSelectedTab(tabToSelect);
}

void Row2Component::updateFromState(const ComponentState& state) {
    using namespace INIConfig::LayoutConstants;
    
    for (int i = 0; i < Row2::tabsCount; ++i) {
        if (INIConfig::isValidPlayerIndex(i)) {
            updateTabText(i, state.playerSettings[i]);
        }
    }

    updateTabVisuals();
    repaint();
}

juce::Rectangle<int> Row2Component::getRowBounds() const {
    using namespace INIConfig::LayoutConstants;
    return juce::Rectangle<int>(0, layoutManager.scaled(Row2::yPosition), 
                               layoutManager.scaled(INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH), 
                               layoutManager.scaled(Row2::height));
}

void Row2Component::lookAndFeelChanged() {
    using namespace INIConfig::LayoutConstants;
    
    // Update colors when look and feel changes
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

void Row2Component::parentSizeChanged() {
    // JUCE 8 compatible responsive layout update
    resized();
    repaint();
}

void Row2Component::setSelectedTab(int tab) {
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

void Row2Component::setClipLaunchMode(bool enabled) {
    using namespace INIConfig::LayoutConstants;
    
    clipLaunchMode = enabled;

    for (int i = 0; i < Row2::tabsCount; ++i) {
        tabs[i].onClick = [this, i] {
            handleTabClick(i);
        };
    }
}

void Row2Component::handleTabClick(int tabIndex) {
    if (clipLaunchMode) {
        if (onClipTriggered) {
            onClipTriggered(tabIndex);
        }
    } else {
        setSelectedTab(tabIndex);
    }
}

void Row2Component::highlightQueuedTab(int tabIndex, bool highlight) {
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

void Row2Component::showClipState(int tabIndex, bool hasClip, bool isPlaying) {
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

void Row2Component::setupPlayerTabsComponents() {
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
                          colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
}

void Row2Component::setupTabs() {
    using namespace INIConfig::LayoutConstants;
    
    // CREATE AND CONFIGURE ALL 8 PLAYER TABS
    for (int i = 0; i < Row2::tabsCount; ++i) {
        // Add tab to component hierarchy for rendering and event handling
        addAndMakeVisible(tabs[i]);
        
        // Set descriptive text for each player tab (1-indexed for user clarity)
        tabs[i].setButtonText("PLAYER " + juce::String(i + 1));

        // COLORSCHEME INTEGRATION: Apply theme colors for consistent appearance
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
    tabs[0].setToggleState(true, juce::dontSendNotification);
}

void Row2Component::updateTabVisuals() {
    using namespace INIConfig::LayoutConstants;
    
    for (int i = 0; i < Row2::tabsCount; ++i) {
        tabs[i].setToggleState(i == selectedTab, juce::dontSendNotification);

        // Update colors using ColorScheme
        tabs[i].setColour(juce::TextButton::buttonColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        tabs[i].setColour(juce::TextButton::buttonOnColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundToggled));
        tabs[i].setColour(juce::TextButton::textColourOnId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        tabs[i].setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
    }
}

void Row2Component::updateTabText(int tabIndex, const PlayerSettings& player) {
    if (!INIConfig::isValidPlayerIndex(tabIndex)) {
        return;
    }

    juce::String tabText = "PLAYER " + juce::String(tabIndex + 1);
    if (!player.enabled) {
        tabText += " (OFF)";
    }

    tabs[tabIndex].setButtonText(tabText);
}

void Row2Component::updatePlayerTabsLayout() {
    auto bounds = getLocalBounds();
    
    // Use responsive calculations instead of hardcoded values
    using namespace INIConfig::LayoutConstants;
    int tabWidth = getResponsiveButtonSize();
    int tabSpacing = getResponsiveSpacing();
    
    // Calculate dynamic horizontal centering based on actual component width
    int totalTabsWidth = (Row2::tabsCount * tabWidth) + ((Row2::tabsCount - 1) * tabSpacing);
    int leftMargin = (bounds.getWidth() - totalTabsWidth) / 2;
    
    int tabTopOffset = static_cast<int>(bounds.getHeight() * 0.1f); // 10% from top
    int tabContentHeight = static_cast<int>(bounds.getHeight() * 0.8f); // 80% of row height

    // Position each player tab using responsive layout
    int currentX = leftMargin;
    for (int i = 0; i < Row2::tabsCount; ++i) {
        tabs[i].setBounds(currentX, tabTopOffset, tabWidth, tabContentHeight);
        
        // Update font size responsively
        float fontSize = getResponsiveFontSize(14.0f);
        // Note: setFont() removed from TextButton in JUCE 8 - font styling handled by LookAndFeel
        
        // Move to next button position using responsive spacing
        currentX += tabWidth;
        if (i < Row2::tabsCount - 1) {
            currentX += tabSpacing;
        }
    }

    // Bottom separator using responsive thickness
    int separatorThickness = juce::jmax(1, static_cast<int>(bounds.getHeight() * 0.05f));
    bottomSeparator.setBounds(0, bounds.getBottom() - separatorThickness,
                             bounds.getWidth(), separatorThickness);
    
    // Position player number display responsively
    int availableLeftSpace = leftMargin;
    int playerNumWidth = static_cast<int>(availableLeftSpace * 0.8f);
    int playerNumHeight = bounds.getHeight();
    int playerNumX = (availableLeftSpace - playerNumWidth) / 2;
    int playerNumY = 0;
    
    playerNumber.setBounds(playerNumX, playerNumY, playerNumWidth, playerNumHeight);
    
    // Update player number font size responsively
    float playerFontSize = getResponsiveFontSize(16.0f);
    playerNumber.setFont(JUCE8_FONT(playerFontSize));
}

//==============================================================================
// ResponsiveComponent Implementation
//==============================================================================

void Row2Component::updateResponsiveLayout() {
    auto category = getCurrentDeviceCategory();
    
    // Device-specific adjustments for player tabs
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            // Mobile: Larger touch targets, simplified layout
            break;
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            // Tablet: Medium touch targets, balanced layout
            break;
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            // Desktop: Standard layout with mouse precision
            break;
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            // Large Desktop: Expanded layout, more information density
            break;
        default:
            break;
    }
    
    // Layout update will happen naturally through component hierarchy
    // Do not call resized() here as it creates infinite recursion
}

int Row2Component::getResponsiveButtonSize() const {
    auto category = getCurrentDeviceCategory();
    auto rules = getCurrentLayoutRules();
    
    // Base size from INI config - tabs should be wider than tall
    int baseWidth = static_cast<int>(getWidth() / INIConfig::LayoutConstants::Row2::tabsCount * 0.8f);
    
    // Apply device-specific adjustments
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            return juce::jmax(static_cast<int>(rules.sizing.minTouchTarget), baseWidth);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            return juce::jmax(static_cast<int>(rules.sizing.minTouchTarget * 0.9f), baseWidth);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            return juce::jmax(60, baseWidth);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            return juce::jmax(70, static_cast<int>(baseWidth * 1.1f));
        default:
            return juce::jmax(60, baseWidth);
    }
}

int Row2Component::getResponsiveSpacing() const {
    auto category = getCurrentDeviceCategory();
    auto rules = getCurrentLayoutRules();
    
    // Base spacing
    int baseSpacing = rules.spacing.defaultSpacing;
    
    // Apply device-specific adjustments
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            return juce::jmax(6, baseSpacing);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            return juce::jmax(4, baseSpacing);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            return juce::jmax(3, baseSpacing);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            return juce::jmax(4, baseSpacing);
        default:
            return baseSpacing;
    }
}

float Row2Component::getResponsiveFontSize(float baseSize) const {
    return ResponsiveComponent::getResponsiveFontSize(baseSize);
}
