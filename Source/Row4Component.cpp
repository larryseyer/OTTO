#include "JUCE8_CODING_STANDARDS.h"
#include "Row4Component.h"
#include "MidiEngine.h"
#include "INIConfig.h"
#include "Animation/AnimationManager.h"
#include "DragDrop/DragDropManager.h"
#include "ResponsiveLayoutManager.h"

Row4Component::Row4Component(MidiEngine& midiEngine,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : ResponsiveComponent()
    , midiEngine(midiEngine)
    , layoutManager(layoutManager)
    , fontManager(fontManager)
    , colorScheme(colorScheme)
    , patternGroupEditButton("pencil", FontManager::PhosphorWeight::Regular)
    , patternGroupLeftChevron("caret-left", FontManager::PhosphorWeight::Regular)
    , patternGroupRightChevron("caret-right", FontManager::PhosphorWeight::Regular)
    , patternGroupFavoriteButton("heart", FontManager::PhosphorWeight::Regular) {
    
    setupPatternGroupComponents();
    setupLabels();
    setupPatternGroupCallbacks();
    setupPatternGroupDragDrop();
    setupPatternGroupAnimations();
}

void Row4Component::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::transparentBlack);
}

void Row4Component::resized() {
    ResponsiveComponent::resized(); // Call parent first
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
    return juce::Rectangle<int>(
        layoutManager.scaled(0),
        layoutManager.scaled(Row4::yPosition),
        layoutManager.scaled(INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH),
        layoutManager.scaled(Row4::height)
    );
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
    
    // Dropdown selection handled in setupPatternGroupAnimations()
}

void Row4Component::updatePatternGroupLayout() {
    auto bounds = getLocalBounds();
    
    // Use responsive calculations instead of hardcoded values
    int buttonSize = getResponsiveButtonSize();
    int spacing = getResponsiveSpacing();
    int margin = getResponsiveMargin(8);
    
    int buttonY = (bounds.getHeight() - buttonSize) / 2;
    
    // Pattern group controls - left side
    int currentX = margin;
    
    // Edit button
    patternGroupEditButton.setBounds(currentX, buttonY, buttonSize, buttonSize);
    currentX += buttonSize + spacing;
    
    // Left chevron
    patternGroupLeftChevron.setBounds(currentX, buttonY, buttonSize, buttonSize);
    currentX += buttonSize + spacing;
    
    // Pattern group dropdown
    int dropdownWidth = static_cast<int>(bounds.getWidth() * 0.15f); // 15% of width
    patternGroupDropdown.setBounds(currentX, buttonY, dropdownWidth, buttonSize);
    
    // Note: Font styling handled by LookAndFeel in JUCE 8
    // float dropdownFontSize = getResponsiveFontSize(12.0f);
    
    currentX += dropdownWidth + spacing;
    
    // Right chevron
    patternGroupRightChevron.setBounds(currentX, buttonY, buttonSize, buttonSize);
    currentX += buttonSize + spacing;
    
    // Favorite button
    patternGroupFavoriteButton.setBounds(currentX, buttonY, buttonSize, buttonSize);
    currentX += buttonSize + spacing * 2; // Extra spacing before labels
    
    // Labels - right side of Row 4
    int labelWidth = static_cast<int>((bounds.getWidth() - currentX - margin) / 5.0f); // Divide remaining space by 5 labels
    int labelHeight = static_cast<int>(buttonSize * 0.8f);
    int labelY = buttonY + (buttonSize - labelHeight) / 2;
    
    // Update label font size responsively
    float labelFontSize = getResponsiveFontSize(10.0f);
    
    togglesLabel.setBounds(currentX, labelY, labelWidth, labelHeight);
    togglesLabel.setFont(JUCE8_FONT(labelFontSize));
    currentX += labelWidth;
    
    fillsLabel.setBounds(currentX, labelY, labelWidth, labelHeight);
    fillsLabel.setFont(JUCE8_FONT(labelFontSize));
    currentX += labelWidth;
    
    swingLabel.setBounds(currentX, labelY, labelWidth, labelHeight);
    swingLabel.setFont(JUCE8_FONT(labelFontSize));
    currentX += labelWidth;
    
    energyLabel.setBounds(currentX, labelY, labelWidth, labelHeight);
    energyLabel.setFont(JUCE8_FONT(labelFontSize));
    currentX += labelWidth;
    
    volumeLabel.setBounds(currentX, labelY, labelWidth, labelHeight);
    volumeLabel.setFont(JUCE8_FONT(labelFontSize));
}

void Row4Component::setupPatternGroupDragDrop() {
    patternGroupDropdown.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    
    patternDragSource = std::make_unique<PatternDragSource>(
        currentPatternGroupIndex, 
        patternGroupDropdown.getText()
    );
}

void Row4Component::setupPatternGroupAnimations() {
    patternGroupDropdown.onChange = [this]() {
        int selectedIndex = patternGroupDropdown.getSelectedItemIndex();
        if (selectedIndex >= 0) {
            if (animationManager && animationManager->shouldUseAnimations()) {
                animatePatternGroupChange(selectedIndex);
            } else {
                setCurrentPatternGroupIndex(selectedIndex);
            }
        }
    };
}

void Row4Component::animatePatternGroupChange(int newIndex) {
    if (animationManager) {
        animationManager->animatePatternSwitch(*this, 200);
        
        juce::Timer::callAfterDelay(100, [this, newIndex]() {
            setCurrentPatternGroupIndex(newIndex);
        });
    } else {
        auto fadeOut = [this, newIndex]() {
            setAlpha(0.7f);
            juce::Timer::callAfterDelay(100, [this, newIndex]() {
                setCurrentPatternGroupIndex(newIndex);
                juce::Timer::callAfterDelay(50, [this]() {
                    setAlpha(1.0f);
                });
            });
        };
        fadeOut();
    }
}

void Row4Component::handlePatternGroupAction(const juce::String& action) {
    if (action == "duplicate") {
    } else if (action == "delete") {
    } else if (action == "favorite") {
        togglePatternGroupFavorite();
    } else if (action == "export") {
    }
}

//==============================================================================
// ResponsiveComponent Implementation
//==============================================================================

void Row4Component::updateResponsiveLayout() {
    auto category = getCurrentDeviceCategory();
    
    // Device-specific adjustments for pattern group controls
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
    
    resized();
}

int Row4Component::getResponsiveButtonSize() const {
    auto category = getCurrentDeviceCategory();
    auto rules = getCurrentLayoutRules();
    
    // Base size from row height
    int baseSize = static_cast<int>(getHeight() * 0.6f); // 60% of row height
    
    // Apply device-specific adjustments
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            return juce::jmax(static_cast<int>(rules.sizing.minTouchTarget), baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            return juce::jmax(static_cast<int>(rules.sizing.minTouchTarget * 0.9f), baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            return juce::jmax(28, baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            return juce::jmax(32, static_cast<int>(baseSize * 1.1f));
        default:
            return juce::jmax(28, baseSize);
    }
}

int Row4Component::getResponsiveSpacing() const {
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

float Row4Component::getResponsiveFontSize(float baseSize) const {
    return ResponsiveComponent::getResponsiveFontSize(baseSize);
}
