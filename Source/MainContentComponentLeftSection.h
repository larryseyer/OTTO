#pragma once
#include <JuceHeader.h>
#include "UtilityComponents.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ColorScheme.h"
#include "ComponentState.h"
#include "MidiFileManager.h"
#include "INIDataManager.h"
#include "INIConfig.h"
#include "MidiAnalysisTypes.h"

class MidiEngine;
class CustomGroupManagerPopup;

class MainContentComponentLeftSection : public juce::Component {
public:
    MainContentComponentLeftSection(MidiEngine& midiEngine,
                                   ResponsiveLayoutManager& layoutManager,
                                   FontManager& fontManager,
                                   ColorScheme& colorScheme);

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void lookAndFeelChanged() override;

    bool getEditMode() const { return isEditMode; }
    void setEditModeVisuals(bool isEditing);

    juce::String getMidiFileAssignment(int buttonIndex) const;
    void setMidiFileAssignment(int buttonIndex, const juce::String& midiFile);

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    int getFavoritesSelection() const;
    void setFavoritesSelection(int selection);

    juce::String getCurrentSelectedGroup() const { return currentSelectedGroup; }
    void setCurrentSelectedGroup(const juce::String& groupName);

    void loadPlayerSpecificState(int playerIndex, const ComponentState& state);
    void checkAndLoadDefaultPatterns(ComponentState& state);
    void loadDefaultGroovePatternsFromAssets(ComponentState& state);

    int getSelectedDrumButton() const { return selectedDrumButton; }
    void setSelectedDrumButton(int buttonIndex);

    void syncWithBeatsButtonGroups(const ComponentState& state);
    void initializePlayerWithMidiFiles(int playerIndex, ComponentState& state);
    void saveCurrentPlayerCompleteState(ComponentState& state);

    MidiFileManager* getMidiFileManager();
    void initializeEmptyPatternGroups(ComponentState& state);
    void forceRefreshMidiFiles(ComponentState& state);

    std::function<void(bool)> onEditModeChanged;
    std::function<void(int, const juce::String&)> onMidiFileChanged;
    std::function<void()> onGrooveAndFillsRequested;

    void setComponentState(ComponentState* state) { componentState.reset(state); }

    BeatsButtonGroup* findOrCreateGroup(const juce::String& groupName, ComponentState& state);

private:
    MidiEngine& midiEngine;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;

    std::unique_ptr<MidiFileManager> midiFileManager;
    std::unique_ptr<juce::Component> activePopup;

    PhosphorIconButton editButton;
    PhosphorIconButton leftChevronButton;
    PhosphorIconButton rightChevronButton;
    HierarchicalComboBox midiBeatsButtonGroup;
    PhosphorIconButton favoriteButton;

    juce::TextButton drumButtons[INIConfig::Audio::NUM_DRUM_PADS];
    SeparatorComponent middleSeparator;

    bool isEditMode = INIConfig::Defaults::DEFAULT_EDIT_MODE;
    juce::String currentSelectedGroup;
    juce::String assignedMidiFiles[INIConfig::Audio::NUM_DRUM_PADS];
    int currentFavoritesSelection = INIConfig::Defaults::DEFAULT_SELECTED_BUTTON;
    int currentPlayerIndex = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;
    int selectedDrumButton = INIConfig::Defaults::DEFAULT_SELECTED_BUTTON;
    int currentlyOpenDropdownButton = INIConfig::MIDI::INACTIVE_PATTERN;
    std::unique_ptr<juce::PopupMenu> currentDropdownMenu;
    bool isInitialized = false;

    std::unique_ptr<ComponentState> componentState;

    void setupComponents();
    void updateFontsAndColors();
    void handleChevrons(bool isRight, ComponentState& state);
    void showMidiFileSelectionForButton(int buttonIndex, ComponentState& state);
    void updateMidiFileButtons(const ComponentState& state);

    void createNewGroup(ComponentState& state);
    void deleteGroup(ComponentState& state);
    void editCurrentGroup(ComponentState& state);

    void updateDropdownForCurrentPlayer();
    void updateSelectedButton();

    void savePlayerBeatsButtonState(int playerIndex, ComponentState& state);
    void loadPlayerBeatsButtonState(int playerIndex, const ComponentState& state);

    void closeCurrentDropdown();
    void onDrumButtonPressed(int buttonIndex, ComponentState& state);
    void onDrumButtonRightClicked(int buttonIndex, ComponentState& state);

    void ensureValidGroupSelection(ComponentState& state);
    void updateStateFromCurrentUI(ComponentState& state);
    void updateUIFromState(const ComponentState& state);
    void syncGroupsFromMidiFileManager(ComponentState& state);

    enum class GroupManagerMode {
        CreateGroup,
        DeleteGroup,
        EditGroup
    };

    void showGroupManagerPopup(GroupManagerMode mode,
                              const juce::String& groupName,
                              ComponentState& state);

    void showAdvancedMidiMenu(int buttonIndex, ComponentState& state);
    void showGrooveAnalysis(int buttonIndex);
    juce::String formatGrooveAnalysis(const MidiGrooveAnalysis& analysis);
    void suggestSimilarGrooves(int buttonIndex);
    void createGrooveVariation(int buttonIndex);
    void exportCurrentPattern();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponentLeftSection)
};
