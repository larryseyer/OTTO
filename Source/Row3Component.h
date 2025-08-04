#pragma once
#include "RowComponentBase.h"
#include "UtilityComponents.h"
#include "INIConfig.h"

class MidiEngine;
class Mixer;

class Row3Component : public RowComponentBase {
public:
    Row3Component(MidiEngine& midiEngine,
                 Mixer& mixer,
                 ResponsiveLayoutManager& layoutManager,
                 FontManager& fontManager,
                 ColorScheme& colorScheme);
    
    ~Row3Component() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void saveStates(ComponentState& state) override;
    void loadStates(const ComponentState& state) override;
    void updateFromState(const ComponentState& state) override;
    juce::String getRowName() const override { return "DrumKitControls"; }
    juce::Rectangle<int> getRowBounds() const override;
    void lookAndFeelChanged() override;
    void mouseDown(const juce::MouseEvent& event) override;
    
    // DrumKit functionality callbacks
    std::function<void(bool)> onEditModeChanged;
    std::function<void(bool)> onDrumKitNavigated; // true = next, false = previous
    std::function<void(const juce::String&)> onDrumKitChanged;
    std::function<void()> onMixerRequested;
    std::function<void(bool)> onMuteToggled;
    
    // Public interface methods
    void updatePlayerDisplay(int playerIndex);
    void setDrumKitName(const juce::String& name);
    juce::String getCurrentDrumKitName() const;
    void setEditMode(bool editMode);
    bool isEditMode() const;
    void setMuteState(bool muted);
    bool isMuted() const;
    
private:
    MidiEngine& midiEngine;
    Mixer& mixer;
    
    // Row 3 Components: DrumKit control buttons using Phosphor icon system
    PhosphorIconButton drumKitEditButton;        // Toggle edit mode for pattern editing
    PhosphorIconButton drumKitLeftChevron;       // Navigate to previous drum kit
    PhosphorIconButton drumKitRightChevron;      // Navigate to next drum kit
    PhosphorIconButton drumKitMuteButton;        // Mute/unmute current player
    PhosphorIconButton drumKitMixerButton;       // Open mixer window
    
    // DrumKit selection components
    HierarchicalComboBox drumKitDropdown;        // DrumKit selection dropdown
    juce::Label drumKitSelectedLabel;            // Shows currently selected drumkit name
    juce::Label playerNumberLabel;               // Large player number display
    
    // State variables
    int currentPlayerIndex = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;
    bool editMode = false;
    bool showingDrumKitLabel = true;             // Toggle state: true = show label, false = show menu
    juce::String currentDrumKitName = "808 Classic";
    
    // Setup and update methods
    void setupDrumKitComponents();
    void updateDrumKitLayout();
    void updatePlayerNumberDisplay();
    void updateMuteButtonState();
    void updateDrumKitDisplayToggle();
    void toggleDrumKitDisplay();
    void handleDrumKitChevrons(bool isRight);
    void handleEditButtonClick();
    void handleMuteButtonClick();
    void handleMixerButtonClick();
    void handleDrumKitDropdownChange();
    void populateDrumKitDropdown();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Row3Component)
};