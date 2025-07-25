#include "MainContentComponentLeftSection.h"
#include "CustomLookAndFeel.h"
#include "MidiEngine.h"
#include "INIConfig.h"
#include "CustomGroupManagerPopup.h"
#include "INIDataManager.h"

MainContentComponentLeftSection::MainContentComponentLeftSection(MidiEngine& midiEngine,
                                                                ResponsiveLayoutManager& layoutManager,
                                                                FontManager& fontManager,
                                                                ColorScheme& colorScheme)
    : midiEngine(midiEngine), layoutManager(layoutManager),
      fontManager(fontManager), colorScheme(colorScheme),
      editButton("edit"), leftChevronButton("left"), rightChevronButton("right"), favoriteButton("heart"),
      middleSeparator(colorScheme), selectedDrumButton(0), currentlyOpenDropdownButton(-1) {

    midiFileManager = std::make_unique<MidiFileManager>();
    setupComponents();
    updateFontsAndColors();
}

void MainContentComponentLeftSection::paint(juce::Graphics& g) {
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));

    auto bounds = getLocalBounds();
    int dividerY = bounds.getHeight() / INIConfig::LayoutConstants::topSectionHeightRatio;

    if (auto* laf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel())) {
        laf->drawHorizontalSeparator(g, 0, dividerY, bounds.getWidth(), INIConfig::LayoutConstants::separatorComponentDefaultThickness * 2);
    } else {
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
        g.fillRect(0, dividerY, bounds.getWidth(), layoutManager.scaled(static_cast<int>(INIConfig::LayoutConstants::separatorComponentDefaultThickness * 2)));
    }
}

void MainContentComponentLeftSection::resized() {
    auto bounds = getLocalBounds();

    int topSectionHeight = bounds.getHeight() / INIConfig::LayoutConstants::topSectionHeightRatio;
    bounds.removeFromTop(topSectionHeight);

    int margin = layoutManager.scaled(INIConfig::LayoutConstants::defaultMargin);
    editButton.setBounds(margin, layoutManager.scaled(INIConfig::LayoutConstants::editButtonY),
                        layoutManager.scaled(INIConfig::LayoutConstants::editButtonSize),
                        layoutManager.scaled(INIConfig::LayoutConstants::editButtonSize));
    leftChevronButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::leftChevronX),
                               layoutManager.scaled(INIConfig::LayoutConstants::leftChevronY),
                               layoutManager.scaled(INIConfig::LayoutConstants::chevronButtonWidth),
                               layoutManager.scaled(INIConfig::LayoutConstants::chevronButtonHeight));

    int menuWidth = getWidth() - layoutManager.scaled(INIConfig::LayoutConstants::midiGroupMenuWidthOffset);
    midiBeatsButtonGroup.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::midiGroupMenuX),
                                  INIConfig::LayoutConstants::midiGroupMenuY,
                                  menuWidth,
                                  topSectionHeight);

    rightChevronButton.setBounds(getWidth() - layoutManager.scaled(INIConfig::LayoutConstants::rightChevronX),
                                layoutManager.scaled(INIConfig::LayoutConstants::rightChevronY),
                                layoutManager.scaled(INIConfig::LayoutConstants::chevronButtonWidth),
                                layoutManager.scaled(INIConfig::LayoutConstants::chevronButtonHeight));

    favoriteButton.setBounds(getWidth() - layoutManager.scaled(INIConfig::LayoutConstants::favoriteButtonX),
                            INIConfig::LayoutConstants::favoriteButtonY,
                            layoutManager.scaled(INIConfig::LayoutConstants::favoriteButtonSize),
                            layoutManager.scaled(INIConfig::LayoutConstants::favoriteButtonSize));

    middleSeparator.setBounds(0, topSectionHeight, getWidth(), layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness));

    int buttonWidth = layoutManager.scaled(INIConfig::LayoutConstants::drumButtonWidth);
    int buttonHeight = layoutManager.scaled(INIConfig::LayoutConstants::drumButtonHeight);
    int colSpacing = layoutManager.scaled(INIConfig::LayoutConstants::drumButtonGridSpacingX);
    int rowSpacing = layoutManager.scaled(INIConfig::LayoutConstants::drumButtonGridSpacingY);
    int gridStartX = layoutManager.scaled(INIConfig::LayoutConstants::drumButtonGridStartX);
    int gridStartY = topSectionHeight + layoutManager.scaled(INIConfig::LayoutConstants::drumButtonGridStartY);

    for (int i = 0; i < 16; ++i) {
        int row = i / INIConfig::LayoutConstants::drumButtonsPerRow;
        int col = i % INIConfig::LayoutConstants::drumButtonsPerRow;
        int x = gridStartX + (col * colSpacing);
        int y = gridStartY + (row * rowSpacing);
        drumButtons[i].setBounds(x, y, buttonWidth, buttonHeight);
    }
}

void MainContentComponentLeftSection::lookAndFeelChanged() {
    updateFontsAndColors();

    for (auto* child : getChildren()) {
        child->lookAndFeelChanged();
    }

    repaint();
}

void MainContentComponentLeftSection::updateFontsAndColors() {
    editButton.setColorScheme(&colorScheme);
    leftChevronButton.setColorScheme(&colorScheme);
    rightChevronButton.setColorScheme(&colorScheme);
    favoriteButton.setColorScheme(&colorScheme);

    midiBeatsButtonGroup.setColour(juce::ComboBox::textColourId,
                                   colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    midiBeatsButtonGroup.setColour(juce::ComboBox::backgroundColourId,
                                   colorScheme.getColor(ColorScheme::ColorRole::ControlBackground));
    midiBeatsButtonGroup.setColour(juce::ComboBox::outlineColourId,
                                   colorScheme.getColor(ColorScheme::ColorRole::Separator));
    midiBeatsButtonGroup.setColour(juce::ComboBox::arrowColourId,
                                   colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));

    for (int i = 0; i < 16; ++i) {
        drumButtons[i].setColour(juce::TextButton::buttonColourId,
                                colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        drumButtons[i].setColour(juce::TextButton::buttonOnColourId,
                                colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundHover));
        drumButtons[i].setColour(juce::TextButton::textColourOnId,
                                colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
        drumButtons[i].setColour(juce::TextButton::textColourOffId,
                                colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
    }
}

void MainContentComponentLeftSection::setupComponents() {
    addAndMakeVisible(editButton);
    addAndMakeVisible(leftChevronButton);
    addAndMakeVisible(rightChevronButton);
    addAndMakeVisible(midiBeatsButtonGroup);
    addAndMakeVisible(favoriteButton);
    addAndMakeVisible(middleSeparator);

    editButton.onClick = [this] {
        closeCurrentDropdown();
        isEditMode = !isEditMode;
        setEditModeVisuals(isEditMode);
        if (onEditModeChanged) {
            onEditModeChanged(isEditMode);
        }
        if (isEditMode && onGrooveAndFillsRequested) {
            onGrooveAndFillsRequested();
        }
    };

    leftChevronButton.onClick = [this] {
        closeCurrentDropdown();
        if (componentState) {
            handleChevrons(false, *componentState);
        }
    };

    rightChevronButton.onClick = [this] {
        closeCurrentDropdown();
        if (componentState) {
            handleChevrons(true, *componentState);
        }
    };

    midiBeatsButtonGroup.setTextWhenNothingSelected("Select MIDI Group...");
    midiBeatsButtonGroup.setTextWhenNoChoicesAvailable("No groups found");
    midiBeatsButtonGroup.setJustificationType(juce::Justification::centred);
    midiBeatsButtonGroup.onPopupRequest = [this]() {
        closeCurrentDropdown();
    };

    for (int i = 0; i < 16; ++i) {
        addAndMakeVisible(drumButtons[i]);
        drumButtons[i].setButtonText("Empty");
        drumButtons[i].setClickingTogglesState(true);
        drumButtons[i].setRadioGroupId(INIConfig::LayoutConstants::drumButtonRadioGroupId);
        drumButtons[i].onClick = [this, i] {
            if (componentState) {
                onDrumButtonPressed(i, *componentState);
            }
        };
    }

    currentSelectedGroup = "Group 1";
    midiBeatsButtonGroup.setText(currentSelectedGroup);
}

void MainContentComponentLeftSection::mouseDown(const juce::MouseEvent& event) {
    if (event.mods.isRightButtonDown()) {
        for (int i = 0; i < 16; ++i) {
            if (drumButtons[i].getBounds().contains(event.getPosition())) {
                if (componentState) {
                    onDrumButtonRightClicked(i, *componentState);
                }
                return;
            }
        }
    } else if (event.mods.isLeftButtonDown() && event.mods.isAltDown()) {
        for (int i = 0; i < 16; ++i) {
            if (drumButtons[i].getBounds().contains(event.getPosition())) {
                showGrooveAnalysis(i);
                return;
            }
        }
    }
    closeCurrentDropdown();
}

void MainContentComponentLeftSection::closeCurrentDropdown() {
    if (currentDropdownMenu != nullptr) {
        currentDropdownMenu->dismissAllActiveMenus();
        currentDropdownMenu.reset();
        currentlyOpenDropdownButton = -1;
    }
}

MidiFileManager* MainContentComponentLeftSection::getMidiFileManager() {
    return midiFileManager.get();
}

void MainContentComponentLeftSection::saveStates(ComponentState& state) {
    componentState = &state;
    savePlayerBeatsButtonState(currentPlayerIndex, state);

    for (int i = 0; i < 16; ++i) {
        state.beatStates[i] = drumButtons[i].getToggleState();
    }

    state.dropdownSelections["currentMidiGroup"] = currentFavoritesSelection;
    state.favoriteStates[currentFavoritesSelection] = favoriteButton.getToggleState();
    state.currentPlayer = currentPlayerIndex;

    updateStateFromCurrentUI(state);
}

void MainContentComponentLeftSection::loadStates(const ComponentState& state) {
    componentState = const_cast<ComponentState*>(&state);
    closeCurrentDropdown();

    for (int i = 0; i < 16; ++i) {
        auto it = state.beatStates.find(i);
        if (it != state.beatStates.end()) {
            drumButtons[i].setToggleState(it->second, juce::dontSendNotification);
        }
    }

    auto groupIt = state.dropdownSelections.find("currentMidiGroup");
    if (groupIt != state.dropdownSelections.end()) {
        currentFavoritesSelection = groupIt->second;
    }

    currentPlayerIndex = INIConfig::clampPlayerIndex(state.currentPlayer);
    loadPlayerBeatsButtonState(currentPlayerIndex, state);

    updateUIFromState(state);

    if (currentFavoritesSelection >= 0 && currentFavoritesSelection < 16) {
        favoriteButton.setToggleState(state.favoriteStates[currentFavoritesSelection], juce::dontSendNotification);
    }
}

void MainContentComponentLeftSection::loadDefaultGroovePatternsFromAssets(ComponentState& state) {
    juce::File assetsFolder = juce::File::getCurrentWorkingDirectory().getChildFile("Assets").getChildFile("MidiFiles");
    if (!assetsFolder.exists()) {
        assetsFolder = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
            .getParentDirectory().getChildFile("Assets").getChildFile("MidiFiles");
    }
    
    if (!assetsFolder.exists()) return;
    
    auto midiFiles = assetsFolder.findChildFiles(juce::File::findFiles, false, "*.mid");
    for (int i = 0; i < juce::jmin(16, midiFiles.size()); ++i) {
        assignedMidiFiles[i] = midiFiles[i].getFullPathName();
        juce::String buttonText = midiFiles[i].getFileNameWithoutExtension();
        if (buttonText.length() > 8) {
            buttonText = buttonText.substring(0, 8);
        }
        drumButtons[i].setButtonText(buttonText);
    }
    
    savePlayerBeatsButtonState(currentPlayerIndex, state);
}

void MainContentComponentLeftSection::checkAndLoadDefaultPatterns(ComponentState& state) {
    bool hasAnyPatterns = false;
    for (int i = 0; i < 16; ++i) {
        if (!assignedMidiFiles[i].isEmpty()) {
            hasAnyPatterns = true;
            break;
        }
    }
    
    if (!hasAnyPatterns) {
        loadDefaultGroovePatternsFromAssets(state);
    }
}

void MainContentComponentLeftSection::loadPlayerSpecificState(int playerIndex, const ComponentState& state) {
    closeCurrentDropdown();

    if (currentPlayerIndex != playerIndex && currentPlayerIndex >= 0 && currentPlayerIndex < 8) {
    }

    currentPlayerIndex = playerIndex;

    if (playerIndex >= 0 && playerIndex < 8) {
        loadPlayerBeatsButtonState(playerIndex, state);

        for (const auto& group : state.beatsButtonGroups) {
            if (group.groupName == currentSelectedGroup) {
                selectedDrumButton = INIConfig::clampButtonIndex(group.selectedButton);
                break;
            }
        }

        updateMidiFileButtons(state);
        updateSelectedButton();
        updateDropdownForCurrentPlayer();
    }
}

void MainContentComponentLeftSection::updateDropdownForCurrentPlayer() {
    juce::String displayText = currentSelectedGroup.isEmpty() ? "Group 1" : currentSelectedGroup;
    midiBeatsButtonGroup.setText(displayText);
    repaint();
}

int MainContentComponentLeftSection::getFavoritesSelection() const {
    return currentFavoritesSelection;
}

void MainContentComponentLeftSection::setFavoritesSelection(int selection) {
    if (selection >= 0 && selection < 4) {
        currentFavoritesSelection = selection;
    }
}

juce::String MainContentComponentLeftSection::getMidiFileAssignment(int buttonIndex) const {
    if (buttonIndex >= 0 && buttonIndex < 16) {
        return assignedMidiFiles[buttonIndex];
    }
    return "";
}

void MainContentComponentLeftSection::setMidiFileAssignment(int buttonIndex, const juce::String& midiFile) {
    if (buttonIndex >= 0 && buttonIndex < 16) {
        assignedMidiFiles[buttonIndex] = midiFile;
        if (!midiFile.isEmpty()) {
            drumButtons[buttonIndex].setButtonText(MidiFileManager::truncateTextForButton(midiFile));
        } else {
            drumButtons[buttonIndex].setButtonText("Empty");
        }
    }
}

#include "MainCCLeftSectionGroup.cpp"
#include "MainCCLeftSectionEdit.cpp"
#include "MainCCLeftSectionDrumButtons.cpp"
