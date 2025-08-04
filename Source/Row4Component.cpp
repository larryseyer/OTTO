#include "Row4Component.h"
#include "MidiEngine.h"
#include "INIConfig.h"
// JUCE 8 coding standards applied throughout this file

Row4Component::Row4Component(MidiEngine& midiEngine,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : RowComponentBase(4, layoutManager, fontManager, colorScheme)
    , midiEngine(midiEngine)
    , patternGroupEditButton("pencil", FontManager::PhosphorWeight::Regular)
    , patternGroupLeftChevron("caret-left", FontManager::PhosphorWeight::Regular)
    , patternGroupRightChevron("caret-right", FontManager::PhosphorWeight::Regular)
    , patternGroupFavoriteButton("heart", FontManager::PhosphorWeight::Regular) {
    
    setupPatternGroupComponents();
    setupLabels();
    setupPatternGroupCallbacks();
}

void Row4Component::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::transparentBlack);
}

void Row4Component::resized() {
    updatePatternGroupLayout();
}

void Row4Component::saveStates(ComponentState& state) {
    // Save pattern group state using dropdown selections map
    state.dropdownSelections["row4_pattern_group"] = currentPatternGroupIndex;
    // Save edit mode using toggle states map
    state.toggleStates[4001] = patternGroupEditMode; // Use unique ID for Row4 edit mode
}

void Row4Component::loadStates(const ComponentState& state) {
    // Load pattern group state
    int groupIndex = state.dropdownSelections.count("row4_pattern_group") ? 
                     state.dropdownSelections.at("row4_pattern_group") : 0;
    setCurrentPatternGroupIndex(groupIndex);
    
    bool editMode = state.toggleStates.count(4001) ? state.toggleStates.at(4001) : false;
    setPatternGroupEditMode(editMode);
}

void Row4Component::updateFromState(const ComponentState& state) {
    loadStates(state);
    updatePatternGroupButtonStates();
}

// Pattern Group Control Interface Implementation
void Row4Component::setPatternGroupEditMode(bool editMode) {
    patternGroupEditMode = editMode;
    patternGroupEditButton.setToggleState(editMode, juce::dontSendNotification);
    updatePatternGroupButtonStates();
}

bool Row4Component::isPatternGroupEditMode() const {
    return patternGroupEditMode;
}

void Row4Component::navigatePatternGroup(bool isNext) {
    int newIndex = currentPatternGroupIndex + (isNext ? 1 : -1);
    
    // Wrap around logic
    int maxIndex = patternGroupDropdown.getNumItems() - 1;
    if (newIndex > maxIndex) {
        newIndex = 0;
    } else if (newIndex < 0) {
        newIndex = maxIndex;
    }
    
    setCurrentPatternGroupIndex(newIndex);
}

void Row4Component::togglePatternGroupFavorite() {
    bool currentFavoriteState = patternGroupFavoriteButton.getToggleState();
    patternGroupFavoriteButton.setToggleState(!currentFavoriteState, juce::dontSendNotification);
    
    // Update favorite icon
    patternGroupFavoriteButton.setIconName(currentFavoriteState ? "heart" : "heart-fill");
}

int Row4Component::getCurrentPatternGroupIndex() const {
    return currentPatternGroupIndex;
}

void Row4Component::setCurrentPatternGroupIndex(int index) {
    if (index >= 0 && index < patternGroupDropdown.getNumItems()) {
        currentPatternGroupIndex = index;
        patternGroupDropdown.setSelectedItemIndex(index, juce::dontSendNotification);
        updatePatternGroupButtonStates();
    }
}

// Label Update Interface - CRITICAL for Phase 5
void Row4Component::updateLabelsFromControls() {
    // This method will be called by MainContentComponent to update labels
    // based on the current control states
    repaint();
}

void Row4Component::setLabelStates(const juce::String& togglesText, const juce::String& fillsText,
                                  const juce::String& swingText, const juce::String& energyText,
                                  const juce::String& volumeText) {
    togglesLabel.setText(togglesText, juce::dontSendNotification);
    fillsLabel.setText(fillsText, juce::dontSendNotification);
    swingLabel.setText(swingText, juce::dontSendNotification);
    energyLabel.setText(energyText, juce::dontSendNotification);
    volumeLabel.setText(volumeText, juce::dontSendNotification);
}

void Row4Component::updatePatternGroupButtonStates() {
    // Update button visual states based on current mode
    patternGroupEditButton.setToggleState(patternGroupEditMode, juce::dontSendNotification);
    
    // Update chevron button states
    bool hasItems = patternGroupDropdown.getNumItems() > 0;
    patternGroupLeftChevron.setEnabled(hasItems);
    patternGroupRightChevron.setEnabled(hasItems);
    
    // Update dropdown state
    patternGroupDropdown.setEnabled(hasItems);
}

juce::Rectangle<int> Row4Component::getRowBounds() const {
    using namespace INIConfig::LayoutConstants;
    return getScaledBounds(0, Row4::yPosition, 
                          INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH, 
                          Row4::height);
}

void Row4Component::setupPatternGroupComponents() {
    // Add all pattern group controls to component hierarchy
    addAndMakeVisible(patternGroupEditButton);
    addAndMakeVisible(patternGroupLeftChevron);
    addAndMakeVisible(patternGroupDropdown);
    addAndMakeVisible(patternGroupRightChevron);
    addAndMakeVisible(patternGroupFavoriteButton);
    
    // Set component IDs for debugging and state management
    patternGroupEditButton.setComponentID("pattern_group_edit");
    patternGroupLeftChevron.setComponentID("pattern_group_left");
    patternGroupDropdown.setComponentID("pattern_group_dropdown");
    patternGroupRightChevron.setComponentID("pattern_group_right");
    patternGroupFavoriteButton.setComponentID("pattern_group_favorite");
    
    // Configure dropdown
    patternGroupDropdown.setTextWhenNothingSelected("Select Pattern Group");
    patternGroupDropdown.setTextWhenNoChoicesAvailable("No Pattern Groups");
}

void Row4Component::setupLabels() {
    // CRITICAL: Add labels to Row 4 component hierarchy - this fixes the positioning issue
    addAndMakeVisible(togglesLabel);
    addAndMakeVisible(fillsLabel);
    addAndMakeVisible(swingLabel);
    addAndMakeVisible(energyLabel);
    addAndMakeVisible(volumeLabel);
    
    // Set component IDs
    togglesLabel.setComponentID("toggles_label");
    fillsLabel.setComponentID("fills_label");
    swingLabel.setComponentID("swing_label");
    energyLabel.setComponentID("energy_label");
    volumeLabel.setComponentID("volume_label");
    
    // Configure label properties using JUCE 8 patterns
    auto setupLabel = [&](juce::Label& label, const juce::String& text) {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setFont(fontManager.getFont(FontManager::FontRole::Body, 12.0f));
        label.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    };
    
    setupLabel(togglesLabel, "TOGGLES");
    setupLabel(fillsLabel, "FILLS");
    setupLabel(swingLabel, "SWING");
    setupLabel(energyLabel, "ENERGY");
    setupLabel(volumeLabel, "VOLUME");
}

void Row4Component::setupPatternGroupCallbacks() {
    // Pattern group navigation callbacks
    patternGroupLeftChevron.onClick = [this]() {
        navigatePatternGroup(false); // Previous
    };
    
    patternGroupRightChevron.onClick = [this]() {
        navigatePatternGroup(true); // Next
    };
    
    // Edit mode toggle
    patternGroupEditButton.onClick = [this]() {
        setPatternGroupEditMode(!patternGroupEditMode);
    };
    
    // Favorite toggle
    patternGroupFavoriteButton.onClick = [this]() {
        togglePatternGroupFavorite();
    };
    
    // Dropdown selection
    patternGroupDropdown.onChange = [this]() {
        int selectedIndex = patternGroupDropdown.getSelectedItemIndex();
        if (selectedIndex >= 0) {
            setCurrentPatternGroupIndex(selectedIndex);
        }
    };
}

void Row4Component::updatePatternGroupLayout() {
    using namespace INIConfig::LayoutConstants::Row4;
    
    auto bounds = getLocalBounds();
    
    // Position components using INI-driven layout constants
    patternGroupEditButton.setBounds(
        layoutManager.scaled(editIconX),
        layoutManager.scaled(editIconY),
        layoutManager.scaled(editIconWidth),
        layoutManager.scaled(iconHeight)
    );
    
    patternGroupLeftChevron.setBounds(
        layoutManager.scaled(leftChevronX),
        layoutManager.scaled(leftChevronY),
        layoutManager.scaled(chevronWidth),
        layoutManager.scaled(iconHeight)
    );
    
    patternGroupDropdown.setBounds(
        layoutManager.scaled(dropdownX),
        layoutManager.scaled(dropdownY),
        layoutManager.scaled(dropdownWidth),
        layoutManager.scaled(dropdownHeight)
    );
    
    patternGroupRightChevron.setBounds(
        layoutManager.scaled(rightChevronX),
        layoutManager.scaled(rightChevronY),
        layoutManager.scaled(chevronWidth),
        layoutManager.scaled(iconHeight)
    );
    
    patternGroupFavoriteButton.setBounds(
        layoutManager.scaled(favoriteIconX),
        layoutManager.scaled(favoriteIconY),
        layoutManager.scaled(favoriteIconWidth),
        layoutManager.scaled(iconHeight)
    );
    
    // CRITICAL: Position labels in Row 4 using INI constants
    togglesLabel.setBounds(
        layoutManager.scaled(togglesLabelX),
        layoutManager.scaled(togglesLabelY),
        layoutManager.scaled(labelWidth),
        layoutManager.scaled(labelHeight)
    );
    
    fillsLabel.setBounds(
        layoutManager.scaled(fillsLabelX),
        layoutManager.scaled(fillsLabelY),
        layoutManager.scaled(labelWidth),
        layoutManager.scaled(labelHeight)
    );
    
    swingLabel.setBounds(
        layoutManager.scaled(swingLabelX),
        layoutManager.scaled(swingLabelY),
        layoutManager.scaled(labelWidth),
        layoutManager.scaled(labelHeight)
    );
    
    energyLabel.setBounds(
        layoutManager.scaled(energyLabelX),
        layoutManager.scaled(energyLabelY),
        layoutManager.scaled(labelWidth),
        layoutManager.scaled(labelHeight)
    );
    
    volumeLabel.setBounds(
        layoutManager.scaled(volumeLabelX),
        layoutManager.scaled(volumeLabelY),
        layoutManager.scaled(labelWidth),
        layoutManager.scaled(labelHeight)
    );
}
