#pragma once
#include "JUCE8_CODING_STANDARDS.h"
#include "UI/Layout/BreakpointManager.h"
#include "UtilityComponents.h"
#include "INIConfig.h"
#include "PopupWindows.h"

class MidiEngine;
class Mixer;
class SFZEngine;
class WaveformDisplay;

class Row3Component : public OTTO::UI::Layout::ResponsiveComponent {
public:
    Row3Component(MidiEngine& midiEngine,
                 Mixer& mixer,
                 ResponsiveLayoutManager& layoutManager,
                 FontManager& fontManager,
                 ColorScheme& colorScheme);
    
    ~Row3Component() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);
    void updateFromState(const ComponentState& state);
    juce::String getRowName() const { return "DrumKitControls"; }
    juce::Rectangle<int> getRowBounds() const;
    void lookAndFeelChanged() override;
    void mouseDown(const juce::MouseEvent& event) override;
    
    // ResponsiveComponent overrides
    void updateResponsiveLayout() override;
    
    // Responsive calculations
    int getResponsiveButtonSize() const;
    int getResponsiveSpacing() const;
    float getResponsiveFontSize(float baseSize) const;
    
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
    
    void setINIDataManager(INIDataManager* manager);
    void setSFZEngine(SFZEngine& engine);
    
    // Window management methods
    void toggleDrumkitEditor();
    void toggleMixerWindow();
    void showDrumkitEditor();
    void hideDrumkitEditor();
    void showMixerWindow();
    void hideMixerWindow();
    
    // PHASE 9D: WaveformDisplay integration
    void setWaveformDisplay(WaveformDisplay* waveformDisplay);
    void updateWaveformForCurrentSample();
    void showWaveformVisualization(bool show);
    bool isWaveformVisible() const;
    
    // PHASE 9D: Touch optimization
    void optimizeForTouch();
    
private:
    MidiEngine& midiEngine;
    Mixer& mixer;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;
    SFZEngine* sfzEngine = nullptr;
    INIDataManager* iniDataManager = nullptr;
    
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
    
    // PHASE 9D: Visualization integration
    WaveformDisplay* waveformDisplay = nullptr;  // Non-owning pointer to visualization
    bool waveformVisible = false;
    
    // Window instances for GUI fixes
    std::unique_ptr<DrumKitEditorWindow> drumkitEditorWindow;
    std::unique_ptr<DrumKitMixerWindow> mixerWindow;
    
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
    void initializeWindows();
    
    // PHASE 9D: Visualization helper methods
    void setupWaveformIntegration();
    void updateWaveformBounds();
    void loadSampleIntoWaveform(const juce::String& samplePath);
    juce::Rectangle<int> getWaveformArea() const;
    juce::String getCurrentDrumKitSamplePath() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Row3Component)
};
