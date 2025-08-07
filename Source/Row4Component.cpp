#include "JUCE8_CODING_STANDARDS.h"
#include "Row4Component.h"
#include "MidiEngine.h"
#include "INIConfig.h"
#include "Animation/AnimationManager.h"
#include "DragDrop/DragDropManager.h"
#include "ResponsiveLayoutManager.h"
#include "PopupWindows.h"

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

void Row4Component::mouseDown(const juce::MouseEvent& event) {
    // Handle label click for pattern group label/menu toggle - Issue 4.2
    if (event.eventComponent == &patternGroupLabel) {
        togglePatternGroupLabelMenu();
        return;
    }
    
    // Call parent for other mouse events
    ResponsiveComponent::mouseDown(event);
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
    // Issue 4.2 & 4.5: Left/Right Chevron Does Not Decrement/Increment Pattern Group Menu
    // Ensure navigatePatternGroup properly decrements/increments as described in GUI_TWEAKS.md
    int currentIndex = getCurrentPatternGroupIndex();
    int maxGroups = patternGroupDropdown.getNumItems(); // Get from dropdown
    
    if (maxGroups == 0) return; // No groups available
    
    if (isNext) {
        currentIndex = (currentIndex + 1) % maxGroups;
    } else {
        currentIndex = (currentIndex - 1 + maxGroups) % maxGroups;
    }
    
    setCurrentPatternGroupIndex(currentIndex);
    animatePatternGroupChange(currentIndex);
    updatePatternGroupButtonStates();
}

void Row4Component::togglePatternGroupFavorite() {
    // Issue 4.6: Favorites Icon Does Not Toggle/Mark Current Group
    // Implement togglePatternGroupFavorite method as described in GUI_TWEAKS.md
    int currentIndex = getCurrentPatternGroupIndex();
    bool isFavorite = isPatternGroupFavorite(currentIndex);
    
    setPatternGroupFavorite(currentIndex, !isFavorite);
    updateFavoriteButtonState();
    
    // TODO: Save to INI data when INIDataManager methods are available
    // if (iniDataManager) {
    //     iniDataManager->setPatternGroupFavorite(currentIndex, !isFavorite);
    // }
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
    addAndMakeVisible(patternGroupLabel);
    addAndMakeVisible(patternGroupRightChevron);
    addAndMakeVisible(patternGroupFavoriteButton);
    
    // Set component IDs for debugging and state management
    patternGroupEditButton.setComponentID("pattern_group_edit");
    patternGroupLeftChevron.setComponentID("pattern_group_left");
    patternGroupDropdown.setComponentID("pattern_group_dropdown");
    patternGroupLabel.setComponentID("pattern_group_label");
    patternGroupRightChevron.setComponentID("pattern_group_right");
    patternGroupFavoriteButton.setComponentID("pattern_group_favorite");
    
    // Configure dropdown
    patternGroupDropdown.setTextWhenNothingSelected("Select Pattern Group");
    patternGroupDropdown.setTextWhenNoChoicesAvailable("No Pattern Groups");
    
    // Set dropdown text colors to avoid using ButtonText (which is now black)
    patternGroupDropdown.setColour(juce::ComboBox::textColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    patternGroupDropdown.setColour(juce::ComboBox::backgroundColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    patternGroupDropdown.setColour(juce::ComboBox::outlineColourId, 
                                  colorScheme.getColor(ColorScheme::ColorRole::Separator));
    
    // Configure pattern group label - Issue 4.2: Label/Menu Toggle System
    patternGroupLabel.setText("Group 1", juce::dontSendNotification);
    patternGroupLabel.setJustificationType(juce::Justification::centred);
    patternGroupLabel.setFont(fontManager.getFont(FontManager::FontRole::Body, 
                             getResponsiveFontSize(fontManager.getDefaultSize(FontManager::FontRole::Body))));
    patternGroupLabel.setColour(juce::Label::textColourId, 
                               colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    
    // Set up label/menu toggle behavior - initially show label
    showPatternGroupLabel();
    
    // Populate dropdown with default pattern groups
    populatePatternGroupDropdown();
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
    
    // Issue 4.7: Labels Too Small - Need Playfair Display Font 2x Larger
    // Use responsive font sizing with INIConfig and FontManager
    float baseFontSize = fontManager.getDefaultSize(FontManager::FontRole::Header);
    float enlargedFontSize = getResponsiveFontSize(baseFontSize * 2.0f);
    
    // Configure label properties using responsive design with INIConfig ColorScheme and FontManager
    auto setupLabel = [&](juce::Label& label, const juce::String& text) {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        // Use Header font role which maps to Playfair Display (fontSecondaryType)
        label.setFont(fontManager.getFont(FontManager::FontRole::Header, enlargedFontSize));
        label.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
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
        bool newEditMode = !patternGroupEditMode;
        setPatternGroupEditMode(newEditMode);
        
        if (newEditMode) {
            showPatternGroupEditor();
        }
    };
    
    // Favorite toggle
    patternGroupFavoriteButton.onClick = [this]() {
        togglePatternGroupFavorite();
    };
    
    // Label click to toggle to menu - Issue 4.2
    patternGroupLabel.setInterceptsMouseClicks(true, false);
    patternGroupLabel.addMouseListener(this, false);
    
    // Dropdown selection handled in setupPatternGroupAnimations()
}

void Row4Component::updatePatternGroupLayout() {
    using namespace INIConfig::LayoutConstants::Row4;
    
    // Use INIConfig constants for precise positioning as defined in INIConfig.h
    // Edit/Pencil icon
    patternGroupEditButton.setBounds(
        layoutManager.scaled(editIconX),
        layoutManager.scaled(editIconY),
        layoutManager.scaled(editIconWidth),
        layoutManager.scaled(iconHeight)
    );
    
    // Left chevron
    patternGroupLeftChevron.setBounds(
        layoutManager.scaled(leftChevronX),
        layoutManager.scaled(leftChevronY),
        layoutManager.scaled(chevronWidth),
        layoutManager.scaled(iconHeight)
    );
    
    // Pattern group dropdown
    patternGroupDropdown.setBounds(
        layoutManager.scaled(dropdownX),
        layoutManager.scaled(dropdownY),
        layoutManager.scaled(dropdownWidth),
        layoutManager.scaled(dropdownHeight)
    );
    
    // Pattern group label (same position as dropdown for toggle behavior)
    patternGroupLabel.setBounds(
        layoutManager.scaled(dropdownX),
        layoutManager.scaled(dropdownY),
        layoutManager.scaled(dropdownWidth),
        layoutManager.scaled(dropdownHeight)
    );
    
    // Right chevron
    patternGroupRightChevron.setBounds(
        layoutManager.scaled(rightChevronX),
        layoutManager.scaled(rightChevronY),
        layoutManager.scaled(chevronWidth),
        layoutManager.scaled(iconHeight)
    );
    
    // Favorite button
    patternGroupFavoriteButton.setBounds(
        layoutManager.scaled(favoriteIconX),
        layoutManager.scaled(favoriteIconY),
        layoutManager.scaled(favoriteIconWidth),
        layoutManager.scaled(iconHeight)
    );
    
    // Labels using INIConfig positioning - Issue 4.7: 2x larger Playfair Display font
    float labelFontSize = getResponsiveFontSize(fontManager.getDefaultSize(FontManager::FontRole::Header) * 2.0f);
    
    // Toggles label
    togglesLabel.setBounds(
        layoutManager.scaled(togglesLabelX),
        layoutManager.scaled(togglesLabelY),
        layoutManager.scaled(labelWidth),
        layoutManager.scaled(labelHeight)
    );
    togglesLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, labelFontSize));
    
    // Fills label
    fillsLabel.setBounds(
        layoutManager.scaled(fillsLabelX),
        layoutManager.scaled(fillsLabelY),
        layoutManager.scaled(labelWidth),
        layoutManager.scaled(labelHeight)
    );
    fillsLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, labelFontSize));
    
    // Swing label
    swingLabel.setBounds(
        layoutManager.scaled(swingLabelX),
        layoutManager.scaled(swingLabelY),
        layoutManager.scaled(labelWidth),
        layoutManager.scaled(labelHeight)
    );
    swingLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, labelFontSize));
    
    // Energy label
    energyLabel.setBounds(
        layoutManager.scaled(energyLabelX),
        layoutManager.scaled(energyLabelY),
        layoutManager.scaled(labelWidth),
        layoutManager.scaled(labelHeight)
    );
    energyLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, labelFontSize));
    
    // Volume label
    volumeLabel.setBounds(
        layoutManager.scaled(volumeLabelX),
        layoutManager.scaled(volumeLabelY),
        layoutManager.scaled(labelWidth),
        layoutManager.scaled(labelHeight)
    );
    volumeLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, labelFontSize));
}

void Row4Component::setupPatternGroupDragDrop() {
    patternGroupDropdown.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    
    patternDragSource = std::make_unique<PatternDragSource>(
        currentPatternGroupIndex, 
        patternGroupDropdown.getText()
    );
}

void Row4Component::setupPatternGroupAnimations() {
    // Issue 4.3: Pattern Group Dropdown Does Nothing
    // Issue 4.4: Pattern Group Label Visibility Toggle
    // Implement dropdown functionality as described in GUI_TWEAKS.md
    patternGroupDropdown.onChange = [this]() {
        int selectedId = patternGroupDropdown.getSelectedId();
        if (selectedId > 0) {
            setCurrentPatternGroupIndex(selectedId - 1); // Convert to 0-based index
            animatePatternGroupChange(selectedId - 1);
            updatePatternGroupButtonStates();
            
            // Update label text and switch back to label view
            showPatternGroupLabel();
        }
    };
    
    // Note: HierarchicalComboBox doesn't have onPopupMenuDismissed
    // The label/menu toggle will be handled through mouse clicks and onChange
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
    
    // Layout update will happen naturally through component hierarchy
    // Do not call resized() here as it creates infinite recursion
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

//==============================================================================
// Pattern Group Editor and Dropdown Implementation
//==============================================================================

void Row4Component::showPatternGroupEditor() {
    // Issue 4.1: Pattern Group Edit Icon Does Not Bring Up Editor
    // Create pattern group editor window as described in GUI_TWEAKS.md
    auto editorWindow = std::make_unique<PatternGroupEditorWindow>(
        getCurrentPatternGroupIndex(),
        iniDataManager,
        colorScheme,
        fontManager
    );
    editorWindow->setSize(900, 650);
    editorWindow->centreWithSize(900, 650);
    editorWindow->setVisible(true);
    editorWindow->toFront(true);
    
    // Set callback for pattern group changes
    editorWindow->onPatternGroupChanged = [this](int newIndex) {
        setCurrentPatternGroupIndex(newIndex);
        animatePatternGroupChange(newIndex);
    };
    
    // Release ownership - window will manage itself
    editorWindow.release();
}

void Row4Component::populatePatternGroupDropdown() {
    patternGroupDropdown.clear();
    
    // Load pattern groups from INI data if available
    if (iniDataManager) {
        // TODO: Implement INI data loading when INIDataManager is available
        // auto patternGroups = iniDataManager->getPatternGroups();
        // for (int i = 0; i < patternGroups.size(); ++i) {
        //     patternGroupDropdown.addItem(patternGroups[i].name, i + 1);
        // }
    }
    
    // Default pattern groups if no INI data
    if (patternGroupDropdown.getNumItems() == 0) {
        for (int i = 0; i < 16; ++i) {
            patternGroupDropdown.addItem(juce::String("Group ") + juce::String(i + 1), i + 1);
        }
    }
    
    // Set current selection
    if (currentPatternGroupIndex < patternGroupDropdown.getNumItems()) {
        patternGroupDropdown.setSelectedId(currentPatternGroupIndex + 1, juce::dontSendNotification);
    }
}

bool Row4Component::isPatternGroupFavorite([[maybe_unused]] int index) {
    if (iniDataManager) {
        // TODO: Implement INI data loading when INIDataManager is available
        // return iniDataManager->isPatternGroupFavorite(index);
    }
    
    // Default: no favorites
    return false;
}

void Row4Component::setPatternGroupFavorite([[maybe_unused]] int index, [[maybe_unused]] bool favorite) {
    if (iniDataManager) {
        // TODO: Implement INI data saving when INIDataManager is available
        // iniDataManager->setPatternGroupFavorite(index, favorite);
    }
    
    updateFavoriteButtonState();
}

void Row4Component::updateFavoriteButtonState() {
    // Issue 4.6: Favorites Icon Does Not Toggle/Mark Current Group
    // Update visual appearance as described in GUI_TWEAKS.md
    bool isFavorite = isPatternGroupFavorite(getCurrentPatternGroupIndex());
    patternGroupFavoriteButton.setToggleState(isFavorite, juce::dontSendNotification);
    
    // Update visual appearance
    if (isFavorite) {
        patternGroupFavoriteButton.setColour(juce::TextButton::buttonColourId,
                                           colorScheme.getColor(ColorScheme::ColorRole::Accent));
        patternGroupFavoriteButton.setIconName("heart-fill");
    } else {
        patternGroupFavoriteButton.setColour(juce::TextButton::buttonColourId,
                                           colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        patternGroupFavoriteButton.setIconName("heart");
    }
}

//==============================================================================
// Custom MIDI File Path Support - Issue 4.5
//==============================================================================

void Row4Component::addCustomMidiPath(const juce::File& customPath) {
    if (customPath.exists() && customPath.isDirectory()) {
        // Check if path already exists
        for (const auto& existingPath : customMidiPaths) {
            if (existingPath.getFullPathName() == customPath.getFullPathName()) {
                return; // Already exists
            }
        }
        
        customMidiPaths.add(customPath);
        
        // Scan the custom path for MIDI files and add to pattern groups
        juce::Array<juce::File> midiFiles;
        customPath.findChildFiles(midiFiles, juce::File::findFiles, true, "*.mid;*.MID;*.midi;*.MIDI");
        
        if (midiFiles.size() > 0) {
            // Create a new pattern group from this custom path
            juce::String groupName = "Custom: " + customPath.getFileName();
            juce::Array<juce::String> fileNames;
            
            for (const auto& midiFile : midiFiles) {
                fileNames.add(midiFile.getFileNameWithoutExtension());
            }
            
            // Add to dropdown
            patternGroupDropdown.addItem(groupName, patternGroupDropdown.getNumItems() + 1);
            
            // Save to INI if available
            if (iniDataManager) {
                // TODO: Save custom path to INI when INIDataManager methods are available
            }
        }
    }
}

void Row4Component::removeCustomMidiPath(const juce::File& customPath) {
    for (int i = customMidiPaths.size() - 1; i >= 0; --i) {
        if (customMidiPaths[i].getFullPathName() == customPath.getFullPathName()) {
            customMidiPaths.remove(i);
            
            // Remove corresponding pattern group from dropdown
            juce::String groupName = "Custom: " + customPath.getFileName();
            for (int j = 0; j < patternGroupDropdown.getNumItems(); ++j) {
                if (patternGroupDropdown.getItemText(j) == groupName) {
                    patternGroupDropdown.clear();
                    populatePatternGroupDropdown(); // Rebuild dropdown without this item
                    break;
                }
            }
            
            // Save to INI if available
            if (iniDataManager) {
                // TODO: Save updated custom paths to INI when INIDataManager methods are available
            }
            break;
        }
    }
}

juce::Array<juce::File> Row4Component::getCustomMidiPaths() const {
    return customMidiPaths;
}

//==============================================================================
// Pattern Group Label/Menu Toggle System - Issue 4.2
//==============================================================================

void Row4Component::togglePatternGroupLabelMenu() {
    if (showingPatternGroupLabel) {
        showPatternGroupMenu();
    } else {
        showPatternGroupLabel();
    }
}

void Row4Component::showPatternGroupLabel() {
    showingPatternGroupLabel = true;
    patternGroupLabel.setVisible(true);
    patternGroupDropdown.setVisible(false);
    
    // Update label text to show current selection
    if (patternGroupDropdown.getSelectedItemIndex() >= 0) {
        patternGroupLabel.setText(patternGroupDropdown.getText(), juce::dontSendNotification);
    }
}

void Row4Component::showPatternGroupMenu() {
    showingPatternGroupLabel = false;
    patternGroupLabel.setVisible(false);
    patternGroupDropdown.setVisible(true);
}
