
#include "MainContentComponentLeftSection.h"
#include "MidiEngine.h"
#include "INIConfig.h"

void MainContentComponentLeftSection::onDrumButtonPressed(int buttonIndex, ComponentState& state) {
    closeCurrentDropdown();
    if (isEditMode) {
        showMidiFileSelectionForButton(buttonIndex, state);
    } else {
        setSelectedDrumButton(buttonIndex);
        if (!assignedMidiFiles[buttonIndex].isEmpty()) {
            midiEngine.playMidiFile(currentPlayerIndex, assignedMidiFiles[buttonIndex]);
            if (onMidiFileChanged) {
                onMidiFileChanged(buttonIndex, assignedMidiFiles[buttonIndex]);
            }
        }
        savePlayerBeatsButtonState(currentPlayerIndex, state);
    }
}

void MainContentComponentLeftSection::onDrumButtonRightClicked(int buttonIndex, ComponentState& state) {
    closeCurrentDropdown();

    if (isEditMode || assignedMidiFiles[buttonIndex].isEmpty()) {
        showMidiFileSelectionForButton(buttonIndex, state);
    } else {
        showAdvancedMidiMenu(buttonIndex, state);
    }
}

void MainContentComponentLeftSection::setSelectedDrumButton(int buttonIndex) {
    if (buttonIndex >= 0 && buttonIndex < 16) {
        selectedDrumButton = INIConfig::clampButtonIndex(buttonIndex);
        updateSelectedButton();
    }
}

void MainContentComponentLeftSection::updateSelectedButton() {
    for (int i = 0; i < 16; ++i) {
        drumButtons[i].setToggleState(i == selectedDrumButton, juce::dontSendNotification);
    }
}

void MainContentComponentLeftSection::updateMidiFileButtons(const ComponentState& state) {
    const BeatsButtonGroup* currentGroup = nullptr;
    for (const auto& group : state.beatsButtonGroups) {
        if (group.groupName == currentSelectedGroup) {
            currentGroup = &group;
            break;
        }
    }

    if (currentGroup && currentGroup->isCustomGroup) {
        const auto& playerSettings = state.playerSettings[currentPlayerIndex];
        for (int i = 0; i < 16; ++i) {
            assignedMidiFiles[i] = playerSettings.assignedMidiFiles[i];
            if (!assignedMidiFiles[i].isEmpty()) {
                drumButtons[i].setButtonText(MidiFileManager::truncateTextForButton(assignedMidiFiles[i]));
            } else {
                drumButtons[i].setButtonText("Empty");
            }
        }
    } else {
        const auto& playerSettings = state.playerSettings[currentPlayerIndex];
        bool hasCustomAssignments = false;
        for (int i = 0; i < 16; ++i) {
            if (!playerSettings.assignedMidiFiles[i].isEmpty()) {
                hasCustomAssignments = true;
                break;
            }
        }

        if (hasCustomAssignments) {
            for (int i = 0; i < 16; ++i) {
                assignedMidiFiles[i] = playerSettings.assignedMidiFiles[i];
                if (!assignedMidiFiles[i].isEmpty()) {
                    drumButtons[i].setButtonText(MidiFileManager::truncateTextForButton(assignedMidiFiles[i]));
                } else {
                    drumButtons[i].setButtonText("Empty");
                }
            }
        } else if (currentGroup) {
            for (int i = 0; i < 16; ++i) {
                if (i < currentGroup->midiFiles.size() && !currentGroup->midiFiles[i].isEmpty()) {
                    assignedMidiFiles[i] = currentGroup->midiFiles[i];
                    drumButtons[i].setButtonText(MidiFileManager::truncateTextForButton(currentGroup->midiFiles[i]));
                } else {
                    assignedMidiFiles[i] = "";
                    drumButtons[i].setButtonText("Empty");
                }
            }
        }
    }

    repaint();
}

