#include "MainContentComponentLeftSection.h"
#include "CustomGroupManagerPopup.h"
#include "INIConfig.h"
#include "PerformanceOptimizations.h"

void MainContentComponentLeftSection::initializeEmptyPatternGroups(ComponentState& state) {
    if (state.beatsButtonGroups.isEmpty()) {
        for (int i = 1; i <= INIConfig::LayoutConstants::defaultGroupCount; ++i) {
            BeatsButtonGroup group("Group " + juce::String(i));
            group.isCustomGroup = false;
            group.selectedButton = 0;
            group.isFavorite = false;
            state.beatsButtonGroups.add(group);
        }
    }

    if (!state.beatsButtonGroups.isEmpty()) {
        const auto& firstGroup = state.beatsButtonGroups.getReference(0);
        currentSelectedGroup = firstGroup.groupName;
        midiBeatsButtonGroup.setText(currentSelectedGroup);
        midiFileManager->selectGroup(currentSelectedGroup);
    }

    updateMidiFileButtons(state);
    updateSelectedButton();
    isInitialized = true;
}

void MainContentComponentLeftSection::forceRefreshMidiFiles(ComponentState& state) {
    midiFileManager->scanMidiFiles();
    syncGroupsFromMidiFileManager(state);
    ensureValidGroupSelection(state);
    updateMidiFileButtons(state);
}

void MainContentComponentLeftSection::syncGroupsFromMidiFileManager(ComponentState& state) {
    auto mfmGroups = midiFileManager->getAvailableGroups();

    for (const auto& mfmGroup : mfmGroups) {
        bool found = false;
        for (auto& stateGroup : state.beatsButtonGroups) {
            if (stateGroup.groupName == mfmGroup.groupName) {
                stateGroup.midiFiles = mfmGroup.midiFiles;
                stateGroup.isCustomGroup = mfmGroup.isCustomGroup;
                found = true;
                break;
            }
        }

        if (!found) {
            BeatsButtonGroup newGroup(mfmGroup.groupName);
            newGroup.midiFiles = mfmGroup.midiFiles;
            newGroup.isCustomGroup = mfmGroup.isCustomGroup;
            state.beatsButtonGroups.add(newGroup);
        }
    }
}

void MainContentComponentLeftSection::ensureValidGroupSelection(ComponentState& state) {
    if (state.beatsButtonGroups.isEmpty()) {
        BeatsButtonGroup defaultGroup;
        defaultGroup.groupName = "Group 1";
        defaultGroup.isCustomGroup = false;
        defaultGroup.selectedButton = 0;
        state.beatsButtonGroups.add(defaultGroup);
    }

    bool found = false;
    for (const auto& group : state.beatsButtonGroups) {
        if (group.groupName == currentSelectedGroup) {
            found = true;
            break;
        }
    }

    if (!found) {
        const auto& firstGroup = state.beatsButtonGroups.getReference(0);
        currentSelectedGroup = firstGroup.groupName;
    }

    midiBeatsButtonGroup.setText(currentSelectedGroup);
    midiFileManager->selectGroup(currentSelectedGroup);
}

void MainContentComponentLeftSection::setCurrentSelectedGroup(const juce::String& groupName) {
    closeCurrentDropdown();

    juce::String newGroupName = groupName.isEmpty() ? "Group 1" : groupName;

    if (newGroupName != currentSelectedGroup) {
        currentSelectedGroup = newGroupName;
        midiFileManager->selectGroup(currentSelectedGroup);
        updateDropdownForCurrentPlayer();
    }
}

void MainContentComponentLeftSection::handleChevrons(bool isRight, ComponentState& state) {
    // Create a defensive copy to prevent race conditions with other components
    // that might be modifying beatsButtonGroups (MidiFileManager, StateManager, etc.)
    juce::Array<BeatsButtonGroup> groupsCopy = state.beatsButtonGroups;
    
    if (groupsCopy.isEmpty()) {
        forceRefreshMidiFiles(state);
        groupsCopy = state.beatsButtonGroups; // Refresh our copy
    }

    if (groupsCopy.isEmpty()) {
        BeatsButtonGroup defaultGroup("Group 1");
        state.beatsButtonGroups.add(defaultGroup);
        groupsCopy.add(defaultGroup);
    }

    int currentIndex = -1;
    const int groupCount = groupsCopy.size();
    
    // Find current group index using our safe copy
    for (int i = 0; i < groupCount; ++i) {
        const auto& group = groupsCopy.getReference(i);
        if (group.groupName == currentSelectedGroup) {
            currentIndex = i;
            break;
        }
    }

    if (currentIndex < 0) {
        currentIndex = 0;
    }

    // Ensure we have groups and prevent division by zero
    if (groupCount == 0) {
        BeatsButtonGroup defaultGroup("Group 1");
        state.beatsButtonGroups.add(defaultGroup);
        setCurrentSelectedGroup("Group 1");
        updateMidiFileButtons(state);
        savePlayerBeatsButtonState(currentPlayerIndex, state);
        return;
    }

    // Safe modulo operations with our known group count
    if (isRight) {
        currentIndex = (currentIndex + 1) % groupCount;
    } else {
        currentIndex = (currentIndex - 1 + groupCount) % groupCount;
    }

    // Final bounds check
    if (currentIndex < 0 || currentIndex >= groupCount) {
        currentIndex = 0;
    }

    // Get the new group name from our safe copy
    juce::String newGroupName;
    if (currentIndex < groupsCopy.size()) {
        newGroupName = groupsCopy.getReference(currentIndex).groupName;
    }
    
    // Fallback to first group if something went wrong
    if (newGroupName.isEmpty() && !groupsCopy.isEmpty()) {
        newGroupName = groupsCopy.getReference(0).groupName;
    }
    
    // Ultimate fallback
    if (newGroupName.isEmpty()) {
        newGroupName = "Group 1";
    }
    
    setCurrentSelectedGroup(newGroupName);
    updateMidiFileButtons(state);
    savePlayerBeatsButtonState(currentPlayerIndex, state);
}

void MainContentComponentLeftSection::createNewGroup(ComponentState& state) {
    showGroupManagerPopup(GroupManagerMode::CreateGroup, "", state);
}

void MainContentComponentLeftSection::deleteGroup(ComponentState& state) {
    closeCurrentDropdown();

    bool isCustomGroup = false;
    for (const auto& group : state.beatsButtonGroups) {
        if (group.groupName == currentSelectedGroup && group.isCustomGroup) {
            isCustomGroup = true;
            break;
        }
    }

    if (currentSelectedGroup.isEmpty() || !isCustomGroup) {
        return;
    }

    showGroupManagerPopup(GroupManagerMode::DeleteGroup, currentSelectedGroup, state);
}

void MainContentComponentLeftSection::editCurrentGroup(ComponentState& state) {
    closeCurrentDropdown();

    bool isCustomGroup = false;
    for (const auto& group : state.beatsButtonGroups) {
        if (group.groupName == currentSelectedGroup && group.isCustomGroup) {
            isCustomGroup = true;
            break;
        }
    }

    if (currentSelectedGroup.isEmpty() || !isCustomGroup) {
        return;
    }

    showGroupManagerPopup(GroupManagerMode::EditGroup, currentSelectedGroup, state);
}

void MainContentComponentLeftSection::showGroupManagerPopup(GroupManagerMode mode,
                                                           const juce::String& groupName,
                                                           ComponentState& state) {
    closeCurrentDropdown();

    auto* topLevel = getTopLevelComponent();
    if (!topLevel) return;

    for (int i = topLevel->getNumChildComponents() - 1; i >= 0; --i) {
        if (auto* popup = dynamic_cast<CustomGroupManagerPopup*>(topLevel->getChildComponent(i))) {
            topLevel->removeChildComponent(popup);
            delete popup; // Properly delete the popup
            break;
        }
    }

    CustomGroupManagerPopup::Mode popupMode;
    switch (mode) {
        case GroupManagerMode::CreateGroup:
            popupMode = CustomGroupManagerPopup::Mode::CreateGroup;
            break;
        case GroupManagerMode::DeleteGroup:
            popupMode = CustomGroupManagerPopup::Mode::DeleteGroup;
            break;
        case GroupManagerMode::EditGroup:
            popupMode = CustomGroupManagerPopup::Mode::EditGroup;
            break;
    }

    switch (popupMode) {
        case CustomGroupManagerPopup::Mode::CreateGroup:
            CustomGroupManagerPopup::showCreateGroupPopup(
                topLevel,
                topLevel->getLocalBounds(),
                layoutManager,
                fontManager,
                colorScheme,
                state,
                [this, &state](const juce::String& newGroupName) {
                    BeatsButtonGroup newGroup(newGroupName);
                    newGroup.isCustomGroup = true;
                    newGroup.selectedButton = selectedDrumButton;

                    for (int i = 0; i < 16; ++i) {
                        newGroup.midiFiles.add(assignedMidiFiles[i]);
                    }

                    state.beatsButtonGroups.add(newGroup);
                    setCurrentSelectedGroup(newGroupName);
                    savePlayerBeatsButtonState(currentPlayerIndex, state);

                    if (auto* topLevel = getTopLevelComponent()) {
                        for (int i = topLevel->getNumChildComponents() - 1; i >= 0; --i) {
                            if (topLevel->getChildComponent(i)->getName() == "CustomGroupManagerPopup") {
                                auto* popup = topLevel->getChildComponent(i);
                                topLevel->removeChildComponent(popup);
                                break;
                            }
                        }
                    }
                },
                [this]() {
                    if (auto* topLevel = getTopLevelComponent()) {
                        for (int i = topLevel->getNumChildComponents() - 1; i >= 0; --i) {
                            if (topLevel->getChildComponent(i)->getName() == "CustomGroupManagerPopup") {
                                auto* popup = topLevel->getChildComponent(i);
                                topLevel->removeChildComponent(popup);
                                break;
                            }
                        }
                    }
                }
            );
            break;

        case CustomGroupManagerPopup::Mode::DeleteGroup:
            CustomGroupManagerPopup::showDeleteGroupPopup(
                topLevel,
                topLevel->getLocalBounds(),
                groupName,
                layoutManager,
                fontManager,
                colorScheme,
                state,
                [this, groupName, &state]() {
                    for (int i = 0; i < state.beatsButtonGroups.size(); ++i) {
                        const auto& group = state.beatsButtonGroups.getReference(i);
                        if (group.groupName == groupName) {
                            state.beatsButtonGroups.remove(i);
                            break;
                        }
                    }

                    if (!state.beatsButtonGroups.isEmpty()) {
                        const auto& firstGroup = state.beatsButtonGroups.getReference(0);
                        setCurrentSelectedGroup(firstGroup.groupName);
                        updateMidiFileButtons(state);
                    } else {
                        setCurrentSelectedGroup("Group 1");
                        midiBeatsButtonGroup.setText("Group 1");
                        initializeEmptyPatternGroups(state);
                    }

                    savePlayerBeatsButtonState(currentPlayerIndex, state);

                    if (auto* topLevel = getTopLevelComponent()) {
                        for (int i = topLevel->getNumChildComponents() - 1; i >= 0; --i) {
                            if (topLevel->getChildComponent(i)->getName() == "CustomGroupManagerPopup") {
                                auto* popup = topLevel->getChildComponent(i);
                                topLevel->removeChildComponent(popup);
                                break;
                            }
                        }
                    }
                },
                [this]() {
                    if (auto* topLevel = getTopLevelComponent()) {
                        for (int i = topLevel->getNumChildComponents() - 1; i >= 0; --i) {
                            if (topLevel->getChildComponent(i)->getName() == "CustomGroupManagerPopup") {
                                auto* popup = topLevel->getChildComponent(i);
                                topLevel->removeChildComponent(popup);
                                break;
                            }
                        }
                    }
                }
            );
            break;

        case CustomGroupManagerPopup::Mode::EditGroup:
            CustomGroupManagerPopup::showEditGroupPopup(
                topLevel,
                topLevel->getLocalBounds(),
                groupName,
                layoutManager,
                fontManager,
                colorScheme,
                state,
                [this, groupName, &state]() {
                    // This should be handled by the popup's internal logic
                    // Just close the popup here
                    if (auto* topLevel = getTopLevelComponent()) {
                        for (int i = topLevel->getNumChildComponents() - 1; i >= 0; --i) {
                            if (topLevel->getChildComponent(i)->getName() == "CustomGroupManagerPopup") {
                                auto* popup = topLevel->getChildComponent(i);
                                topLevel->removeChildComponent(popup);
                                break;
                            }
                        }
                    }
                },
                [this]() {
                    if (auto* topLevel = getTopLevelComponent()) {
                        for (int i = topLevel->getNumChildComponents() - 1; i >= 0; --i) {
                            if (topLevel->getChildComponent(i)->getName() == "CustomGroupManagerPopup") {
                                auto* popup = topLevel->getChildComponent(i);
                                topLevel->removeChildComponent(popup);
                                break;
                            }
                        }
                    }
                }
            );
            break;
    }
}

BeatsButtonGroup* MainContentComponentLeftSection::findOrCreateGroup(const juce::String& groupName, ComponentState& state) {
    for (auto& group : state.beatsButtonGroups) {
        if (group.groupName == groupName) {
            return &group;
        }
    }

    BeatsButtonGroup newGroup(groupName);
    newGroup.isCustomGroup = true;
    state.beatsButtonGroups.add(newGroup);
    return &state.beatsButtonGroups.getReference(state.beatsButtonGroups.size() - 1);
}



void MainContentComponentLeftSection::saveCurrentPlayerCompleteState(ComponentState& state) {
    if (currentPlayerIndex >= 0 && currentPlayerIndex < 8) {
        savePlayerBeatsButtonState(currentPlayerIndex, state);
        auto* group = findOrCreateGroup(currentSelectedGroup, state);
        if (group) {
            group->selectedButton = selectedDrumButton;
            group->midiFiles.clear();
            for (int i = 0; i < 16; ++i) {
                group->midiFiles.add(assignedMidiFiles[i]);
            }
        }
    }
}

void MainContentComponentLeftSection::updateUIFromState(const ComponentState& state) {
    if (currentPlayerIndex >= 0 && currentPlayerIndex < 8) {
        const auto& playerSettings = state.playerSettings[currentPlayerIndex];

        if (!playerSettings.selectedMidiGroup.isEmpty()) {
            setCurrentSelectedGroup(playerSettings.selectedMidiGroup);
        } else {
            if (!state.beatsButtonGroups.isEmpty()) {
                const auto& firstGroup = state.beatsButtonGroups.getReference(0);
                setCurrentSelectedGroup(firstGroup.groupName);
            } else {
                setCurrentSelectedGroup("Group 1");
            }
        }

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

void MainContentComponentLeftSection::syncWithBeatsButtonGroups(const ComponentState& state) {
    if (midiFileManager) {
        syncGroupsFromMidiFileManager(const_cast<ComponentState&>(state));

        bool found = false;
        for (const auto& group : state.beatsButtonGroups) {
            if (group.groupName == currentSelectedGroup) {
                found = true;
                break;
            }
        }

        if (!found && !state.beatsButtonGroups.isEmpty()) {
            const auto& firstGroup = state.beatsButtonGroups.getReference(0);
            setCurrentSelectedGroup(firstGroup.groupName);
        }

        updateMidiFileButtons(state);
    }
}

void MainContentComponentLeftSection::updateStateFromCurrentUI(ComponentState& state) {
    // Update the current state from UI elements
    for (int i = 0; i < 16; ++i) {
        state.beatStates[i] = drumButtons[i].getToggleState();
    }
    
    // Update current group selection
    if (currentPlayerIndex >= 0 && currentPlayerIndex < 8) {
        auto& playerSettings = state.playerSettings[currentPlayerIndex];
        playerSettings.selectedMidiGroup = currentSelectedGroup;
        playerSettings.selectedButton = selectedDrumButton;
        
        for (int i = 0; i < 16; ++i) {
            playerSettings.assignedMidiFiles[i] = assignedMidiFiles[i];
        }
    }
}

void MainContentComponentLeftSection::loadPlayerBeatsButtonState(int playerIndex, const ComponentState& state) {
    if (playerIndex < 0 || playerIndex >= 8) {
        return;
    }
    
    const auto& playerSettings = state.playerSettings[playerIndex];
    
    // Load assigned MIDI files
    for (int i = 0; i < 16; ++i) {
        assignedMidiFiles[i] = playerSettings.assignedMidiFiles[i];
    }
    
    // Load selected group
    if (!playerSettings.selectedMidiGroup.isEmpty()) {
        setCurrentSelectedGroup(playerSettings.selectedMidiGroup);
    }
    
    // Load selected button
    selectedDrumButton = INIConfig::clampButtonIndex(playerSettings.selectedButton);
    
    updateMidiFileButtons(state);
    updateSelectedButton();
}

void MainContentComponentLeftSection::savePlayerBeatsButtonState(int playerIndex, ComponentState& state) {
    if (playerIndex < 0 || playerIndex >= 8) {
        return;
    }
    
    auto& playerSettings = state.playerSettings[playerIndex];
    playerSettings.selectedMidiGroup = currentSelectedGroup;
    playerSettings.selectedButton = selectedDrumButton;
    
    for (int i = 0; i < 16; ++i) {
        playerSettings.assignedMidiFiles[i] = assignedMidiFiles[i];
    }
    
    // Update the group in beatsButtonGroups as well
    for (auto& group : state.beatsButtonGroups) {
        if (group.groupName == currentSelectedGroup) {
            group.selectedButton = selectedDrumButton;
            group.midiFiles.clear();
            for (int i = 0; i < 16; ++i) {
                group.midiFiles.add(assignedMidiFiles[i]);
            }
            break;
        }
    }
}