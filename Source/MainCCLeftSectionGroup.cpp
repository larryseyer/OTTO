#include "MainContentComponentLeftSection.h"
#include "CustomGroupManagerPopup.h"
#include "INIConfig.h"

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
        currentSelectedGroup = state.beatsButtonGroups[0].groupName;
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
        currentSelectedGroup = state.beatsButtonGroups[0].groupName;
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
    if (state.beatsButtonGroups.isEmpty()) {
        forceRefreshMidiFiles(state);
    }

    if (state.beatsButtonGroups.isEmpty()) {
        BeatsButtonGroup defaultGroup("Group 1");
        state.beatsButtonGroups.add(defaultGroup);
    }

    int currentIndex = -1;
    for (int i = 0; i < state.beatsButtonGroups.size(); ++i) {
        if (state.beatsButtonGroups[i].groupName == currentSelectedGroup) {
            currentIndex = i;
            break;
        }
    }

    if (currentIndex < 0) {
        currentIndex = 0;
    }

    if (isRight) {
        currentIndex = (currentIndex + 1) % state.beatsButtonGroups.size();
    } else {
        currentIndex = (currentIndex - 1 + state.beatsButtonGroups.size()) % state.beatsButtonGroups.size();
    }

    juce::String newGroupName = state.beatsButtonGroups[currentIndex].groupName;
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
        if (topLevel->getChildComponent(i)->getName() == "CustomGroupManagerPopup") {
            auto* popup = topLevel->getChildComponent(i);
            topLevel->removeChildComponent(popup);
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
                        if (state.beatsButtonGroups[i].groupName == groupName) {
                            state.beatsButtonGroups.remove(i);
                            break;
                        }
                    }

                    if (!state.beatsButtonGroups.isEmpty()) {
                        setCurrentSelectedGroup(state.beatsButtonGroups[0].groupName);
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
                    auto* group = findOrCreateGroup(groupName, state);
                    if (group) {
                        group->midiFiles.clear();
                        for (int i = 0; i < 16; ++i) {
                            group->midiFiles.add(assignedMidiFiles[i]);
                        }
                        group->selectedButton = selectedDrumButton;
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
    }
}

BeatsButtonGroup* MainContentComponentLeftSection::findOrCreateGroup(const juce::String& groupName, ComponentState& state) {
    for (auto& group : state.beatsButtonGroups) {
        if (group.groupName == groupName) {
            return &group;
        }
    }

    BeatsButtonGroup newGroup;
    newGroup.groupName = groupName;
    newGroup.isCustomGroup = true;
    newGroup.selectedButton = 0;
    state.beatsButtonGroups.add(newGroup);

    return &state.beatsButtonGroups.getReference(state.beatsButtonGroups.size() - 1);
}

void MainContentComponentLeftSection::savePlayerBeatsButtonState(int playerIndex, ComponentState& state) {
    if (playerIndex >= 0 && playerIndex < 8) {
        auto& playerSettings = state.playerSettings[playerIndex];
        playerSettings.selectedMidiGroup = currentSelectedGroup;
        playerSettings.selectedButton = selectedDrumButton;

        for (int i = 0; i < 16; ++i) {
            playerSettings.assignedMidiFiles[i] = assignedMidiFiles[i];
        }

        auto* group = findOrCreateGroup(currentSelectedGroup, state);
        if (group) {
            group->selectedButton = selectedDrumButton;
        }
    }
}

void MainContentComponentLeftSection::loadPlayerBeatsButtonState(int playerIndex, const ComponentState& state) {
    if (playerIndex >= 0 && playerIndex < 8) {
        const auto& playerSettings = state.playerSettings[playerIndex];

        currentSelectedGroup = playerSettings.selectedMidiGroup.isEmpty() ?
            ("Group " + juce::String(playerIndex + 1)) : playerSettings.selectedMidiGroup;

        selectedDrumButton = INIConfig::clampButtonIndex(playerSettings.selectedButton);

        for (int i = 0; i < 16; ++i) {
            assignedMidiFiles[i] = playerSettings.assignedMidiFiles[i];
        }
    }
}

void MainContentComponentLeftSection::initializePlayerWithMidiFiles(int playerIndex, ComponentState& state) {
    if (playerIndex >= 0 && playerIndex < 8 && midiFileManager) {
        if (!isInitialized) {
            initializeEmptyPatternGroups(state);
        }

        const juce::String defaultGroup = "Group " + juce::String(playerIndex + 1);
        auto& playerSettings = state.playerSettings[playerIndex];
        playerSettings.selectedMidiGroup = defaultGroup;
        playerSettings.selectedButton = 0;

        juce::String targetGroup = defaultGroup;
        bool found = false;
        for (const auto& group : state.beatsButtonGroups) {
            if (group.groupName == defaultGroup) {
                found = true;
                break;
            }
        }

        if (!found && !state.beatsButtonGroups.isEmpty()) {
            targetGroup = state.beatsButtonGroups[0].groupName;
            playerSettings.selectedMidiGroup = targetGroup;
        }

        for (const auto& group : state.beatsButtonGroups) {
            if (group.groupName == targetGroup) {
                for (int i = 0; i < 16 && i < group.midiFiles.size(); ++i) {
                    playerSettings.assignedMidiFiles[i] = group.midiFiles[i];
                }
                break;
            }
        }
    }
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

void MainContentComponentLeftSection::updateStateFromCurrentUI(ComponentState& state) {
    if (currentPlayerIndex >= 0 && currentPlayerIndex < 8) {
        auto& playerSettings = state.playerSettings[currentPlayerIndex];
        playerSettings.selectedMidiGroup = currentSelectedGroup;
        playerSettings.selectedButton = selectedDrumButton;

        for (int i = 0; i < 16; ++i) {
            playerSettings.assignedMidiFiles[i] = assignedMidiFiles[i];
        }

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
                setCurrentSelectedGroup(state.beatsButtonGroups[0].groupName);
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
            setCurrentSelectedGroup(state.beatsButtonGroups[0].groupName);
        }

        updateMidiFileButtons(state);
    }
}
